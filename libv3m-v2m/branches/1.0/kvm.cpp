/*
 *  Copyright (c) 2008, Panyong Zhang <pyzhang@gmail.com>
 *                     All rights reserved
 *  Copyright (c) 2008, Geoffroy Vallee <valleegr@ornl.gov>
 *                      All rights reserved
 *  This file is part of the libv3m software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <sstream>
#include <fstream>
#include <iostream>
#include "kvm.h"
#include "ProfileXMLNode.h"
#include "VMSettings.h"
#include "vm_status.h"
#include "boot.h"

/**
  * @author Panyong Zhang.
  *
  * Class constructor.
  *
  * @param p Virtual machine profile data structure.
  * @param preCommand Command to execute before the creation of the virtual
  * machine.
  * @param cmd Command to execution for the creation a virtual machine.
  */
kvmVM::kvmVM(ProfileXMLNode* p, Glib::ustring preCommand, Glib::ustring cmd) 
{
    profile = p;
    preVMCommand = preCommand;
    kvmCommand = cmd;
    boot_mode = NORMAL_BOOT;
}

/**
  * @author Panyong Zhang.
  *
  * Class constructor.
  *
  * @param p Virtual machine profile data structure.
  */
kvmVM::kvmVM(ProfileXMLNode* p) 
{
    profile = p;

    // We load configuration information from /etc/v3m/vm.conf
    VMSettings settings;
    cout << "Creation of a new VM instantiation, settings loaded" << endl;
    kvmCommand = settings.getKvmCommand();
    preVMCommand = settings.getKvmPrecommand();
    boot_mode = NORMAL_BOOT;
    cout << "New class instantiation created" << endl;
}

/**
  * @author Panyong Zhang.
  *
  * Class destructor.
  */
kvmVM::~kvmVM()
{
}

/**
  * @author Panyong Zhang.
  *
  * Just a tool: transforms an int into a string.
  *
  * @param num Integer to transform into string.
  * @return The corresponding string.
  */
std::string kvmVM::IntToString(int num)
{
  std::ostringstream myStream;
  myStream << num << std::flush;

  return(myStream.str()); //returns the string form of the stringstream object
}

/**
  * @author Panyong Zhang.
  *
  * Creates a Kvm image. 
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::create_image()
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
    Glib::ustring cmd = "qemu-img create " + location +" " + size + "M";
    cout << "Command to create the KVM image: " << cmd.c_str() << endl;
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        return -1;
    }
    return 0;
}

/**
  * @author Panyong Zhang.
  *
  * Creates a Kvm image. Location is the image path (including the image name)
  * and size the image size in MB 
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::install_vm_from_cdrom ()
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
  * @author Panyong Zhang.
  *
  * Creates the virtual machine image via a network installation.
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::install_vm_from_net ()
{
    boot_mode = NETWORK_BOOT;
    cerr << "ERROR: not yet supported (" << boot_mode << ")" << endl;
    return -1;
}

/**
  * @author Panyong Zhang.
  *
  * Migrates the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::migrate (string node_id)
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Panyong Zhang.
  *
  * Pauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::pause ()
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Panyong Zhang.
  *
  * Unpauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::unpause ()
{
    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Panyong Zhang.
  *
  * Gets the virtual machine's status.
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::status()
{
    int pos, status = -1;
    string line;

    string cmd = "/usr/bin/egrep -lf kvm |awk 'print $4' > kvm_status.tmp ";
    profile_data_t data = profile->get_profile_data ();
    string vmname = data.name;

    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        exit(-1);
    }
    ifstream kvm_status_file("kvm_status.tmp");

    if(!kvm_status_file.is_open()){
        cout << "Unable to open kvm status file";
    } else {
        while (!kvm_status_file.eof()){
            getline(kvm_status_file,line);
            pos = line.find(vmname, 0);
            if (pos > 0){
                status = RUNNING;
            }
        }
    }
    kvm_status_file.close();

    cmd = "rm -f kvm_status.tmp ";
    if (system (cmd.c_str())) {
        cerr << "ERROR executing " << cmd << endl;
        exit(-1);
    }

    return status;

    cerr << "ERROR: not yet supported" << endl;
    return -1;
}

/**
  * @author Panyong Zhang.
  * @author Geoffroy Vallee.
  *
  * Sets the command to execute before the creation of a virtual machine.
  *
  * @param cmd Command to execute before the creation of a virtual machine.
  */
