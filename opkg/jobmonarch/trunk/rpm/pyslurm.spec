Summary: Slurm Interface for Python
Name: python-pyslurm
#Name: python-slurm
Version: 17.11.0.8
URL: http://www.gingergeeks.co.uk/pyslurm/index.html
Release: 1%{?dist}
License: GPL
Packager: Olivier LAHAYE <olivier.lahaye@cea.fr>
Group: Development/Languages
Source: pyslurm-%{version}.tar.bz2
Patch0: pyslurm_sphinx_theme.patch
#Patch1: pyslurm_doc_no_python_github.patch
#Patch2: pyslurm_doc_version.patch
BuildRoot: %{_tmppath}/%{name}
BuildRequires: python-devel => 2.7 Cython >= 0.19 python-sphinx >= 1.1 slurm-devel >= 17.11.6

%description
PySLURM is a Python/Cython extension module to the Simple Linux Unified
Resource Manager (SLURM) API. It can be used to contact slurmctld.
SLURM is typically used on HPC clusters such as those listed on the TOP500
but can used on the smallest to the largest cluster. 

%prep
%setup -q -n pyslurm-%{version}
%patch0 -p1

%build
%{__python} setup.py build --slurm=%{_prefix}
sed -i -e '/sphinx.ext.githubpages/d' doc/source/conf.py
(cd doc; PYTHONPATH=../build/lib.linux-x86_64-2.7/:/usr/lib/python2.7/site-packages/ make html; PYTHONPATH=../build/lib.linux-x86_64-2.7/:/usr/lib/python2.7/site-packages/ make man)

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
* Fri Apr 20 2018 Olivier Lahaye <olivier.lahaye@cea.fr> 17.11.0.7-1
- New version
* Thu Jun 22 2017 Olivier Lahaye <olivier.lahaye@cea.fr> 17.02-1
- New version
* Fri Mar 04 2016 Olivier Lahaye <olivier.lahaye@cea.fr> 15.08.2-1
- New version
* Tue Jan 14 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 2.5.6-1
- Initial packaging
