package OSCARInstaller::Installer;

#
# Copyright (c) 2008 Oak Ridge National Laboratory.
#                    Geoffroy R. Vallee <valleegr@ornl.gov>
#                    All rights reserved.
#
# This file is part of the OSCAR software package.  For license
# information, see the COPYING file in the top level directory of the
# OSCAR source distribution.
#

#
# $Id: Installer.pm 6954 2008-03-14 20:54:25Z valleegr $
#

use strict;
use warnings "all";
use Carp;
use base qw(Exporter);
use vars qw(@EXPORT);
@EXPORT = qw(
            distro_is_valid
            download_files
            cleanup_downloads
            check_md5sum
            install_files
            tar_dir
            );

################################################################################
# Check if the selected distro is in the list of supported distro. At the end, #
# we just check if the id of the selected distro is in the array of supported  #
# distros.                                                                     #
#                                                                              #
# Input: distros, reference to an array that represents the list of supported  #
#                 distros.                                                     #
#        my_distro, the selected Linux distribution id.                        #
# Return: 0 if the distro is supported, -1 else.                               #
################################################################################
sub distro_is_valid ($$$) {
    my ($distros, $my_distro, $verbose) = @_;
    print "Checking validy of the distro id...\n" if $verbose;
    if (! defined ($distros) || scalar (@$distros) == 0) {
        carp "ERROR: the array of supported distros is empty";
        return -1;
    }
    if ( !defined ($my_distro) || $my_distro eq "") {
        carp "ERROR: Invalid selected distro ($my_distro)";
        return -1;
    }
    chomp ($my_distro);
    foreach my $dist (@$distros) {
        if ($dist eq $my_distro) {
            print "\tValid distro id\n" if $verbose;
            return 0;
        }
    }
    print "\tInvalid distro id\n" if $verbose;
    return -1;
}

################################################################################
# Download a list of files.                                                    #
#                                                                              #
# Input: files_to_download, reference to an array that has the list of files   #
#                           that need to be downloaded. Note that this is only #
#                           the file name, no need to give the full URL. The   #
#                           variable base_url gives the URL of the directory   #
#                           where all the files are avaiable.                  #
#        base_url, URL of the directory where all the files are available.     #
#        dest, local directory where the files need to be saved.               #
#        verbose, do we have to print output or not? 0 if not, anything else   #
#                 if yes.                                                      #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub download_files ($$$$) {
    my ($files_to_download, $base_url, $dest, $verbose) = @_;
    print "Downloading OSCAR files...\n" if $verbose;
    if (!defined ($files_to_download)) {
        carp "ERROR: The list of files to download is empty";
        return -1;
    }
    if (!defined ($dest) || $dest eq "" || ! -d $dest) {
        carp "ERROR: Invalid destination";
        return -1;
    }
    my $cmd;
    foreach my $file (@$files_to_download) {
        print "\tDownloading $file in $dest" if $verbose;
        $cmd = "cd $dest; wget $base_url/$file";
        if (system ($cmd)) {
            carp "ERROR: Impossible to download the base tarball";
            return -1;
        }
    }
    print "\tSuccess\n" if $verbose;
    return 0;
}

################################################################################
# Clean-up files that were previously downloaded.                              #
#                                                                              #
# Input: files, list of files to check when we clean up. Note that this is     #
#               only the file name, not the full path.                         #
#        dest, directory where the files may be present (single directory).    #
#        verbose, do we have to print output or not? 0 if not, anything else   #
#                 if yes.                                                      #
# Return: 0 if success, -1 else.                                               #
#                                                                              #
# TODO: we should have an option to say "I want to delete the previous         #
# downloads" or not.                                                           #
################################################################################
sub cleanup_downloads ($$$) {
    my ($files, $dest, $verbose) = @_;
    print "Deleting previous downloads...\n" if $verbose;
    if (!defined ($files)) {
        carp "ERROR: The list of files to download is empty";
        return -1;
    }
    if (!defined ($dest) || $dest eq "") {
        carp "ERROR: Invalid destination";
        return -1;
    }
    if ( ! -d $dest ) {
        mkdir $dest or (carp "Impossible to create the directory $dest",
                        return -1);
    }
    foreach my $file (@$files) {
        print "\tDeleting $dest/$file...\n" if $verbose;
        unlink ("$dest/$file");
    }
    print "Success\n" if $verbose;
    return 0;
}

