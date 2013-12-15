# Copyright (c) 2002-2005, Najib Ninaba <najib@eml.cc>
# All rights reserved.
# Copyright (C) 2005-2006 Bernard Li <bli@bcgsc.ca>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright owner nor the names of contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# This spec file creates both arch dependent and arch independent RPMs,
# therefore it is necessary to pass the 'noarch' argument to target when
# you want to rebuild the noarch RPM as well, eg.:
#
# % rpmbuild --bb sge.spec --target noarch,i686
#
# By default, it should just build the arch dependent RPMs

%define appuser sge
%define prefix /opt/sge
%define dirname gridengine

# Don't need debuginfo RPM
%define debug_package %{nil}
%define __check_files %{nil}

%define is_fc5 0
%define is_fc %(test -f /etc/fedora-release && echo 1 || echo 0)
%if %is_fc
  %define fc_ver %(cat /etc/fedora-release | awk {'print $4'})
  %define is_fc5 %(test %fc_ver -gt 4 && echo 1 || echo 0)
%endif

%define xorg_lt7 %(X -version 2>&1 | grep "X Window System Version" | awk '{print ($5 < "7.0.0")}')

Summary: Sun Grid Engine -- Distributed Resource Management software
Name: sge
Version: 6.0u9
Release: 9oscar
Group: Applications/System
License: SISSL
URL: http://gridengine.sunsource.net/
Source0: %{name}-V60u9_TAG-src.tar.gz
Source1: oscar_cluster.conf
Source2: LICENSE
Source3: %{name}-modulefile
Source4: %{name}-lam-pe-scripts.tar.gz
Source5: %{name}-pvm-pe-scripts.tar.gz
Source6: README.pvm
Source7: README.pe
Source8: %{name}-openmpi-pe-scripts.tar.gz
%ifnarch noarch
Requires: openmotif
Requires: sed coreutils /bin/sh /bin/ksh
BuildRequires: openmotif-devel, ncurses-devel, libgcj-devel
BuildRequires: pvm
%if %{_vendor} == "suse"
%if %{sles_version} == 10
BuildRequires: xorg-x11-devel
%else
BuildRequires: xorg-x11-libX11-devel xorg-x11-libXpm-devel
%endif
%else
# The crap below is for RedHat and alike
%if %{xorg_lt7}
BuildRequires: xorg-x11-devel
%else
BuildRequires: libX11-devel libXpm-devel
%endif
%endif
%endif
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Patch0: inst_common.sh.patch
Patch1: inst_execd.sh.patch
Patch2: qmon_bigicon.patch
Patch3: qtcsh.sh.h.patch
# The following patch (aimk.patch) removes all "-Werror"
# from the aimk script. Otherwise, the SGE build fails
# after warnings (not errors) when built against
# recent versions of glibc - Babu Sundaram, 04/13/06
Patch4: aimk.patch
Patch5: qmon_geometryp_h.patch
Patch6: pvm_rsh.patch

%ifarch noarch
%package modulefile
Summary: Sun Grid Engine modulefile
Requires: %{name} = %{version}-%{release}
Group: Applications/System
BuildPreReq: rpm >= 3.0.5
Requires: modules-oscar >= 3.1 /bin/sed
%endif

%description
In a typical network that does not have distributed resource management
software, workstations and servers are used from 5% to 20% of the time.
Even technical servers are generally less than fully utilized. This
means that there are a lot of cycles that can be used productively if
only users know where they are, can capture them, and put them to work.

Sun[tm] Grid Engine finds a pool of idle resources and harnesses it
productively, so an organization gets as much as five to ten times the
usable power out of systems on the network. That can increase utilization
to as much as 98%.

Sun Grid Engine software aggregates available compute resources and
delivers compute power as a network service.

These are the local files shared by both the qmaster and execd
daemons. You must install this package in order to use any one of them.

