%define binpref /usr/local/bin
%define libpref /usr/lib
%define manpref /usr/share/man/man3
%define bintarget ${RPM_BUILD_ROOT}%{binpref}
%define mantarget ${RPM_BUILD_ROOT}%{manpref}
%define libtarget ${RPM_BUILD_ROOT}%{libpref}

Summary:		Library abstracting system virtualization solution such as Xen or QEMU.
Name:      		libv3m
Version:   		0.9.6
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
make install DESTDIR=${RPM_BUILD_ROOT}
make install-etc DESTDIR=${RPM_BUILD_ROOT}

%clean
%__rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
/usr/lib/libv3m.a
/usr/include/vmware.h
/usr/include/vmm-hpc.h
/usr/include/xen.h
/usr/include/vm_status.h
/usr/include/VMContainer.h
/usr/include/vm.h
/usr/include/nic.h
/usr/include/qemu.h
/usr/include/ProfileXMLNode.h
/usr/include/xen-hvm.h
/etc/libv3m/v3m_profile.dtd
/etc/libv3m/v3m_config.dtd

%changelog
* Wed Jul 09 2008 Geoffroy Vallee <valleegr@ornl.gov> 0.9.6-1
- New upstream version.
* Tue Jul 02 2008 Geoffroy Vallee <valleegr@ornl.gov> 0.9.5-1
- New upstream version.
* Fri Sep 01 2006 Geoffroy Vallee <valleegr@ornl.gov>
- Import upstream version
