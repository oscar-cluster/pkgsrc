DESTDIR=
DEBTMP=/tmp/rapt
BUILDTMP=/tmp/rapt-build
VERSION=2.8.1

all:

install:
	install -d $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/usr/share/man/man8/
	install -m 644 rapt.8 $(DESTDIR)/usr/share/man/man8/rapt.8
	install -m 755 rapt $(DESTDIR)/usr/bin/rapt

uninstall:
	rm -f $(DESTDIR)/usr/share/man/man8/rapt.8
	rm -f $(DESTDIR)/usr/bin/rapt

deb ::
	rm -rf $(DEBTMP)
	mkdir -p $(DEBTMP)
	cp -rf * $(DEBTMP)
	cd $(DEBTMP); rm -rf `find . -name .svn`; dpkg-buildpackage -rfakeroot
	echo "The Debian package is ready in /tmp"

dist: mrproper
	rm -rf $(BUILDTMP); \
	mkdir -p $(BUILDTMP)/rapt-$(VERSION); \
	cp -rf * $(BUILDTMP)/rapt-$(VERSION); \
	PWD=`pwd`; \
	cd $(BUILDTMP); rm -rf `find . -name .svn`; \
	tar czf rapt-$(VERSION).tar.gz rapt-$(VERSION); \
	cp rapt-$(VERSION).tar.gz $(PWD);

mrproper: clean
	rm -f build-stamp configure-stamp
	rm -f debian/files
	rm -rf debian/rapt

clean:
	rm -f *~
	rm -f rapt-*.tar.gz
	rm -rf deb
