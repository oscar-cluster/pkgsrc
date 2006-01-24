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
use SystemImager::Server;
use SystemImager::Common;
use SystemImager::Config;

use strict;

@EXPORT = qw(createimage_window add2rsyncd delfromrsyncd);

sub createimage_window {
	my $config = init_si_config();

	my $window = shift;
	my %vars = (
		title => "Create an SIS Image",
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

	#
	# Validate image name.
	#
	my @images = listimages("/etc/systemimager/rsyncd.conf");
	if( grep {$vars{imgname} eq $_} @images ) {
		my $last = 0;
		foreach (@images) {
			if( /^\Q$vars{imgname}\E(\d+)$/ ) {
				$last = $1 if $1 > $last;
			}
		}
		$vars{imgname} .= $last + 1;
	}

	my %defaults = %vars;
	my %noshow = %{$vars{noshow}};

	$noshow{vdiskdev}="yes" unless -d '/proc/iSeries';

	my $image_window = $window->Toplevel();
	$image_window->withdraw;
	$image_window->title($vars{title});
	my $message = $image_window->Message(-text => "Fill out the following fields to build a System Installation Suite image.  If you need help on any field, click the help button next to it", -justify => "left", -aspect => 700);
	$message->grid("-","-","-");

	#
	#  First line:  What is you image name?
	# 

	label_entry_line($image_window, "Image Name", \$vars{imgname},"","x",helpbutton($image_window, "Image Name"))
		unless $noshow{pkgpath};
	
	#
	#  Second line: Where is your package file
	#
	my $package_button = $image_window->Button(
		-text=>"Choose a File...",
		-command=> [\&selector2entry, \$vars{pkgfile}, "Select RPM list", [["RPM list", ".rpmlist"],["All files", "*"]], $image_window],
		-pady => 4,
		-padx => 4,
	);
	label_entry_line($image_window, "Package File", \$vars{pkgfile}, "", 
		$package_button, helpbutton($image_window, "Package File"))
		unless $noshow{pkgfile};

	#
	#  Third Line:  where are your packages?
	#
	label_entry_line($image_window, "Packages Directory", \$vars{pkgpath},"","x",
		helpbutton($image_window, "Package Directory"))
		unless $noshow{pkgpath};

	
	my $disk_button = $image_window->Button(
		-text=>"Choose a File...",
		-command=> [\&selector2entry, \$vars{diskfile}, "Select disk configuration", [["Disk configuration", ".disk"],["All files", "*"]], $image_window],
		-pady => 4,
		-padx => 4,
	);

	#
	# (only for iseries).  Virtual disk enable.
	# 

	label_entry_line($image_window, "Virtual Disk", \$vars{vdiskdev},"","x",helpbutton($image_window, "Virtual Disk"))
		unless $noshow{vdiskdev};

	#
	# Disk partition file
	#
	
	label_entry_line($image_window, "Disk Partition File", \$vars{diskfile}, "", 
		$disk_button, helpbutton($image_window, "Disk File"))
		unless $noshow{diskfile};

	# 
	# Set root password
	#
	
	my $passlabel=$image_window->Label(-text=>"Root password(confirm):", -anchor=>"w");
	my $pass = $image_window->Entry(-textvariable=>\$vars{pass1}, -show=>"*");
	my $passconfirm = $image_window->Entry(-textvariable=>\$vars{pass2}, -show=>"*", -width=>14);
	$passlabel->grid($pass,$passconfirm,helpbutton($image_window, "Root password"))
		unless $noshow{password};

	#
	#  What is the architecture?
	#
	
	my @archoptions = qw( i386 i486 i586 i686 ia64 ppc x86_64 athlon amd64 );

	my $archoption = label_option_line($image_window, "Target Architecture",
		\$vars{arch},\@archoptions, "x",
		helpbutton($image_window,"Target Architecture"))
		unless $noshow{arch};

	#
	#  Fourth Line: what is your ip assignment method?
	#

	my @ipoptions = qw( dhcp replicant static );

	my $ipoption = label_option_line($image_window, "IP Assignment Method",
		\$vars{ipmeth},\@ipoptions, "x",
		helpbutton($image_window, "IP Method"))
		unless $noshow{ipmeth};

	#
	#  Fifth Line: enable multicasting? Yes or No.
	#

	my @multicastOpts = qw(on off);
	my $multicastOpts= label_option_line($image_window, "Multicasting",
		\$vars{mcast},\@multicastOpts, "x",
		helpbutton($image_window, "Multicast"))
		unless $noshow{mcast};

	#
	#  Sixth Line: what is the post install action?
	#

	my @postinstall = qw(beep reboot shutdown);

	my $postoption = label_option_line($image_window, "Post Install Action",
		\$vars{piaction},\@postinstall, "x",
		helpbutton($image_window, "Post Install Action"))
		unless $noshow{piaction};

	# Then a whole bunch of control buttons
	
	my $activate_button = $image_window->Button(
		-text => "Build Image",
		-command => [\&add_image, \%vars, $image_window],
		-pady => 8,
		-padx => 8,
	);
	
	my $reset_button = $image_window->Button(
		-text=>"Reset",
		-command=> [
			\&reset_window, $image_window, \%vars, \%defaults,
			{
				piaction => $postoption,
				arch => $archoption,
				ipmeth => $ipoption,
				mcast => $multicastOpts
			},
		],
		-pady => 8,
		-padx => 8,
	);

	$reset_button->grid($activate_button, quit_button($image_window),"-" , -sticky => "nesw");
	
	# key bindings
	$image_window->bind("<Control-q>",sub {$image_window->destroy});
	$image_window->bind("<Control-r>",sub {$reset_button->invoke});
	
	center_window( $image_window );
}

our $progress_status;
our $progress_window;
our $progress_widget;
sub progress_bar {
	my ($window, $title) = @_;

	our $progress_window = $window->Toplevel();
	$progress_window->withdraw;
	$progress_window->title($title);

	$progress_window->Label(
		-text => "Progress to completion:",
		-anchor => "w",
	)->pack(
		-fill => 'x',
		-padx => 4,
		-pady => 4,
	);

	our $progress_widget = $progress_window->ProgressBar(
		-takefocus => 0,
		-width => 20,
		-length => 400,
		-anchor => 'w',
		-from => 0,
		-to => 100,
		-blocks => 20,
		-colors => [0, 'green'], # [0, 'green', 50, 'yellow' , 80, 'red'],
	);
	$progress_widget->pack(
		-fill => 'x',
		-padx => 4,
	);

	$progress_window->Button(
		-text => "Cancel",
		-command => [ \&progress_cancel ],
		-padx => 8,
		-pady => 8,
	)->pack(
		-pady => 4,
	);
	our $progress_status = 1;
	center_window( $progress_window );

}

sub progress_cancel {
	our $progress_status = 0;
}

sub progress_destroy {
	our $progress_window->destroy();
}

sub progress_update {
	my $value = shift;
	our $progress_widget->value( $value );
	$progress_widget->update();
}

sub progress_continue {
	our $progress_status;
	return $progress_status;
}

sub reset_window {
	my ($window, $curvars, $defvars, $optiondefaults) = @_;
	resethash($curvars, $defvars);
	foreach my $key (keys %$optiondefaults) {
		$$optiondefaults{$key}->setOption($$curvars{$key}) if($$optiondefaults{$key} && $$curvars{$key});
	}
}

sub listimages {
	my @list;
	if( open IN, "mksiimage --list |" ) {
		while (<IN>) {
			next if $. <= 2;
			chomp;
			my @items = split;
			push @list, $items[1] if $items[1];
		}
		close IN;
	}
	return @list;
}

sub add_image {
	my $vars = shift;
	my $window = shift;


	my $config = init_si_config();
	my $rsyncd_conf = $config->rsyncd_conf();
	my $rsync_stub_dir = $config->rsync_stub_dir();

	$window->Busy(-recurse => 1);

	if( imageexists("/etc/systemimager/rsyncd.conf", $$vars{imgname}) ) {
		unless( yn_window( $window, "\"$$vars{imgname}\" exists.\nDo you want to replace it?" ) ) {
			$window->Unbusy();
			return undef;
		}
		#
		# This should work, but it's not trustworthy.
		#
		system("mksiimage -D --name $$vars{imgname}");
		#
		# Belt and suspenders for above.
		#
		SystemImager::Server->remove_image_stub($rsync_stub_dir, $$vars{imgname});
		SystemImager::Server->gen_rsyncd_conf($rsync_stub_dir, $rsyncd_conf);
		$window->update();
	}

	if ($$vars{pass1} ne $$vars{pass2}) {
		error_window($window, "The root passwords specified do not match");
		$window->Unbusy();
		return undef;
	}

	progress_bar( $window, "Building Image..." );
	my $result = add_image_build( $vars, $window );
	progress_destroy();
	if( $result ) {
		done_window($window, "Successfully created image \"$$vars{imgname}\"");
		if( $$vars{imgname} =~ /(.*?)(\d+)$/ ) {
			$$vars{imgname} = $1.($2 + 1);
		} else {
			$$vars{imgname} .= 1;
		}
	} else {
		if( progress_continue() ) {
			error_window($window, "Failed building image \"$$vars{imgname}\"");
		} else {
			error_window($window, "User cancelled building image \"$$vars{imgname}\"");
		}
		#
		# This should work, but it's not trustworthy.
		#
		system("mksiimage -D --name $$vars{imgname}");
		#
		# Belt and suspenders for above.
		#
		SystemImager::Server->remove_image_stub($rsync_stub_dir, $$vars{imgname});
		SystemImager::Server->gen_rsyncd_conf($rsync_stub_dir, $rsyncd_conf);
	}

	$window->Unbusy();
	return $result;
}

sub add_image_build {
	my $vars = shift;
	my $window = shift;

	my $cmd = "mksiimage -A --name $$vars{imgname} --location $$vars{pkgpath} --filename $$vars{pkgfile} --arch $$vars{arch} --path $$vars{imgpath}/$$vars{imgname} --verbose $$vars{extraflags}";
	

	my $totallines = 0;
	my $lines_so_far = 0;
	$SIG{PIPE} = 'IGNORE';
	my $pid = open( OUTPUT, "$cmd |" );
	unless( $pid ) {
		carp("Couldn't run command $cmd");
		return 0;
	}

	progress_update( 0 );
	while( <OUTPUT>) {
		unless( progress_continue() ) {
			kill( "TERM", $pid );
			last;
		}
		$totallines = $1 if(/Expected lines of output: (\d+)/);
		if($totallines) {
			$lines_so_far++;
			my $value = 90 * $lines_so_far / $totallines;
			$value = 90 if $value > 90;
			progress_update($value);
			print "$lines_so_far: $_";
		}
	}
	close(OUTPUT);
	return 0 unless progress_continue();

	progress_update(90);

	print "Built image from rpms\n";

	# Now set the root password if given
	return 0 unless progress_continue();
	if ($$vars{pass1}) {
		update_user(
			imagepath => $$vars{imgpath}."/".$$vars{imgname},
			user => 'root',
			password => $$vars{pass1}
		);
	}
	return 0 unless progress_continue();
	progress_update(91);

	#
	# Update flamethrower.conf
	#
	return 0 unless progress_continue();
	if ($$vars{mcast} eq "on") {

		# Backup original flamethrower.conf
		$cmd = "/bin/mv -f /etc/systemimager/flamethrower.conf /etc/systemimager/flamethrower.conf.bak";
		if( system( $cmd ) ) {
			carp("Couldn't run command $cmd");
			return 0;
		}
		return 0 unless progress_continue();
		progress_update(92);

		$cmd = "sed -e 's/START_FLAMETHROWER_DAEMON = no/START_FLAMETHROWER_DAEMON = yes/' /etc/systemimager/flamethrower.conf.bak > /etc/systemimager/flamethrower.conf";
		if( system( $cmd ) ) {
			carp("Error encountered while changing START_FLAMETHROWER_DAEMON = no to yes in /etc/systemimager/flamethrower.conf");
			return 0;
		}
		return 0 unless progress_continue();
		progress_update(93);

		# add entry for boot-i386-standard module
		my $march = $$vars{arch};
		$march =~ s/i.86/i386/;
		$cmd = "/usr/lib/systemimager/perl/confedit --file /etc/systemimager/flamethrower.conf --entry boot-$march-standard --data \" DIR=/usr/share/systemimager/boot/i386/standard/\"";
		if( system( $cmd ) ) {
			carp("Couldn't run command $cmd");
			return 0;
		}
		return 0 unless progress_continue();
		progress_update(95);
		print "Updated flamethrower.conf\n";

		$cmd = "/etc/init.d/systemimager-server-flamethrowerd restart";
		if( system( $cmd ) ) {
			carp("Couldn't start flamethrower");
			return 0;
		}
	}
	return 0 unless progress_continue();
	progress_update(96);

	$cmd = "mksidisk -A --name $$vars{imgname} --file $$vars{diskfile}";
	if( system($cmd) ) {
		carp("Couldn't run command $cmd");
		return 0;
	}
	return 0 unless progress_continue();
	progress_update(97);

	print "Added Disk Table for $$vars{imgname} based on $$vars{diskfile}\n";
	
	if ( $$vars{vdiskdev} =~ (/\/dev\/[a-zA-Z]*/) ) {
		$cmd = $main::config->mkaiscript . " -quiet -image $$vars{imgname} -force -ip-assignment $$vars{ipmeth} -post-install $$vars{piaction} -iseries-vdisk=$$vars{vdiskdev}" ;
	} else {
		$cmd = $main::config->mkaiscript . " -quiet -image $$vars{imgname} -force -ip-assignment $$vars{ipmeth} -post-install $$vars{piaction}"; 
	}
	if( system($cmd) ) {
		carp("Couldn't run $cmd");
		return 0;
	}
	return 0 unless progress_continue();
	progress_update(98);

	print "Ran mkautoinstallscript\n";

	# This allows for an arbitrary callback to be registered.
	# It will get a reference to all the variables that have been defined for the image

	if(ref($$vars{postinstall}) eq "CODE") {
		unless( &{$$vars{postinstall}}($vars) ) {
			carp("Couldn't run postinstall"), 
			return 0;
		}
	}
	return 0 unless progress_continue();
	progress_update(99);
	if(ref($$vars{postinstall}) eq "ARRAY") {
		my $sub = shift(@{$$vars{postinstall}});
		unless( &$sub($vars, @{$$vars{postinstall}}) ) {
			carp("Couldn't run postinstall");
			return 0;
		}
	}
	return 0 unless progress_continue();
	progress_update(100);
	return 1;
}

sub delfromrsyncd {
	my ($rsyncconf, $imagename) = @_;
	
	return 1 unless imageexists($rsyncconf, $imagename);

	open(IN,"<$rsyncconf") or return undef;
	my @lines = <IN>;
	close IN;

	return undef unless open(OUT,">$rsyncconf");
	my $state = 1;
	foreach (@lines) {
		if(/^\[$imagename\]/) {
			$state = 0;
		} elsif (/^\[/) {
			$state = 1;
		}
		print OUT $_ if $state;
	}
	close(OUT);
	return 1;
}

sub add2rsyncd {
	my ($rsyncconf, $imagename, $imagedir) = @_;
	
	unless(imageexists($rsyncconf, $imagename)) {
		open(OUT,">>$rsyncconf") or return undef;
		print OUT "[$imagename]\n\tpath=$imagedir/$imagename\n\n";
		close OUT;
	}
	return 1;
}

1;
