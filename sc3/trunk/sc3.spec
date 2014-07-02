# $Id$
Summary: Subcluster command and control tools
Name: sc3
Version: 1.2.4
Vendor: NEC HPCE
Release: 1
License: GPL
Packager: Erich Focht <efocht@hpce.nec.com>
Source: sc3.tar.gz
Group: System Environment/Tools
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}
Requires: c3 >= 5.0.1
Requires: systeminstaller
#AutoReqProv: no

%description 

Subcluster tools based on the C3 cluster command and control utilities.
One can define subclusters by one of: image, domain name, nodelist. The user
commands scexec, scpush and scrpm allow, respectively, the parallel execution
of commands on the entire subcluster, pushing files to each node of a subcluster
and rpm operations to each subcluster node. If the subcluster definition is
image-based the commands can be applied to the node's image, too. Or only to the
image. This helps to keep cluster nodes and their images in sync.

%prep
%setup -q -n %{name}

%build


%install
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/scexec
%{_bindir}/scrpm
%{_bindir}/scpush
%{perl_vendorlib}/HPCL

%changelog
* Thu Mar 12 2009 Geoffroy Vallee <valleegr@ornl.gov> 1.2.3-1
- New upstream release (see ChangeLog for more details).
* Fri Nov 07 2008 Geoffroy Vallee <valleegr@ornl.gov> 1.2.1-1
- New upstream release (see ChangeLog for more details).
* Thu Dec 01 2005 Erich Focht
- Subcluster fix: filehandle instead of file.
- Added verbose output to subcluster commands
- Added SC3_MINSCAL env variable for scalable config limit.
- Increased default scalable config limit to 32.
* Thu Dec 01 2005 Erich Focht
- added gstat ganglia check for online nodes, falls back to cexec
- reduced limit for scalable setup to 16
* Wed Dec 29 2004 Erich Focht
- changed location of module to /usr/lib/systeminstaller/HPCL
* Thu Dec 23 2004 Erich Focht
- initial RPM
