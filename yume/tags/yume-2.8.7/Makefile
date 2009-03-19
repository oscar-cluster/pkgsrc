DESTDIR=
PKGDEST=
BINDIR=$(DESTDIR)/usr/bin
DATADIR=$(DESTDIR)/usr/share
MANDIR=$(DESTDIR)/usr/share/man
SOURCEDIR=/usr/src/redhat/SOURCES
NAME=yume
VERSION=2.8.7

FILES := ChangeLog ptty_try yume yume-opkg-4.2.1 yume-opkg yum-repoquery3 \
		rhel4-i386.rpmlist yume.8 yume.spec Makefile rhel4-x86_64.rpmlist \
		yum-repoquery 
MANSCRIPTS := ptty_try
DEBIANFILES := debian/changelog debian/compat debian/control debian/copyright \
		debian/rules

all:

manpages:
	install -d -m 0755 $(DESTDIR)/usr/local/man/man1/
	for bin in ${MANSCRIPTS} ; do ( pod2man --section=1 $$bin $(DESTDIR)/usr/local/man/man1/$$bin.1 ) ; done

install: manpages
	install -d -o root -g root -m 755 $(BINDIR)
	install -d -o root -g root -m 755 $(DATADIR)/$(NAME)
	install -d -o root -g root -m 755 $(MANDIR)/man8
	install -o root -g root -m 755  yume $(BINDIR)
	install -o root -g root -m 755  yume-opkg $(BINDIR)
	install -o root -g root -m 755  yum-repoquery $(BINDIR)
	install -o root -g root -m 755  ptty_try $(BINDIR)
	install -o root -g root -m 755  *.rpmlist $(DATADIR)/$(NAME)
	install -o root -g root -m 755  yume.8 $(MANDIR)/man8

dist: clean
	mkdir -p /tmp/$(NAME)-$(VERSION)/debian
	cp ${FILES} /tmp/$(NAME)-$(VERSION)
	cp ${DEBIANFILES} /tmp/$(NAME)-$(VERSION)/debian
	cd /tmp; tar czf $(NAME)-$(VERSION).tar.gz $(NAME)-$(VERSION)
	mv /tmp/$(NAME)-$(VERSION).tar.gz .

clean:
	rm -f *~
	rm -rf debian/files debian/yume
	rm -f build-stamp configure-stamp
	rm -rf /tmp/$(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz

deb:
	@if [ -n "$$UNSIGNED_OSCAR_PKG" ]; then \
        echo "dpkg-buildpackage -rfakeroot -us -uc"; \
        dpkg-buildpackage -rfakeroot -us -uc; \
    else \
        echo "dpkg-buildpackage -rfakeroot"; \
        dpkg-buildpackage -rfakeroot; \
    fi
	@if [ -n "$(PKGDEST)" ]; then \
        mv ../$(NAME)*.deb $(PKGDEST); \
    fi

rpm: dist
	cp $(NAME)-$(VERSION).tar.gz $(SOURCEDIR)
	rpmbuild -bb ./$(NAME).spec
	@if [ -n "$(PKGDEST)" ]; then \
        mv `rpm --eval '%{_topdir}'`/RPMS/noarch/$(NAME)-*.noarch.rpm $(PKGDEST); \
    fi
