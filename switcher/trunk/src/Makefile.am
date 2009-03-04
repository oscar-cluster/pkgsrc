# -*- makefile -*-
#
# Copyright (c) 2002-2004 The Trustees of Indiana University.  
#                         All rights reserved.
#
# This file is part of the Env-switcher software package.  For license
# information, see the LICENSE file in the top-level directory of the
# Env-switcher source distribution.
#
# $Id: Makefile.am,v 1.4 2004/03/07 22:06:54 jsquyres Exp $
#

include $(top_srcdir)/dist/Makefile.options

bin_SCRIPTS	= switcher

install-data-local:
	if test ! -d $(DESTDIR)$(MODULEFILESDIR)/switcher; then \
	  mkdir -p $(DESTDIR)$(MODULEFILESDIR)/switcher; \
	fi
	if test ! -d $(DESTDIR)$(sysconfdir); then \
	  mkdir -p $(DESTDIR)$(sysconfdir); \
	fi
	if test ! -d $(DESTDIR)$(SWITCHER_PKGDATADIR); then \
	  mkdir -p $(DESTDIR)$(SWITCHER_PKGDATADIR); \
	fi
	cp switcher.tcl $(DESTDIR)$(MODULEFILESDIR)/switcher/$(VERSION)
	rm -f $(DESTDIR)$(MODULEFILESDIR)/switcher/.version
	echo '#%Module1.0' > $(DESTDIR)$(MODULEFILESDIR)/switcher/.version
	echo "set ModulesVersion \"$(VERSION)\"" >> \
		$(DESTDIR)$(MODULEFILESDIR)/switcher/.version

uninstall-local:
	rm -f $(DESTDIR)$(MODULEFILESDIR)/switcher/.version
	rm -f $(DESTDIR)$(MODULEFILESDIR)/switcher/$(VERSION)
