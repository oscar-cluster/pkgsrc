/* Copyright (c) 2006-2007, Oak Ridge National Laboratory.
 *
 * This file is part of libv3m.
 */

/**
  * @file vm.h
  * @brief This defines a basic class for the specification of a virtual 
  *        machine.
  * @author Geoffroy Vallee.
  *
  * This defines a basic class for the specification of a virtual machine.
  */

#ifndef __VM_H
#define __VM_H

#include <iostream>

#include "ProfileXMLNode.h"
#include "VMContainer.h"
#include "qemu.h"
#include "xen.h"
#include "vmware.h"
#include "xen-hvm.h"

using namespace std;

class VM {

public:
    VM (string);
    ~VM ();
    int boot_vm ();
    int create_image_from_cdrom ();
//    int install_vm_from_net ();
    int create_image_with_oscar ();
    int migrate (string);
    int pause ();
    int unpause ();
    int status();

private:
    ProfileXMLNode *profile;
    profile_data_t data_profile;
    VMContainer<qemuVM> *qemu_vm;
    VMContainer<xen> *xen_vm;
    VMContainer<xen_hvm> *xenhvm_vm;
    VMContainer<vmware> *vmware_vm;

    std::string get_profile_type ();
    std::string IntToString(int num);
    int check_vm_type (std::string type);
};

#endif
