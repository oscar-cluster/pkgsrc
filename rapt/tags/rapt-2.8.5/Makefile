DESTDIR=
PKGDEST=
DEBTMP=/tmp/rapt
BUILDTMP=/tmp/rapt-build
VERSION=2.8.5
PKG=rapt

all:

install:
	install -d $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/usr/share/man/man8/
	install -m 644 rapt.8 $(DESTDIR)/usr/share/man/man8/rapt.8
	install -m 755 rapt $(DESTDIR)/usr/bin/rapt

uninstall:
	rm -f $(DESTDIR)/usr/share/man/man8/rapt.8
	rm -f $(DESTDIR)/usr/bin/rapt

rpm ::
	@echo "RAPT is not yet supported on RPM system"

deb ::
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

dist: mrproper
	rm -rf $(BUILDTMP); \
	mkdir -p $(BUILDTMP)/$(PKG)-$(VERSION); \
	cp -rf * $(BUILDTMP)/$(PKG)-$(VERSION); \
	PWD=`pwd`; \
	cd $(BUILDTMP); rm -rf `find . -name .svn`; \
	tar czf $(PKG)-$(VERSION).tar.gz $(PKG)-$(VERSION); \
	cp $(PKG)-$(VERSION).tar.gz $(PWD);

mrproper: clean
	rm -f build-stamp configure-stamp
	rm -f debian/files
	rm -rf debian/$(PKG)

clean:
	rm -f *~
	rm -f $(PKG)-*.tar.gz
	rm -rf deb
