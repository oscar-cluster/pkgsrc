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

%define binpref /usr/lib/perl5/site_perl
%define manpref /usr/share/man/man3
%define bintarget $RPM_BUILD_ROOT%{binpref}
%define mantarget $RPM_BUILD_ROOT%{manpref}

Summary:		A tool to ease the OSCAR installation.
Name:      		oscar-installer
Version:   		5.0
Release:   		1
Vendor:			Open Cluster Group <http://OSCAR.OpenClusterGroup.org/>
Distribution:	OSCAR
Packager:		Geoffroy Vallee <valleegr@ornl.gov>
License: 		GPL
Group:     		Development/Libraries
Source:			%{name}-%{version}.tar.gz
BuildRoot: 		/usr/src/rpm/BUILD/%{name}-%{version}
#BuildRoot:      /var/tmp/%{name}-buildroot
BuildArch:		noarch

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
%__rm -rf /tmp/oscar-installer-sourceroot
mkdir -p /tmp/oscar-installer-sourceroot
cd /tmp/oscar-installer-sourceroot
%__tar -xzf $RPM_SOURCE_DIR/%{name}-%{version}.tar.gz
cd /tmp/oscar-installer-sourceroot/%{name}-%{version}
perl Makefile.PL
make manifest

make
%__rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
/usr/local/bin/oscar-installer
/usr/local/lib/perl/5.8.8/auto/oscar-installer/.packlist
/usr/local/lib/perl/5.8.8/perllocal.pod
/usr/local/man/man1/oscar-installer.1
/usr/local/share/perl/5.8.8/OSCARInstaller/ConfigManager.pm
/usr/local/share/perl/5.8.8/OSCARInstaller/Installer.pm
/etc/oscar-installer/oscar-installer.conf


%changelog
* Sun Mar 17 2008 Geoffroy Vallee <valleegr@ornl.gov>
- v5.0, new upstream version.
