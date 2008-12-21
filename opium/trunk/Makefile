PKG=ssh-oscar

FILES := ssh-oscar.csh ssh-oscar.sh Makefile install

all:

install:
	./install $(DESTDIR)

rpm: dist
	cp $(PKG).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -bb ./$(PKG).spec

deb:
	dpkg-buildpackage -rfakeroot

clean:
	rm -f *~
	rm -f ./$(PKG).tar.gz
	rm -f *.rpm

dist: clean
	rm -rf /tmp/$(PKG)
	mkdir /tmp/$(PKG)
	cp -rf ${FILES} /tmp/$(PKG)
	cd /tmp; tar czf ./$(PKG).tar.gz $(PKG)
	mv /tmp/$(PKG).tar.gz .
