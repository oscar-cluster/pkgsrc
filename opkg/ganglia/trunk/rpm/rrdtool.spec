# $Id: rrdtool.spec 3101 2005-04-04 20:13:17Z dag $
# Authority: matthias
# Upstream: Tobi Oetiker <oetiker$ee,ethz,ch>

%define perl_vendorarch %(eval "`%{__perl} -V:installvendorarch`"; echo $installvendorarch)
%define perl_vendorlib %(eval "`%{__perl} -V:installvendorlib`"; echo $installvendorlib)
#define python_sitearch %(%{__python} -c 'from distutils import sysconfig; print sysconfig.get_python_lib(1)')
#define python_version %(%{__python} -c 'import string, sys; print string.split(sys.version, " ")[0]')
%define ruby_sitearch %(/usr/bin/ruby -rrbconfig -e "puts Config::CONFIG['sitearchdir']")
%define ruby_archdir %(/usr/bin/ruby -rrbconfig -e "puts Config::CONFIG['archdir']")

%if 0%{?el4}0%{?el5}
%define python_pkg_name python2.6
%else
%define python_pkg_name python
%endif

Summary: Round Robin Database Tool to store and display time-series data
Name: rrdtool
Version: 1.4.7
Release: 1%{?dist}
License: GPL
Group: Applications/Databases
URL: http://oss.oetiker.ch/rrdtool/
Source: http://oss.oetiker.ch/rrdtool/pub/rrdtool-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: freetype-devel
BuildRequires: gcc-c++
BuildRequires: gettext-devel
BuildRequires: intltool
BuildRequires: libpng-devel
BuildRequires: libxml2-devel
BuildRequires: openssl-devel
BuildRequires: %{python_pkg_name}-devel >= 2.6
BuildRequires: ruby
BuildRequires: ruby-devel
BuildRequires: tcl-devel
BuildRequires: tk-devel
BuildRequires: lua-devel
BuildRequires: zlib-devel
Requires: gettext
Requires: libxml2
Requires: openssl
Requires: perl >= %(rpm -q --qf '%%{epoch}:%%{version}' perl)
Requires: python
Requires: ruby
Requires: xorg-x11-fonts-Type1
Requires: zlib

%if 0%{?el4}
BuildRequires: evolution28-cairo-devel
BuildRequires: evolution28-glib2-devel
BuildRequires: evolution28-pango-devel
Requires: evolution28-cairo
Requires: evolution28-glib2
Requires: evolution28-pango
%else
BuildRequires: cairo-devel
BuildRequires: glib2-devel
BuildRequires: pango-devel
BuildRequires: xulrunner-devel
Requires: cairo
Requires: glib2
Requires: pango
%endif

%description
RRD is the Acronym for Round Robin Database. RRD is a system to store and
display time-series data (i.e. network bandwidth, machine-room temperature,
server load average). It stores the data in a very compact way that will not
expand over time, and it presents useful graphs by processing the data to
enforce a certain data density. It can be used either via simple wrapper
scripts (from shell or Perl) or via frontends that poll network devices and
put a friendly user interface on it.

%package devel
Summary: RRDtool static libraries and header files
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
RRD is the Acronym for Round Robin Database. RRD is a system to store and
display time-series data (i.e. network bandwidth, machine-room temperature,
server load average). This package allow you to use directly this library.

%package -n perl-rrdtool
Summary: Perl RRDtool bindings
Group: Development/Languages
Requires: %{name} = %{version}
Obsoletes: rrdtool-perl <= %{version}-%{release}
Provides: rrdtool-perl = %{version}-%{release}

%description -n perl-rrdtool
The Perl RRDtool bindings

%package -n tcl-rrdtool
Summary: TCL bindings
Group: Development/Languages
Requires: %{name} = %{version}
Obsoletes: rrdtool-tcl <= %{version}-%{release}
Provides: rrdtool-tcl = %{version}-%{release}

%description -n tcl-rrdtool
The TCL RRDtool bindings

%package -n %{python_pkg_name}-rrdtool
Summary: Python RRDtool bindings
Group: Development/Languages
BuildRequires: %{python_pkg_name}
Requires: %{python_pkg_name} >= %{python_version}
Requires: %{name} = %{version}
Obsoletes: rrdtool-python <= %{version}-%{release}
Provides: rrdtool-python = %{version}-%{release}

