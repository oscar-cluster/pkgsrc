DESTDIR=

TOPDIR=..
include $(TOPDIR)/Config.mk

SUBDIRS :=
FILES := network-configurator

doc:
	install -d -m 0755 $(DESTDIR)/$(MANDIR)/man1/
	for bin in ${FILES} ; do ( pod2man --section=1 $$bin $(DESTDIR)/$(MANDIR)/man1/$$bin.1 ) ; done

all:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

install: doc
	install -d -m 0755 $(DESTDIR)/usr/bin
	install    -m 0755 ${FILES} $(DESTDIR)/usr/bin
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done

uninstall:
	cd $(DESTDIR)/usr/bin; rm -f ${FILES}
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} uninstall ) ; done

clean:
	@rm -f *~
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} clean ) ; done

