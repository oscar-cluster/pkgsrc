Summary:        OSCAR Packaging helpers.
Name:           oscar-packager
Version:        1.0.1
Release:        2
Vendor:         Open Cluster Group <http://OSCAR.OpenClusterGroup.org/>
Distribution:   OSCAR
Packager:       Olivier Lahaye <olivier.lahaye@cea.fr>
License:        GPL
Group:          Development/Libraries
Source:         %{name}.tar.gz
BuildRoot:      %{_localstatedir}/tmp/%{name}-root
BuildArch:      noarch
AutoReqProv: 	no
Requires:       wget, oscar-base-lib, rpm-build

%description
Set of scripts and Perl modules for the automatic packaging of the OSCAR.

%prep
%setup -n %{name}

%install
%__make install DESTDIR=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/*
%{perl_vendorlib}/*
%{_mandir}/*
%{_sysconfdir}/oscar/%{name}/*

%changelog
* Wed Nov 14 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.1-2
- moved "make install" into install section to avoid RPM_BUILD_ROOT being erased
  after install.
- removed empty build section
- used __make macro instead of make. makeinstall macro is useless here.
- used macros for paths.

* Tue May 31 2011 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.1-1
- new upstream version (see ChangeLog for more details).
- moved "make install" into install section to avoid RPM_BUILD_ROOT being erased
  after install.
- removed empty build section

* Fri Jan 02 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.0-1
- new upstream version (see ChangeLog for more details).
