package SystemInstaller::Package::RpmNoScripts;

#   $Header: /cvsroot/systeminstaller/systeminstaller/lib/SystemInstaller/Package/RpmNoScripts.pm,v 1.5 2002/11/06 22:54:07 mchasal Exp $

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
use SystemInstaller::Image qw(find_distro);
use Carp;
use File::Copy;
use Data::Dumper;

use vars qw($VERSION $config);

# The following two env vars are needed by mandrake.  They should not affect
# anyone else in a negative way

$ENV{SECURE_LEVEL} = 1;
 
$VERSION = sprintf("%d.%02d", q$Revision: 1.5 $ =~ /(\d+)\.(\d+)/);

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
        if ($mode eq "install") {
                if (glob "$path/*.rpm")  {
                        my $line = `$rpmcmd --version 2>/dev/null`;
                        chomp $line;
                        my $rpmver = (split /\s+/, $line)[2];
                        my ($V1,$V2,$V3)=split(/\./,$rpmver);
                        return $V1 >= 4 if $V1 <=> 4;
                        return $V2 >= 0 if $V2;
                        return $V3 >= 3;
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
                my $cmd="cd $pkgpath;$rpmcmd -ir $imgpath $rpmargs --nopost --notriggers $stages[$stage]{ARGS} $pkglist";
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
                &pre_run_scriptlets($imgpath);
                &run_scriptlets($imgpath,$pkgpath,@order);
        }

	return 1;
} #files_install

sub pre_run_scriptlets {
        my $imgpath=shift;
        if ( -f "$imgpath/var/lib/rpm/Packages" ) {
            # undo db1 configuration
            unlink("$imgpath/etc/rpm/macros.db1");
        }
} #pre_run_scriptlets

sub run_scriptlets {
        my $imgpath=shift;
        my $pkgpath=shift;
        my @order=@_;
        my $postfile="/tmp/SIS_post_scriptlet";
	my $rpmcmd=$main::config->rpm;
        my $post_writing=0;
        $ENV{PATH}="/sbin:/usr/sbin:/bin:/usr/bin:$ENV{PATH}";
        verbose("Extracting and running scriptlets from rpms.");
        foreach my $pkg (@order) {
                my $piprog=undef;
                print "$pkg:" if &get_verbose;
                my $cmd="$rpmcmd -q --scripts --root $imgpath $pkg";
                open (SCRIPTQ,"$cmd |");
                while (<SCRIPTQ>) {
                        chomp;
                        if (/^postinstall script/) {
                                my $shell = $_;
                                $shell =~ s/^[^\/]*//;
                                $shell =~ s/\).*$//;
                                print " $shell" if &get_verbose;
                                open (POST,">$imgpath$postfile");
                                print POST "#!$shell\n";
                                print POST "# Post scriptlet for $pkg\n\n";
                                $post_writing=1;
                        } elsif (/^postinstall program/) {
                                $piprog=$_;
                                $piprog=~s/^.*://;
                                $piprog=~s/^ *//;
                                $piprog=~s/ *$//;
                                $piprog=undef if $piprog eq "/bin/sh";
                                print " $piprog" if $piprog && &get_verbose;
                        } elsif (/^(post|pre)(un)?install/) {
                                $post_writing=0;
                        } elsif( $post_writing ) {
                                print POST "$_\n";
                        }
                }
                print "\n" if &get_verbose;
                close (POST);
                close SCRIPTQ;
                if ($piprog) {
                        if (system("chroot $imgpath $piprog")) {
                                carp("Post program for $pkg, failed.");
                        }
                }
                if (-f $imgpath.$postfile) {
                        chmod(0777,$imgpath.$postfile);
                        my $cmd="chroot $imgpath $postfile 1";
                        if (system($cmd)) {
                                carp("Post scriptlet for $pkg, failed.");
                                copy($imgpath.$postfile,$imgpath.$postfile.".failed.".$pkg);
                        }
                        unlink($imgpath.$postfile);
                }

        }

} # run_scriptlets


### POD from here down

=head1 NAME
 
SystemInstaller::Package::RpmNoScripts - Rpm package installation functions.
 
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
