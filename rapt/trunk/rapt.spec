# $Id$
Summary: Wrapper to apt-get for clusters
Name: rapt
Version: 2.8.10
Vendor: NEC HPCE
Release: 2
License: GPL
Packager: Erich Focht <efocht@hpce.nec.com>
Source: %{name}-%{version}.tar.gz
Group: System Environment/Tools
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}
#Requires: apt
Requires: dpkg-devel
Requires: debootstrap
#Requires: deb
AutoReqProv: no

%description 

Tool for setting up, exporting apt repositories and executing
apt-get commands for only these repositories.
- prepare an apt repository
- export it through apache
- execute apt-get commands applying only to this repository (locally)
- execute apt-get commands on the cluster nodes applying only to this repository
- execute apt-get commands inside a chrooted image.

%prep
%setup -n %{name}-%{version}


%build


%install

make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/*
%{_mandir}/man8/rapt*

%changelog
* Thu Dec 20 2012 Olivier Lahaye <olivier.lahaye@cea.fr> - 2.8.10-2
- Updated Requirements so it works on fedora-17.
* Thu Dec 20 2012 Geoffroy Vallee - 2.8.10-1
- New upstream version (see ChangeLog file for more details).
* Sat Aug 21 2010 Geoffroy Vallee - 2.8.8-1
- New upstream version (see ChangeLog file for more details).
* Mon Nov 30 2009 Geoffroy Vallee - 2.8.7-1
- New upstream version (see ChangeLog file for more details).
* Thu Jul 16 2009 Geoffroy Vallee - 2.8.6-1
- New upstream version (see ChangeLog file for more details).
* Fri Apr 23 2009 Geoffroy Vallee - 2.8.5-1
- New upstream version (see ChangeLog file for more details).
* Tue Feb 10 2009 Geoffroy Vallee - 2.8.4-1
- New upstream version (see ChangeLog file for more details).
* Mon Sep 30 2008 Geoffroy Vallee - 2.8.2-1
- New upstream version (see ChangeLog file for more details).
* Fri Sep 26 2008 Geoffroy Vallee - 2.8.1-1
- New upstream version (see ChangeLog file for more details).
* Mon Sep 22 2008 Geoffroy Vallee - 2.8-1
- New upstream version (see ChangeLog file for more details).
* Tue Aug 12 2008 Geoffroy Vallee - 2.7-1
- New upstream version (see ChangeLog file for more details).
* Fri Aug 01 2008 Geoffroy Vallee - 2.6-1
- New upstream version (see ChangeLog file for more details).
* Mon Jul 30 2008 Geoffroy Vallee - 2.5-1
- New upstream version (see ChangeLog file for more details).
* Mon Jul 28 2008 Geoffroy Vallee - 2.4-1
- New upstream version (see ChangeLog file for more details).
* Fri Jul 25 2008 Geoffroy Vallee - 2.3-1
- New upstream version (see ChangeLog file for more details).
* Wed May 10 2006 Erich Focht
- initial RPM
