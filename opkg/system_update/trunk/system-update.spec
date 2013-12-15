Summary:        OSCAR System Updater
Name:           system-update
Version:        1.0.1
Release:        1
Vendor:         Open Cluster Group <http://OSCAR.OpenClusterGroup.org/>
Distribution:   OSCAR
Packager:       Geoffroy Vallee <valleegr@ornl.gov>
License:        GPL
Group:          Development/Libraries
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_localstatedir}/tmp/%{name}-root
BuildArch:      noarch
#AutoReqProv:    no
Requires:       oscar-base-lib >= 6.0.5
Requires:       packman >= 3.2.2
Requires:       diff

%description
Set of scripts and Perl modules for the update of OSCAR compute nodes.

%prep
%setup

%build

%install
%{__make} install DESTDIR=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc VERSION
%{_bindir}/oscar-%{name}
%{perl_vendorlib}/OSCAR/*
%{_mandir}/man1/oscar-%{name}.1*
%{_defaultdocdir}/%{name}/templates

%changelog
* Sun Dec 15 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.1-1
- New version that match packman 3.2.2
- Updated packman min version
- Removed AutoReqProv:no so we have correct deps.

* Thu Apr 04 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.0-2
- Updated spec file using macros.
- Now tarball name includes version.

* Fri Jun 18 2010 Geoffroy Vallee <valleegr@ornl.gov> 1.0.0-1
- Initial version.
