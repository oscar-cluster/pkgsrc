package OSCAR::Network_Configurator;

# Copyright (C) 2009 Oak Ridge National Laboratory
#                    Geoffroy Vallee <valleegr at ornl dot gov>
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

use OSCAR::SystemServices;
use OSCAR::SystemServicesDefs;

sub new {
    my $invocant = shift;
    my $class = ref($invocant) || $invocant;
    my $self = { 
        config_file => "/etc/oscar/oscar.conf", 
        @_,
    };
    bless ($self, $class);
    if (sanity_check ($self)) {
        carp "ERROR: tools are missong";
        return undef;
    }
    return $self;
}


# Return: 0 if success, -1 else.
sub sanity_check ($) {
    my $self = @_;

    # we check if the brctl tool is available
    if (! -f "/usr/sbin/brctl") {
        carp "ERROR: the brctl tool is not available";
        return -1;
    }

    return 0;
}

# Parse the output of the "brctl show" command.
#
# Return: hash representing data about all bridges. The hash has the following
#         format:
#         %cfg = { 'qemubr0' =>
#                   {
#                       'id'    => '8000.001217318d3e',
#                       'stp'   => 'no',
#                       'interfaces'    => 'eth0',
#                   },
#                }
sub parse_brctl_show_output ($$) {
    my ($self, $output) = @_;

    my @lines = split ("\n", $output);
    # Remember the first line is the headers
    my %cfg;
    for (my $i=1; $i<scalar (@lines); $i++) {
        my @elts = split (" ", $lines[$i]);
        my %data;
        # The first elt is the bridge name
        # The second elt is the bridge id
        $data{'id'} = $elts[1];
        # The third elt is the tag to specify is the bridge is STP enable
        $data{'stp'} = $elts[2];
        # The forth elt is the network interfaces inside the bridge
        $data{'interfaces'} = $elts[3];

        # Now we create the final hash with all the data
        $cfg{$elts[0]} = \%data;
    }

    return %cfg;
}

# Add a create a new bridge.
#
# Return: -1 if error, 0 is success and 1 if the bridge already exists.
sub create_bridge ($$) {
    my ($self, $bridge_id) = @_;
    my $cmd;

    # First we check if the bridge already exists
    my $output = `/usr/sbin/brctl show`;
    my %cfg = $self->parse_brctl_show_output ($output);

    # If not, we create it.
    $cmd = "/usr/sbin/brctl addbr $bridge_id";
    if (system $cmd) {
        carp "ERROR: Impossible to execute $cmd";
        return -1
    }

    return 0;
}


# Return: 0 if success, -1 else.
sub add_nic_to_bridge ($$$$) {
    my ($self, $nic, $bridge, $options) = @_;
    my $cmd;

    # First we create the bridge
    if (create_bridge($bridge) == -1) {
        carp "ERROR: Impossible to create the bridge $bridge";
        return -1;
    }

    # Then we add the nic to the bridge
    $cmd = "/usr/sbin/brctl addif $bridge $nic";
    if (system $cmd) {
        carp "ERROR: Impossible to execute $cmd";
        return -1l
    }

    # Then we parse the options to see if we need to do something else
    my %opts = %{$options};
    if (exists $opts{'dhcp_restart'} && $opts{'dhcp_restart'} eq "yes") {
        !system_service(DHCP(),RESTART())
            or (carp "ERROR: Couldn't restart dhcp service.\n", return -1); 
        }
    }
}

1;

__END__
