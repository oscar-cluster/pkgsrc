###################################################################
# Copyright (c) 2007 Oak Ridge National Laboratory,
#                    Geoffroy Vallee <valleegr@ornl.gov>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import sys
import getopt
from xsltprocessor import *

def main():
    opts, args = getopt.getopt(sys.argv[1:], "h", ["help"]) 
    config_file = str(args[0])
    print "OPKG description file: " + config_file
    # We apply transformation for the generation of the spec file
    xslt_transformator = XSLT_transform (config_file,
        "../xslt-doc/opkg-core-spec.xsl",
        "./test.spec")
if __name__ == "__main__":
    main()
