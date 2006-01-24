	package SystemInstaller::Tk::Image;

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
use vars qw(@EXPORT);
use Data::Dumper;
use AppConfig;
use File::Copy;
use POSIX;
use Tk;
use Tk::ProgressBar;
use Tk::FileSelect;
use SystemInstaller::Tk::Common;
use SystemInstaller::Tk::Help;
use SystemInstaller::Passwd qw(update_user);
use Carp;
use strict;

@EXPORT = qw(createimage_window add2rsyncd delfromrsyncd);

sub createimage_window {
    my $config = init_si_config();

    my $window = shift;
    my %vars = (
                imgpath => $config->default_image_dir,
                imgname => "",
                arch => (uname)[4],
                pkgfile => "",
                pkgpath => "",
                ipmeth => "",
                mcast => "",
                piaction => "",
                diskfile => "",
                vdiskdev => "none",
                extraflags => "",
                pass1 => "",
                pass2 => "",
                # This is the dummy post install.  Postinstalls MUST return true lest things go funky.
                postinstall => sub {return 1},
                noshow => {},
                @_,
               );

    my %defaults = %vars;
    my %noshow = %{$vars{noshow}};
   
    if ( ! -d '/proc/iSeries' ) { 
	$noshow{vdiskdev}="yes";
    }

    my $image_window = $window->Toplevel();
    $image_window->title("Create a System Installation Suite Image");
    my $message = $image_window->Message(-text => "Fill out the following fields to build a System Installation Suite image.  If you need help on any field, click the help button next to it", -justify => "left", -aspect => 700);
    $message->grid("-","-","-");

    #
    #  First line:  What is you image name?
    # 

    label_entry_line($image_window, "Image Name", \$vars{imgname},"","x",helpbutton($image_window, "Image Name")) unless $noshow{pkgpath};
    
    #my $imgoption = label_option_line($image_window, "Image Name",
    #                  \$vars{imgname},["",listimages($config->rsyncd_conf)]) unless $noshow{imgname};

    #
    #  Second line: Where is your package file
    #

    my $package_selector = $image_window->FileSelect(-directory => "/tftpboot");
    my $package_button = $image_window->Button(
                                               -text=>"Choose a File...",
                                               -command=> [\&selector2entry, \$vars{pkgfile}, $package_selector],
                                               -pady => 4,
                                               -padx => 4,
                                        );
    label_entry_line($image_window, "Package File", \$vars{pkgfile}, "", 
                     $package_button, helpbutton($image_window, "Package File")) unless $noshow{pkgfile};

    #
    #  Third Line:  where are your packages?
    #
    
    label_entry_line($image_window, "Packages Directory", \$vars{pkgpath},"","x",
                     helpbutton($image_window, "Package Directory")) unless $noshow{pkgpath};

    
    my $disk_selector = $image_window->FileSelect(-directory => "/tftpboot");
    my $disk_button = $image_window->Button(
                                               -text=>"Choose a File...",
                                               -command=> [\&selector2entry, \$vars{diskfile}, $disk_selector],
                                               -pady => 4,
                                               -padx => 4,
                                        );

   #
   # (only for iseries).  Virtual disk enable.
   # 
   
    label_entry_line($image_window, "Virtual Disk", \$vars{vdiskdev},"","x",helpbutton($image_window, "Virtual Disk")) unless $noshow{vdiskdev};

    #
    # Disk partition file
    #
    
    label_entry_line($image_window, "Disk Partition File", \$vars{diskfile}, "", 
                     $disk_button, helpbutton($image_window, "Disk File")) unless $noshow{diskfile};

    # 
    # Set root password
    #
    
    my $passlabel=$image_window->Label(-text=>"Root password(confirm):", -anchor=>"w");
    my $pass = $image_window->Entry(-textvariable=>\$vars{pass1}, -show=>"*");
    my $passconfirm = $image_window->Entry(-textvariable=>\$vars{pass2}, -show=>"*", -width=>14);
    $passlabel->grid($pass,$passconfirm,helpbutton($image_window, "Root password")) unless $noshow{password};

    #
    #  What is the architecture?
    #
    
    my @archoptions = qw( i386 i486 i586 i686 ia64 ppc );

    my $archoption = label_option_line($image_window, "Target Architecture",
                                       \$vars{arch},\@archoptions, "x",
                                       helpbutton($image_window,"Target Architecture")) unless $noshow{arch};

    #
    #  Fourth Line: what is your ip assignment method?
    #

    my @ipoptions = qw( dhcp replicant static );

    my $ipoption = label_option_line($image_window, "IP Assignment Method",
                                     \$vars{ipmeth},\@ipoptions, "x",
                                     helpbutton($image_window, "IP Method")) unless $noshow{ipmeth};

    #
    #  Fifth Line: enable multicasting? Yes or No.
    #

    my @multicastOpts = qw(on off);
    my $multicastOpts= label_option_line($image_window, "Multicasting",
                                     \$vars{mcast},\@multicastOpts, "x",
                                     helpbutton($image_window, "Multicast")) unless $noshow{mcast};

    #
    #  Sixth Line: what is the post install action?
    #

    my @postinstall = qw(beep reboot shutdown);

    my $postoption = label_option_line($image_window, "Post Install Action",
                                       \$vars{piaction},\@postinstall, "x",
                                      helpbutton($image_window, "Post Install Action")) unless $noshow{piaction};
   
    # Then a whole bunch of control buttons
    
    my $reset_button = $image_window->Button(
                                             -text=>"Reset",
                                             -command=> [\&reset_window, $image_window, 
                                                         \%vars, \%defaults, {piaction => $postoption,
                                                                              arch => $archoption,
                                                                              ipmeth => $ipoption,
                                                                              mcast => $multicastOpts
                                                                             },
                                                        ],
                                            -pady => 8,
                                            -padx => 8,
                                           );

    my $progress = $image_window->ProgressBar(
                                              -takefocus => 0,-width => 20,-length => 200,
                                              -anchor => 'w',-from => 0,-to => 100,
                                              -blocks => 500,-gap => 0,
                                              -colors => [0, 'red'], # [0, 'green', 50, 'yellow' , 80, 'red'],
                                              -variable => \$vars{percent_done}
                                             );
    

    my $activate_button = $image_window->Button(
                                                -text => "Build Image",
                                                -command => [\&add_image, \%vars, $image_window, $progress],
                                                -pady => 8,
                                                -padx => 8,
                                               );
    
    $reset_button->grid($activate_button, quit_button($image_window),"-" , -sticky => "nesw");
    
    $progress->grid("-","-", -sticky => "nesw");

    # key bindings
    $image_window->bind("<Control-q>",sub {$image_window->destroy});
    $image_window->bind("<Control-r>",sub {$reset_button->invoke});
    
}

