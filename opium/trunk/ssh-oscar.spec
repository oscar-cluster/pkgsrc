# Name of package
%define name ssh-oscar

# Version of package
%define version 1.3.1

#==============================================================

Summary: OSCARified User Synchronization System
Name: %{name}
Version: %{version}
Release: 1
BuildArchitectures: noarch
Packager: Jason Brechin <brechin@ncsa.uiuc.edu>
URL: http://oscar.sourceforge.net/
Source0: ssh-oscar.tar.gz

License: GPL
Group: System
BuildPreReq: rpm >= 3.0.5
Requires: rpm >= 3.0.5, oscar-base-lib
Conflicts: sync-users-oscar
#==============================================================

%description
The OSCAR User Synchronization System keeps the users and groups
synchronizaed from the the central OSCAR server out to the OSCAR
compute nodes.

#==============================================================

%prep

%setup -n %{name}

%install
%__make install DESTDIR=RPM_BUILD_ROOT MANDIR=%_mandir

%post

#%clean
#rm -rf $RPM_BUILD_ROOT

#==============================================================

%preun

#==============================================================

%files

/etc/profile.d/ssh-oscar.sh
/etc/profile.d/ssh-oscar.csh
/etc/profile.d/ssh-oscar
%_mandir/man1/ssh-oscar.1*

#==============================================================

%changelog
* Fri Sep 25 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.3.1-1
- New upstream version (see ChangeLog for more details).

* Thu Apr 30 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.3-2
- Use the mandir macro instead of a hardcoded path.

* Tue Feb 24 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.3-1
- New upstream version (see ChangeLog for more details).

* Tue Feb 10 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.2.2-1
- New upstream version (see ChangeLog for more details).

* Mon Dec 22 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.2.1-1
- New upstream version (see ChangeLog for more details).

* Mon Dec 12 2005 Bernard Li <bli@bcgsc.ca>
- Ensure that SSH keys are not generated for user "nobody"

* Thu Nov 13 2003 Jason Brechin <brechin@ncsa.uiuc.edu>
- Update scripts to use getent

* Mon Jun 10 2002 Jason Brechin <brechin@ncsa.uiuc.edu>
- ssh-oscar and sync_users made into seperate packages

* Wed May 22 2002  16:35:09PM    Thomas Naughton <naughtont@ornl.gov>
- (1.1-1) Updated to work with C3-3.

* Wed Aug 1 2001 Neil Gorsuch <ngorsuch@ncsa.uiuc.edu>
- Initial RPMification
