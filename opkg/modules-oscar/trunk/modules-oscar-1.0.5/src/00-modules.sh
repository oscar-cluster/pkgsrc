#
# Copyright (c) 2002 The Trustees of Indiana University.  
#                    All rights reserved.
# Copyright (c) 2014 CEA Commissariat a l'Energie Atomique.
#                    All rights reserved.
#
# This file is part of the modules-oscar software package.  For
# license information, see the LICENSE file in the top-level directory
# of the modules-oscar source distribution.
#
# $Id: 00-modules.sh,v 2.0 2014/03/12 10:51:51 olahaye74 Exp $
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

    # Init environment modules
    if test "$MODULE_VERSION" = ""; then
        shell=`/bin/basename \`/bin/ps -p $$ -ocomm=\``
        if [ -f /usr/share/Modules/init/$shell ]
        then
            . /usr/share/Modules/init/$shell
        else
            . /usr/share/Modules/init/sh
        fi

    fi
    #----------------------------------------------------------------------#
    # set this if $shell exists on your system and to use it
    # instead of sh - so per-process dot files will be sourced.
    #----------------------------------------------------------------------#

    if test "$shell" !="sh"; then
        sh() { $shell "$@"; }
    fi

    # Now load all the OSCAR modules

    module load oscar-modules
fi

# All done.  Do *NOT* exit here!  Bash will exit the calling script if
# we "exit" here.
