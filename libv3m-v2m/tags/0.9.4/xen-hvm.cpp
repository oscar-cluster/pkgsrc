/*
 *  Copyright (c) 2006 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the libv3m software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include "xen-hvm.h"
#include "ProfileXMLNode.h"
#include "VMSettings.h"
#include "boot.h"

/**
  * @author Geoffroy Vallee.
  */
xen_hvm::xen_hvm(ProfileXMLNode* p, Glib::ustring preCommand, Glib::ustring
cmd) 
{
    profile = p;
    preVMCommand = preCommand;    
    xenCommand = cmd;
    unbridge_nic1 = 0;
    unbridge_nic2 = 0;
    boot_mode = NORMAL_BOOT;
}

/**
  * @author Geoffroy Vallee.
  */
xen_hvm::xen_hvm(ProfileXMLNode* p) 
{
    profile = p;

    // We load configuration information from /etc/v3m/vm.conf
    cout << "Creating a new Xen VM\n" << endl;
    VMSettings settings;
    xenCommand = settings.getXenCommand();
    preVMCommand = settings.getXenPrecommand();
    netbootImage = settings.getNetbootImage();
    unbridge_nic1 = 0;
    unbridge_nic2 = 0;
    boot_mode = NORMAL_BOOT;
}

/**
  * @author Geoffroy Vallee.
  *
  * Class destructor.
  */
xen_hvm::~xen_hvm()
{
}

/**
  * @author Geoffroy Vallee.
  *
  * Create a Xen image. Location is the image path (including the image name)
  * and size the image size in MB.
  *
  * @return: 0 if success, -1 else.
  */
int xen_hvm::create_image()
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
    Glib::ustring cmd = "dd if=/dev/zero of=" + location + " bs=1M count=" +
size;
    cout << "Command to create the Xen image: " << cmd.c_str() << std::endl;
    if (system (cmd.c_str())) {
        cout << "ERROR executing " << cmd << endl;
        return -1;
    }
    cmd = "/sbin/mke2fs -F -j -T ext2 " + location;
    cout << "Command to format the Xen image: " << cmd << endl;
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Generate the configuration file in order to be able to boot on the CDROM.
  *
  * @return: 0 is success, -1 else.
  */
int xen_hvm::generate_config_file_for_bootable_cdrom ()
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
  *
  * Create a Xen image. Location is the image path (including the image name)
  * and size the image size in MB.
  *
  * @return: 0 if sucess, -1 else.
  */
