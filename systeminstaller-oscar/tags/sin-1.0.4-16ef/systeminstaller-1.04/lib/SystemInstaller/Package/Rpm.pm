package SystemInstaller::Package::Rpm;

#   $Header: /cvsroot/systeminstaller/systeminstaller/lib/SystemInstaller/Package/Rpm.pm,v 1.68 2003/04/09 20:26:21 mchasal Exp $

#   Copyright (c) 2001 International Business Machines
 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
 
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
#   Michael Chase-Salerno <mchasal@users.sf.net>
use strict;

use  File::Path;
use  File::Basename;
use SystemInstaller::Log qw(verbose get_verbose);
use SystemInstaller::PackageBest;
use SystemInstaller::Image qw(find_distro split_version);
use Carp;

use vars qw($VERSION $config);

# The following two env vars are needed by mandrake.  They should not affect
# anyone else in a negative way

$ENV{SECURE_LEVEL} = 1;
 
$VERSION = sprintf("%d.%02d", q$Revision: 1.68 $ =~ /(\d+)\.(\d+)/);

#
## API FUNCTIONS
#

sub footprint {
# Look at a directory and determine if it looks like rpms.
# Input:        Directory name
# Returns:      Boolean of match
	my $class=shift;
        my $mode=shift;
        my $path=shift;
        if (($mode eq "files") || 
                ($mode eq "install") || ($mode eq "post_install") ||
                ($mode eq "pre_install")) {
                if (glob "$path/*.rpm") {
                        return 1;
                }
        }

        return 0;

} #footprint

