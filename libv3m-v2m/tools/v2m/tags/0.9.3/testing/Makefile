TOPDIR=..

# We should test first if the DTD are in /etc/v3m
# We should aso check if /etc/v3m/vm.conf exists
qemu-basic-test:
	$(TOPDIR)/v2m qemu_test.xml --boot-vm

qemu-bootcd-test:
	$(TOPDIR)/v2m qemu_bootable_cdrom_test.xml --create-vm-image-from-cdrom

