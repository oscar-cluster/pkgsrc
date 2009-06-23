#lib_LIBRARIES = libv3m.a

#libv3m_a_SOURCES = libv3m.h vm_status.h nic.h ProfileXMLNode.cpp \
#	ProfileXMLNode.h qemu.h qemu.cpp vm.h vm.cpp VMContainer.h VMSettings.cpp \
#	VMSettings.h xen.h xen.cpp xen-hvm.h xen-hvm.cpp vmware.cpp vmware.h \
#	kvm.cpp kvm.h \
#	vmm-hpc.h vmm-hpc.cpp boot.h etc/v3m_config.dtd etc/v3m_profile.dtd \
#	doxygen_config.in doc/user_guide.sgml doc/example_profile_xen.xml \
#	doc/example_profile_qemu.xml doc/Makefile

include_HEADERS = vm.h ProfileXMLNode.h VMContainer.h qemu.h xen.h vmware.h \
				  xen-hvm.h nic.h vmm-hpc.h vm_status.h kvm.h
libv3mdir = ${prefix}/lib
libv3m_LTLIBRARIES=libv3m.la
libv3m_la_SOURCES = libv3m.h vm_status.h nic.h ProfileXMLNode.cpp \
    ProfileXMLNode.h qemu.h qemu.cpp vm.h vm.cpp VMContainer.h VMSettings.cpp \
    VMSettings.h xen.h xen.cpp xen-hvm.h xen-hvm.cpp vmware.cpp vmware.h \
    kvm.cpp kvm.h \
    vmm-hpc.h vmm-hpc.cpp boot.h etc/v3m_config.dtd etc/v3m_profile.dtd \
    doxygen_config.in doc/user_guide.sgml doc/example_profile_xen.xml \
    doc/example_profile_qemu.xml doc/Makefile
libv3m_la_LDFLAGS = -version 0:0:0

autotools_generated_files = configure Makefile.in config.log depcomp \
              INSTALL .deps Makefile config.status autom4te.cache \
              missing version.h aclocal.m4 install-sh

clean-autotools:
	rm -rf $(autotools_generated_files)

# Generate library documentation using Doxygen.
# If it's not available, just touch a stamp file in the right directory.
# The configure.ac sets DOXYGEN to NO_DOXYGEN if it's not found.
libdocdir = @DOXYGEN_OUTPUT_DIRECTORY@
DOXYGEN=@DOXYGEN@

docclean:
	cd $(libdocdir); make clean; 

clean-distrib-tarball:
	rm -f libv3m*.tar.gz

mrproper: clean clean-distrib-tarball docclean clean-autotools
	rm -f doxygen_config

clean-generic: docclean

install-etc:
	install -d -m644 $(DESTDIR)/etc/libv3m
	install etc/*.dtd $(DESTDIR)/etc/libv3m
	install etc/*.conf $(DESTDIR)/etc/libv3m

install-data: install-etc

doxygen-build:
	echo "Generating documentation with Doxygen...";
	if test -d "$(libdocdir)/html"; then \
        echo "Doxygen documentation already generated."; \
    else \
        $(DOXYGEN) doxygen_config; \
    fi
	if test "$(DOXYGEN)" != NO_DOXYGEN; then \
        $(DOXYGEN) doxygen_config; \
    else \
      echo "No doxygen available -- library documentation not generated" \
           >$(libdocdir)/stamp; \
    fi

$(libdocdir): *cpp *h doc/html/index.html doc/user_guide.pdf
	echo "Sources have changed, regenerating the Doxygen documentation..."
	make doxygen-build

doc/html/index.html:
	echo "Doxygen documentation not found, generating it..."
	make doxygen-build

doc/user_guide.pdf:
	echo "User documentation not found, generating it..."
	cd doc; make

