DESTDIR=
INSTALLDIR=$(DESTDIR)/opt/sync_files
TMPDIR=/tmp
PKG=sync-files
VERSION=2.4.1
DISTROS := 
DISTROS += mdv
DISTROS += redhat
DISTROS += suse

all:

install:
	mkdir -p $(INSTALLDIR)/bin
	mkdir -p $(INSTALLDIR)/etc
	mkdir -p $(INSTALLDIR)/tmp
	cp -p sync_files $(INSTALLDIR)/bin/sync_files
	cp -p sync_files.conf $(INSTALLDIR)/etc/sync_files.conf
	cp -p confmgr $(INSTALLDIR)/bin/confmgr
	@for distro in $(DISTROS); do \
		mkdir -p $(INSTALLDIR)/templates/distro/$$distro; \
		cp templates/distro/$$distro/group \
			$(INSTALLDIR)/templates/distro/$$distro; \
		cp templates/distro/$$distro/passwd \
			$(INSTALLDIR)/templates/distro/$$distro; \
		cp templates/distro/$$distro/shadow \
			$(INSTALLDIR)/templates/distro/$$distro; \
	done

dist:
	rm -rf $(TMPDIR)/$(PKG)-$(VERSION)
	mkdir -p $(TMPDIR)/$(PKG)-$(VERSION)
	cp -rf * $(TMPDIR)/$(PKG)-$(VERSION)
	rm -rf `find $(TMPDIR)/$(PKG)-$(VERSION) .svn`
	cd $(TMPDIR) && tar czf $(PKG)-$(VERSION).tar.gz $(PKG)-$(VERSION)
	cp $(TMPDIR)/$(PKG)-$(VERSION).tar.gz .

clean:
	rm -f $(INSTALLDIR)/bin/sync_files
	rm -f $(INSTALLDIR)/etc/sync_files.conf
	rm -f $(INSTALLDIR)/bin/confmgr
	rm -f $(INSTALLDIR)/templates
	rm -f ./*tar.gz

rpm: dist
	sed -e "s/PERLLIBPATH/$(SEDLIBDIR)/" < $(PKG).spec.in \
        > $(PKG).spec
	cp $(PKG)-$(VERSION).tar.gz $(SOURCEDIR)
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
