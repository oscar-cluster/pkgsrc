DESTDIR=
PKG=oscar-benchmarking

SUBDIRS := bin tmp

all: get-binaries
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

install:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done

uninstall:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} uninstall ) ; done

get-binaries: 
	cd tmp; make

clean:
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

rpm: dist get-binaries
	cp tmp/cbench_release_1.2.0.tar.gz /usr/src/redhat/SOURCES
	rpmbuild -bb ./cbench.spec
	rpmbuild -bb ./cbenchsource.spec

deb:
	dpkg-buildpackage -rfakeroot
