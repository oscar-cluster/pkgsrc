#########################################################################
# Perl class for subclusters
# A subcluster is defined by one of:
#        - image name
#        - domain name
#        - nodelist
# Methods:
#   ->nodes()  : returns space separated string with list of member nodes
#                of the subcluster. This info is generated dynamically.
#   ->status() : returns a hash array with two keys: online and offline.
#                They contain space separated lists of nodes.
#   ->execone($) : Execute command on the first online member node.
#   ->cexec($,$) : call C3 cexec for the whole subcluster. Returns a
#                temporary filename with the output. The temporary file
#		 will be deleted when the object is destroyed.
#   ->cpush($,$) : call C3 cpush command for the whole subcluster.
#
# $Id: Subcluster.pm,v 1.11 2005/12/01 13:06:31 focht Exp $
# (c) 2004 NEC EHPCTC, Erich Focht

package HPCL::Subcluster;

use Carp;
use lib '/usr/lib/systeminstaller';
use SIS::Client;
use SIS::Adapter;
use SIS::Image;
use SIS::DB;
use FileHandle;
use POSIX;
use Data::Dumper;
use Fcntl;

my $C3PATH="/usr/bin";

sub new {
    my $classname = shift;
    my $self = { };
    bless($self, $classname);
    if ($self->_init(@_)) {
	return $self;
    } else {
	return undef;
    }
}

sub _init {
    my $self = shift;
    if (@_) {
	my %extra = @_;
	@$self{keys %extra} = values %extra;
    }
    if (defined($self->{image})) {
	return $self->init_image();
    } elsif (defined($self->{domain})) {
	return $self->init_domain();
    } elsif (defined($self->{nodelist})) {
	return $self->init_nodelist();
    }
}

#### Validate definition ####

sub init_nodelist {
    my $self = shift;
    my @nodes = split /\s+/, $self->{nodelist};
    # ... cnum ...
    # validate node names
    my $ret = 1;
    foreach (@nodes) {
	if (!scalar(list_client(hostname => $_)) &&
	    !scalar(list_client(name => $_))) {
	    $self->vprint("WARNING: Node $_ not found in cluster!\n");
	    $ret = 0;	
	}
    }
    return $ret;
}

sub init_image {
    my $self = shift;
    my $image = $self->{image};
    # validate: does image exist in database?
    my @images = list_image(name => $image);
    return 1 if (scalar(@images));
    return 0;
}

sub init_domain {
    my $self = shift;
    my $domain = $self->{domain};
    # validate: does domain exist anywhere?
    my @clients = list_client(domainname => $domain);
    return 1 if (scalar(@clients));
    return 0;
}

sub DESTROY {
    my $self = shift;
    if (defined($self->{scconf})) {
	my $f = $self->{scconf};
	if (-f $f) {
	    unlink $f;
	}
    }
    if (defined($self->{scoutf})) {
	while (my $f = shift @{$self->{scoutf}}) {
	    if (-f $f) {
		unlink $f;
	    }
	}
    }
}

#### Helper subroutines #####

# strip away domain name (i.e. everything behind the first dot in the string)
#
sub strip_domain {
    my ($s) = @_;
    $s =~ s:\..*$::g;
    return $s;
}


# filter hosts know to be alive by ganglia gstat
#
sub alive_gstat {
    my $self = shift;
    my ($gip, $gpo, @online, @nodes, @alive);
    
    if (! -x "/usr/bin/gstat") {
	return (1, ());
    }
    # gmetad server name can be passed in an environment variable
    if ($ENV{SC3_GMETAD_IP}) {
	$gip = $ENV{SC3_GMETAD_IP};
    } else {
	my ($d1, $d2, $d3, $d4, @a) = gethostbyname("oscar-server");
	my $hname = `hostname`;
	chomp $hname;
	if (grep /^$hname$/, split(/ /,"$d1 $d2")) {
	    $gip = "localhost";
	} else {
	    $gip = "oscar-server";
	}
    }
    # gmetad server port number can be passed in an environment variable
    if ($ENV{SC3_GMETAD_PORT}) {
	$gpo = $ENV{SC3_GMETAD_PORT};
    } else {
	$gpo = 8651;
    }

    # get node list of this subcluster
    @nodes = $self->nodes();
    # get list of hosts which are alive
    local *IN;
    my $cmd = "/usr/bin/gstat -a -1 -l -i$gip -p$gpo";
    open IN, "$cmd |" or
	croak ("Could not run $cmd");
    while (<IN>) {
	m/^(\S+)\s/;
	my $up = $1;
	if ($self->{c3domnames}) {
	    # this doesn't catch the case where gstat shows no domain names
	    push @alive, $up;
	} else {
	    push @alive, strip_domain($up);
	}
    }
    close IN;

    my $match = join("|", @alive);
    for $n (@nodes) {
	if ($n =~ m/^($match)$/ || $n =~ m/^($match)\./) {
	    push @online, $n;
	}
    }
    return (0, @online);
}


