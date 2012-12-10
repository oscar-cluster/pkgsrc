# -*- makefile -*-
#
# Copyright (c) 2002 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the modules-oscar software package.  For
# license information, see the LICENSE file in the top-level directory
# of the modules-oscar source distribution.
#
# $Id: Makefile.am,v 1.3 2002/10/27 13:16:06 jsquyres Exp $
#

include $(top_srcdir)/dist/Makefile.options

SUBDIRS		= dist src

EXTRA_DIST	= AUTHORS.OSCAR README.OSCAR LICENSE.OSCAR

rpm:
	if test ! -f modules-oscar-$(MORPM_VERSION).tar.gz; then \
		make dist; \
	fi
	(cd dist; ./buildrpm.sh ../modules-3.1.6.tar.gz \
		../modules-oscar-$(MORPM_VERSION).tar.gz)
