
%define debug_packages	%{nil}
%define debug_package %{nil}


Summary: The Cbench sources
Name: cbench-sources
Version: 1.1.5
Release: 1
Group: Development
License: GPL
Source:cbench_sources.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

Provides: cbench-sources
Autoreqprov: no

%description
The cbench sources to build cbench rpms later per user.


%prep
%setup -c -n cbench-sources  
mkdir -p $RPM_BUILD_ROOT/home/oscartst/SOURCES
mkdir $RPM_BUILD_ROOT/home/oscartst/SPECS


%build

%install
cp -r *.gz $RPM_BUILD_ROOT/home/oscartst/SOURCES
cp cbench.spec $RPM_BUILD_ROOT/home/oscartst/SPECS


%clean
rm -rf $RPM_BUILD_ROOT


%files
/home/oscartst/SOURCES/cbench_hpcc.tar.gz
/home/oscartst/SOURCES/hpcc-1.0.0.tar.gz
/home/oscartst/SPECS/cbench.spec
