%define _unpackaged_files_terminate_build 0

%define binpref /usr/bin
%define libpref /usr/lib
%define manpref /usr/share/man/man3
%define configdir /etc/v3m
%define contribdir /opt/v2m/contrib
%define bintarget ${RPM_BUILD_ROOT}%{binpref}
%define mantarget ${RPM_BUILD_ROOT}%{manpref}
%define libtarget ${RPM_BUILD_ROOT}%{libpref}
%define imagedir /opt/v2m/data
%define imagetarget ${RPM_BUILD_ROOT}%{imagedir}
%define configtarget ${RPM_BUILD_ROOT}%{configdir}
%define contribtarget ${RPM_BUILD_ROOT}%{contribdir}

%define libv3mversion 0.2

Summary:		CLI for libv3m.
Name:      		v2m
Version:   		0.9.7
Release:   		1
Vendor:			Oak Ridge National Laboratory
Distribution:   CentOS 5
Packager:		Geoffroy Vallee <valleegr@ornl.gov>
License: 		GPL
Group:     		Application/System
Source0:		%{name}-%{version}.tar.gz
Source1:  		libv3m-%{libv3mversion}.tar.gz        
BuildRoot: 		/tmp/%{name}-buildroot
Requires:       util-linux, device-mapper-multipath, libxml++, bzip2, sudo

%description
V2M (Virtual Machine Management) is a CLI for libv3m.

%package contrib
Summary: Contributions for V2M (netboot simulator image)
Group: Application/System
Requires: v2m = %{version}

%description contrib
V2M (Virtual Machine Management) is a CLI for libv3m, contribution component.
This component currently includes an image for the simulation of an OSCAR
network installation (the booting part), i.e., simulation PXE, dhcp and so on.

%prep
%setup -b 1 -n libv3m-%{libv3mversion}
%setup -b 0 -n %{name}-%{version}

cd ../libv3m-%{libv3mversion}; export PKG_CONFIG_PATH=/usr/lib/pkgconfig && ./configure && make
cd ../v2m-%{version}; ./configure --with-libv3m=${RPM_BUILD_DIR}/libv3m-%{libv3mversion} --prefix=${RPM_BUILD_ROOT}/usr

%build
make

%install
mkdir -p %{bintarget}
make install
mkdir -p %{imagetarget}
mkdir -p %{configtarget}
cp ../libv3m-%{libv3mversion}/dtd/*dtd %{configtarget}
mkdir -p %{contribtarget}/bin
cp contrib/bin/* %{contribtarget}/bin

%clean
%__rm -rf $RPM_BUILD_ROOT

%post contrib
%__rm -f %{contribdir}/bin/netboot_emulation_1.9.img
/usr/bin/bunzip2 %{contribdir}/bin/netboot_emulation_1.9.img.bz2

%files 
%defattr(-,root,root)
%{binpref}/v2m
%{configdir}/v3m_config.dtd
%{configdir}/v3m_profile.dtd
%dir %{imagedir}

%files contrib
%defattr(-,root,root)
%{contribdir}/bin/netboot_emulation_1.9.img.bz2
%{contribdir}/bin/oscar_bootcd.iso

%changelog
* Tue Jun 23 2009 Geoffroy Vallee <valleegr@ornl.gov> - 0.9.7-1
- New upstream version.
* Thu Oct 30 2007 Geoffroy Vallee <valleegr@ornl.gov> 
- New upstream version.
* Mon Apr 23 2007 Geoffroy Vallee <valleegr@ornl.gov>
- Add a dependency to sudo.
* Thu Apr 19 2007 Geoffroy Vallee <valleegr@ornl.gov>
- Add a contrib RPM.
* Wed Apr 18 2007 Geoffroy Vallee <valleegr@ornl.gov>
- Add dependencies to util-linux, device-mapper-multipath.
* Tue Apr 10 2007 Geoffroy Vallee <valleegr@ornl.gov>
- Create a new /opt/v2m/data directory to store images.
- Copy the configuration files in /etc/v3m.
* Fri Feb 23 2007 Geoffroy Vallee <valleegr@ornl.gov>
- Import new upstream version.
* Fri Sep 01 2006 Geoffroy Vallee <valleegr@ornl.gov>
- Import upstream version.
