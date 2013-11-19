package OSCAR::VirtualCluster;

# Copyright (C) 2006-2008   Oak Ridge National Laboratory
#                           Geoffroy Vallee <valleegr@ornl.gov>
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
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#
# $Id$
#

BEGIN {
    if (defined $ENV{OSCAR_HOME}) {
        unshift @INC, "$ENV{OSCAR_HOME}/lib";
    }
}

use strict;
use OSCAR::Logger;
use OSCAR::Utils;
use OSCAR::FileUtils;
use OSCAR::SystemServices;
use OSCAR::SystemServicesDefs;
use OSCAR::OCA::OS_Settings;
# use OSCAR::MAC;
# use SIS::DB;
# use SIS::Client;
# use SIS::Adapter;
use Carp;

use vars qw($VERSION @EXPORT);
use base qw(Exporter);

our @new_virtual_nodes = ();
our $config_path = "/etc/oscar/oscarv";
our $mapping_file = "$config_path/mapping.xml";
our $image_file = "$config_path/vm_images";
our $vm_file = "$config_path/virtual_compute_nodes.xml";
our $oscarv_interface = "eth0";
our $destroyed = 0;
our %HOSTOS = ();
our %VIRTUALNODES = ();
my $v2m_file_dir = '/tmp/oscarv/';
my $ORDER = 1;

@EXPORT = qw (
                add_vm_image
                assign_vm_to_hostos
                generate_new_vm_config_file
                generate_new_vm_image_config_file
                init_oscarv
                load_virtual_nodes_from_file
                sortclients
                vc_deploy
                @new_virtual_nodes
                $oscarv_interface
             );


################################################################################
# This function initialize OSCAR-V, we typically check that needed files       #
# exists.                                                                      #
#                                                                              #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub init_oscarv ($) {
    my $verbose = shift;

    if (! -d "$config_path") {
        print "[INFO] The directory $config_path does not exist, i create it\n"
            if ($verbose);
        my $ret = mkdir ($config_path, 0750);
        if ($ret != 1) {
            carp "ERROR: Impossible to create $config_path ($ret)";
            return -1;
        }
    }

    if (! -f $image_file) {
        print "[INFO] The file for VM images does not exist, i create it\n" 
            if $verbose;
        system ("touch $image_file");
    }

    if (! -f $mapping_file) {
        print "[INFO] the file for the mapping VM/HostOS does not exist ".
              "($mapping_file), i create it\n";
        if (OSCAR::FileUtils::generate_empty_xml_file ($mapping_file, 
                                                       $verbose)) {
            carp "ERROR: Impossible to create the $mapping_file XML file";
            return -1;
        }
    }

    return 0;
}

sub assign_vm_to_hostos ($$$) {
    my ($vm, $hostos, $verbose) = @_;

    if (!OSCAR::Utils::is_a_valid_string ($vm)
        || !OSCAR::Utils::is_a_valid_string ($hostos)) {
        carp "ERROR: Invalid parameters";
        return -1;
    }

    if (load_mapping_from_file ()) {
        carp "ERROR: Impossible to load the mapping VM/HostOS";
        return -1;
    }

    if (add_vm_to_hash ($vm, $hostos, $verbose)) {
        carp "ERROR: Impossible to add the vm ($vm, $hostos)";
        return -1;
    }

    if (save_mapping_vm_hostos ()) {
        carp "ERROR: Impossible to save the mapping VM/HostOS";
        return -1;
    }

    return 0;
}

################################################################################
# This function adds an image for a VM: it saves meta-data and create the      #
# image if needed.                                                             #
#                                                                              #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub add_vm_image ($$) {
    my ($vm_image, $distro) = @_;

    if (! -f $image_file) {
        carp "ERROR: $config_path does not exist, please check if OSCAR-V is ".
             "correctly initialized";
        return -1;
    }

    #
    # We create the image if needed
    #
    require OSCAR::ImageMgt;
    my %vars = OSCAR::ImageMgt::get_image_default_settings ();
    $vars{distro} = $distro;
    if (OSCAR::ImageMgt::image_exists ($vm_image) == 0) {
        if (OSCAR::ImageMgt::create_image ($vm_image, %vars)) {
            carp "ERROR: Impossible to create the image for $distro";
            return -1;
        }
    }

    #
    # We save meta-data
    #
    my $line = "$vm_image\t$distro\n";
    if (OSCAR::FileUtils::add_line_to_file_without_duplication ($line,
                                                                $image_file)) {
        carp "ERROR: Impossible to add the line $line to $image_file";
        return -1;
    }

    return 0;
}

