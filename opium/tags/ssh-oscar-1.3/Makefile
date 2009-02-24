DESTDIR=
PKGDEST=
SOURCEDIR=/usr/src/redhat/SOURCES
PKG=ssh-oscar

MANPAGES := ssh-oscar
SUBDIRS := 

all:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

doc:
	install -d -m 0755 $(DESTDIR)/usr/local/man/man1/
	for bin in ${MANPAGES} ; do ( pod2man --section=1 $$bin $(DESTDIR)/usr/local/man/man1/$$bin.1 ) ; done

install: clean doc
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done
	@echo "Installing ssh-oscar..."
	# We use an install script because the installation is completely different
	# on Debian and RPM based systems.
	./install $(DESTDIR)

deb:
	cp -f postinst debian/
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

clean:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} clean ) ; done
	rm -f *~
	rm -f ./$(PKG).tar.gz
	rm -f *.rpm
	rm -f debian/postinst debian/files build-stamp configure-stamp
	rm -rf debian/$(PKG)

dist: clean
	rm -rf /tmp/$(PKG)
	mkdir /tmp/$(PKG)
	cp -rf * /tmp/$(PKG)
	cd /tmp/$(PKG); rm -rf `find . -name ".svn"`
	cd /tmp; tar czf ./$(PKG).tar.gz $(PKG)
	mv /tmp/$(PKG).tar.gz .
	rm -rf /tmp/$(PKG)/
	rm -f /tmp/$(PKG).tar.gz

rpm: dist
	cp $(PKG).tar.gz $(SOURCEDIR)
	rpmbuild -bb ./$(PKG).spec
	@if [ -n "$(PKGDEST)" ]; then \
        mv `rpm --eval '%{_topdir}'`/RPMS/noarch/$(PKG)-*.noarch.rpm $(PKGDEST); \
    fi
