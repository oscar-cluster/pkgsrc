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

    __template_dir = ''
    __dest_dir = ''

    def __init__(self, dest_dir, template_dir):
        self.__dest_dir = dest_dir
        self.__template_dir = template_dir

    def getDestDir(self):
        return self.__dest_dir

    def getTemplateDir(self):
        return self.__template_dir

    def transform(self, template, orig, dest):
        """ Transform 'orig' to 'dest' with template 'template'
        
        'template' is a XSLT file
        """
        print "Generate '" + dest + "' with template '" + template + "'"
        xslt_transformator = XSLT_transform (orig, self.getTemplateDir() + template, dest)

    def compile(self, file):
        """ Abstract method to generate packaging files
        """
        raise NotImplementedError

    def build(self):
        """ Abstract method to build 
        """
        raise NotImplementedError

class CompilerRpm(Compiler):
    """ Extend Compiler for RPM packaging
    """

    __template = 'opkg-core-spec.xsl'
    __dest = 'test.spec'

    def compile(self, file):
        self.transform(self.__template, file, self.getDestDir() + "/" + self.__dest)

    def build(self):
        print "Not yet implemented"
