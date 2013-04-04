DESTDIR=
PKGDEST=
VERSION=$(shell cat VERSION)
NAME=oscarv
PKG=$(NAME)-$(VERSION)

SUBDIRS := bin lib


all:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

install:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done

uninstall:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} uninstall ) ; done

clean:
	@rm -f build-stamp configure-stamp
	@rm -rf debian/$(NAME)* debian/files
	@rm -f $(PKG).tar.gz
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
	cp $(PKG).tar.gz $(shell rpm --eval '%_sourcedir')
	rpmbuild -bb ./$(NAME).spec
	@if [ -n "$(PKGDEST)" ]; then \
		RPMDIR=$(shell rpm --eval '%{_rpmdir}') ;\
		RPMSPEC_CMD="/usr/bin/rpm --specfile -q"; \
		[ -f /usr/bin/rpmspec ] && RPMSPEC_CMD="/usr/bin/rpmspec -q";\
		FILES=$$($${RPMSPEC_CMD} --target noarch $(NAME).spec --qf '%{name}-%{version}-%{release}.%{arch}.rpm ');\
		echo "Moving file(s) ($${FILES}) to $(PKGDEST)"; \
		for FILE in $${FILES}; \
		do \
			echo "   $${FILE} --> $(PKGDEST)"; \
			mv $${RPMDIR}/noarch/$${FILE} $(PKGDEST); \
		done; \
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
