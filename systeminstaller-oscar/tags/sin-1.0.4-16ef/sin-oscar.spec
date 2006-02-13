%define name            systeminstaller
%define version         1.04
%define release         16ef
%define releasex        %{release}
%define prefix          /usr

%define _unpackaged_files_terminate_build 0

Summary: System Installer
Name: %name
Version: %version
Release: %release
License: GPL
URL: http://systeminstaller.sourceforge.net
Group: Applications/System
Source: %{name}-%{version}.tar.gz
Patch1: sin-01-nodesort
Patch2: sin-02-rhel_support
Patch3: sin-03-updaterpms
Patch4: sin-04-rpmnoscripts
Patch5: sin-05-devfs_devstyle
Patch6: sin-06-api_si35x
Patch7: sin-07-partition_x86_64
Patch8: sin-08-forcearch
Patch9: sin-09-guifix
Patch10: sin-10-guifix2_api_si35x
Patch11: sin-11-fedora
Patch12: sin-12-mandrake
Patch13: sin-13-rhel_stage1
Patch14: sin-14-efi_kernel
Patch15: sin-15-mdraid1
Patch16: sin-16-sl_centos
Patch17: sin-17-mandriva
Patch18: sin-18-mksiimage_update
Patch19: sin-19-dhcpd_multiarch
Patch20: sin-20-buildimage_x86_64
Patch21: sin-21-flame_entry
Patch22: sin-22-flame_mksiimage
Patch23: sin-23-progress
Patch24: sin-24-yn_window
Patch25: sin-25-mkdhcpconf
Patch26: sin-26-getOpenFile
Patch27: sin-27-Help-expand
Patch28: sin-28-mksiadapter
Patch29: sin-29-mdadmconf

BuildArchitectures: noarch
BuildRequires: /usr/bin/perl, perl(AppConfig), systemimager-server >= 3.5.0, systemconfigurator, perl(MLDBM)
Requires: /usr/bin/perl, perl(AppConfig), systemimager-server >= 3.5.0, systemconfigurator, perl(MLDBM)
Vendor: http://oscar.openclustergroup.org
Packager: OSCAR developer team
Prefix: %prefix
Buildroot: /var/tmp/%{name}-%{version}-root
AutoReqProv: no

%package x11
Summary: System Installer Tk Gui
Version: %version
Release: %releasex
License: GPL
URL: http://systeminstaller.sourceforge.net
Group: Applications/System
Requires: systeminstaller >= 1.00, perl-Tk
Vendor: http://oscar.openclustergroup.org
Packager: OSCAR developer team
Prefix: %prefix
Buildroot: /var/tmp/%{name}-%{version}-root
AutoReqProv: no

%description
System Installer provides a unified image building tool. 
It is intended to be distribution and architecture 
independent. It interfaces with SystemImager and 
System Configurator.

%description x11
System Installer Perl Tk User Interface

%prep
%setup -q
%patch1 -p0
%patch2 -p0
%patch3 -p0
%patch4 -p0
%patch5 -p0
%patch6 -p0
%patch7 -p0
%patch8 -p0
%patch9 -p0
%patch10 -p0
%patch11 -p0
%patch12 -p0
%patch13 -p0
%patch14 -p0
%patch15 -p0
%patch16 -p0
%patch17 -p0
%patch18 -p0
%patch19 -p0
%patch20 -p0
%patch21 -p0
%patch22 -p0
%patch23 -p0
%patch24 -p0
%patch25 -p0
%patch26 -p0
%patch27 -p0
%patch28 -p0
%patch29 -p0