%ifarch noarch
%description modulefile
In a typical network that does not have distributed resource management
software, workstations and servers are used from 5% to 20% of the time.
Even technical servers are generally less than fully utilized. This
means that there are a lot of cycles that can be used productively if
only users know where they are, can capture them, and put them to work.

Sun[tm] Grid Engine finds a pool of idle resources and harnesses it
productively, so an organization gets as much as five to ten times the
usable power out of systems on the network. That can increase utilization
to as much as 98%.

Sun Grid Engine software aggregates available compute resources and
delivers compute power as a network service.

This package includes a modulefile for SGE on OSCAR cluster.  It
is used to set SGE_ROOT, PATH, MANPATH and LD_LIBRARY_PATH.
%endif

%prep
if [ -z "$PVM_ROOT" ]; then
    echo "\$PVM_ROOT not set!  Please set and re-run rpmbuild."
    exit 1
fi

# create a top level directory
%ifnarch noarch
%setup -q -n %{dirname}
%setup -a 4 -D -n %{dirname}
%setup -a 5 -D -n %{dirname}
%setup -a 8 -D -n %{dirname}
%patch0 -p0
%patch1 -p0
%patch2 -p0
%patch4 -p0
%patch5 -p0
%patch6 -p0
%endif

%ifarch i386
%patch3 -p1
%endif

%ifarch i686
%patch3 -p1
%endif

%build
if [ "$JAVA_HOME" == '' ]; then
    export JAVA_HOME=''
fi

%ifnarch noarch
cd source && \
touch aimk && \
./aimk -only-depend && \
scripts/zerodepend && \
./aimk depend && \
./aimk -spool-classic -no-secure && \
./aimk -man 
%endif

%install 
%ifnarch noarch
# set up the target installation directories
export SGE_ROOT=$RPM_BUILD_ROOT%{prefix}
export SGE_ARCH=`source/dist/util/arch`
%__mkdir -p $SGE_ROOT
%__mkdir -p $SGE_ROOT/pe/lam
%__mkdir -p $SGE_ROOT/pe/pvm/bin/$SGE_ARCH
%__mkdir -p $SGE_ROOT/pe/openmpi

cd source && \
echo 'y'| scripts/distinst -nobdb -noopenssl -local -allall -noexit $SGE_ARCH
install -m644 %{SOURCE1} $SGE_ROOT/util/install_modules
install -m644 %{SOURCE2} $SGE_ROOT
install -m644 %{SOURCE7} $SGE_ROOT/pe

# LAM/MPI Parallel Environment integration scripts
cd ..
install -m755 startlam.sh stoplam.sh rsh $SGE_ROOT/pe/lam
install -m644 README.lam $SGE_ROOT/pe/lam

# PVM Parallel Environment integration scripts
cd pvm/src && \
./aimk
cd $SGE_ARCH
install -m755 spmd master slave start_pvm stop_pvm $SGE_ROOT/pe/pvm/bin/$SGE_ARCH
cd ../..
install -m755 hostname rsh startpvm.sh stoppvm.sh $SGE_ROOT/pe/pvm
install -m644 README $SGE_ROOT/pe/pvm
install -m644 %{SOURCE6} $SGE_ROOT/pe/pvm

# Open MPI Parallel Environment integration scripts
cd ../openmpi
install -m755 startopenmpi.sh stopopenmpi.sh rsh $SGE_ROOT/pe/openmpi
install -m644 README.openmpi $SGE_ROOT/pe/openmpi

%else
mkdir -p $RPM_BUILD_ROOT/opt/modules/oscar-modulefiles/%{name}
install -m644 %{SOURCE3} $RPM_BUILD_ROOT/opt/modules/oscar-modulefiles/%{name}/%{version}
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%post
%ifnarch noarch

