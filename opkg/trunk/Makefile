#
# Makefile for opkg tools
#
TOOLS=opkg-upload

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

INSTALL=$(shell which install)
install_bin=$(INSTALL) -m 755

all: $(TOOLS)

opkg-upload:

install: $(TOOLS)
	mkdir -p $(BINDIR)
	for tool in $(TOOLS); do \
	  $(install_bin) $$tool $(BINDIR)/; \
	done