sub reset_window {
    my ($window, $curvars, $defvars, $optiondefaults) = @_;
    resethash($curvars, $defvars);
    foreach my $key (keys %$optiondefaults) {
        if($$optiondefaults{$key} and $$curvars{$key}) {
            $$optiondefaults{$key}->setOption($$curvars{$key});
        }
    }
}

sub del_image {
    my $vars = shift;
    my $window = shift;
    my $option = shift;
    !system("mksiimage -D --name $$vars{imgname}") or return undef;
    delfromrsyncd("/etc/systemimager/rsyncd.conf", $$vars{imgname});
    $option->options(["",listimages("/etc/systemimager/rsyncd.conf")]);
    $option->setOption("");
    $option->update();
    done_window($window,"Successfully deleted image $$vars{imgname}");
    return 1;
}

sub add_image {
    my $vars = shift;
    my $window = shift;
    my $progress = shift;
    $window->Busy(-recurse => 1);
    if ($$vars{pass1} ne $$vars{pass2}) {
            error_window($window, "The root passwords specified do not match");
            $window->Unbusy();
             return undef;
    }


    my $cmd = "mksiimage -A --name $$vars{imgname} --location $$vars{pkgpath} --filename $$vars{pkgfile} --arch $$vars{arch} --path $$vars{imgpath}/$$vars{imgname} --verbose $$vars{extraflags}";
    
    my $totallines = 0;
    open(OUTPUT,"$cmd |") or (carp("Couldn't run command $cmd"), 
                              $window->Unbusy(), return undef);

    my $lines_so_far;
    while(<OUTPUT>) {
        if(/Expected lines of output: (\d+)/) {
            $totallines = $1;
        }
        if($totallines) {
            $lines_so_far++;
            $progress->value(100 * $lines_so_far / $totallines);
            $progress->update();
            print "$lines_so_far: $_";
        }
    }

    close(OUTPUT) or (carp("Command $cmd failed to run properly"), 
                      error_window($window, "Failed to create Image!"),
                      $progress->value(0),
                      $progress->update(),
                      $window->Unbusy(),
                      return undef);

    print "Built image from rpms\n";

    # Now set the root password if given
    if ($$vars{pass1}) {
            update_user(
                imagepath => $$vars{imgpath}."/".$$vars{imgname},
                user => 'root',
                password => $$vars{pass1}
            );
    }

    ##############################################
    # Update flamethrower.conf                   #
    ##############################################
    if ($$vars{mcast} eq "on") {

       # Backup original flamethrower.conf
       $cmd = "/bin/mv -f /etc/systemimager/flamethrower.conf /etc/systemimager/flamethrower.conf.bak";
       open(OUTPUT,"$cmd |") or (carp("Couldn't run command $cmd"), 
               $window->Unbusy(), return undef);

       !system("sed -e 's/START_FLAMETHROWER_DAEMON = no/START_FLAMETHROWER_DAEMON = yes/' /etc/systemimager/flamethrower.conf.bak > /etc/systemimager/flamethrower.conf") or carp("Error encountered while changing START_FLAMETHROWER_DAEMON = no to yes in /etc/systemimager/flamethrower.conf");

       # add entry in flamethrower for the image
       my $entryArg = "[$$vars{imgname}]";
       $cmd = "/usr/lib/systemimager/perl/confedit --file /etc/systemimager/flamethrower.conf --entry $entryArg --data \"$entryArg \n DIR=/var/lib/systemimager/scripts/\"";
       open(OUTPUT,"$cmd |") or (carp("Couldn't run command $cmd"), 
               $window->Unbusy(), return undef);

       # add entry for boot-i386-standard module
       if ($$vars{arch} eq "i686" or $$vars{arch} eq "i586"  or $$vars{arch} eq "i486" or $$vars{arch} eq "i386"){
           $entryArg = "[boot-i386-standard]";
       }
       else{
           $entryArg = "[boot-$$vars{arch}-standard]";
       }
       $cmd = "/usr/lib/systemimager/perl/confedit --file /etc/systemimager/flamethrower.conf --entry $entryArg --data \"$entryArg \n DIR=/usr/share/systemimager/boot/i386/standard/\"";
       open(OUTPUT,"$cmd |") or (carp("Couldn't run command $cmd"), 
               $window->Unbusy(), return undef);
       print "Updated flamethrower.conf\n";

       !system("/etc/init.d/systemimager-server-flamethrowerd restart") or carp("Couldn't start flamethrower");
    }

    my $diskcmd = "mksidisk -A --name $$vars{imgname} --file $$vars{diskfile}";
    
    !system($diskcmd) or (carp("Couldn't run command $diskcmd"),
                          error_window($window, "Failed to set disk partitioning in image!"),
                          $window->Unbusy(),
                          return undef);

        print "Added Disk Table for $$vars{imgname} based on $$vars{diskfile}\n";
    
        my $mkaiscmd;
        if ( $$vars{vdiskdev} =~ (/\/dev\/[a-zA-Z]*/) ) {
                $mkaiscmd = "mkautoinstallscript -quiet -image $$vars{imgname} -force -ip-assignment $$vars{ipmeth} -post-install $$vars{piaction} -iseries-vdisk=$$vars{vdiskdev}" ;
        } else {
                $mkaiscmd = "mkautoinstallscript -quiet -image $$vars{imgname} -force -ip-assignment $$vars{ipmeth} -post-install $$vars{piaction}"; 
        }

        !system($mkaiscmd) or (carp("Couldn't run $mkaiscmd"), 
                error_window($window, "Failed to build auto install script for image!"),
                $window->Unbusy(), 
                return undef);

    print "Ran mkautoinstallscript\n";

    # This allows for an arbitrary callback to be registered.
    # It will get a reference to all the variables that have been defined for the image

    if(ref($$vars{postinstall}) eq "CODE") {
        &{$$vars{postinstall}}($vars) or (carp("Couldn't run postinstall"), 
                                          error_window($window,"There was an error running the post image building script, please check your logs for more info"), 
                                          $window->Unbusy(),
                                          return 0);
    }
    if(ref($$vars{postinstall}) eq "ARRAY") {
        my $sub = shift(@{$$vars{postinstall}});
        &$sub($vars, @{$$vars{postinstall}}) or (carp("Couldn't run postinstall"), 
                                                 error_window($window,"There was an error running the post image building script, please check your logs for more info"), 
                                                 $window->Unbusy(), 
                                                 return 0);
    }

    done_window($window, "Successfully created image $$vars{imgname}", sub {$a = shift; $a->value(0); $a->update();}, $progress);
    $window->Unbusy();
    return 1;
}

sub delfromrsyncd {
    my ($rsyncconf, $imagename) = @_;
    
    if(!imageexists($rsyncconf, $imagename)) {
        return 1;
    }
    copy($rsyncconf, "$rsyncconf.tksisbak") or return undef;
    open(IN,"<$rsyncconf.tksisbak") or return undef;
    open(OUT,">$rsyncconf") or return undef;
    my $state = 1;
    while(<IN>) {
        if(/^\[$imagename\]/) {
            $state = 0;
        } elsif (/^\[/) {
            $state = 1;
        }
        print OUT $_ if $state;
    }
    close(IN);
    close(OUT);
    return 1;
}

sub add2rsyncd {
    my ($rsyncconf, $imagename, $imagedir) = @_;
    
    if(!imageexists($rsyncconf, $imagename)) {
        open(OUT,">>$rsyncconf") or return undef;
        print OUT "[$imagename]\n\tpath=$imagedir/$imagename\n\n";
        close OUT;
        return 1;
    }
    return 1;
}

1;
