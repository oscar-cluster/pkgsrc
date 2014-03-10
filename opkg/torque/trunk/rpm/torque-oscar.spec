# comment out snap if building a real release
%define name torque-oscar
%define version 4.1.7

%define release 4

# The following options are supported:
#   --with server_name=hostname
#   --with homedir=directory
#   --with libdir=directory
#   --with includedir=directory
#   --with prefix=directory
#   --with[out] scp
#   --with[out] syslog
#   --with[out] rpp
#   --with[out] drmaa
#   --with[out] blcr
#   --with[out] nvidia_gpus
#   --with[out] use_munge
#   --with[out] gui
#   --with[out] tcl
# Note that prefix overrides homedir, libdir, and includedir


# Hrm, should we default to the name of the buildhost?  That seems only
# slightly better than picking a hostname at random.  This is exactly the kind
# of compile-time default that doesn't work well with distributable packages.
# Let's force the issue with the non-sensical "localhost".
#
# Note that "localhost" doesn't actually work.  You must either define the
# correct hostname here, pass '--with server_name=foo' to rpmbuild, or be sure
# that $PBS_SERVER_HOME/server_name contains the correct hostname.
# OSCAR name set below %define server_name localhost

# change as you wish
%define use_syslog 1
%define use_scp 1
%define use_rpp 1
%define use_tcl 1
%define use_drmaa 1
%define use_blcr 1
%define use_nvidia_gpus 1
%define use_munge 1
%define build_gui 1

# these are non-defaults, but fit better into most RPM-based systems
%global torquehomedir %{_localstatedir}/lib/torque

%define debug_package %{nil}

# Adjustments for OSCAR
#    prefix = /opt/pbs

# Installation directory base prefix
%define torqueprefix  /opt/pbs
%define torquebindir  %{torqueprefix}/bin
%define torquesbindir %{torqueprefix}/sbin
%define torquelibdir  %{torqueprefix}/%{_lib}
%define torquemandir  %{torqueprefix}/man
%define torqueincdir  %{torqueprefix}/include

# Name of file that contains the default server for clients to use
%define server_name_file server_name
# Default server name
%define server_name pbs_oscar

# where to install initscripts
%if ! %{?_initrddir:1}0
%define _initrddir /etc/init.d
%endif

# --with/--without processing
# first, error if conflicting options are used
%{?_with_syslog: %{?_without_syslog: %{error: both _with_syslog and _without_syslog}}}
%{?_with_rpp: %{?_without_rpp: %{error: both _with_rpp and _without_rpp}}}
%{?_with_scp: %{?_without_scp: %{error: both _with_scp and _without_scp}}}
%{?_with_tcl: %{?_without_tcl: %{error: both _with_tcl and _without_tcl}}}
%{?_with_drmaa: %{?_without_drmaa: %{error: both _with_drmaa and _without_drmaa}}}
%{?_with_blcr: %{?_without_blcr: %{error: both _with_blcr and _without_blcr}}}
%{?_with_nvidia_gpus: %{?_without_nvidia_gpus: %{error: both _with_nvidia_gpus and _without_nvidia_gpus}}}
%{?_with_munge: %{?_without_munge: %{error: both _with_munge and _without_munge}}}
%{?_with_gui: %{?_without_gui: %{error: both _with_gui and _without_gui}}}

# did we find any --with options?
%{?_with_syslog: %define use_syslog 1}
%{?_with_rpp: %define use_rpp 1}
%{?_with_scp: %define use_scp 1}
%{?_with_tcl: %define use_tcl 1}
%{?_with_drmaa: %define use_drmaa 1}
%{?_with_blcr: %define use_blcr 1}
%{?_with_nvidia_gpus: %define use_nvidia_gpus 1}
%{?_with_munge: %define use_munge 1}
%{?_with_gui: %define %define build_gui 1}

