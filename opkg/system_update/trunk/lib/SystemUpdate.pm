package SystemUpdate;

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#   Copyright (c) 2010  Geoffroy Vallee <valleegr@ornl.gov>
#                       Oak Ridge National Laboratory
#                       All rights reserved.

BEGIN {
    if (defined $ENV{OSCAR_HOME}) {
        unshift @INC, "$ENV{OSCAR_HOME}/lib";
    }
}

use strict;
use warnings "all";
use Carp;
use Fcntl;

use OSCAR::FileUtils;
use OSCAR::Logger;
use OSCAR::PackMan;

use vars qw($VERSION @EXPORT);
use base qw(Exporter);

@EXPORT = qw (
             );

# Get the list of binary packages installed on the local system.
sub get_list_local_binary_packages ($) {
    my ($output_file) = @_;
    my @data;

    # PackMan initialization
    my $os = OSCAR::OCA::OS_Detect::open ();
    if (!defined $os) {
        carp "ERROR: Impossible to decompose the distro ID";
        return -1;
    }

    my $pm;
    if ($os->{pkg} eq "deb") {
        $pm = PackMan::DEB->new;
    } elsif ($os->{pkg} eq "rpm") {
        $pm = PackMan::RPM->new;
    } else {
        carp "ERROR: Unknown binary package format (".$os->{pkg}.")";
        return -1;
    }

    if (!defined $pm) {
        carp "ERROR: Impossible to create a PackMan object";
        return -1;
    }

    # We get the list of installed binary packages
    @data = $pm->query_list_installed_pkgs();
        OSCAR::Utils::print_array (@data);

    # We save the list into the output file
    if (-f $output_file) {
        unlink ($output_file);
    }
    foreach my $p (@data) {
        OSCAR::FileUtils::add_line_to_file_without_duplication ("$p\n", 
                                                                $output_file);
    }

    return 0;
}

get_list_local_binary_packages ("/tmp/toto");

1;
