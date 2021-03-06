package OSCAR::Packager;

#
# Copyright (c) 2009 Geoffroy Vallee <valleegr@ornl.gov>
#                    Oak Ridge National Laboratory
#                    All rights reserved.
#
#   $Id$
#
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
# $Id$
#

BEGIN {
    if (defined $ENV{OSCAR_HOME}) {
        unshift @INC, "$ENV{OSCAR_HOME}/lib";
    }
}

use strict;
use Carp;
use vars qw($VERSION @EXPORT);
use base qw(Exporter);
use Cwd;
use File::Basename;
use File::Copy;
use File::Path;
use OSCAR::ConfigFile;
use OSCAR::Defs;
use OSCAR::FileUtils;
use OSCAR::Logger;
use OSCAR::OCA::OS_Detect;
use OSCAR::Utils;

@EXPORT = qw(
            available_releases
            package_opkg
            prepare_prereqs
            );

my $verbose = 1;

############################################################################
# Read in package_build config file.
# This is called build.cfg and should be located in the SRPMS directory.
# - matches the first header of the form [$distro:$version:$arch] that
#   fits the current machine
#
# Return: the lines of the matched block in an array, undef if error.
############################################################################
sub get_config {
    my ($path, $distro, $distver, $arch) = @_;
    local *IN;
    my ($line, $match, @config);

    open IN, "$path" or (carp "ERROR: Could not open $path: $!", return undef);
    while ($line = <IN>) {
        chomp $line;
        if ($line =~ /^\s*\[([^:]+):([^:]+):([^:]+)\]/) {
            my ($d,$v,$a) = ($1,$2,$3);
            $d =~ s/\*/\.*/g;
            $v =~ s/\*/\.*/g;
            $a =~ s/\*/\.*/g;
            my $str = "$distro:$distver:$arch";
            my $mstr = "($d):($v):($a)";
            $match = 0;
            if ($str =~ m/^$mstr$/) {
            $match = 1;
            print "found matching block [$d:$v:$a] for $distro:$distver:$arch\n" 
                if $verbose;
            last;
            }
        }
    }
    if ($match) {
        while ($line = <IN>) {
            chomp $line;
            last if ($line =~ /^\[([^:]+):([^:]+):([^:]+)\]/);
            next if ($line =~ /^\s*\#/);
            $line =~ s/^ *//g;
            next if ($line =~ /^$/);
            push @config, $line;
        }
    }
    close IN;
    if (@config) {
        return @config;
    } else {
        return undef;
    }
}

sub parse_build_file ($) {
    my $dir = shift;
    my @config;

    my $os = OSCAR::OCA::OS_Detect::open();
    if (!defined $os) {
        die "ERROR: Impossible to detect the binary package format";
    }

    # read in package build config file
    my $cfile = "$dir/build.cfg";
    if (-e $cfile) {
        @config = get_config($cfile,
                             $os->{compat_distro},
                             $os->{compat_distrover},
                             $os->{arch});
    } else {
        carp "ERROR: Build configuration file $cfile not found!";
        return undef;
    }
    return @config;
}

#########################
# Split up config lines into config blocks headed by requires lines.
# This allows multiple requires to be included in one opkg and
# the usage of requirements coming from the own opkg.
#
# The resulting array contains references to hashes with following structure:
# {
#   'requires' => requires_string, # requirements for current block
#   'common' => {                  # build results go to distro/common-rpms/
#                 'pkg_match1' => {       # match string (glob) for srpm name
#                                   'opt' => 'rpmbuild_additional_options'
#                                 },
#                 'pkg_match2' => {...},
#               },
#   'dist' => {                  # build results go to distro/$dist$ver-$arch/
#               'pkg_match3' => {       # match string (glob) for srpm name
#                                 'opt' => 'rpmbuild_additional_options'
#                               },
#               'pkg_match4' => {...},
#               ...
#             },
#   'env' => {
#               'variable' => value,
#            }
# },
#########################
sub split_config {
    my (@config_array) = @_;

    my @conf_blocks;
    my $conf = {};
    for (my $i = 0; $i <= $#config_array; $i++) {
        my $line = $config_array[$i];

        if ($line =~ /^(\S+)\s*(.*)\s*$/) {
            my $cmd = $1;
            my $data = $2;
            #my ($cmd,@data) = split(/\s+/,$line);
            if ($cmd =~ /^requires:/) {
                if (exists($conf->{requires}) ||
                (!exists($conf->{requires}) && $i > 0)) {
                    push @conf_blocks, $conf;
                    $conf = {};
                }
                $conf->{requires} = $data;
            } elsif($cmd =~ /^common:/) {
                my ($pkg,@data) = split(/\s+/,$data);
                $conf->{common}{"$pkg"}{opt} = join(" ",@data);
            } elsif($cmd =~ /^env:/) {
                if (exists($conf->{env})) {
                    $conf->{env} .= " $data";
                } else {
                    $conf->{env} = $data;
                }
            } elsif($cmd !~ /:$/) {
                $conf->{dist}{"$cmd"}{opt} = $data;
            }
        }
    }
    push @conf_blocks, $conf;

    return @conf_blocks;
}

# Return: undef if error.
sub prepare_rpm_env ($$$$$) {
    my ($name, $os, $sel, $confp, $dest) = @_;

    #
    # We check the parameters
    #
    if (!defined ($confp) && ref($confp) ne "HASH") {
        carp "ERROR: Invalid configuration data";
        return undef;
    }
    if (!OSCAR::Utils::is_a_valid_string ($name)) {
        carp "ERROR: Invalid OPKG name";
        return undef;
    }
    if (!defined ($os)) {
        carp "ERROR: Invalid OS_Detect data";
        return undef;
    }
    if (! -d $dest) {
        carp "ERROR: Invalid destination";
        return undef;
    }

    my %conf = %{$confp};
    my $env;

    my $arch = $os->{arch};
    my $march = $arch;
    $march =~ s/^i.86$/i?86/;
    my $build_arch = $arch;
    $build_arch =~ s/^i.86$/i686/;

    # gather list of srpms which need to be built
    my (@sbuild, @buildopts);
    # we get data for the specific package from the config data
    my %data = %{${$conf{$sel}}{$name}};
    # TODO: we should check if the package is already there or not, but right
    # now i cannot get the package version.
#     my $name = $conf{$sel}{"$g"}{name};
#     my $ver = $conf{$sel}{"$g"}{ver};
#     my $str = "$dest/$name-*$ver.{$march,noarch}.rpm";
#     OSCAR::Logger::oscar_log_subsection "str: $str\n" if $verbose;
#     my @gres = glob($str);
#     if (!scalar(@gres)) {
        push @sbuild, $data{srpm};
        push @buildopts, $data{opt};
#     } else {
#         OSCAR::Logger::oscar_log_subsection "Found binary packages ".
#             "for $g:\n\t".join("\n\t",@gres).
#             "\nSkipping build.\n";
#     }
    if (exists($conf{env})) {
        $env = $conf{env};
    }
    if (! -d "$dest") {
        OSCAR::Logger::oscar_log_subsection ("Creating directory: $dest");
        eval { File::Path::mkpath ("$dest") };
        if ($@) {
            carp "ERROR: Couldn't create $dest: $@";
            return 1;
        }
        chdir("$dest");
    }

    my $opt = $data{opt};
    if ($sel eq "common") {
        if ($opt !~ m,--target,) {
            $opt .= " --target noarch";
        }
    } else {
        if ($opt !~ m,--target,) {
            $opt .= " --target ".$build_arch;
        }
    }
    $ENV{RPMBUILDOPTS} = $opt;
    if ($opt) {
        OSCAR::Logger::oscar_log_subsection ("Setting \$RPMBUILDOPTS=$opt...");
    }
    return $opt;
}

# This routine effectively create binary packages, hiding the differences 
# between RPMs and Debs.
#
# Return: -1 if error, 0 else.
sub create_binary ($$$$$$) {
    my ($basedir, $name, $conf, $sel, $test, $output) = @_;

    OSCAR::Logger::oscar_log_subsection ("Packaging $name");

    # We can sure the directory were we save downloads is ready
    # TODO: We should be able to specify that via a config file.
    my $download_dir = "/var/lib/oscar-packager/downloads";
    my $build_dir = "/var/lib/oscar-packager/build";

    my $binaries_path = OSCAR::ConfigFile::get_value ("/etc/oscar/oscar.conf",
                                                      undef,
                                                      "OSCAR_SCRIPTS_PATH");

    my $os = OSCAR::OCA::OS_Detect::open();
    if (!defined $os) {
        carp "ERROR: Impossible to detect the binary package format";
        return -1;
    }

    if (! -d $download_dir) {
        eval { File::Path::mkpath ($download_dir) };
        if ($@) {
            carp "ERROR: Couldn't create $download_dir: $@";
            return -1;
        }
    }

    # Is the config file for the package creation here or not?
    my $config_file = "$basedir/$name.cfg";
    if (! -f $config_file) {
        carp "ERROR: $config_file does not exist";
        return -1;
    }

    # Now, since we can access the config file, we parse it and download the
    # needed source files.
    OSCAR::Logger::oscar_log_subsection "Downloading sources for $name...";
    my $source_data = OSCAR::ConfigFile::get_value ("$config_file",
                                                    undef,
                                                    "source");
    my ($method, $source) = split (",", $source_data);
    if (OSCAR::FileUtils::download_file ($source,
                                         $download_dir,
                                         $method,
                                         OSCAR::Defs::NO_OVERWRITE())) {
        carp "ERROR: Impossible to download the source file ($source)";
        return -1
    }

    my $source_file = File::Basename::basename ($source);
    my $source_type = 
        OSCAR::FileUtils::file_type ("$download_dir/$source_file");
    if (!defined $source_type) {
        carp "ERROR: Impossible to detect the source file format";
        return -1;
    }

    my $cmd;
    if ($os->{pkg} eq "rpm") {
        if ($source_type eq OSCAR::Defs::SRPM()) {
            my $s = prepare_rpm_env ($name, $os, $sel, $conf, "/tmp");
            $cmd = "$binaries_path/build_rpms --only-rpm $download_dir/$source_file $s";
            OSCAR::Logger::oscar_log_subsection "Executing: $cmd";
            if (!$test) {
                if (system($cmd)) {
                    carp "ERROR: Command execution failed: $! ($cmd)";
                    return -1;
                } 
            }
            $cmd = "mv *$name*.rpm $output";
            print "Executing: $cmd\n";
            if (system ($cmd)) {
                carp "ERROR: Impossible to execute $cmd";
                return -1;
            }
        } elsif ($source_type eq OSCAR::Defs::TARBALL()) {
            # We copy the tarball in %{_sourcedir}
            my $sf = "$download_dir/$source_file";
            my $d = `/bin/rpm --eval %{_sourcedir}`;
            File::Copy::copy ($sf, $d) 
                or (carp "ERROR: impossible to copy the file ($sf, $d)",
                    return -1);

            # We try to execute rpmbuild using the spec file
            my $spec_file = "./$name.spec";
            if (! -f $spec_file) {
                $spec_file = "./rpm/$name.spec";
            } 
            $cmd = "rpmbuild -bb $spec_file";
            if (system ($cmd)) {
                carp "ERROR: Impossible to execute $cmd";
                return -1;
            }
        } else {
            carp "ERROR: Unsupported file type for binary package creation ".
                 "($source_type)";
            return -1;
        }
    } elsif ($os->{pkg} eq "deb") {
        if ($source_type eq OSCAR::Defs::TARBALL()) {
            # TODO: we should use the build_dir to untar the tarball and try
            # to create the package.

            # We untar the tarball and try to execute "make deb" against the 
            # source code
            $cmd = "cd $download_dir; tar xzf $source_file";
            if (system $cmd) {
                carp "ERROR: Impossible to execute $cmd";
                return -1;
            }
            $cmd = "make deb";
            if (system $cmd) {
                carp "ERROR: Impossible to execute $cmd";
                return -1;
            }
        } else {
            carp "ERROR: Unsupported file type for binary package creation ".
                 "($source_type)";
            return -1;
        }
    } else {
        carp "ERROR: $os->{pkg} is not currently supported";
        return -1;
    }

    return 0;
}

################################################################################
# Core binary package building routine
# - detects name and version-release from source rpms
# - checks the binary package(s) already exist in the target dir
# - builds by calling create_binary, adds build options from config file
# - all binary packages resulting from build end up in the target directory
#
# Return: 0 if no error, else the number of errors during the build process.
sub build_if_needed ($$$$) {
    my ($confp, $pdir, $sel, $target) = @_;
    my ($march, $build_arch, $OHOME, $test, $err);
    $test = 0;

    OSCAR::Logger::oscar_log_subsection ("Building binary packages");

    my $env;
    my %conf = %{$confp};
    if (exists($conf{$sel})) {
#         &srpm_name_ver($pdir,$conf{$sel});

        for my $g (keys(%{$conf{$sel}})) {
            if (create_binary ($pdir, $g, $confp, $sel, $test, $target)) {
                carp "ERROR: Impossible to create the binary ".
                     "($g, $test, $target)";
                $err++;
            }
        }
    }

    if ($err) {
        OSCAR::Logger::oscar_log_subsection ("ERROR: Impossible to create ".
            "some binary packages");
    } else {
        OSCAR::Logger::oscar_log_subsection ("Binary packages created");
    }
    return $err;
}

################################################################################
# Install requires by invoking packman. Requires are remembered in the global  #
# array @installed_reqs, which is used for deleting them after the successful  #
# build.                                                                       #
#                                                                              #
# Input: requires, a string representing the list of binary packages to        #
#        install, package being separated by spaces.                           #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub install_requires {
    my ($requires) = @_;

    OSCAR::Logger::oscar_log_subsection ("Installing requirements");

    if (!$requires) {
        OSCAR::Logger::oscar_log_subsection ("No requirements to install");
        return 0;
    }
    my $test = 0;
    my @installed_reqs = ();

    my @reqs = split(" ",$requires);
    OSCAR::Logger::oscar_log_subsection ("Requires: ".join(" ",@reqs));

    my @install_stack;
    for my $r (@reqs) {
    if ($r =~ /^(.*):(.*)$/) {
        my $opkg = $1;
        my $pkg = $2;
        push (@install_stack, $pkg);
    } else {
        push (@install_stack, $r);
    }
    }
    if (!$test) {
        my %before;
        # TODO: create an abstraction to do based on both RPM and Debian tools.
#         # remember bianry packages which were installed before
#         my %before = map { $_ => 1 }
#                     split("\n",`rpm -qa --qf '%{NAME}.%{ARCH}\n'`);
	my $os = OSCAR::OCA::OS_Detect::open ();
	if (!defined $os && ref($os) ne "HASH") {
            carp "ERROR: Impossible to detect the local distro";
            return -1;
        }
	my $distro_id = "$os->{distro}-$os->{distro_version}-$os->{arch}";
        my $cmd = "/usr/bin/packman install ".join(" ",@install_stack)." --distro $distro_id";
        $cmd .= " --verbose" if $verbose;
        OSCAR::Logger::oscar_log_subsection ("Executing: $cmd");
        if (system($cmd)) {
            print "Warning: failed to install requires: ".join(" ",@reqs)."\n";
        }
        # TODO: update that for both RPM and Debian
#         local *CMD;
#         open CMD, "rpm -qa --qf '%{NAME}.%{ARCH}\n' |" or do {
#             print "ERROR: Could not run rpm command\n";
#             return;
#             };
        while (<CMD>) {
            chomp;
            next if exists $before{$_};
            push @installed_reqs, $_;
        }
        close CMD;
        undef %before;
    }

    OSCAR::Logger::oscar_log_subsection ("Requirements installed");
    return 0;
}


# Input: pdir, where the source code is
#        confp, an array representing the build.cfg file for the software to package.
# Return: 0 if success, -1 else.
sub build_binaries ($$$) {
    my ($pdir, $confp, $output) = @_;
    my $err;

    my @conf_blocks = split_config(@$confp);

    for my $cblock (@conf_blocks) {
        my %conf = %{$cblock};

        # install requires
        if (install_requires($conf{requires})) {
            carp "ERROR: Impossible to install requirements";
            return -1;
        }

        # check and build common-rpms if needed
        $err = build_if_needed(\%conf, $pdir, "common", $output);
        if ($err) {
            carp "ERROR: Impossible to build a binary ($pdir)";
            return -1;
        }

        # check and build dist specific binary packages if needed
        $err = build_if_needed(\%conf, $pdir, "dist", $output);
        if ($err) {
            carp "ERROR: Impossible to build a binary ($pdir)";
            return -1;
        }
    }

    return 0;
}

# Return: 0 if success, -1 else.
sub package_opkg ($$) {
    my ($build_file, $output) = @_;
    if (! -f ($build_file)) {
        carp "ERROR: Invalid path ($build_file)";
        return -1;
    }

    if (! -d $output) {
        carp "ERROR: Output directory does not exist ($output)";
        return -1;
    }

    my $pdir = File::Basename::dirname ($build_file);;
    my $pkg = File::Basename::basename ($pdir);
    if (! -d $pdir) {
        carp "ERROR: Could not locate package location based on $build_file!\n";
        return -1;
    }
    if (!OSCAR::Utils::is_a_valid_string ($pkg)) {
        carp "ERROR: Impossible to get the OPKG name";
        return -1;
    }

    OSCAR::Logger::oscar_log_subsection "============ $pkg ===========";


    my @config = parse_build_file ($pdir);
    if (scalar (@config) == 0) {
        die "ERROR: Impossible to parse the build file";
    }

    OSCAR::Utils::print_array (@config);

    # main build routine
    if (build_binaries ($pdir, \@config, $output)) {
        carp "ERROR: Impossible to build some binaries";
        return -1;
    }
    
# 
#     # remove installed requires
#     &remove_installed_reqs;

    return 0;
}

sub available_releases () {
    my $path = "/etc/oscar/oscar-packager";
    my @files = OSCAR::FileUtils::get_files_in_path ("$path");

    die "ERROR: Impossible to scan $path" if (scalar @files == 0);

    my @releases;
    foreach my $f (@files) {
        if ($f =~ m/^core_stable_(.*).cfg$/) {
            push (@releases, $1);
        }
    }

    return (@releases);
}

# This function deals with prereqs for the packaging of a given OSCAR 
# component. The management of prereqs is based on a build.cfg file at the top
# directory of the OSCAR component source tree. If the file does not exist, we
# assume there is no prereq to deal with.
#
# Return: -1 if error, 0 else.
sub prepare_prereqs ($$) {
    my ($dir, $output) = @_;

    my $build_file = "$dir/build.cfg";
    if (! -f "$build_file") {
        OSCAR::Logger::oscar_log_subsection ("No $build_file, no prereqs");
    } else {
        OSCAR::Logger::oscar_log_subsection ("Managing prereqs ($build_file)");
        if (package_opkg ($build_file, $output)) {
            carp "ERROR: Impossible to prepare the prereqs ($dir, $output)";
            return -1;
        }
    }

    return 0;
}


__END__

=head1 Exported functions

=over 4

=item package_opkg

Package a given OPKG. Example: package_opkg ("/var/lib/oscar/package/c3/build.cfg");

=item available_releases

Returns the list of OSCAR releases that can be packaged. Example: my @oscar_releases = available_releases ();

=back

=head1 AUTHORS

=over 4

=item Erich Fotch

=item Geoffroy Vallee, Oak Ridge National Laboratory <valleegr at ornl dot gov>

=back

=cut
