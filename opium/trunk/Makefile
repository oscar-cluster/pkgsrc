PKG=ssh-oscar

FILES := ssh-oscar.csh ssh-oscar.sh Makefile install postinst

all:

install:
	./install.pl $(DESTDIR)

rpm: dist
	cp $(PKG).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -bb ./$(PKG).spec

deb:
	cp -f postinst debian/
	dpkg-buildpackage -rfakeroot

clean:
	rm -f *~
	rm -f ./$(PKG).tar.gz
	rm -f *.rpm
	rm -f debian/postinst

dist: clean
	rm -rf /tmp/$(PKG)
	mkdir /tmp/$(PKG)
	cp -rf ${FILES} /tmp/$(PKG)
	cd /tmp; tar czf ./$(PKG).tar.gz $(PKG)
	mv /tmp/$(PKG).tar.gz .
