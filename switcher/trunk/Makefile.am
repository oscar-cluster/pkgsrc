# -*- makefile -*-
#
# Copyright (c) 2002 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the Env-switcher software package.  For license
# information, see the LICENSE file in the top-level directory of the
# Env-switcher source distribution.
#
# $Id: Makefile.am,v 1.4 2002/10/27 12:54:20 jsquyres Exp $
#

include $(top_srcdir)/dist/Makefile.options

DESTDIR=
PKGDEST=
SOURCEDIR=/usr/src/redhat/SOURCES
PKG=env-switcher
SUBDIRS		= dist src man

EXTRA_DIST	= AUTHORS README LICENSE INSTALL

dist-hook:
	(cd $(distdir); \
	 year="`date +%Y`"; \
	 sed -e 's/CURRENTYEAR/'$$year/ LICENSE > LICENSE.tmp; \
	 cp LICENSE.tmp LICENSE; \
	 rm LICENSE.tmp; \
	 date="`date '+%B, %Y'`"; \
	 sed -e 's/-RELEASEDATE-/'"$$date"/ \
	     -e 's/-RELEASEVERSION-/'$(SWITCHER_VERSION)/ \
	     man/switcher.1 > switcher.1; \
	 cp switcher.1 man; \
	 rm -f switcher.1)

dist:
	@rm -rf /tmp/$(PKG)
	@mkdir -p /tmp/$(PKG)
	@cp -rf * /tmp/$(PKG)
	@cd /tmp/$(PKG); rm -rf `find . -name ".svn"`
	@cd /tmp; tar czf $(PKG).tar.gz $(PKG)
	@cp -f /tmp/$(PKG).tar.gz .
	@rm -rf /tmp/$(PKG)/
	@rm -f /tmp/$(PKG).tar.gz

rpm: dist
	sed -e "s/PERLLIBPATH/$(SEDLIBDIR)/" < $(PKG).spec.in \
        > $(PKG).spec
	cp $(PKG).tar.gz $(SOURCEDIR)
	rpmbuild -bb ./$(PKG).spec
	@if [ -n "$(PKGDEST)" ]; then \
        mv `rpm --eval '%{_topdir}'`/RPMS/noarch/$(PKG)-*.noarch.rpm $(PKGDEST); \
    fi

deb:
	@if [ -n "$$UNSIGNED_OSCAR_PKG" ]; then \
        echo "dpkg-buildpackage -rfakeroot -us -uc"; \
        dpkg-buildpackage -rfakeroot -us -uc; \
    else \
        echo "dpkg-buildpackage -rfakeroot"; \
        dpkg-buildpackage -rfakeroot; \
    fi
	@if [ -n "$(PKGDEST)" ]; then \
        mv ../$(PKG)*.deb $(PKGDEST); \
    fi
