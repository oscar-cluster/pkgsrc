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

  // We load configuration information from configuration file.
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
  *
  * Creates a vmware image. Location is the image path (including the image
  * name) and size the image size in MB.
  *
  * @return 0 if success, -1 else.
  */
int vmware::create_image()
{
    std::cerr << "ERROR: not yet supported" << std::endl;
    return -1;
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
  std::cerr << "ERROR: not yet supported" << std::endl;
  return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Creates a vmware image from a network installation, using OSCAR.
  *
  * @return 0 if success, -1 else.
  */
int vmware::install_vm_from_net ()
{
    std::cerr << "ERROR: not yet supported" << std::endl;
    return -1;
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
  *
  * Pauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int vmware::pause ()
{
    std::cerr << "ERROR: not yet supported" << std::endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Unpauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int vmware::unpause ()
{
    std::cerr << "ERROR: not yet supported" << std::endl;
    return -1;
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
  * @author Geoffroy Vallee.
  *
  * Function called to create a new vmware virtual machine.
  */
int vmware::boot_vm () 
{
  Glib::ustring cmd;

  std::cout << "Create_vm for vmware" << std::endl;

  if (profile == NULL) {
    std::cerr << "Profile not found" << std::endl;
    return -1;
  }

  profile_data_t data = profile->get_profile_data ();

  /* we first prepare the enviromnent for the VM */
  cmd = getPreVMCommand();
  std::cout << "Command to execute before the creation of the VM: " << cmd <<
std::endl;
  if (system (cmd.c_str())) {
  	std::cerr << "ERROR executing " << cmd << std::endl;
	return -1;
  }

  /* We get the vmware command */
  std::cout << "Preparing the command for the VM creation..." << std::endl;
  cmd = getCommand();
  cmd += "  ";
  cmd += data.image;
  /* we add the network configuration */
  if ((data.nic1_type).compare("N/A") != 0 && (data.nic1_type).compare("") != 0)
{
    if ((data.nic1_type).compare("TUN/TAP") == 0) {
      cmd += " -net nic -net tap";
    }
    else {
      cmd += " -net nic,macaddr=";
      cmd += data.nic1_mac;
      cmd += " -net socket,connect=localhost:1234";
    }
  }
  if ((data.nic2_type).compare("N/A") != 0 && (data.nic2_type).compare("") != 0)
{
    if ((data.nic2_type).compare("TUN/TAP") == 0) {
      cmd += " -net nic -net tap";
    }
    else {
      cmd += " -net nic,macaddr=";
      cmd += data.nic2_mac;
      cmd += " -net socket,listen=localhost:1234";
    }
  }
  cmd += " &";
  while (cmd.find("\n") != -1) {
    std::cout << cmd.find("\n") << std::endl;
    cmd.erase(cmd.find("\n"), 1);
  }
  while (cmd.find("file://") != -1) {
    cmd.erase(cmd.find("file://"), 7);
  }
  std::cout << "Creating the vmware virtual machine:" << cmd << std::endl;
  if (system (cmd.c_str())) {
    std::cerr << "ERROR executing " << cmd << std::endl;
    return -1;
  }
}

/**
  * @author Geoffroy Vallee.
  */
int vmware::status ()
{
    std::cerr << "ERROR: not yet supported" << std::endl;
    return -1;
}