%description -n %{python_pkg_name}-rrdtool
Python RRDtool bindings.

%package -n ruby-rrdtool
Summary: RRDtool module for Ruby
Group: Development/Languages
Requires: %{name} = %{version}, ruby-devel
Obsoletes: rrdtool-ruby <= %{version}-%{release}
Provides: rrdtool-ruby = %{version}-%{release}

%description -n ruby-rrdtool
The ruby-%{name} package includes a library that implements RRDtool bindings
for the Ruby language.

%package -n lua-rrdtool
Summary: RRDtool module for Lua
Group: Development/Languages
Requires: %{name} = %{version}, ruby-devel
Obsoletes: rrdtool-lua <= %{version}-%{release}
Provides: rrdtool-lua = %{version}-%{release}

%description -n lua-rrdtool
The lua-%{name} package includes a library that implements RRDtool bindings
for the Lua language.

%prep
%if 0%{?el4}
# Filter auto-requires for pango
cat > find-requires-%{name} <<EOT
#!/bin/sh
%{__find_requires} | grep -v 'pango'
exit 0
EOT
chmod 755 find-requires-%{name}
%define __find_requires %{_builddir}/find-requires-%{name}
%define _use_internal_dependency_generator 0
%endif

%setup

%build
export PYTHON=%{__python}
%if 0%{?el4}
export LD_LIBRARY_PATH=/usr/evolution28/%{_lib}
export PKG_CONFIG_PATH=/usr/evolution28/%{_lib}/pkgconfig
export RUBYARCHDIR=%{ruby_sitearch}
export CFLAGS="$(pkg-config --cflags cairo pangocairo pango pangoft2)"
export LDFLAGS="$(pkg-config --libs  cairo pangocairo pango pangoft2)"
%endif

%configure \
    --with-tcllib="%{_libdir}" \
    --with-perl-options='INSTALLDIRS="vendor"' \
    --enable-ruby-site-install

%if 0%{?el4}
%{__make} %{?_smp_mflags}  LDFLAGS="-Wl,-rpath-link /usr/evolution28/%{_lib} -Wl,-rpath /usr/evolution28/%{_lib} $LDFLAGS"
%else
%{__make} %{?_smp_mflags}
%endif

%install
export PYTHON=%{__python}
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"

find %{buildroot} -name .packlist -exec %{__rm} {} \;
%{__rm} -f %{buildroot}%{perl_archlib}/perllocal.pod
%{__rm} -f %{buildroot}%{perl_vendorarch}/ntmake.pl

%clean
%{__rm} -rf %{buildroot}
%if 0%{?el4}
%{__rm} -f %{_builddir}/find-requires-%{name}
%endif

