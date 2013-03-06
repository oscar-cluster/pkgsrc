# $Id$

# Check if Suggests: tag can be sused.
%define has_rpm_suggests %(rpm --help | grep "\-\-suggest" | wc -l)

Summary: Wrapper to yum for clusters
Name: yume
Version: 2.8.12
Vendor: Open Source Cluster Group
Release: 0.2
License: GPL
Packager: Geoffroy Vallee <valleegr@ornl.gov>
Source: %{name}-%{version}.tar.gz
Group: System Environment/Tools
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}
Requires: yum >= 2.4.0
Requires: oscar-utils >= 6.1.2
# perl-IO-Tty required by ptty_try which is used in tests.
Requires: perl-IO-Tty
# actually "createrepo" is also needed, but only on the master node,
# so don't add it to the requires. (opkg-yume-server handles this Requires:)
AutoReqProv: no
# If rpm version >= 4.6, then Suggests yum-utils (need for repoquery)
%if %has_rpm_suggests
Suggests: yum-utils
%endif

%description 

Tool for setting up, exporting yum repositories and executing
yum commands for only these repositories. Use it as high level RPM
replacement which resolves dependencies automatically. This tool
is very useful for clusters. It can:
- prepare an rpm repository
- export it through apache
- execute yum commands applying only to this repository (locally)
- execute yum commands on the cluster nodes applying only to this repository.
This makes installing packages, creating cluster node images, updating
revisions much simpler than with rpm.
In addition, yume can just query the specified repositories by invoking
repoquery.

%prep
%setup -n %{name}-%{version}


%build


%install

%__make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/*
%{_datadir}/%{name}/*
%{_mandir}/man8/yume*
%{_mandir}/man1/ptty_try*

%changelog
* Wed Mar  6 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 2.8.12-0.2
- Updated the requirement for distro-query: => oscar-utils
* Wed Mar  6 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 2.8.12-0.1
- new upstream version (see ChangeLog for more details).
- Use make install instead of manual copy of files.
- Made ptty_try man correctly generated and packaged.
- Restored perl-IO-Tty requirement so ptty_try works (used in tests)
- Added /usr/bin/distro-query requirement (yume needs this even on nodes)
* Fri Oct 30 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.11-1
- new upstream version (see ChangeLog for more details).
* Thu Jul 16 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.10-1
- new upstream version (see ChangeLog for more details).
* Thu Apr 23 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.9-1
- new upstream version (see ChangeLog for more details).
* Tue Apr 07 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.8-1
- new upstream version (see ChangeLog for more details).
* Thu Mar 19 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.7-1
- new upstream version (see ChangeLog for more details).
* Wed Mar 18 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.6-1
- new upstream version (see ChangeLog for more details).
* Tue Feb 10 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.5-1
- new upstream version (see ChangeLog for more details).
* Wed Jan 22 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.4-1
- new upstream version (see ChangeLog for more details).
* Mon Jan 19 2009 Geoffroy Vallee <valleegr@ornl.gov> 2.8.3-1
- new upstream version (see ChangeLog for more details).
* Wed Nov 05 2008 Geoffroy Vallee <valleegr@ornl.gov> 2.8.1-1
- new upstream version (see ChangeLog for more details).
* Mon Mar 17 2008 Erich Focht -> 2.8-1
- added master recognition for failover case (heartbeat version).
* Thu Nov 1 2007 Geoffroy Vallee 2.7-2
- fix a bad handling of the return code.
* Sun Oct 28 2007 Erich Focht 2.7-1
- added repoadd, repodel, repolist repository manipulation funtionality for OSCAR headnodes.
* Fri Oct 26 2007 Erich Focht 2.6-1
- certain yum versions are too chatty, fixed version detection
* Fri Sep 14 2007 Erich Focht 2.5-1
- fixed repoquery for newer yum versions
* Tue Sep 19 2006 Erich Focht
- Improved scalability by dealing with timeouts when calling distro-query
- More detailed errors
- version: 2.3-1
* Wed Aug 10 2006 Erich Focht
- fixed problem with yume invocation on SUSE clients
* Wed Jun 21 2006 Erich Focht
- suse_bootstrap support for installing into empty suse images
* Thu Jun 01 2006 Erich Focht
- including yum-repoquery and removing dependency of yum-utils.
* Wed May 31 2006 Erich Focht
- added rpm groups support (e.g. yume install @eclipse)
- added repoquery support (--repoquery)
- moved ptty_try log file to /tmp, cleaning it up at interruption
* Thu May 25 2006 Erich Focht
- added mirror:http://mirrorlist_url/ option handling
* Mon Mar 06 2006 Erich Focht
- significantly improved functionality of yume-opkg
* Tue Feb 21 2006 Erich Focht
- limit architectures of installed packages (if not specified),
  this should avoid installing all compatible architectures of a package
  on a x86_64. Detects arch from name of repository.
* Mon Feb 20 2006 Erich Focht
- added env variable YUME_VERBOSE
- added debugging output
- added correct return codes when subcommands fail
* Thu Feb 16 2006 Erich Focht
- removed need for "--" to separate yum arguments
- changed exported repository URL path to /repo/$repopath
- added default repository detection for OSCAR clusters.
* Wed Feb 01 2006 Erich Focht
- added ptty_try (otherwise no progress bar in systeminstaller)
- updated version to 0.3-1
* Mon Dec 12 2005 Erich Focht
- chop trailing "/" from repo paths, otherwise getting trouble with basename
- version 0.2-6
* Thu Sep 15 2005 Erich Focht
- added yume-opkg
- added rpmlists for rhel4 i386 and x86_64 to /usr/share/yume
* Thu Sep 08 2005 Erich Focht
- initial RPM
