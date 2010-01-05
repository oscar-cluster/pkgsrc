# $Id: netbootmgr.spec 6252 2007-04-13 12:06:10Z focht $
Summary: Manage next network boot action for a cluster
Name: netbootmgr
Version: 1.7
Vendor: NEC HPCE
Release: 1
License: GPL
Packager: Erich Focht <efocht@hpce.nec.com>
Source: %{name}-%{version}.tar.gz
Group: System Environment/Tools
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}
BuildRequires: perl-Qt
Requires: perl-Qt
AutoReqProv: no

%description 
Netbootmgr provides a GUI interface for managing the next boot action for
network booted hosts. It does so by using pxelinux' or elilo.efi's capability
to load an IP specific configuration file from the network. Netbootmgr
creates/deletes symbolic links pointing to predefined pxelinux/elilo config
files.

%prep
%setup -n %{name}-%{version}


%build
# currently the puic path is hardcoded
/opt/perl-Qt/bin/puic -o netBootMgr.pm netbootmgr.ui
/opt/perl-Qt/bin/puic -o sureDialog.pm suredialog.ui

%install

for dir in /usr/bin /usr/share/%{name} /etc %{_mandir}/man8 ; do
    install -d -o root -g root -m 755 $RPM_BUILD_ROOT$dir
done
for file in netbootmgr netbootmgr-status/* netbootmgr-cmd ; do
    base=`basename $file`
    install -o root -g root -m 755 $file $RPM_BUILD_ROOT/usr/bin/$base
done
for file in *.pm localboot kernel-x memtest86 memtest86+-1.65.bin hostdb.test ; do
    install -o root -g root -m 755 $file $RPM_BUILD_ROOT/usr/share/%{name}
done
install -o root -g root -m 755 netbootmgr.conf $RPM_BUILD_ROOT/etc
install -o root -g root -m 644 netbootmgr.8 $RPM_BUILD_ROOT%{_mandir}/man8

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin/*
/usr/share/%{name}/*
%config(noreplace) /etc/netbootmgr.conf
%{_mandir}/man8/netbootmgr.*

%post
if [ -d /tftpboot ]; then
   for f in localboot kernel-x memtest86 memtest86+-1.65.bin; do
      if [ ! -f /tftpboot/$f ]; then
         cp -p /usr/share/netbootmgr/$f /tftpboot
      fi
   done
fi

%preun
# remove installed sample files if last instance of netbootmgr
# is being erased
if [ $1 -eq 0 ]; then
   for f in localboot kernel-x memtest86 memtest86+-1.65.bin; do
      if [ -f /tftpboot/$f ]; then
         rm -f /tftpboot/$f
      fi
   done
fi

%changelog
* Fri Apr 13 2007 Erich Focht 1.7-1
- added verbose variable for netbootlib
- added netbootmgr-cmd as command line interface for netbootmgr functionality
* Tue Feb 13 2007 Erich Focht 1.6-1
- fixed issue with ID LED not working.
* Tue Feb 13 2007 Erich Focht 1.5-1
- added cpower support for node status and power actions
* Fri Nov 04 2005 Erich Focht
- added man page
- added %post section, copying some sample files to /tftpboot
- added %preun section for deleting sample files from /tftpboot
* Wed Nov 02 2005 Erich Focht
- initial RPM
