DESTDIR=

include ./Config.mk

SUBDIRS := bin

all:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

install:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done

uninstall:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} uninstall ) ; done

clean:
	@rm -f build-stamp configure-stamp
	@rm -rf debian/oscar-v
	@rm -f oda.tar.gz
	@rm -f oda.spec
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} clean ) ; done

dist: clean
	@rm -rf /tmp/oscar-v
	@mkdir -p /tmp/oscar-v
	@cp -rf * /tmp/oscar-v
	@cd /tmp/oda; rm -rf `find . -name ".svn"`
	@cd /tmp; tar czf oscar-v.tar.gz oda
	@cp -f /tmp/oscar-v.tar.gz .
	@rm -rf /tmp/oscar-v/
	@rm -f /tmp/oscar-v.tar.gz

rpm: dist
	sed -e "s/PERLLIBPATH/$(SEDLIBDIR)/" < oscar-v.spec.in \
        > oscar-v.spec
	rpmbuild -bb ./oscar-v.spec

deb:
	dpkg-buildpackage -rfakeroot
