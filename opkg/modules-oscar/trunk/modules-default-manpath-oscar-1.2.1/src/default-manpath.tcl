#%Module -*- tcl -*-
#
# This modulefile exists because of the way "man" gets its path.
# if $MANPATH is set, it ignored /etc/man.config, which is where all
# the default man paths are set.  There's an undocumented feature of
# man in that if you have ":" at the beginning or end (or, more specifically,
# any path in $MANPATH is empty), it'll read the MANPATH entries from
# /etc/man.config.  However, the modules commands "append-path" and 
# "prepend-path" are too smart -- they won't let us append or prepend
# an empty path.  
#
# So the solution we devised is to have a new modulefile that reads
# in the MANPATH entries from /etc/man.config and add them here
# with append-path.  Then, if the user doesn't want them, they can
# just unload this module.
#

proc ModulesHelp { } {
  puts stderr "\tThis module adds in the default MANPATH entries"
  puts stderr "\tto the $MANPATH environment variable from /etc/man_db.conf."
}

module-whatis   "Add default entries to the MANPATH environment variable"

# Read in /etc/man.config, find all MANPATH entires

# Backup current MANPATH and unset MANPATH
set MANPATH_BACKUP ""
if [info exists ::env(MANPATH) ] {
    set MANPATH_BACKUP ::$env(MANPATH)
    unset ::env(MANPATH)
}

# Get the default MANPATH using manpath command.
#set system_path [exec /usr/bin/manpath -q]
# OL => -q switch not available on old manpath commands.
set system_path [exec /usr/bin/manpath i2> /dev/null]

# Restore MANPATH
set ::env(MANPATH) $MANPATH_BACKUP

# Append default MANPATH.
foreach path [split $system_path :] {
	if {[string length $path] > 0} {
		append-path MANPATH $path
	}
}

