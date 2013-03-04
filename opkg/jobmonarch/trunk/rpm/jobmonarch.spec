# Don't need debuginfo RPM
%define debug_package %{nil}
%define __check_files %{nil}

Summary: Tools and addons to Ganglia to monitor and archive batch job info
Name: jobmonarch
Version: 0.4
URL: https://subtrac.sara.nl/oss/jobmonarch
Release: 0.3
License: GPL
Packager: Erich Focht (NEC HPCE)
Group: Applications/Base
Source: jobmonarch-%{version}-pre.tar.gz
Patch0: jobmonarch-0.4_pbs_python_array.patch
Patch1: jobmonarch-0.4_pbs_python_attr.patch
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}

BuildRequires: ganglia-web >= 3.1
Requires: python >= 2.3 ganglia-gmetad >= 3.0 ganglia-web >= 3.0
Requires: postgresql >= 8.1.22
Requires: postgresql-server >= 8.1.22
Requires: pyPgSQL >= 2.5.1
Requires: python-rrdtool

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

%define jobmonarchinstdir /opt/jobmonarch
%define gangliatemplatedir %{_datadir}/ganglia/templates
%define gangliaaddonsdir   %{_datadir}/ganglia/addons

%prep
%setup -q

# Patch from Daems Dirk <dirk.daems@vi...>
# Fix the fact that pbs_python now returns an array
%patch0 -p1

# Patch from  Jeffrey J. Zahari <jeffreyz@bii.a-star.edu.sg>
# Fix the retrieval of jobs attributes
%patch1 -p1

%build

%install
rm -rf $RPM_BUILD_ROOT
sed -i -e 's|/usr/sbin|%{jobmonarchinstdir}/sbin|g' pkg/rpm/init.d/jobmond pkg/rpm/init.d/jobarchived

# Restore the correct GANGLIA_PATH.
sed -i -e '/test-ganglia/d' -e 's|//$GANGLIA_PATH|$GANGLIA_PATH|g' web/addons/job_monarch/conf.php

# Set the correct PATH for the rrd database
sed -i -e 's|/path/to/my/archive|%{_sharedstatedir}/jobarchived|g' web/addons/job_monarch/conf.php

# Fix default gmond.conf location.
for FILE in ./jobmond/jobmond.conf ./jobmond/jobmond.py
do
	sed -i -e 's|/etc/gmond.conf|/etc/ganglia/gmond.conf|g' $FILE
done
# Fix gmetad.conf path (correct in ./jobarchived/jobarchived.conf but not in example)
sed -i -e 's|/etc/gmetad.conf|/etc/ganglia/gmetad.conf|g' ./jobarchived/examples/jobarchived.conf

# Fix rrdtool web link in footer:
sed -i -e 's|http://www.rrdtool.com/|http://oss.oetiker.ch/rrdtool/|g' ./web/addons/job_monarch/templates/footer.tpl

# Fix real version (0.4-pre instead of 0.3.1)
for FILE in ./jobmond/jobmond.py ./jobarchived/jobarchived.py ./web/addons/job_monarch/version.php
do
	sed -i -e 's/0.3.1/0.4-pre/g' $FILE
done

