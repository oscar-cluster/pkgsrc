/*
 *  Copyright (c) 2006 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <sstream>
#include <fstream>
#include <iostream>
#include "qemu.h"
#include "ProfileXMLNode.h"
#include "VMSettings.h"
#include "boot.h"

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor.
  *
  * @param p Virtual machine profile data structure.
  * @param preCommand Command to execute before the creation of the virtual
  * machine.
  * @param cmd Command to execution for the creation a virtual machine.
  */
qemuVM::qemuVM(ProfileXMLNode* p, Glib::ustring preCommand, Glib::ustring cmd) 
{
    profile = p;
    preVMCommand = preCommand;
    qemuCommand = cmd;
    boot_mode = NORMAL_BOOT;
}

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor.
  *
  * @param p Virtual machine profile data structure.
  */
qemuVM::qemuVM(ProfileXMLNode* p) 
{
    profile = p;

    // We load configuration information from /etc/v3m/vm.conf
    VMSettings settings;
    cout << "Creation of a new VM instantiation, settings loaded" << endl;
    qemuCommand = settings.getQemuCommand();
    preVMCommand = settings.getQemuPrecommand();
    boot_mode = NORMAL_BOOT;
    cout << "New class instantiation created" << endl;
}

/**
  * @author Geoffroy Vallee.
  *
  * Class destructor.
  */
qemuVM::~qemuVM()
{
}

/**
  * @author Geoffroy Vallee.
  *
  * Just a tool: transforms an int into a string.
  *
  * @param num Integer to transform into string.
  * @return The corresponding string.
  */
std::string qemuVM::IntToString(int num)
{
  std::ostringstream myStream;
  myStream << num << std::flush;

  return(myStream.str()); //returns the string form of the stringstream object
}

