
%define debug_packages	%{nil}
%define debug_package %{nil}


Summary: The Cbench sources
Name: cbench-sources
Version: 1.2.2
Release: 1%{?dist}
Group: Development
License: GPL
Source: cbench-%{version}.tar.gz
Source1: oscar-benchmarking.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

Provides: cbench-sources
Autoreqprov: no

%description
The cbench sources to build cbench rpms later per user.


%prep
%setup -c -n cbench-sources  


%build

%install
mkdir -p $RPM_BUILD_ROOT/home/oscartst/rpmbuild/SOURCES
mkdir $RPM_BUILD_ROOT/home/oscartst/rpmbuild/SPECS
cp %SOURCE1 $RPM_BUILD_ROOT/home/oscartst/rpmbuild/SOURCES/
#cp -r *.gz $RPM_BUILD_ROOT/home/oscartst/rpmbuild/SOURCES
cp opensource/hpcc/hpcc-1.2.0.tar.gz $RPM_BUILD_ROOT/home/oscartst/rpmbuild/SOURCES/
cp cbench.spec $RPM_BUILD_ROOT/home/oscartst/rpmbuild/SPECS


%clean
rm -rf $RPM_BUILD_ROOT


%files
#/home/oscartst/rpmbuild/SOURCES/cbench_hpcc.tar.gz
/home/oscartst/rpmbuild/SOURCES/oscar-benchmarking.tar.gz
/home/oscartst/rpmbuild/SOURCES/hpcc-1.3.1.tar.gz
/home/oscartst/rpmbuild/SPECS/cbench.spec
