# Name of package
%define name maui-oscar
%define torque_name torque-oscar

# Version of maui
%define version 3.3.1

# Maui installation directory
%define maui_prefix /opt/maui

# Do we want to use PBS as the resource manager (1=yes 0=no)
%define yes_pbs 1

# Installation directory base prefix
%define pbs_prefix /opt/pbs

# Execution home/spool directory for PBS
%define pbs_server_home /var/lib/torque

# Name of file that contains the default server for clients to use
%define server_name_file server_name

# Default server name
%define default_server pbs_oscar

#==============================================================

# Options for Maui version:
# rpm -ba|--rebuild --define "pbs 1"
%{?pbs:%define yes_pbs 1}
%{?nopbs:%define yes_pbs 0}

#==============================================================

Summary: OSCARified Maui Scheduler
Name: %{name}
Version: %{version}
Release: 4%{?dist}
Packager: Bernard Li <bli@bcgsc.ca>
URL: http://www.clusterresources.com/pages/products/maui-cluster-scheduler.php
Source0: maui-%{version}.tar.gz
%define untarring_directory maui-%{version}
Source1: maui-oscar-extra.tgz
Patch0: maui-torque4.patch

License: Maui Scheduler General Public License
Group: Applications/batch
Obsoletes: maui
BuildRequires: rpm >= 3.0.5
BuildRequires: %{torque_name}-client %{torque_name}-server %{torque_name}
Requires: %{torque_name}-client %{torque_name}-server %{torque_name}
Requires: glibc >= 2.2.4
BuildRoot: %{_tmppath}/%{name}-buildroot

#==============================================================

%description
Maui is an advanced job scheduler for use on clusters and supercomputers.
It is a highly configurable tool capable of supporting a large array of
fairness policies, dynamic priorities, extensive reservations, and fairshare.
It is currently in use at many of the leading government and academic labs
throughtout the US and around the world.  It is running on machines ranging
from clusters of a few processors to multi-teraflop supercomputers.

This version of Maui has been modified slightly to be usable under the
OSCAR cluster software system.

#==============================================================

%prep

%setup -q -n %{untarring_directory}
%setup -a 1 -n %{untarring_directory}
%patch0

%build

export LDFLAGS=-L/opt/pbs/lib64
%configure --prefix=%{maui_prefix} --with-spooldir=%{maui_prefix} --with-key=21303 --with-pbs=%{pbs_prefix}
%{__make}  %{?_smp_mflags}

%install
%__make BUILDROOT=${RPM_BUILD_ROOT} install

# Install the service init script
install -m 0755 -d ${RPM_BUILD_ROOT}/%{_initrddir}
if [ -d ${RPM_BUILD_ROOT}/%{_initrddir} ] ; then
%if %{?suse_version:1}0
	cp -p maui.SuSE ${RPM_BUILD_ROOT}/%{_initrddir}/maui
%else
	cp -p maui ${RPM_BUILD_ROOT}/%{_initrddir}
%endif
fi

# Install user profile scripts
install -m 0755 -d ${RPM_BUILD_ROOT}/%{_sysconfdir}/profile.d
install -p -m 0755 etc/maui.sh ${RPM_BUILD_ROOT}/%{_sysconfdir}/profile.d/
install -p -m 0755 etc/maui.csh ${RPM_BUILD_ROOT}/%{_sysconfdir}/profile.d/

install -m 0755 -d ${RPM_BUILD_ROOT}%{maui_prefix}/traces/
#cp $RPM_BUILD_DIR/%{untarring_directory}/traces/* ${RPM_BUILD_ROOT}%{maui_prefix}/traces/
%__make BUILDROOT=${RPM_BUILD_ROOT} setup
cp -f LICENSE LICENSE.mcompat CHANGELOG ${RPM_BUILD_ROOT}%{maui_prefix}


%post
perl -pi -e "s~SERVERHOST.*~SERVERHOST\t\t`hostname`~" %{maui_prefix}/maui.cfg
perl -pi -e "s~RMPOLLINTERVAL.*~RMPOLLINTERVAL\t00:00:10~" %{maui_prefix}/maui.cfg
perl -pi -e "s~BACKFILLPOLICY.*~BACKFILLPOLICY\tON~" %{maui_prefix}/maui.cfg
perl -pi -e "s~RMCFG\[.*\]~RMCFG[$HOSTNAME]~" %{maui_prefix}/maui.cfg
perl -pi -e 's~\@RMNMHOST@~~' %{maui_prefix}/maui.cfg
perl -pi -e "s~ADMIN1.*~ADMIN1\t\t\troot~" %{maui_prefix}/maui.cfg
echo "NODEACCESSPOLICY	DEDICATED" >> %{maui_prefix}/maui.cfg
if [ -e /sbin/chkconfig ] ; then
	/sbin/chkconfig --add maui