/**
  * @author Geoffroy Vallee.
  *
  * Creates a Qemu image. 
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::create_image()
{
    /* We get profile data */
    cout << "Getting profile data..." << endl;
    profile_data_t data = profile->get_profile_data ();

    /* We check if the image location and size exist */
    cout << "Checking the image..." << endl;
    if ((data.image).compare ("N/A") == 0 || (data.image).compare("") == 0) {
      	cerr << "Impossible to create the image, location not found" << endl;
    	return -1;
    }
    if ((data.image_size).compare ("N/A") == 0 || 
        (data.image_size).compare("") == 0) {
      	cerr << "Impossible to create the image, size not found" << endl;
    	return -1;
    }

    /* we get the image location */
    string location = data.image;
    while (location.find("file://") != -1) {
        location.erase(location.find("file://"), 7);
    }
    cout << "file string location: " << location.find("file://") << endl;
    string size = data.image_size;
    Glib::ustring cmd = "qemu-img create " + location + " " + size + "M";
    cout << "Command to create the Qemu image: " << cmd.c_str() << endl;
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        return -1;
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Creates a Qemu image. Location is the image path (including the image name)
  * and size the image size in MB 
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::install_vm_from_cdrom ()
{
    Glib::ustring cmd;

    boot_mode = CDROM_BOOT;

    if (profile == NULL) {
        cerr << "Profile not found" << endl;
        return -1;
    }

    profile_data_t data = profile->get_profile_data ();

    // Check if the cdrom information is specified into the profile
    if ((data.cdrom).compare ("N/A") == 0 && (data.cdrom).compare ("") == 0) {
        cerr << "Impossible to create an image from cdrom, no cdrom seems "
             << "to be specified for the VM" << endl;
        return -1;
    }

    /* We generate the network configuration file */
    generate_bridged_network_config_file ();  

    __boot_vm();

    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Creates the virtual machine image via a network installation.
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::install_vm_from_net ()
{
    boot_mode = NETWORK_BOOT;
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Migrates the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::migrate (string node_id)
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Pauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::pause ()
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Unpauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::unpause ()
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Gets the virtual machine's status.
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::status()
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Sets the command to execute before the creation of a virtual machine.
  *
  * @param cmd Command to execute before the creation of a virtual machine.
  */
void qemuVM::setPreVMCommand(Glib::ustring cmd)
{
  qemuVM::preVMCommand = cmd;
}

/**
  * @author Geoffroy Vallee.
  *
  * Gets the command to execute before the creation of a virtual machine.
  *
  * @return String representing the command to execute before the creation of a
  * QEMU virtual machine.
  */
Glib::ustring qemuVM::getPreVMCommand()
{
  return qemuVM::preVMCommand;
}

/**
  * @author Geoffroy Vallee.
  * 
  * Sets the Qemu command.
  *
  * @param command Command to execute for the creation of a QEMU virutal machine
  */
void qemuVM::setCommand(Glib::ustring command)
{
  qemuVM::qemuCommand = command;
}


/**
  * @author Geoffroy Vallee.
  *
  * Gets the Qemu command.
  *
  * @return String representing the command for the creation of a QEMU virtual
  * machine.
  */
Glib::ustring qemuVM::getCommand()
{
  return qemuVM::qemuCommand;
}

/** @author Geoffroy Vallee.
  * 
  * Generates the network configuration file for QEMU virtual machines.
  *
  * @todo Get the eth0 IP and assign it to the bridge.
  * @todo We should check if the bridge already exists.
  * @return 0 if success, -1 else.
  */
int qemuVM::generate_bridged_network_config_file ()
{
    profile_data_t data = profile->get_profile_data ();
    string filename = "/tmp/qemu-" + data.name + "-ifup.sh";
    string cmd;
    ofstream file_op;
    file_op.open(filename.c_str());
    file_op << "#!/bin/sh\n";
    file_op << "#\n";
    file_op << "# File generated by libv3m, modify only if know exactly what "
            << "you are doing\n";
    file_op << "#\n\n";
    file_op << "# test if the bridge exists\n";
    file_op << "BRIDGE_MAC=`/sbin/ifconfig qemubr0 2>&1 | grep HWaddr | awk ' "
            << "{ print $5 } ' `\n";
    file_op << "if [ x$BRIDGE_MAC != \"x\" ]\nthen\n";
    file_op << "\techo \"The bridge already exists and we support currently "
            << "only one bridge\"\n";
    file_op << "\texit -1\n";
    file_op << "fi\n";
    file_op << "# test if eth0 is a wireless connection or not\n";
    file_op << "WIRELESS=`grep eth0 /proc/net/wireless`\n";
    file_op << "if [ x$WIRELESS != \"x\" ]\nthen\n";
    file_op << "\techo \"The bridge tries to use a wireless interface, it is "
            << "not supported\"\n";
    file_op << "\t exit -1;\n";
    file_op << "fi\n\n";
    file_op << "# get the eth0's IP\n";
    file_op << "IP=`/sbin/ifconfig eth0 | grep \"inet addr\" | awk ' { print "
            << "$2 } ' | sed -e 's/addr://'`\n";
    file_op << "sudo /sbin/ifconfig $1 0.0.0.0\n";
    file_op << "sudo /sbin/ifconfig eth0 0.0.0.0\n";
    file_op << "sudo /usr/sbin/brctl addbr qemubr0\n";
    file_op << "sudo /sbin/ifconfig qemubr0 $IP\n";
    file_op << "sudo /usr/sbin/brctl addif qemubr0 eth0\n";
    file_op << "sudo /usr/sbin/brctl addif qemubr0 $1\n"; 
    file_op.close();
    cmd = "chmod a+x " + filename;
    if (system(cmd.c_str())) {
        cerr << "Error generating the network configuration file" << endl;
        return -1;
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Function called to create a new qemu virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int qemuVM::boot_vm () 
{
    cout << "Create_vm for Qemu" << endl;

    if (profile == NULL) {
        cerr << "Profile not found" << endl;
        return -1;
    }

    /* We generate the network configuration file */
    generate_bridged_network_config_file ();

    __boot_vm ();
}

/**
  * @author Geoffroy Vallee.
  *
  * Boot up a virtual machine, based on a configuration file (low-level
  * function).
  * Private function.
  * Note that boot_vm is the interface exposed to users in order to boot
  * a virtual machine for which the image already exists. This function only
  * call the the command for the creation of a virtual machine.
  *
  * @return 0 if success, -1 else.
  * @todo Revisit the management of virtual disks.
  */
int qemuVM::__boot_vm ()
{
    Glib::ustring cmd;
    profile_data_t data = profile->get_profile_data ();

    /* we first prepare the enviromnent for the VM */
    cmd = getPreVMCommand();
    cout << "Command to execute before the creation of the VM: " << cmd 
         << endl;
    if (system (cmd.c_str())) {
        cerr << "ERROR: Impossible to execute " << cmd << endl;
        return -1;
    }

    /* We get the qemu command */
    cout << "Preparing the command for the VM creation..." << endl;
    cmd = getCommand();
    cmd += "  ";
    cmd += data.image;
    /* We manage the amount of memory */
    cmd += " -m ";
    cmd += data.memory;
    if (boot_mode == CDROM_BOOT) {
        cmd += " -cdrom ";
        cmd += data.cdrom;
        cmd += " -boot d ";
    }
    /* We add virtual disks */
    list<virtual_fs_t>::iterator iter;
    for (iter = (data.list_virtual_fs).begin(); 
         iter != (data.list_virtual_fs).end(); ++iter) 
        cmd = cmd + " -" + (*iter).id + " " + (*iter).location;

    /* we add the network configuration */
    if ((data.nic1_type).compare("N/A") != 0 
        && (data.nic1_type).compare("") != 0) {
        if ((data.nic1_type).compare("BRIDGED_TAP") == 0) {
            cmd += " -net nic -net tap,script=/tmp/qemu-";
            cmd += data.name;
            cmd += "-ifup.sh";
        }
        if ((data.nic1_type).compare("TUN/TAP") == 0) {
            cmd += " -net nic -net tap";
        }
        if ((data.nic1_type).compare("VLAN") == 0) {
            cmd += " -net nic,macaddr=";
            cmd += data.nic1_mac;
            cmd += " -net socket,connect=localhost:1234";
        }
    }
    if ((data.nic2_type).compare("N/A") != 0 
        && (data.nic2_type).compare("") != 0) {
        if ((data.nic2_type).compare("TUN/TAP") == 0) {
            cmd += " -net nic -net tap";
        }
        if ((data.nic2_type).compare("BRIDGED_TAP") == 0) {
            cmd += " -net nic -net tap,script=/tmp/qemu-";
            cmd += data.name;
            cmd += "-ifup.sh";
        }
        // WARNING: the VLAN master cannot be nic2
        if ((data.nic2_type).compare("VLAN") == 0) {
            cmd += " -net nic,macaddr=";
            cmd += data.nic2_mac;
            cmd += " -net socket,listen=localhost:1234";
        }
    }

    cmd += " &";
    while (cmd.find("\n") != -1) {
        cout << cmd.find("\n") << endl;
        cmd.erase(cmd.find("\n"), 1);
    }
    while (cmd.find("file://") != -1) {
        cmd.erase(cmd.find("file://"), 7);
    }
    cout << "Creating the qemu virtual machine:" << cmd << endl;
    if (system (cmd.c_str())) {
  	    cerr << "ERROR: Impossible to execute " << cmd << endl;
    	return -1;
    }

    return 0;
}
