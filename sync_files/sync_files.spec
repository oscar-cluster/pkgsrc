# Installation Location
%define install_dir /opt/sync_files

#==============================================================

Summary: OSCARized File Synchronization System
Name: sync_files
Version: 2.4
Release: 1
BuildArchitectures: noarch
Distribution: OSCAR
Packager: Erich Focht <efocht@hpce.nec.com>
URL: http://oscar.sourceforge.net/
Source0: sync_files.tar.gz

License: GPL
Group: System
BuildPreReq: rpm >= 3.0.5
AutoReq: 0
Requires: c3 >= 3.0
Requires: /usr/bin/md5sum
Requires: /bin/sh 
Requires: util-linux
Requires: perl
Requires: /usr/bin/perl
Requires: perl-AppConfig
Requires: perl(Getopt::Long)

Obsoletes: sync-users-oscar

BuildRoot: /var/tmp/%{name}-%{version}-buildroot
#==============================================================

%description
The OSCAR File Synchronization System keeps any list of files 
synchronizaed from the the central OSCAR server out to the OSCAR
compute nodes and (optionally) client image(s).  Whenever any of the 
following happens, the files on the OSCAR server are copied to the OSCAR clients:
periodically if changed (defaults to every 15 minutes)
forced update (/opt/sync_files/bin/sync_files --force).

#==============================================================

%prep

%setup -n sync_files

%install
mkdir -p $RPM_BUILD_ROOT%{install_dir}/bin
mkdir -p $RPM_BUILD_ROOT%{install_dir}/etc
mkdir -p $RPM_BUILD_ROOT%{install_dir}/tmp
cp -p sync_files $RPM_BUILD_ROOT%{install_dir}/bin/sync_files
cp -p sync_files.conf $RPM_BUILD_ROOT%{install_dir}/etc/sync_files.conf
cp -p confmgr $RPM_BUILD_ROOT%{install_dir}/bin/confmgr
cp -pr templates $RPM_BUILD_ROOT%{install_dir}

#==============================================================

%preun
grep -ve '%{install_dir}/bin/sync_files' /etc/crontab > /etc/crontab.preun
mv /etc/crontab.preun /etc/crontab

#==============================================================

%files

%{install_dir}/bin/sync_files
%{install_dir}/bin/confmgr
%config %{install_dir}/etc/sync_files.conf
%dir %{install_dir}/tmp
%{install_dir}/templates

#==============================================================

%changelog
* Tue Oct 25 2006 Erich Focht
- added filter for image specific templates
- added options for filter-only
- moved templates to templates/distro/*
- added mandriva distro templates
- version: 2.4-1
* Wed Dec 14 2005 Erich Focht
- fixed bug: missing "use" for SIS::DB and SIS::Image
- version: 2.1-1
* Tue Nov 29 2005 Erich Focht
- Changed distro_detect.
- Added multi-distro functionality and failure (host-down) detection.
- When executed by cron sync_users doesn't have the USER env variable set.
  Changed crontab entry format to add "env USER=root".

* Thu Aug 18 2005 (1.8-1) Erich Focht
- Added error checking when creating passwd, group and shadow files.
  Solves the issue that a filled / FS leads to all nodes getting
  zero length passwd/shadow files and become unaccessible.
* Fri Dec 03 2004 (1.7-1) Jason Brechin
- Fix permissions on temp files

* Mon Feb 16 2004 (1.6-5) Jason Brechin
- Added Thomas Naughton's request of sorting passwd/group by id, not name

* Fri Feb 13 2004 (1.6-4) Jason Brechin
- Tested and fixed a couple bugs

* Wed Dec 31 2003 (1.6-3) Jason Brechin
- Make sync_files more independent of c3 version by sourcing c3 profile script

* Thu Dec 18 2003 (1.6-2) Jason Brechin
- Add configuration manager to add/remove files easier 

* Mon Dec 08 2003 (1.6-1) Jason Brechin <brechin@ncsa.uiuc.edu>
- Update name to sync_files, separate out user-file specifics

* Tue Jun 24 2003 (1.5-5) Thomas Naughton <naughtont@ornl.gov> 
- Updated to work with C3-4, path changed c3-3 -> c3-4 (for oscar-2.3)

* Thu May 29 2003 (1.5-3) Jason Brechin <brechin@ncsa.uiuc.edu>
- Fixed some bugs (missing features), added some features

* Thu May 29 2003 (1.5-1)  Benoit des Ligneris <benoit@des.ligneris.net>
- Added support for pam.d based authentification 

* Thu Jan 16 2003 (1.4-2) Added SuSE compatible stuff

* Wed Aug 21 2002 (1.3-8) Fixed --crononly problem

* Sun Aug 18 2002 (1.3-4) Fixed --force failure (patched)

* Fri Jul 26 2002 (1.3-1) Ported to Perl, .conf file changed

* Thu Jun 13 2002 (1.2-7) More cron changes

* Thu Jun 13 2002 (1.2-6) Reordered cron function to go more smoothly

* Wed Jun 12 2002 (1.2-5) Updated cron functionality (again)

* Wed Jun 12 2002 (1.2-4) Added logger command to sync_users and some commenting to the .conf file

* Wed Jun 12 2002  Jason Brechin <brechin@ncsa.uiuc.edu>
- (1.2-3) Updated CLI and default cron interval

* Mon Jun 10 2002 (1.2-2) Added crontab functionality and updated .conf file

* Mon Jun 10 2002  Jason Brechin <brechin@ncsa.uiuc.edu>
- (1.2-1) Split sync_users from the ssh scripts and updated functionality

* Wed May 22 2002  16:35:09PM    Thomas Naughton <naughtont@ornl.gov>
- (1.1-1) Updated to work with C3-3.

* Wed Aug 1 2001 Neil Gorsuch <ngorsuch@ncsa.uiuc.edu>
- Initial RPMification
