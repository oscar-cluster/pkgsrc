package OSCAR::SCVirt;

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
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

#
# $Id$
#

use strict;
use lib "$ENV{OSCAR_HOME}/lib";
use Carp;

use vars qw($VERSION @EXPORT);
use base qw(Exporter);

@EXPORT = qw (
                parse_sc_file
             );

my $number_kernels;

our @lines;
our @kernels;
our $default_kernel;
our @prelude = ();

sub find_kernels {
    my $n = 0;
    my $line;
    my $pos=0; 
    my $kernel_info;
    print "*************************************\n";
    print " Searching kernels ... **************\n";
    print "*************************************\n";
    while ($pos<=scalar (@lines)) {
        $line = $lines[$pos];
        chomp ($line);
        print "Looking for a kernel in: $line\n";
        if (substr($line, 0, 7) eq "[KERNEL") {
            my $kernel_path;
            my $kernel_label;
            print "A kernel definition has been found\n";
            $n++;
            $pos++;
            my $exit = 1;
            # when we find a kernel def, we parse the next block
            # where kernel info are
            while ($exit && ($pos < scalar (@lines))) {
                $line = $lines[$pos];
                chomp($line);
                print "Analyzing line $pos/".scalar (@lines).": $line\n";
                if ($line =~ /PATH/) {
                    print "Kernel path found ($line)\n";
                    $kernel_path = $line;
                } 
                if ($line =~ /LABEL/) {
                    print "Kernel label found ($line)\n";
                    $kernel_label = $line;
                }
                if (($pos+1 < scalar (@lines)) && 
                    (substr($lines[$pos+1], 0, 7) eq "[KERNEL")) {
                        print "Next line is a new kernel, we stop\n";
                        $exit = 0;
                }
                $pos++;
            }
            $kernel_info = ({path => $kernel_path,
                            label => $kernel_label});
            push(@kernels, $kernel_info);
        } else {
            print "Nothing to do with the line $pos, we continue\n";
            $pos++;
        }
    }

    print "We found the following kernels:\n";
    my $test = $kernels[0];
    use Data::Dumper;
    print Dumper(@kernels);
    my $toto = $kernels[0]{'path'};
    print "$toto\n";
    return 1;
}

sub find_default_kernel {
    my $pos = 0;
    my $line = $lines[$pos];
    chomp($line);
    print "*************************************\n";
    print " Searching the default kernel ... ***\n";
    print "*************************************\n";
    print "Number of lines to analyze: ".scalar (@lines)."\n";
    while ($pos<=scalar (@lines)) {
        print "Analyzing $line...\n";
        if ($line =~ /DEFAULTBOOT/) {
            print "We found the default kernel ($line)\n";
            $default_kernel = $line;
            last;
        } 
        $pos++;
        $line = $lines[$pos];
        chomp($line);
    }
}

sub get_prelude {
    my $pos = 0;
    print "*************************************\n";
    print " Getting the prelude ... ************\n";
    print "*************************************\n";
    my $line = $lines[$pos];
    while ($pos<=scalar (@lines)) {
        if ($line =~ /DEFAULTBOOT/) {
            print "We found the end of the prelude\n";
            last;
        }
        print $line;
        push (@prelude, $line);
        $pos++;
        $line = $lines[$pos];
    }
}

sub generate_new_config_file {
    my $path = shift;
    my $file = $path . "/systemconfig.conf";

    print "Creating a new SystemConfigurator config file ($file)\n";

    open (FILE, ">$file");
    foreach my $line (@prelude) {
        print FILE $line;
    }
    print FILE "\tDEFAULTBOOT = Xen\n\n";
    my $i=1;
    print FILE "[KERNEL0]\n";
    print FILE "\tPATH = /boot/xen.gz\n";
    print FILE "\tAPPEND = dom0_mem=131072\n";
    print FILE "\tHOSTOS = /boot/vmlinuz-2.6.16-xen\n";
    print FILE "\tINITRD = /boot/initrd-2.6-xen.img\n";
    print FILE "\tLABEL = Xen\n\n";
    foreach my $k (@kernels) {
        print FILE "[KERNEL$i]\n";
        print FILE $k->{'path'};
        print FILE "\n";
        print FILE $k->{'label'};
        print FILE "\n";
        print FILE "\n";
        $i++;
    }

    close (FILE);
}

sub parse_sc_file {
    my $file = shift;
    my $path = shift;

    if (! -f $file) {
        carp ("Impossible to open the configuration file ($file)\n");
    }
    open (FILE, "<$file");
    @lines = <FILE>;
    close (FILE);

    find_kernels ();
    find_default_kernel ();
    get_prelude ();
    generate_new_config_file ($path);
}

1;
