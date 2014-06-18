%{!?python_sitearch: %define python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print get_python_lib(1)")}

### Abstract ###

Name: pbs_python
Version: 4.4.0
Release: 1%{?dist}
License: See LICENSE
Group: Development/Libraries
Summary: This package contains the PBS python module.
URL: https://oss.trac.surfsara.nl/pbs_python
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Source: ftp://ftp.sara.nl/pub/outgoing/%{name}-%{version}.tar.gz 

### Dependencies ###
# None

### Build Dependencies ###

BuildRequires: libtorque-devel >= %{libtorque_version}
BuildRequires: python-devel >= %{python_version}

%description
This package contains the pbs python module.

%prep
%setup -q -n pbs_python-%{version}

%build
if test -d /opt/pbs/bin
then
	export PATH=/opt/pbs/bin:$PATH
        export PBS_PYTHON_INCLUDEDIR=/opt/pbs/include
fi
%configure
%{__python} setup.py build

%install
if test -d /opt/pbs/bin
then
	export PATH=/opt/pbs/bin:$PATH
        export PBS_PYTHON_INCLUDEDIR=/opt/pbs/include
fi
%{__python} ./setup.py install --prefix $RPM_BUILD_ROOT%{_prefix} ;

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0755)
%doc README TODO examples
%{python_sitearch}/pbs.pth
%dir %{python_sitearch}/pbs
%{python_sitearch}/pbs/*

%changelog
* Wed Jun 18 2014 Olivier Lahaye <olivier.lahaye@cea.fr>
- New upstream version 4.4.0 (supports torque >= 4.2)
* Wed Mar 13 2013 Olivier Lahaye <olivier.lahaye@cea.fr>
- New upstream version 4.3.5
* Wed Mar 13 2013 Olivier Lahaye <olivier.lahaye@cea.fr>
- Fixed %{python_sitearch}/pbs package ownership
* Wed May 11 2011 Olivier Lahaye <olivier.lahaye@cea.fr>
- Updates for new version 4.3.3
* Wed Mar 24 2010 Ramon Bastiaans <ramon.bastiaans@sara.nl>
- Updates for new version
* Tue Oct 06 2009 Ramon Bastiaans <ramon.bastiaans@sara.nl>
- Fixed tmppath, %setup sourcedir
* Tue Mar 24 2009 David Chin <chindw@wfu.edu>
- Fedora-ize
* Sun Mar  9 2008 Michael Sternberg <sternberg@anl.gov>
- libdir and python defines
* Wed Nov 23 2005 Ramon Bastiaans <bastiaans@sara.nl>
- Fixed missing prep setup and added configure
* Tue Nov 22 2005 Martin Pels <pels@sara.nl>
- Changed default directory permissions
* Tue Nov 01 2005 Martin Pels <pels@sara.nl> 
- Initial version

