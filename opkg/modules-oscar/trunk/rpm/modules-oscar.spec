#
# Copyright (c) 2002-2003 The Trustees of Indiana University.  
#                         All rights reserved.
#
# This file is part of the modules-oscar software package.  For
# license information, see the LICENSE file in the top-level directory
# of the modules-oscar source distribution.
#
# $Id: modules-oscar.spec,v 1.17 2003/07/21 13:15:59 jsquyres Exp $
#

#############################################################################
#
# Helpful Defines
#
#############################################################################

%define _moddir opt/modules
%define _profiledir /etc/profile.d
%define _modrpmfilelist /etc/%{name}-%{version}-rpmfiles
%define main_version 3.3.a

# Added to get around RPM 4.2 debugging additions (starting in RH 8.0)
#%define debug_package %{nil}
#%define __check_files %{nil}


#############################################################################
#
# Preamble Section
#
#############################################################################

Summary: Modules package
Name: modules-oscar
Version: %{main_version}
Release: 3
License: GPL
Group: Applications/Environment
Source0: modules-oscar-1.0.5.tar.gz
Source1: modules-%{version}.tar.bz2
Source2: Modules-Paper.pdf
Source3: Modules-Paper.doc
Patch0: modules-3.3.a_Modules_fix.patch
Patch1: modules-3.3.a_configure_ac.patch
Patch2: modules-3.3.a_modulespath.patch
Patch3: modules_3.3.a_ModulePath.patch
URL: http://modules.sourceforge.net/
Packager: Open Cluster Group / OSCAR working group
BuildRequires: tcl-devel
Requires: tcl
Requires: coreutils grep make
#Requires: %__mv %__rm %__cp %__cat %__mkdir %__chmod %__grep %__make
Requires: /bin/ed
Requires: /usr/bin/test

%description 
The Modules package provides for the dynamic modification of the
environment of the user via modulefiles.  Each modulefile contains the
information needed to configure the shell for an application. Once the
Modules package is initialized, the environment can be modified on a
per-module basis using the module command which interprets
modulefiles.  Typically modulefiles instruct the module command to
alter or set shell environment variables such as PATH, MANPATH, etc.
modulefiles may be shared by many users on a system and users may have
their own collection to supplement or replace the shared modulefiles.

This RPM is named "modules-oscar" to differentiate it from the Linux
kernel modules RPM.  It includes a series of customiations of a
vanilla Modules install suitable for OSCAR clusters.


#############################################################################
#
# Prep Section
#
#############################################################################
%prep
# Cleanup previous build if any.
%__rm -rf %{buildroot}
%setup -q -n modules-oscar-1.0.5
cd ..
%setup -q -T -D -b 1 -n modules-%{main_version}

cp %SOURCE2 ./doc/
cp %SOURCE3 ./doc/

%patch0 -p1
%patch1 -p1
%patch2 -p1
%patch3 -p1

# Otherwise, this directory shows up on security reports

chmod -R o-w $RPM_BUILD_DIR/modules-oscar-1.0.5
chmod -R o-w $RPM_BUILD_DIR/modules-%{main_version}


#############################################################################
#
# Build Section
#
#############################################################################
%build

# Essentially taken from the modules-supplied RKOConfigure script
# (took out the X stuff, and added --without-x because we don't use
# any of the extra X stuff); modified for OSCAR clusters.  The
# @VERSION@ macro is part of the Modules configure process and is
# expanded DURING configure.  If you instead specify
# /opt/modules/VERSION (or whatever version you are building), it
# seems that the configure process gets very confused.  So we are just
# going to do what the modules people recommend. :)

#CFLAGS="$RPM_OPT_FLAGS"
#export CFLAGS

./configure \
	--prefix=/%{_moddir} \
	--with-module-path=/%{_moddir}/modulefiles \
	--with-version-path=/%{_moddir}/version \
	--with-etc-path=/etc \
	--with-skel-path=/etc/skel \
	--with-split-size=960 \
	--with-tcl-inc=/usr/include \
	--with-tcl-lib=/usr/%{_lib} \
	--without-x
make


#############################################################################
#
# Install Section
#
#############################################################################
%install

#__cp %SOURCE2 %SOURCE3 ./doc/
#makeinstall # DESTDIR=%{buildroot}
#makeinstall
%__make install DESTDIR=%{buildroot}

