# Don't need debuginfo RPM
%define debug_package %{nil}
%define __check_files %{nil}

%define gangliatemplatedir %{_datadir}/ganglia/templates
%define gangliaaddonsdir   %{_datadir}/ganglia/addons

Summary: Tools and addons to Ganglia to monitor and archive batch job info
Name: jobmonarch
Version: 1.0
URL: https://oss.trac.surfsara.nl/jobmonarch
Release: 1%{?dist}
License: GPL
Packager: Olivier Lahaye <olivier.lahaye@cea.fr>
Group: Applications/Base
Source: ganglia_jobmonarch-%{version}.tar.bz2
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}
Requires: jobmonarch-jobarchived jobmonarch-jobmond jobmonarch-webfrontend

# Following requires were moved to the config.xml file in order to keep the
# RPM distro-independent
#Requires: mysql-client python-mysql
#%if %suse_version
#Requires: php5-gd >= 2.0 php5-mbstring
#%else
#Requires: php-gd >= 2.0 php-mbstring
#%endif
#AutoReqProv: no

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

This package is the meta package that for installation of all jobmonarche components:
(jobmonarch-jobarchived - jobmonarch-jobmond - jobmonarch-webfrontend)


%package -n jobmonarch-jobarchived
Summary: jobarchived is the archiving daemon for jobmonarch.
Requires: python >= 2.5
Requires: pbs_python
Requires: postgresql >= 8.1.22
Requires: postgresql-server >= 8.1.22
Requires: pyPgSQL >= 2.5.1
Requires: python-rrdtool

%description -n jobmonarch-jobarchived
jobmonach-jobarchived is the Job Archiving Daemon. It listens to Ganglia's
XML stream and archives the job and node statistics. It stores the job
statistics in a Postgres SQL database and the node statistics in RRD
files. Through this daemon, users are able to lookup a old/finished job and
view all it's statistics.

%package -n jobmonarch-jobmond
Summary: jobmond is the job monitoring daemon for jobmonarch.
Requires: python >= 2.5
Requires: pbs_python
Requires: postgresql >= 8.1.22
Requires: postgresql-server >= 8.1.22
Requires: pyPgSQL >= 2.5.1
Requires: python-rrdtool

%description -n jobmonarch-jobmond
jobmonarch-jobmond is the job monitoring daemon that gathers PBS/Torque/SGE
batch statistics on jobs/nodes and submits them into Ganglia's XML stream.

%package -n jobmonarch-webfrontend
Summary: webfrontend is the ganglia webfrontend for jobmonarch.
Requires: ganglia-gmetad >= 3.5.7 ganglia-web >= 3.5.7
Requires: jobmonarch-jobmond = 1.0

%description -n jobmonarch-webfrontend
 The Job Monarch web frontend interfaces with the
jobmond data and (optionally) the jobarchived and presents the data and
graphs. It does this in a similar layout/setup as Ganglia itself, so the
navigation and usage is intuitive.

%prep
%setup -q -n ganglia_jobmonarch-%{version}

%build

%install
rm -rf $RPM_BUILD_ROOT
#sed -i -e 's|/usr/sbin|%{jobmonarchinstdir}/sbin|g' pkg/rpm/init.d/jobmond pkg/rpm/init.d/jobarchived

# Restore the correct GANGLIA_PATH.
#sed -i -e '/test-ganglia/d' -e 's|/$GANGLIA_PATH|$GANGLIA_PATH|g' web/addons/job_monarch/conf.php

# Set the correct PATH for the rrd database
sed -i -e 's|/path/to/my/archive|%{_sharedstatedir}/jobarchived|g' web/addons/job_monarch/conf.php

# Fix default gmond.conf location.
#for FILE in ./jobmond/jobmond.conf ./jobmond/jobmond.py
#do
#	sed -i -e 's|/etc/gmond.conf|/etc/ganglia/gmond.conf|g' $FILE
#done
# Fix gmetad.conf path (correct in ./jobarchived/jobarchived.conf but not in example)
#sed -i -e 's|/etc/gmetad.conf|/etc/ganglia/gmetad.conf|g' ./jobarchived/examples/jobarchived.conf

# Fix rrdtool web link in footer:
sed -i -e 's|http:/www.rrdtool.com/|http:/oss.oetiker.ch/rrdtool/|g' ./web/addons/job_monarch/templates/footer.tpl

# Fix real version (0.4-pre instead of 0.3.1)
#for FILE in ./jobmond/jobmond.py ./jobarchived/jobarchived.py ./web/addons/job_monarch/version.php
#do
#	sed -i -e 's/0.3.1/0.4-pre/g' $FILE
#done

