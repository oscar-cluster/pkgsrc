dnl
dnl   Configure script for V2M
dnl
dnl   Copyright 2006-2007, Oak Ridge National Laboratory
dnl   See file LICENCE for conditions.
dnl

AC_INIT(qemu.cpp)

LIBV3M_VERSION="0.9.8"

AM_INIT_AUTOMAKE(libv3m,$LIBV3M_VERSION)

dnl AM_CONFIG_HEADER([config.h])

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_CPP
AC_PROG_INSTALL
dnl LT_INIT
dnl LT_LANG([C++])

dnl === Check for libxml++ 2.6 =================================================

dnl Check first for pkg-config

AC_PATH_PROGS([PACKAGE_CONFIG], [pkg-config], [none])
if test "x$PACKAGE_CONFIG" = "xnone"; then

        AC_MSG_ERROR([*** pkg-config, needed to check for libxml++ existence
has not been found.])
fi

AC_MSG_CHECKING(for libxml++)

if $PACKAGE_CONFIG libxml++-2.6 --libs > /dev/null 2>&1; then
        AC_MSG_RESULT(yes)
else
        AC_MSG_RESULT(no)
        AC_MSG_ERROR([*** libxml++ version 2.6 not found!])
fi
LIBXMLPP_CFLAGS=`$PACKAGE_CONFIG libxml++-2.6 --cflags`
LIBXMLPP_LIBS=`$PACKAGE_CONFIG libxml++-2.6 --libs`
CFLAGS="$CFLAGS $LIBXMLPP_CFLAGS"
CXXFLAGS="$CXXFLAGS $LIBXMLPP_CFLAGS"
LIBS="$LIBS $LIBXMLPP_LIBS"

dnl ============================================================================

dnl ============================================================================
dnl === Test for doxygen; code here and in Makefile.am tests for ===============
dnl === equality with 'NO_DOXYGEN' =============================================
AC_PATH_PROG(DOXYGEN, doxygen, NO_DOXYGEN)
if test "$DOXYGEN" = NO_DOXYGEN; then
    AC_MSG_NOTICE([Couldn't find Doxygen -- I'll just create a stub library
    document.  Doxygen's good though: you can get it from www.doxygen.org])
fi

AC_SUBST(DOXYGEN_OUTPUT_DIRECTORY)
DOXYGEN_OUTPUT_DIRECTORY="doc"
dnl ============================================================================

dnl === Update the Doxygen configuration ===
sed -e "s/LIB_V3M_VERSION/$LIBV3M_VERSION/" < doxygen_config.in \
        > doxygen_config

AC_OUTPUT(Makefile)