# Set the default symlink, which is used in modules...
#%__mkdir_p %{buildroot}/%{_moddir}/Modules
#(cd %{buildroot}; %__ln_s /%{_moddir}/Modules/%{main_version} %{_moddir}/Modules/default)
(cd %{buildroot}; %__ln_s /%{_moddir}/%{main_version} %{_moddir}/default)

# Make the directory where people are supposed to install their own config files...
%__mkdir_p %{buildroot}/%{_moddir}/modulefiles

# Now make a directory where OSCAR-specific modulefiles will go
%__mkdir_p %{buildroot}/%{_moddir}/oscar-modulefiles

# Install the *.OSCAR files
srcdir="$RPM_BUILD_DIR/modules-oscar-1.0.5"
%__cp $srcdir/README.OSCAR .
%__cp $srcdir/LICENSE.OSCAR .
%__cp $srcdir/AUTHORS.OSCAR .

# Install the "oscar" module and set its default
destdir="%{buildroot}/%{_moddir}/modulefiles/oscar-modules"
%__mkdir_p $destdir

%__cp $srcdir/src/oscar.tcl $destdir/1.0.5
%__cat > $destdir/.version << EOF
#%Module
set ModulesVersion 1.0.5
EOF
unset destdir

# For the OSCAR RPM, we have to make some changes to the shell setup
# stuff that comes with the vanilla modules distribution.  We:
#
# - discard most of the per-login make-empty-variables setup stuff
#   that's in the modules distro in etc/global/csh.login and
#   etc/global/profile.
# - keep the per-login modules init stuff from etc/global/csh.modules
#   and etc/global/profile.modules.
# - keep the per-shell-invcation modules init stuff from
#   etc/global/csh.cshrc and etc/global/bashrc.
#
# Additionally, we have to combine all three things into one file for
# each shell because this one file has to go into /etc/profile.d so
# that it can get run for every shell invocation.  The per-login stuff
# is protected by statements that check for the existence of
# environment variables, so even though the script is run for every
# shell invocation, that stuff is only run once (for the first --
# login -- shell).
#
# The reason that this stuff is all combined into one file and is put
# in /etc/profile.d is because of Evilness in rsh/ssh (they both do
# the same thing).  When a user executes "rsh somehost who", the shell
# that "who" runs in on the target machine is *not* a login shell.  So
# all the login-setup stuff is not run (/etc/csh.login and
# /etc/profile).  Only the per-shell-invcation stuff is run.  Hence,
# we have to put everything into the per-shell-invocation stuff (i.e.,
# scripts in /etc/profile.d), and protect the only-run-once stuff with
# environment variable checking.
#
# BTW, this is all documented in the spec file in order to speed
# parsing/processing time of the scripts that are put in
# /etc/profile.d.  Putting this many comments in those scripts --
# scripts that are run for *EVERY* shell invocation -- just seems like
# needless overhead.

# Save any original %{_profiledir}/00-modules.* files

#for file in 00-modules.csh 00-modules.sh; do
#	if test -f %{_profiledir}/$file; then
#		cp %{_profiledir}/$file %{buildroot}%{_profiledir}/$file.rpmbuild
#	fi
#done

# Now create the profile directory were modules environment init files will go.
%__mkdir_p %{buildroot}%{_profiledir}

# Take our OSCAR-ized template files and insert the files from the
# modules distribution.

file=ed-commands.txt

# First, bash: Fix init scripts PATH.
sed -i -e 's|@MODDIR@|/%{_moddir}|g' $srcdir/src/00-modules.sh

# Copy the resulting file to %{_profiledir}.

%__cp $srcdir/src/00-modules.sh %{buildroot}%{_profiledir}
%__chmod +x %{buildroot}%{_profiledir}/00-modules.sh

# Now do the csh version.

%__cat > $file <<EOF
/INSERT-CSH-MODULES-HERE
d
-
. r etc/global/csh.modules
/INSERT-CSH-CSHRC-HERE
d
-
. r etc/global/csh.cshrc
w
q
EOF
ed $srcdir/src/00-modules.csh < ed-commands.txt
%__rm -f $file

# Copy the resulting file to the %{_profiledir}.

%__cp $srcdir/src/00-modules.csh %{buildroot}%{_profiledir}
%__chmod +x %{buildroot}%{_profiledir}/00-modules.csh


#############################################################################
#
# Post Section
#
#############################################################################
# Not needed on rhel >= 5 and Fedora >= 10
%if ! (0%{?fedora} >= 10 || 0%{?rhel} >= 6)

%post

