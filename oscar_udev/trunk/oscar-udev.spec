
Summary:        OSCAR udev configuration tool.
Name:           oscar-udev
Version:        1.0
Release:        3
Vendor:         Open Cluster Group <http://OSCAR.OpenClusterGroup.org/>
Distribution:   OSCAR
Packager:       Geoffroy Vallee <valleegr@ornl.gov>
License:        GPL
Group:          Development/Libraries
Source:         %{name}.tar.gz
BuildRoot:      %{_localstatedir}/tmp/%{name}-root
BuildArch:      noarch

%description
Set of scripts and Perl modules for the management of udev within an OSCAR cluster.

%prep
%setup -n %{name}

%install 
make install DESTDIR=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{perl_vendorlib}/OSCAR/*

%changelog
* Sun Dec 15 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0-3
- Removed AutoReqProv: no
- Avoid owning %{perl_vendorlib}/OSCAR

* Mon Feb  4 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0-2
- Use rpm macros for paths.

* Wed Feb 25 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.0-1
- New upstream version (see ChangeLog for more details).