void kvmVM::setPreVMCommand(Glib::ustring cmd)
{
  kvmVM::preVMCommand = cmd;
}

/**
  * @author Panyong Zhang.
  *
  * Gets the command to execute before the creation of a virtual machine.
  *
  * @return String representing the command to execute before the creation of a
  * QEMU virtual machine.
  */
Glib::ustring kvmVM::getPreVMCommand()
{
  return kvmVM::preVMCommand;
}

/**
  * @author Panyong Zhang.
  * 
  * Sets the Kvm command.
  *
  * @param command Command to execute for the creation of a QEMU virutal machine
  */
void kvmVM::setCommand(Glib::ustring command)
{
  kvmVM::kvmCommand = command;
}


/**
  * @author Panyong Zhang.
  *
  * Gets the Kvm command.
  *
  * @return String representing the command for the creation of a QEMU virtual
  * machine.
  */
Glib::ustring kvmVM::getCommand()
{
  return kvmVM::kvmCommand;
}

/** @author Panyong Zhang.
  * @author Geoffroy Vallee.
  * 
  * Generates the network configuration file for QEMU virtual machines.
  *
  * @todo Get the eth0 IP and assign it to the bridge.
  * @todo We should check if the bridge already exists.
  * @return 0 if success, -1 else.
  */
int kvmVM::generate_bridged_network_config_file ()
{
    profile_data_t data = profile->get_profile_data ();
    string filename = "/tmp/kvm-" + data.name + "-ifup.sh";
    string scriptdown = "/etc/qemu-" + data.name + "ifdown.sh";
    string config_file = "/tmp/qemu-" + data.name + ".cfg";
    string cmd;
    string nic_id;
    ofstream file_op, conffile, script_down;
    file_op.open (filename.c_str());
    conffile.open (config_file.c_str());
    script_down.open (scriptdown.c_str());
    if (((data.nic1_type).compare("BRIDGED_TAP") == 0)
        || ((data.nic2_type).compare("BRIDGED_TAP") == 0)) {
        if ((data.nic1_option).compare("") == 0) {                                 
            nic_id = "eth0";                                                       
        } else {                                                                   
            nic_id = data.nic1_option;                                             
        }
        
        /* First we generate the configuration file for network-configurator */
        conffile << "# This file is intended to be used as the input file of \n";
        conffile << "# the network-configurator tool.\n\n\n";
        conffile << "add_nic=" << nic_id << "\n";
        conffile << "bridge=qemubr0\n";
        conffile << "option=restart_dhcpd\n";
        
        file_op << "#!/bin/sh\n";
        file_op << "#\n";
        file_op << "# File generated by libv3m, modify only if know exactly "
                << "what you are doing.\n\n";
        file_op << "# First we create a bridge including the NIC for each we\n"
                << "# want to reuse the IP\n\n";
        file_op << "/usr/bin/network-configurator -f " << config_file <<"\n";
        file_op << "# Then the bridge is ready, we add the other NIC\n\n";
        file_op << "/usr/bin/network-configurator --add-nic $1 "
                << "--bridge qemubr0\n";
                
        script_down << "# File generated by libv3m, modify only if know exactly"
                    << " what you are doing.\n\n";
        script_down << "/sbin/ifconfig qemubr0 down\n";
        script_down << "/usr/sbin/brctl delbr qemubr0\n";
        script_down << "/sbin/ifconfig " << nic_id << " down\n";
        script_down << "/sbin/ifup " << nic_id << "\n";
    }
    if (((data.nic1_type).compare("TUN/TAP") == 0)
        || ((data.nic2_type).compare("TUN/TAP") == 0)) {
        file_op << "#!/bin/sh\n";
        file_op << "sudo -p \"Password for $0:\" /sbin/ifconfig $1 172.20.0.1\n";
    }
    if (((data.nic1_type).compare("TUN/TAP+NAT") == 0)
        || ((data.nic2_type).compare("TUN/TAP+NAT") == 0)) {
        file_op << "#!/bin/sh\n";
        file_op << "sudo -p \"Password for $0:\" /sbin/ifconfig $1 172.20.0.1\n";
        file_op << "IF=" << data.nic1_option << "\n";
        file_op << "echo \"1\" | sudo tee -a /proc/sys/net/ipv4/ip_forward\n";
        file_op << "sudo echo \"iptables -t nat -A POSTROUTING -o $IF -j MASQUERADE\"\n";
        file_op << "sudo /sbin/iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT\n";
        file_op << "sudo /sbin/iptables -t nat -A POSTROUTING -o $IF -j MASQUERADE\n";
    }
    file_op.close();
    conffile.close();
    script_down.close();
    cmd = "chmod a+x " + filename;
    if (system(cmd.c_str())) {
        cerr << "Error generating the network configuration file" << endl;
        return -1;
    }
    cmd = "chmod a+x " + scriptdown;
    if (system(cmd.c_str())) {
        cerr << "Error generating the network configuration file" << endl;
        return -1;
    }

    return 0;
}

