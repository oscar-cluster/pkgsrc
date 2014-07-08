package netbootlib;

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
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# Copyright (c) Erich Focht <efocht@hpce.nec.com>
#               All rights reserved
#
# $Id: netbootlib.pm 6252 2007-04-13 12:06:10Z focht $

use strict;
use Exporter;
our (@ISA,@EXPORT,$VERSION);
$VERSION = 1.0;
@ISA = ("Exporter");
@EXPORT = qw(
	     readConfig
	     setAction
	     getHostDB
	     ip_to_hex
	     conf_filename
	     read_host_action
	     read_host_status
	     get_action
	     %config
	     %hosts
	     $verbose
	      );

our %config;
our %hosts;
our $verbose;

$config{tftpdir}   = "/tftpboot";
$config{hostdb}    = "SIS";
$config{refresh}   = 10;
$config{statuscmd} = "";

my $nbaconfig = "/etc/netbootmgr.conf";


sub readConfig {
    local *IN;
    open IN, "< $nbaconfig" or die "Could not open $nbaconfig";
    while (<IN>) {
	next if /^#/;
	chomp;
	s/^\s*//;
	my ($cmd, $rest) = split;
	if ($cmd eq "REFRESH") {
	    if (($rest < 10000) && ($rest > 0)) {
		$config{refresh} = $rest;
	    } else {
		print "Refresh value in config file outside valid range (1..10000).\n";
		print "Ignoring, refresh value is $config{refresh} seconds\n";
	    }
	} elsif ($cmd eq "STATUS") {
	    if (-x $rest) {
		$config{statuscmd} = $rest;
	    }
	} elsif ($cmd eq "TFTPROOT") {
	    if ((-d $rest) && ($rest =~ m:^/: )) {
		$config{tftpdir} = $rest;
	    } else {
		print "TFTPROOT argument $rest is wrong. Use a directory and absolute path!";
		print "Using default value: $config{tftpdir}\n";
	    }
	} elsif ($cmd eq "HOSTDB") {
	    if ($rest ne "SIS") {
		if (-f $rest) {
		    $config{hostdb} = $rest;
		} else {
		    print "HOSTDB should be either SIS or a file.\n";
		}
	    } else {
		$config{hostdb} = "SIS";
	    }
	} elsif ($cmd eq "MENU") {
	    
	    my ($label, $link) = split /:/, $rest;
	    if (-e "$config{tftpdir}/$link" || ($link =~ /^__default__$/ )) {
		$config{nba}{$label} = $link;
		if ($link =~ /^__default__$/) {
		    $config{nba}{default_action} = $label;
		}
	    } else {
		print "File $link not found! Ignoring $label.\n";
	    }
	}
    }
    close IN;
}


sub setAction {
    my ($action, $selp) = @_;
    my $link = $config{nba}{$action};

    for my $h (@{$selp}) {
	my $file = $hosts{$h}->{file};
	if (-l $file || -f $file) {
	    print STDERR "Deleting $file\n" if $verbose;
	    unlink $file;
	    if ($?) {
		print STDERR "Error returned while trying to unlink $file";
		next;
	    }
	} 
	if (! ($link =~ /^__default__$/ )) {
	    if ($hosts{$h}->{arch} eq "ia64") {
		print STDERR "Creating symlink $file -> $link\n" if $verbose;
		symlink $link, $file;
	    } else {
		print STDERR "Creating symlink $file -> ../$link\n" if $verbose;
		symlink "../$link", $file;
	    }
	    if (!$?) {
		$hosts{$h}->{nba} = $action;
	    } else {
		print STDERR "Some error occured while trying to set symbolic link for $file\n";
		$hosts{$h}->{nba} = "unknown";
	    }
	} else {
	    $hosts{$h}->{nba} = $action;
	}
    }
}


sub getHostDB {
    my $hostdb = $config{hostdb};
    if ($hostdb eq "SIS") {
        # read host data from SIS database
	eval 'use SIS::Client';
	eval 'use SIS::Adapter';
	eval 'use SIS::Image';
	eval 'use SIS::DB';
	for my $mach (list_client()) {
	    my @adap = list_adapter(devname=>"eth0",client=>$mach->name);
	    next if (!scalar(@adap));
	    my $ip = $adap[0]->ip; 
	    my @img = list_image(name=>$mach->imagename);
	    next if (!scalar(@img));
	    my $arch = $img[0]->arch;
	    $hosts{$mach->name} = {
		"arch" => $arch,
		"ip" => $ip,
		"file" => conf_filename($ip, $arch),
		"nba" => "unknown",
		"stat" => "-",
		"power" => "",
	    };
	}
    } else {
            # for testing purposes: read from file
	open IN, "< $hostdb" or die "Could not open $hostdb";
	while (<IN>) {
	    chomp;
	    my ($host, $arch, $ip) = split /\s+/;
	    $hosts{$host} = {
		"arch" => $arch,
		"ip" => $ip,
		"file" => conf_filename($ip, $arch),
		"nba" => "unknown",
		"stat" => "-",
		"power" => "",
	    };
	}
	close IN;
    }
}

