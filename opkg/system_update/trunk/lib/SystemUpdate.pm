package OSCAR::SystemUpdate;

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

# We do not include all the needed Perl modules here because some functions
# are executed in chroot env when dealing with images, in such cases, not all
# Perl modules are available.use OSCAR::Database;
use OSCAR::FileUtils;
use OSCAR::Logger;
use OSCAR::Utils;

use vars qw($VERSION @EXPORT);
use base qw(Exporter);

@EXPORT = qw (
                get_list_local_binary_packages
                get_package_list_from_image
             );

our $update_script = "/usr/bin/oscar-system-update";
our $file_list_binary = "/tmp/list_binary.txt";
our $remote_list = "/tmp/image_package_list.txt";
our $images_dir = "/var/lib/systemimager/images";

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
    require OSCAR::PackMan;
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

# This function translates a list of hostnames to C3 indexes. This allows one to
# later use C3 cmds. We hide this in a function because the current C3 
# implementation suffers of limitations regarding the parsing of the results.
# Having this function, it is easy to simplify the code based on improvements of
# new C3 versions.
sub translate_hostnames_to_c3indexes (@) {
    my (@hosts) = @_;
    my ($cmd, $output);
    my @indexes;

    foreach my $host (@hosts) {
        $cmd = "cnum $host";
        $output = `$cmd`;
        if ($output =~ /index (.*) in/) {
            push (@indexes, $1);
        }
    }

    return @indexes;
}

sub get_package_list_from_image ($) {
    my ($image_name) = @_;
    my $image_path = "$images_dir/$image_name";

    # We check first that the image includes our package
    my $script_in_image = "$image_path$update_script";
    if (! -f "$script_in_image") {
        carp "ERROR: Image does not include oscar-update ($script_in_image)";
        return -1;
    }

    # We get the list of binary packages from the image
    my $cmd = "chroot $image_path $update_script --get-local-config";
    print "Executing: $cmd\n";
    if (system ($cmd)) {
        carp "ERROR: Impossible to execute $cmd";
        return -1;
    }

    if (! -f "$image_path$file_list_binary") {
        carp "ERROR: $image_path$file_list_binary not available";
        return -1;
    }

    # We get the list of clients associated to that image
    require OSCAR::Database;
    require OSCAR::Database_generic;
    my $sql = "select id from Images where name='$image_name'";
    my $image_id = OSCAR::Database::oda_query_single_result ($sql, "id");
    $sql = "select hostname from Nodes where image_id='$image_id'";
    my @res;
    if (OSCAR::Database_generic::do_select ($sql, \@res, undef, undef) != 1) {
        carp "ERROR: Impossible to execute SQL command: $sql";
        return -1;
    }

    if (scalar @res <= 0) {
        carp "ERROR: No nodes are assigned to the image $image_name";
        return -1;
    }
    my @nodes;
    foreach my $i (@res) {
        push (@nodes, $i->{'hostname'});
    }

    # We try to push the file to the nodes
    my @indexes = translate_hostnames_to_c3indexes (@nodes);
    OSCAR::Utils::print_array (@indexes);
    my $args = ":".join (",", @indexes);
    $cmd = "/usr/bin/cpush $args $image_path$file_list_binary $remote_list";
    if (system ($cmd)) {
        carp "ERROR: Impossible to execute $cmd";
        return -1;
    }

    return 0;
}

1;
