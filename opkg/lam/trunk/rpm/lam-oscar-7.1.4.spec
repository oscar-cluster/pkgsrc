# 
# Copyright (c) 2001-2006 The Trustees of Indiana University.  
#                         All rights reserved.
# Copyright (c) 1998-2001 University of Notre Dame. 
#                         All rights reserved.
# Copyright (c) 1994-1998 The Ohio State University.  
#                         All rights reserved.
# 
# This file is part of the LAM/MPI software package.  For license
# information, see the LICENSE file in the top level directory of the
# LAM/MPI source distribution.
# 

# This spec file creates RPMs for two packages, the main lam-oscar
# package and the lam-oscar-modulefile package.  The lam-oscar-modulefile
# package is built as noarch and lam-oscar is architecture dependent.
# What this means is by default, only the arch dependent package is
# built, if you would like to build both, execute the following:
#
# shell$ rpmbuild --rebuild lam..src.rpm --target x86_64,noarch
# (substitute "x86_64" with whatever architecture you are on)

#############################################################################
#
# Helpful defines
#
#############################################################################

# This is an OSCAR-specific RPM specfile.  It differs from the
# official LAM RPM in that it compiles/installs to OSCARPREFIX rather
# than a common system directory such as /usr and places a modulefile
# in %{_modulefiledir}.  The assumtion in this specfile is that this
# RPM will be installed into its own tree that is not shared with any
# other packages.

# This specfile offers several customizable options from the rpmbuild
# command line (a poor-man's manualy "configure" process for building
# RPMs without requiring the editing of the specfile).  The generic
# form for passing in these options to rpmbuild is:

# shell$ rpmbuild --rebuild lam...src.rpm --define "name options"

# where "name" is one of the names below, and "options" can be a
# multi-word value.  Such as:

# shell$ rpmbuild --rebuild lam...src.rpm \
#    --define "config_options CC=gcc4 CXX=g++4 FC=gfortran"

# The name of this package. The default is "lam-oscar", but other
# variants are also recognized by various OSCAR derivatives, such as
# lam-with-gm-oscar, lam-with-blcr-oscar, and
# lam-with-blcr-and-gm-oscar.

%{!?lam_name: %define lam_name lam-oscar}

# The prefix that is given to configure.  Default is /opt/lam.  The
# default configure/build process for LAM should substitute this into
# the lam_modulefile.tcl automatically.

%{!?oscar_prefix: %define oscar_prefix /opt/lam}

# Arbitrary options to pass to LAM's configure script.

%{!?config_options: %define config_options --with-blcr --with-gm=/opt/gm --with-tm=/opt/pbs --with-wrapper-extra-ldflags="-L/usr/lib64 /usr/lib64/libcr.so" --with-fc=/usr/bin/gfortran --enable-shared}

# Passed to LAM's configure script, this is meant to provide a default
# RPI for LAM.

%{!?rpi: %define rpi --with-rpi=usysv}

# Passed to LAM's configure script, this is meant to provide a default
# rsh/ssh-like agent for LAM.  The default for OSCAR clusters is ssh,
# so we pass it here.

%{!?rsh: %define rsh --with-rsh=/usr/bin/ssh}
%{!?requires: %define requires "gcc gcc-g++ gcc-g77"}

# Override default paths.

%define _prefix %{oscar_prefix}-%{version}
%define _libdir %{oscar_prefix}-%{version}/lib
%define _sysconfdir %{oscar_prefix}-%{version}/etc
%define _datadir %{oscar_prefix}-%{version}/share
%define _pkgdatadir %{oscar_prefix}-%{version}/share/lam
%define _localstatedir %{oscar_prefix}-%{version}/var/lib
%define _mandir %{oscar_prefix}-%{version}/man
%define _infodir %{oscar_prefix}-%{version}/info
%define _modulefiledir /opt/modules/modulefiles/lam

# Defining these to nothing overrides the annoying automatic RedHat
# (and friends) functionality of making "debuginfo" RPMs.

%define debug_package %{nil}
%define __check_files %{nil}


#############################################################################
#
# Preamble Section
#
#############################################################################

Summary: OSCAR-specific LAM/MPI programming environment
Name: %{lam_name}
Version: 7.1.4
Release: 2%{?dist}
Vendor: LAM/MPI Team
License: BSD
Group: Development/Libraries
Source: lam-%{version}.tar.gz
Patch0: lam_module.tcl.in.patch
Patch1: lam_blcr_64bit.patch
URL: http://www.lam-mpi.org/
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires: %__rm %__make %__mkdir %__sed %__mv %__chmod
Provides: mpi
Requires: %{requires} blcr-libs
BuildRequires: rsh %{requires} blcr-devel
AutoReqProv: no

