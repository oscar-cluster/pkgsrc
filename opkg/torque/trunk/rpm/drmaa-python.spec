# Don't need debuginfo RPM
%define debug_package %{nil}
%define __check_files %{nil}

Summary: Python bindings for DRMAA libraries
Name: drmaa-python
Version: 0.5
URL: http://www.drmaa.org
Release: 2 
License: GPL
Group: Applications/Base
Source: drmaa-%{version}.tar.gz
BuildArch:      noarch
# Libdrmaa.so.0 is provided either by oscar-SGE or by torque-drmaa from EPEL
Requires: libdrmaa.so.0()(64bit)

%description
Distributed Resource Management Application API (DRMAA) is a uniform specification for the 
submission and control of jobs to one or more Distributed Resource Management (DRM) systems.

##
## PREP
##

%prep 
%setup -n drmaa-%{version}

##
## BUILD
##
%build
%{__python} setup.py build

##
## INSTALL
##
%install
%{__python} setup.py install \
        --optimize=2 \
        --root=$RPM_BUILD_ROOT

##
## CLEAN
##
%clean
%__rm -rf $RPM_BUILD_ROOT

%files
%dir %{python_sitelib}/drmaa-0.5-py%{python_version}.egg-info
%{python_sitelib}/drmaa-0.5-py%{python_version}.egg-info/*
%dir %{python_sitelib}/drmaa
%{python_sitelib}/drmaa/*

%changelog
* Thu Mar 28 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 0.5-2
- Fix egg-info path (avoid hardcoding python version)

* Thu May 10 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 0.5-1
- New upstream release.
- New depend on libdrmaa.so.0 (should work with SGE or torque-drmaa)
- lowercase name for compliance with python rpm naming scheme
- noarch build

* Tue Mar 02 2010 Olivier Lahaye <olivier.lahaye@cea.fr> 0.4b1
- New upstream release.

* Tue Jun 16 2006 Babu Sundaram <babu@cs.uh.edu> 0.2
- Prepare first rpm for DRMAA Python interface 0.2

