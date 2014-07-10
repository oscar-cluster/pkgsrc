DESTDIR=

TOPDIR=..
include $(TOPDIR)/Config.mk

SCRIPTS = oscar-nat
SUBDIRS =

man:
	install -d -m 0755 $(DESTDIR)$(MANDIR)/man1/
	for bin in $(SCRIPTS) ; do ( pod2man --section=1 $$bin $(DESTDIR)$(MANDIR)/man1/$$bin.1 ) ; done

all:
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) all ) ; done

install: man
	install -d -m 0755 $(DESTDIR)/usr/bin
	for script in $(SCRIPTS) ; do \
		install    -m 0755 $$script $(DESTDIR)/usr/bin ; \
	done
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) install ) ; done

uninstall:
	cd $(DESTDIR)/usr/bin; rm -f $(SCRIPTS)
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) uninstall ) ; done

clean:
	@rm -f *~
