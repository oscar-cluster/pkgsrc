#!/bin/sh
#

#autoheader
aclocal
libtoolize
automake --add-missing
autoconf
