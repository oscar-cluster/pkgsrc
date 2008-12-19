PKG=ssh-oscar

BINDIR=etc/profile.d

FILES := ssh-oscar.csh ssh-oscar.sh Makefile

all:

install:
	@echo "Installing Perl modules in $(DESTDIR)/$(BINDIR)"
	install -d -m 0755 $(DESTDIR)/$(BINDIR)
	install    -m 0755 ${FILES} $(DESTDIR)/$(BINDIR)

rpm: dist
	cp $(PKG).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -bb ./$(PKG).spec

clean:
	rm -f *~
	rm -f ./$(PKG).tar.gz

dist: clean
	rm -rf /tmp/$(PKG)
	mkdir /tmp/$(PKG)
	cp -rf ${FILES} /tmp/$(PKG)
	cd /tmp; tar czf ./$(PKG).tar.gz $(PKG)
	mv /tmp/$(PKG).tar.gz .