#if 0
/** @author Panyong Zhang.
  * 
  * Generates the network configuration file for KVM virtual machines.
  *
  * @todo Get the eth0 IP and assign it to the bridge.
  * @todo We should check if the bridge already exists.
  * @return 0 if success, -1 else.
  */
int KVM::generate_nat_network_config_file()
{
    "# Open the ip fordwarding"
    "echo 1>/proc/sys/net/ipv4/ip_forward"
    "echo 1>/proc/sys/net/ipv6/conf/default/forwarding"

    "/sbin/iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE"
    "/sbin/iptables -I INPUT -i dummy+ -j ACCEPT"
    return 0;
}
#endif
/**
  * @author Panyong Zhang.
  *
  * Function called to create a new kvm virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int kvmVM::boot_vm () 
{
    cout << "Create_vm for Kvm" << endl;

    if (profile == NULL) {
        cerr << "Profile not found" << endl;
        return -1;
    }

    /* We generate the network configuration file */
    generate_bridged_network_config_file ();

    __boot_vm ();
}

/**
  * @author Panyong Zhang.
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
int kvmVM::__boot_vm ()
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

    /* We get the kvm command */
    cout << "Preparing the command for the VM creation..." << endl;
    cmd = getCommand();
    cmd += "  ";
    cmd += data.image;
    /* We manage the amount of memory */
    cmd += " -m ";
    cmd += data.memory;
    cmd += " -smp ";
    cmd += data.cpu;
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
            cmd += " -net nic,macaddr=";
            cmd += data.nic1_mac;
            cmd += " -net tap,script=/tmp/kvm-";
            cmd += data.name;
            cmd += "-ifup.sh";
        }
        if ((data.nic1_type).compare("TUN/TAP") == 0) {
            cmd += " -net nic,macaddr=";
            cmd += data.nic1_mac;
            cmd += " -net tap";
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
            cmd += " -net nic,macaddr=";
            cmd += data.nic2_mac;
            cmd += " -net tap";
        }
        if ((data.nic2_type).compare("BRIDGED_TAP") == 0) {
            cmd += " -net nic,macaddr=";
            cmd += data.nic2_mac;
            cmd += " -net tap,script=/tmp/kvm-";
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
    cout << "Creating the kvm virtual machine:" << cmd << endl;
    if (system (cmd.c_str())) {
  	    cerr << "ERROR: Impossible to execute " << cmd << endl;
    	return -1;
    }

    return 0;
}