# Only do the following if this the first install
if [ "$1" = "1" ]; then
  # Add SGE services
  # sge_qmaster       536/tcp
  # sge_execd         537/tcp
  if [ "`grep ^sge_qmaster /etc/services`" == "" ]; then
      echo "sge_qmaster       536/tcp      # communication port for Grid Engine" >> /etc/services
  fi

  if [ "`grep ^sge_execd /etc/services`" == "" ]; then
      echo "sge_execd         537/tcp      # communication port for Grid Engine" >> /etc/services
  fi

  # Add the "SGE" group and user
  if [ -x /usr/sbin/useradd.real ]
  then
      USERADD=/usr/sbin/useradd.real
  else
      USERADD=/usr/sbin/useradd
  fi

  /usr/sbin/groupadd -r sge
  $USERADD -c "Sun Grid Engine" \
  -s /sbin/nologin -r -d %{prefix} -g sge sge 2> /dev/null || :
fi

# Set file permission
chown -R sge.sge %{prefix}
%endif

%preun
%ifnarch noarch

# Stop SGE services
if [ -x /etc/init.d/sgemaster ]; then
  /etc/init.d/sgemaster stop
#else
#  ps -ef | grep "sge_qmaster|sge_schedd" | awk {'print $2'} | xargs kill -9
fi

if [ -x /etc/init.d/sgeexecd ]; then
  /etc/init.d/sgeexecd stop
#else 
#  ps -ef | grep "sge_execd" | awk {'print $2'} | xargs kill -9 
fi
%endif

%postun
%ifnarch noarch
if [ "$1" = "0" ]; then

  # Remove SGE services
  if [ "`grep ^sge_qmaster /etc/services`" != "" ]; then
      grep -v ^sge_qmaster /etc/services > /tmp/services.tmp
      mv -f /tmp/services.tmp /etc/services
  fi

  if [ "`grep ^sge_execd /etc/services`" != "" ]; then
      grep -v ^sge_execd /etc/services > /tmp/services.tmp
      mv -f /tmp/services.tmp /etc/services
  fi

  # Unregister the SGE service
  /sbin/chkconfig --del sgemaster > /dev/null 2>&1 
  /sbin/chkconfig --del sgeexecd  > /dev/null 2>&1

  # Cleanup SGE init.d scripts if they exist
  if [ -x /etc/init.d/sgemaster ]
  then
     rm -rf /etc/init.d/sgemaster
  fi

  if [ -x /etc/init.d/sgeexecd ]
  then
     rm -rf /etc/init.d/sgeexecd
  fi

  # Remove SGE user
  if [ -x /usr/sbin/userdel.real ]
  then
      USERDEL=/usr/sbin/userdel.real
  else
      USERDEL=/usr/sbin/userdel
  fi

  $USERDEL sge > /dev/null 2>&1 
  /usr/sbin/groupdel sge 2> /dev/null || :
fi
%endif   

%ifnarch noarch
%files
%defattr(-, root,root)
%{prefix}
%else

%files modulefile
%dir /opt/modules/oscar-modulefiles/%{name}
/opt/modules/oscar-modulefiles/%{name}/%{version}
%endif

%changelog
* Mon Oct 15 2007 Erich Focht 6.0u9-8oscar
- Updated to 6.0u9
- Added ifdefs for dependencies for suse/sles, which are different from the rhel/fc.
* Tue Oct 6 2007 DongInn Kim <dikim@osl.iu.edu> 6.0u8-7
- Drop the "requires" of XFree86-devel because it is deprecated and 
- xorg-X11 satisfies the requirements of XFree86-devel.
- xorg-X11 is installed to the most distros by default.

* Tue Sep 19 2006 Bernard Li <bli@bcgsc.ca> 6.0u8-6
- Fixed patch for PVM, replace || with &&.

* Tue Aug 28 2006 Bernard Li <bli@bcgsc.ca> 6.0u8-5
- Removed -no-java -no-jni from aimk call such that the resulting
  libdrmaa.so works with Java bindings
