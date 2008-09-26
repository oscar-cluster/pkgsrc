package OSCAR::VirtualCluster;

# Copyright (C) 2006    Oak Ridge National Laboratory
#                       Geoffroy Vallee <valleegr@ornl.gov>
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

use strict;
use lib "$ENV{OSCAR_HOME}/lib";
use Tk;
use Tk::Tree;
use OSCAR::Tk;
use OSCAR::WizardEnv;
use OSCAR::Logger;
use OSCAR::MAC;
use SIS::DB;
use SIS::Client;
use SIS::Adapter;
use Carp;

use vars qw($VERSION @EXPORT);
use base qw(Exporter);

our @new_virtual_nodes = ();

@EXPORT = qw (  
                vc_deploy
                load_virtual_nodes_from_file
                sortclients
                generate_empty_xml_file
                @new_virtual_nodes
                $oscarv_interface
             );
             
our $oscarv_interface = "eth0";
our $destroyed = 0;
our %HOSTOS = ();
our %VIRTUALNODES = ();
our $mapping_file = $ENV{OSCAR_HOME}.'/data/mapping.xml';
my $v2m_file_dir = '/tmp/oscarv/';
my $ORDER = 1;

# window for the definition of a virtual cluster
sub vc_deploy {
    $destroyed = 1;
    print "Virtual Cluster Setup Window\n";
    my $parent = shift;
    our ($vars) = @_;

    my $path = $ENV{OSCAR_HOME}."/data";
    if (! -d $path) {
        print "It seems that $path does not exist, we create it\n";
        mkdir ($path, 0700);
    }

    my $file = $path . "/hostos_mapping.xml";
    if (! -f $file) {
        print "It seems that no virtal clusters have been defined previously\n";
    }

    $parent->Busy(-recurse => 1);
    
    our $window = $parent->Toplevel;
    $window->withdraw;
    $window->title("Setup Virtual Cluster");

    my $frame = $window->Frame();
    my $topframe = $window->Frame();

    $frame->pack(-side => "bottom", -fill => "both", -expand => 1);
    $topframe = $window->Frame();

    our $exitbutton = $frame->Button(
        -text => "Close",
        -borderwidth => "6",
        -relief => "groove",
        -command => sub {
                        undef $destroyed;
                        oscar_log_subsection("Success");
                        $parent->Unbusy();
                        $window->destroy;
                        },
        );

    our $vlistbox = $topframe->ScrlListbox(
        -selectmode => 'single',
        -background => "white",
        -scrollbars => 'osoe',
        );

    # the "All Clients" widget
    our $vtree = $topframe->Scrolled("Tree",
        -background => "white",
        -itemtype => 'imagetext',
        -separator => '|',
        -selectmode => 'single',
        -scrollbars => 'osoe',
        -width => 40,
        );

    $vlistbox->bind( "<ButtonRelease>", \&set_buttons );
    $vtree->bind( "<ButtonRelease>", \&set_buttons );

    $frame->pack(-side => "bottom", -fill => "both", -expand => 1);
    $topframe->pack(-side => 'top', -fill => "both", -expand => 1);

    $vlistbox->grid('-', $vtree, -sticky => 'nsew');
    $topframe->gridColumnconfigure(0, -weight => 1);
    $topframe->gridColumnconfigure(1, -weight => 1);
    $topframe->gridColumnconfigure(2, -weight => 2);

    our $loadbutton = $frame->Menubutton(
        -text => "Import IPs of Host OSes from",
        -menuitems => [ [ 'command' => "file...",
                          "-command" => [\&hostosip_file_selector, "load", $frame] ],
                        [ 'command' => "user input...",
                          "-command" => \&ips_inputer ],
                      ],
        -tearoff => 0,
        -direction => "right",
        -relief => "raised",
        -indicatoron => 1,
        );

    our $assignall   = $frame->Button(
                                     -text => "Assign all Host OSes",
                                     -command => \&assign_all_hostos,
                                     -state => "disabled",
                                    );

    our $assignbutton = $frame->Button(
                                      -text => "Assign Virtual Machine to Host OS",
                                      -command => \&assign_hostos_to_vm,
                                      -state => "disabled",
                                     );

    our $deploy_vc_button = $frame->Button(
        -text => "Deploy the Virtual Cluster",
        -command => [\&deploy_virtual_cluster, $oscarv_interface],
        -state => "active",
        );

    our $setup_vc_button = $frame->Button(
        -text => "Setup the Virtual Cluster",
        -command => [\&run_post_install, $window, $step_number],
#[\&setup_virtual_cluster, undef, undef],
        -state => "active",
        );


# This is what the widget looks like:
# |------------------------------------------------------------------------------|
# |                           Virtual Cluster Management                         |
# |------------------------------------------- ----------------------------------|
# | Import IPs of Host OSes from | Assign all Host OSes | Assign Host OS to Node |
# | -----------------------------------------------------------------------------|
# |                           Deploy the Virtual Cluster                         |
# | -----------------------------------------------------------------------------|
# |                            Setup the Virutal Cluster                         |
# | -----------------------------------------------------------------------------|
# |                                    Close                                     |
# |------------------------------------------------------------------------------|


    my $vc_label = $frame->Label(-text => "Virtual Cluster Deployment",
                                  -relief => 'sunken');
    $vc_label->grid("-", "-", -sticky => "ew");
    $loadbutton->grid($assignall, $assignbutton, -sticky => "ew");
#    $assignall->grid($assignbutton, -sticky => "ew");
    $deploy_vc_button->grid("-","-",-sticky=>"nsew",-ipady=>"4");
    $setup_vc_button->grid("-","-",-sticky=>"nsew",-ipady=>"4");
    $exitbutton->grid("-","-",-sticky=>"nsew",-ipady=>"4");
    $window->bind('<Destroy>', sub {
                                    if ( defined($destroyed) ) {
                                      undef $destroyed;
                                      $exitbutton->invoke();
                                      return;
                                    }
                                   });

    # this populates the tree as it exists
    populate_virtual_nodes();
    populate_hostos();

    regenerate_tree();
    regenerate_listbox();
    center_window( $window );
}

