# no packaged files checking
%define _unpackaged_files_terminate_build 0

# Name of package
%define base mpich

# define compilers
%define compiler gcc
%define ccompiler gcc
%define cxxcompiler g++
%define fcompiler g77
%define f90compiler g77

# Device we're building for
%define device ch_p4
%define devicename %{device}

# Version of software
%define version 1.2.7

# rpm release
%define release 8

# Name of package including oscar indication
%define name %{base}-%{devicename}-%{compiler}-oscar

# Do we want to enable building of the switcher module rpm? (1=yes 0=no)
%define yes_module 1
%define module_name %{base}-%{devicename}-%{compiler}-%{version}

# Installation directory base prefix
%define prefix /opt/%{base}-%{devicename}-%{compiler}-%{version}

# switcher data directory for this package
%define switchdatadir %{prefix}/share/%{base}

#==============================================================

# Options for Module version:
# rpm -ba|--rebuild --define "module 1"
%{?module:%define yes_module 1}
%{?nomodule:%define yes_module 0}

#==============================================================

Summary: Portable Implementation of MPI (Message-Passing Interface).
Name: %{name}
Version: %{version}
Release: %{release}
Packager: Erich Focht
URL: http://www-unix.mcs.anl.gov/mpi/mpich
BuildRoot:/var/tmp/%{name}-%{version}
Source: %{base}-%{version}.tar.bz2
%define untarring_directory %{base}-%{version}
Source3: mpich_module.tcl.in
Patch0: mpich-mpe32bit.patch
Patch1: mpich-symlink.patch

Vendor: OSCAR Open Cluster Group
License: freely available, read COPYRIGHT
Group: Applications/cluster

Obsoletes: %{base}
#BuildRequires: /usr/bin/symlinks
Requires: rpm >= 3.0.5
AutoReqProv: no
Requires: glibc

%description
MPICH is a freely available, portable implementation of MPI <http://www.mcs.anl.gov/mpi/index.html>, the Standard for message-passing libraries.

This package is built with the %{device} device.

#==============================================================

%package module
Summary: Portable Implementation of MPI OSCAR-specific modulefile
Group: Applications/cluster
BuildPreReq: rpm >= 3.0.5
Requires: rpm >= 3.0.5
AutoReqProv: no
Requires: modules-oscar
Requires: switcher
Requires: %{prefix}/bin
Requires(post): switcher
Conflicts: mpich-oscar-profiled

%description module
This RPM contains a modulefile for MPICH on OSCAR clusters for
use with the OSCAR environment switcher. Loading this module 
will add MPICH to the PATH and MANPATH when mpich is selected 
through the switcher.

#==============================================================

%prep
%setup -q -n %{untarring_directory}
%patch0 -p1
%patch1 -p0


%build
export RSHCOMMAND="ssh -x"
export CFLAGS="-O3 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64"
export FFLAGS="-O3"

export PREFIX=$RPM_BUILD_ROOT%{prefix}

export CC=%{ccompiler}
export FC=%{fcompiler}
export F90=%{f90compiler}

%define use_gfortran %(test `which gfortran` && echo 1 || echo 0 )
%if %use_gfortran
  %define fcompiler gfortran
  %define f90compiler gfortran
%else
  %define fcompiler g77
  %define f90compiler g77
%endif

export CC=%{ccompiler}
export FC=%{fcompiler}
export F90=%{f90compiler}
if [ "$FC" = "gfortran" ]; then
    export F77_GETARGDECL=" "
fi

%ifnarch ia64
[ -f configure.in ] && libtoolize --copy --force;
%endif

./configure --with-device=%{device} -prefix=$PREFIX \
  --with-arch=LINUX \
  --disable-devdebug \
  -cflags="$CFLAGS" -fflags="$FFLAGS" \
  -mpe_opts=--enable-upshot=no \
  --enable-shared \
  --with-romio=--with-file_system=nfs+ufs \
  -opt="-O3" \
  -cc=%{ccompiler} -f77=%{fcompiler} \
  -f90=%{f90compiler} -c++=%{cxxcompiler} \
  -clinker=%{ccompiler} -flinker=%{fcompiler} -c++linker=%{cxxcompiler} \


make

%install
#make install PREFIX=$RPM_BUILD_ROOT%{prefix}
make install PREFIX=$RPM_BUILD_ROOT%{prefix}

