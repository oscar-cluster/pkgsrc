#
# Copyright (c) 2002 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the modules-oscar software package.  For
# license information, see the LICENSE file in the top-level directory
# of the modules-oscar source distribution.
#
# $Id: 00-modules.sh,v 1.3 2002/10/27 12:24:07 jsquyres Exp $
#

# Because of weirdness, this file may get sourced more than once.  Add
# protection to ensure that we actually only *run* once.

if test "$MODULE_OSCAR" = ""; then
    MODULE_OSCAR=foo

    # If we change users (e.g., "su") or change shells, we need to
    # reset everything.

    MODULE_OSCAR_NEED_RESET=0

    # Keep a record of who the user who is that we setup for.  If it
    # changed, set MODULE_OSCAR_NEED_RESET.  Likewise, if MODULE_SHELL
    # is not "csh", then we also need a full reset.

    MODULE_OSCAR_USER_NOW="`whoami`"
    if test "$MODULE_OSCAR_USER" != "$MODULE_OSCAR_USER_NOW"; then
	export MODULE_OSCAR_USER="$MODULE_OSCAR_USER_NOW"
	MODULE_OSCAR_NEED_RESET=1
    elif test "$MODULE_SHELL" != "sh"; then
	MODULE_OSCAR_NEED_RESET=1
    fi

    # This list of variables to unset is highly specific to version
    # 3.1.6 of modules.  If there is ever a new version of modules,
    # we'll need to ensure that this list of variables is still
    # correct.

    if test "$MODULE_OSCAR_NEED_RESET" = "1"; then
	unset MODULE_VERSION MODULEPATH LOADEDMODULES MODULESHOME \
	    _LMFILES_ MODULE_VERSION_STACK MODULE_VERSION _MODULESBEGINENV_
    fi
    export MODULE_SHELL=sh

    # NOTE: Because the RH 7.x /etc/bashrc sucks, the csh init file
    # corresponding to this one actually lives in /etc/profile.d.  This
    # file is not in /etc/profile.d and is instead sourced directly from
    # /etc/bashrc (otherwise things like "rsh somehost who" will fail use
    # the user has module commands in their $HOME/.bashrc).

    if test "$MODULE_VERSION" = ""; then
# -- BEGIN -- Modules distribution file: etc/global/profile.modules
# INSERT-PROFILE-MODULES-HERE
# -- END -- Modules distribution file: etc/global/profile.modules

	# -- BEGIN -- Modules distribution snipit: etc/global/profile
	#--------------------------------------------------------------------#
	# set this if bash exists on your system and to use it
	# instead of sh - so per-process dot files will be sourced.
	#--------------------------------------------------------------------#
    
	ENV=$HOME/.bashrc
	export ENV
	# -- END -- Modules distribution snipit: etc/global/profile
    fi

    # For #!/bin/bash scripts, $0 will be set to the name of the script
    # (RTFM: bash man page) instead of some form of the word "bash".  This
    # screws up the logic that will be included below, because it expects
    # $0 to be some form of "sh", "ksh", or "bash".  If it's not, nothing
    # Bad happens, but modules are effectively misconfigured because the
    # modules subroutine will use an empty value for $modules_shell.
    #
    # So this is a total hack -- pre-initialize modules_shell to be "sh".
    # If $0 is set properly, then this value will be overriden.  If it's
    # not, then we'll use "sh" settings.  This isn't Right, but I can't
    # figure out any other way to do it.  :-( (I should note that this is
    # not quite as evil as it sounds because the profile.modules (above)
    # essentially makes this same assumption.  Also note that this is
    # *only* done for sh-like shells, not csh-like shells.)
    
    modules_shell=sh

# -- BEGIN -- Modules distribution file: etc/global/bashrc
# INSERT-BASHRC-HERE
# -- END -- Modules distribution file: etc/global/bashrc

    # Now load all the OSCAR modules

    module load oscar-modules
fi

# All done.  Do *NOT* exit here!  Bash will exit the calling script if
# we "exit" here.
