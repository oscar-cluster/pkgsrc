Summary: OSCAR Perl libraries
Name: perl-OSCAR
Version: 1.0
Release: 1
License: GPL
URL: http://oscar.openclustergroup.org
Group: Applications/System
#Source: %{name}-%{version}.tar.gz
Requires: perl
Vendor: OSCAR
Distribution: OSCAR
Packager: Bernard Li <bernard@vanhpc.org>
#Prefix: %prefix
Buildroot: %{tmp}/%{name}-%{version}-root
BuildArch: noarch
AutoReqProv: no
Provides: perl(OSCAR::Configbox), perl(OSCAR::Database), perl(OSCAR::Opkg), perl(OSCAR::OCA::OS_Detect), perl(OSCAR::Distro), perl(OSCAR::Package), perl(OSCAR::PackageSmart), perl(OSCAR::Network), perl(OSCAR::Logger), perl(OSCAR::PackagePath), perl(OSCAR::oda)

%description
Perl libraries for OSCAR

Currently this is just a bogus package trying to get around dependency issues with installing OSCAR trunk (5.1)

%prep

%build

%clean
rm -rf %{tmp}/%{name}-%{version}-root

%files
%defattr(-,root,root)

%changelog
* Tue Sep 25 2007 Bernard Li <bernard@vanhpc.org>
- Genesis
