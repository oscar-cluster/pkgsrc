package SystemInstaller::Tk::Common;

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

use base qw(Exporter);
use vars qw(@EXPORT %Labels);
use AppConfig;
use File::Basename;
use strict;

@EXPORT = qw(selector2entry
             quit_button
             label_entry_line 
             label_option_line 
             label_listbox_line 
             resethash
             imageexists
             init_si_config
             done_window
             error_window
            );

%Labels = ();

#
#  selector2entry sets up the callback needed for a fileselector to be tied to 
#  an entry box
#  

sub selector2entry {
    my ($var, $selector) = @_;

    # now we attempt to do some reasonable directory setting
    my $dir = $$var;
    if(-d $dir) {
        $selector->configure(-directory => $dir);
    } else {
        my $dir2 = dirname($dir);
        if(-d $dir2) {
            $selector->configure(-directory => $dir2);
        }
    }
    $$var = $selector->Show();
}

sub reset_window {
    my ($window, $curvars, $defvars, $optiondefaults) = @_;
    resethash($curvars, $defvars);
    foreach my $key (keys %$optiondefaults) {
        if($$optiondefaults{$key}) {
            $$optiondefaults{$key}->setOption($$curvars{$key});
        }
    }
}

sub close_after {
    my ($window, $onclose, @args) = @_;
    if(ref($onclose) eq "CODE") {
        &$onclose(@args);
    }
    $window->destroy;
}

sub done_window {
    my ($window, $message, $onclose, @args) = @_;
    my $done = $window->Toplevel();
    $done->title("Done!");
    my $label = $done->Message(-text => $message, 
                               -foreground => "blue",
                              );
    $label->grid();
    my $button = $done->Button(
                               -text=>"Close",
                               -command=> [\&close_after, $done, $onclose, @args],
                               -pady => 8,
                               -padx => 8,
                              );
    $button->grid();
}

sub error_window {
    my ($window, $message, $onclose, @args) = @_;
    my $done = $window->Toplevel();
    $done->title("ERROR!");
    my $label = $done->Message(-text => $message, 
                               -foreground => "red",
                              );
    $label->grid();
    my $button = $done->Button(
                               -text=>"Close",
                               -command=> [\&close_after, $done, $onclose, @args],
                               -pady => 8,
                               -padx => 8,
                              );
    $button->grid();
}

sub init_si_config {
    my $config = new AppConfig(
                               DEFAULT_IMAGE_DIR => { ARGCOUNT => 1},
                               AUTOINSTALL_SCRIPT_DIR => { ARGCOUNT => 1},
                               AUTOINSTALL_BOOT_DIR => { ARGCOUNT => 1},
                               RSYNCD_CONF => { ARGCOUNT => 1},
                               RSYNC_STUB_DIR => { ARGCOUNT => 1},
                               CONFIG_DIR => { ARGCOUNT => 1},
                               TFTP_DIR => { ARGCOUNT => 1},
                               NET_BOOT_DEFAULT => { ARGCOUNT => 1},
                               # now for tksis configuration parameters
                               ICON_DIR => { ARGCOUNT => 1, DEFAULT => "/usr/share/systeminstaller/images"},
                               XTERM_CMD => { ARGCOUNT => 1, 
                                              DEFAULT => "xterm -bg black -fg magenta",
                                            },
                              );
    $config->file("/etc/systemimager/systemimager.conf", "/etc/systeminstaller/tksis.conf");
    return $config;
}

#
#  resethash sets one hash to another hash.
#

sub resethash {
    my ($hash1, $hash2) = @_;
    foreach my $key (keys %$hash2) {
        $$hash1{$key} = $$hash2{$key};
    }
}

#
#  The following creates a set of widgets with do 'Label: [Entry]' 
#

sub label_entry_line {
    my ($window, $labeltext, $variable, $validate, @morewidgets) = @_;
    my @options;
    if($validate) {
        @options = (
                    -validatecommand => $validate,
                    -validate => "focusout",
                   );
    }
    my $label = $window->Label(-text => "$labeltext: ",
                               -anchor => "w");
    my $entry = $window->Entry(-textvariable => $variable, @options);
    $label->grid($entry,@morewidgets);
}

# This creates a small list box with 1 item ($selection) selected.
sub label_listbox_line {
    my ($window, $labeltext, $selection, $listitems , @morewidgets) = @_;
    my $label = $window->Label(-text => "$labeltext: ",
                               -anchor => "w");
    my $listbox = $window->Scrolled("Listbox",-scrollbars => 'e', -height => 2, 
                 -width => 17, -selectmode => "single", -exportselection=>0);
    $listbox->insert(0,$selection);
    $listbox->insert('end',@$listitems);
    $listbox->selectionSet(0);

    $label->grid($listbox,@morewidgets);
    return $listbox;
}

sub label_entry_file_line {
    
}

sub quit_button {
    my $window = shift;
    my $quit_button = $window->Button(
                                      -text=>"Close",
                                      -command=> [sub { shift->destroy }, $window],
                                      -pady => 8,
                                      -padx => 8,
                                     );
    return $quit_button;
}

sub label_option_line {
    my ($window, $labeltext, $variable, $options, @morewidgets) = @_;
    my $label = $window->Label(-text => "$labeltext: ",
                               -anchor => "w");

    my $default = $$variable;
    my $optionmenu = $window->Optionmenu(-options => $options,
                                      -variable => $variable);
    $optionmenu->setOption($default) if $default;

    $label->grid($optionmenu, @morewidgets, -sticky => "nesw");
    return $optionmenu;
}

sub imageexists {
    my ($rsyncconf, $imagename) = @_;
    open(IN,"<$rsyncconf") or return undef;
    if(grep(/\[$imagename\]/, <IN>)) {
        close(IN);
        return 1;
    }
    close(IN);
    return undef;
}


1;
