Summary:        Script that helps at setting up the network.
Name:           network-configurator
Version:        1.0.0
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
Script that helps at setting up the network with various configurations, e.g., bridges management.

%prep
%setup -n %{name}

%install
%__make install DESTDIR=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/*
%{perl_vendorlib}/OSCAR/*
%{_mandir}/*

%changelog
* Fri Dec 13 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.0-3
- Removed AutoReqProv: no so we have automatic deps
- Avoid owning %{perl_vendorlib}/OSCAR dir (owned by oscar-base-lib)

* Mon Feb  4 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.0-2
- Use rpm standard macro locations.

* Wed Jan 21 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.0.0-1
- new upstream version (see ChangeLog for more details).