# filter hosts which are alive and respond to cexec
#
sub alive_cexec {
    my $self = shift;
    my @online;
    my $fh = $self->cexec("echo ISUP:","-p");
    while (<$fh>) {
	if (/ISUP:/) {
	    my ($c, $h, $rest) = split / /;
	    chop $h;
	    push @online, $h;
	}
    }
    close $fh;
    return @online;
}


#### Methods ####

# switch on/off verbose mode
sub verbose {
    my $self = shift;
    my $arg = (@_);
    if ($arg > 0) {
	$self->{verbose} = $arg;
    } else {
	$self->{verbose} = 0;
    }
}

# verbose print if needed
sub vprint {
    my $self = shift;
    print @_ if ($self->{verbose});
}

# return array of node-names of current members
# !! there is no mechanism to inform about changes!!
sub nodes {
    my $self = shift;
    my @nodes;
    if (defined($self->{image})) {
	#
	# subcluster defined by image name
	#
	my @machines = list_client(imagename=>$self->{image});
	for my $m (@machines) {
	    next if ($m->imagename ne $self->{image});
	    push @nodes, $m->hostname;
	}
    } elsif (defined($self->{domain})) {
	#
	# subcluster defined by domain name
	#
	my @machines = list_client(domainname=>$self->{domain});
	for my $m (@machines) {
	    push @nodes, $m->hostname;
	}
    } else {
	#
	# subcluster defined by nodelist
	#
	@nodes = split /\s+/,$self->{nodelist};
    }
    #
    # check whether c3 config file uses domain names or not
    #
    if (!defined($self->{c3domnames}) && scalar(@nodes)) {
	# if any of the nodes has no domain, strip domains
	$self->{c3domnames} = 1;
	for my $n (@nodes) {
	    if ($n eq strip_domain($n)) {
		$self->{c3domnames} = 0;
	    }
	}
	# only continue if all nodes had domains
	if ($self->{c3domnames}) {
	    # try cnum for first node in list
	    if (`$C3PATH/cnum $nodes[0] | grep -c $nodes[0]` == 1) {
		$self->{c3domnames} = 1;
	    } else {
		my $sname = strip_domain($nodes[0]);
		if (`$C3PATH/cnum $sname | grep -c $sname` == 1) {
		    $self->{c3domnames} = 0;
		} else {
		    print "WARNING: node $nodes[0] not found in c3 configuration!\n";
		    print "         C3 could be misconfigured!\n";
		}
	    }
	}
    }
    # strip domain names if needed
    if (!$self->{c3domnames}) {
	for (my $i=0; $i <=$#nodes; $i++) {
	    $nodes[$i] = strip_domain($nodes[$i]);
	}
    }
    $self->vprint("Nodes() : " . join(" ",@nodes) . "\n");
    $self->{nodes} = \@nodes;
    return @nodes;
}


# build online and offline nodes lists
#   this method calls implicitely ->nodes()
sub status {
    my $self = shift;
    my @online;
    my @offline;
    my $err;

    ($err,@online) = $self->alive_gstat();
    if ($err) {
	# reverting to slow status check
	@online = $self->alive_cexec();
    }

    my $e = "(" . join("|",@online) . ")";
    @offline = grep !/$e/, @{$self->{nodes}};

    # arrays saved in object
    $self->{online} = \@online;
    $self->{offline} = \@offline;

    # build scalable config if cluster big enough
    my $minscal = 31;
    if ($ENV{SC3_MINSCAL}) {
	$minscal = $ENV{SC3_MINSCAL};
    }
    if (scalar(@online) >= $minscal) {
	$self->vprint("Building dynamic scalable c3 config, number of nodes exceeds $minscal.\n");
	$self->{scconf} = $self->build_scconf(@online);
    }

    # return hash
    my %s;
    $s{online} = join(" ",@online);
    $s{offline} = join(" ",@offline);
    return %s;
}

sub _tmpfile {
    my $tmpname;
    my $fh = new FileHandle;
    # Keep trying names until we get one that's brand new.
    do {
	$tmpname = tmpnam();
    } until sysopen($fh, $tmpname, O_RDWR | O_CREAT | O_EXCL, 0600);
    return ($tmpname,$fh);
}

#
# Build dynamical scalable c3 configuration file
#
sub build_scconf {
    my $self = shift;
    my (@nodes) = @_;
    my @conf;
    my $n = scalar(@nodes);
    my $npart = int(sqrt($n));
    my $i;
    my $cnt = 0;
    for (my $p = 0; $cnt < $n; $p++) {
	push @conf,"cluster part$p {\n";
	push @conf,"   $nodes[$cnt]\n";
	for ($i = $cnt; $i < $cnt+$npart; $i++) {
	    last if ($i > $n);
	    push @conf,"   $nodes[$i]\n";
	}
	$cnt = $i;
	push @conf,"}\n";
    }
    my ($tmpname,$fh) = &_tmpfile();
    print $fh @conf;
    close $fh;
    push @{$self->{scoutf}}, $tmpname;
    return $tmpname;
}

