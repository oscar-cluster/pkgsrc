# I am not an expert in spec files but i try to do the following and it does not
# have to change (let's have a real method): i try to use the generated tarball
# and the Makefile for the creation of the RPM. Therefore, I assume that the
# tarball for oscar-installer is in /usr/src/rpm/SOURCES and i assume we use
# a temporary directory to be able to run 'make' and 'make install'. The 
# temporary is currently hard-coded (/tmp/oscar-installer-sourceroot).
# Typically, we decompress the source tarball, go to the according directory and
# run a set of commands to build and install oscar-install. We install 
# oscar-installer into $RPM_BUILD_ROOT and use this directory to actually create
# the package.
# AND PLEASE, DO NOT MODIFY THIS SPEC FILE IN ORDER TO NOT USE ANY MORE THE 
# MAKEFILE OR THE TARBALL, IF YOU DO THAT, THAT WILL QUICKLY BECOME MESSY!!!

Summary:		A tool to ease the OSCAR installation.
Name:      		oscar-installer
Version:   		6.1.2
Release:   		1
Vendor:			Open Cluster Group <http://OSCAR.OpenClusterGroup.org/>
Distribution:		OSCAR
Packager:		Geoffroy Vallee <valleegr@ornl.gov>
License: 		GPL
Group:     		Development/Libraries
Source:			%{name}-%{version}.tar.gz
BuildRoot: 		/usr/src/redhat/BUILD/%{name}-%{version}
#BuildRoot:		/var/tmp/%{name}-buildroot
BuildArch:		noarch
Requires:		perl-AppConfig

%description
oscar-installer is a tool that installs OSCAR in a transparent manner. Two 
different modes are available: online (the current machine has internet access 
and while be used as the OSCAR headnode), and offline (the current machine is 
only used to download OSCAR, OSCAR will be installed on another machine).
By default, the online mode is used (i.e., if no option is specified, the online
mode is used).
The identifier of the target linux distribution for the headnode must be 
specified. In order to get the list of supported Linux distributions, just type
"oscar-installer".

%prep
%setup -q

%build
%__perl Makefile.PL INSTALLDIRS=vendor # INSTALLDIRS=vendor tells perl that we are in a package
%__make manifest

%__make
%__rm -rf $RPM_BUILD_ROOT
%__make install SITEPREFIX=/usr DESTDIR=$RPM_BUILD_ROOT

# For some reasons weird files are installed. We remove them.
%__rm -f $RPM_BUILD_ROOT/usr/lib64/perl5/perllocal.pod
%__rm -f $RPM_BUILD_ROOT/usr/lib64/perl5/vendor_perl/auto/oscar-installer/.packlist

%files 
%defattr(-,root,root)
%{_bindir}/oscar-installer
%{_mandir}/man1/oscar-installer.1
%{perl_vendorlib}/OSCARInstaller/ConfigManager.pm
%{perl_vendorlib}/OSCARInstaller/Installer.pm
%{_sysconfdir}/oscar-installer/oscar-installer.conf


%changelog
* Wed Jul 08 2012 Olivier Lahaye <olivier.lahaye@cea.fr>
- v6.1.2, new upstream version.
- use setup macro to prepare build env
- use INSTALLDIRS=vendor to build the Makefile
- use of macros when it is possible
* Sun Mar 17 2008 Geoffroy Vallee <valleegr@ornl.gov>
- v5.0, new upstream version.
