DESTDIR=

TOPDIR=..
include $(TOPDIR)/Config.mk

SUBDIRS :=
FILES := Network_Configurator.pm

all:
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} all ) ; done

install:
	install -d -m 0755 $(DESTDIR)/usr/bin
	install    -m 0755 ${FILES} $(DESTDIR)$(LIBDIR)/OSCAR
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} install ) ; done

uninstall:
	cd $(DESTDIR)$(LIBDIR)/OSCAR; rm -f ${FILES}
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} uninstall ) ; done

clean:
	@rm -f *~
	for dir in ${SUBDIRS} ; do ( cd $$dir ; ${MAKE} clean ) ; done

