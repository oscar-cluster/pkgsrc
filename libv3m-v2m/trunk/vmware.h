/* Copyright (c) 2006-2007, Geoffroy Vallee, Oak Ridge National Laboratory.
 *
 * This file is part of libv3m.
 */

/** 
  * @file vmware.h
  * @brief Declares all classes for the management of VMware virtual machines.
  * @author Geoffroy Vallee.
  *
  * This class defines all the caracteristics of a vmware VM. 
  * Should be created every time we want to manipulate a vmware VM.
  */

#ifndef __VMWAREVM_H
#define __VMWAREVM_H

#include <iostream>

#include "ProfileXMLNode.h"
#include "VMContainer.h"

using namespace std;

#define MAX_LINE_LENGHT 128

class Glib::ustring;

class vmware : public VirtualMachine {

public:
	vmware(ProfileXMLNode* profile);
	vmware(ProfileXMLNode* profile, Glib::ustring preCommand, Glib::ustring
cmd);
	~vmware();
	int boot_vm ();
	int create_image();	
	int install_vm_from_cdrom ();
	int install_vm_from_net ();
	int migrate (string node_id);
	int pause ();
	int unpause ();
    int reboot ();
	int status ();

private:
	Glib::ustring vmwareCommand;	/* command to create a VM (e.g. "vmware"
					   or "vmware-system-x86_64") */
	Glib::ustring preVMCommand; 	/* command to launch kvmware (e.g. "sudo
					   modprobe kvmware") */
	Glib::ustring netbootImage;	/* Location of the image the emulation
					   of a netboot */
	ProfileXMLNode* profile;

	void load_config (); 			/* load vmware configuration from the file
~/.v2m/vmwarerc */
	void write_config ();  			/* write vmware configuration to the file
~/.v2m/vmwarerc */
	void setCommand(Glib::ustring command);	/* set the command to create a vmware
VM */
	void setPreVMCommand(Glib::ustring command);	/* set the command to execute
before the creation of a vmware VM,
						   for instance in order to launch the tun kernel module */
	Glib::ustring getCommand();	/* get the command to create a vmware 
	                                   VM */
	Glib::ustring getPreVMCommand();	/* get the command to execute
						   before the creation of a 
						   vmware VM */
	Glib::ustring getNetbootImage();	/* get the image location for 
						   emulation of a netboot */

	int generate_config_file ();/*generate config file for vmware*/
    int generate_config_file_for_bootable_cdrom();/*generate config file to boot from cdrom*/

    int __boot_vm (Glib::ustring config_file);/*boot the vm using the specificed config file*/

	void openConfigFile ();			/* open the configuration file (~/.v2m/vmwarerc)
*/
	void closeConfigFile ();		/* close the configuration file (~/.v2m/vmwarerc)
*/
	void readConfig ();			/* read configuration file and load configuration */
	void setDefaultValues(); 		/* get default vmware configuration */
};

#endif