- Added libgcj-devel to BuildRequires (for libdrmaa.so)
- Patch for PVM integration script for hostname, see:
  http://svn.oscar.openclustergroup.org/trac/oscar/ticket/217

* Fri Jul 07 2006 Bernard Li <bli@bcgsc.ca> 6.0u8-4
- Patch for qmon spinbox issue under x86_64 2.6 kernel (and others) 
- Moved useradd call from %pre to %post
- Removed sge from Provides since this is implicitly provided
- Adapted %post and %postun to handle RPM upgrades better
- Building of sge-modulefile no longer extracts the sge source etc.
- Removed drmaa from Provides since libdrmaa.so() is already provided

* Wed Jun 07 2006 Babu Sundaram <babu@cs.uh.edu> 6.0u8-3
- Added 'drmaa' to 'Provides' tag so DRMAA bindings can be installed

* Tue Jun 06 2006 Bernard Li <bli@bcgsc.ca> 6.0u8-2
- Made sure that $RPM_BUILD_ROOT/opt/sge is created
- Added BuildRequires ncurses-devel
- Moved code to create target installation directories to install section
- Call groupadd prior to useradd (workaround for SUSE Linux) and groupdel
  after userdel

* Mon May 8 2006 Babu Sundaram <babu@cs.uh.edu> 6.0u8-1
- Updated the spec file to use the latest source SGE6.0u8

* Thu Apr 27 2006 Bernard Li <bli@bcgsc.ca> 6.0u7_1-3
- Added Open MPI integration scripts

* Sun Apr 23 2006 Bernard Li <bli@bcgsc.ca> 6.0u7_1-2
- Fixed spec file so it builds on Fedora Core 5

* Thu Apr 13 2006 Babu Sundaram <babu@cs.uh.edu> 6.0u7_1-1
- Updated the spec file to use the latest SGE6.0u7_1
- Added a new patch (#4 - aimk.patch) to remove -Werror in aimk

* Wed Jan 11 2006 Bernard Li <bli@bcgsc.ca> 6.0u7-2
- Added XFree86-devel to BuildRequires
- Prevent building of debuginfo RPM

* Mon Dec 19 2005 Bernard Li <bli@bcgsc.ca>
- Do not hardcode UID for sge user creation
- Changed %{appdir} and $RPM_INSTALL_PREFIX/sge to %{prefix}
- modulefile now uses $SGE_ROOT/util/arch to determine correct PATHs for SGE

* Sat Dec 10 2005 Bernard Li <bli@bcgsc.ca> 6.0u7-1
- Update to 6.0u7, change %{name} to "sge"

* Mon Nov 21 2005 Bernard Li <bli@bcgsc.ca>
- Stop sgemaster/sgeexecd daemons before un-installation
- Clean up init.d scripts too

* Mon Nov 07 2005 Bernard Li <bli@bcgsc.ca>
- Added LAM/MPI loose qrsh integration scripts

* Thu Nov 03 2005 Bernard Li <bli@bcgsc.ca>
- Added modulefile for SGE to set SGE_ROOT, and update MANPATH, PATH and LD_LIBRARY_PATH

* Tue Nov 01 2005 Bernard Li <bli@bcgsc.ca>
- Put LICENSE in /opt/sge
- Include LAM/MPI Parallel Environment scripts by Reuti

* Tue Nov 01 2005 Babu Sundaram <babu@cs.uh.edu> 6.0u6
- Edited the spec file to use SGE's 6.0u6 distro
- Edited the installation location to /opt/sge
- Removed old patches (aimk- and distinst-related patches) as the new update compiles fine without them
- Added a section to check for i686/i386 and then apply qtcsh.sh.h patch (Patch3); Not needed for 64-bit
- Added the SISSL license to be copied over with the sources
- Removed all ROCKS-specific file copying (such as the MPI templates and so on)

* Tue Jul 05 2005 Tsai Li Ming <me@ltsai.com> 6.0u4
- Prepare first rpm for 6.0u4