%description 
This RPM is created specifically for the OSCAR distribution.  The main
difference between this RPM and the official LAM RPMs distributed by
the LAM Team is that it will be installed into the OSCARPREFIX tree.
Since LAM is the only package installed in OSCARPREFIX, the man pages
and documentation files are installed in slightly different places as
well (the "share" subdirectory is not really necessary).

LAM (Local Area Multicomputer) is an MPI programming environment and
development system for heterogeneous computers on a network. With
LAM/MPI, a dedicated cluster or an existing network computing
infrastructure can act as a single parallel computer.  LAM/MPI is
considered to be "cluster friendly", in that it offers daemon-based
process startup/control as well as fast client-to-client message
passing protocols.  LAM/MPI can use TCP/IP and/or shared memory for
message passing (currently, different RPMs are supplied for this --
see the main LAM web site for details).

LAM features a full implementation of MPI-1 (with the exception that
LAM does not support cancelling of sends), and much of MPI-2.
Compliant applications are source code portable between LAM/MPI and
any other implementation of MPI.  In addition to providing a
high-quality implementation of the MPI standard, LAM/MPI offers
extensive monitoring capabilities to support debugging.  Monitoring
happens on two levels.  First, LAM/MPI has the hooks to allow a
snapshot of process and message status to be taken at any time during
an application run.  This snapshot includes all aspects of
synchronization plus datatype maps/signatures, communicator group
membership, and message contents (see the XMPI application on the main
LAM web site).  On the second level, the MPI library is instrumented
to produce a cummulative record of communication, which can be
visualized either at runtime or post-mortem.

LAM/MPI is a precursor of the Open MPI project.  We encourage you to
look into upgrading to Open MPI: http://www.open-mpi.org/.

#############################################################################
#
# Preamble (modulefile) Section
#
#############################################################################

%ifarch noarch
%package modulefile
Summary: OSCAR-specific LAM/MPI modulefile
Group: Development/Libraries
Requires: modules-oscar
Requires: env-switcher
Requires: lam-oscar
AutoReqProv: no

%description modulefile
This RPM contains a modulefile for LAM/MPI on OSCAR clusters.  Loading
this module will add LAM/MPI to the PATH and MANPATH.
%endif

#############################################################################
#
# Prep Section
#
#############################################################################
%prep
%setup -q -n lam-%{version}
%patch0 -p1

%ifarch x86_64
%patch1 -p0
%endif

# Otherwise, this directory shows up on security reports
%__chmod -R o-w "$RPM_BUILD_DIR/lam-%{version}"


#############################################################################
#
# Build Section
#
#############################################################################
%build

%ifarch x86_64
# Can't figure out where to cleanly patch the configure stuff, thus do it the hard way.
for file2patch in $(grep -rl '/usr/lib/libcr.so' .)
do
  sed -i -e 's|/usr/lib/libcr.so|/usr/lib64/libcr.so|g' $file2patch
done

for file2patch in $(grep -rl 'lib="$searchdir/lib${name}${search_ext}"' .)
do
  sed -i -e 's|searchdir/lib|searchdir/lib64|g' $file2patch
done
%endif
%configure %{rpi} %{rsh} %{config_options}

%ifnarch noarch
%__make all
%endif

#############################################################################
#
# Install Section
#
#############################################################################
%install
%__rm -rf "$RPM_BUILD_ROOT"
%ifnarch noarch
%makeinstall

# Rename the ROMIO doc files so that we can install them in the same
# doc root later, and not overwrite LAM's doc files.

for file in README README_LAM COPYRIGHT; do
	%__mv $RPM_BUILD_DIR/lam-%{version}/romio/$file \
		$RPM_BUILD_DIR/lam-%{version}/romio/romio-$file
done
%__mv $RPM_BUILD_DIR/lam-%{version}/romio/doc/users-guide.ps.gz \
	$RPM_BUILD_DIR/lam-%{version}/romio/doc/romio-users-guide.ps.gz

# lam_module.tcl is generated by configure but not installed.  Copy
# (and rename) it into its final destination.

%else
%__mkdir -p "$RPM_BUILD_ROOT/%{_modulefiledir}"
%__cp $RPM_BUILD_DIR/lam-%{version}/config/lam_module.tcl "$RPM_BUILD_ROOT/%{_modulefiledir}/%{name}-%{version}"
%endif

