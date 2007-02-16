#!/usr/bin/python

###################################################################
# Copyright (c) 2007 Oak Ridge National Laboratory,
#                    Geoffroy Vallee <valleegr@ornl.gov>,
#                    All rights reserved
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

###################################################################
#
# Description:
#   Transform config.xml description file for OSCAR packages into
#   Debian or RPM packages.
#
# Requires:
#   python-lxml
#
###################################################################

import sys
import getopt
from OpkgcCompiler import *

def usage():
    """ Print command usage
    """
    print "Usage: " + sys.argv[0] + " [-h] [--build] [--output=dir] --deb --rpm config.xml"
    print ""
    print "\tAt least, one of these option must be enabled:"
    print "\t--deb    : output Debian packaging files"
    print "\t--rpm    : output RPM packaging files"
    print "\t--build  : build packages"

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hdrbo:", ["help", "deb", "rpm", "build", "output="])
    except getopt.GetoptError:
        # Print help information
        usage()
        sys.exit(2)

    # Default values
    debian = False
    rpm = False
    build = False
    output = "."
    config_file = ""
    
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        if o in ("-d", "--deb"):
            debian = True
        if o in ("-r", "--rpm"):
            rpm = True
        if o in ("-b", "--build"):
            build = True
        if o in ("-o", "--output"):
            output = a

    # Other options checking
    if (not debian) and (not rpm):
        usage()
        sys.exit(2)

    config_file = str(args[0])
    if config_file == "":
        usage()
        sys.exit(2)

    targets = []
    print "Source file: " + config_file
#    if debian:
#        targets.append( CompilerDebian() )
    if rpm:
        targets.append( CompilerRpm( output ) )

    for target in targets[:]:
        target.compile( config_file )
        if build:
            target.build()
    
if __name__ == "__main__":
    main()
