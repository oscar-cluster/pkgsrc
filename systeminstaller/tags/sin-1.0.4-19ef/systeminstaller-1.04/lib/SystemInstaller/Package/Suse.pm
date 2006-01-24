package SystemInstaller::Package::Suse;

#   $Header: /cvsroot/systeminstaller/systeminstaller/lib/SystemInstaller/Package/Suse.pm,v 1.21 2002/11/14 21:16:55 mchasal Exp $

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
use SystemInstaller::Package::RpmNoScripts;
use SystemInstaller::Image qw(find_distro);
use Carp;
use File::Copy;
use Data::Dumper;

use vars qw($VERSION $config);

# The following two env vars are needed by mandrake.  They should not affect
# anyone else in a negative way

$ENV{SECURE_LEVEL} = 1;
 
$VERSION = sprintf("%d.%02d", q$Revision: 1.21 $ =~ /(\d+)\.(\d+)/);

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
	my $rpmcmd=$main::config->rpm;
        if (($mode eq "install") || ($mode eq "post_install")) {
                if (glob "$path/aaa*.rpm") {
                        return 1;
                }
        }

        return 0;

} #footprint


sub files_install {
# Install the packages.
# Input:        image dir, filelist	
# Returns:	Boolean

	my $class=shift;
        my $imgpath=shift;
        my $pkgpath=shift;
        my @stages=@_;
        select(STDOUT);
        $|=1;

	
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
                my @order=();
	        # Make a string of all the filenames
	        my $pkglist=join(" ",@{$stages[$stage]{FILES}});
                my $cmd="cd $pkgpath;$rpmcmd -ir $imgpath $rpmargs --noscripts --notriggers $stages[$stage]{ARGS} $pkglist";
                &verbose("Performing RPM stage $stage install, command is:");
                &verbose("$cmd");

                open (INSTALL,"$cmd |");
                while (<INSTALL>) {
                       unless (/^\%\%/) {
                               chomp;
                               print "$_\n" if &get_verbose;
                               unless (/ /) {
                                        push (@order,$_);
                               }
                       }
                }
	        unless (close(INSTALL)) {
                        carp("Rpm installation failed.");
		        return 0;
	        }
                my @forder;
                foreach my $pkg (@order) {
                        $pkg=~s/-[^-]*-[^-]*$//;
                        push(@forder,$stages[$stage]{PACKAGES}{$pkg});
                }
                &SystemInstaller::Package::RpmNoScripts::run_scriptlets($imgpath,$pkgpath,@forder);
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

    # Generate shadow files
    if(-x "$imagedir/usr/sbin/pwconv") {
        system("chroot $imagedir /usr/sbin/pwconv");
    }
    if(-x "$imagedir/usr/sbin/grpconv") {
        system("chroot $imagedir /usr/sbin/grpconv");
    }

    
    # run ldconfig to ensure libraries are accounted for
    system("chroot $imagedir /sbin/ldconfig");

    # Run SuSEconfig to genrat all sorts of files.
    if(-x "$imagedir/sbin/SuSEconfig") {
        system("chroot $imagedir /sbin/SuSEconfig");
    }

    return 1;
} # files_post_install
### POD from here down

=head1 NAME
 
SystemInstaller::Package::Suse - Suse rpm package installation functions.
 
=head1 DESCRIPTION

This module provides the SystemInstall package API functions for Rpm based
installation. This module runs the post scriptlets after the installation of 
all files to circumvent some limitations. It can only be used with versions of
rpm that support the --nopost flag (4.0.3+).
This module provides the following API subroutines:
files_install.

See the SystemInstaller::Package manpage for details
on the API specification.

=head1 AUTHOR
 
Michael Chase-Salerno <mchasal@users.sf.net>
 
=head1 SEE ALSO

L<SystemInstaller::Package>
 
=cut

1;
