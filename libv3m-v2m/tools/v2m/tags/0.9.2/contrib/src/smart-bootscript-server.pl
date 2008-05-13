#!/usr/bin/perl
# $Id: smart-bootscript-server.pl 67 2007-04-18 22:15:52Z gvallee $
#
# Descr: Simple server to run on hostOS and listen for messages from
#        the 'smart-bootscript-client.pl' (see OSCAR-CLI's --bootscript flag).
#
#        A msg from the 'smart-bootscript-client.pl' is the indication that 
#        a nodeVM should be booted for installation.  Once the install is
#        complete and the node has rebooted, and is ready for use, a
#        response is sent to the client so it may continue/exit.
#
#        NOTE: Currently, there are two phases to a nodeVM's install/reboot
#           1) booted with a virtual CD-rom attached for network boot/install,
#              and then it will shutdown (not reboot) so as to indicate
#              completion with this phase of the install.
#          
#           1.5) Check that the virtual HDD image has changed, and we
#                didn't just die or reboot without doing install.
#
#           2) boot again without the CD-rom attached for a normal/local
#              startup off the freshly installed CD-rom.
#              (Don't forget to *not* have '-snapshot' enabled!) :)
#              
#
#  ****
#  NOTE: This version only works with a single node, i.e., 1 connection!!!
#  ****
#
#    FIXME: This version does not yet actually startup the VM's, it just
#           has the stub/comments where the startup should take place.
# 
# Ref: "Programming Perl" 3Ed, p.441

use IO::Socket::INET;

my $server_port = "2345";
my $V2M_PROFILE_ROOT = "/home/tjn/projects/oscar/oscar-testing/v2m-profiles";
my $DEBUG = 1;

print "\n  ***DEBUG MODE ENABLED***\n\n" if($DEBUG);
print "Setup server\n" if($DEBUG);

my $server = IO::Socket::INET->new(LocalPort => $server_port,
                                   Type      => SOCK_STREAM,
                                   Reuse     => 1,
                                   Listen    => SOMAXCONN)
    or die "Error: couldn't start server on port $server_port: $!\n";

print "Listening on port: $server_port\n" if($DEBUG);
my $client = $server->accept();

print "Client connection established\n" if($DEBUG);
my $nodename = <$client>;
chomp($nodename);

print "Got message from client:  $nodename\n";

print "Do work...\n" if($DEBUG);
do_work($nodename);

print "Work done, send notice to client\n" if($DEBUG);
print $client "done.\n";

print "Done.\n" if($DEBUG);
exit(0);




# Input: nodename (maps to a v2m profile filename)
# NOTE: hardcoding the path to all profiles
sub do_work
{
	my $nodename = shift;
	my ($cmd, $v2m_profile);

	 #
	 # 1) Boot node using CDROM version of profile.
	 #
	$v2m_profile  = $V2M_PROFILE_ROOT . "/" . $nodename . "_bootcd.xml";

	die "Error: $! - \'$v2m_profile\'\n" unless( -e $v2m_profile );

	print "Boot #1: $nodename (CD-rom / network boot)\n" if($DEBUG);
	$cmd = "v2m $v2m_profile --create-vm-image-from-cdrom";

	print "DBG: RUN($cmd)\n" if($DEBUG);
	#!system($cmd) or die "Error: v2m failed on Boot#1\n";

	######################################################################
	# TODO 
	#
	# DOH!  Somehow we need to block/wait on this command to return
	#       before we continue...if it blocked that'd work...but it doesn't
	#       so we need some way to get the PID of our v2m and then 
	#       wait on that PID, when it dies, we move to the next step.
	#       Possibly use something like "$pid = pidof $cmd"; wait $pid;
	#       
	######################################################################

	 #
	 # 2) Boot node with NON CD-rom version of profile.
	 #
	$v2m_profile  = $V2M_PROFILE_ROOT . "/" . $nodename . ".xml";

	die "Error: $! - \'$v2m_profile\'\n" unless( -e $v2m_profile );

	print "Boot #2: $nodename (std boot - from VM's local HDD)\n" if($DEBUG);
	$cmd = "v2m $v2m_profile --boot-vm";

	print "DBG: RUN($cmd)\n" if($DEBUG);
	#!system($cmd) or die "Error: v2m failed on Boot#2\n";
	
	######################################################################
	# TODO
	#
	# DOH!  Here we do NOT want to wait for the $cmd to return, we just
	#       want to wait until the node has booted!  So, after starting
	#       the nodeVM, we should use some heurstic like trying to ping/ssh
	#       to the nodeVM (we need an IP for this!) to see if it is ready.
	#       At which point we can return() so the program can continue/exit.
	#
	######################################################################
	
	return(1);
}
