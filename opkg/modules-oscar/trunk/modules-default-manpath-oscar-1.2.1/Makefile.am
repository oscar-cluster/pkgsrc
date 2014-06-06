# -*- makefile -*-
#
# Copyright (c) 2005 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the modules-default-manpath-oscar software
# package.  For license information, see the LICENSE file in the
# top-level directory of the modules-default-manpath-oscar source
# distribution.
#
# $Id: Makefile.am,v 1.1 2005/02/28 23:15:21 jsquyres Exp $
#

include $(top_srcdir)/dist/Makefile.options

SUBDIRS		= dist src

EXTRA_DIST	= AUTHORS.OSCAR README.OSCAR LICENSE.OSCAR

rpm:
	if test ! -f modules-oscar-$(MORPM_VERSION).tar.gz; then \
		make dist; \
	fi
	(cd dist; ./buildrpm.sh ../modules-default-manpath-oscar-$(MORPM_VERSION).tar.gz)
