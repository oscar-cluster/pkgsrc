bin_PROGRAMS = v2m

v2m_SOURCES = ArgumentParser.cpp v2m.cpp ArgumentParser.h debug.h \
              contrib/bin/netboot_emulation_1.9.img.bz2 \
              contrib/bin/oscar_bootcd.iso doxygen_config.in \
			  doc/Makefile

autotools_generated_files = configure Makefile.in config.log depcomp \
			  INSTALL .deps Makefile config.status autom4te.cache \
			  missing version.h aclocal.m4 install-sh

get-binaries:
	cd contrib/src; ./get_netboot_image; ./get_oscarboot_image

clean-binaries:
	cd contrib/bin/; rm -f *.bz2 && rm -f *.img && rm -f *.iso

# Generate library documentation using Doxygen.
# If it's not available, just touch a stamp file in the right directory.
# The configure.ac sets DOXYGEN to NO_DOXYGEN if it's not found.
libdocdir = @DOXYGEN_OUTPUT_DIRECTORY@
DOXYGEN=@DOXYGEN@

docclean:
	cd doc; make clean

clean-contrib: clean-binaries

clean-autotools:
	rm -rf $(autotools_generated_files)

clean-dist-tarballs:
	rm -f v2m*.tar.gz

mrproper: clean clean-contrib clean-autotools clean-dist-tarballs docclean
	rm -f doxygen_config

clean-generic: docclean

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

$(libdocdir): *cpp *h doc/html/index.html 
	echo "Sources have changed, regenerating the Doxygen documentation..."
	make doxygen-build

doc/html/index.html:
	echo "Doxygen documentation not found, generating it..."
	make doxygen-build

