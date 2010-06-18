package OSCAR::NAT;

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

use OSCAR::Database;
use OSCAR::Logger;
use OSCAR::Utils;

use vars qw($VERSION @EXPORT);
use base qw(Exporter);

@EXPORT = qw (
            get_extif
            generate_iptables_script
             );

sub get_extif () {
    my $sql;
    my $n_id;
    my $extif;

    $sql = "select n_id from Networks where name='public'";
    $n_id = OSCAR::Database::oda_query_single_result ($sql, "n_id");
    $sql = "select name from Nics where network_id='$n_id'";
    $extif = OSCAR::Database::oda_query_single_result ($sql, "name");

    return $extif;
}

sub generate_iptables_script ($$) {
    my ($file, $extif) = @_;

    OSCAR::Logger::oscar_log_subsection ("Generating the NAT script $file");

    if (-f $file) {
        unlink ($file);
    }

    sysopen (MYFILE, "$file", O_RDWR|O_CREAT, 0755) or (carp "ERROR: Impossible to write to $file", 
                                return -1);
    print MYFILE "#!/bin/sh\n#\n\n";
    print MYFILE "echo \"1\" > /proc/sys/net/ipv4/ip_forward\n";
    print MYFILE "/sbin/iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT\n";
    print MYFILE "/sbin/iptables -t nat -A POSTROUTING -o $extif -j MASQUERADE\n";

    close (MYFILE);

    return 0;
}

1;

