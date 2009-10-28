# $Id:$

# TJN: Addded to get around RPM 4.2 debugging additions (default in RH9.0)
%define debug_package %{nil}
#%define __check_files %{nil}



#--------------------------------------------------------------------------

%define  name       pvm
%define  dname      pvm3
%define  ver        3.4.5+6
%define  rel        2
%define  prefix     /opt/pvm3
%ifarch x86_64
%define  arch       LINUX64
%else
%ifarch ia64
%define  arch       LINUX64
%else
%define  arch       LINUX
%endif
%endif


# Modules-oscar related defines
%define  modulename         pvm                   
%define  moduledir          /opt/modules/oscar-modulefiles/pvm



Summary: Parallel Virtual Machine - Libraries for distributed computing.
Name: %{name} 
Version: %{ver}
Release: %{rel}
License: freely distributable
Group: Development/Libraries
Vendor: Oak Ridge National Laboratory
Source0: %{name}%{ver}.tar.gz 
Patch0: pvm-3.4.5-Pvmtev.patch
Packager: Thomas Naughton <naughtont@ornl.gov>
URL: http://www.csm.ornl.gov/pvm/pvm_home.html
Requires: /usr/sbin/useradd
Requires: /usr/sbin/userdel
BuildRoot: %{_tmppath}/%{name}-%{ver}-root

%description
PVM is a software system that enables a collection of heterogeneous
computers to be used as a coherent and flexible concurrent computational
resource.

The individual computers may be shared- or local-memory multiprocessors,
vector supercomputers, specialized graphics engines, or scalar
workstations, that may be interconnected by a variety of networks,
such as Ethernet, FDDI.

User programs written in C, C++ or Fortran access PVM through library
routines.




%package modules-oscar
Summary: Parallel Virtual Machine (PVM) - OSCAR-ized modules script
Group: Development/Libraries
Requires: pvm = %{ver}
Requires: modules-oscar >= 3.1

%description modules-oscar
This RPM contains a modulefile for PVM on OSCAR clusters.  This module
should be Auto-loaded by the system to set the PATH, MANPATH, PVM_ROOT,
and PVM_ARCH.


#---------------------------------------------------------------------
# Prep install section
#---------------------------------------------------------------------

%prep
# Get rid of any previously built stuff that might cause problems.
# (only worried about things in RPM-land '/usr/src/redhat/', etc.)
%__rm -rf $RPM_BUILD_DIR/%{dname}
%__rm -rf $RPM_BUILD_ROOT/%{prefix}

# Extract the tarball (pvm-3.4.4.tar.gz) to simply "pvm3".
%setup -n %{dname}
%patch0 -p1

#---------------------------------------------------------------------
# Prep [Module-oscar] install section
#---------------------------------------------------------------------

#% prep  pvm-modules-oscar
#%__rm -rf $RPM_BUILD_ROOT/%{moduledir}




#---------------------------------------------------------------------
# Build section
#---------------------------------------------------------------------

%build

# Export our build dir so PVM will make properly
export PVM_ROOT=$RPM_BUILD_DIR/%{dname}
export PVM_ARCH=%{arch}
cd $PVM_ROOT
make



%install

