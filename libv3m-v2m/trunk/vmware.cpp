/*
 *  Copyright (c) 2006-2007 Oak Ridge National Laboratory, 
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the libv3m software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <fstream>
#include <iostream>
#include "vmware.h"
#include "ProfileXMLNode.h"
#include "VMSettings.h"
#include "vm_status.h"

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor.
  *
  * @param p Virtual machine's profile (data structure).
  * @param preCommand Command to execute before the creation of the virtual
  *        machine (string).
  * @param cmd Command for the creation of the virtual machine.
  */
vmware::vmware(ProfileXMLNode* p, Glib::ustring preCommand, Glib::ustring
cmd) 
{
  profile = p;
  preVMCommand = preCommand;
  vmwareCommand = cmd;
}

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor.
  *
  * @param p Virtual machine's profile (data structure).
  */
vmware::vmware(ProfileXMLNode* p) 
{
  profile = p;

  // We load configuration information from /etc/v3m/vm.conf
  VMSettings settings;
  vmwareCommand = settings.getVmwareCommand();
  preVMCommand = settings.getVmwarePrecommand();
  netbootImage = settings.getNetbootImage();
}

/**
  * @author Geoffroy Vallee.
  *
  * Class destructor.
  */
vmware::~vmware()
{
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Creates a vmware image. Location is the image path (including the image
  * name) and size the image size in MB.
  *
  * @return 0 if success, -1 else.
  */
int vmware::create_image()
{
    /* We get profile data */
    profile_data_t data = profile->get_profile_data ();

    /* We check if the image location and size exist */
    if ((data.image).compare ("N/A") == 0 || (data.image).compare("") == 0) {
        cerr << "Impossible to create the image, location not found ("
             << data.image << ")" << endl;
        return -1;
    }
    if ((data.image_size).compare ("N/A") == 0 ||
        (data.image_size).compare("") == 0) {
        cerr << "Impossible to create the image, size not found" << endl;
        return -1;
    }

    /* we get the image location */
    string location = data.image;
    string size = data.image_size;

    /* We create the image, Currently we use lsilogic*/
    Glib::ustring cmd = "sudo /usr/bin/vmware-vdiskmanager -c -s " + size
                        + "Mb -a lsilogic -t 0 " + location;
    cout << "Command to create the vmware image: " << cmd.c_str() << endl;
    if (system (cmd.c_str())) {
        cout << "ERROR executing " << cmd << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Creates a VMware image. Location is the image path (including the image
  * name) and size the image size in MB 
  *
  * @return 0 if success, -1 else.
  */
int vmware::install_vm_from_cdrom ()
{
    profile_data_t data = profile->get_profile_data ();

    if (vmware::generate_config_file_for_bootable_cdrom()) {
        cerr << "ERROR: Impossible to generate the config file in order to \
                 install the VM using a bootable CDROM" << endl;
        return -1;
    }

    /* The configuration file is created, we can boot the VM up */
    if (vmware::__boot_vm ("/tmp/" + data.name + "_vmware.vmx")) {
        cerr << "ERROR impossible to boot the VM up" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Creates a vmware image from a network installation, using OSCAR.
  *
  * @return 0 if success, -1 else.
  */
int vmware::install_vm_from_net ()
{
	Glib::ustring cmd, mac_addr;
	Glib::ustring netboot_image = getNetbootImage ();

	profile_data_t data = profile->get_profile_data ();

	cout << "Netboot image: " << netboot_image << endl;
	cout << "Network configuration: " << data.nic1_mac << " - " << data.nic2_mac << endl;

	/* We check that the VM has at least one network connection */
	/* we take the first mac as network interface for network boot */
	if ((data.nic1_mac).compare ("N/A") == 0 || 
        (data.nic1_mac).compare ("") == 0) {
    		if ((data.nic2_mac).compare ("N/A") == 0 ||
                (data.nic2_mac).compare ("") == 0) {
			    cerr << "ERROR: the VM does not have a NIC, network \
                         installation impossible" << endl;
    		} else {
	    		mac_addr = data.nic2_mac;
		    }
	} else {
		mac_addr = data.nic1_mac;
	}

    vmware::install_vm_from_cdrom();

	return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Migrates the virtual machine to a remote node.
  *
  * @return 0 if success, -1 else.
  */
int vmware::migrate (string node_id)
{
    std::cerr << "ERROR: not yet supported" << std::endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Pauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int vmware::pause ()
{
    Glib::ustring cmd;
    profile_data_t data = profile->get_profile_data ();

    cmd += "sudo /usr/bin/vmrun suspend /tmp/" + data.name + "_vmware.vmx";
    std::cout << "Executing: " << cmd << std::endl;
    if (system (cmd.c_str())) {
        std::cerr << "ERROR executing: " << cmd << std::endl;
        cerr << "ERROR pause the VMWare VM" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Unpauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int vmware::unpause ()
{
    Glib::ustring cmd;
    profile_data_t data = profile->get_profile_data ();

    cmd += "sudo /usr/bin/vmrun start /tmp/" + data.name + "_vmware.vmx";
    std::cout << "Executing: " << cmd << std::endl;
    if (system (cmd.c_str())) {
        std::cerr << "ERROR executing: " << cmd << std::endl;
        cerr << "ERROR unpause the VMWare VM" << endl;
        return -1;
    }

    return 0;
}

/**
 * @author Panyong Zhang
 *
 * Reboot the virtual machine.
 *
 * @return 0 if success, -1 else.
 */
int vmware::reboot ()
{
    Glib::ustring cmd;
    profile_data_t data = profile->get_profile_data ();

    cmd += "sudo /usr/bin/vmrun reset /tmp/" + data.name + "_vmware.vmx hard";
    std::cout << "Executing: " << cmd << std::endl;
    if (system (cmd.c_str())) {
        std::cerr << "ERROR executing: " << cmd << std::endl;
        cerr << "ERROR Reboot the VMWare VM" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Sets the command to execute before the creation of the virtual machine.
  *
  * @param cmd Command to execute before the creation of the virtual machine.
  */
void vmware::setPreVMCommand(Glib::ustring cmd)
{
  vmware::preVMCommand = cmd;
}

/**
  * @author Geoffroy Vallee.
  *
  * Gets the command to execute before the creation of the virtual machine.
  *
  * @return The command to execute before the creation of the virtual machine
  *         (string).
  */
Glib::ustring vmware::getPreVMCommand()
{
  return vmware::preVMCommand;
}

/**
  * @author Geoffroy Vallee.
  * 
  * Sets the vmware command.
  *
  * @param command Command for the creation of the virtual machine.
  */
void vmware::setCommand(Glib::ustring command)
{
  vmware::vmwareCommand = command;
}


/**
  * @author Geoffroy Vallee.
  *
  * Gets the vmware command.
  */
Glib::ustring vmware::getCommand()
{
  return vmware::vmwareCommand;
}


/**
  * @author Geoffroy Vallee.
  *
  * Get the image location used for netboot emulation. Used for system 
  * installation within the VM using OSCAR.
  *
  * @return the location is available, an empty string else.
  */
Glib::ustring vmware::getNetbootImage ()
{
	return vmware::netbootImage;
}

/**
  * @author Panyong Zhang.
  *
  * Generate the configuration file in order to be able to boot on the CDROM.
  *
  * @return: 0 is success, -1 else.
  */
int vmware::generate_config_file_for_bootable_cdrom ()
{
    profile_data_t data = profile->get_profile_data ();

    /* Check if the cdrom information is specified into the profile */
    if ((data.cdrom).compare ("N/A") == 0 && (data.cdrom).compare ("") == 0) {
        cerr << "Impossible to create an image from cdrom, no cdrom seems "
             << "to be specified for the VM" << endl;
        return -1;
    }

    /* We first generate a basic configuration file */
    cout << "We generate the basic config file for the VM" << endl;
    if (generate_config_file ()) {
        cerr << "Impossible to generate the basic configuration file" << endl;
        cerr << "Impossible to generate the configuration file to the use "
             << "of a bootable CDROM\n" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Function that create a basic configuration file.
  * Private function.
  *
  * @return: 0 if success, -1 else.
  */
int vmware::generate_config_file ()
{
    profile_data_t data = profile->get_profile_data ();

    /* We create the vmware configuration file. This file is saved in /tmp */
    string filename = "/tmp/" + data.name + "_vmware.vmx";
    ofstream file_op;
    file_op.open(filename.c_str());
    file_op << "config.version=\"8\"\n";
    file_op << "virtualHW.version=\"4\"\n";
    file_op << "guestOS= \"otherlinux-64\"\n";
    file_op << "workingDir = \".\"\n";
    file_op << "displayName=\"" + data.name + "\"\n";
    file_op << "memsize = " << data.memory << "\n";
    /*we use the SCSI disk for demo*/
    file_op << "scsi0:0.present = \"TRUE\"\n";
    file_op << "scsi0.virtualDev = \"lsilogic\"\n";
    file_op << "scsi0:0.fileName = \"" + data.image + "\"\n";

    /*config the CDROM*/
    if (!((data.cdrom).compare ("N/A") == 0 
        && (data.cdrom).compare ("") == 0)) {
        file_op << "ide1:0.present = \"TRUE\"\n";
        file_op << "ide1:0.fileName = \"" + data.cdrom + "\"\n";
        file_op << "ide1:0.deviceType = \"cdrom-image\"\n";
    } else {
    }

    /* Network setup: first we deal with nic1 */
    if ((data.nic1_type).compare ("BRIDGED_TAP") == 0){
        file_op << "Ethernet0.present = \"TRUE\"\n";
        file_op << "Ethernet0.virtualDev = \"e1000\"\n";
        file_op << "ethernet0.addressType = \"generated\"\n";
        /*setup the MAC address*/
        file_op << "ethernet0.generatedAddress = \""+ data.nic1_mac + "\"\n";
        /*setup the Network type to Bridge*/
        file_op << "Ethernet0.connectionType = \"bridged\"\n";
    }
    if ((data.nic1_type).compare ("NAT") == 0) {
        file_op << "Ethernet0.present = \"TRUE\"\n";
        file_op << "Ethernet0.virtualDev = \"e1000\"\n";
        file_op << "ethernet0.addressType = \"generated\"\n";
        file_op << "ethernet0.generatedAddress = \""+ data.nic1_mac + "\"\n";
        file_op << "Ethernet0.connectionType = \"nat\"\n";
    }
    if ((data.nic1_type).compare ("HOST_ONLY") == 0) {
        file_op << "Ethernet0.present = \"TRUE\"\n";
        file_op << "Ethernet0.virtualDev = \"e1000\"\n";
        file_op << "ethernet0.addressType = \"generated\"\n";
        file_op << "ethernet0.generatedAddress = \""+ data.nic1_mac + "\"\n";
        file_op << "Ethernet0.connectionType = \"hostonly\"\n";
    }

    /* Network setup: first we deal with nic2 */
    if ((data.nic2_type).compare ("BRIDGED_TAP") == 0){
        file_op << "Ethernet1.present = \"TRUE\"\n";
        file_op << "Ethernet1.virtualDev = \"e1000\"\n";
        file_op << "ethernet1.addressType = \"generated\"\n";
        file_op << "ethernet1.generatedAddress = \""+ data.nic2_mac + "\"\n";
        file_op << "Ethernet1.connectionType = \"bridged\"\n";
    }
    if ((data.nic2_type).compare ("NAT") == 0) {
        file_op << "Ethernet1.present = \"TRUE\"\n";
        file_op << "Ethernet1.virtualDev = \"e1000\"\n";
        file_op << "ethernet1.addressType = \"generated\"\n";
        file_op << "ethernet1.generatedAddress = \""+ data.nic2_mac + "\"\n";
        file_op << "Ethernet1.connectionType = \"nat\"\n";
    }
    if ((data.nic2_type).compare ("HOST_ONLY") == 0) {
        file_op << "Ethernet1.present = \"TRUE\"\n";
        file_op << "Ethernet1.virtualDev = \"e1000\"\n";
        file_op << "ethernet1.addressType = \"generated\"\n";
        file_op << "ethernet1.generatedAddress = \""+ data.nic2_mac + "\"\n";
        file_op << "Ethernet1.connectionType = \"hostonly\"\n";
    }

    /*Virtual CPU Number*/
    file_op << "numvcpus = \"2\"\n";

    file_op.close();

    /*change the vmx file execute mode for VMWare play*/
    Glib::ustring cmd;
    cmd = "sudo /bin/chmod +x " + filename;
    std::cout << "Executing: " << cmd << std::endl;
    if (system (cmd.c_str())) {
        std::cerr << "ERROR executing: " << cmd << std::endl;
        cerr << "ERROR change the execute mode for vmware config file" << endl;
        return -1;
    }

    std::cout << "Configuration file created: " << filename << std::endl;

    return 0;
}

/** 
  * @author Panyong Zhang.
  *
  * Boot up a virtual machine, based on a configuration file (low-level
  * function).
  * Private function.
  * Note that boot_vm is the interface exposed to users in order to boot
  * a virtual machine for which the image already exists. This function only
  * call the command for the creation of a virtual machine.
  */
int vmware::__boot_vm (Glib::ustring config_file)
{
    Glib::ustring cmd;

    cmd = getCommand ();
    cmd += " ";
    cmd += config_file;
    std::cout << "Executing: " << cmd << std::endl;
    if (system (cmd.c_str())) {
        std::cerr << "ERROR executing: " << cmd << std::endl;
        cerr << "ERROR booting the VM" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Function called to create a new vmware virtual machine.
  */
int vmware::boot_vm () 
{
    Glib::ustring cmd;

    std::cout << "Create_vm for VMWare" << std::endl;

    if (profile == NULL) {
        std::cerr << "Profile not found" << std::endl;
        return -1;
    }

    profile_data_t data = profile->get_profile_data ();

    if (generate_config_file ()) {
        cerr << "Error generating the VMWARE configuration file" << endl;
        return -1;
    }

    if (__boot_vm ("/tmp/" + data.name + "_vmware.vmx")) {
        cerr << "ERROR booting the VM" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  * @author Panyong Zhang.
  *
  * Get the VMWare's status
  */
int vmware::status ()
{
    int pos,status = -1;
    string line;
    string cmd = "sudo /usr/bin/vmrun list> /tmp/vmware_status.tmp ";

    profile_data_t data = profile->get_profile_data ();
    string vmname = "/tmp/" + data.name + "_vmware.vmx";

    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        exit(-1);
    }

    ifstream vmware_status_file("/tmp/vmware_status.tmp");

    if(!vmware_status_file.is_open()){
        cout << "Unable to open file";
    } else {
        status = UNKNOWN;
        while (!vmware_status_file.eof()){
            getline (vmware_status_file,line);
            if (vmname.compare(line) == 0){
                status = RUNNING;
            }
        }
    }
    vmware_status_file.close();

    cmd = "rm -f /tmp/vmware_status.tmp ";
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        exit(-1);
    }

    return status;

}