sub files_find {
# Check that the files exist on the specified media.
# Set the PFILES list to the filenames found.
# Input:	rpm path, architecture, pkg list
# Returns:	file list or null if failure.

	my $class=shift;
        my $rpmdir=shift;
        my $arch=shift;
        my @pkglist=@_;
        my @stages;
        my $cachefile=$main::config->pkgcachefile;
	&verbose("Finding best Rpms, this may take a minute.");
        my %files=&find_files(CACHEFILE=>$cachefile,ARCH=>$arch, PKGDIR=>$rpmdir, PKGLIST=>\@pkglist);
        unless (%files) {
                carp("Couldn't find best files.");
                return;
        }

        &verbose("Identifying distro.");
        my ($distro,$version)=find_distro($rpmdir);
        my ($maj,$min)=split_version($version);
        &verbose("Distro is $distro, version is $version.");
        my $stage=0;
        my @alreadyinstalled;
        my $stagefile= $main::config->distinfo . "/" . $distro . "/" . $maj ."/".$min . "/stages";
        if (-e $stagefile) {
                &verbose("Found stage file for this distro and full version.");
        } else {
                $stagefile= $main::config->distinfo . "/" . $distro ."/". $maj. "/stages";        
        }
        if (-e $stagefile) {
                &verbose("Found stage file for this distro and major version.");
        } else {
                $stagefile= $main::config->distinfo . "/" . $distro . "/stages";        

        }
        if (-e $stagefile) {
                &verbose("No version stage file found, falling back to distro file.");
        } else {
                &verbose("No stage file found, using single stage install.");
        }
                
        if (-e $stagefile) {
                &verbose("Reading stage definition file, $stagefile");
                unless (open (STAGE,"< $stagefile")) {
                        carp("Unable to open stage file $stagefile");
                        return;
                }
                my $fail=0;
                while (<STAGE>) {
                        chomp;
                        $_=~s/\s*$//;
                        if (/^#/) {next;}
                        if (/^$/) {next;}
                        if (/^SISstage*/){
                                $stage++;
                                $stages[$stage]{ARGS}=$_;
                                $stages[$stage]{ARGS}=~s/^SISstage\s*//;
                        } else {
                                unless ($files{$_}) {
                                        carp("Couldn't find file for required package $_");
                                        $fail++;
                                } else {
                                        push(@{$stages[$stage]{FILES}},$files{$_});
                                        $stages[$stage]{PACKAGES}{$_}=$files{$_};
                                        push(@alreadyinstalled,$_);
                                }
                        }
                }
                if ($fail) {
                        carp("Failed to find required packages.");
                        return;
                }
        } 

        # Now add the user files minus the stage packages
        $stage++;
        foreach my $pkg (keys %files) {
                if (grep {$_ eq $pkg} @alreadyinstalled) {
                        next;
                }
                push @{$stages[$stage]{FILES}},$files{$pkg};
                $stages[$stage]{PACKAGES}{$pkg}=$files{$pkg};
        }

                                
        return @stages;

} #files_find


sub files_pre_install {
# Rpm pre install routine
# Make sure rpm database dir exists.
# Input: imagedir, pkgdir
# Output: boolean
    my $class=shift;
    my $imagedir = shift;
    my $pkgdir = shift;

    &verbose("Identifying distro.");
    my ($distro,$version)=find_distro($pkgdir);
    &verbose("Distro is $distro, version is $version.");

    my $imgroot=find_imgroot($distro,$version);
    if ($imgroot) {
        &verbose("Seeding image from distinfo tree");
        if (system("cp -af $imgroot/* $imagedir")) {
                carp("Copying seed files to image failed");
                return 0;
        }
    } else {
            &verbose("No seed directory found, using defaults.");
    }

    &verbose("Priming Rpm database");
    unless (mkpath("$imagedir/var/lib/rpm/")) {
            carp("Unable to create rpm database directory.");
            return 0;
    }
    unless (mkpath("$imagedir/dev/")) {
            carp("Unable to create dev directory.");
            return 0;
    }

    &verbose("Creating /dev/null");
    if (system("/bin/mknod $imagedir/dev/null c 1 3")) {
            carp("Unable to create /dev/null device, continuing.");
    }


    return 1;
} # files_pre_install

sub files_install {
# Install the packages.
# Input:        image dir, filelist	
# Returns:	Boolean

	my $class=shift;
        my $imgpath=shift;
        my $pkgpath=shift;
        my @stages=@_;

	
	# Install them
	my $rpmcmd=$main::config->rpm;
	my $rpmargs=$main::config->rpmargs;
	my $redir;
	if (get_verbose) {
		$redir="";
	} else {
		$redir=">/dev/null";
	}

        foreach my $stage (1..(scalar(@stages)-1)) {
	        # Make a string of all the filenames
	        my $pkglist=join(" ",@{$stages[$stage]{FILES}});
                my $cmd="cd $pkgpath;$rpmcmd -ir $imgpath $rpmargs $stages[$stage]{ARGS} $pkglist $redir";
                &verbose("Performing RPM stage $stage install, command is:");
                &verbose("$cmd");
                open (INSTALL,"$cmd |");
                while (<INSTALL>) {
                       unless (/^\%\%/) {
                               chomp;
                               print "$_\n" if &get_verbose;
                       }
                }
                unless (close(INSTALL)) {
                        carp("Rpm installation failed.");
                        return 0;
                }

        }

	return 1;
} #files_install

sub files_post_install {
# Rpm post install routine
# Input: imagedir, pkgdir
# Output: boolean
    my $class=shift;
    my $imagedir = shift;
    my $pkgdir = shift;
    my @file;
    # Here are some things that I know we need to do...

    # The following is a Mandrakeism.
    if(-x "$imagedir/usr/sbin/msec") {
        system("chroot $imagedir /usr/sbin/msec 3");
    }
    if ((! -e "$imagedir/boot/lilo") && (-e "$imagedir/boot/lilo-text")) {
            symlink("lilo-text","$imagedir/boot/lilo");
    }

    # Generate shadow files
    if(-x "$imagedir/usr/sbin/pwconv") {
        system("chroot $imagedir /usr/sbin/pwconv");
    }
    if(-x "$imagedir/usr/sbin/grpconv") {
        system("chroot $imagedir /usr/sbin/grpconv");
    }

    
    # run ldconfig to ensure libraries are accounted for
    system("chroot $imagedir /sbin/ldconfig");

    return 1;
} # files_post_install

#
## ADDITIONAL FUNCTIONS
#

sub find_imgroot{
# Find a valid imgroot tree
# Input: distro, version
# Output: Path to the tree or 0 if not found.

        my $distro=shift;
        my $version=shift;
        # Split the version to major and minor parts
        my ($maj,$min)=split_version($version);
        # Now loop through the parent directories until an imgroot is found
        # or we hit the top of distinfo.
        my $imgroot=$main::config->distinfo ."/".$distro."/".$maj."/".$min."/imgroot";
        until ((-d $imgroot) || ($imgroot eq $main::config->distinfo."/imgroot")) {
                $imgroot=~s/\/+/\//g; # get rid of any double /
                $imgroot=~s/\/[^\/]+\/imgroot$/\/imgroot/;
        }
        # If we didn't hit the top, return the directory.
        unless (-d $imgroot) {
                return 0;
        } else {
                return $imgroot;
        }

} #find_imgroot

### POD from here down

=head1 NAME
 
SystemInstaller::Package::Rpm - Rpm package installation functions.
 
=head1 DESCRIPTION

This module provides the SystemInstall package API functions for Rpm based
installation. This is the default type of installation when no other modules
have provided a suitable match. This module provides the following API subroutines:
files_find, files_pre_install, files_install, files_post_install.

See the SystemInstaller::Package manpage for details
on the API specification.

=head1 AUTHOR
 
Michael Chase-Salerno <mchasal@users.sf.net>
 
=head1 SEE ALSO

L<SystemInstaller::Package>
 
=cut

1;
