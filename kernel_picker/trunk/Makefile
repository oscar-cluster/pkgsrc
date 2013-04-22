DESTDIR=
PKGDEST=
VERSION=1.4.1
NAME=kernel-picker
PKG=$(NAME)-$(VERSION)

include ./Config.mk

SUBDIRS := 

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
	@rm -f debian/files
	@rm -f $(NAME).tar.gz
	@rm -f $(NAME).spec
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} clean ) ; done

dist: clean
	@rm -rf /tmp/$(PKG)
	@mkdir -p /tmp/$(PKG)
	@sed -e 's/__VERSION__/$(VERSION)/g' $(NAME).spec.in > $(NAME).spec
	@cp -rf * /tmp/$(PKG)
	@cd /tmp/$(PKG); rm -rf `find . -name ".svn"`
	@cd /tmp; tar czf $(PKG).tar.gz $(PKG)
	@cp -f /tmp/$(PKG).tar.gz .
	@rm -rf /tmp/$(PKG)/
	@rm -f /tmp/$(PKG).tar.gz

rpm: dist
	cp $(PKG).tar.gz `rpm --eval '%_sourcedir'`
	rpmbuild -bb ./$(NAME).spec
	@if [ -n "$(PKGDEST)" ]; then \
		RPMDIR=$(shell rpm --eval '%{_rpmdir}') ;\
		(which rpmspec 2>/dev/null) && RPMSPEC_CMD="rpmspec --target noarch" || RPMSPEC_CMD="rpm --specfile --define '%_target_cpu noarch'"; \
		CMD="$$RPMSPEC_CMD -q $(NAME).spec --qf '%{name}-%{version}-%{release}.%{arch}.rpm '"; \
		echo "Determining which files to retreive using: $$CMD";\
		FILES=`eval $$CMD`;\
		echo "Moving file(s) ($$FILES) to $(PKGDEST)"; \
		for FILE in $$FILES; \
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
