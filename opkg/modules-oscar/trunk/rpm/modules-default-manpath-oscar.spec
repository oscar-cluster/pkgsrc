#
# Copyright (c) 2005 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the modules-default-manpath-oscar software
# package.  For license information, see the LICENSE file in the
# top-level directory of the modules-default-manpath-oscar source
# distribution.
#
# $Id: modules-default-manpath-oscar.spec,v 1.1 2005/02/28 23:15:33 jsquyres Exp $
#

#############################################################################
#
# Helpful Defines
#
#############################################################################

%define _moddir /opt/modules/oscar-modulefiles/default-manpath

# Added to get around RPM 4.2 debugging additions (starting in RH 8.0)
%define debug_package %{nil}
%define __check_files %{nil}


#############################################################################
#
# Preamble Section
#
#############################################################################

Summary: Modules default manpath package
Name: modules-default-manpath-oscar
Version: 1.2.1
Release: 1
License: BSD
Group: Applications/Environment
Source0: modules-default-manpath-oscar-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Packager: Open Cluster Group / OSCAR working group
Requires: modules-oscar
Requires: %__cp %__mkdir %__chmod %__cat

%description 
A "feature" of the man command in Linux is that whenever you add
something to MANPATH, it stops looking in the /etc/man.config file to
find the default paths where to look for man pages.  This RPM contains
a modulefile that will be loaded by default on OSCAR clusters that
manually reads all the MANPATH entries from /etc/man.config and adds
them to the MANPATH environment variable.


#############################################################################
#
# Prep Section
#
#############################################################################
%prep
%setup -q -n modules-default-manpath-oscar-%{version}

# Otherwise, this directory shows up on security reports

chmod -R o-w $RPM_BUILD_DIR/modules-default-manpath-oscar-%{version}


#############################################################################
#
# Build Section
#
#############################################################################


#############################################################################
#
# Install Section
#
#############################################################################
%install

# Nothing to build -- just some files to install

# Install the "default-manpath" modulefile and set its default version

env | sort | egrep -i 'RPM|root'
destdir="$RPM_BUILD_ROOT/%{_moddir}"
%__mkdir_p "$destdir"
%__chmod 0755 "$destdir"

# manpath -q option supported only since V2.5.2.
# if -q option not supported, do not use it.
#if ! manpath -q >/dev/null 2>&1
#then
#    sed -i -e 's/manpath -q/manpath/g' $srcdir/src/default-manpath.tcl
#fi

srcdir="$RPM_BUILD_DIR/modules-default-manpath-oscar-%{version}"
%__cp "$srcdir/src/default-manpath.tcl" "$destdir/%{version}"
%__cat > "$destdir/.version" << EOF
#%Module
set ModulesVersion %{version}
EOF
unset destdir


#############################################################################
#
# Files Section
#
#############################################################################
%files

%defattr(-,root,root)
%doc README.OSCAR AUTHORS.OSCAR LICENSE.OSCAR
%{_moddir}


#############################################################################
#
# ChangeLog
#
#############################################################################
%changelog
* Fri Jun  6 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 1.2.1-1
- New version. (fixes manpath -q on old distros like rhel6)

* Mon Mar 10 2014 Olivier Lahaye <olivier.lahaye@cea.fr> 1.1.1-1
- New version.

* Sun Dec 15 2013 Olivier Lahaye <olivier.lahaye@cea.fr> 1.0.1-2
- Re-enabled automatic dependancy generator.

* Mon Mar 14 2005 Jeff Squyres <jsquyres@lam-mpi.org> 1.0.1-1
- Only examine /etc/man.config if it exists

* Mon Feb 28 2005 Jeff Squyres <jsquyres@lam-mpi.org> 1.0.0-1
- First version