# No configure, no make, just copy files to the output dir.
%build
cd $RPM_BUILD_DIR/%{name}-%{version}
mkdir -p /var/tmp/%{name}-%{version}-root/usr/share/man/man5
mkdir -p /var/tmp/%{name}-%{version}-root/usr/share/man/man1
perl Makefile.PL PREFIX=/var/tmp/%{name}-%{version}-root%{prefix} INSTALLSITELIB=/var/tmp/%{name}-%{version}-root/usr/lib/systeminstaller INSTALLMAN1DIR=/var/tmp/%{name}-%{version}-root/usr/share/man/man1 INSTALLMAN3DIR=/var/tmp/%{name}-%{version}-root/usr/share/man/man3
make
make test
make install
rm -rf /var/tmp/%{name}-%{version}-root/usr/lib/systeminstaller/auto*
rm -f /var/tmp/%{name}-%{version}-root/var/lib/sis/*

%clean
#rm -fr $RPM_BUILD_DIR/%{name}-%{version}
rm -rf /var/tmp/%{name}-%{version}-root

%files
%defattr(-,root,root)
%doc README 
%doc CHANGELOG
%doc COPYING
%doc INSTALL
%doc samples/systeminstaller.conf
%doc samples/disktable
%{prefix}/bin/mk*
%{prefix}/bin/buildimage*
%{prefix}/bin/simigratedb*
%doc /usr/share/man/man1/buildimage*
%doc /usr/share/man/man1/simigratedb*
%doc /usr/share/man/man1/mk*
%doc /usr/share/man/man1/SIS*
%doc /usr/share/man/man5/systeminstaller*
%doc /usr/share/man/man3/SIS*
%doc /usr/share/man/man3/SystemInstaller::*
/usr/lib/systeminstaller/SIS
/usr/lib/systeminstaller/Util
/usr/lib/systeminstaller/SystemInstaller/*pm
/usr/lib/systeminstaller/SystemInstaller/Package
/usr/lib/systeminstaller/SystemInstaller/PackageBest
/usr/lib/systeminstaller/SystemInstaller/Image
/usr/lib/systeminstaller/SystemInstaller/Partition
/usr/share/systeminstaller/distinfo
%dir /usr/lib/systeminstaller
%dir /usr/lib/systeminstaller/SystemInstaller
%dir /etc/systeminstaller
%dir /var/lib/sis/
%config /etc/systeminstaller/*

%files x11
%defattr(-,root,root)
%{prefix}/bin/tksis
%dir /usr/lib/systeminstaller/SystemInstaller/Tk
%dir /usr/lib/systeminstaller/Tk
%dir /usr/share/systeminstaller/images
/usr/share/systeminstaller/images/*
%doc /usr/share/man/man1/tksis*
/usr/lib/systeminstaller/SystemInstaller/Tk/*
/usr/lib/systeminstaller/Tk/*

%post
for i in client image adapter; do
    touch /var/lib/sis/$i
done
/usr/bin/simigratedb

%preun
# if last installed instance remove the sis database files
# 
if [ $1 -eq 0 ]; then
    cd /var/lib/sis
    for i in client image adapter; do
        rm -f $i $i.dir $i.pag
    done
fi


%changelog
* Wed Nov 30 2005 Erich Focht <efocht@hpce.nec.com>
- generating /etc/mdadm.conf when the disktable contains RAID definitions

* Fri Jul 29 2005 Erich Focht <efocht@hpce.nec.com>
- fixed removal of sis database files on deinstallation
- added flamethrower.conf update in mksiimage
- deleted image addition to flamethrower.conf in lib/SystemInstaller/Tk/Image.pm

* Wed Jul 27 2005 Erich Focht <efocht@hpce.nec.com>
- fixed flamethrower setup for images

* Wed Jul 20 2005 Erich Focht <efocht@hpce.nec.com>
- added Mandriva and CentOS support
- added --update option to mksiimage (update arch or location)
- enabld coexistence of architectures in dhcpd (mkdhcpd_conf)
- added x64_64 to bin/buildimage

* Fri Jul 15 2005 Erich Focht <efocht@hpce.nec.com>
- repackaged for OSCAR
- moved ugly external patches (after RPM install) into RPM
- splitted up OSCAR 4.X patches into digestible pieces
- support for systemimager-3.5.X (API change)
- support for x86_64
- added forced architecture in rpmlists ($pkg:$arch)
- added support for Scientific Linux
- added support for software raid1

* Tue Dec 17 2002 Michael Chase-Salerno <mchasal@users.sf.net>

- Changes for stable prereqs.

* Wed Dec 4 2002 Michael Chase-Salerno <mchasal@users.sf.net>

- Changes for MLDBM based database.

* Thu Aug 30 2001 Sean Dague <japh@us.ibm.com>

- Initial spec file.
