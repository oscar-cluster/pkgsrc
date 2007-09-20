#!/usr/bin/make -f
#
# Makefile to build OSCAR repositories system
#
BASEDIR=/local/buildd
CHROOTDIR=/srv/chroot
LVM_VG=vg-build
USR=buildd
GRP=buildd

# Debian specific
DEB_DISTS=etch sid
DEB_ARCHS=i386 amd64

DEB_ENVS=$(foreach dist,$(DEB_DISTS),$(foreach arch,$(DEB_ARCHS),$(CHROOTDIR)/$(dist)_$(arch)))

BUILDD_SOURCE_BASE=debian-admin.list
BUILDD_SOURCE=/etc/apt/sources.list.d/$(BUILDD_SOURCE_BASE)
BUILDD=/usr/bin/buildd

# RPM specific
RPM_DISTS=fc6 fc7 rhel4 rhel5 sles10
RPM_ARCHS=i386 x86_64

RPM_ENVS=$(foreach dist,$(RPM_DISTS),$(foreach arch,$(RPM_ARCHS),$(CHROOTDIR)/$(dist)_$(arch)))

all:
	@if test -e "/etc/debian_version"; then \
	  $(MAKE) apt; \
	elif test -d "/etc/yum.repos.d"; then \
	  $(MAKE) rpm; \
	else \
	  echo "E: Unknown distribution"; \
	fi

apt: check_user_debian $(BUILDD) $(BASEDIR) create_deb_envs
	@echo "I: Install sbuildrc file in $(USR) homedir"
	@install -o $(USR) -g $(GRP) ./sbuildrc `getent passwd buildd | cut -d':' -f6`/.sbuildrc

$(BUILDD): $(BUILDD_SOURCE)
	@echo "I: Install buildd"
	@aptitude update
	@aptitude --assume-yes install buildd

create_deb_envs: buildd_dirs $(DEB_ENVS)

$(CHROOTDIR)/%:
	@export USR=$(USR); \
	  dist=$(shell echo $(@F) | cut -d'_' -f1); \
	  arch=$(shell echo $(@F) | cut -d'_' -f2); \
	  echo "I: Create build env for $${dist} ($${arch}) into $@"; \
	  ./create_build_env -a $${arch} -d $${dist} -v $(LVM_VG) && \

buildd_dirs: check_user_debian
	@echo "I: Create buildd dirs"
	@( \
	  cd $(BASEDIR); \
	  install -d build mqueue -o $(USR) -g $(GRP); \
	  install -d logs stats -o $(USR) -g $(GRP); \
	  install -d secret -o $(USR) -g adm -m 2770; \
	  install -d /var/debbuild/srcdep-lock -o $(USR); \
	  install -d /var/lib/sbuild/srcdep-lock -o $(USR); \
	  install -d bin -o $(USR) -g adm -m 775 \
	)

$(BUILDD_SOURCE): $(BUILDD_SOURCE_BASE) check_root
	@echo "I: Install apt source for Debian admin tools"
	@if [ -e "$@" ]; then \
	  cp $@ $@.bak; \
	fi
	cp -f $< $@

rpm:

$(BASEDIR):
	@echo "I: Create $(BASEDIR)"
	@install -d $(BASEDIR) -o $(USR) -g $(GRP)

check_root:
	@[ `id -u` = 0 ] || ( \
	  echo "E: Must be run as root"; \
	  exit 1 \
	)

check_user_debian: check_root
	@if ! getent passwd buildd > /dev/null; then \
	  echo "I: Add user $(USR)"; \
	  adduser $(USR); \
	fi
	@if ! getent group buildd > /dev/null; then \
	  echo "I: Add group $(GRP)";\
	  addgroup $(GRP);\
	fi
	@if ! groups $(USR) | cut -d':' -f2 | grep $(GRP) > /dev/null; then \
	  echo "I: Add user $(USR) to group $(GRP)"; \
	  adduser $(USR) $(GRP); \
	fi

clean:
	@if test -e "/etc/debian_version"; then \
	  $(MAKE) clean_apt; \
	elif test -d "/etc/yum.repos.d"; then \
	  $(MAKE) clean_rpm; \
	else \
	  echo "E: Unknown distribution"; \
	fi

clean_apt: clean_deb_envs

clean_deb_envs:
	@for dist in $(DEB_DISTS); do \
	  for arch in $(DEB_ARCHS); do \
	    export USR=$(USR); \
	    export ENV_NAME=$$dist_$$arch; \
	    export TARGET=/srv/chroot/$$ENV_NAME; \
	    echo "I: Delete build env for $$dist ($$arch) into $$TARGET"; \
	    ./create_build_env -a $$arch -d $$dist -v $(LVM_VG) -c; \
	    rm -f $(BASEDIR)/build/chroot-$$ENV_NAME; \
	  done; \
	done

clean_rpm: