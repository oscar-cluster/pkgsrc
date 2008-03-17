package OSCARInstaller::ConfigManager;

#
# Copyright (c) 2008 Oak Ridge National Laboratory.
#                    Geoffroy R. Vallee <valleegr@ornl.gov>
#                    All rights reserved.
#
# This file is part of the OSCAR software package.  For license
# information, see the COPYING file in the top level directory of the
# OSCAR source distribution.
#

#
# The file is the implementation of the ConfigManager class. This class allows
# the creation of object that represents the content of the OSCAR configuration
# file
#

#
# TODO: the current implementation only grabs few information from the 
# configuration file and therefore we do not have a generic API to access each
# different values. That could be improved in order to avoid an uncontroled
# growing the list of functions in the API.
#

#
# $Id: ConfigManager.pm 6954 2008-03-14 20:54:25Z valleegr $
#

use strict;
use warnings "all";
use Carp;

##########################################################
# A bunch of variable filled up with creating the object #
##########################################################

# URL of the file giving all the md5sums. Useful to check downloaded files.
our $md5sum_file;

# List of supported distros (everything in a single string, each distro 
# separated by ","
our $supported_distros;
# The list of supported distros but within an array
our @distros;
# URL where file can be downloaded, e.g.,
# http://ftp.ussg.iu.edu/oscar/oscar-5.0-MD5SUMS
our $base_url;
# Location where the downloaded files are saved
our $download_dir;
# Location where OSCAR is installed (e.g., /opt)
our $install_dir;

sub new {
    my $invocant = shift;
    my $class = ref($invocant) || $invocant;
    my $self = { 
        config_file => "/etc/oscar-installer/oscar-installer.conf", 
        @_,
    };
    bless ($self, $class);
    load_config ($self);
    return $self;
}

sub load_config ($) {
    my $self = shift;
    my $config_file = $self->{config_file};

    require AppConfig;

    if (!defined($config_file) || ! -f $config_file) {
        print "ERROR: the configuration file does not exist ($config_file)\n";
        return -1;
    }

    use vars qw($config);
    $config = AppConfig->new(
        'BASE_URL'                  => { ARGCOUNT => 1 },
        'DOWNLOAD_DIR'              => { ARGCOUNT => 1 },
        'INSTALL_DIR'               => { ARGCOUNT => 1 },
        'MD5SUM_FILE'               => { ARGCOUNT => 1 },
        'SUPPORTED_DISTROS'         => { ARGCOUNT => 1 },
        );
    $config->file ($config_file);

    # Load configuration values
    $base_url           = $config->get('BASE_URL');
    $download_dir       = $config->get('DOWNLOAD_DIR');
    $install_dir        = $config->get('INSTALL_DIR');
    $md5sum_file        = $config->get('MD5SUM_FILE');
    $supported_distros  = $config->get('SUPPORTED_DISTROS');
    # We clean-up the string we got, the one that gives the list of supported
    # distros
    while (index($supported_distros, " ") != -1) {
        $supported_distros  =~ s/\s+/,/;
    }
    while (index($supported_distros, "\t") != -1) {
        $supported_distros  =~ s/\t+/,/;
    }
    @distros = split (",", $supported_distros);
}

sub get_config () {
    my $self = shift;
    my %cfg = (
                'base_url'          => $base_url,
                'download_dir'      => $download_dir,
                'install_dir'       => $install_dir,
                'md5sum_file'       => $md5sum_file,
                'distros'           => \@distros,
              );
    return \%cfg;
}

1;

__END__
