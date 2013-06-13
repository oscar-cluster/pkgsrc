Name: loghost
Summary: loghost configurator
Version: 1.0
Release: 2
Packager: Jeremy Enos <jenos@ncsa.uiuc.edu>
License: GPL
Group: Applications/System
#Requires: sh-utils
Requires: initscripts sysklogd
BuildArch: noarch
Summary: Configures server to accept remote syslog entries
Group: Applications/System

%define syslogd_conf /etc/sysconfig/syslog
%define tmp_file /tmp/loghost.tmp

%description
loghost configures syslog settings.

%preun
. %{syslogd_conf}
rm_option="-r"
for option in $SYSLOGD_OPTIONS; do
  if [ "$option" != "$rm_option" ]; then
    if [ -z $new_options ] ; then
      new_options="$option"
    else
      new_options="$new_options $option"
    fi
  fi
done

cat %{syslogd_conf} |sed "s/SYSLOGD_OPTIONS=.*$/SYSLOGD_OPTIONS=\"$new_options\"/g" > %{tmp_file}
%__mv -f %{tmp_file} %{syslogd_conf}
service syslog restart 2> /dev/null > /dev/null

%post

. %{syslogd_conf}
addon_option="-r"
set_option=1
for option in $SYSLOGD_OPTIONS; do
  if [ "$option" = "$addon_option" ]; then
    set_option=0
  fi
  if [ -z $new_options ] ; then
    new_options="$option"
  else
    new_options="$new_options $option"
  fi
done
if [ $set_option = 1 ]; then
  new_options="$new_options $addon_option"
fi

cat %{syslogd_conf} |sed "s/SYSLOGD_OPTIONS=.*$/SYSLOGD_OPTIONS=\"$new_options\"/g" > %{tmp_file}
%__mv -f %{tmp_file} %{syslogd_conf}
service syslog restart 2> /dev/null > /dev/null

%files

%changelog

