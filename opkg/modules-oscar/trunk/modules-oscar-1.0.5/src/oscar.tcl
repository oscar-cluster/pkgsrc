#%Module -*- tcl -*-
#
# This file is part of the modules OSCAR RPM software package.
#
# See the LICENSE.OSCAR file in the top-level directory for Copyright
# notices.
#
# $Id: oscar.tcl,v 1.8 2004/03/08 17:36:48 jsquyres Exp $
#

proc ModulesHelp { } {
  puts stderr "\tThis module sets up the OSCAR modules subsystem."
}

module-whatis	"Sets up the OSCAR modules subsystem."

# Need to do some things differently if we're loading vs. unloading

set omdir /opt/modules/oscar-modulefiles
set am_removing [module-info mode remove]

# Tell modules to use the datadir, where the directory tree containing
# all the OSCAR-related modulesfiles live.  Only do this if we're not
# unloading this modulefile.  This is because upon unload, if we
# "unuse" this directory and then try to "module unload" the files
# below, it won't work.  We'll unuse this directory if we're unloading
# *after* we do all the unload of individual modules.

if { ! $am_removing } {
  module use $omdir
}

# Set the MANPATH to have just a ":" in it.  This is undocumented man
# behavior that if $MANPATH begins with :, "man foo" will first look
# for a foo man page in the directories listed in /etc/man.config and
# then in the directories specified by $MANPATH.  So putting : at the
# front of MANPATH allows other modules to append to the MANPATH with
# their own directories.

setenv MANPATH :/opt/modules/default/man

# Load all the modules in that tree.  We have to call "module" for
# each one rather than building up a list of them and then running
# "module load $all_the_modules" because we'd have to pipe that
# through eval so that "$all_the_modules" expands into multiple argv
# arguments.  But eval will expand meta characters, and potentially
# run commands.  So instead, we run "module load" for each directory
# in $omdir.

foreach modulefile [glob -nocomplain $omdir/*] { 
  module load [file tail $modulefile] 
}

# Per the note above, only unuse the directory if we're unloading.
# Note that we leave it "use" (vs. "unuse") because when we're
# unloading, the modules infrastructure will automatically change the
# meaning of "use" to "unuse".

if { $am_removing } {
  module use $omdir
}