%{?_with_server_name:%define server_name %(set -- %{_with_server_name}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_homedir:%define torquehomedir %(set -- %{_with_homedir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_libdir:%define _libdir %(set -- %{_with_libdir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_includedir:%define _includedir %(set -- %{_with_includedir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_prefix:%define torqueprefix %(set -- %{_with_prefix}; echo $1 | grep -v with | sed 's/=//')}

# did we find any --without options?
%{?_without_syslog: %define use_syslog 0}
%{?_without_rpp: %define use_rpp 0}
%{?_without_scp: %define use_scp 0}
%{?_without_tcl: %define use_tcl 0}
%{?_without_drmaa: %define use_drmaa 0}
%{?_without_blcr: %define use_blcr 0}
%{?_without_nvidia_gpus: %define use_nvidia_gpus 0}
%{?_without_munge: %define use_munge 0}
%{?_without_gui: %define build_gui 0}

# Set up all options as disabled
%define syslogflags --disable-syslog
%define rppflags    --disable-rpp
%define scpflags    %{nil}
%define tclflags    --without-tcl
%define drmaaflags  --disable-drmaa
%define blcrflags   --disable-blcr
%define nvidiagpusflags --disable-nvidia-gpus
%define mungeflags  --disable-munge-auth
%define guiflags    --disable-gui

# Enable options that we want
%if %use_syslog
%define syslogflags --enable-syslog
%endif
%if %use_rpp
%define rppflags    --enable-rpp
%endif
%if %use_scp
%define scpflags    --with-rcp=scp
%endif

%if %use_drmaa
%define drmaaflags --enable-drmaa
%endif
%if %use_blcr
%define blcrflags --enable-blcr
BuildRequires: blcr-devel
%endif
%if %use_nvidia_gpus
%define nvidiagpusflags --enable-nvidia-gpus
%endif
%if use_munge
%define mungeflags  --enable-munge-auth
BuildRequires: munge-devel
%endif

# dealing with tcl and gui is way too complicated
%if %build_gui
%define guiflags   --enable-gui
%define use_tcl 1
%endif

%if %use_tcl
%if %build_gui
%define tclflags    --with-tcl
%else
%define tclflags    --with-tcl
%endif
%endif

# finish up the configs...
%define server_nameflags --with-default-server=%{server_name} --with-server-name-file=%{server_name_file}


%define shared_description %(echo -e "TORQUE (Tera-scale Open-source Resource and QUEue manager) is a resource \\nmanager providing control over batch jobs and distributed compute nodes.  \\nTorque is based on OpenPBS version 2.3.12 and incorporates scalability, \\nfault tolerance, and feature extension patches provided by USC, NCSA, OSC, \\nthe U.S. Dept of Energy, Sandia, PNNL, U of Buffalo, TeraGrid, and many \\nother leading edge HPC organizations.\\n\\nThis build was configured with:\\n  %{syslogflags}\\n  %{tclflags}\\n  %{rppflags}\\n  %{server_nameflags}\\n  %{guiflags}\\n  %{scpflags}\\n")

Summary: Tera-scale Open-source Resource and QUEue manager
Name: %{name}
Version: %{version}
Release: %{?snap:snap.%snap.}%{release}
Source0: torque-%{version}%{?snap:-snap.%snap}.tar.gz
%define untarring_directory %{name}-%{version}
Source1: torque-modulefile
Source2: mom_config
Source3: xpbs.desktop
Source4: xpbsmon.desktop
Source5: xpbs.png
Source6: xpbsmon.png
Source7: README.QuickStart
License: OpenPBS and TORQUEv1.1
Group: System Environment/Daemons
URL: http://www.clusterresources.com/products/torque/
BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot
Obsoletes: torque < 4.1.0
Provides: pbs = %{version}-%{release}
Provides: torque = %{version}-%{release}
BuildRequires:  desktop-file-utils
BuildRequires:  ed
BuildRequires:  bison
BuildRequires:  flex
BuildRequires:  groff
BuildRequires:  sed
BuildRequires:  xauth
BuildRequires:  gperf
BuildRequires:  glibc-devel
BuildRequires:  binutils-devel
BuildRequires:  ncurses-devel
BuildRequires:  readline-devel
%if %{use_tcl}
BuildRequires:  tcl-devel
%endif
%if %{build_gui}
BuildRequires:  tk-devel
BuildRequires:  tclx-devel
%endif
BuildRequires:  openssh-clients
BuildRequires:  readline-devel
BuildRequires:  gcc-gfortran
BuildRequires:  gcc-c++
BuildRequires:  pam-devel
BuildRequires:  openssl-devel
BuildRequires:  libxml2-devel
BuildRequires:  hwloc
BuildRequires:  hwloc-devel

#doxygen appears to be broken in rawhide at the moment
#Dec 9th 2010 so don't build the drmaa documentation
#for now.
%if ! 0%{?fc15}
%global doxydoc 1
%endif

%if 0%{?doxydoc}
BuildRequires:  graphviz
BuildRequires:  doxygen
%if "%{?rhel}" == "5"
BuildRequires: graphviz-gd
%endif
%if %{?fedora}%{!?fedora:0} >= 9
BuildRequires:  tex(latex)
%else
%if %{?rhel}%{!?rhel:0} >= 6
BuildRequires:  tex(latex)
%else
BuildRequires:  tetex-latex
%endif
%endif
%endif


Conflicts: pbspro, openpbs, openpbs-oscar
Obsoletes: scatorque
#AutoReqProv: no

%if ! %build_gui
Obsoletes: torque-oscar-gui
%endif

# add LSB info + fixes various bugs in pbs_server
Patch0:         torque-4.1.1-initdserver.patch
Patch1:         torque-4.1.1-initdsched.patch
Patch2:         torque-4.1.1-initdmom.patch
Patch3:         torque-4.1.1-initdtrqauthd.patch
#Patch4:         torque_413_chk_file_sec_linkbug.patch

%description
%shared_description
This package holds just a few shared files and directories.

%prep
%setup -n torque-%{version}%{?snap:-snap.%snap}
#ifarch noarch
#setup -a 1 -n torque-%{version}%{?snap:-snap.%snap}
#endif
%patch0 -p1 -b .old
%patch1 -p1 -b .old
%patch2 -p1 -b .old
%patch3 -p1 -b .old
#patch4 -p1 -b .old
install -pm 644 %{SOURCE1} \
                %{SOURCE2} \
                %{SOURCE3} \
                %{SOURCE4} \
                %{SOURCE5} \
                %{SOURCE6} \
                %{SOURCE7} .
# rm x bit on some documentation.
chmod 644 torque.setup

%build
#ifnarch noarch
./configure --prefix=%{torqueprefix} \
            --mandir=%{torquemandir} \
            --libdir=%{torquelibdir} \
            --includedir=%{torqueincdir} \
            --with-server-home=%{torquehomedir} \
            --with-pam=/%{_lib}/security \
            --with-sendmail=%{_sbindir}/sendmail \
            %{server_nameflags} \
            %{guiflags} \
            %{syslogflags} \
            %{tclflags} \
            %{rppflags} \
            %{scpflags} \
            %{drmaaflags} \
            %{blcrflags} \
            %{nvidiagpusflags} \
            %{mungeflags} \
            CC="$RPMCC" \
            CFLAGS="$RPMCFLAGS" \
            LDFLAGS="$RPMLDFLAGS"

%ifnarch noarch
%{__make} %{?_smp_mflags}
%else
%{__make} buildutils/modulefiles
(cd src/drmaa; %{__make} doc)
%endif
 
%install
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf "$RPM_BUILD_ROOT"
%ifnarch noarch
%{__make} DESTDIR=$RPM_BUILD_ROOT install
# On some distros, a buggy man is generated and installed. Remove this buggy
# man: _home_<user>_rpmbuild_BUILD_torque-4.1.5.1_src_drmaa_src_.3.gz
find $RPM_BUILD_ROOT -name \*_rpmbuild_BUILD_torque-\* -exec rm {} \;
%endif

%ifnarch noarch
if [ -f /etc/SuSE-release ];then
  initpre="suse."
else
  initpre=""
fi

#install starting scripts
%__mkdir_p %{buildroot}%{_initrddir}
install -p -m 755 contrib/init.d/pbs_mom    %{buildroot}%{_initrddir}/pbs_mom
install -p -m 755 contrib/init.d/pbs_sched  %{buildroot}%{_initrddir}/pbs_sched
install -p -m 755 contrib/init.d/pbs_server %{buildroot}%{_initrddir}/pbs_server
install -p -m 755 contrib/init.d/trqauthd   %{buildroot}%{_initrddir}/trqauthd
#end starting scripts

# install ld.so config file
mkdir -p %{buildroot}%{_sysconfdir}/ld.so.conf.d/
cat > %{buildroot}%{_sysconfdir}/ld.so.conf.d/%{name}.conf <<EOF
%{torquelibdir}
EOF
chmod 444 %{buildroot}%{_sysconfdir}/ld.so.conf.d/%{name}.conf
# end install ld.so config file

for initscript in pbs_mom pbs_sched pbs_server; do
  %__sed -e 's|^PBS_HOME=.*|PBS_HOME=%{torquehomedir}|' \
         -e 's|^PBS_DAEMON=.*|PBS_DAEMON=%{torquesbindir}/'$initscript'|' \
        < contrib/init.d/$initpre$initscript > $RPM_BUILD_ROOT%{_initrddir}/$initscript
  %__chmod 755 $RPM_BUILD_ROOT%{_initrddir}/$initscript
done

%if %{build_gui}
# This is really trivial, but cleans up an rpmlint warning
#sed -i -e 's|%{_lib}/../||' %{buildroot}%{_bindir}/xpbs

desktop-file-install --dir %{buildroot}%{_datadir}/applications xpbs.desktop
desktop-file-install --dir %{buildroot}%{_datadir}/applications xpbsmon.desktop
install -d %{buildroot}%{_datadir}/pixmaps
install -p -m0644 xpbs.png xpbsmon.png %{buildroot}%{_datadir}/pixmaps
%endif

# alternatives stuff
for bin in qalter qdel qhold qrls qselect qstat qsub pbsdsh
do
    mv %{buildroot}%{torquebindir}/$bin \
       %{buildroot}%{torquebindir}/${bin}-torque
    mv %{buildroot}%{torquemandir}/man1/${bin}.1 \
       %{buildroot}%{torquemandir}/man1/${bin}-torque.1
done

# remove libtool droppings
%{__rm} -f $RPM_BUILD_ROOT/%{_lib}/security/pam_pbssimpleauth.{a,la}

# recreate pbs_environment.
cat > pbs_environment <<EOF
PATH=%{torquebindir}:%{torquesbindir}:/bin:/usr/bin
LANG=C
LC_ALL=C
EOF

# Relocate configuration files.
mkdir -p %{buildroot}%{_sysconfdir}/torque
pushd %{buildroot}%{torquehomedir}
mv pbs_environment %{buildroot}%{_sysconfdir}/torque
mv server_name %{buildroot}%{_sysconfdir}/torque
ln -s %{_sysconfdir}/torque/pbs_environment .
ln -s %{_sysconfdir}/torque/server_name .
popd

# Relocate mom_logs to /var/log
mkdir -p %{buildroot}%{_var}/log/torque
pushd %{buildroot}%{torquehomedir}
mv mom_logs %{buildroot}%{_var}/log/torque
ln -s %{_var}/log/torque/mom_logs .
popd

# Install mom_priv/config file to /etc/torque/mom
mkdir -p %{buildroot}%{_sysconfdir}/torque/mom
# create mom oscar configfile
#cat > %{buildroot}%{_sysconfdir}/torque/mom/config <<\EOF
#$logevent 127
#$pbsserver pbs_oscar
#$restricted pbs_oscar
#$usecp pbs_oscar:/home /home
#EOF
install -m 644 mom_config %{buildroot}%{_sysconfdir}/torque/mom/config
#chmod 644 %{buildroot}%{_sysconfdir}/torque/mom/config
pushd %{buildroot}%{torquehomedir}/mom_priv
ln -s %{_sysconfdir}/torque/mom/config .
popd

# Install sched_config files to /etc/torque/sched
mkdir -p %{buildroot}%{_sysconfdir}/torque/sched
pushd %{buildroot}%{torquehomedir}/sched_priv
for CONFIG in dedicated_time holidays resource_group sched_config ; do
  mv $CONFIG %{buildroot}%{_sysconfdir}/torque/sched/.
  ln -s %{_sysconfdir}/torque/sched/$CONFIG .
done
popd

# Relocate sched_logs to /var/log
pushd %{buildroot}%{torquehomedir}
mv sched_logs %{buildroot}%{_var}/log/torque
ln -s %{_var}/log/torque/sched_logs .
popd

# Relocate server_logs to /var/log
pushd %{buildroot}%{torquehomedir}
mv server_logs %{buildroot}%{_var}/log/torque
ln -s %{_var}/log/torque/server_logs .
popd

# Move drmaa man pages to correct place 
# and delete the three copies of the same documentation.

%if 0%{?doxydoc}
mv %{buildroot}%{torqueprefix}/share/doc/torque-drmaa/man/man3/* %{buildroot}%{torquemandir}/man3/
rm -rf %{buildroot}%{torqueprefix}/share/doc/torque-drmaa/html/*
rm -rf %{buildroot}%{torqueprefix}/share/doc/torque-drmaa/latex/*
# Compress uncompressed mans
find %{buildroot}%{torquemandir} -name '*.[0-9]' -type f -exec gzip -9 {} \;
# Fix links
find %{buildroot}%{torquemandir} -name '*.[0-9]' -type l -exec /bin/rm -f {} \;
pushd %{buildroot}%{torquemandir}/man7
ln -s pbs_resources_linux.7.gz pbs_resources.7.gz
popd
pushd %{buildroot}%{torquemandir}/man8
ln -s pbs_sched_cc.8.gz pbs_sched.8.gz
popd

# Include drmaa.pdf later from the src tree.
rm %{buildroot}%{torqueprefix}/share/doc/torque-drmaa/drmaa.pdf
%endif

#Remove man page for binary that is not included.
rm %{buildroot}%{torquemandir}/man1/basl2c.1*

%else
# NOARCH stuffs: (modules files and doc (not manuals))
mkdir -p %{buildroot}/opt/modules/oscar-modulefiles/%{name}
cp -p buildutils/modulefiles %{buildroot}/opt/modules/oscar-modulefiles/%{name}/%{version}
#cp -p torque-modulefile %{buildroot}/opt/modules/oscar-modulefiles/%{name}/%{version}
%endif

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf $RPM_BUILD_ROOT

%post
%ifnarch noarch

if %__grep -q "PBS services" /etc/services;then
   : PBS services already installed
else

cat >> /etc/services << EOF
# Standard PBS services
pbs_server      15001/tcp                       # pbs server
pbs_server      15001/udp                       # pbs server
pbs_mom         15002/tcp                       # mom to/from server
pbs_mom         15002/udp                       # mom to/from server
pbs_resmon      15003/tcp                       # mom resource management requests
pbs_resmon      15003/udp                       # mom resource management requests
pbs_sched       15004/tcp                       # scheduler 
pbs_sched       15004/udp                       # scheduler
trqauthd        15005/tcp                       # authd
trqauthd        15005/udp                       # authd
EOF

fi


%files
%defattr(-, root, root)
%doc INSTALL README.torque torque.setup Release_Notes
%doc CHANGELOG PBS_License.txt README.QuickStart
%doc contrib/PBS_License*.txt
%dir %{torquehomedir}
%dir %{torquehomedir}/aux
%dir %{torquehomedir}/spool
%dir %{torquehomedir}/undelivered
%dir %{torquehomedir}/checkpoint
%{torquehomedir}/pbs_environment
%{torquehomedir}/server_name
%config(noreplace) %{_sysconfdir}/torque/pbs_environment
%config(noreplace) %{_sysconfdir}/torque/server_name
%{torquemandir}/man1/pbs.1*
%{_sysconfdir}/ld.so.conf.d/%{name}.conf

%package scheduler
Group: System Environment/Daemons
Summary: Simple fifo scheduler for TORQUE
Requires: %{name}-libs = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes: torque-scheduler < 4.1.0
Provides: pbs-scheduler = %{version}-%{release}
Provides: torque-scheduler = %{version}-%{release}
Requires(post):  chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts

%description scheduler
%shared_description
This package holds the fifo C scheduler.

%files scheduler
%defattr(-, root, root)
%{torquesbindir}/pbs_sched
%{torquesbindir}/qschedd
%{_initrddir}/pbs_sched
%dir %{torquehomedir}/sched_priv
%config(noreplace) %{torquehomedir}/sched_priv/*
%{torquehomedir}/sched_logs
%dir %{_var}/log/torque/sched_logs
%dir %{_sysconfdir}/torque/sched
%{torquemandir}/man8/pbs_sched.8*
%{torquemandir}/man8/pbs_sched_basl.8*
%{torquemandir}/man8/pbs_sched_cc.8*
%{torquemandir}/man8/pbs_sched_tcl.8*
%config(noreplace) %{_sysconfdir}/torque/sched/dedicated_time
%config(noreplace) %{_sysconfdir}/torque/sched/holidays
%config(noreplace) %{_sysconfdir}/torque/sched/resource_group
%config(noreplace) %{_sysconfdir}/torque/sched/sched_config

%post scheduler
/sbin/chkconfig --add pbs_sched

%preun scheduler
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_sched


%package server
Group:             System Environment/Daemons
Summary:           The main part of TORQUE
Requires:          %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:         torque-server < 4.1.0
Provides:          pbs-server = %{version}-%{release}
Provides:          torque-server = %{version}-%{release}
%if ! %{use_scp}
Requires:          openssh-server
%endif
Requires(post):    chkconfig
Requires(preun):   chkconfig
Requires(preun):   initscripts
#AutoReqProv: no

%description server
%shared_description
This package holds the server.

%files server
%defattr(-, root, root)
%attr(0755, root, root) %{torquesbindir}/pbs_server
%attr(0755, root, root) %{torquesbindir}/momctl
%{torquesbindir}/qserverd
%{_initrddir}/pbs_server
%dir %{_var}/log/torque/server_logs
%{torquehomedir}/server_logs
%{torquehomedir}/server_priv
%{torquemandir}/man8/pbs_server.8*

%post server
/sbin/chkconfig --add pbs_server

%preun server
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_server

%if 0%{?use_drmaa}
%package drmaa
Group:             System Environment/Daemons
Summary:           Run time files for the drmaa interface
Requires:          %{name}-libs = %{version}-%{release}
Obsoletes:         torque-drmaa < 4.1.0
Provides:          drmaa
Provides:          torque-drmaa = %{version}-%{release}

%description drmaa
%shared_description
Run time files for working the DRMAA interface to torque.
DRMAA is "Distributed Resource Management Application API"

%files drmaa
%defattr(-, root, root, -)
%{torquelibdir}/libdrmaa.so.*

%post   drmaa -p /sbin/ldconfig
%postun drmaa -p /sbin/ldconfig

%package drmaa-devel
Group:             System Environment/Daemons
Summary:           Development files for the drmaa interface.
Requires:          %{name}-drmaa = %{version}-%{release}
Requires:          %{name}-devel = %{version}-%{release}
Obsoletes:         torque-drmaa-devel < 4.1.0
Provides:          dramm-devel
Provides:          torque-drmaa-devel = %{version}-%{release}

%description drmaa-devel
%shared_description
Developement files for working the DRMAA interface to torque.
DRMAA is "Distributed Resource Management Application API"

%files drmaa-devel
%defattr(-, root, root, -)
%{torquelibdir}/libdrmaa.so
%{torqueincdir}/drmaa.h
%if 0%{?doxydoc}
%{torquemandir}/man3/compat.h.3*
%{torquemandir}/man3/drmaa.3*
%{torquemandir}/man3/drmaa.h.3*
%{torquemandir}/man3/drmaa_attr_names_s.3*
%{torquemandir}/man3/drmaa_attr_values_s.3*
%{torquemandir}/man3/drmaa_attrib.3*
%{torquemandir}/man3/drmaa_attrib_info_s.3*
%{torquemandir}/man3/drmaa_def_attr_s.3*
%{torquemandir}/man3/drmaa_job_ids_s.3*
%{torquemandir}/man3/drmaa_job_iter_s.3*
%{torquemandir}/man3/drmaa_job_s.3*
%{torquemandir}/man3/drmaa_job_template_s.3*
%{torquemandir}/man3/drmaa_jobt.3*
%{torquemandir}/man3/drmaa_session.3*
%{torquemandir}/man3/drmaa_session_s.3*
%{torquemandir}/man3/drmaa_submission_context_s.3*
%{torquemandir}/man3/drmaa_viter.3*
%{torquemandir}/man3/error.h.3*
%{torquemandir}/man3/jobs.3*
%{torquemandir}/man3/jobs.h.3*
%{torquemandir}/man3/lookup3.h.3*
%{torquemandir}/man3/pbs_attrib.3*
%endif

%endif

%package mom
Group:          System Environment/Daemons
Summary:        Node execution daemon for TORQUE
Requires:       %{name}-libs = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:      torque-mom < 4.1.0
Provides:       pbs-mom = %{version}-%{release}
Provides:       torque-mom = %{version}-%{release}
%if ! %{use_scp}
Requires:       openssh-clients
%endif
Requires(post):  chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts


%description mom
%shared_description
This package holds the execute daemon required on every node.
#AutoReqProv: no

%files mom
%defattr(-, root, root)
%{torquesbindir}/pbs_demux
%{torquesbindir}/pbs_mom
%{torquesbindir}/qnoded
%{torquebindir}/pbs_track
%{_initrddir}/pbs_mom
%if ! %{use_scp}
%attr(4755 root root) %{_sbindir}/pbs_rcp
%endif
%dir %{torquehomedir}
%dir %{torquehomedir}/mom_logs
%dir %{torquehomedir}/mom_priv
%dir %{torquehomedir}/undelivered
%dir %{torquehomedir}/checkpoint
%{torquehomedir}/mom_priv/*
%{torquemandir}/man8/pbs_mom.8*
%dir %{_var}/log/torque
%dir %{_var}/log/torque/mom_logs
%dir %{_sysconfdir}/torque/mom
%config(noreplace) %{_sysconfdir}/torque/mom/config

%post mom
/sbin/chkconfig --add pbs_mom

%preun mom
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_mom


%package              client
Group:                Applications/System
Summary:              Client part of Torque
Requires:             %{name}-libs = %{?epoch:%{epoch}:}%{version}-%{release}
Requires(posttrans):  chkconfig
Requires(preun):      chkconfig
Requires:             munge
Obsoletes:            torque-client < 4.1.0
Provides:             pbs-client = %{version}-%{release}
Provides:             torque-client = %{version}-%{release}
#AutoReqProv: no

%description client
%shared_description
This package holds the command-line client programs.

%files client
%defattr(-, root, root)
%{torquebindir}/q*
%{torquebindir}/chk_tree
%{torquebindir}/hostn
%{torquebindir}/nqs2pbs
%{torquebindir}/pbsdsh-torque
%{torquebindir}/pbsnodes
%{torquebindir}/printjob
%{torquebindir}/printserverdb
%{torquebindir}/printtracking
%{torquebindir}/tracejob
%{torquesbindir}/momctl
%{_initrddir}/trqauthd
%attr(4755 root root) %{torquesbindir}/trqauthd
%{torquesbindir}/pbs_demux
%if %{use_tcl}
%{torquebindir}/pbs_tclsh
%endif
%{torquemandir}/man1/nqs2pbs.1*
%{torquemandir}/man1/qchkpt.1*
%{torquemandir}/man1/qmgr.1*
%{torquemandir}/man1/qmove.1*
%{torquemandir}/man1/qmsg.1*
%{torquemandir}/man1/qorder.1*
%{torquemandir}/man1/qrerun.1*
%{torquemandir}/man1/qsig.1*
%{torquemandir}/man1/qgpumode.1*
%{torquemandir}/man1/qgpureset.1*
%{torquemandir}/man8/pbsnodes.8*
%{torquemandir}/man8/qdisable.8*
%{torquemandir}/man8/qenable.8*
%{torquemandir}/man8/qrun.8*
%{torquemandir}/man8/qstart.8*
%{torquemandir}/man8/qstop.8*
%{torquemandir}/man8/qterm.8*
%{torquemandir}/man7/pbs_job_attributes.7*
%{torquemandir}/man7/pbs_queue_attributes.7*
%{torquemandir}/man7/pbs_resources.7*
%{torquemandir}/man7/pbs_resources_aix4.7*
%{torquemandir}/man7/pbs_resources_aix5.7*
%{torquemandir}/man7/pbs_resources_darwin.7*
%{torquemandir}/man7/pbs_resources_digitalunix.7*
%{torquemandir}/man7/pbs_resources_freebsd.7*
%{torquemandir}/man7/pbs_resources_fujitsu.7*
%{torquemandir}/man7/pbs_resources_hpux10.7*
%{torquemandir}/man7/pbs_resources_hpux11.7*
%{torquemandir}/man7/pbs_resources_irix5.7*
%{torquemandir}/man7/pbs_resources_irix6.7*
%{torquemandir}/man7/pbs_resources_irix6array.7*
%{torquemandir}/man7/pbs_resources_linux.7*
%{torquemandir}/man7/pbs_resources_netbsd.7*
%{torquemandir}/man7/pbs_resources_solaris5.7*
%{torquemandir}/man7/pbs_resources_solaris7.7*
%{torquemandir}/man7/pbs_resources_sp2.7*
%{torquemandir}/man7/pbs_resources_sunos4.7*
%{torquemandir}/man7/pbs_resources_unicos8.7*
%{torquemandir}/man7/pbs_resources_unicosmk2.7*
%{torquemandir}/man7/pbs_server_attributes.7*
# And the following are alternative managed ones.
%{torquemandir}/man1/qsub-torque.1*
%{torquemandir}/man1/qalter-torque.1*
%{torquemandir}/man1/qdel-torque.1*
%{torquemandir}/man1/qhold-torque.1*
%{torquemandir}/man1/qrls-torque.1*
%{torquemandir}/man1/qselect-torque.1*
%{torquemandir}/man1/qstat-torque.1*
%{torquemandir}/man1/pbsdsh-torque.1*

%posttrans client
/usr/sbin/alternatives --install %{_bindir}/qsub qsub %{torquebindir}/qsub-torque 10 \
        --slave %{_mandir}/man1/qsub.1.gz qsub-man \
                %{torquemandir}/man1/qsub-torque.1.gz \
        --slave %{_bindir}/qalter qalter %{torquebindir}/qalter-torque \
        --slave %{_mandir}/man1/qalter.1.gz qalter-man \
                %{torquemandir}/man1/qalter-torque.1.gz \
        --slave %{_bindir}/qdel qdel %{torquebindir}/qdel-torque \
        --slave %{_mandir}/man1/qdel.1.gz qdel-man \
                %{torquemandir}/man1/qdel-torque.1.gz \
        --slave %{_bindir}/qhold qhold %{torquebindir}/qhold-torque \
        --slave %{_mandir}/man1/qhold.1.gz qhold-man \
                %{torquemandir}/man1/qhold-torque.1.gz \
        --slave %{_bindir}/qrls qrls %{torquebindir}/qrls-torque \
        --slave %{_mandir}/man1/qrls.1.gz qrls-man \
                %{torquemandir}/man1/qrls-torque.1.gz \
        --slave %{_bindir}/qselect qselect %{torquebindir}/qselect-torque \
        --slave %{_mandir}/man1/qselect.1.gz qselect-man \
                %{torquemandir}/man1/qselect-torque.1.gz \
        --slave %{_bindir}/qstat qstat %{torquebindir}/qstat-torque \
        --slave %{_mandir}/man1/qstat.1.gz qstat-man \
                %{torquemandir}/man1/qstat-torque.1.gz \
        --slave %{_bindir}/pbsdsh pbsdsh %{torquebindir}/pbsdsh-torque \
        --slave %{_mandir}/man1/pbsdsh.1.gz pbsdsh-man \
                %{torquemandir}/man1/pbsdsh-torque.1.gz

%post client
/sbin/ldconfig
/sbin/chkconfig --add trqauthd

%preun client
[ $1 = 0 ] || exit 0
/sbin/ldconfig
/usr/sbin/alternatives --remove qsub %{_bindir}/qsub-torque
/sbin/chkconfig --del trqauthd


%if %{build_gui}
%package              gui
Group:                Applications/System
Summary:              Graphical clients for TORQUE
Requires:             %{name}-client = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:            torque-gui < 4.1.0
Provides:             torque-gui = %{version}-%{release}
Provides:             xpbs xpbsmon
#AutoReqProv: no

%description gui
%shared_description
This package holds the graphical clients.


%files gui
%defattr(-, root, root)
%{torquebindir}/pbs_wish
%{torquebindir}/xpbs
%{torquebindir}/xpbsmon
%{torquelibdir}/xpbs
%{torquelibdir}/xpbsmon
%{_datadir}/applications/*.desktop
%{_datadir}/pixmaps/*.png
%if 0%{?doxydoc}
%{torquemandir}/man1/xpbs.1*
%{torquemandir}/man1/xpbsmon.1*
%endif
%endif

%package libs
Summary:      Run-time libs for programs which will use the %{name} library
Group:        Development/Libraries
Requires:     torque = %{version}-%{release}
Obsoletes:    libtorque  < 4.1.0
Obsoletes:    torque-libs  < 4.1.0
Provides:     libtorque = %{version}-%{release}
Provides:     torque-libs = %{version}-%{release}
Requires:     munge

%description libs
%shared_description
This package includes the shared libraries necessary for running TORQUE
programs.

%files libs
%defattr(-, root, root, -)
%{torquelibdir}/libtorque.so.*

%package localhost
Group: Applications/System
Summary: installs and configures a minimal localhost-only batch queue system
Requires: pbs-mom pbs-server pbs-client pbs-scheduler

%description localhost
%shared_description
This package installs and configures a minimal localhost-only batch queue system.

%files localhost
%defattr(-, root, root)

%post localhost
/sbin/chkconfig pbs_mom on
/sbin/chkconfig pbs_server on
/sbin/chkconfig pbs_sched on
/bin/hostname --long > %{torquehomedir}/server_priv/nodes
/bin/hostname --long > %{torquehomedir}/server_name
/bin/hostname --long > %{torquehomedir}/mom_priv/config
%{torquesbindir}/pbs_server -t create
%{torquebindir}/qmgr -c "s s scheduling=true"
%{torquebindir}/qmgr -c "c q batch queue_type=execution"
%{torquebindir}/qmgr -c "s q batch started=true"
%{torquebindir}/qmgr -c "s q batch enabled=true"
%{torquebindir}/qmgr -c "s q batch resources_default.nodes=1"
%{torquebindir}/qmgr -c "s q batch resources_default.walltime=3600"
%{torquebindir}/qmgr -c "s s default_queue=batch"
%{torquebindir}/qmgr -c 's s log_keep_days = 10'
%{_initrddir}/pbs_mom restart
%{_initrddir}/pbs_sched restart
%{_initrddir}/pbs_server restart
%{_initrddir}/trqauthd restart
%{torquebindir}/qmgr -c "s n `/bin/hostname --long` state=free" -e

%package devel
Summary:     Development tools for programs which will use the %{name} library.
Group:       Development/Libraries
Requires:    %{name}-libs = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:   libtorque-devel < 4.1.0
Provides:    lib%{name}-devel = %{version}-%{release}
Provides:    libtorque-devel = %{version}-%{release}
#AutoReqProv: no

%description devel
%shared_description
This package includes the header files and static libraries
necessary for developing programs which will use %{name}.

%files devel
%defattr(-, root, root)
%{torquelibdir}/*.*a
%{torquelibdir}/*.so
%{torqueincdir}/pbs_error.h
%{torqueincdir}/pbs_error_db.h
%{torqueincdir}/pbs_ifl.h
%{torqueincdir}/rm.h
%{torqueincdir}/rpp.h
%{torqueincdir}/tm.h
%{torqueincdir}/tm_.h
%{torqueincdir}/array_func.h
%{torqueincdir}/attr_node_func.h
%{torqueincdir}/catch_child.h
%{torqueincdir}/checkpoint.h
%{torqueincdir}/chk_file_sec.h
%{torqueincdir}/common_cmds.h
%{torqueincdir}/issue_request.h
%{torqueincdir}/ji_mutex.h
%{torqueincdir}/job_func.h
%{torqueincdir}/job_route.h
%{torqueincdir}/lib_dis.h
%{torqueincdir}/lib_ifl.h
%{torqueincdir}/lib_mom.h
%{torqueincdir}/lib_net.h
%{torqueincdir}/libcmds.h
%{torqueincdir}/license_pbs.h
%{torqueincdir}/log_event.h
%{torqueincdir}/mom_comm.h
%{torqueincdir}/mom_job_func.h
%{torqueincdir}/mom_main.h
%{torqueincdir}/mom_process_request.h
%{torqueincdir}/mom_server_lib.h
%{torqueincdir}/node_func.h
%{torqueincdir}/node_manager.h
%{torqueincdir}/pbs_cmds.h
%{torqueincdir}/pbs_constants.h
%{torqueincdir}/pbs_log.h
%{torqueincdir}/pbsd_init.h
%{torqueincdir}/pbsd_main.h
%{torqueincdir}/pbsnodes.h
%{torqueincdir}/process_request.h
%{torqueincdir}/prolog.h
%{torqueincdir}/qsub_functions.h
%{torqueincdir}/queue_func.h
%{torqueincdir}/queue_recov.h
%{torqueincdir}/queue_recycler.h
%{torqueincdir}/reply_send.h
%{torqueincdir}/req_delete.h
%{torqueincdir}/req_deletearray.h
%{torqueincdir}/req_getcred.h
%{torqueincdir}/req_gpuctrl.h
%{torqueincdir}/req_holdarray.h
%{torqueincdir}/req_holdjob.h
%{torqueincdir}/req_jobobit.h
%{torqueincdir}/req_locate.h
%{torqueincdir}/req_manager.h
%{torqueincdir}/req_message.h
%{torqueincdir}/req_modify.h
%{torqueincdir}/req_movejob.h
%{torqueincdir}/req_quejob.h
%{torqueincdir}/req_register.h
%{torqueincdir}/req_rerun.h
%{torqueincdir}/req_rescq.h
%{torqueincdir}/req_runjob.h
%{torqueincdir}/req_select.h
%{torqueincdir}/req_shutdown.h
%{torqueincdir}/req_signal.h
%{torqueincdir}/req_stat.h
%{torqueincdir}/req_track.h
%{torqueincdir}/setup_env.h
%{torqueincdir}/svr_connect.h
%{torqueincdir}/svr_func.h
%{torqueincdir}/svr_jobfunc.h
%{torqueincdir}/svr_movejob.h
%{torqueincdir}/svr_task.h
%{torqueincdir}/tcp.h
%{torqueincdir}/trq_auth_daemon.h
%{torqueincdir}/u_hash_map_structs.h
%{torqueincdir}/u_lock_ctl.h
%{torqueincdir}/u_memmgr.h
%{torqueincdir}/uthash.h
%{torquebindir}/pbs-config
%{torquemandir}/man3/pbs_alterjob.3*
%{torquemandir}/man3/pbs_connect.3*
%{torquemandir}/man3/pbs_default.3*
%{torquemandir}/man3/pbs_deljob.3*
%{torquemandir}/man3/pbs_disconnect.3*
%{torquemandir}/man3/pbs_geterrmsg.3*
%{torquemandir}/man3/pbs_holdjob.3*
%{torquemandir}/man3/pbs_locate.3*
%{torquemandir}/man3/pbs_manager.3*
%{torquemandir}/man3/pbs_movejob.3*
%{torquemandir}/man3/pbs_msgjob.3*
%{torquemandir}/man3/pbs_orderjob.3*
%{torquemandir}/man3/pbs_rerunjob.3*
%{torquemandir}/man3/pbs_rescquery.3*
%{torquemandir}/man3/pbs_rescreserve.3*
%{torquemandir}/man3/pbs_rlsjob.3*
%{torquemandir}/man3/pbs_runjob.3*
%{torquemandir}/man3/pbs_selectjob.3*
%{torquemandir}/man3/pbs_selstat.3*
%{torquemandir}/man3/pbs_sigjob.3*
%{torquemandir}/man3/pbs_stagein.3*
%{torquemandir}/man3/pbs_statjob.3*
%{torquemandir}/man3/pbs_statnode.3*
%{torquemandir}/man3/pbs_statque.3*
%{torquemandir}/man3/pbs_statserver.3*
%{torquemandir}/man3/pbs_submit.3*
%{torquemandir}/man3/pbs_terminate.3*
%{torquemandir}/man3/pbs_checkpointjob.3*
%{torquemandir}/man3/pbs_fbserver.3*
%{torquemandir}/man3/pbs_get_server_list.3*
%{torquemandir}/man3/rpp.3*
%{torquemandir}/man3/tm.3*
%{torquemandir}/man3/pbs_gpumode.3*
%{torquemandir}/man3/pbs_gpureset.3*

%pre devel
# previous versions of this spec file installed these as symlinks
test -L %{torquelibdir} && rm -f %{torquelibdir}
test -L %{torqueincdir} && rm -f %{torqueincdir}
exit 0


%package pam
Summary: PAM module for PBS MOM nodes.
Group: System Environment/Base

%description pam
%shared_description
A simple PAM module to authorize users on PBS MOM nodes with a running job.

%files pam
%defattr(-, root, root)
%doc src/pam/README.pam
/%{_lib}/security/pam_pbssimpleauth.so

%else

%package              docs
Group:                Documentation
Summary:              Documentation files for TORQUE
Requires:             %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Obsoletes:            torque-docs < 4.1.0
Provides:             pbs-docs = %{version}-%{release}
Provides:             torque-docs = %{version}-%{release}
%if 0%{?fedora} >= 10 || 0%{?rhel} >= 6
BuildArch:            noarch
%endif

%description docs
%shared_description
This package holds the documentation files.

%files docs
%defattr(-, root, root)
%doc doc/admin_guide.ps
%if 0%{?doxydoc}
%doc src/drmaa/drmaa.pdf
%endif

%package modulefile
Summary: OSCARified TORQUE Resource Manager modulefile
Requires: %{name} = %{version}-%{release}
Group: Applications/batch
Requires: modules-oscar >= 3.1
#AutoReqProv: no

%description modulefile
The TORQUE Resource Manager is a flexible workload management
system.  It operates on networked, multi-platform UNIX environments,
including heterogeneous clusters of workstations, supercomputers,
and massively parallel systems.

This package includes a modulefile for TORQUE on OSCAR cluster.  It
is used to set the corresponding PATH and MANPATH.

%files modulefile
%dir /opt/modules/oscar-modulefiles/%{name}
/opt/modules/oscar-modulefiles/%{name}/%{version}

%endif

%changelog
* Mon Feb 17 2014 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.7-3
- Obsoletes *torque* < 4.1.0 to make sure we do not install our server
  with old torquelibs for example.

* Tue Nov 26 2013 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.7-2
- Update pbs_environment so it includes torquebindir and torquesbindir.

* Tue Oct 23 2013 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.7-1
- Final release of upstream version 4.1.7.

* Tue Apr 02 2013 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.5.1-1
- Final release of upstream version 4.1.5.1.

* Wed Jan 30 2013 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.4-1
- Final release of upstream version 4.1.4.

* Wed Dec 12 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.4-0.2
- Limit server logs in default configuration to 10 days.
- Removed the logrotate setup (useless).

* Tue Dec 11 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.4-0.1
- New upstream release (4.1.4-snap.201211201307)

* Wed Dec  5 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.3-6
- Updated pbs_server initd script (qterm path)
- removed torque-oscar-extras archive and replaced remaining requirements
  as sources.
- using mom_config source file instead of generating the config file by hand.

* Tue Dec  4 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.3-5
- Avoid redefining _prefix so _sbindir still points to /usr/sbin
  (needed for sendmail location).

* Wed Nov 28 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.3-4
- comments does not work with ifarch marco. (removed the%)
- spec file cleanup and cosmetic.

* Wed Nov 28 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.3-3
- Add a patch for pbs_senvironment security check when it's a link.

* Fri Nov 16 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.3-2
- full spec rewrite. (also fix noarch build)

* Fri Nov 09 2012 Olivier Lahaye <olivier.lahaye1@free.fr> 4.1.3-1
- Upgrade package to latest stable release: 4.1.3.

* Wed Jun 15 2011 Olivier Lahaye <olivier.lahaye1@free.fr> 2.4.14-1
- Upgrade package to latest stable release: 2.4.14.

* Tue Feb 15 2011 Geoffroy Vallee <valleegr@ornl.gov> 
- Fix the post install script of the localhost package.

* Mon Feb 14 2011 Geoffroy Vallee <valleegr@ornl.gov> 2.3.7-6
- Deactivate the automatic management of dependencies for a few packages since
  it was creating issues.

* Fri Feb 11 2011 Geoffroy Vallee <valleegr@ornl.gov> 2.3.7-5
- Fix the post script of torque-oscar-server.

* Fri Sep 11 2009 Emir Imamagic <eimamagi@srce.hr> 2.3.7-4
- Upgraded torque version

* Tue Jun 10 2008 DongInn Kim <dikim@osl.iu.edu> 2.1.10-4
- Rename torque by adding postfix "oscar".