# Create the directory structure.
install -m 0755 -d $RPM_BUILD_ROOT/%{_sbindir}
install -m 0755 -d $RPM_BUILD_ROOT%{_initrddir}
install -m 0755 -d $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig
install -m 0755 -d $RPM_BUILD_ROOT%{gangliatemplatedir}
install -m 0755 -d $RPM_BUILD_ROOT%{gangliaaddonsdir}
install -m 0755 -d $RPM_BUILD_ROOT%{_datadir}/jobarchived/
install -m 0755 -d $RPM_BUILD_ROOT%{_sharedstatedir}/jobarchived/

# Install jobmond files
install -m 0644 jobmond/jobmond.conf $RPM_BUILD_ROOT%{_sysconfdir}/
install -m 0755 jobmond/jobmond.py $RPM_BUILD_ROOT/%{_sbindir}/
install -m 0755 pkg/rpm/init.d/jobmond $RPM_BUILD_ROOT%{_initrddir}/
install -m 0755 pkg/rpm/sysconfig/jobmond $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/
(cd $RPM_BUILD_ROOT/%{_sbindir}/; ln -s jobmond.py jobmond)

# Install jobarchived files
install -m 0644 jobarchived/jobarchived.conf $RPM_BUILD_ROOT%{_sysconfdir}/
install -m 0755 jobarchived/jobarchived.py $RPM_BUILD_ROOT/%{_sbindir}/
install -m 0755 pkg/rpm/init.d/jobarchived $RPM_BUILD_ROOT%{_initrddir}/
install -m 0755 pkg/rpm/sysconfig/jobarchived $RPM_BUILD_ROOT%{_sysconfdir}/sysconfig/
install -m 0755 jobarchived/job_dbase.sql $RPM_BUILD_ROOT%{_datadir}/jobarchived/
(cd $RPM_BUILD_ROOT/%{_sbindir}/; ln -s jobarchived.py jobarchived)

# Install gangliaweb interface
#cp %{gangliatemplatedir}/default/images/logo.jpg web/templates/job_monarch/images
cp -r web/templates/job_monarch $RPM_BUILD_ROOT%{gangliatemplatedir}/job_monarch
cp -r web/addons/job_monarch $RPM_BUILD_ROOT%{gangliaaddonsdir}/job_monarch

%clean
%__rm -rf $RPM_BUILD_ROOT

%post -n jobmonarch-jobmond
if [ -x /sbin/chkconfig ]; then
    /sbin/chkconfig --add jobmond
fi

%post -n jobmonarch-jobarchived
if [ -x /sbin/chkconfig ]; then
    if [ ! -d /var/lib/pgsql/data/base ]; then
        /sbin/service postgresql initdb
    fi
    /sbin/service postgresql start
    su -l postgres -c "/usr/bin/createdb jobarchive"
    su -l postgres -c "/usr/bin/psql -f /usr/share/jobarchived/job_dbase.sql jobarchive"
    /sbin/chkconfig --add jobarchived
fi

%post -n jobmonarch-webfrontend
echo "Make sure to set your Ganglia template to job_monarch now"
echo ""
echo "In your Ganglia conf.php, set this line:"
echo "\$template_name = \"job_monarch\";"

%preun -n jobmonarch-jobmond
if [ "$1" = 0 ]; then
    if [ -x /sbin/chkconfig ]; then
	/sbin/service jobmond stop
	/sbin/chkconfig --del jobmond
    fi
fi

%preun -n jobmonarch-jobarchived
if [ "$1" = 0 ]; then
    if [ -x /sbin/chkconfig ]; then
	/sbin/service jobarchived stop
	/sbin/chkconfig --del jobarchived
    fi
fi

%preun -n jobmonarch-webfrontend
if [ "$1" = 0 ]; then
    echo "Make sure to set your Ganglia template to previous config now"
    echo ""
    echo "In your Ganglia conf.php, remove this line:"
    echo "\$template_name = \"job_monarch\";"
fi

%files
%doc AUTHORS CHANGELOG INSTALL LICENSE README TODO UPGRADE

%files -n jobmonarch-jobmond
%doc jobmond/examples
%config %{_sysconfdir}/jobmond.conf
%{_sbindir}/jobmond.py
%{_sbindir}/jobmond

