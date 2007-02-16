###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import sys
import exceptions
from OpkgcXslt import *

__all__ = ['Compiler', 'CompilerRpm']

class Compiler:
    """ Generic class for compiling config.xml
    """

    __template_dir = '../xslt-doc/'

    def transform(self, template, orig, dest):
        """ Transform 'orig' to 'dest' with template 'template'
        
        'template' is a XSLT file
        """
        print "Generate '" + dest + "' with template '" + template + "'"
        xslt_transformator = XSLT_transform (orig, self.__template_dir + template, dest)

    def compile(self, file):
        """ Abstract method to generate packaging files
        """
        raise NotImplementedError

    def build(self):
        """ Abstract method to build 
        """
        raise NotImplementedError

class CompilerRpm(Compiler):
    """ Implement Dist for RPM packaging
    """

    __template = 'opkg-core-spec.xsl'
    __dest = 'test.spec'
    __dest_dir = ''

    def __init__(self, dest_dir):
        self.__dest_dir = dest_dir

    def compile(self, file):
        self.transform(self.__template, file, self.__dest_dir + "/" + self.__dest)

    def build(self):
        print "Not yet implemented"