%files
%defattr(-, root, root, 0755)
%doc CHANGES CONTRIBUTORS COPYING COPYRIGHT NEWS README THREADS TODO
%doc examples/
%doc %{_mandir}/man1/*.1*
%{_bindir}/rrdcgi
%{_bindir}/rrdtool
%{_bindir}/rrdupdate
%{_bindir}/rrdcached
%{_libdir}/librrd.so.*
%{_libdir}/librrd_th.so.*
%{_libdir}/librrd.a
%{_libdir}/librrd_th.a
%{_libdir}/pkgconfig/librrd.pc
%{_datadir}/rrdtool/
%{_includedir}/rrd_format.h

%files devel
%defattr(-, root, root, 0755)
%{_includedir}/rrd.h
%{_includedir}/rrd_client.h
%{_libdir}/librrd.so
%{_libdir}/librrd_th.so
%doc %{_mandir}/man3/librrd.3.*
%exclude %{_libdir}/librrd.la
%exclude %{_libdir}/librrd_th.la

%files -n perl-rrdtool
%defattr(-, root, root, 0755)
%doc bindings/perl-shared/MANIFEST bindings/perl-shared/README
%doc %{_mandir}/man3/RRDp.3*
%doc %{_mandir}/man3/RRDs.3*
%{perl_vendorarch}/RRDs.pm
%{perl_vendorarch}/auto/RRDs/*
%{perl_vendorlib}/RRDp.pm

%files -n tcl-rrdtool
%defattr(-, root, root, 0755)
%doc bindings/tcl/README
%{_libdir}/rrdtool/ifOctets.tcl
%{_libdir}/rrdtool/pkgIndex.tcl
%{_libdir}/tclrrd%{version}.so

%files -n %{python_pkg_name}-rrdtool
%defattr(-, root, root, 0755)
%doc bindings/python/ACKNOWLEDGEMENT bindings/python/AUTHORS bindings/python/COPYING bindings/python/README
%{python_sitearch}/rrdtoolmodule.so
%{python_sitearch}/py_rrdtool-0.2.1-py2.6.egg-info

%files -n ruby-rrdtool
%defattr(-, root, root, 0755)
%doc bindings/ruby/CHANGES bindings/ruby/README
%{ruby_sitearch}/RRD.so

%files -n lua-rrdtool
%defattr(-, root, root, 0755)
%doc bindings/lua/README
%{_prefix}/lib/lua/5.1/*

%changelog
* Tue Feb 12 2012 Olivier Lahaye <olivier.lahaye@cea.fr> - 1.4.7-1
- Updated to version 1.4.7.
- Handle distro with python2.6 or python (el5 / el6)

* Fri Oct 29 2010 Olivier Lahaye <olivier.lahaye@cea.fr> - 1.4.4-1
- Updated to version 1.4.4.

* Thu Apr  8 2010 Olivier Lahaye <olivier.lahaye@cea.fr> - 1.4.3-1
- Updated to version 1.4.3.

* Tue Jan  5 2010 Christoph Maser <cmr@financial.com> - 1.4.2-1
- Updated to version 1.4.2.

* Mon Jan  4 2010 Christoph Maser <cmr@financial.com> - 1.3.9-1
- Updated to version 1.3.9.

* Wed Jul 22 2009 Dag Wieers <dag@wieers.com> - 1.3.8-2
- Fixed the Typo in the xorg-x11-fonts-Typo1 requirement :) (Ben)

* Tue Jul 21 2009 Christoph Maser <cmr@financial.com> - 1.3.8-1
- Updated to version 1.3.8.

* Fri May 15 2009 Christoph Maser <cmr@financial.com> - 1.3.7-1
- Update to 1.3.7

* Fri Feb 13 2009 Christoph Maser <cmr@financial.com> - 1.3.6-1
- Update to 1.3.6

* Mon Jan 05 2009 Christoph Maser <cmr@financial.com> - 1.3.5-2
- Remove fc10 conditionals
- Compile against evolution28 version of pango,cairo,glib on el4

* Tue Dec 30 2008 Christoph Maser <cmr@financial.com> - 1.3.5-1
- Update version
- Add BuildRequires: ruby for macro expansion
- Add BuildRequires: gettext-devel
- Add Requires: gettext
- Add fc10 conditionals

* Sun Nov 23 2008 Christoph Maser <cmr@financial.com> - 1.3.4-2
- Removed 1.2.x patches.
- Removed dependencies cgilib.
- Added dependencies pango, cairo.

* Sun Nov 23 2008 Christoph Maser <cmr@financial.com> - 1.3.4-1
- Updated to release 1.3.4.

* Wed Oct 15 2008 Christoph Maser <cmr@financial.com> - 1.2.28-1
- Updated to release 1.2.28.

* Wed Jun 06 2007 Dag Wieers <dag@wieers.com> - 1.2.23-1
- Updated to release 1.2.23.

* Wed May 02 2007 Dag Wieers <dag@wieers.com> - 1.2.21-1
- Updated to release 1.2.21.

* Fri Jan 26 2007 Dag Wieers <dag@wieers.com> - 1.2.18-1
- Updated to release 1.2.18.

* Wed Jul 19 2006 Dag Wieers <dag@wieers.com> - 1.2.15-1
- Updated to release 1.2.15.

* Fri May 05 2006 Dag Wieers <dag@wieers.com> - 1.2.13-1
- Updated to release 1.2.13.

* Mon Dec 19 2005 Dag Wieers <dag@wieers.com> - 1.2.12-1
- Updated to release 1.2.12.

* Wed Jul 27 2005 Dag Wieers <dag@wieers.com> - 1.2.11-1
- Updated to release 1.2.11.
- Fixes for x86_64 and perl/tcl/python bindings.

* Sat Jun 04 2005 Dag Wieers <dag@wieers.com> - 1.2.9-1
- Updated to release 1.2.9.

* Wed May 18 2005 Dag Wieers <dag@wieers.com> - 1.2.8-1
- Updated to release 1.2.8.

* Tue May 10 2005 Dag Wieers <dag@wieers.com> - 1.2.6-1
- Updated to release 1.2.6.

* Sat May 07 2005 Dag Wieers <dag@wieers.com> - 1.2.2-1
- Updated to release 1.2.2.

* Sat May 07 2005 Dag Wieers <dag@wieers.com> - 1.2.1-1
- Updated to release 1.2.1.

* Fri Apr 29 2005 Dag Wieers <dag@wieers.com> - 1.2.0-1
- Updated to release 1.2.0.

* Mon Apr 04 2005 Dag Wieers <dag@wieers.com> - 1.0.49-2
- Fix for the php-rrdtool patch. (Joe Pruett)

* Thu Aug 25 2004 Dag Wieers <dag@wieers.com> - 1.0.49-1
- Updated to release 1.0.49.

* Wed Aug 25 2004 Dag Wieers <dag@wieers.com> - 1.0.48-3
- Fixes for x86_64. (Garrick Staples)

* Fri Jul  2 2004 Matthias Saou <http://freshrpms.net/> 1.0.48-3
- Actually apply the patch for fixing the php module, doh!

* Thu May 27 2004 Matthias Saou <http://freshrpms.net/> 1.0.48-2
- Added php.d config entry to load the module once installed.

* Thu May 13 2004 Dag Wieers <dag@wieers.com> - 1.0.48-1
- Updated to release 1.0.48.

* Tue Apr 06 2004 Dag Wieers <dag@wieers.com> - 1.0.47-1
- Updated to release 1.0.47.

* Thu Mar  4 2004 Matthias Saou <http://freshrpms.net/> 1.0.46-2
- Change the strict dependency on perl to fix problem with the recent
  update.

* Mon Jan  5 2004 Matthias Saou <http://freshrpms.net/> 1.0.46-1
- Update to 1.0.46.
- Use system libpng and zlib instead of bundled ones.
- Added php-rrdtool sub-package for the php4 module.

* Fri Dec  5 2003 Matthias Saou <http://freshrpms.net/> 1.0.45-4
- Added epoch to the perl dependency to work with rpm > 4.2.
- Fixed the %% escaping in the perl dep.

* Mon Nov 17 2003 Matthias Saou <http://freshrpms.net/> 1.0.45-2
- Rebuild for Fedora Core 1.

* Sun Aug  3 2003 Matthias Saou <http://freshrpms.net/>
- Update to 1.0.45.

* Wed Apr 16 2003 Matthias Saou <http://freshrpms.net/>
- Update to 1.0.42.

* Mon Mar 31 2003 Matthias Saou <http://freshrpms.net/>
- Rebuilt for Red Hat Linux 9.

* Wed Mar  5 2003 Matthias Saou <http://freshrpms.net/>
- Added explicit perl version dependency.

* Sun Feb 23 2003 Matthias Saou <http://freshrpms.net/>
- Update to 1.0.41.

* Fri Jan 31 2003 Matthias Saou <http://freshrpms.net/>
- Update to 1.0.40.
- Spec file cleanup.

* Fri Jul 05 2002 Henri Gomez <hgomez@users.sourceforge.net>
- 1.0.39

* Mon Jun 03 2002 Henri Gomez <hgomez@users.sourceforge.net>
- 1.0.38

* Fri Apr 19 2002 Henri Gomez <hgomez@users.sourceforge.net>
- 1.0.37

* Tue Mar 12 2002 Henri Gomez <hgomez@users.sourceforge.net>
- 1.0.34
- rrdtools include zlib 1.1.4 which fix vulnerabilities in 1.1.3

