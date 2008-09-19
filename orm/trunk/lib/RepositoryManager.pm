package OSCAR::RepositoryManager;

#
# Copyright (c) 2008 Oak Ridge National Laboratory.
#                    Geoffroy R. Vallee <valleegr@ornl.gov>
#                    All rights reserved.
#
# This file is part of the OSCAR software package.  For license
# information, see the COPYING file in the top level directory of the
# OSCAR source distribution.
#

# $Id$

use strict;
use warnings;
use OSCAR::OCA::OS_Detect;
use Data::Dumper;
use Carp;

sub new {
    my $invocant = shift;
    my $class = ref($invocant) || $invocant;
    my $self = { 
        repos => undef,
        distro => undef,
        format => undef,
        pm => undef,
        @_,
    };
    bless ($self, $class);
    if (!defined ($self->{repos})
        && !defined ($self->{distro})
        && !defined ($self->{pm})) {
        die "ERROR: Invalid object, the initialization is most certainly ".
            "incorrect, please read the documentation for more information ".
            "(perldoc OSCAR::RepositoryManager)";
    }
    if (!defined ($self->{repos}) && defined ($self->{distro})) {
        require OSCAR::PackagePath;
        my ($dist, $ver, $arch) 
            = OSCAR::PackagePath::decompose_distro_id ($self->{distro});
        my $os = OSCAR::OCA::OS_Detect::open (fake=>
            {distro=>$dist, version=>$ver, arch=>$arch});
        my $drepo = OSCAR::PackagePath::distro_repo_url(os=>$os);
        my $orepo = OSCAR::PackagePath::oscar_repo_url(os=>$os);
        $self->{repos} = "$drepo,$orepo";
    }
    if (!defined ($self->{pm})) {
        if ($self->create_packman_object ()) {
            print "ERROR: Impossible to associate a PackMan object";
        }
    }

    return $self;
}

# Returns the list of repos for the current object. In order of preference:
#   - repos that have been specified manually during initialization,
#   - repos associated to the packman object specified during initialization
sub get_repos ($) {
    my $self = shift;

    if (defined ($self->{repos})) {
        return ($self->{repos});
    }

    if (defined ($self->{distro})) {
        require OSCAR::PackagePath;
        my ($dist, $ver, $arch) 
            = OSCAR::PackagePath::decompose_distro_id ($self->{distro});
        my $os = OSCAR::OCA::OS_Detect::open (fake=>
            {distro=>$dist, version=>$ver, arch=>$arch});
        my $drepo = OSCAR::PackagePath::distro_repo_url(os=>$os);
        my $orepo = OSCAR::PackagePath::oscar_repo_url(os=>$os);
        $self->{repos} = "$drepo,$orepo";
        return $self->{repos};
    }

    return undef;
}

################################################################################
# Create a packman object from basic info available. We typically have two     #
# cases:                                                                       #
# (i) we know the distro, from there, we know everything about the repos we    #
# need to use and the binary format used underneath (deb vs. rpm), we create   #
# the PackMan object from there;                                               #
# (ii) we have a list of repos, in that case, we can detect the format and     #
# instanciate the PackMan object.                                              #
#                                                                              #
# Input: None.                                                                 #
# Return: 0 if success, -1 else.                                               #
################################################################################
sub create_packman_object ($) {
    my $self = shift;

    if (!defined $self->{repos}) {
        carp "ERROR: This is bad, no repos are defined, this should never ".
             "happen" . status();
        return -1;
    }

    my @repos = split (",", $self->{repos});
    if (scalar (@repos) == 0) {
        carp "ERROR: Impossible to get the repositories";
        return -1;
    }
    require OSCAR::PackageSmart;
    my $format = OSCAR::PackageSmart::detect_pools_format (@repos);
    if (!defined ($format)) {
        carp "ERROR: Impossible to detect the binary format";
        return -1;
    }
    $self->{format} = $format;
    if ($format eq "deb") {
        require OSCAR::PackMan;
        $self->{pm} = PackMan::DEB->new;
        if (!defined $self->{pm}) {
            carp "ERROR: Impossible to create a PackMan object";
            return -1;
        }
    } elsif ($format eq "rpm") {
        require OSCAR::PackMan;
        $self->{pm} = PackMan::RPM->new;
        if (!defined $self->{pm}) {
            carp "ERROR: Impossible to create a PackMan object";
            return -1;
        }
    } else {
        carp "ERROR: Impossible to get the repo format";
        return -1;
    }
    $self->{pm}->repo (@repos);
    $self->{pm}->distro ($self->{distro}) if (defined $self->{distro});

    return 0;
}

################################################################################
# Search about packages.                                                       #
#                                                                              #
# Input: pattern, pattern to use for the search.                               #
# Return: return from PackMan->search->repo().                                 #
################################################################################
sub search_opkgs ($$) {
    my ($self, $pattern) = @_;

    return ($self->{pm}->search_repo ($pattern));
}

################################################################################
# Show details about OPKGs.                                                    #
#                                                                              #
# Input: opkg, a string representing the list of OPKGs (space between each     #
#              name).                                                          #
# Return: return from PackMan->show().                                         #
################################################################################
sub show_opkg ($$) {
    my ($self, $opkg) = @_;

    return $self->{pm}->show_repo ($opkg);
}

################################################################################
# Install a list of packages based on current ORM data.                        #
#                                                                              #
# Input: dest, where you want to install the package (chroot for instance),    #
#              undef if you want to install the packages on the local system.  #
# Return: return from PackMan->smart_install().                                #
################################################################################
sub install_pkg ($$@) {
    my ($self, $dest, @pkgs) = @_;

    $self->{pm}->chroot($dest);
    return $self->{pm}->smart_install (@pkgs);
}

################################################################################
# Gives the status of the current RepositoryManager object,                    #
#                                                                              #
# Input: none.                                                                 #
# Return: a string representing the status, undef if error.                    #
################################################################################
sub status ($) {
    my $self = shift;
    my $status = "Repository Manager Status:\n";
    $status .= "\tRepos: ".$self->{repos}."\n" if (defined $self->{repos});
    $status .= "\tDistro: ".$self->{distro}."\n" if (defined $self->{distro});
    $status .= "\tFormat: ".$self->{format}."\n" if (defined $self->{format});
    $status .= $self->{pm}->status() if (defined $self->{pm});
    return $status;
}


1;

__END__

=head1 NAME

RepositoryManager

=head1 DESCRIPTION

RepositoryManager is a Perl nodule that simply abstracts Packman; the goal is to
hide all details about the underneath binary package format, list of
repositories and so on.

=head1 AUTHOR

=item Geoffroy Vallee <valleegr@ornl.gov>

=cut