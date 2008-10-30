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

use strict;
use Tk;
use Tk::Tree;
use OSCAR::Tk;
use OSCAR::WizardEnv;

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

1;

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

