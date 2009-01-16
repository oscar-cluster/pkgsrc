DESTDIR=
PKG=network-configurator

include ./Config.mk

SUBDIRS := bin

all:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

install:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done

uninstall:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} uninstall ) ; done

clean:
	@rm -f *~
	@rm -f build-stamp configure-stamp
	@rm -rf debian/$(PKG)
	@rm -f $(PKG).tar.gz
	@rm -f $(PKG).spec
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} clean ) ; done

dist: clean
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
	cp $(PKG).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -bb ./$(PKG).spec

deb:
	dpkg-buildpackage -rfakeroot
