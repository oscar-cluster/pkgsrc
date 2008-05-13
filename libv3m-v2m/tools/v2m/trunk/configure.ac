dnl
dnl   Configure script for V2M
dnl
dnl   Copyright 2006-2007, Oak Ridge National Laboratory
dnl   See file LICENCE for conditions.
dnl

AC_INIT(v2m, [0.9.1])
AM_INIT_AUTOMAKE

dnl AC_CONFIG_HEADERS([config.h])
dnl AM_CONFIG_HEADER

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_RANLIB

echo "#ifndef __VERSION_H
#define __VERSION_H
#define VERSION $VERSION
#endif" > version.h

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

dnl === Check for libv3m =======================================================

# Ask user for path to libv3m stuff:.
AC_CHECK_LIB(v3m, exit, , [
  AC_ARG_WITH(libv3m, 
    [ --with-libv3m=<path>     path to libv3m C++ library and headers. 
                               e.g.  --with-libv3m=/usr/local or /usr],
    [LIBV3M_PREFIX=$with_libv3m],
    AC_MSG_ERROR([You must call configure with the --with-libv3m option.
    This tells configure where to find the libv3m C++ library and headers.
    e.g. --with-libv3m=/usr/local or --with-libv3m=/usr])
    )
  ]
)

dnl ============================================================================

LIBV3M_PREFIX="/usr/local/lib"
AC_SUBST(LIBV3M_PREFIX)
LIBV3M_LIBS="-L${LIBV3M_PREFIX} -lv3m"
LIBV3M_CFLAGS="-I${LIBV3M_PREFIX}"
AC_SUBST(LIBV3M_LIBS)
AC_SUBST(LIBV3M_CFLAGS)
CFLAGS="$CFLAGS $LIBV3M_CFLAGS"
CXXFLAGS="$CXXFLAGS $LIBV3M_CFLAGS"
LIBS="$LIBS $LIBV3M_LIBS"


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
dnl =======================================================================

dnl === Update the Doxygen configuration ===
sed -e "s/V2M_VERSION/$VERSION/" < doxygen_config.in \
        > doxygen_config

dnl AM_CONFIG_HEADER(config.h)

AC_OUTPUT(Makefile)
