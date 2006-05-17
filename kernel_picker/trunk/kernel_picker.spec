# -*- rpm-spec -*-
#
# Copyright (c) 2003 National Center for Supercomputing Applications (NCSA)
#                    All rights reserved.
#
# This file is part of the kernel_picker software package. 
# Version: $Id: kernel_picker.spec,v 1.5 2003/11/04 22:16:50 tfleury Exp $
#
                                                                                
########################################################################
# Preamble Section
########################################################################
Summary: A Perl script to substitute a kernel into an OSCAR (SIS) image.
Name: kernel_picker
Version: 1.4.1
Release: 3
License: GPL
Group: Applications/System
Source: kernel_picker-%{version}.tar.gz
URL: http://oscar.openclustergroup.org
Vendor: NCSA
Packager: Terry Fleury <tfleury@ncsa.uiuc.edu>
Autoreq: 0
Requires: perl >= 5.5, systemconfigurator >= 2.0
Provides: kernel_picker
buildarch: noarch

%description
kernel_picker allows you to substitute a given kernel into your OSCAR (SIS)
image prior to building your nodes.  If executed with no command line
options, you will be prompted for all required information.  You can also
specify command line options for (mostly) non-interactive execution.  Any
necessary information that you do not give via an option will cause the
program to prompt you for that information.

########################################################################
# Prep Section
########################################################################
%prep
%setup

########################################################################
# Install Section
########################################################################
%install
rm -rf /opt/kernel_picker
install -d 755 /opt/kernel_picker/bin /opt/kernel_picker/doc /opt/kernel_picker/html /opt/kernel_picker/man/man1 /opt/modules/oscar-modulefiles/kernel_picker
install -m 755 kernel_picker.pl   /opt/kernel_picker/bin/kernel_picker
install -m 644 kernel_picker.txt  /opt/kernel_picker/doc/kernel_picker.txt
install -m 644 kernel_picker.tex  /opt/kernel_picker/doc/kernel_picker.tex
install -m 644 kernel_picker.ps   /opt/kernel_picker/doc/kernel_picker.ps
install -m 644 kernel_picker.pdf  /opt/kernel_picker/doc/kernel_picker.pdf
install -m 644 kernel_picker.html /opt/kernel_picker/html/kernel_picker.html
install -m 644 kernel_picker.1    /opt/kernel_picker/man/man1/kernel_picker.1
install -m 644 %{version}.%{release} /opt/modules/oscar-modulefiles/kernel_picker/%{version}.%{release}

%files 
%dir /opt/kernel_picker
%dir /opt/kernel_picker/bin
%dir /opt/kernel_picker/doc
%dir /opt/kernel_picker/html
%dir /opt/kernel_picker/man
%dir /opt/kernel_picker/man/man1
%dir /opt/modules/oscar-modulefiles/kernel_picker
/opt/kernel_picker/bin/kernel_picker
/opt/kernel_picker/doc/kernel_picker.txt
/opt/kernel_picker/doc/kernel_picker.tex
/opt/kernel_picker/doc/kernel_picker.ps
/opt/kernel_picker/doc/kernel_picker.pdf
/opt/kernel_picker/html/kernel_picker.html
/opt/kernel_picker/man/man1/kernel_picker.1
/opt/modules/oscar-modulefiles/kernel_picker/%{version}.%{release}

