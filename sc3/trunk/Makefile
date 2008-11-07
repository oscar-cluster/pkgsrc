DESTDIR=
LIBS := Subcluster.pm
SCRIPTS := scexec scrpm scpush

all:

install:
	install -d -o root -g root -m 755 $(DESTDIR)/usr/lib/systeminstaller/HPCL
	install -d -o root -g root -m 755 $(DESTDIR)/usr/bin
	install -o root -g root -m 644 ${LIBS} $(DESTDIR)/usr/lib/systeminstaller/HPCL
	install -o root -g root -m 755 ${SCRIPTS} $(DESTDIR)/usr/bin

clean:
	rm -f *~
	rm -f build-stamp configure-stamp
	rm -rf debian/files debian/sc3

deb:
	dpkg-buildpackage -rfakeroot