# Now make the bash system startup file source the
# %{_profiledir}/00-modules.sh files.  Blech.  We have to do this
# because some distros (e.g., RH 7.1) have a /etc/bashrc that doesn't
# run the scrips in /etc/profile.d for non-interactive shells (e.g.,
# rsh somehost who).  Sucks!!

special_string="MODULES-%{version}-%{release}-RPM-ADDITION"

# NOTE: This acts different on different Linux distros.  :-(

# Summation of cases (RH 7.x/8.0/9.x, MDK 8.x/9.x)
# - bash user logs in: /etc/profile is run, all /etc/profile.d/*.sh
#   scripts are run
# - bash user invokes non-interactive shell (e.g., rsh somehost who):
#   $HOME/.bashrc is run, which invokes /etc/bashrc, which manually
#   invokes /etc/profile.d/00-modules.sh (because /etc/profile.d/*.sh
#   scripts are *not* run)
# - user invokes "su" to root: $HOME/.bashrc is run, which invokes
#   /etc/bashrc is run, and all /etc/profile.d/*.sh scripts are run

# Strategy for modifying the top-level bash startup file:

# 1a. If /etc/bashrc exists, use that
# 1b. If not, if /etc/bash.bashrc exists, use that
# 1c. If not, abort
# 2. If "/etc/bash.bashrc.local" exists in the file, use
#    /etc/bash.bashrc.local

# For SuSE 8.1 and 8.2, we can simply put things in
# /etc/bash.bashrc.local and /etc/csh.cshrc.local -- no need to modify
# the system-level files.

bashfile="/etc/bashrc";
if test ! -f $bashfile; then
    bashfile="/etc/bash.bashrc"
    if test ! -f $bashfile; then
	echo "Can't find bash startup file to edit.  :-("
	echo "Aborting in despair!"
	exit 1
    fi
fi	

localfile="/etc/bash.bashrc.local"
grep $localfile $bashfile 2>&1 > /dev/null
if test "$?" = "0"; then
    bashfile="$localfile"
fi

if test -f "$bashfile"; then
    %__cp $bashfile $bashfile.rpmsave
fi
%__cat >> $bashfile <<EOF
if test "\$MODULE_OSCAR" = "" -a -f %{_profiledir}/00-modules.sh; then   # $special_string
        . %{_profiledir}/00-modules.sh          # $special_string
fi                                              # $special_string
EOF

# We have to do essentially the same thing for /etc/csh.cshrc for
# essentially the same reasons.  Sucks!!

cshfile="/etc/csh.cshrc";
if test ! -f $cshfile; then
    echo "Can't find csh startup file to edit.  :-("
    echo "Aborting in despair!"
    exit 1
fi

localfile="/etc/csh.cshrc.local"
grep $localfile $cshfile 2>&1 > /dev/null
if test "$?" = "0"; then
    cshfile="$localfile"
fi

if test -f "$cshfile"; then
    %__cp $cshfile $cshfile.rpmsave
fi
%__cat >> $cshfile <<EOF
if ("\$?MODULE_OSCAR" == "0" && -f %{_profiledir}/00-modules.csh) then   # $special_string
        source %{_profiledir}/00-modules.csh    # $special_string
endif                                           # $special_string
EOF

unset special_string

# Save the names of the files that we altered so that %postun can know
# what files to clean.

rm -f %{_modrpmfilelist}
cat > %{_modrpmfilelist} <<EOF
$bashfile
$cshfile
EOF


#############################################################################
#
# Postun Section
#
#############################################################################
%postun

special_string="MODULES-%{version}-%{release}-RPM-ADDITION"

# Look for the listing of the files that we modified

if test -f %{_modrpmfilelist}; then
    for file in `cat %{_modrpmfilelist}`; do
	egrep -v '^.*# '$special_string $file > $file.tmp
	%__cp $file.tmp $file
	%__rm -f $file.tmp
    done
    rm -f /%{_moddir}/%{main_version}/share/rpmfiles
fi

%endif

#############################################################################
#
# Clean Section
#
#############################################################################
%clean

#############################################################################
#
# Files Section
#
#############################################################################
%files

%defattr(-,root,root)
%doc doc/Modules-Paper.pdf ChangeLog INSTALL INSTALL.RH7x LICENSE.GPL 
%doc README README.OSCAR AUTHORS.OSCAR LICENSE.OSCAR
/%{_moddir}
%{_profiledir}/00-modules.*


