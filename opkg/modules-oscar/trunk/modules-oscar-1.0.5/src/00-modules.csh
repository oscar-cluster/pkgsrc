#
# Copyright (c) 2002 The Trustees of Indiana University.  
#                    All rights reserved.
#
# This file is part of the modules-oscar software package.  For
# license information, see the LICENSE file in the top-level directory
# of the modules-oscar source distribution.
#
# $Id: 00-modules.csh,v 1.3 2002/10/27 12:24:07 jsquyres Exp $
#

# Because of weirdness, this file may get sourced more than once.  Add
# protection to ensure that we actually only *run* once.

if ($?MODULE_OSCAR == "0") then
    set MODULE_OSCAR=foo

    # If we change users (e.g., "su") or change shells, we need to
    # reset everything.

    set MODULE_OSCAR_NEED_RESET=0

    # Keep a record of who the user who is that we setup for.  If it
    # changed, set MODULE_OSCAR_NEED_RESET.  Likewise, if MODULE_SHELL
    # is not "csh", then we also need a full reset.

    set MODULE_OSCAR_USER_NOW="`whoami`"
    if ($?MODULE_OSCAR_USER == 0) set MODULE_OSCAR_USER=
    if ("$MODULE_OSCAR_USER" != "$MODULE_OSCAR_USER_NOW") then
	setenv MODULE_OSCAR_USER "$MODULE_OSCAR_USER_NOW"
	set MODULE_OSCAR_NEED_RESET=1
    else if ($?MODULE_SHELL == 1) then
	if ("$MODULE_SHELL" != "csh") then
	    set MODULE_OSCAR_NEED_RESET=1
	endif
    endif

    # This list of variables to unset is highly specific to version
    # 3.1.6 of modules.  If there is ever a new version of modules,
    # we'll need to ensure that this list of variables is still
    # correct.

    if ("$MODULE_OSCAR_NEED_RESET" == "1") then
	unsetenv MODULE_VERSION MODULEPATH LOADEDMODULES MODULESHOME \
	    _LMFILES_ MODULE_VERSION_STACK MODULE_VERSION _MODULESBEGINENV_
    endif
    setenv MODULE_SHELL "csh"

    # NOTE: Because the RH 7.x /etc/bashrc sucks, the sh/bash init file
    # corresponding to this one actually lives in /opt/modules/etc, and is
    # sourced directly from /etc/bashrc (otherwise things like "rsh
    # somehost who" will fail use the user has module commands in their
    # $HOME/.bashrc).
    
    if ($?MODULE_VERSION == 0) then
# -- BEGIN -- Modules distribution file: etc/global/csh.modules
# INSERT-CSH-MODULES-HERE
# -- END -- Modules distribution file: etc/global/csh.modules
    endif

# -- BEGIN -- Modules distribution file: etc/global/csh.cshrc
# INSERT-CSH-CSHRC-HERE
# -- END -- Modules distribution file: etc/global/csh.cshrc

    # Now load all the OSCAR modules

    module load oscar-modules
endif

# All done

exit 0
