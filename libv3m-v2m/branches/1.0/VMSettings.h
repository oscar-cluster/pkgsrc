/*
 *  Copyright (c) 2006 Oak Ridge National Laboratory, 
 *                     Geoffroy Vallee <valleegr@ornl.gov>
 *                     All rights reserved
 *  This file is part of the libv3m software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#ifndef _VMSETTINGS_H_
#define _VMSETTINGS_H_

#define MAX_LINE_LENGHT 128

class VMSettings{

public:
    /** Class constructor */
    VMSettings();

    /** Class destructor */
    ~VMSettings();

    /** Get the KVM command for the creation of a virtual machine
      * (the configuration file has been parsed previously) */
    std::string getKvmCommand();

    /** Get the command to execute before the creation of a KVM 
      * (the configuration file has been parsed previously) */
    std::string getKvmPrecommand();

    /** Get the QEMU command for the creation of a virtual machine
      * (the configuration file has been parsed previously) */
    std::string getQemuCommand();

    /** Get the command to execute before the creation of a QEMU VM 
      * (the configuration file has been parsed previously) */
    std::string getQemuPrecommand();

    /** Get the Xen command for the creation of a virtual machine
      * (the configuration file has been parsed previously) */
    std::string getXenCommand();

    /** Get the command to execute before the creation of a Xen VM 
      * (the configuration file has been parsed previously) */
	std::string getXenPrecommand();

    /** Get the VMware command for the creation of a virtual machine
      * (the configuration file has been parsed previously) */
    std::string getVmwareCommand();

    /** Get the command to execute before the creation of a VMware VM 
      * (the configuration file has been parsed previously) */
    std::string getVmwarePrecommand();

    /** Get the VMM-HPC command for the creation of a virtual machine
      * (the configuration file has been parsed previously) */
    std::string getVMMHPCCommand();

    /** Get the command to execute before the creation of a VMM-HPC VM 
      * (the configuration file has been parsed previously) */
    std::string getVMMHPCPrecommand();

    /** Get the location of the image for the simulation of a network boot of a
      * virtual machine. */
	std::string getNetbootImage();

private:
    Glib::ustring get_node_content (const xmlpp::Node*);

    std::string kvmCommand;
    std::string kvmPrecommand;
    std::string qemuCommand;
    std::string qemuPrecommand;
    std::string xenCommand;
    std::string xenPrecommand;
    std::string vmwareCommand;
    std::string vmwarePrecommand;
    std::string vmmhpcCommand;
    std::string vmmhpcPrecommand;
	std::string netboot;
};

#endif // MYSETTINGS_H