################################################################################
# Generate a V2M profile based on VM's characteristics.                        #
#                                                                              #
# Input: vm_info, reference to a hash representing data about the VM. The hash #
#                 has the following structure:                                 #
#                 my %vm_info = (                                              #
#                   name => 'my_vm',                                           #
#                   memory => '512',                                           #
#                   disk_image => '/tmp/my_vm.img',                            #
#                   techno => 'Kvm',                                           #
#                 );                                                           #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub generate_v2m_profile ($) {
    my ($vm_info) = @_;

#     my $adapter = list_adapter(client=>$vm_name,devname=>"eth0");
#     my $mac = $adapter->mac;

    if (! -d $v2m_file_dir) {
        print "The directory for saving V2M configuration file ".
            "($v2m_file_dir) does not exist, we create it\n";
        mkdir ($v2m_file_dir, 0750);
    }
    my $vm_name = $$vm_info{'name'};
    my $vm_techno = $$vm_info{'techno'};
    my $path = $v2m_file_dir.$vm_name.".xml";
    open (FILE, ">$path");
    print FILE "<?xml version=\"1.0\"?>\n";
    print FILE "<!DOCTYPE profile PUBLIC \"\" \"/etc/v3m/v3m_profile.dtd\">\n";
    print FILE "<profile>\n";
    print FILE "\t<name>$vm_name</name>\n";
    print FILE "\t<type>$vm_techno</type>\n";
    print FILE "\t<image size=\"2000\">/opt/v2m/data/$vm_name.img</image>\n";
    print FILE "\t<cdrom>/opt/v2m/contrib/bin/oscar_bootcd.img</cdrom>\n";
    print FILE "\t<nic1>\n";
    print FILE "\t\t<type>TUN/TAP</type>\n";
#     print FILE "\t\t<mac>$mac</mac>\n";
    print FILE "\t</nic1>\n";
    print FILE "</profile>\n";
    close (FILE);
    print "Configuration file for $vm_name generated!\n";
    return -1;
}


# Load the list of virtual compute nodes. Note that this list is currently
# in a speudo-database, i.e., an XML file:
# ($ENV{OSCAR_HOME}/data/virtual_compute_nodes.xml
sub load_virtual_nodes_from_file () {
    if (! -f $mapping_file) {
        print "The file $mapping_file does not exist, " .
              "no previous virtual cluster exists\n";
    } else {
        my $xml_data = OSCAR::FileUtils::parse_xmlfile ($mapping_file);
        @new_virtual_nodes = ();
        my $base = $xml_data->{virtual_nodes}->[0]->{node};
        #
        # If the file is empty, we just exist successfully
        #
        return 0 if (!defined $base);
        print "Nb of nodes: ".scalar(@{$base})."\n";
        for (my $i=0; $i < scalar(@{$base}); $i++) {
            my $name = $xml_data->{virtual_nodes}->[0]->{node}->[$i]->{name}->[0]."\n";
            my @list = list_client();
            # We get information about the virtual compute node via the SIS DB
            # Note that if a node is not anymore in the SIS DB (if the node has
            # been deleted for example), the virtual node is automatically
            # removed.
            foreach my $c (@list) {
                chomp($name);
#                print "Comparing $name to ".$c->name."\n";
                if ($name eq $c->name) {
                    print "Client found in the DB\n";
                    push (@new_virtual_nodes, $c);
                    last;
                }
            }
       }
       print "Loaded nodes: ";
       foreach my $i (@new_virtual_nodes) {
           print $i->name." ";
       }
       print "\n";
    }
    return 1;
}

# Use Schwartzian transform to sort clients by node names alphabetically and numerically.
# Names w/o numeric suffix precede those with numeric suffix.
# Duplicated code
sub sortclients(@) {
    return map { $_->[0] }
           sort { $a->[1] cmp $b->[1] || ($a->[2]||-1) <=> ($b->[2]||-1) }
           map { [$_, $_->name =~ /^([\D]+)([\d]*)$/] }
           @_;
}

