Name:    naemon-oscar-plugins
Summary: Collection of naemon plugins required for OSCAR Monitoring
Version: 1.0
Release: 1
Packager: Olivier LAHAYE <olivier.lahaye@cea.fr>
License: GPL
URL: http://exchange.nagios.org
Group: Applications/System
BuildArch: noarch
Source: naemon-oscar-plugins-1.0.tar.bz2

%define naemon_plugin_dir %{_libdir}/nagios/plugins

%description
Collection of naemon plugins required for OSCAR Monitoring.

%prep
%setup

%install
%{__mkdir_p} %{buildroot}%{_libdir}/nagios/plugins/
%{__cp} plugins/* %{buildroot}%{_libdir}/nagios/plugins/

%files
%doc AUTHORS README
%{_libdir}/nagios/plugins/*

%changelog
* Wed Jun 25 2014 Olivier Lahaye <olivier.lahaye@cea.fr>
- Initial packaging.


