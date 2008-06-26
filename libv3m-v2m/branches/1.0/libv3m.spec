%define binpref /usr/local/bin
%define libpref /usr/lib
%define manpref /usr/share/man/man3
%define bintarget ${RPM_BUILD_ROOT}%{binpref}
%define mantarget ${RPM_BUILD_ROOT}%{manpref}
%define libtarget ${RPM_BUILD_ROOT}%{libpref}

Summary:		Library abstracting system virtualization solution such as Xen or QEMU.
Name:      		libv3m
Version:   		0.9.4
Release:   		1
Vendor:			Oak Ridge National Laboratory
Distribution:   	Fedore Core 4
Packager:		Geoffroy Vallee <valleegr@ornl.gov>
License: 		GPL
Group:     		Development/Libraries
Source:			%{name}-%{version}.tar.gz
BuildRoot: 		%{_localstatedir}/tmp/%{name}
BuildArch:		i686

%description
libv3m is a library absracting system-virtualization solutions. With libv3m, it
is possible to manage XML profile that describes a virtual machine with an 
high-level description, then all the technical details are managed by libv3m
(network configuration, image management, and so on).

%prep
%setup -n %{name}
./configure

%build
make

%install
%__install -m 755 -d %{libtarget}
%__install -m 755 libv3m.a %{libtarget}

%clean
%__rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
/usr/lib/libv3m.a

%changelog
* Fri Sep 01 2006 Geoffroy Vallee <valleegr@ornl.gov>
- Import upstream version
