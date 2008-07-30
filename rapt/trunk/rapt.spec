# $Id$
Summary: Wrapper to apt-get for clusters
Name: rapt
Version: 2.5
Vendor: NEC HPCE
Release: 1
License: GPL
Packager: Erich Focht <efocht@hpce.nec.com>
Source: %{name}-%{version}.tar.gz
Group: System Environment/Tools
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}
#Requires: apt
Requires: debootstrap
Requires: deb
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
* Mon Jul 30 2008 Geoffroy Vallee - 2.5-1
- New upstream version (see ChangeLog file for more details).
* Mon Jul 28 2008 Geoffroy Vallee - 2.4-1
- New upstream version (see ChangeLog file for more details).
* Fri Jul 25 2008 Geoffroy Vallee - 2.3-1
- New upstream version (see ChangeLog file for more details).
* Wed May 10 2006 Erich Focht
- initial RPM