sub populate_virtual_nodes {
    my @clients = sortclients @new_virtual_nodes;
    # We load the mapping we already did
    load_mapping_from_file();
    # We now add new virtual nodes. Note that if the node is already in the
    # hash, it will not be added to be sure we do not loose information.
    foreach my $client (@clients) {
        add_vn_to_hash($client->name, "");
    }
}

sub populate_hostos {
    # HostOSes are possibly defined OSCAR clients minus virtual compute nodes
    my @oscar_clients = sortclients list_client();
    my @virtual_cn = sortclients @new_virtual_nodes;
    for (my $t=0; $t < scalar(@oscar_clients); $t++) {
        my $n2 = $oscar_clients[$t]->name;
        for (my $i=0; $i < scalar(@virtual_cn); $i++) {
            my $n = $virtual_cn[$i]->name;
            if (defined (list_client(name=>$n))) {
                print "Comparing $n2 and $n\n";
                if ($n2 eq $n) {
                    print "We remove a virtual node (".$n.") from the list\n";
                    delete @oscar_clients[$t];
                    last;
                }
            }
        } 
    }
    # Now oscar_clients should contain only clients that are not virtual nodes
    foreach my $c (@oscar_clients) {
        if (defined $c) {
            print "One possible HostOS found: ".$c->name."\n";
            add_client_to_hash($c->name);
        }
    }
}

# Return: 0 if success, -1 else.
sub add_vm_to_hash ($$$) {
    my ($vm_name, $hostos_name, $verbose) = @_;

    if (!OSCAR::Utils::is_a_valid_string ($vm_name) 
        || !OSCAR::Utils::is_a_valid_string ($hostos_name)) {
        carp "ERROR: Invalid arguments";
        return -1;
    }

    if (!exists $VIRTUALNODES{$vm_name}) {
        $VIRTUALNODES{$vm_name} = {
                  vm => $vm_name,
                  hostos => $hostos_name,
                 };
    } else {
        print "[INFO] The key already exists, we do not overload\n" if $verbose;
    }

    return 0;;
}


sub load_from_file ($) {
    my $file = shift;
    print "File: $file\n";
    open(IN,"<$file") or croak "Couldn't open file: $file for reading";
    while(<IN>) {
        my $name = $1;
        add_client_to_hash($name);
    }
    close(IN);
    return 1;
}


sub add_client_to_hash ($) {
    my $client = shift;
    $HOSTOS{$client} = {
                  vm => $client,
                 };
    return 1;
}

# Return: 0 if sucess, -1 else.
sub save_mapping_vm_hostos () {
    my %data_to_save;
    my $n = 0;

    if (! -f $mapping_file) {
        carp "ERROR: the $mapping_file file does not exist";
        return -1;
    }

    my $xsimple = XML::Simple->new();
    foreach my $key (keys %VIRTUALNODES) {
        # We only need to save the name of each virtual compute node, other
        # data are available via the SIS database (the name is the key for
        # machines)
        %data_to_save->{'mapping'}->{'map'}[$n]->{'vm_name'}
            = $VIRTUALNODES{$key}->{'vm'};
        %data_to_save->{'mapping'}->{'map'}[$n]->{'host_name'}
            = $VIRTUALNODES{$key}->{'hostos'};
        $n++;
    }

    $xsimple->XMLout(\%data_to_save,
                     noattr => 1,
                     OutputFile => $mapping_file,
                     xmldecl => '<?xml version="1.0" encoding="ISO-8859-1"?>');
    return 0;
}

# Return: 0 if success, -1 else.
sub load_mapping_from_file () {
    if (-f $mapping_file) {
        my $xml_data = OSCAR::FileUtils::parse_xmlfile($mapping_file);
        my $base = $xml_data->{mapping}->{map};

        #
        # If the file is empty, we simply exit successfully
        #
        return 0 if !defined $base;

        print "Nb of maps: ".scalar(@{$base})."\n";
        for (my $i=0; $i < scalar(@{$base}); $i++) {
            my $hostos_name
                = $xml_data->{mapping}->{map}->[$i]->{host_name}."\n";
            my $vm_name
                = $xml_data->{mapping}->{map}->[$i]->{vm_name}."\n";
            chomp($vm_name);
            chomp($hostos_name);
            # We check if the node really exist, i.e., is in the SIS DB
#             my $vnode = list_client(name=>$vm_name);
            # WARNING we do not check if the map is already in the hash
            print "Map found (vm,hostos)=($vm_name,$hostos_name)\n";
            add_vm_to_hash ($vm_name, $hostos_name, 1);
        }
    } else {
        carp "ERROR: file $mapping_file does not exist";
        return -1;
    }
    return 0;
}


