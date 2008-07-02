#ifndef __QEMUVM_H
#define __QEMUVM_H

#include <iostream>

#include "ProfileXMLNode.h"
#include "VMContainer.h"

using namespace std;

/** @author Geoffroy Vallee.
  *
  * This class defines all the caracteristics of a qemu VM. 
  * Should be created every time we want to manipulate a qemu VM.
  */

#define MAX_LINE_LENGHT 128

class Glib::ustring;

class qemuVM : public VirtualMachine{

public:
	qemuVM(ProfileXMLNode* profile);
	qemuVM(ProfileXMLNode* profile, Glib::ustring preCommand, Glib::ustring cmd);
	~qemuVM();
	int boot_vm(); 	/* create a new VM based on information of the
                                 * instantiation */
	int create_image();	/* create a new Qemu image */
	int install_vm_from_cdrom ();	/* install the system of the VM,
                                       using an installation CD */
	int install_vm_from_net ();
	int migrate (string node_id);
	int pause ();
	int unpause ();
	int status ();

private:
	Glib::ustring qemuCommand; 	/* command to create a VM (e.g. "qemu" 
                                         * or "qemu-system-x86_64") */
	Glib::ustring preVMCommand; 	/* command to launch kqemu (e.g. "sudo
                                         * modprobe kqemu") */
	ProfileXMLNode* profile;
    int boot_mode;

	void load_config (); 			/* load Qemu configuration from the file ~/.v2m/qemurc */
	void write_config ();  			/* write Qemu configuration to the file ~/.v2m/qemurc */
	void setCommand(Glib::ustring command);	/* set the command to create a qemu VM */
	void setPreVMCommand(Glib::ustring command);	/* set the command to execute before the creation of a qemu VM,
						   for instance in order to launch the tun kernel module */
	Glib::ustring getCommand();			/* get the command to create a qemu VM */
	Glib::ustring getPreVMCommand(); 		/* set the command to execute before the creation of a qemu VM */
    int generate_network_config_file ();

	
	void openConfigFile ();			
    /* openConfigFile: open the configuration file (~/.v2m/qemurc), 
       deprecated */
	void closeConfigFile ();		
    /* close the configuration file (~/.v2m/qemurc),
       deprecated */
	void readConfig ();
	/* read configuration file and load configuration,
       deprecated */
	void setDefaultValues(); 		
    /* setDefaultValues: get default Qemu configuration,
       deprecated */
    std::string IntToString(int num);
    int __boot_vm ();
};

#endif
