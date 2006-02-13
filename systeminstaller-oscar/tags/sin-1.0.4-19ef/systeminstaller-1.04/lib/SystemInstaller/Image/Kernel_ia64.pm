package SystemInstaller::Image::Kernel_ia64;

#   $Header: /cvsroot/systeminstaller/systeminstaller/lib/SystemInstaller/Image/Kernel_ia64.pm,v 1.4 2002/06/11 21:06:45 mchasal Exp $

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

use  File::Basename;
use SystemInstaller::Log qw(verbose get_verbose);
use Carp;

use vars qw($VERSION);

$VERSION = sprintf("%d.%02d", q$Revision: 1.4 $ =~ /(\d+)\.(\d+)/);

#
## API FUNCTIONS
#

sub footprint {
# Look at a directory and determine if it looks like rpms.
# Input:        Package path, image path
# Returns:      Boolean of match
	my $class=shift;
        my $imgpath=shift;
        if (-e "$imgpath/boot/efi") {
                        return 1;
        }
        return 0;

} #footprint

sub find_kernels {
        # Find all the kernels and stick them in a file for later.
        # Input: imagedir
        # Output: boolean
        my $class=shift;
        my $imagedir = shift;
        &verbose("Finding all kernels");
        my @files= glob("$imagedir/boot/efi/*vmlinuz*");
        my @kernels;

        foreach (@files) {
                unless (-B $_ ) {
                        # Chuck non-binary files
                        next;
                }
                my $fn=basename($_);
                my $lab=$fn;
                $lab=~s/^vmlinuz-//;
                my $path=$_;
                $path=~s/^$imagedir//;
                # Put SMP kernels in the front
                if (/smp/) {
                        unshift (@kernels,"$path $lab");
                } else {
                        push (@kernels,"$path $lab");
                }
        }
        return @kernels;
} # find_kernels



### POD from here down

=head1 NAME
 
SystemInstaller::Package::Kernel_ia64 - systemconfig.conf kernel setup.
 
=head1 DESCRIPTION

This module provides the SystemInstall package API function, I<kernel_setup>
for Itanium systems. See the SystemInstaller::Package manpage for details
on the API specification.

=head1 AUTHOR
 
Michael Chase-Salerno <mchasal@users.sf.net>
 
=head1 SEE ALSO

L<SystemInstaller::Package>
L<SystemInstaller::Package::Kernel_x86>
 
=cut

1;