# Copy the build distribution to target install dir.  The files must
# exist in the desired location (path) when RPM checks the filelist. This
# is greatly simplified by using the BuildRoot (chroot sort of thing) method.
%__mkdir  -p $RPM_BUILD_ROOT/%{prefix}
%__cp -Rf $RPM_BUILD_DIR/%{dname}/*  \
	  $RPM_BUILD_ROOT/%{prefix}



#---------------------------------------------------------------------
# Build [Module-oscar] section
#---------------------------------------------------------------------

%__mkdir -p $RPM_BUILD_ROOT/%{moduledir}
%__cat > $RPM_BUILD_ROOT/%{moduledir}/%{ver} <<EOF
#%Module -*- tcl -*-
#
# PVM modulefile for OSCAR clusters
# (based on LAM modulefile)
#

proc ModulesHelp { } {
  puts stderr "\tThis module adds PVM to the PATHand MANPATH."
  puts stderr "\tAdditionally the PVM_ROOT and PVM_ARCH are set."
}

module-whatis   "Sets up the PVM environment for an OSCAR cluster."


# Hardcoding the path/arch and setting up the MANPATH and PATH too.

setenv PVM_RSH  ssh
setenv PVM_ROOT %{prefix} 
setenv PVM_ARCH %{arch}

append-path MANPATH %{prefix}/man

append-path PATH %{prefix}/lib
append-path PATH %{prefix}/lib/%{arch}
append-path PATH %{prefix}/bin/%{arch}

EOF

%__cat > $RPM_BUILD_ROOT/%{moduledir}/.version <<EOF
#%Module1.0
set ModulesVersion %{ver}

EOF


#---------------------------------------------------------------------
# Clean section
#---------------------------------------------------------------------

%clean
# Get rid of any tmp files in RPM land, ie. '/usr/src/redhat/BUILD/...'
%__rm -rf $RPM_BUILD_DIR/%{dname}
%__rm -rf $RPM_BUILD_ROOT




#---------------------------------------------------------------------
# Pre  section
#---------------------------------------------------------------------

%pre


# Descr: Find an open (available) system-level group id (GID)
# Notes:  
#   - use starting GID of 100 and max of 500 (ie. ignoring /etc/login.defs)
#   - typically the system GID's are < 500 so set MAX_GID=500

for (( gid=100,found=0,max_gid=500; $gid < $max_gid && !$found; gid=$gid+1 )) ;
 do
    if ! test `grep :$gid: /etc/group` ; then
        # GID not found, i.e., available
        found=1
    fi
done
gid=$((gid-1))   # Decrement to last used gid

if test $found -gt 0 ; then
    #echo "Use $gid for PVM's GID" 
    pvm_gid=$gid
else
    echo "Error: no available system GID's for PVM" 
    pvm_gid=-1
    exit 1
fi

# Add PVM group or do nothing
/usr/sbin/groupadd -g $pvm_gid  pvm  > /dev/null 2>&1 || :

# Add PVM user or do nothing
/usr/sbin/useradd -d %{prefix} -g $pvm_gid -r -s /bin/bash pvm  > /dev/null 2>&1 || :




#---------------------------------------------------------------------
# Post  section
#---------------------------------------------------------------------
%post
chown -R pvm:pvm $RPM_BUILD_ROOT/%{prefix}




#---------------------------------------------------------------------
# Post-un(install)  section
#---------------------------------------------------------------------

%postun
# Remove the PVM user or do nothing
/usr/sbin/userdel -r  pvm >  /dev/null 2>&1 || :
/usr/sbin/groupdel    pvm >  /dev/null 2>&1 || :



#---------------------------------------------------------------------
# Files section
#
#  List all files that will make it to the target machine here.  Note,
#  that listing the entire dir, gets all the files withing.  The %doc
#  files are treated special and copied into the system doc area (eg. share).
#---------------------------------------------------------------------
%files 
%defattr(-,pvm,pvm)
%doc Readme Readme.Beolin Readme.Beoscyld Readme.Cygwin Readme.mp Readme.Os2 Readme.Win32 doc
%{prefix}


#---------------------------------------------------------------------
# Files [Modules-oscar] section
#---------------------------------------------------------------------
%files modules-oscar
%defattr(-,root,root)
%{moduledir}




#---------------------------------------------------------------------
# ChangeLog section
#---------------------------------------------------------------------
%changelog
* Wed Jun 14 2006    Thomas Naughton  <naughtont@ornl.gov>
- (3.4.5+6-2) Add pvm group using groupadd (not RH-centric "useradd -g")

* Wed May 31 2006    Thomas Naughton  <naughtont@ornl.gov>
- (3.4.5+6-1) Update to newer pvm. removed USESTDERR patch. Add pvmtev patch.

* Sat May 06 2006 Bernard Li <bli@bcgsc.ca>
- Copyright -> License
- Patch global.h to fix build problem with gcc4 (patch taken from
  Fedora Core 5 SRPM)

* Wed Oct 12 2005  Erich Focht
- building on ia64 and x86_64 with LINUX64, on other arches with LINUX.
  This should avoid the need to use "--with linux64" when building an rpm.
* Mon Aug 08 2005  11:40:31AM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.5+4-1) update to newer release, mainly for x86_64 fixes
- Removed the patch for USESTRERROR conf/LINUX.def (already in 3.4.5+4)
- Add a USESTRERROR patch for conf/LINUX64.def   ;)

* Fri Jul 25 2003  14:47:05PM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.4_11-3) add the USESTRERROR patch for conf/LINUX.def
- Rolled the pvm-modules-oscar spec into the standard PVM spec since
  most of the changes to that file were simply to sync the name/ver
  The only useful changelog items were:
   o Wed Jul 03 2002  12:44:32PM    Thomas Naughton  <naughtont@ornl.gov>
     (pvm-3.4.4_6-2)
     Opps, forgot to build on ia64 w/ PVM_ARCH=LINUX64
     Also, had forgot to add PVM_RSH to pvm_modules.tcl so things would work ok.
   o Wed May 29 2002  18:29:37PM    Thomas Naughton  <naughtont@ornl.gov>
     (3.4.4+6-1) First pass at a PVM modules RPM.
- Also, i looked at Jeff Squyres LAM .spec to see why multi-package rpm
  syntax was breaking...not sure, just list seperate file sections.
- Added hooks (hack) for cmdln --with linux64 to build for IA64
   
* Wed Jul 16 2003  23:56:13PM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.4_11-2) adding defines to work with RPM 4.2 on RH9.0.
   specifically: debug_package and __check_files

* Tue Jul 15 2003  01:03:46AM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.4_11-1) 
- Updating to newest version (pre-release) of PVM 3.4.4+11.

* Mon Aug 12 2002  21:46:31PM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.4_6-3)
- Updating pvm-modules to new name and dep for "modules-oscar" renaming,
   I'm also updating the numbering here to keep them consistent.

* Wed Jul 03 2002  12:44:32PM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.4_6-2) 
- Opps, forgot to build on ia64 w/ PVM_ARCH=LINUX64, must set manually
  before you do the 'rpm -ba pvm3.spec' (or make pvm3).
- Also, had forgot to add PVM_RSH to pvm_modules.tcl so things would work ok.

* Wed May 29 2002  18:43:32PM    Thomas Naughton  <naughtont@ornl.gov>
- (pvm-3.4.4_6-1) Upgrading to newer pvm3.4.4_6
- TAG=(pvm-3-4-4-6-r1) to get from CVS, 'cvs -r pvm-3-4-4-6-r1 rpm-specs/pvm'
- Using the BuildRoot version of things (provided by Jeff Squyres for C3 rpm)
  Greatly simplifies things and fixes several other problems that existed.
- Removing the Source1&2 patches and related 'profile.d/' stuff-o.
- Commenting out the MAN stuff.
- Fixed the "-e" problems, and should fix the "-U' problems also!

* Tue May 14 2002  10:33:21AM    modified by:  tjn <naughtont@ornl.gov>
- Adding this to a CVS repository...not changing the rel# (3.4.3+7-2)  
- TAG=(pvm-3-4-3-7-2) to get from CVS, 'cvs -r pvm-3-4-3-7-2 rpm-specs/pvm'

* Tue Jul 31 2001  14:06:02PM    modified by:  tjn <naughtont@ornl.gov>
- check for exiting defs on the /etc/profile.d/  files

* Wed Jun 20 2001 Thomas Naughton <naughtontiii@ornl.gov>
- Converted over to using the defines to allow a bit more reuse for other RPMs.

* Mon Jun 18 2001 Thomas Naughton <naughtontiii@ornl.gov>
- Copying shell stubs to /etc/profile.d/.  Also, cleaning up ENV_VAR use.
  Refer to p.181 of "Maximum RPM" by Edward Bailey for RPM ENV VARs

* Fri Jun 08 2001 Thomas Naughton <naughtontiii@ornl.gov>
- Changed some sh and added stuff for removal of man pages

* Thu Jun 7 2001 massive debugging John & Thomas

* Thu Jun  7 2001 Thomas Naughton <naughtontiii@ornl.gov>
- Added some /bin/sh to help with install.

* Wed Jun  6 2001 John Mugler <muglerj@ornl.gov>
- Creation of spec file (using RedHat's as template) for ORNL's RPM ver.