fi

%clean
rm -rf ${RPM_BUILD_ROOT}

#==============================================================

%preun
if [ $1 = 0 ]; then
# This is an uninstall
  /sbin/service maui status && /sbin/service maui stop
  /sbin/chkconfig --del maui
else
  # This is an upgrade
  /sbin/service maui status && /sbin/service maui restart
fi

#==============================================================

%files

%{maui_prefix}/bin
%{maui_prefix}/include
%{maui_prefix}/lib
%{maui_prefix}/log
%{maui_prefix}/sbin
%{maui_prefix}/spool
%{maui_prefix}/stats
%{maui_prefix}/tools
%{maui_prefix}/traces
%{maui_prefix}/CHANGELOG
%{maui_prefix}/LICENSE
%{maui_prefix}/LICENSE.mcompat
%config %{maui_prefix}/maui.cfg
%config %{maui_prefix}/maui-private.cfg
%{_initrddir}/maui
%{_sysconfdir}/profile.d/maui.*sh

#==============================================================

%changelog
* Sun Dec 15 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 3.3.1-4
- Re-enabled automatic dependancy generator.
- Rebuilt with torque 4.1.7 stable.
* Tue Dec 11 2012 Olivier Lahaye 3.3.1-3
- Rebuild with torque-4.1.4.snap.201211201307
- Use LDFLAGS to ease libtorque.so link.
* Tue Nov 15 2011 Olivier Lahaye 3.3.1-2
- Fix build with torque-4.1.3
- Spec cleanup.
- Fixed %preun when upgrading
- Add user profiles scripts.
* Thu Apr 22 2010 Olivier Lahaye 3.3.1-1
- New release.
* Mon Feb 05 2007 Erich Focht 3.2.6p18.5
- Using maui-3.2.6p19-snap.1169758944 snapshot of p19. According to the
  mailing lists the bugs seen in 3.2.6p18 should be fixed here. As this
  is not yet the official p19 release, I called it p18.5.
* Tue Jan 30 2007 Erich Focht 3.2.6p18-4nec
- Updated to 3.2.6p18 : fixes issue with building with newer torque
- Removed patch for BUILD_ROOT added by BLi, the version already has support
  for that
- Added ifdefs for suse/non-suse in order to deal with /etc/init.d/pbs_server
  location
* Fri Jun 23 2006 Bernard Li <bli@bcgsc.ca> 3.2.6p14-4
- Substitute RMCFG from buildhost's hostname to the hostname of the
  machine that RPM is being installed on
- Hardcode ADMIN1 to be the root user

* Thu Jun 15 2006 Bernard Li <bli@bcgsc.ca>
- Added "status" for SUSE init script

* Wed Jun 14 2006 Bernard Li <bli@bcgsc.ca> 3.2.6p14-3
- Added SUSE init script (adapted from Moab script by Martin Siegert)
- Removed src/ and maui.cfg.old from maui-oscar-extra.tgz

* Wed Oct 26 2005 Bernard Li <bli@bcgsc.ca>
- Update to 3.2.6p13
- Removed expect script

* Thu Oct 28 2004 David N. Lombard <dnl@speakeasy.net>
- Use canonical /etc/init.d path; use RPM_BUILD_ROOT, not live system!!!
- Updated to 3.2.5p2-9

* Thu Sep 2 2004 Benoit des Ligneris <benoit.des.ligneris@revolutionlinux.com>
- Remove the dependency on openpbs for building and install

* Thu Apr 17 2003 Jason Brechin <brechin@ncsa.uiuc.edu>
- Updated to "new" 3.2.5p2

* Wed Mar 05 2003 Jason Brechin <brechin@ncsa.uiuc.edu>
- Updated to 3.2.5p2 and added an expect script to do configure

* Wed Sep 25 2002 Jeremy Enos <jenos@ncsa.uiuc.edu>
- Changed maui.cfg so NODEACCESSPOLICY is DEDICATED.

* Thu Jul 25 2002 Jason Brechin <brechin@ncsa.uiuc.edu>
- Minor updates and spec file changes.

* Fri Jun 14 2002 Jason Brechin <brechin@ncsa.uiuc.edu>
- Updated to 3.0.7p8

* Thu Aug 23 2001 Neil Gorsuch <ngorsuch@ncsa.uiuc.edu>
- Initial RPMification
