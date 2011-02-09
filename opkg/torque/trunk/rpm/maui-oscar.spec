Summary: OSCARified Maui Scheduler
Name: %{name}
Version: %{version}
Release: 8
Packager: Bernard Li <bli@bcgsc.ca>
URL: http://www.clusterresources.com/pages/products/maui-cluster-scheduler.php
Source0: maui-%{version}.tar.gz
%define untarring_directory maui-%{version}
Source1: maui-oscar-extra.tgz

License: Maui Scheduler General Public License
Group: Applications/batch
Obsoletes: maui
BuildPreReq: rpm >= 3.0.5
# pbs/torque client
BuildPreReq: /opt/pbs/bin/qalter
# pbs/torque server
%if %{?suse_version:1}0
BuildPreReq: /etc/init.d/pbs_server
Requires: /etc/init.d/pbs_server
%else
BuildPreReq: /etc/rc.d/init.d/pbs_server
Requires: /etc/rc.d/init.d/pbs_server
%endif
# pbs/torque 
BuildPreReq: /var/spool/pbs/pbs_environment
#BuildPreReq: glibc >= 2.2.4
AutoReqProv: no
Requires: rpm >= 3.0.5
# pbs/torque client
Requires: /opt/pbs/bin/qalter
# pbs/torque 
Requires: /var/spool/pbs/pbs_environment
Requires: glibc >= 2.2.4
BuildRoot: %{_tmppath}/%{name}-buildroot


#==============================================================

%prep

%setup -q -n %{untarring_directory}
%setup -a 1 -n %{untarring_directory}
#%patch0 -p1

%build

./configure --prefix=%{maui_prefix} --with-spooldir=%{maui_prefix} --with-key=21303 --with-pbs=%{pbs_prefix}
make

%install
make BUILDROOT=${RPM_BUILD_ROOT} install
install -m 0755 -d ${RPM_BUILD_ROOT}/etc/init.d
if [ -d ${RPM_BUILD_ROOT}/etc/init.d ] ; then
%if %{?suse_version:1}0
    cp -p maui.SuSE ${RPM_BUILD_ROOT}/etc/init.d/maui
%else
    cp -p maui ${RPM_BUILD_ROOT}/etc/init.d
%endif
fi
install -m 0755 -d ${RPM_BUILD_ROOT}%{maui_prefix}/traces/
#cp $RPM_BUILD_DIR/%{untarring_directory}/traces/* ${RPM_BUILD_ROOT}%{maui_prefix}/traces/
make BUILDROOT=${RPM_BUILD_ROOT} setup
cp -f LICENSE LICENSE.mcompat CHANGELOG ${RPM_BUILD_ROOT}%{maui_prefix}


%post
if [ -e /sbin/chkconfig ] ; then
    /sbin/chkconfig --add maui
fi
perl -pi -e "s~SERVERHOST.*~SERVERHOST\t\t`hostname`~" %{maui_prefix}/maui.cfg
perl -pi -e "s~RMPOLLINTERVAL.*~RMPOLLINTERVAL\t00:00:10~" %{maui_prefix}/maui.cfg
perl -pi -e "s~BACKFILLPOLICY.*~BACKFILLPOLICY\tON~" %{maui_prefix}/maui.cfg
perl -pi -e "s~RMCFG\[.*\]~RMCFG[$HOSTNAME]~" %{maui_prefix}/maui.cfg
perl -pi -e 's~\@RMNMHOST@~~' %{maui_prefix}/maui.cfg
perl -pi -e "s~ADMIN1.*~ADMIN1\t\t\troot~" %{maui_prefix}/maui.cfg
echo "NODEACCESSPOLICY  DEDICATED" >> %{maui_prefix}/maui.cfg

%clean
rm -rf ${RPM_BUILD_ROOT}

#==============================================================

%preun
if [ -e /etc/init.d/maui ] ; then
  /etc/init.d/maui stop
fi
if [ "$1" = 0 ] ; then
  if [ -e /sbin/chkconfig ] ; then
    /sbin/chkconfig --del maui
  fi
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
/etc/init.d/maui

#==============================================================

%changelog
* Wed Feb 09 2011 Geoffroy Vallee
- Create a separate spec file for maui so we can maintain the different packagesmore easily.