sub deploy_virtual_cluster {
    my $net_interface = shift;

    print "Deploying the virtual cluster (using the NIC $net_interface)\n";

    # First we update the DHCP server configuration
    rebuild_dhcp_conf ($net_interface);
    __setup_dhcpd ($net_interface);

    # Then we generate all the config file for v2m and we copy them on the 
    # hostOS
    foreach my $vm (keys %VIRTUALNODES) {
        generate_v2m_profile (\($VIRTUALNODES{$vm}));
        copy_v2m_config_file_to_hostos ($VIRTUALNODES{$vm}->{'name'}, $VIRTUALNODES{$vm}->{'hostos'});
    }

    # Based on the v2m config files, we start up the VMs
    foreach my $vm (keys %VIRTUALNODES) {
        boot_vm ($VIRTUALNODES{$vm}->{'name'}, $VIRTUALNODES{$vm}->{'hostos'});
    }

    return 0;
}


sub copy_v2m_config_file_to_hostos {
    my $vm = shift;
    my $hostos = shift;
    my $cmd = "scp ".$v2m_file_dir.$vm.".xml $hostos:/opt/v2m/data";

    print "Executing: $cmd\n";
    system ($cmd) or carp "Impossible to copy the V2M file to the hostOS";
}

# Duplicated code (from scripts/update_live_macs)
sub rebuild_dhcp_conf {
    my $net_interface = shift;
    my $dhcp_conf = OSCAR::OCA::OS_Settings::getitem(DHCP()."_configfile");
    #   find default gateway in DHCP_server config file 
    open IN, "$dhcp_conf" or die "Could not open $dhcp_conf!";
    my ($gwip, $netmask);
    while (<IN>) {
        next if (/^\s*\#/);
        if (/\s*option routers (\d+\.\d+\.\d+\.\d+);/) {
        $gwip = $1;
        last if ($netmask);
        }
        if (/\s*option subnet-mask (\d+\.\d+\.\d+\.\d+);/) {
        $netmask = $1;
        last if ($gwip);
        }
    }
    close (IN);
    if (!defined($gwip) || !defined($netmask)) {
        die "Could not determine gateway IP for $dhcp_conf and/or netmask!";
    }

    # OL: FIXME: Need to test return codes.
    system("mkdhcpconf -o $dhcp_conf --interface=$net_interface --gateway=$gwip");

    # Restart the DHCP service.
    !system_service(DHCP(),RESTART())
        or (print "ERROR: Couldn't restart dhcp service.\n", return -1);
}

sub boot_vm {
    my $vm = shift;
    my $hostos = shift;

    print "Booting VM $vm on Host OS $hostos\n";
    my $cmd = "scp ".$v2m_file_dir.$vm.".xml $hostos:/opt/v2m/data";
    print "Executing: $cmd\n";
    system ($cmd);
    my $cmd = "ssh $hostos 'v2m /opt/v2m/data/".$vm.".xml --install-vm-with-oscar &'";
    print "Executing: $cmd\n";
    system ($cmd);
}

1;

__END__

=head1 Configuration Files

OSCAR-V is based on 2 different configuration files, all of them stored into
/etc/oscar/oscarv (those configuration can be manually modified but we strongly
advice you to use the oscar-v script to do so):

=over 8

=item vm_images

DEPRECATED?

=item mapping.xml

This file stores the mapping HostOS / virtual machine. It typically allows one
to locate a given virtual machine and it also allows OSCAR-V to know where
commands for virtual machine management should be executed.

=back

=head1 Exported functions

=over 8

=item add_vm_image

Create a new image for the deployment of virtual machines.

=item assign_vm_to_hostos

Specify the mapping virtual machine / host OS.

=item generate_new_vm_config_file

Generate a new configuration file (a.k.a. profile) for a given virtual machine.

=item generate_new_vm_image_config_file

=item init_oscarv

Initialize OSCAR-V and its execution environment.

=item load_virtual_nodes_from_file

=item sortclients

=item vc_deploy

=item @new_virtual_nodes

=item $oscarv_interface

=back

=cut
