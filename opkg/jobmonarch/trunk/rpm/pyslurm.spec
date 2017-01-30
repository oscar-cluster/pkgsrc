# Don't need debuginfo RPM
%define debug_package %{nil}
%define rel 1

Summary: Slurm Interface for Python
Name: python-pyslurm
#Name: python-slurm
Version: 15.08.8
URL: http://www.gingergeeks.co.uk/pyslurm/index.html
Release: %{rel}%{?dist}
License: GPL
Packager: Olivier LAHAYE <olivier.lahaye@cea.fr>
Group: Development/Languages
#Source: pyslurm-%{version}-%{rel}.tar.gz
Source: pyslurm-%{version}.tar.xz
#Patch0: pyslurm_build_150808.patch
Patch1: pyslurm_sphinx_theme.patch
BuildRoot: %{_tmppath}/%{name}
BuildRequires: python-devel => 2.6 Cython >= 0.19 python-sphinx >= 1.1 slurm-devel >= 15.08.2

#AutoReqProv: no

%description
PySLURM is a Python/Cython extension module to the Simple Linux Unified
Resource Manager (SLURM) API. It can be used to contact slurmctld.
SLURM is typically used on HPC clusters such as those listed on the TOP500
but can used on the smallest to the largest cluster. 

%prep
#setup -q
#setup -q -n pyslurm-%{version}-%{rel}
%setup -q -n pyslurm-%{version}
#patch0 -p1
%patch1 -p2

%build
%{__python} setup.py build --slurm=%{_prefix}
(cd doc; make html; make man)

%install
rm -rf $RPM_BUILD_ROOT
%{__python} setup.py install --prefix=%{_prefix} --optimize=2 --root=%buildroot --install-lib=%{python_sitearch}
install -d %{buildroot}%{_mandir}/man1
install -m 644 doc/build/man/pyslurm.1 %{buildroot}%{_mandir}/man1/

%clean
%__rm -rf $RPM_BUILD_ROOT

%files
%doc CONTRIBUTORS.rst COPYING.txt README.rst THANKS.rst
%doc doc/build/html
%doc examples
%{python_sitearch}/pyslurm*
%{_mandir}/man1/pyslurm.1*

%changelog
* Fri Jan 14 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 2.5.6-1
- Initial packaging
