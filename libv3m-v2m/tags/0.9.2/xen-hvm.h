#ifndef __XENHVM_H
#define __XENHVM_H

#include <iostream>

#include "ProfileXMLNode.h"
#include "VMContainer.h"

/** Author: Geoffroy Vallee.
  *
  * This class defines all the caracteristics of a xen VM using hardware
  * virtualization support (HVM, i.e., Intel-VT or AMD-V). 
  * Should be created every time we want to manipulate a xen VM.
  */

#define MAX_LINE_LENGHT 128

using namespace std;

class Glib::ustring;

class xen_hvm : public VirtualMachine {

public:
    /** net_script1 is a script that can be used for networking */
    string net_script1; 

    /** net_script2 is a second script that can be used for networking */
    string net_script2;

    /** unbridge_nic1 is a flag to know if the nic1 has to be bridged or not.
      * 0 means that the nic should be bridged, 1 that the nics should not */
    int unbridge_nic1; 

    /** unbridge_nic2 is a flag to know if the nic2 has to be bridged or not. 
      * 0 means that the nic should be bridged, 1 that the nics should not */
    int unbridge_nic2;

	xen_hvm(ProfileXMLNode* profile);
	xen_hvm(ProfileXMLNode* profile, Glib::ustring preCommand, Glib::ustring
cmd);
	~xen_hvm();
	int boot_vm();
	int create_image();	
    int install_vm_from_cdrom ();
	int install_vm_from_net ();
    int migrate (string destination_id);
    int pause ();
    int unpause ();
    int status ();

private:
    int boot_mode;
	Glib::ustring xenCommand;	/* command to create a VM (e.g. "xen"
					   or "xen-system-x86_64") */
	Glib::ustring preVMCommand; 	/* command to launch kxen (e.g. "sudo
					   modprobe kxen") */
	Glib::ustring netbootImage;	/* Location of the image the emulation
					   of a netboot */
	ProfileXMLNode* profile;

	void load_config (); 			/* load xen configuration from the file
~/.v2m/xenrc */
	void write_config ();  			/* write xen configuration to the file
~/.v2m/xenrc */
	void setCommand(Glib::ustring command);	/* set the command to create a xen VM
*/
	void setPreVMCommand(Glib::ustring command);	/* set the command to execute
before the creation of a xen VM,
						   for instance in order to launch the tun kernel module */
	Glib::ustring getCommand();	/* get the command to create a xen 
	                                   VM */
	Glib::ustring getPreVMCommand();	/* get the command to execute
						   before the creation of a 
						   xen VM */
	Glib::ustring getNetbootImage();	/* get the image location for 
						   emulation of a netboot */
	
	void openConfigFile ();			/* open the configuration file (~/.v2m/xenrc) */
	void closeConfigFile ();		/* close the configuration file (~/.v2m/xenrc) */
	void readConfig ();			/* read configuration file and load configuration */
	void setDefaultValues(); 		/* get default xen configuration */

    string generate_script_unbridge_nic (int);
    int check_xen_net_config ();
    int generate_config_file ();
    int generate_config_file_for_bootable_cdrom();
    int __boot_vm(Glib::ustring);
};

#endif