#############################################################################
#
# ChangeLog
#
#############################################################################
%changelog
* Fri Apr 24 2015 Olivier Lahaye <olivier.lahaye@cea.fr> 3.3.a-3
- Updated Requires: use package names instead of commands path in
  order to avoid upgrade issues.

* Wed Mar 12 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 3.3.a-2
- Bugfix release.

* Tue Mar 11 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 3.3.a-1
- New upstream version.

* Sun Dec 15 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 3.2.9c-4
- Re-enabled automatic dependancy generator.

* Fri Nov 23 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 3.2.9c-3
- Clean buildroot in prep so install stage doesn't fail if a previous build had occured.

* Fri Nov 23 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 3.2.9c-2
- Fixed module version path (does not include the "c")
- Removes %post* on modern redhat distro (unneeded)

* Thu Jun 14 2012 Olivier Lahaye <olivier.lahaye@cea.fr> 3.2.9c-1
- Upgrade module from 3.2.5 to 3.2.9c
- Use DESTDIR in make install

* Thu Oct 11 2007 DongInn Kim <dikim@osl.iu.edu>
- Upgrade module from 3.1.6 to 3.2.5

* Sun Mar 04 2007 Bernard Li <bernard@vanhpc.org>
- Added tcl-devel to BuildRequires

* Sun Jul 20 2003 Jeff Squyres <jsquyres@lam-mpi.org>
- Updates for SuSE 8.1 and 8.2; be a little smarter about bash and csh
  startup files

* Wed Jul 16 2003  21:02:42PM    Thomas Naughton  <naughtont@ornl.gov>
- Release 3.1.6-3
- Mods to work with new RPM 4.2 issues:
   o defined debug_package=nil to override the default Debuginfo which
     caused a bomb
   o defined __check_files=nil to override default as suggested for
     legacy apps, see also '/usr/lib/rpm/macros'
   o if encounter section name twice (even in comments) freaks out so
     I added a space BTW "% install" and "% prep" in a few places.

* Sun Oct 27 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added AUTHORS.OSCAR and LICENSE.OSCAR files to %doc
- Added IU copyrights to all the RPM-packaging and OSCAR-izations in this
  RPM
- Clarified in README what exactly is copyrighted/licensed by IU --
  everything else is copyrighted/licensed by the base modules package.

* Thu Aug 08 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Change the name of the RPM to modules-oscar so that we don't
  conflict with the Linux modules RPM.
- Remove "oscar" from the release number, per new OSCAR standards.

* Sat Apr 27 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Use %__ kinds of builtin macros for common unix utilities

* Fri Apr 26 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added "Requires" lines for bunches of unix utilities

* Sat Apr 13 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Fixed OSCAR bug 543153 by overriding RPM's auto-dependency analysis
  and making this RPM only depend on "tcl" on the rationale that "the
  friend of my friend is my friend" -- since tcl will have its own
  dependencies, we'll implicitly depend on those as well.  This was
  done to make this RPM installable on multiple different flavors of
  Linux (e.g., RH 7.2 has a different version of TCL than Mandrake
  8.2).
- Fixed OSCAR bug 543155 where the SRPM wouldn't build because some
  source files were missing.  Made all the additional files into a
  real tarball, so the SRPM contains two tarballs.  Added additional
  logic in "% prep" to make this all work properly.
- Ensure to save %{_profiledir}/00-modules.* if they already exist
  while building this RPM.
- Cleaned up the OSCAR.README file; added more information about
  exactly what this RPM does.
- Removed all references to $RPM_BUILD_ROOT because the modules
  package doesn't support DESTDIR kinds of installs, so
  $RPM_BUILD_ROOT was always "/" anyway.  :-(
- Propogate the RPM architeture opt flags to the compile properly.
- Bumped the release number up to "2oscar"

* Tue Apr  9 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added hacks for /etc/csh.cshrc analogous to what we do for
  /etc/bashrc :-(
- Make /opt/modules/oscar-modulefiles for OSCAR-specific modules
- Install oscar module in oscar-modulefiles

* Sun Feb 17 2002 Jeff Squyres <jsquyres@lam-mpi.org>
- Added use of /%{_moddir} just in case we ever move the location of
  where modules are installed
- Moved four scripts into separate files and added additional SourceN
  lines in the preamble
- Added documentation files
- Merged all the shell initialization files into one file per shell
  and put it into /etc/profile.d

* Sat Feb 16 2002 Brian William Barrett <brbarret@osl.iu.edu>
- Initial try at a SPEC file for modules.
