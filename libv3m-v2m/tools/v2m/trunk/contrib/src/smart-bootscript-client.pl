#!/usr/bin/perl
# $Id: smart-bootscript-client.pl 67 2007-04-18 22:15:52Z gvallee $
#
# Descr: The 'smart' bootscript, to be used as the argument to the
#        OSCAR-CLI's '--bootscript' option.   
#
#   ****
#   NOTE: This version only works with one, hardcoded nodename, $oscar_node.
#   ****
#
# It does the following:
#    1) sends a message to the $remote_host:$remote_port to
#       tell it to boot a node.  The msg sent will be the nodename.
#       which will ultimately directly map to the V2M profile filename, 
#       i.e., oscarnode1 => oscarnode1.xml, 
#             node1.oscardomain => node1.oscardomain.xml
#  
#    2) it then blocks, waiting for a response from the server indicating
#       the node is been fully installed.
#
#    3) Last we close connection and exit so we can then continue 
#       the OSCAR-CLI installation, i.e., exit(0).
#  
#
# Ref: "Programming Perl" 3Ed, p.439

use IO::Socket::INET;

my $remote_host = "172.20.0.1";
my $remote_port = "2345";
my $oscar_node = "oscarnode1";
my $DEBUG = 1;

my $socket = IO::Socket::INET->new(PeerAddr  => $remote_host,
                                   PeerPort  => $remote_port,
                                   Proto     => "tcp",
                                   Type      => SOCK_STREAM)
    or die "Error: couldn't connect to $remote_host:$remote_port : $!\n";


 # 1) Send nodename.
my $msg = "$oscar_node";

print "Send: $msg\n" if($DEBUG);
print $socket "$msg\n";

print " ...wait for response from server...\n" if($DEBUG);

 # 2) Block waiting for server response, indicating node is ready.
my $response = <$socket>;
print "Recv: $response\n" if($DEBUG);

 # 3) Tear down socket and exit to continue to next OSCAR-CLI step.
close($socket);
exit(0);
