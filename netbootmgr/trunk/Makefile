DESTDIR=
VERSION=$(shell cat VERSION)
NAME=netbootmgr
PKG=$(NAME)-$(VERSION)

all: netbootmgr netBootMgr.pm sureDialog.pm

Ui_NetBootMgr.pm: netbootmgr.ui.h
	puic4 -o Ui_NetBootMgr.pm netbootmgr.ui

Ui_SureDialog.pm: suredialog.ui.h
	puic4 -o Ui_SureDialog.pm suredialog.ui

clean:
	@rm -f sureDialog.pm netBootMgr.pm
	@rm -f *~
	@rm -f $(NAME).spec
	@rm -f debian/files debian/changelog
	@rm -rf debian/netbootmgr
	@rm -f $(PKG).tar.bz2
	@rm -f Ui_*.pm

dist: clean
	@rm -rf /tmp/$(PKG)
	@mkdir -p /tmp/$(PKG)
	@cp -rf * /tmp/$(PKG)
	@cd /tmp/$(PKG); rm -rf `find . -name ".svn"`
	@sed -e 's/__VERSION__/$(VERSION)/g' $(NAME).spec.in > $(NAME).spec
	@sed -e 's/__VERSION__/$(VERSION)/g' debian/changelog.in > debian/changelog
	@cd /tmp; tar cjf $(PKG).tar.bz2 $(PKG)
	@cp -f /tmp/$(PKG).tar.bz2 .
	@rm -rf /tmp/$(PKG)/
	@rm -f /tmp/$(PKG).tar.bz2

rpm: dist
	@cp $(PKG).tar.bz2 `rpm --eval '%_sourcedir'`
	@rpmbuild -bb --target noarch ./$(NAME).spec
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
	@sed -e 's/__VERSION__/$(VERSION)/g' debian/changelog.in > debian/changelog
	@dpkg-buildpackage -rfakeroot -b -uc -us
	@if [ -n "$(PKGDEST)" ]; then \
		FILES=../oscar-packager*.deb ;\
		echo "Moving file(s) ($$FILES) to $(PKGDEST)"; \
		for FILE in $$FILES; \
		do \
			echo "   $${FILE} --> $(PKGDEST)"; \
			mv $${FILE} $(PKGDEST); \
		done; \
	fi

