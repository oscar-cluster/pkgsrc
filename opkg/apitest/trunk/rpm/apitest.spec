%define name apitest
%define version 1.0.0
%define libvers 1.0
%define release 12.2
%define _unpackaged_files_terminate_build 0

%{expand:%%define py_ver %(python -V 2>&1| awk '{print $2}')}
%{expand:%%define py_libver %(python -V 2>&1| awk '{print $2}'|cut -d. -f1-2)}

Summary: A test driver application. 
Name: %{name}
Version: %{version}
Release: %{release}
License: LGPL
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root

# Architecture
BuildArch: noarch

Requires: python-twisted >= 1.3
BuildRequires: python-twisted >= 1.3

Requires: python-elementtree
BuildRequires: python-elementtree

Requires: python >= 2.2
BuildRequires: python >= 2.2


%description
APItest version %{version}.
A test driver application. 


##########
# PREP
##########
%prep
%setup -q -n %{name}-%{version}

%build
echo "==========[ BUILD ]===================================="
echo "python%{py_libver}"
echo "buildroot=%{buildroot}"
%define sitepackages %{_prefix}/%{_lib}/python%{py_libver}/site-packages
#python%{py_libver} setup.py build


%install
echo "==========[ INSTALL ]=================================="
echo %{buildroot}
%define doc_prefix /usr/share/doc/apitest
python%{py_libver} setup.py install --no-compile --prefix=%{buildroot}/usr/ --install-lib=%{buildroot}%{sitepackages}
###--install-data=%{buildroot}/%{doc_prefix}


%files
%defattr(-,root,root)
%dir /usr/%{_lib}/python%{py_libver}/site-packages/libapitest
/usr/%{_lib}/python%{py_libver}/site-packages/libapitest/*
%dir %{doc_prefix}
%{doc_prefix}/*
%dir /usr/bin
/usr/bin/*


%clean
echo "cleaning $RPM_BUILD_ROOT"
rm -rf $RPM_BUILD_ROOT

%changelog
* Thu Jan 12 2006    Thomas Naughton  <naughtont@ornl.gov>
- (1.0.0-12) Removed unused profile.d portions.
- Changed to use (what appears) more standard 'python-twisted', doesn't
  cover case where it is v2.0 and twisted-web is seperate, but should be ok. 