# We don't use the shell setup files anymore.

%ifnarch noarch
%__rm -f lam-shell-setup.sh
%__rm -f lam-shell-setup.csh

# Many RPM installations automatically run "strip" (in one form or
# another) on all executables in the %files tree.  However, for proper
# TotalView support, we need to have debugging symbols available in
# both libmpi and mpirun.  Normally, stripping is a good thing, and we
# don't want to turn it off in all cases.  So we somehow have to fool
# it.  :-(

# Turn off the x bit on the totalview shared library so that rpm
# doesn't strip it.  #$%@#$%!!!  Only do this on platforms where the
# totalview library is built (currently: 32 bit platforms).

if test -f $RPM_BUILD_ROOT%{_libdir}/lam/liblam_totalview*so; then
    %__chmod a-x $RPM_BUILD_ROOT%{_libdir}/lam/liblam_totalview*so 2> /dev/null || :
fi
%endif

#############################################################################
#
# Clean Section
#
#############################################################################
%clean
%__rm -rf "$RPM_BUILD_ROOT"


#############################################################################
#
# Files Section
#
#############################################################################
%ifnarch noarch
%files
%defattr(-,root,root)
%doc LICENSE HISTORY INSTALL README
%doc examples
%doc doc/*.pdf
# Need to fix ROMIO install script to install its docs in the Right place
%doc %{_pkgdatadir}/*
%config %{_sysconfdir}
%{_bindir}/*
%{_mandir}/*
%{_includedir}/*
%{_libdir}/*
%dir %{_prefix}
%dir %{_bindir}
%dir %{_includedir}
%dir %{_libdir}
%dir %{_mandir}
%dir %{_datadir}
%dir %{_pkgdatadir}
%else

#############################################################################
#
# Files (modulefile) Section
#
#############################################################################
%files modulefile
%defattr(-,root,root)
%{_modulefiledir}/%{name}-%{version}
%dir %{_modulefiledir}
%endif

#############################################################################
#
# Changelog
#
#############################################################################
%changelog
* Fri May 21 2010 Olivier Lahaye <olivier.lahaye1@free.fr> 7.1.4-2
- Patch for x86_64 blcr (Berkley Checkpoint/Restart) support

* Tue Apr 04 2006 Bernard Li <bli@bcgsc.ca>
- modulefile was conflicting mpi instead of lam

* Fri Mar 31 2006 Bernard Li <bli@bcgsc.ca>
- package modulefile can now be built as noarch

* Thu Mar 30 2006 Bernard Li <bli@bcgsc.ca>
- Added %dir in %files section

* Mon Mar 27 2006 Bernard Li <bli@bcgsc.ca>
- lam_module.tcl.in.patch is incorrectly referencing LAM_PREFIX for PATH, it
  should be LAM_BINDIR

* Fri Mar 17 2006 Bernard Li <bli@bcgsc.ca>
- Fixed %define config_options line for --without-blcr such that it builds
  correctly, also fixed typo "/opt/pbss" -> "/opt/pbs"

* Thu Jan 6 2005 Jeff Squyres <jsquyres@lam-mpi.org>
- Bring over lots of changes from the main lam-generic.spec file
  (mainly because OSCAR was previously still shipping LAM 7.0.x, not
  7.1.x)
  - Kill the epoch
  - Ditch the supplemental tarfile for forcing rpm to not strip the
    totalview DSO

* Thu Jan 1 2004 Jeff Squyres <jsquyres@lam-mpi.org>
- Change name of "module" sub-RPMs to be "modulefile".
- Modulefiles now are installed into a new location that is
  independant of switcher; removed switcher-based preun/post
  scriptlets.  These modulefiles are now un/loaded by a dispatcher
  modulefile in a separate RPM.  This allows multiple LAM
  installations with different capabilities (e.g., BLCR, GM, etc.) to
  be installed, and the dispatcher modulefile will pick the right
  installation to use.  This will all be obsolete in LAM 7.1 when we
  have dynamic SSI modules, and it won't be necessary to have multiple
  LAM installations.
- Significantly cleaned up the modulefile installation in the install
  scriptlet.
- Removed blcr patch (it's now in the main LAM distribution).
- Added a few more requires (tar, chmod).
- Convert to use proper forms of system commands (__mv, etc).

* Tue Nov 11 2003 Jeff Squyres <jsquyres@lam-mpi.org>
- Add configure script options for gm, tm, and blcr
- Add ability to do "with" kinds of things, and disable
  auto-generation of dependencies, and instead depend on the main
  lam-oscar RPM (which has all the auto-dependencies)
- Disable automatic RH9 debuginfo RPM generation
- Remove mpi++.h sym links
- Made SSS-specific blcr RPM
- Fix ROMIO doc files for RH9
- Added patch for BLCR module configure scripts to make it compile
  properly when BLCR is installed in /usr (for existing LAM/MPI 7.0.2
  -- to be removed in future versions of this specfile when 7.0.3 is
  released)
- Stupidness because RH9 RPM automatically strips all binaries and
  libraries.  Some of our binaries and libraries *need* the debugging
  symbols for debugger support (e.g., TotalView).  So tar up a good
  copy of the desired files (e.g., mpirun) during % install, and then
  un-tar them during % post.  This is totally ugly, but there does not
  appear to be an easy, portable, and forward-compaible way of turning
  off the stripping that occurs automatically as part of the
  RPM-building process.

* Wed Sep 24 2003 Jeff Squyres <jsquyres@lam-mpi.org>
- Added explicit requires for gcc, gcc-c++, and gcc-g77.

* Mon May 29 2003 Jeff Squyres <jsquyres@lam-mpi.org>
- Updated for LAM/MPI 7.0; added doc/*.pdf; removed some other
  (outdated) %doc files
- Ensure extracted LAM source directory is not world-writeable;
  otherwise it shows up in nightly security reports
- Fixed references to get ROMIO docs

* Sun Aug 11 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Require "modules-oscar", not "modules" (the RPM was renamed to not
  conflict with the Linux kernel "modules" RPM).
- Change Requires of the module package to be "lam-oscar" to match the
  new LAM RPM name.

* Wed May 15 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Removed the profile-d sub RPM because I'm tired of everyone copying
  the LAM .spec file and making profile-f sub RPMs.

* Sun May 11 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added Requires line for a bunch of basic unix utilities to ensure
  that LAM is not installed before things like cp, mv, etc.
- Robust-ize the lam-module RPM so that it won't bail in %preun if
  switcher and/or modules have previously been uninstalled

* Sun Apr 21 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Fixed so that main LAM RPM does not include the modulefile that is
  in lam-module RPM.  This involved moving the generation of the lam
  modulefile and shell setup files to pkgdatadir, not sysconfdir.
- Removed "| cat" from switcher commands; not necessary any more (were
  initally for debugging)
- Added "--silent" to the switcher commands so that we don't see the
  normal and expected warning messages.

* Thu Apr 18 2002 Brian William Barrett <brbarret@lam-mpi.org>
- Updated Epoch to 2 - Make us prefered over RH's stoopid RPM
- Fixed the handling of the profile.d and modules scripts so that
  someone can actually use --rebuild on a LAM rpm.

* Sun Apr  7 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Modified the module sub-package to depend on the switcher package
  and to call the switcher to add/remove itself during the %pre and
  %postun.  Also, no longer copy the modulefile into the
  /opt/modules/modulefiles, instead leave it in %{_sysconfdir} and let
  switcher copy it wherever it wants to copy it.

* Sun Feb 17 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added mechanisms to install/remove 
  /opt/oscar/modules/modulesfiles/mpi/lam-<version>
- Removed wildcards from %files lists, because this allows the
  directories themselves to be listed, so rpm -e will actually remove
  the directories (but only if they are empty, so it's safe).
- Added %postun section to actually remove the %{_prefix} directory
  upon RPM uninstall (but only if it is empty)
- Make three sub-package RPMs: main LAM, the profile.d scripts, and
  the modulefile.  The lam-profile-d RPM depends only on the LAM RPM,
  but the lam-module RPM depends both on LAM and the module RPM.

* Sun Feb 10 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added mechanisms to install/remove /etc/profile.d/mpi-01lam.* files.

* Wed Oct 31 2001 Jeff Squyres <jsquyres@lam-mpi.org>
- Changed license to be BSD.

* Thu Jul 05 2001 Jeff Squyres <jsquyres@lam-mpi.org>
- Changed to use %config directive to treat sysconf files as
  configuration files that may have local-admin-installed changes.
- Fixed to get the copyright properly.
- Changed "Copyright" to "License" to be a bit more accurate.

* Mon Jun 04 2001 Jeff Squyres <jsquyres@lam-mpi.org>
- Adapted the standard LAM RPM spec file for OSCAR.

* Mon Mar 17 2001 Brian William Barrett <bbarrett@lam-mpi.org>
- Borrowed SPEC file from Trond at RedHat.  Converted to be able to 
  make RPMs for all RPIs