install -m 0755 -d $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin
install -m 0755 -d $RPM_BUILD_ROOT%{_initrddir}
install -m 0755 -d $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig
install -m 0755 -d $RPM_BUILD_ROOT%{gangliatemplatedir}
install -m 0755 -d $RPM_BUILD_ROOT%{gangliaaddonsdir}
install -m 0755 -d $RPM_BUILD_ROOT%{_datadir}/jobarchived/
install -m 0755 -d $RPM_BUILD_ROOT%{_sharedstatedir}/jobarchived
install -m 0644 jobmond/jobmond.conf $RPM_BUILD_ROOT%{_sysconfdir}/
sed -i -e 's|/etc/gmetad.conf|/etc/ganglia/gmetad.conf|g' jobarchived/jobarchived.conf
install -m 0644 jobarchived/jobarchived.conf $RPM_BUILD_ROOT%{_sysconfdir}/
install -m 0755 jobmond/jobmond.py $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin/jobmond
install -m 0755 jobarchived/jobarchived.py $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin/jobarchived
#install -m 0755 jobarchived/DBClass.py $RPM_BUILD_ROOT/%{jobmonarchinstdir}/sbin/
install -m 0755 pkg/rpm/init.d/jobmond $RPM_BUILD_ROOT%{_initrddir}/
install -m 0755 pkg/rpm/sysconfig/jobmond $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/
install -m 0755 pkg/rpm/init.d/jobarchived $RPM_BUILD_ROOT%{_initrddir}/
install -m 0755 pkg/rpm/sysconfig/jobarchived $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/
install -m 0755 jobarchived/job_dbase.sql $RPM_BUILD_ROOT%{_datadir}/jobarchived/
cp %{gangliatemplatedir}/default/images/logo.jpg web/templates/job_monarch/images
cp -r web/templates/job_monarch $RPM_BUILD_ROOT%{gangliatemplatedir}/job_monarch
cp -r web/addons/job_monarch $RPM_BUILD_ROOT%{gangliaaddonsdir}/job_monarch

%clean
%__rm -rf $RPM_BUILD_ROOT

%post
echo "Make sure to set your Ganglia template to job_monarch now"
echo ""
echo "In your Ganglia conf.php, set this line:"
echo "\$template_name = \"job_monarch\";"

if [ -x /sbin/chkconfig ]; then
    /sbin/chkconfig --add jobmond
    /sbin/chkconfig --add jobarchived
    if [ ! -d /var/lib/pgsql/data/base ]; then
        /sbin/service postgresql initdb
    fi
    /sbin/service postgresql start
    su -l postgres -c "/usr/bin/createdb jobarchive"
    su -l postgres -c "/usr/bin/psql -f /usr/share/jobarchived/job_dbase.sql jobarchive"
fi

%preun
if [ "$1" = 0 ]; then
    if [ -x /sbin/chkconfig ]; then
	/sbin/service jobmond stop
	/sbin/service jobarchived stop
	/sbin/chkconfig --del jobmond
	/sbin/chkconfig --del jobarchived
    fi
fi

%files
%{jobmonarchinstdir}/sbin/*
%config %{_sysconfdir}/jobmond.conf
%config %{_sysconfdir}/jobarchived.conf
%{_initddir}/*
%{_sysconfdir}/sysconfig/*
%dir %{gangliatemplatedir}/job_monarch
%{gangliatemplatedir}/job_monarch/*
%dir %{gangliaaddonsdir}/job_monarch
%{gangliaaddonsdir}/job_monarch/*
%{_datadir}/jobarchived/*
%dir %{_sharedstatedir}/jobarchived

%changelog
* Mon Mar  4 2013 Olivier Lahaye <olivier.lahaye1@free.fr> 0.4-0.3
- Added Requires: pyPgSQL python-rrdtool
- Fixed postinstall (Postgress initdb if required)
- Fixed gangliaaddonsdir
- Add %dir in file sections for gangliaaddonsdir and gangliatemplatedir
  so rpm -qf know those dirs belong to jobmonarch package.
- Fix web/addons/job_monarch/conf.php (GANGLIA_PATH and JOB_ARCHIVE_DIR)
- Fix default gmond.conf path (/etc/ganglia/gmond.conf)
- Mark %{_sharedstatedir}/jobarchived directory as part of the package
- Fix rrdtool web URL in footer
- Fix VERSION (it is a 0.4-pre, not a 0.3.1)
- Patch from Daems Dirk: new pbs_python with arrays
- Patch from Jeffrey J. Zahari: jobs attributes retrieval

* Fri May 11 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 0.4-0.2
- Update to support EPEL/RF ganglia rpm.
- Using 0.4 prerelease as there is an important bugfix over 0.3.1
- Use macros

* Fri Jul 29 2011 Olivier Lahaye <olivier.lahaye1@free.fr> 0.4-0.1
- Update to V0.4SVN

* Sun Aug 12 2006 Babu Sundaram <babu@cs.uh.edu> 0.3.1-1
- Prepare first rpm for Job Monarch's jobmond Daemon

