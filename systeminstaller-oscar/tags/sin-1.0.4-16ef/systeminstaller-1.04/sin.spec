%define name            systeminstaller
%define version         1.04
%define release         1
%define releasex         1
%define prefix          /usr


Summary: System Installer
Name: %name
Version: %version
Release: %release
Copyright: GPL
URL: http://systeminstaller.sourceforge.net
Group: Applications/System
Source: %{name}-%{version}.tar.gz
BuildArchitectures: noarch
Requires: /usr/bin/perl, libappconfig-perl, systemimager-server >= 3.0.0, systemconfigurator, perl-MLDBM
Vendor: http://sisuite.org
Packager: SIS Devel Team <sisuite-devel@lists.sf.net>
Prefix: %prefix
Buildroot: /var/tmp/%{name}-%{version}-root
AutoReqProv: no

%package x11
Summary: System Installer Tk Gui
Version: %version
Release: %releasex
Copyright: GPL
URL: http://systeminstaller.sourceforge.net
Group: Applications/System
Requires: systeminstaller >= 1.00, perl-Tk
Vendor: http://sisuite.org
Packager: SIS Devel Team <sisuite-devel@lists.sf.net>
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
%setup -c -n %{name}-%{version}

# No configure, no make, just copy files to the output dir.
%build
cd $RPM_BUILD_DIR/%{name}-%{version}/%{name}-%{version}/
mkdir -p /var/tmp/%{name}-%{version}-root/usr/share/man/man5
mkdir -p /var/tmp/%{name}-%{version}-root/usr/share/man/man1
perl Makefile.PL PREFIX=/var/tmp/%{name}-%{version}-root%{prefix} INSTALLSITELIB=/var/tmp/%{name}-%{version}-root/usr/lib/systeminstaller INSTALLMAN1DIR=/var/tmp/%{name}-%{version}-root/usr/share/man/man1 INSTALLMAN3DIR=/var/tmp/%{name}-%{version}-root/usr/share/man/man3
make
make test
make install
rm -rf /var/tmp/%{name}-%{version}-root/usr/lib/systeminstaller/auto*

%clean
rm -fr $RPM_BUILD_DIR/%{name}-%{version}/%{name}-%{version}/
rm -rf /var/tmp/%{name}-%{version}-root

%files
%defattr(-,root,root)
%doc %{name}-%{version}/README 
%doc %{name}-%{version}/CHANGELOG
%doc %{name}-%{version}/COPYING
%doc %{name}-%{version}/INSTALL
%doc %{name}-%{version}/samples/systeminstaller.conf
%doc %{name}-%{version}/samples/disktable
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
%dir /var/lib/sis
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
# This populates the database
for i in client image adapter; do
    touch /var/lib/sis/$i
done
/usr/bin/simigratedb

%preun
# This removes the clamdr database files when
# the last instance of systeminstaller is removed
if [ $1 -eq 0 ]; then
    for i in client image adapter; do
        rm -f /var/lib/sis/$i
        rm -f /var/lib/sis/$i.dir
        rm -f /var/lib/sis/$i.pag
    done
fi


%changelog
* Tue Dec 17 2002 Michael Chase-Salerno <mchasal@users.sf.net>

- Changes for stable prereqs.

* Wed Dec 4 2002 Michael Chase-Salerno <mchasal@users.sf.net>

- Changes for MLDBM based database.

* Thu Aug 30 2001 Sean Dague <japh@us.ibm.com>

- Initial spec file.
