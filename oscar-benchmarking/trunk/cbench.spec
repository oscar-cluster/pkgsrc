
%define debug_packages	%{nil}
%define debug_package %{nil}


Summary: The Cbench infrastructure
Name: Cbench
Version: 1.1.5
Release: 1
Group: Development
License: GPL
Source0:cbench_hpcc.tar.gz
Source1:hpcc-1.0.0.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

Provides: Cbench
Autoreqprov: no
Requires: perl python

%description
The cbench infrastructure.

%package hpcc
Summary: The HPCC benchmark suite
Version: 1.0.0
Release: 1
Group: Development
License: GPL
BuildRoot: /var/tmp/%{name}-buildroot

Provides: hpcc
Autoreqprov: no
Requires: Cbench blas libaio libaio-devel

%description -n Cbench-hpcc
The hpcc benchmark suite

%prep
%setup -n cbench -T -b 0 -n cbench/opensource -a 1 -q
mkdir $RPM_BUILD_ROOT


%build
cd $CBENCHOME
make -C opensource/hpcc distclean
make -C opensource/hpcc 
make -C opensource/hpcc install
cd $CBENCHOME

%install
cd $CBENCHOME
make itests_hpcc
mkdir -p $RPM_BUILD_ROOT%{_builddir}/cbench
cp -r $CBENCHTEST $RPM_BUILD_ROOT%{_builddir}/cbench


%clean
rm -rf $CBENCHOME
rm -rf $RPM_BUILD_ROOT


%files
#%defattr(-,root,root)
%dir %{_builddir}/cbench
%dir %{_builddir}/cbench/testset_cbench
%dir %{_builddir}/cbench/testset_cbench/bin
%{_builddir}/cbench/testset_cbench/cbench_functions
%{_builddir}/cbench/testset_cbench/cbench-init.csh
%{_builddir}/cbench/testset_cbench/cbench-init.sh
%{_builddir}/cbench/testset_cbench/cbench.pl
%{_builddir}/cbench/testset_cbench/cluster.def
%{_builddir}/cbench/testset_cbench/common_footer.in
%{_builddir}/cbench/testset_cbench/common_header.in
%{_builddir}/cbench/testset_cbench/gen_jobs.pl
%{_builddir}/cbench/testset_cbench/interactive_header.in
%dir %{_builddir}/cbench/testset_cbench/perllib
%dir %{_builddir}/cbench/testset_cbench/perllib/hw_test
%dir %{_builddir}/cbench/testset_cbench/perllib/output_parse
%{_builddir}/cbench/testset_cbench/perllib/parse_filter
%{_builddir}/cbench/testset_cbench/perllib/Statistics
%{_builddir}/cbench/testset_cbench/sbin
%{_builddir}/cbench/testset_cbench/start_jobs.pl
%{_builddir}/cbench/testset_cbench/tools
%{_builddir}/cbench/testset_cbench/torque_header.in


%files hpcc
%{_builddir}/cbench/testset_cbench/bin/hpcc
%{_builddir}/cbench/testset_cbench/hpcc
%{_builddir}/cbench/testset_cbench/perllib/hw_test/hpcc.pm
%{_builddir}/cbench/testset_cbench/perllib/output_parse/hpcc.pm

