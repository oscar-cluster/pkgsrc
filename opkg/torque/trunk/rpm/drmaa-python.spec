# Don't need debuginfo RPM
%define debug_package %{nil}
%define __check_files %{nil}
%define host_arch %(arch)

Summary: Python bindings for DRMAA libraries
Name: drmaa-python
Version: 0.7.6
URL: http://www.drmaa.org
Release: 2
License: free
Group: Applications/Base
Source: drmaa-%{version}.tar.gz
BuildArch: noarch
# Libdrmaa.so.0 is provided either by oscar-SGE or by torque-drmaa from EPEL
%if "%{host_arch}" == "x86_64"
Requires: libdrmaa.so.0()(64bit)
%else
Requires: libdrmaa.so.0()(32bit)
%endif
%if 0%{?fedora} > 10 || 0%{?rhel} > 6
Requires: python-sphinx >= 1.1
%endif


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

%if 0%{?fedora} > 10 || 0%{?rhel} > 6
# Generate man
(cd docs; make man)
# fix man section.
sed -i -e 's/"DRMAAPYTHON" "1"/"DRMAAPYTHON" "3"/g' docs/_build/man/drmaapython.1
mv docs/_build/man/drmaapython.1 docs/_build/man/drmaapython.3
%endif

##
## INSTALL
##
%install
%{__python} setup.py install \
        --optimize=2 \
        --root=$RPM_BUILD_ROOT
%if 0%{?fedora} > 10 || 0%{?rhel} > 6
%__mkdir_p $RPM_BUILD_ROOT%{_mandir}/man3
install -m 644 docs/_build/man/drmaapython.3 $RPM_BUILD_ROOT%{_mandir}/man3/
%endif

##
## CLEAN
##
%clean
%__rm -rf $RPM_BUILD_ROOT

%files
#dir %{python_sitelib}/drmaa-0.5-py%{python_version}.egg-info
#{python_sitelib}/drmaa-0.5-py%{python_version}.egg-info/*
%doc README.rst license.txt examples/
%dir %{python_sitelib}/drmaa
%{python_sitelib}/drmaa/*
%if 0%{?fedora} > 10 || 0%{?rhel} > 6
%{_mandir}/man3/*
%endif

%changelog
* Wed May 14 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 0.7.6-2
- Disabled man building on rhel6 (too old python-phinx that lack mathjax extension)

* Mon May 12 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 0.7.6-1
- New upstream version.
- Added man.
- Added docs.
- Added examples.

* Thu Mar 28 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 0.5-2
- Fix libdrmaa.so arch dependancy using %%__isa_bits
- Fix egg-info path (avoid hardcoding python version)

* Thu May 10 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 0.5-1
- New upstream release.
- New depend on libdrmaa.so.0 (should work with SGE or torque-drmaa)
- lowercase name for compliance with python rpm naming scheme
- noarch build

* Tue Mar 02 2010 Olivier Lahaye <olivier.lahaye@cea.fr> 0.4b1
- New upstream release.

* Fri Jun 16 2006 Babu Sundaram <babu@cs.uh.edu> 0.2
- Prepare first rpm for DRMAA Python interface 0.2

