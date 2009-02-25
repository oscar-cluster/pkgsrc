package OSCAR::udev;

# Copyright (C) 2009 Oak Ridge National Laboratory
#                    Geoffroy Vallee <valleegr at ornl dot gov>
#                    All rights reserved.
# Copyright (C) 2009 Felipe Zipitria
#                    All rights reserved.
#
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
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US

#
# $Id$
#

BEGIN {
    if (defined $ENV{OSCAR_HOME}) {
        unshift @INC, "$ENV{OSCAR_HOME}/lib";
    }
}

use strict;
use vars qw(@EXPORT);
use base qw(Exporter);
use Carp;

use File::Copy;

@EXPORT = qw(update_udev_net);

sub update_master_script ($) {
    my $image = shift;
    my $file = "/var/lib/systemimager/scripts/$image.master";

    # We look for the string "[ -z $DEVICE ] && DEVICE=eth0"
    my $seek_line1 = "[ -z \$DEVICE ] && DEVICE=eth0"

    # We recreate the master script with the new line
    my $line1 = "$seek_line1\n".
                "logmsg \"--- FIXING ETH0 ---\"\n".
                "ETH0_ADDR=`/sbin/ifconfig \$DEVICE | grep HWaddr | awk '{print \$5}'`\n";
    OSCAR::FileUtils::replace_line_in_file ($file, $seek_line1, $line1);

    my $seek_line2 = "[INTERFACE0]";
    my $line2 = "$seek_line2\n".
                "[USEREXIT0]\n".
                "CMD = sed\n".
                "PARAMS = \"-ie s/MAC/\$ETH0_ADDR/ ".
                    "/etc/udev/rules.d/70-persistent-net.rules";
    OSCAR::FileUtils::replace_line_in_file ($file, $seek_line2, $line2);

    return 0;
}

sub update_udev_net ($) {
    my $image = shift;

    my $udev_dir = "/var/lib/systemimager/images/$image/etc/udev/rules.d";
    my $file = "$udev_dir/60-net.rules";

    # If the net rule exists, we back it up and delete it
    if (-f $file) {
        my $dir = "/var/lib/oscar/backup";
        mkpath ($dir);
        File::Copy::move ($file, $dir);
    }

    # Now we deal with the 70-persistent-net.rules
    my $udev_net = "$udev_dir/70-persistent-net.rules";
    open (UDEVFILE, $udev_net);
    print UDEVFILE "SUBSYSTEM==\"net\", SYSFS{address}==\"MAC\", NAME=\"eth0\"";
    close (UDEVFILE);

    return 0;
}

1;

=head1 DESCRIPTION

A set of usefull functions to update the udev configuration of a given image.

=head1 Exported Functions

=over 4

=item update_udev_net

update_udev_net ($image_name)

=back

=head1 AUTHORS

=over 4

=item Geoffroy Vallee, Oak Ridge National Laboratory

=item Felipe Zipitria

=back

=cut

__END__