mkdir -p $RPM_BUILD_ROOT%{switchdatadir}
sed -e 's,@prefix@,%{prefix},g' \
        $RPM_SOURCE_DIR/mpich_module.tcl.in \
	> $RPM_BUILD_ROOT%{switchdatadir}/%{module_name}
chmod 644 $RPM_BUILD_ROOT%{switchdatadir}/%{module_name}

perl -pi -e "s#$RPM_BUILD_ROOT##g" `find $RPM_BUILD_ROOT -type f ! -name '*.a'`

#/usr/bin/symlinks -c $RPM_BUILD_ROOT%{prefix}/examples
#/usr/bin/symlinks -c $RPM_BUILD_ROOT%{prefix}/share/examples
ls -al $RPM_BUILD_ROOT%{prefix}/share/examples

%clean
rm -rf $RPM_BUILD_ROOT

#==============================================================

%post module

# Add ourselves into the switcher repository.  Run the modules and
# switcher setup scripts because this RPM install may be part of a
# massive "rpm -ivh ..." that includes the modules and switcher RPMs
# themselves -- in which case, the environment for those packages will
# not yet have been setup.

f=/etc/profile.d/00-modules.sh
if [ -r $f ] ; then
	. $f
	switcher mpi --add-name %{module_name} %{switchdatadir} \
	--force --silent 2>/dev/null || echo switcher not found, ignoring
else
	echo modules not found, ignoring
fi


%preun module

# Remove ourselves from the switcher repository.  Run the modules and
# switcher setup scripts because this RPM install may be part of the
# same shell that "rpm -ivh ..." the switcher/modules RPM's, in which
# case, the environment for those packages will not yet have been
# setup.  This is important to do *before* we are uninstalled because
# of the case where "rpm -Uvh lam-module..." is used; the current RPM
# will be uninstalled and then the new one will be installed.  If we
# are %postun here, then the new RPM will be installed, and then this
# will run, which will remove the [new] tag from switcher, which is
# obviously not what we want.

# Grrr...  It seems that "rpm -ivh a.rpm b.rpm c.rpm" is smart enough
# to re-order the order of installation to ensure that dependencies
# are met.  However, "rpm -e a b c" does *not* order the
# uninstallations to ensure that dependencies are still met.  So if
# someone does "rpm -e switcher modules lam-module", it is quite
# possible that rpm will uninstall switcher and/or modules *before*
# this RPM is uninstalled.  As such, the following lines will fail,
# which will cause all kinds of Badness.  Arrggh!!  So we have to test
# to ensure that these files are still here before we try to use them.

f=/etc/profile.d/00-modules.sh
if [ -r $f ] ; then
	. $f
        # Find default mpi and switch to none if we're the system default
        sysdef=`switcher mpi --show --system | grep default | cut -d '=' -f 2`
        if [ "$sysdef" != "%{module_name}" ]; then
                switcher mpi = none --system --force --silent
        fi
	switcher mpi --rm-name %{module_name} --force \
	--silent 2>/dev/null || echo switcher not found, ignoring
else
	echo modules not found, ignoring
fi

#==============================================================

%files 
%dir %{prefix}/bin
%{prefix}/etc
#%{prefix}/log
%{prefix}/man
%{prefix}/sbin
%{prefix}/share/Makefile.sample
%{prefix}/share/examples/*
%{prefix}/share/machines*
%{prefix}/www
%{prefix}/bin
%{prefix}/include
%{prefix}/lib
%{prefix}/examples
%{prefix}/doc

%files module
%{switchdatadir}/%{module_name}

#==============================================================

%changelog
* Fri Sep 23 2005 Erich Focht
- added back part of the %post scriptlet
* Tue Sep 20 2005 Erich Focht
- using prepend in module file
* Mon Sep 19 2005 Erich Focht
- fixed files: eliminated module file from main package
- eliminated %post scriptlet (not needed).
* Tue Jan 25 2005 Erich Focht
- eliminated devel package, simplified %file section
* Tue Jan 18 2005 Erich Focht
- unifying spec for all devices, archs, versions
* Wed Apr 16 2003 Jason Brechin <brechin@ncsa.uiuc.edu>
- New naming scheme
* Thu May 09 2002 Neil Gorsuch <ngorsuch@ncsa.uiuc.edu>
- Initial RPMification
