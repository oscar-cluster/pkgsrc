/* Copyright (c) 2006-2007, Panyong Zhang<pyzhang@gmail.com>.
 *
 * This file is part of libv3m.
 */

/**
 * @file kvm.h
 * @brief Declares all classes for the management of KVM virtual machines.
 * @author Geoffroy Vallee
 *
 * This class defines all the caracteristics of a kvm VM. 
 * Should be created every time we want to manipulate a kvm VM.
 */

#ifndef __KVMVM_H
#define __KVMVM_H

#include <iostream>

#include "ProfileXMLNode.h"
#include "VMContainer.h"

using namespace std;

#define MAX_LINE_LENGHT 128

class Glib::ustring;

class kvmVM : public VirtualMachine{

public:
	kvmVM(ProfileXMLNode* profile);
	kvmVM(ProfileXMLNode* profile, Glib::ustring preCommand, Glib::ustring cmd);
	~kvmVM();
	int boot_vm(); 	/* create a new VM based on information of the
                                 * instantiation */
	int create_image();	/* create a new kvm image */
	int install_vm_from_cdrom ();	/* install the system of the VM,
                                       using an installation CD */
	int install_vm_from_net ();
	int migrate (string node_id);
	int pause ();
	int unpause ();
	int status ();

private:
	Glib::ustring kvmCommand; 	/* command to create a VM (e.g. "kvm" 
                                         * or "kvm-system-x86_64") */
	Glib::ustring preVMCommand; 	/* command to launch kvm (e.g. "sudo
                                         * modprobe kvm") */
	ProfileXMLNode* profile;
    int boot_mode;

	void load_config (); 			/* load kvm configuration from the file ~/.v2m/kvmrc */
	void write_config ();  			/* write kvm configuration to the file ~/.v2m/kvmrc */
	void setCommand(Glib::ustring command);	/* set the command to create a kvm VM */
	void setPreVMCommand(Glib::ustring command);	/* set the command to execute before the creation of a kvm VM,
						   for instance in order to launch the tun kernel module */
	Glib::ustring getCommand();			/* get the command to create a kvm VM */
	Glib::ustring getPreVMCommand(); 		/* set the command to execute before the creation of a kvm VM */
    int generate_bridged_network_config_file ();

	
	void openConfigFile ();
    /* openConfigFile: open the configuration file (~/.v2m/kvmrc),
       deprecated */
	void closeConfigFile ();
    /* close the configuration file (~/.v2m/kvmrc),
       deprecated */
	void readConfig ();
	/* read configuration file and load configuration,
       deprecated */
	void setDefaultValues(); 
    /* setDefaultValues: get default kvm configuration,
       deprecated */
    std::string IntToString(int num);
    int __boot_vm ();
};

#endif
