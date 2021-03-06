
# comment out snap if building a real release
%define name torque-oscar
%define version 2.1.10

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
%define build_gui 1

# these are non-defaults, but fit better into most RPM-based systems
%define torquehomedir   /var/spool/pbs
#%%define _libdir       %{_prefix}/%{_lib}/%{name}/lib
#%%define _includedir   %{_prefix}/%{_lib}/%{name}/include

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



# --with/--without processing
# first, error if conflicting options are used
%{?_with_syslog: %{?_without_syslog: %{error: both _with_syslog and _without_syslog}}}
%{?_with_rpp: %{?_without_rpp: %{error: both _with_rpp and _without_rpp}}}
%{?_with_scp: %{?_without_scp: %{error: both _with_scp and _without_scp}}}
%{?_with_tcl: %{?_without_tcl: %{error: both _with_tcl and _without_tcl}}}
%{?_with_gui: %{?_without_gui: %{error: both _with_gui and _without_gui}}}

# did we find any --with options?
%{?_with_syslog: %define use_syslog 1}
%{?_with_rpp: %define use_rpp 1}
%{?_with_scp: %define use_scp 1}
%{?_with_tcl: %define use_tcl 1}
%{?_with_gui: %define build_gui 1}

%{?_with_server_name:%define server_name %(set -- %{_with_server_name}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_homedir:%define torquehomedir %(set -- %{_with_homedir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_libdir:%define _libdir %(set -- %{_with_libdir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_includedir:%define _includedir %(set -- %{_with_includedir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_prefix:%define _prefix %(set -- %{_with_prefix}; echo $1 | grep -v with | sed 's/=//')}

# did we find any --without options?
%{?_without_syslog: %define use_syslog 0}
%{?_without_rpp: %define use_rpp 0}
%{?_without_scp: %define use_scp 0}
%{?_without_tcl: %define use_tcl 0}
%{?_without_gui: %define build_gui 0}

# Set up all options as disabled
%define syslogflags --disable-syslog
%define rppflags    --disable-rpp
%define scpflags    %{nil}
%define tclflags    --without-tcl
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
Source1: torque-oscar-extra.tar.gz
License: distributable
Group: System Environment/Daemons
URL: http://www.clusterresources.com/products/torque/
BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot
Provides: pbs
BuildRequires: ed
Conflicts: pbspro, openpbs, openpbs-oscar
Obsoletes: scatorque

%if ! %build_gui
Obsoletes: torque-gui
%endif

%description
%shared_description
This package holds just a few shared files and directories.

%prep
%setup -n torque-%{version}%{?snap:-snap.%snap}
%ifarch noarch
%setup -a 1 -n torque-%{version}%{?snap:-snap.%snap}
%endif

%build
%ifnarch noarch
./configure --prefix=%{torqueprefix} --mandir=%{torquemandir} --libdir=%{torquelibdir} \
 --includedir=%{torqueincdir} --with-server-home=%{torquehomedir} \
 --with-pam=/%{_lib}/security \
 %{server_nameflags} \
 %{guiflags} %{syslogflags} %{tclflags} %{rppflags} %{scpflags} \
 CC="$RPMCC" CFLAGS="$RPMCFLAGS" LDFLAGS="$RPMLDFLAGS"

%{__make} %{?_smp_mflags}
%endif
 


%install
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf "$RPM_BUILD_ROOT"
%ifnarch noarch
%{__make} DESTDIR=$RPM_BUILD_ROOT install
%endif

%ifnarch noarch
if [ -f /etc/SuSE-release ];then
  initpre="suse."
else
  initpre=""
fi

# install initscripts
%if ! %{?_initrddir:1}0
%define _initrddir /etc/init.d
%endif
%{__mkdir_p} $RPM_BUILD_ROOT%{_initrddir}
for initscript in pbs_mom pbs_sched pbs_server; do
  %__sed -e 's|^PBS_HOME=.*|PBS_HOME=%{torquehomedir}|' \
         -e 's|^PBS_DAEMON=.*|PBS_DAEMON=%{torquesbindir}/'$initscript'|' \
        < contrib/init.d/$initpre$initscript > $RPM_BUILD_ROOT%{_initrddir}/$initscript
  %__chmod 755 $RPM_BUILD_ROOT%{_initrddir}/$initscript
done

# create oscar configfile
cat > $RPM_BUILD_ROOT/%{torquehomedir}/mom_priv/config <<\EOF
$logevent 127
$clienthost pbs_oscar
$usecp pbs_oscar:/home /home
$restricted pbs_oscar
EOF

# remove libtool droppings
%{__rm} -f $RPM_BUILD_ROOT/%{_lib}/security/pam_pbssimpleauth.{a,la}

%else
mkdir -p %{buildroot}/opt/modules/oscar-modulefiles/%{name}
cp -p torque-modulefile %{buildroot}/opt/modules/oscar-modulefiles/%{name}/%{version}
%endif


%clean
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf $RPM_BUILD_ROOT

%post
%ifnarch noarch
if %__grep -q "PBS services" /etc/services;then
   : PBS services already installed
else
   cat<<-__EOF__>>/etc/services
	# Standard PBS services
	pbs           15001/tcp           # pbs server (pbs_server)
	pbs           15001/udp           # pbs server (pbs_server)
	pbs_mom       15002/tcp           # mom to/from server
	pbs_mom       15002/udp           # mom to/from server
	pbs_resmom    15003/tcp           # mom resource management requests
	pbs_resmom    15003/udp           # mom resource management requests
	pbs_sched     15004/tcp           # scheduler
	pbs_sched     15004/udp           # scheduler
	__EOF__
fi


%files
%defattr(-, root, root)
%doc INSTALL README.torque torque.setup Release_Notes CHANGELOG PBS_License.txt
%config(noreplace) %{torquehomedir}/pbs_environment
%config(noreplace) %{torquehomedir}/server_name
%{torquehomedir}/aux
%{torquehomedir}/spool
%endif


%ifnarch noarch

%package docs
Group: Documentation
Summary: docs for Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-docs
%description docs
%shared_description
This package holds the documentation files.

%files docs
%defattr(-, root, root)
%doc doc/admin_guide.ps
%{torquemandir}/man*/*

%package scheduler
Group: System Environment/Daemons
Summary: scheduler part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-scheduler
%description scheduler
%shared_description
This package holds the fifo C scheduler.

%files scheduler
%defattr(-, root, root)
%{torquesbindir}/pbs_sched
%{_initrddir}/pbs_sched
%dir %{torquehomedir}/sched_priv
%config(noreplace) %{torquehomedir}/sched_priv/*
%{torquehomedir}/sched_logs

%post scheduler
/sbin/chkconfig --add pbs_sched

%preun scheduler
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_sched


%package server
Group: System Environment/Daemons
Summary: server part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-server
%description server
%shared_description
This package holds the server.

%files server
%defattr(-, root, root)
%{torquesbindir}/pbs_server
%{torquesbindir}/momctl
%{_initrddir}/pbs_server
%{torquehomedir}/server_logs
%{torquehomedir}/server_priv

%post server
/sbin/chkconfig --add pbs_server

%preun server
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_server


%package mom
Group: System Environment/Daemons
Summary: execution part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-mom
%description mom
%shared_description
This package holds the execute daemon required on every node.

%files mom
%defattr(-, root, root)
%{torquesbindir}/pbs_mom
%{_initrddir}/pbs_mom
%if ! %{use_scp}
%attr(4755 root root) %{_sbindir}/pbs_rcp
%endif
%{torquehomedir}/mom_priv/*
%{torquehomedir}/mom_logs
%{torquehomedir}/checkpoint
%{torquehomedir}/undelivered

%post mom
/sbin/chkconfig --add pbs_mom

%preun mom
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_mom


%package client
Group: Applications/System
Summary: client part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-client
%description client
%shared_description
This package holds the command-line client programs.

%files client
%defattr(-, root, root)
%{torquebindir}/q*
%{torquebindir}/chk_tree
%{torquebindir}/hostn
%{torquebindir}/nqs2pbs
%{torquebindir}/pbsdsh
%{torquebindir}/pbsnodes
%{torquebindir}/printjob
%{torquebindir}/printtracking
%{torquebindir}/tracejob
%{torquelibdir}/*.so*
%attr(4755 root root) %{torquesbindir}/pbs_iff
%{torquesbindir}/pbs_demux
%if %use_tcl
%{torquebindir}/pbs_tclsh
%endif

%package gui
Group: Applications/System
Summary: graphical client part of Torque
Requires: %{name}-client = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: xpbs xpbsmon
%description gui
%shared_description
This package holds the graphical clients.


%if %{build_gui}
%files gui
%defattr(-, root, root)
%{torquebindir}/pbs_wish
%{torquebindir}/xpbs
%{torquebindir}/xpbsmon
%{torquelibdir}/xpbs
%{torquelibdir}/xpbsmon
%endif


%package localhost
Group: Applications/System
Summary: installs and configures a minimal localhost-only batch queue system
PreReq: pbs-mom pbs-server pbs-client pbs-scheduler

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
pbs_server -t create
qmgr -c "s s scheduling=true"
qmgr -c "c q batch queue_type=execution"
qmgr -c "s q batch started=true"
qmgr -c "s q batch enabled=true"
qmgr -c "s q batch resources_default.nodes=1"
qmgr -c "s q batch resources_default.walltime=3600"
qmgr -c "s s default_queue=batch"
%{_initrddir}/pbs_mom restart
%{_initrddir}/pbs_sched restart
%{_initrddir}/pbs_server restart
qmgr -c "s n `/bin/hostname --long` state=free" -e

%package devel
Summary: Development tools for programs which will use the %{name} library.
Group: Development/Libraries
Provides: lib%{name}-devel
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}

%description devel
%shared_description
This package includes the header files and static libraries
necessary for developing programs which will use %{name}.

%files devel
%defattr(-, root, root)
%{torquelibdir}/*.*a
%{torquelibdir}/*.so
%{torqueincdir}/*.h
%{torquebindir}/pbs-config

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

%package modulefile
Summary: OSCARified TORQUE Resource Manager modulefile
Requires: %{name} = %{version}-%{release}
Group: Applications/batch
BuildPreReq: rpm >= 3.0.5
Requires: modules-oscar >= 3.1
AutoReqProv: no

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
* Tue Jun 10 2008 DongInn Kim <dikim@osl.iu.edu> 2.1.10-4
- Rename torque by adding postfix "oscar".
