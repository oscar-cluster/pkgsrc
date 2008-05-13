#!/usr/bin/perl
# $Id: autoboot-nodeVM.pl 67 2007-04-18 22:15:52Z gvallee $
#
# Descr:  Automation script to be passed to the OSCAR CLI via 
#         the '--bootscript' option.  
#         This is used to roughly do the following: 
#            - start/signal nodeVMs for network boot/build, and 
#            - then reboot nodeVMs for standard (no PXE cdrom img) boot.  
#            - lastly, once all these nodeVMs have fully rebooted, signal
#               headVM, that it is ok to continue, i.e., exit(0).
#
#
#   Input: Path to V2M node profiles (XML files).
#  Return: SUCCESS (0) or FAILURE (non-zero) [follow standard UNIX semantics]
#

 # Unix semantics regarding success/failure
use constant { UNIX_SUCCESS => 0,
               UNIX_WARNING => 1,    # can be 1..254
               UNIX_FAILURE => 255,
			 };

 # Application semantics regarding error/ok
use constant { APP_ERR => 0,
               APP_OK  => 1,
			 };

#----------------------------------------------------------------------

 # 0) Quick heuristic/sanity checks  (possibly just source a dir of checks?)
 #       - heuristic to check that the nodes will call 'shutdown' 
 #       - verify 'v2m' work (--version)
 #       - verify the VM profiles are valid (--validate)
 #
 # Q: How do we determine if a node will call 'shutdown'?
 #


 # 1) Start the nodeVMs for network boot/build
 # 
 #  Q: What does the input look like?  How do we know what nodes/XML files?
 #
 # v2m <node-with-bootcd_xml_profile>  --create-vm-image-from-cdrom


 # 2) When the node(s) have stopped (shutdown) after building
 #    start the nodeVMs for normal boot.
 #
 # v2m <node_xml_profile> --boot-vm


 # 3) Once all node(s) fully rebooted, signal headVM to continue, 
 #    i.e., exit(0).



exit(UNIX_SUCCESS);
