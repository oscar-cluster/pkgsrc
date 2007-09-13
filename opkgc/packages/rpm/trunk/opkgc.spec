%{!?python_sitelib: %define python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()")}

Name:           opkgc
Version:        0.3.1
Release:        1
Summary:        Compiler for OSCAR package

Group:          Development/Languages
License:        GPL
URL:            http://oscar.openclustergroup.org/comp_opkgc
Source0:        opkgc-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildArch:      noarch
BuildRequires:  python-devel, xmlto
Requires: 	libxslt, python-lxml, python-cheetah

%description
opkgc transform the description of an OSCAR package into a set of native packages (.deb or RPM).
It includes the opkg-convert tool to convert OSCAR packages from old form to current form.

%prep
%setup -q


%build
./configure --prefix=/usr
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
 
%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc 
%{python_sitelib}/*
%{_bindir}/opkgc
%{_bindir}/opkg-convert
%{_datadir}/%{name}/
%{_defaultdocdir}/%{name}/
%{_mandir}/man1/opkgc.1.gz
%{_mandir}/man5/opkg.5.gz
%config %{_sysconfdir}/opkgc.conf

%changelog
* Mon Sept 13 2007 Jean Parpaillon <jean.parpaillon@irisa.fr> 0.3.2-1
- Update from upstream (0.3.2)
- Fix dependency (libxslt that provides xsltproc, needed by opkg-convert)
* Mon Aug 6 2007 Jean Parpaillon <jean.parpaillon@irisa.fr> 0.3.1-1
- Update from upstream (0.3.1)
* Wed Jul 18 2007 Jean Parpaillon <jean.parpaillon@irisa.fr> 0.3-1
- Update from upstream (0.3)
* Wed Jun 27 2007 Jean Parpaillon <jean.parpaillon@irisa.fr> 0.2.1-1
- First RPM release