sub ip_to_hex {
    my ($ip) = @_;
    my @hex = split /\./, $ip;
    return sprintf("%2.2X%2.2X%2.2X%2.2X",@hex);
}
	
sub conf_filename {
    my ($ip, $arch) = (@_);
    my $hex = ip_to_hex($ip);
    my $file;
    if ($arch eq "ia64") {
	$file = $config{tftpdir} . "/" . $hex . ".conf";
    } else {
	$file = $config{tftpdir} . "/" . "pxelinux.cfg/" . $hex;
    }
    return $file;
}

#
# Read host next boot action and return an array of hosts for which
# this has changed.
#
sub read_host_action {
    my @changed;
    for my $h (keys %hosts) {
	my $file = $hosts{$h}->{file};
	my $action = "unknown";
	if (-l $file) {
	    $action = get_action(readlink($file), 1);
	} else {
	    if (-e $file) {
		print STDERR "File $file should be a symbolic link!" if $verbose;
		# find if file is equal to any of the nba files and replace with link
		$action = get_action($file, 0);
		if ($action ne "unknown") {
		    if ($verbose) {
			print STDERR "$file is not a symbolic link but equal to one of the NBAs.\n";
			print STDERR "Relinking...\n";
		    }
		    unlink $file."__new" if (-e $file."__new");
		    if ($hosts{$h}->{arch} eq "ia64") {
			symlink $config{nba}->{$action}, $file."__new";
		    } else {
			symlink "../".$config{nba}->{$action}, $file."__new";
		    }
		    rename $file."__new", $file;
		}
	    } else {
                    # this is the __default__ action
		$action = $config{nba}{default_action};
	    }
	}
	if ($action ne $hosts{$h}->{nba}) {
	    $hosts{$h}->{nba} = $action;
	    push @changed, $h;
	}
    }
    return @changed;
}

#
# Invoke the STATUS command for all nodes and return an array of those
# which changed.
#
sub read_host_status {
    my @changed;
    local *ST;
    my $cmd = $config{statuscmd};
    return () if (!$cmd);

    $cmd .= " ".join(" ",sort keys %hosts);
    open ST, "$cmd |" or die "Could not run $cmd\n$!";
    while (<ST>) {
	chomp;
	if (/^(\S+): (.*)$/) {
	    my $h = $1;
	    my $s = $2;
	    next if ! exists $hosts{$h}->{stat};
	    if ($s ne $hosts{$h}->{stat}) {
		push @changed, $h;
		$hosts{$h}->{stat} = $s;
	    }
	}
    }
    close ST;
    return @changed;
}

#
# Invoke a cpower command for nodes passed as argument,
# interpret returned lines as status and return an array
# of nodes which changed the power status.
#
sub apply_cpower_cmd {
    my ($cmd, @nodes) = @_;
    my @changed;
    local *ST;
    $cmd .= " ".join(" ", sort @nodes);
    open ST, "$cmd |" or die "Could not run $cmd\n$!";
    while (<ST>) {
	chomp;
	if (/^(\S+) : (.*)$/) {
	    my $h = $1;
	    my $s = $2;
	    next if ! exists $hosts{$h}->{power};
	    if ($s ne $hosts{$h}->{power}) {
		push @changed, $h;
		$hosts{$h}->{power} = $s;
	    }
	} elsif(/^(\S+) :\s*$/) {
	    my $h = $1;
	    push @changed, $h;
	    $hosts{$h}->{power} = "";
	}
    }
    close ST;
    return @changed;
}

#
# Read next boot action (NBA) by looking at the "file" passed.
# Replace on-the-fly PXE config files with symlinks if they are identical
# with known action files.
#
sub get_action {
    my ($file, $islink) = @_;
    my %nba = %{$config{nba}};
    my $tftpdir = $config{tftpdir};
    $file =~ s:^\.\./::;
    for my $action (keys %nba) {
	my $tgt = $nba{$action};
	if ($islink) {
	    if ($file eq $tgt) {
		return $action;
	    }
	} else {
	    if (!system("cmp -s $file $tftpdir/$tgt")) {
		return $action;
	    }
	}  
    }
    return "unknown";
}

1;