################################################################################
# Check the md5sum for a list of files. Note that the md5sums are available    #
# from a file with the following syntax: each line was two columns: (i) the    #
# file name, and (ii) the md5sum.                                              #
# Also note that all the files (files to check + the md5sum file) need to be   #
# saved in the same directory.                                                 #
#                                                                              #
# Input: files,  list of files to check. Note that this is only the file name, #
#                not the full path.                                            #
#        md5sum, file name (without the full path) of the md5sum file.         #
#        dest, where the md5sum file and the files to check are saved.         #
#        verbose, do we have to print output or not? 0 if not, anything else   #
#                 if yes.                                                      #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub check_md5sum ($$$$) {
    my ($files, $md5sum, $dest, $verbose) = @_;
    print "Verifying md5sums...\n" if $verbose;
    if (!defined ($files)) {
        carp "ERROR: The list of files to download is empty";
        return -1;
    }
    if (!defined ($dest) || $dest eq "" || ! -d $dest) {
        carp "ERROR: Invalid destination";
        return -1;
    }
    if (!defined ($md5sum) || $md5sum eq "" || ! -f "$dest/$md5sum") {
        carp "ERROR: Invalid md5sum file ($dest/$md5sum)";
        return -1;
    }
    my $file_md5sum;
    foreach my $file (@$files) {
        # Note that we do not check the md5sum, it does not make sense.
        if ($file ne $md5sum) {
            print "Checking $file...\n" if $verbose;
            # We find the md5sum
            $file_md5sum 
                = `grep $file $dest/oscar-5.0-MD5SUMS | awk '{print \$1}'`;
            my $res = `/usr/bin/md5sum $dest/$file | awk '{print \$1}'`;
            if ( !defined ($file_md5sum) ) {
                carp "ERROR: Impossible to get the md5sum for $file";
                return -1;
            }
            if ($res ne $file_md5sum) {
                carp "ERROR: Invalid md5sum for $file";
                return -1;
            } else {
                print "\t$dest/$file is valid\n" if $verbose;
            }
        }
    }
    print "\Success\n" if $verbose;
    return 0;
}

################################################################################
# Actually install OSCAR from the downloaded tarballs.                         #
#                                                                              #
# Input: oscar_tarball, name of the tarball for the OSCAR files.               #
#        repos_tarballs, reference to an array that has the names of the       #
#                        tarballs for the OSCAR repositories.                  #
#        dest, path of the directory where the tarballs are saved.             #
#        install_dest, path of the directory where OSCAR needs to be installed #
#                      (typically /opt).                                       #
#        verbose, do we have to print output or not? 0 if not, anything else   #
#                 if yes.                                                      #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub install_files ($$$$$) {
    my ($oscar_tarball, $repos_tarballs, $dest, $install_dest, $verbose) = @_;

    print "Download OK, installing OSCAR...\n" if $verbose;
    if (!defined ($repos_tarballs)) {
        carp "ERROR: Invalid list of repo tarballs";
        return -1;
    }
    if (!defined($oscar_tarball) || $oscar_tarball eq "") {
        carp "ERROR: Invalid OSCAR tarball";
        return -1;
    }
    if (!defined($dest) || $dest eq "" || ! -d $dest) {
        carp "ERROR: Invalid download directory";
        return -1;
    }
    if (!defined($install_dest) || $install_dest eq "") {
        carp "ERROR: Invalid installation directory";
        return -1;
    }
    if (! -d $install_dest) {
        mkdir $install_dest 
            or (carp "ERROR: Impossible to create dir $install_dest", 
                return -1);
    }

    # We are now ready to go, we untar what we downloaded.

    my $repos_path = "/tftpboot";
    my $cmd;
    # Untar the repos
    foreach my $repo (@$repos_tarballs) {
        $cmd = "cd $repos_path/oscar; /bin/tar -xzf $dest/$repo";
        print "\t$cmd\n" if $verbose;
        if (system ($cmd)) {
            carp "ERROR: Impossible to install the OSCAR repository";
            return -1;
        }
    }
    # Untar OSCAR
    $cmd = "cd $install_dest; /bin/tar -xzf $dest/$oscar_tarball";
    print "\t$cmd\n" if $verbose;
    if (system ($cmd)) {
        carp "ERROR: Impossible to install OSCAR";
        return -1;
    }
    print "\tSuccess.\n" if $verbose;
    return 0;
}

################################################################################
# Tar the directory where the OSCAR downloaded files are saved. This is used   #
# to create a "package" for offline installation.                              #
#                                                                              #
# Input: path, path where the directory used to create the package (typically  #
#              path of the directory where the OSCAR tarballs are saved).      #
#        dir_to_tar, name of the directory to tar.                             #
#        verbose, do we have to print output or not? 0 if not, anything else   #
#                 if yes.                                                      #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub tar_dir ($$$) {
    my ($path, $dir_to_tar, $verbose) = @_;
    print "Creating the installation tarball...\n" if $verbose;
    unlink ("$path/oscar-offline-install.tar.gz") 
        if ( -f "$path/oscar-offline-install.tar.gz");
    my $cmd = "cd $path; tar -czf oscar-offline-install.tar.gz $dir_to_tar";
    print "\t$cmd\n" if $verbose;
    if (system ($cmd)) {
        carp "ERROR: impossible to tar the target directory";
        return -1;
    }
    print "\tSuccess.\n" if $verbose;
    return 0;
}

################################################################################
# Untar the "package" used for offline installation.                           #
#                                                                              #
# Input: tar_file, absolute path of the offline installation package.          #
#        dest, where the package needs to be untar.                            #
#        verbose, do we have to print output or not? 0 if not, anything else   #
#                 if yes.                                                      #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub untar_file ($$$) {
    my ($tar_file, $dest, $verbose) = @_;
    print "Decompressing the installation tarball...\n" if $verbose;
    my $cmd = "cd $dest; tar -xzf $tar_file";
    print "\t$cmd\n" if $verbose;
    if (system($cmd)) {
        carp "ERROR: Impossible to untar the file for offline installation";
        return -1;
    }
    print "\tSuccess.\n" if $verbose;
    return 0;
}

1;