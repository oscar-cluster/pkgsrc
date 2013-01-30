# Don't need debuginfo RPM
%define debug_package %{nil}
%define __check_files %{nil}

Summary: Tools and addons to Ganglia to monitor and archive batch job info
Name: jobmonarch
Version: 0.3.1
URL: https://subtrac.sara.nl/oss/jobmonarch
Release: 1
License: GPL
Packager: Erich Focht (NEC HPCE)
Group: Applications/Base
Source: jobmonarch-%{version}.tar.gz
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}

BuildRequires: ganglia-web >= 3.1
Requires: python >= 2.3 ganglia-gmetad >= 3.0 ganglia-web >= 3.0

# Following requires were moved to the config.xml file in order to keep the
# RPM distro-independent
#Requires: mysql-client python-mysql
#%if %suse_version
#Requires: php5-gd >= 2.0 php5-mbstring
#%else
#Requires: php-gd >= 2.0 php-mbstring
#%endif
AutoReqProv: no


%description
Job Monarch is a set of tools to monitor and optionally archive (batch)job
information. It is a addon for the Ganglia monitoring system and plugs into an
existing Ganglia setup. jobmond is the job monitoring daemon that gathers
PBS/Torque/SGE batch statistics on jobs/nodes and submits them into Ganglia's
XML stream. jobarchived is the Job Archiving Daemon. It listens to Ganglia's
XML stream and archives the job and node statistics. It stores the job
statistics in a Postgres SQL database and the node statistics in RRD
files. Through this daemon, users are able to lookup a old/finished job and
view all it's statistics. The Job Monarch web frontend interfaces with the
jobmond data and (optionally) the jobarchived and presents the data and
graphs. It does this in a similar layout/setup as Ganglia itself, so the
navigation and usage is intuitive.

#define jobmonarchinstdir /opt/jobmonarch
%define jobmonarchinstdir /usr

%prep
%setup -q

%build

%install
rm -rf $RPM_BUILD_ROOT
install -m 0755 -d $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin
install -m 0755 -d $RPM_BUILD_ROOT/etc/init.d
install -m 0755 -d $RPM_BUILD_ROOT/etc/sysconfig
install -m 0755 -d $RPM_BUILD_ROOT/var/www/html/ganglia/templates
install -m 0755 -d $RPM_BUILD_ROOT/var/www/html/ganglia/addons
install -m 0644 jobmond/jobmond.conf $RPM_BUILD_ROOT/etc/
install -m 0644 jobarchived/jobarchived.conf $RPM_BUILD_ROOT/etc/
install -m 0755 jobmond/jobmond.py $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin/jobmond
install -m 0755 jobarchived/jobarchived.py $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin/jobarchived
#install -m 0755 jobarchived/DBClass.py $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin/
install -m 0755 pkg/rpm/init.d/jobmond $RPM_BUILD_ROOT/etc/init.d/jobmond
install -m 0755 pkg/rpm/init.d/jobarchived $RPM_BUILD_ROOT/etc/init.d/jobarchived
install -m 0755 pkg/rpm/sysconfig/jobmond $RPM_BUILD_ROOT/etc/sysconfig/jobmond
install -m 0755 pkg/rpm/sysconfig/jobarchived $RPM_BUILD_ROOT/etc/sysconfig/jobarchived

cp /var/www/html/ganglia/templates/default/images/logo.jpg web/templates/job_monarch/images
cp -r web/templates/job_monarch $RPM_BUILD_ROOT/var/www/html/ganglia/templates/job_monarch
cp -r web/addons/job_monarch $RPM_BUILD_ROOT/var/www/html/ganglia/addons/job_monarch

%clean
%__rm -rf $RPM_BUILD_ROOT

%post
if [ -x /sbin/chkconfig ]; then
    /sbin/chkconfig --add jobmond
    /sbin/chkconfig --add jobarchived
fi

%preun
if [ "$1" = 0 ]; then
    if [ -x /sbin/chkconfig ]; then
	/etc/init.d/jobmond stop
	/etc/init.d/jobarchived stop
	/sbin/chkconfig --del jobmond
	/sbin/chkconfig --del jobarchived
    fi
fi

%files
%{jobmonarchinstdir}/sbin/*
%config /etc/jobmond.conf
%config /etc/jobarchived.conf
%config /etc/sysconfig/jobmond
%config /etc/sysconfig/jobarchived
/etc/init.d/*
/var/www/html/ganglia/templates/job_monarch/*
/var/www/html/ganglia/addons/job_monarch/*

%changelog

* Sun Aug 12 2006 Babu Sundaram <babu@cs.uh.edu>
- Prepare first rpm for Job Monarch's jobmond Daemon

