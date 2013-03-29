#
# spec file for package PyPgSQL (Version 3.8.1)
#
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
#
# norootforbuild

Name:           pyPgSQL
BuildRequires:  openssl-devel postgresql postgresql-devel python-devel
URL:            http://pypgsql.sourceforge.net/
Summary:        Python DB-API 2 compliant library for using PostgreSQL databases
Version:        2.5.1
Release:        4%{?dist}
License:        Other License(s), see package
Group:          Productivity/Databases/Clients
Source0:        %{name}-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Provides:       python-pgsql
Requires:       python >= %{python_version}
Requires:	mx

%description
pyPgSQL is a package of two modules that provide a Python DB-API 2.0 compliant
interface to PostgreSQL databases. The first module, libpq, exports the
PostgreSQL C API to Python. This module is written in C and can be compiled
into Python or can be dynamically loaded on demand. The second module, PgSQL,
provides the DB-API 2.0 compliant interface and support for various PostgreSQL
data types, such as INT8, NUMERIC, MONEY, BOOL, ARRAYS, etc. This module is
written in Python.

http://pypgsql.sourceforge.net/

Authors:
--------
    Billy G. Allie et al

%prep
%setup -q

%build
%{__python} setup.py build

%install
%{__python} setup.py install --prefix=%{_prefix} --optimize=2 --root=%buildroot
sed -i -e 's/#!\/usr\/local\/bin\/python/#!\/usr\/bin\/python/' examples/*.py

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc Announce ChangeLog PKG-INFO README* examples/
%{python_sitearch}/*

%changelog
* Mon Mar  4 2013 - Olivier LAHAYE <olivier.lahaye@cea.Fr> 2.5.1-4
- Used macros for python paths
- Cleaned up useless defines (provided by standard rpm macros)
- Use the --optimize option for installation.
- Added Requires: mx for mxDateTime.so() requirement.

* Tue Jun  2 2009 - knight@princeton.edu
- Changes to allow compilation on PU_IAS systems (only)

* Fri Oct 31 2008 - linux@weberhofer.at
- Changes to allow compilation on RH systems

* Mon Oct 27 2008 - linux@weberhofer.at
- Initial version: 2.5.1