int xen_hvm::install_vm_from_cdrom ()
{
    boot_mode = CDROM_BOOT;
    profile_data_t data = profile->get_profile_data ();

    if (xen_hvm::generate_config_file_for_bootable_cdrom()) {
        cerr << "ERROR: Impossible to generate the config file in order to \
                 install the VM using a bootable CDROM" << endl;
        return -1;
    }

    /* The configuration file is created, we can boot the VM up */
    if (xen_hvm::__boot_vm ("/tmp/" + data.name + "_xen.cfg")) {
        cerr << "ERROR impossible to boot the VM up" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Create a Xen image from a network installation, using OSCAR.
  * Note that currently with the Xen full-virtualization, this is done via a
  * bootable CDROM.
  *
  * @return: 0 if success, -1 else.
  */
int xen_hvm::install_vm_from_net ()
{
	Glib::ustring cmd, mac_addr;
	Glib::ustring netboot_image = getNetbootImage ();

    boot_mode = NETWORK_BOOT;

	profile_data_t data = profile->get_profile_data ();

	cout << "Netboot image: " << netboot_image << endl;

	cout << "Network configuration: " << data.nic1_mac << " - " 
<< data.nic2_mac << endl;
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

    xen_hvm::install_vm_from_cdrom();

	return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Migrate a virtual machine.
  *
  * @return: 0 is success, -1 else.
  */
int xen_hvm::migrate (string destination_id)
{
    cout << "Migrating VM to " << destination_id << endl;

    /* first we check that the destination id is not empty */
    if (destination_id.compare("") == 0) {
        cerr << "ERROR destination ID invalid" << endl;
        return -1;
    }

    /* we get the VM's name */
    profile_data_t data = profile->get_profile_data ();

    /* we then migrate the VM */
    string cmd = "xm migrate " + data.name + " " + destination_id;
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        return -1;
    }

    return 0;
}

int xen_hvm::check_xen_net_config ()
{
    // We test first few commands
    std::string cmd;
    fstream fin, fin2;
    fin.open("/sbin/ip",ios::in);
    fin2.open("/bin/ip",ios::in);
    if( !fin.is_open() && !fin2.is_open()) {
        cout << "The 'ip' command seems to not be available, it is not ";
        cout << "possible to have any virtual network";
        return -1;
    } 
    fin.close();
    fin2.close();
    fin.open("/usr/sbin/brctl",ios::in);
    if ( !fin.is_open() ) {
        cout << "The 'brctl command seems to not available, it is not ";
        cout << "possible to have any virtual network";
        return -1;
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  * 
  * Pause a virtual machine.
  *
  * @return 0 is success, -1 else.
  */
int xen_hvm::pause ()
{
    cout << "Pausing VM" << endl;

    /* we get the VM's name */
    profile_data_t data = profile->get_profile_data ();

    /* we then pause the VM */
    string cmd = "xm pause " + data.name;
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Unpause a virtual machine.
  *
  * @return 0 is success, -1 else.
  */
int xen_hvm::unpause ()
{
    cout << "Unpausing a VM" << endl;

    /* we get the VM's name */
    profile_data_t data = profile->get_profile_data ();

    /* we then unpause the VM */
    string cmd = "xm unpause " + data.name;
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        return -1;
    }

    return 0;
}


/**
  * @author Geoffroy Vallee.
  *
  * Set the command to execute before the creation of a virtual machine.
  */
void xen_hvm::setPreVMCommand(Glib::ustring cmd)
{
  xen_hvm::preVMCommand = cmd;
}

/**
  * @author Geoffroy Vallee.
  *
  * Get the command to execute before the creation of a virtual machine.
  */
Glib::ustring xen_hvm::getPreVMCommand()
{
  return xen_hvm::preVMCommand;
}

/**
  * @author Geoffroy Vallee.
  * 
  * Set the Xen command.
  */
void xen_hvm::setCommand(Glib::ustring command)
{
  xen_hvm::xenCommand = command;
}


/**
  * @author Geoffroy Vallee.
  *
  * Get the Xen command.
  */
Glib::ustring xen_hvm::getCommand()
{
  return xen_hvm::xenCommand;
}


/**
  * @author Geoffroy Vallee.
  *
  * Get the image location used for netboot emulation. Used for system 
  * installation within the VM using OSCAR.
  *
  * @return the location is available, an empty string else.
  */
Glib::ustring xen_hvm::getNetbootImage ()
{
	return xen_hvm::netbootImage;
}


/**
  * @author Geoffroy Vallee.
  *
  * Generate the script that will unbridged a nic (bridge created by default by
  * Xen).
  *
  * @param nic_id NIC id (0 for the first NIC, 1 for the second NIC).
  * @return script path if success.
  */
string xen_hvm::generate_script_unbridge_nic (int nic_id) 
{
    ostringstream myStream;
    myStream << nic_id; // convert int to string
    string nicId = myStream.str();
    profile_data_t data = profile->get_profile_data ();
    string f = "/tmp/" + data.name + "_unbridge_nic" + nicId;
    ofstream script;
    script.open(f.c_str());
    script << "#!/bin/sh\n";
    script << "#\n\n";
    script << "# Get the domain id\n";
    script << "DOMAIN_ID=`xm list | grep " << data.name << " | \
awk '{ print $2}'`\n";
    script << "# we assume the bridge is xenbr0\n";
    script << "/usr/sbin/brctl delif xenbr0 vif${DOMAIN_ID}." << nicId << "\n";
    script << "# we do not give an IP to the unbridged interface, do not\n";
    script << "# really know how to deal with that.\n";
    script.close();
    string cmd = "chmod a+x " + f;
    if (system (cmd.c_str())) {
        cerr << "ERROR creating a Xen network configuration script" << endl;
        exit (-1);
    }
    return f;
}

/**
  * @author Geoffroy Vallee.
  *
  * Function that create a basic configuration file.
  * Private function.
  *
  * @return: 0 if success, -1 else.
  */
int xen_hvm::generate_config_file ()
{
    profile_data_t data = profile->get_profile_data ();

    /* We create the Xen configuration file. This file is saved in /tmp */
    string filename = "/tmp/" + data.name + "_xen.cfg";
    ofstream file_op;
    file_op.open(filename.c_str());
    file_op << "import os, re\n";
    file_op << "arch = os.uname()[4]\n";
    file_op << "if re.search('64', arch):\n";
    file_op << "\tarch_libdir = 'lib64'\n";
    file_op << "else:\n";
    file_op << "\tarch_libdir = 'lib'\n\n";
    file_op << "name = \"" << data.name <<"\"\n";
    file_op << "kernel = \"/usr/lib/xen/boot/hvmloader\"\n";
    file_op << "builder='hvm'\n";
    file_op << "shadow_memory = 8\n";
    file_op << "memory = " << data.memory << "\n";
    file_op << "disk = ['file:" << data.image << ",hda,w'";
    if (!((data.cdrom).compare ("N/A") == 0 
        && (data.cdrom).compare ("") == 0)) {
        file_op << ", 'file:" << data.cdrom << ",hdc:cdrom,r']\n";
    } else {
        file_op << "];\n";
    }
    if (boot_mode == CDROM_BOOT) {
        file_op << "boot='d'\n";
    }
    /* Network setup: first we deal with nic1 */
    if ((data.nic1_type).compare ("BRIDGED_TAP") == 0)
        file_op << "vif = [ 'mac=" + data.nic1_mac + "' ]\n";
    if ((data.nic1_type).compare ("TUN/TAP") == 0 
        || (data.nic1_type).compare ("VLAN") == 0) {
        // We declare the NIC
        if (check_xen_net_config ()) {
            cerr << "Tools are missing in order to setup the network";
            return -1;
        }
        file_op << "vif = [ 'type=ioemu, mac=" + data.nic1_mac 
                   + ", bridge=xenbr0' ]\n";
        // We generate the script to modify the bridge automatically created
        // by Xen
        net_script1 = generate_script_unbridge_nic (0); // 0 because it is the
                                                        // first NIC
        unbridge_nic1 = 1;
    }
    if ((data.nic2_type).compare ("BRIDGED_TAP") == 0) {
       if (check_xen_net_config ()) {
            cerr << "Tools are missing in order to setup the network";
            return -1;
        }
        file_op << "vif = [ 'mac=" + data.nic2_mac + "' ]\n";
    }
    if ((data.nic2_type).compare ("TUN/TAP") == 0 ||
        (data.nic2_type).compare ("VLAN") == 0) {
            if (check_xen_net_config ()) {
                cerr << "Tools are missing in order to setup the network";
                return -1;
            }
            // We declare the NIC
            file_op << "vif = [ 'mac=" + data.nic2_mac + "' ]\n";
            // We generate the script to modify the bridge automatically created
            // by Xen
            net_script2 = generate_script_unbridge_nic (1); // 1 because it is
                                                            // the second NIC
            unbridge_nic2 = 1;
    }
    file_op << "device_model = '/usr/' + arch_libdir + '/xen/bin/qemu-dm'\n";
    file_op << "sdl=1\nvnc=0\nstdvga=0\nserial='pty'\n";
    file_op.close();

    std::cout << "Configuration file created: " << filename << std::endl;

    return 0;
}

/** 
  * @author Geoffroy Vallee.
  *
  * Boot up a virtual machine, based on a configuration file (low-level
  * function).
  * Private function.
  * Note that boot_vm is the interface exposed to users in order to boot
  * a virtual machine for which the image already exists. This function only
  * call the command for the creation of a virtual machine.
  */
int xen_hvm::__boot_vm (Glib::ustring config_file)
{
    Glib::ustring cmd;

    cmd = getCommand ();
    cmd += " ";
    cmd += config_file;
    std::cout << "Executing: " << cmd << std::endl;
    if (system (cmd.c_str())) {
        std::cerr << "ERROR executing: " << cmd << std::endl;
        return -1;
    }
    if (unbridge_nic1 = 1)
        if (system (net_script1.c_str())) {
            cerr  << "ERROR setting the network up" << endl;
            return -1;
        }
    if (unbridge_nic2 = 1)
        if (system (net_script2.c_str())) {
            cerr  << "ERROR setting the network up" << endl;
            return -1;
        }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  * 
  * Function called to create a new xen VM.
  * Metwork configuration:
  * - if BRIDGED_TAP, nothing to do, default Xen behavior,
  * - if TUN/TAP, the NIC has to be removed from the bridge created by Xen
  * - if VLAN, same thing.
  *
  * @return: 0 if success, -1 else.
  */
int xen_hvm::boot_vm () 
{
    Glib::ustring cmd;
//    int unbridge_nic1 = 0;
//    int unbridge_nic2 = 0;
//    string net_script1, net_script2;

    std::cout << "Create_vm for Xen" << std::endl;

    if (profile == NULL) {
        std::cerr << "Profile not found" << std::endl;
        return -1;
    }

    profile_data_t data = profile->get_profile_data ();

    if (generate_config_file ()) {
        cerr << "Error generating the Xen configuration file" << endl;
        return -1;
    }

    if (__boot_vm ("/tmp/" + data.name + "_xen.cfg")) {
        cerr << "ERROR booting the VM" << endl;
        return -1;
    }

    return 0;
}

/**
  * @author Geoffroy Vallee.
  */
int xen_hvm::status()
{
    cerr << "ERROR: not yet implemented" <<endl;
    return -1;
}