%files -n jobmonarch-jobarchived
%doc jobarchived/examples
%config %{_sysconfdir}/jobarchived.conf
%dir %{_datadir}/jobarchived
%{_sbindir}/jobarchived.py
%{_sbindir}/jobarchived
%{_datadir}/jobarchived/*
#%dir %{_sharedstatedir}/jobarchived

%files -n jobmonarch-webfrontend
%dir %{gangliatemplatedir}/job_monarch
%dir %{gangliaaddonsdir}/job_monarch
%{gangliatemplatedir}/job_monarch/cluster_extra.tpl
%{gangliatemplatedir}/job_monarch/host_extra.tpl
%dir %{gangliatemplatedir}/job_monarch/images
%{gangliatemplatedir}/job_monarch/images/logo.jpg
%config %{gangliaaddonsdir}/job_monarch/conf.php
%{gangliaaddonsdir}/job_monarch/ajax-loader.gif
%{gangliaaddonsdir}/job_monarch/cal.gif
%{gangliaaddonsdir}/job_monarch/clusterconf
%{gangliaaddonsdir}/job_monarch/document_archive.jpg
%dir %{gangliaaddonsdir}/job_monarch/dwoo
%{gangliaaddonsdir}/job_monarch/dwoo/dwooAutoload.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/plugins
%dir %{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin
%dir %{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/filters
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/filters/html_format.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/helper.array.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/processors
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/processors/pre.smarty_compat.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/forelse.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/capture.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/if.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/elseif.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/block.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/smartyinterface.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/foreachelse.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/loop.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/textformat.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/template.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/withelse.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/with.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/strip.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/for.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/a.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/dynamic.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/else.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/topLevelBlock.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/auto_escape.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/section.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/blocks/foreach.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/cat.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/fetch.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/extendsCheck.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/count_characters.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/regex_replace.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/truncate.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/escape.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/safe.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/replace.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/return.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/math.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/isset.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/strip_tags.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/capitalize.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/dump.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/cycle.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/upper.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/eval.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/mailto.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/counter.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/spacify.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/default.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/optional.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/include.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/eol.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/reverse.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/lower.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/extends.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/wordwrap.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/load_templates.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/count_paragraphs.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/indent.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/assign.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/count_sentences.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/tif.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/nl2br.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/string_format.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/whitespace.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/count_words.php
%{gangliaaddonsdir}/job_monarch/dwoo/plugins/builtin/functions/date_format.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/compiled
%dir %{gangliaaddonsdir}/job_monarch/dwoo/cache
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/IPluginProxy.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Filter.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Compiler.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Compilation
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Compilation/Exception.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Template
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Template/String.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Template/File.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/IDataProvider.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/ICompilable.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Block
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Block/Plugin.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/ICompiler.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/ICompilable
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/ICompilable/Block.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Processor.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Smarty
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Smarty/Adapter.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Exception.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Plugin.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Core.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/ILoader.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Data.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Security
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Security/Policy.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Security/Exception.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/IElseable.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/ITemplate.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Loader.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/ZendFramework
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/ZendFramework/README
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/ZendFramework/PluginProxy.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/ZendFramework/View.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/ZendFramework/Dwoo.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/views
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/views/dwoowelcome.tpl
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/views/page.tpl
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/controllers
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/controllers/dwoowelcome.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/libraries
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/libraries/Dwootemplate.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/README
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/config
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CodeIgniter/config/dwootemplate.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CakePHP
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CakePHP/README
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/CakePHP/dwoo.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/Agavi
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/Agavi/README
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/Agavi/DwooRenderer.php
%dir %{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/Agavi/dwoo_plugins
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/Agavi/dwoo_plugins/t.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo/Adapters/Agavi/dwoo_plugins/url.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo.compiled.php
%{gangliaaddonsdir}/job_monarch/dwoo/Dwoo.php
%{gangliaaddonsdir}/job_monarch/footer.php
%{gangliaaddonsdir}/job_monarch/graph.php
%{gangliaaddonsdir}/job_monarch/host_view.php
%{gangliaaddonsdir}/job_monarch/image.php
%{gangliaaddonsdir}/job_monarch/index.php
%{gangliaaddonsdir}/job_monarch/jobmonarch.gif
%{gangliaaddonsdir}/job_monarch/libtoga.js
%{gangliaaddonsdir}/job_monarch/libtoga.php
%{gangliaaddonsdir}/job_monarch/logo_ned.gif
%{gangliaaddonsdir}/job_monarch/next.gif
%{gangliaaddonsdir}/job_monarch/overview.php
%{gangliaaddonsdir}/job_monarch/prev.gif
%{gangliaaddonsdir}/job_monarch/redcross.jpg
%{gangliaaddonsdir}/job_monarch/search.php
%{gangliaaddonsdir}/job_monarch/styles.css
%dir %{gangliaaddonsdir}/job_monarch/templates
%{gangliaaddonsdir}/job_monarch/templates/footer.tpl
%{gangliaaddonsdir}/job_monarch/templates/header.tpl
%{gangliaaddonsdir}/job_monarch/templates/host_view.tpl
%{gangliaaddonsdir}/job_monarch/templates/overview.tpl
%{gangliaaddonsdir}/job_monarch/templates/search.tpl
%{gangliaaddonsdir}/job_monarch/ts_picker.js
%{gangliaaddonsdir}/job_monarch/ts_validatetime.js
%{gangliaaddonsdir}/job_monarch/version.php


%changelog
* Mon Apr 22 2013 Olivier Lahaye <olivier.lahaye@free.fr> 1.0-1
- Major rewrite of the spec file (sub packages)
- Final upstream release.

* Wed Mar 13 2013 Olivier Lahaye <olivier.lahaye1@free.fr> 0.4-0.4
- Added Requires: pbs_python

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

