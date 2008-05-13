#ifndef __BOOT_H
#define __BOOT_H

#define NORMAL_BOOT 0    
// NORMAL_BOOT: We want to boot the VM without extra options
#define CDROM_BOOT 1
// CDROM_BOOT: We want to boot the VM using a bootable CDROM
#define NETWORK_BOOT 2
// NETWORK_BOOT: We want to boot the VM using a virtual network boot
#endif
