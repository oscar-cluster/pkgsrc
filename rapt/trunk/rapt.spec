# $Id$
Summary: Wrapper to apt-get for clusters
Name: rapt
Version: 1.0
Vendor: NEC HPCE
Release: 1
License: GPL
Packager: Erich Focht <efocht@hpce.nec.com>
Source: %{name}.tar.gz
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
%setup -n %{name}


%build


%install

install -d -o root -g root -m 755 $RPM_BUILD_ROOT%{_bindir}
install -d -o root -g root -m 755 $RPM_BUILD_ROOT%{_mandir}/man8
install -o root -g root -m 755  rapt $RPM_BUILD_ROOT%{_bindir}
install -o root -g root -m 644  rapt.8 $RPM_BUILD_ROOT%{_mandir}/man8

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/*
%{_mandir}/man8/rapt*

%changelog
* Wed May 10 2006 Erich Focht
- initial RPM
