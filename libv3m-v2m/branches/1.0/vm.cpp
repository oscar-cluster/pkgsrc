/*
 *  Copyright (c) 2006-2007 Oak Ridge National Laboratory,
 *                          Geoffroy Vallee <valleegr@ornl.gov>
 *                          All rights reserved
 *  This file is part of the KVMs software.  For license information,
 *  see the COPYING file in the top level directory of the source
 */

#include <sstream>
#include <libxml++/libxml++.h>

#include "vm.h"

#define NB_VIRT_TECHNO 6
/** List of supported virtualization solutions */
string virt_techno[NB_VIRT_TECHNO] = {"Qemu", "Kvm", "Xen", "XenHVM", "VMWare",
                                      "VMM-HPC"};

/**
  * @author Geoffroy Vallee.
  *
  * Class constructor. The constructor loads and parses the VM profile. 
  *
  * @param file The virtual machine's profile.
  */
VM::VM (string file) {
    cout << "VM Instantiation" << endl;
    cout << "Creating a VM instantiation based on the " << file << "profile"
         << endl;
    xmlpp::DomParser parser;
    parser.set_validate();
    parser.parse_file(file);
    if (!parser) {
        cerr << "Error, profile not valid. Check your profile." << endl;
        return;
    }

    if (!parser) {
        cerr << "Error: profile parser is empty, impossible to analyse" << endl;
        return;
    }
    xmlpp::Node* root = parser.get_document()->get_root_node(); 
    cout << "VM: we have the root node of the XML profile" << endl;

    if(root) {
        xmlpp::Node::NodeList list = root->get_children("profile");
/*        if (list.size() != 1) {
                cerr << "Error: there is " << IntToString(list.size()) << "
profile(s) in your file, you should have only one" <<     endl;
            return;
        }*/
        cout << "Profile found" << endl;
        // we should have only one node (one profile per file)
        xmlpp::Node::NodeList::iterator iter = list.begin(); 
        profile = new ProfileXMLNode (root);
        data_profile = profile->get_profile_data();
        if (check_vm_type (data_profile.type)) {
            cerr << "ERROR creating a new VM instantiation" << endl;
            exit (-1);
        }
        if ((data_profile.type).compare ("Qemu") == 0) 
            qemu_vm = new VMContainer<qemuVM> (profile);
        if ((data_profile.type).compare ("Kvm") == 0) 
            kvm_vm = new VMContainer<kvmVM> (profile);
        if ((data_profile.type).compare ("Xen") == 0)
            xen_vm = new VMContainer<xen> (profile);
        if ((data_profile.type).compare ("XenHVM") == 0)
            xenhvm_vm = new VMContainer<xen_hvm> (profile);
        if ((data_profile.type).compare ("VMWare") == 0)
            vmware_vm = new VMContainer<vmware> (profile);
        if ((data_profile.type).compare ("VMM-HPC") == 0)
            vmware_vm = new VMContainer<vmware> (profile);
    }
}

/**
  * @author Geoffroy Vallee.
  *
  * Class destructor.
  */
VM::~VM() {
}


/**
  * @author Geoffroy Vallee.
  *
  * Check the VM's type. Note the list of supported virtualization tencho
  * is set in the virt_techno array to ease the code maintainance.
  *
  * @param type VM's type
  * @return 0 if the VM techno is supported, -1 else.
  */
int VM::check_vm_type (string type)
{
    for (int i=0; i<NB_VIRT_TECHNO; i++) {
        if (type.compare (virt_techno[i]) == 0) {
            return 0;
        }
    }
    cerr << "ERROR: virtualization technology (" << type << ") not supported."
         << endl;
    return -1;
}

/**
  * @author: Geoffroy Vallee.
  *
  * Boots a virtual machine based on an XML profile. Since the VM class is the
  * abstraction of the virtualization technology used, we start here to deal
  * with details (for instance QEMU vs. Xen and so on).
  * The object has to be instantiated before, the function may be directly 
  * called.
  *
  * @return 0 if success, -1 if not.
  */