# merge additional cluster node ID into existing C3 sequence
sub merge_add {
    my ($s, $n) = @_;
    # last entry
    # print "s=$s n=$n\n";
    if ($s =~ m/^(.+)(,|-)(\d+)$/) {
	my $first = $1;
	my $sign = $2;
	my $last = $3;
	# print "first=$first sign=$sign last=$last\n";
	if ($sign eq ",") {
	    if ($n == ($last + 1)) {
		$s = $s . "-" . $n;
	    } else {
		$s = $s . "," . $n;
	    }
	} elsif ($sign eq "-") {
	    if ($n == ($last + 1)) {
		$s = $first . "-" . $n;
	    } else {
		$s = $s . "," . $n;
	    }
	}
    } else {
	# can be only one number
	if ($n == ($s + 1)) {
	    $s = $s . "-" . $n;
	} else {
	    $s = $s . "," . $n;
	}
    }	    
    return $s;
}

# return c3hash with members of subcluster
sub c3members {
    my $self = shift;
    my %members;
    my @marray = sort($self->nodes());
    if (!scalar(@marray)) {
	return %members;
    }
    my $nodelist = join(" ",@marray);
    local *IN;
    open IN, "$C3PATH/cnum $nodelist |" or carp("Could not run cnum!");
    while (<IN>) {
	if (/index (\d+) in cluster (\S+)$/) {
	    my $cluster = $2;
	    my $indx = $1;
	    if (defined($members{$cluster})) {
		$members{$cluster} = merge_add($members{$cluster}, $indx);
	    } else {
		$members{$cluster} = $indx;
	    }
	}
    }
    close(IN);
    return %members;
}

# return image path
sub imgpath {
    my $self = shift;
    if (defined($self->{image})) {
	my $img = list_image(name => $self->{image});
	return $img->location;
    }
    return undef;
}

# return image arch
sub imgarch {
    my $self = shift;
    if (defined($self->{image})) {
	my $img = list_image(name => $self->{image});
	return $img->arch;
    }
    return undef;
}

# execute command on one of the online nodes of the subcluster
# by passing the arguments to ssh.
# arguments:   command to execute in chrooted environment
# Returns a filehandle with the stdout. This needs to be closed after
# the command has been processed.
sub execone {
    my $self = shift;
    my ($cmd) = @_;
    my $err;
    my $fh = new FileHandle;
    # execute on first remote host
    # - find hosts that are up and running
    my %n = $self->status();
    my @up = split(" ", $n{online});
    if (!scalar(@up)) {
	print("No nodes online!\n");
	return undef;
    }
    my $node = $up[0];
    # execute the command
    $self->vprint("execone() : Trying to execute command $cmd on node $node\n");
    open $fh, "ssh $node $cmd |" or undef($fh);
    return $fh;
}

# cexec command on members of subcluster
# returns a filehandle from wher one can read the stdout!!!
sub cexec {
    my $self = shift;
    my ($cmd, $opts) = @_;
    my $fh = new FileHandle;
    if ($self->{scconf}) {
	my $scale = "--all -f " . $self->{scconf};
	$cmd = "$C3PATH/cexec $scale $opts $cmd";
    } else {
	my %c3 = $self->c3members();
	if (scalar(keys(%c3))) {
	    my $s = "";
	    foreach my $c (keys %c3) {
		$s .= $s . " " . $c . ":" . $c3{$c};
	    }
	    $cmd = "$C3PATH/cexec $opts" . $s . " $cmd";
	    # print "Subnode: executing: $cmd\n";
	} else {
	    return undef;
	}
    }
    my $fh = new FileHandle;
    $self->vprint("cexec() : $cmd\n");
    if (!open $fh, "$cmd |") {
	undef($fh);
	carp("cexec returned an error: $!");
    }
    return $fh;
}

# cpush files to members of subcluster
sub cpush {
    my $self = shift;
    my ($src, $tgt) = @_;
    my $cmd;
    if ($self->{scconf}) {
	my $scale = "--all -f " . $self->{scconf};
	$cmd = "$C3PATH/cpush $scale $src $tgt";
    } else {
	my %c3 = $self->c3members();
	if (scalar(keys(%c3))) {
	    my $s = "";
	    foreach my $c (keys %c3) {
		$s .= $s . " " . $c . ":" . $c3{$c};
	    }
	    $cmd = "$C3PATH/cpush " . $s . " $src $tgt";
	}
    }
    $self->vprint("cpush() : $cmd\n");
    return system($cmd);
}

return 1;
