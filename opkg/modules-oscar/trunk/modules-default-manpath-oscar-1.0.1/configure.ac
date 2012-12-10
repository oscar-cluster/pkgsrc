# -*- shell-script -*-
#
# Copyright (c) 2005 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the modules-default-manpath-oscar software
# package.  For license information, see the LICENSE file in the
# top-level directory of the modules-default-manpath-oscar source
# distribution.
#
# $Id: configure.ac,v 1.1 2005/02/28 23:15:21 jsquyres Exp $
#

# Init autoconf

AC_INIT(./src/default-manpath.tcl)
AC_CONFIG_AUX_DIR(./dist)

show_title() {
  cat <<EOF

*** $1
EOF
}

show_title "Initialization, setup"

# Init automake

get_version="sh $srcdir/dist/get_morpm_version $srcdir"
MORPM_VERSION="`eval $get_version --full`"
MAJOR_VERSION="`eval $get_version --major`"
MINOR_VERSION="`eval $get_version --minor`"
RELEASE_VERSION="`eval $get_version --release`"
ALPHA_VERSION="`eval $get_version --alpha`"
BETA_VERSION="`eval $get_version --beta`"
AC_DEFINE_UNQUOTED(MAJOR_VERSION, $MAJOR_VERSION)
AC_DEFINE_UNQUOTED(MINOR_VERSION, $MINOR_VERSION)
AC_DEFINE_UNQUOTED(RELEASE_VERSION, $RELEASE_VERSION)
AC_DEFINE_UNQUOTED(ALPHA_VERSION, $ALPHA_VERSION)
AC_DEFINE_UNQUOTED(BETA_VERSION, $BETA_VERSION)
AC_DEFINE_UNQUOTED(VERSION, "$MORPM_VERSION")

# Need to also AC_SUBST these for share/include/patchlevel.h and
# share/include/mpif.h

AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(RELEASE_VERSION)
AC_SUBST(ALPHA_VERSION)
AC_SUBST(BETA_VERSION)
AC_SUBST(MORPM_VERSION)

echo "Configuring modules/modules-default-manpath-oscar version $MORPM_VERSION"

#
# The third argument to AM_INIT_AUTOMAKE surpresses the PACKAGE and
# VERSION macors
#

AM_INIT_AUTOMAKE(modules-default-manpath-oscar, $MORPM_VERSION, 'no')

#
# Make automake clean emacs ~ files for "make clean"
#

CLEANFILES="*~ .*~"
AC_SUBST(CLEANFILES)

#
# All done
#

AC_OUTPUT([
    Makefile

    dist/Makefile

    src/Makefile
])
