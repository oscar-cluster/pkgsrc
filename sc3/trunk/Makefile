DESTDIR=
SOURCEDIR=/usr/src/redhat/SOURCES

NAME:=sc3
LIBS := Subcluster.pm
SCRIPTS := scexec scrpm scpush


all:

install:
	install -d -o root -g root -m 755 $(DESTDIR)/usr/lib/systeminstaller/HPCL
	install -d -o root -g root -m 755 $(DESTDIR)/usr/bin
	install -o root -g root -m 644 ${LIBS} $(DESTDIR)/usr/lib/systeminstaller/HPCL
	install -o root -g root -m 755 ${SCRIPTS} $(DESTDIR)/usr/bin

dist: clean
	@rm -rf /tmp/$(NAME)
	@mkdir -p /tmp/$(NAME)
	@cp -rf * /tmp/$(NAME)
	@cd /tmp/$(NAME); rm -rf `find . -name ".svn"`
	@cd /tmp; tar czf $(NAME).tar.gz $(NAME)
	@cp -f /tmp/$(NAME).tar.gz .
	@rm -rf /tmp/$(NAME)/
	@rm -f /tmp/$(NAME).tar.gz

clean:
	rm -f *~
	rm -f build-stamp configure-stamp
	rm -rf debian/files debian/sc3
	rm -f $(NAME).tar.gz

deb:
	dpkg-buildpackage -rfakeroot

rpm: dist
	cp $(NAME).tar.gz $(SOURCEDIR)
	rpmbuild -bb ./sc3.spec
