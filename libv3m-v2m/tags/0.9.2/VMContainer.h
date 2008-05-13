#ifndef __VM_CONTAINER_H
#define __VM_CONTAINER_H

/** @author Geoffroy Vallee.
 *
 * The class provides the abstraction of the VM technology used. All functions 
 * here are common to virtualization technology:
 * boot_vm: the image already exists, we just want to create a VM 
 *            instantiation.
 * migrate: the VM is migrated on a remote node.
 * pause: the VM is paused.
 * unpause: a paused VM is unpaused.
 * create_image: create an empty image file (the file format depends on the 
 *               virtualization technology).
 * install_vm_from_cdrom: an image exists (empty or not), we "install" the 
 *                        system using a bootable CDROM.
 */

using namespace std;

class VirtualMachine {
public:
    /** Virtual function to enforce the presence a function to boot a virtual 
      * machine. */
    virtual int boot_vm() = 0;

    /** Virtual function to enforce the presence a function to migration a 
      * virtual machine. */
    virtual int migrate(string node_id) = 0;

    /** Virtual function to enforce the presence a function to pause a virtual
      * machine. */
    virtual int pause() = 0;

    /** Virtual function to enforce the presence a function to unpause a
      * virtual machine. */
    virtual int unpause() = 0;

    /** Virtual function to enforce the presence a function to create a
      * virtual machine image from a bootable CDROM. */
    virtual int install_vm_from_cdrom() = 0;

    /** Virtual function to enforce the presence a function to create a
      * virtual machine image via a network installation. */
    virtual int install_vm_from_net() = 0;

    /** Virtual function to enforce the presence a function to  get the
      * virutal machine status. */
    virtual int status() = 0;
private:
};

template <class C> class VMContainer {
public:
    /** Class constructor. We transparently get the type of the virtual machine
      * and store a pointer to a virtual machine instantiation in vm.
      * @param profile Virtual machine's profile. */
    VMContainer (ProfileXMLNode* profile) { vm = new C (profile);}

    /** Redirect the virtual boot function to the actual boot function
      * specific the virtual machine type. */
    int boot_vm() {return (vm->boot_vm());}

    /** Redirect the virtual migration function to the actual migration function
      * specific the virtual machine type. */
    int migrate (string node_id) {return (vm->migrate(node_id));}

    /** Redirect the virtual pause function to the actual pause function
      * specific the virtual machine type. */
    int pause () {return (vm->pause());}

    /** Redirect the virtual unpause function to the actual unpause function
      * specific the virtual machine type. */
    int unpause () {return (vm->unpause());}

    /** Redirect the virtual create_image function to the actual create_image
      * function specific the virtual machine type. */
    int create_image() {return (vm->create_image());}

    /** Redirect the virtual install_vm_from_cdrom function to the actual
      * install_vm_from_cdrom function specific the virtual machine type. */
    int install_vm_from_cdrom() {return (vm->install_vm_from_cdrom());}

    /** Redirect the virtual install_vm_from_net function to the actual
      * install_vm_from_net function specific the virtual machine type. */
    int install_vm_from_net() {return (vm->install_vm_from_net());}

    /** Redirect the virtual status function to the actual status function
      * specific the virtual machine type. */
    int status() {return (vm->status());}

private:
    C *vm;
};


#endif

