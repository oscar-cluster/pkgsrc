Summary:        OSCAR DatabAse.
Name:           oda
Version:        1.4.17
Release:        0.1
Vendor:         Open Cluster Group <http://OSCAR.OpenClusterGroup.org/>
Distribution:   OSCAR
Packager:       Olivier Lahaye <olivier.lahaye@cea.fr>
License:        GPL
Group:          Development/Libraries
Source:         %{name}.tar.gz
BuildRoot:      %{_localstatedir}/tmp/%{name}-root
BuildArch:      noarch
AutoReqProv:    no
Requires:       oscar-base-lib > 6.0.4
Requires:       orm

%description
Set of scripts and Perl modules for the management of the OSCAR database.

%prep
%setup -n %{name}

%build

%install 
%__make install DESTDIR=$RPM_BUILD_ROOT
# Make sure postinstall is executable
%__chmod +x $RPM_BUILD_ROOT/%{_datadir}/oscar/prereqs/oda/etc/Migration_AddGpuSupport.sh

%files
%defattr(-,root,root)
%{_bindir}/*
%{perl_vendorlib}/*
%{_datadir}/oscar/prereqs/oda/*
%{_mandir}/man1/*

%post
# If install successfull, then migrate the database.
# Don't know to be 100% safe here. In pre, the migration can succeed but
# installation can fail. in post, installation can succeed, but migration
# can fail. In either pre or post, I don't know how to revert in coherent
# situation. (not skilled enought).
%{_datadir}/oscar/prereqs/oda/etc/Migration_AddGpuSupport.sh

%changelog
* Tue Mar 05 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.4.17-0.1
- New upstream beta version.
* Fri Feb 22 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.4.16-3
- Fixed postinstall script on fresh installs (no database)
* Fri Feb  1 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.4.16-2
- Added postinstall script for database migration.
* Tue Nov 13 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 1.4.16-1
- Align man location to bin location (no more /usr/local)
- Update spec: make use of rpm macros paths.
- Use %_sourcedir to detect the source directory on RPM based systems.
* Tue Feb 08 2011 Geoffroy Vallee <valleegr@ornl.gov> 1.4.15-1
- new upstream version (see ChangeLog for more details).
* Sat Aug 21 2010 Geoffroy Vallee <valleegr@ornl.gov> 1.4.14-1
- new upstream version (see ChangeLog for more details).
* Mon Dec 07 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.13-1
- new upstream version (see ChangeLog for more details).
* Fri Dec 04 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.12-1
- new upstream version (see ChangeLog for more details).
* Mon Nov 30 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.11-1
- new upstream version (see ChangeLog for more details).
* Tue Nov 24 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.10-1
- new upstream version (see ChangeLog for more details).
* Tue Nov 10 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.9-1
- new upstream version (see ChangeLog for more details).
* Fri Oct 30 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.8-1
- new upstream version (see ChangeLog for more details).
* Thu Oct 08 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.7-1
- new upstream version (see ChangeLog for more details).
* Fri Sep 25 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.6-1
- new upstream version (see ChangeLog for more details).
* Thu May 07 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.5-1
- new upstream version (see ChangeLog for more details).
* Thu Apr 23 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.4-1
- new upstream version (see ChangeLog for more details).
* Mon Mar 23 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.3-1
- new upstream version (see ChangeLog for more details).
* Wed Mar 18 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.2-1
- new upstream version (see ChangeLog for more details).
* Thu Feb 26 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4.1-1
- new upstream version (see ChangeLog for more details).
* Mon Feb 09 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.4-1
- new upstream version (see ChangeLog for more details).
* Tue Feb 03 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.3.5-1
- new upstream version (see ChangeLog for more details).
* Tue Jan 20 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.3.4-1
- new upstream version (see ChangeLog for more details).
* Thu Jan 15 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.3.3-1
- new upstream version (see ChangeLog for more details).
* Thu Dec 11 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.3.2-1
- new upstream version (see ChangeLog for more details).
* Thu Dec 04 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.3.1-3
- Move the libraries into a noarch directory.
* Fri Nov 28 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.3.1-2
- Disable automatic dependencies.
* Wed Nov 26 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.3.1-1
- includes the man pages into the RPM.
* Tue Sep 23 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.3-1
- new upstream version (see ChangeLog for more details).
* Thu Aug 21 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.2-1
- new upstream version (see ChangeLog for more details).
* Wed Aug 13 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.1-1
- new upstream version (see ChangeLog for more details).
* Sun Aug 10 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.0-1
- new upstream version (see ChangeLog for more details).