# Load the list of virtual compute nodes. Note that this list is currently
# in a speudo-database, i.e., an XML file:
# ($ENV{OSCAR_HOME}/data/virtual_compute_nodes.xml
sub load_virtual_nodes_from_file {
    my $data_file = $ENV{OSCAR_HOME}.'/data/virtual_compute_nodes.xml';
    if (! -f $data_file) {
        print "The file $data_file does not exist, " .
              "no previous virtual cluster exists\n";
    } else {
        my $simple = XML::Simple->new(ForceArray => 1);
        my $xml_data = $simple->XMLin($data_file);
#    @new_virtual_nodes = ();
        print "Loading virtual compute nodes from the DB...\n";
        my $base = $xml_data->{virtual_nodes}->[0]->{node};
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

sub add_vn_to_hash {
    my ($client_name, $hostos_name) = @_;
    if (!exists $VIRTUALNODES{$client_name}) {
        $VIRTUALNODES{$client_name} = {
                  name => $client_name,
                  hostos => $hostos_name,
                 };
    } else {
        print "The key already exists, we do not overload\n";
    }
    return 1;
}

sub regenerate_tree {
    our $vtree;
    $vtree->delete("all");
    $vtree->add("|",-text => "Virtual Compute Nodes",-itemtype => "text");
    foreach my $vm (keys %VIRTUALNODES) {
        my $adapter = list_adapter(client=>$VIRTUALNODES{$vm}->{'name'},devname=>"eth0");
        $vtree->add("|".$VIRTUALNODES{$vm}->{'name'}, -text => $VIRTUALNODES{$vm}->{'name'}, -itemtype => "text");
        $vtree->add("|".$VIRTUALNODES{$vm}->{'name'} . "|hostOS",
                   -text => "hostOS = " . $VIRTUALNODES{$vm}->{'hostos'}, -itemtype => "text");
        $vtree->add("|".$VIRTUALNODES{$vm}->{'name'} . "|ip" . $adapter->devname,
           -text => $adapter->devname . " ip = " . $adapter->ip, -itemtype => "text");
    }
    $vtree->autosetmode;
    set_buttons();
}

sub set_buttons {
    our $vlistbox;
    our $vtree;
    my $state;
    my $lbs;
    my $trs;
#
#   Enabled iff at least one item selected in the listbox.
#
    $lbs = defined $vlistbox->curselection();
    $state = $lbs ? "normal" : "disabled";
#    our $clear->configure( -state => $state );

#   Enabled iff at least one item in listbox.

    $lbs = defined $vlistbox->get( 0, 'end' );
    $state = $lbs ? "normal" : "disabled";
#    our $clearall->configure( -state => $state );

#   Enabled iff at least one item in listbox and one item in tree.

    $trs = $vtree->infoNext( "|" );
    $state = ($lbs && $trs) ? "normal" : "disabled";
    our $assignall->configure( -state => $state );

#   Enabled iff at least one MAC exists.

#    $state = (scalar keys %HOSTOS) ? "normal" : "disabled";
#    our $savebutton->configure( -state => $state );

#   Enabled iff at least one item selected in the listbox and the tree.

    $trs = defined $vtree->infoSelection();
    $state = ($lbs && $trs) ? "normal" : "disabled";
    our $assignbutton->configure( -state => $state );

#   Enabled iff at least one item selected in listbox and selected item in tree has a MAC.

    my $node = $vtree->infoSelection();

    # hack to support both perl-Tk-800 and perl-Tk-804
    $node = $$node[0] if ref($node) eq "ARRAY";

    if( $trs && $node =~ /^\|([^\|]+)/) {
        my $client = list_client(name=>$1);
        my $adapter = list_adapter(client=>$client->name,devname=>"eth0");
        $state = $adapter->mac ? "normal" : "disabled";
    } else {
        $state = "disabled";
    }
#    our $deletebutton->configure( -state => $state );
}

sub hostosip_file_selector {
    my ($op, $widget) = @_;

    # now we attempt to do some reasonable directory setting
    my $dir = $ENV{HOME};
    $dir = dirname( $dir ) unless -d $dir;
    $dir = "/" unless -d $dir;

    if( $op eq "load" ) {
        my $file = $widget->getOpenFile(
            -initialdir => $dir,
            -title => "Import Host OSes from file",
        );
        return 1 unless $file;
        load_from_file( $file );
    } else {
        my $file = $widget->getSaveFile(
            -initialdir => $dir,
            -initialfile => "hostoses",
            -title => "Export Host OSes to file",
        );
        return 1 unless $file;
        save_to_file( $file );
    }
    regenerate_listbox();
    return 1;
}

sub load_from_file {
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

sub regenerate_listbox {
    our $vlistbox;
    $vlistbox->delete(0,"end");
    foreach my $key (keys %HOSTOS) {
#        my $name = $HOSTOS{$key)->{'name'};
        $vlistbox->insert("end",$key);
    }
    $vlistbox->update;
    set_buttons();
}

sub add_client_to_hash {
    my $client = shift;
    $HOSTOS{$client} = {
                  name => $client,
                 };
    return 1;
}

sub assign_hostos_to_vm {
    our $vlistbox;
    our $vtree;
    my $hostos;
    my $vnode;
    my $sel = $vlistbox->curselection;
    if ( defined( $sel ) ) {
        $hostos = $vlistbox->get($vlistbox->curselection) or return undef;
    } else { return undef; }

    if ( defined( $vtree->infoSelection() ) ) {
        $vnode = $vtree->infoSelection() or return undef;
    } else { return undef; }

    my $client;

    # hack to support both perl-Tk-800 and perl-Tk-804...
    $vnode = $$vnode[0] if ref($vnode) eq "ARRAY";
    my $name;
    if($vnode =~ /^\|([^\|]+)/) {
        $name = $1;
        $client = list_client(name=>$name);
    } else {
        return undef;
    }

    print("Assigned ".$hostos." to ".$name."\n");
    foreach my $key (keys %VIRTUALNODES) {
        if ($VIRTUALNODES{$key}->{'name'} eq $name) {
            print "We save the assignement\n";
            $VIRTUALNODES{$key}->{'hostos'} = $hostos;
            last;
        }
    }
    regenerate_tree();
    save_mapping_vm_hostos();
}

sub save_mapping_vm_hostos {
    my $n = 0;
    my %data_to_save;

    if (! -f $mapping_file) {
        generate_empty_xml_file ();
    }
    my $xsimple = XML::Simple->new();

    foreach my $key (keys %VIRTUALNODES) {
        # We only need to save the name of each virtual compute node, other
        # data are available via the SIS database (the name is the key for
        # machines)
        %data_to_save->{'mapping'}->{'map'}[$n]->{'vm_name'} = $VIRTUALNODES{$key}->{'name'};
        %data_to_save->{'mapping'}->{'map'}[$n]->{'host_name'} = $VIRTUALNODES{$key}->{'hostos'};
        $n++;
    }

    $xsimple->XMLout(\%data_to_save,
                        noattr => 1,
                        OutputFile => $mapping_file,
                        xmldecl => '<?xml version="1.0" encoding="ISO-8859-1"?>');
    print "Mapping file saved!\n";
}

sub load_mapping_from_file {
    my $data_file = $mapping_file;
    if (-f $data_file) {
        my $simple = XML::Simple->new(ForceArray => 1);
        my $xml_data = $simple->XMLin($data_file);
        print "Loading virtual compute nodes from the DB...\n";
        my $base = $xml_data->{mapping}->[0]->{map};
        print "Nb of maps: ".scalar(@{$base})."\n";
        for (my $i=0; $i < scalar(@{$base}); $i++) {
            my $hostos_name = $xml_data->{mapping}->[0]->{map}->[$i]->{host_name}->[0]."\n";
            my $vm_name = $xml_data->{mapping}->[0]->{map}->[$i]->{vm_name}->[0]."\n";
            chomp($vm_name);
            chomp($hostos_name);
            # We check if the node really exist, i.e., is in the SIS DB
            my $vnode = list_client(name=>$vm_name);
            # WARNING we do not check if the map is already in the hash
            print "Map found (vm,hostos)=($vm_name,$hostos_name)\n";
            add_vn_to_hash ($vm_name, $hostos_name);
        }
    }
    return 1;    
}

sub generate_empty_xml_file {
    if (! -d $ENV{OSCAR_HOME}.'/data/') {
        print "The directory " . $ENV{OSCAR_HOME}.'/data/' . "does not exist,"; 
        print " i create it";
        mkdir ($ENV{OSCAR_HOME}.'/data/', 0750);
    }
    my $data_file = $ENV{OSCAR_HOME}.'/data/virtual_compute_nodes.xml';

    open (FILE, ">$data_file") or die "can't open $data_file $!";
    print FILE "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    print FILE "<opt/>";
    close (FILE);
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
        generate_v2m_config_file ($VIRTUALNODES{$vm}->{'name'});
        copy_v2m_config_file_to_hostos ($VIRTUALNODES{$vm}->{'name'}, $VIRTUALNODES{$vm}->{'hostos'});
    }
    
    # Based on the v2m config files, we start up the VMs
    foreach my $vm (keys %VIRTUALNODES) {
        boot_vm ($VIRTUALNODES{$vm}->{'name'}, $VIRTUALNODES{$vm}->{'hostos'});
    }
}

sub generate_v2m_config_file {
    my $vm_name = shift;
    my $adapter = list_adapter(client=>$vm_name,devname=>"eth0");
    my $mac = $adapter->mac;
    
    if (! -d $v2m_file_dir) {
        print "The directory for saving V2M configuration file ".
            "($v2m_file_dir) does not exist, we create it\n";
        mkdir ($v2m_file_dir, 0750);
    }
    my $path = $v2m_file_dir.$vm_name.".xml";
    open (FILE, ">$path");
    print FILE "<?xml version=\"1.0\"?>\n";
    print FILE "<!DOCTYPE profile PUBLIC \"\" \"/etc/v3m/v3m_profile.dtd\">\n";
    print FILE "<profile>\n";
    print FILE "\t<name>$vm_name</name>\n";
    print FILE "\t<type>XenHVM</type>\n";
    print FILE "\t<image size=\"2000\">/opt/v2m/data/$vm_name.img</image>\n";
    print FILE "\t<cdrom>/opt/v2m/contrib/bin/oscar_bootcd.img</cdrom>\n";
    print FILE "\t<nic1>\n";
    print FILE "\t\t<type>TUN/TAP</type>\n";
    print FILE "\t\t<mac>$mac</mac>\n";
    print FILE "\t</nic1>\n";
    print FILE "</profile>\n";
    close (FILE);
    print "Configuration file for $vm_name generated!\n";
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

    #   find default gateway in /etc/dhcpd.conf
    open IN, "/etc/dhcpd.conf" or die "Could not open /etc/dhcpd.conf!";
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
        die "Could not determine gateway IP for dhcpd.conf and/or netmask!";
    }

    system("mkdhcpconf -o /etc/dhcpd.conf --interface=$net_interface --gateway=$gwip");
    system("/etc/init.d/dhcpd restart");
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