int VM::boot_vm() {
    cout << "Creating a VM" << endl;
    if ((data_profile.type).compare("Qemu") == 0) {
        cout << "Creating a Qemu VM" << endl;
        if (qemu_vm) {
            if (qemu_vm->boot_vm()) {
                cerr << "ERROR creating the Qemu VM" << endl;
                return -1;
            }
            return 0;
        }
        cerr << "ERROR creating the Qemu VM" << endl;
        return -1;
    }
    if ((data_profile.type).compare("Kvm") == 0) {
        cout << "Creating a KVM VM" << endl;
        if (kvm_vm) {
            if (kvm_vm->boot_vm()) {
                cerr << "ERROR creating the KVM VM" << endl;
                return -1;
            }
            return 0;
        }
        cerr << "ERROR creating the KVM VM" << endl;
        return -1;
    }
    if ((data_profile.type).compare("Xen") == 0) {
        cout << "Creating a Xen VM" << endl;
        if (xen_vm) {
            if (xen_vm->boot_vm()) {
                cerr << "ERROR: creating the Xen VM" << endl;
                return -1;
            }
            return 0;
        }
        cerr << "ERROR: creating the Xen VM" << endl;
        return -1;
    }
    if ((data_profile.type).compare("XenHVM") == 0) {
        cout << "Creating a Xen VM (HVM mode)" << endl;
        if (xen_vm) {
            if (xenhvm_vm->boot_vm()) {
                cerr << "ERROR: creating the Xen VM (HVM mode)" << endl;
                return -1;
            }
            return 0;
        }
        cerr << "ERROR: creating the Xen VM" << endl;
        return -1;
    }


    cerr << "ERROR: unknown VM type (" << data_profile.type << ")" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Migrates a virtual machine on a remote node
  *
  * @param node_id Identifier of the migration's target.
  * @return 0 if success, -1 else.
  */
int VM::migrate (string node_id) 
{
    cout << "Migrating a VM" << endl;
    if ((data_profile.type).compare("Xen") == 0) {
        if (xen_vm) {
            if (xen_vm->migrate (node_id)) {
                cerr << "ERROR migrating the VM" << endl;
                return -1;
            }
        }
    }
    if ((data_profile.type).compare("Qemu") == 0) {
        cerr << "VM migration not currently supported by Qemu" << endl;
        return -1;
    }
    if ((data_profile.type).compare("VMWare") == 0) {
        cerr << "VM migration not currently supported by VMWare" << endl;
        return -1;
    }
    return 0;
}

/**
  * @author Geoffroy Vallee.
  *
  * Pauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int VM::pause ()
{
    cerr << "ERROR: feature not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Unpauses the virtual machine.
  *
  * @return 0 if success, -1 else.
  */
int VM::unpause ()
{
    cerr << "ERROR: feature not yet supported" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee
  *
  * Creates an image for a VM from a bootable CDROM. Since the VM class is the 
  * abstraction of the virtualization technology used, we start here to deal 
  * with details (for instance QEMU vs. Xen and so on).
  * The object has to be instantiated before, the function may be directly
  * called.
  *
  * @return 0 if sucess, -1 if not.
  */
int VM::create_image_from_cdrom() 
{
    cout << "Creating image for a VM from a bootable CDROM" << endl;
    if ((data_profile.type).compare("Qemu") == 0) {
        cout << "VM::create_image_from_cdrom(): Creating " 
             << "an image for a Qemu VM from a bootable CDROM" << endl;
        qemu_vm->create_image();
        qemu_vm->install_vm_from_cdrom ();
        cout << "VM image created" << endl;
        return 0;
    } 
    if ((data_profile.type).compare("XenHVM") == 0) {
        cout << "VM::create_image_from_cdrom(): Creating "
             << "an image for a XenHVM VM from a bootable CDROM" << endl;
        xenhvm_vm->create_image();
        xenhvm_vm->install_vm_from_cdrom ();
        cout << "VM image created" << endl;
        return 0;
    }
    cerr << "Sorry this functionnality is not yet supported "
         << "for this virtualization technology" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Creates the virtual machine image using a network boot. Usefull to create a
  * virtual machine using OSCAR for instance.
  *
  * @return 0 if success, -1 else
  */
int VM::create_image_with_oscar() 
{
    cout << "Creating image for a VM using a network installation" << endl;
    if ((data_profile.type).compare("Xen") == 0) {
        cout << "Creating a Xen VM using OSCAR" << endl;
        // We create first an empty image
        if (xen_vm->create_image ()) {
            cerr << "ERROR creating the image" << endl;
            return -1;
        }
        // then we install the system within the image
        if (xen_vm->install_vm_from_net ()) {
            cerr << "ERROR installing the VM" << endl;
            return -1;
        }
        cout << "VM image created" << endl;
        return 0;
    }
    if ((data_profile.type).compare("Qemu") == 0) {
        cout << "Creating a Qemu VM using OSCAR" << endl;
        // We create first an empty image
        if (qemu_vm->create_image ()) {
            cerr << "ERROR creating the image" << endl;
            return -1;
        }
        // then we install the system within the image
        if (qemu_vm->install_vm_from_cdrom ()) {
            cerr << "ERROR installing the VM" << endl;
            return -1;
        }
        cout << "VM image created" << endl;
        return 0;
    }
    if ((data_profile.type).compare("XenHVM") == 0) {
        cout << "Creating a XenHVM VM using OSCAR" << endl;
        // We create first an empty image
        if (xenhvm_vm->create_image ()) {
            cerr << "ERROR creating the image" << endl;
            return -1;
        }
        // then we install the system within the image
        if (xenhvm_vm->install_vm_from_cdrom ()) {
            cerr << "ERROR installing the VM" << endl;
            return -1;
        }
        cout << "VM image created" << endl;
        return 0;
    }
    cerr << "Sorry this functionnality is not yet supported "
         << "for this virtualization technology" << endl;
    return -1;
}

/**
  * @author Geoffroy Vallee.
  *
  * Gets the status of the virtual machine.
  *
  * @return The virtual machine status.
  * @todo Why do we have a comparison to the virtual machine's type here?
  */
int VM::status()
{
    if ((data_profile.type).compare("Qemu") == 0) {
        if (qemu_vm->status ()) {
            cerr << "ERROR: Impossible to get the VM status" << endl;
            return -1;
        }
    }
    if ((data_profile.type).compare("Xen") == 0) {
        if (xen_vm->status ()) {
            cerr << "ERROR: Impossible to get the VM status" << endl;
            return -1;
        }
    }
}

/**
  * @author Geoffroy Vallee.
  *
  * Converts an integer into a string.
  *
  * @param num Integer to convert.
  * @return String representing the integer.
  */
string VM::IntToString(int num)
{
    ostringstream myStream; 
    myStream << num << flush;

    return(myStream.str()); //returns the string form of the stringstream object
}

