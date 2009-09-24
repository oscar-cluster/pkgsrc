package OSCAR::NetworkConfigDefs;

# $Id: SystemServicesDefs.pm 7784 2009-01-06 18:12:09Z valleegr $
#
# Copyright (c) 2008-2009 Geoffroy Vallee <valleegr@ornl.gov>
#                         Oak Ridge National Laboratory.
#                         All rights reserved.

use strict;
use base qw(Exporter);

use constant SUPPORTED_OPTIONS => qw (restart_dhcpd);

my @ISA = qw(Exporter);

our @EXPORT = qw(SUPPORTED_OPTIONS);

1;

__END__

=head1 DESCRIPTION

This Perl module gives the list of supported options by the network-configurator scripts.

=head1 SUPPORTED OPTIONS

=over 8

=item restart_dhcpd

When a NIC is added to a bridge, the DCHP daemon is automatically restarted.

=back

=head1 AUTHORS

Geoffroy Vallee <valleegr at ornl dot gov>

=cut