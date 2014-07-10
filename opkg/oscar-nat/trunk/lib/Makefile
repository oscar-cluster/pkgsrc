DESTDIR=
PKGDEST=
PKG=

include ../Config.mk

FILES	= NAT.pm
SUBDIRS = 

all:
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) all ) ; done

install:
	install -d -m 0755 $(DESTDIR)$(LIBDIR)/OSCAR
	for file in $(FILES) ; do \
		install    -m 0755 $$file $(DESTDIR)$(LIBDIR)/OSCAR ;\
	done
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) install ) ; done

uninstall:
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) uninstall ) ; done

clean:
	for dir in $(SUBDIRS) ; do ( cd $$dir ; $(MAKE) clean ) ; done
