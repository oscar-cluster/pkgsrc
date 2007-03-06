###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import sys
import os
import re
import shutil
import exceptions
from OpkgcXslt import *

__all__ = ['Compiler', 'CompilerRpm', 'CompilerDebian']

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

    def xsltTransform(self, template, orig, dest):
        """ Transform 'orig' to 'dest' with XSLT template 'template'
        
        'template' is a XSLT file
        """
        xslt_transformator = XSLT_transform (orig, os.path.join (self.getTemplateDir(), template), dest)

    def rmDir(self, d):
        """ Remove recursively a directory, even if not empty, like rm -r
        """
        for p in os.listdir(d):
            if os.path.isdir(os.path.join(d,p)):
                cleandir(os.path.join(d,p))
            else:
                os.remove(os.path.join(d,p))
        os.rmdir(os.path.join(d))

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
        self.xsltTransform(self.__template, file, os.path.join(self.getDestDir(), self.__dest))

    def build(self):
        print "Not yet implemented"

class CompilerDebian(Compiler):
    """ Extend Compiler for Debian packaging
    """

    __deb_dir = 'debian'
    __pkg_dir = 'opkg'

    def compile(self, file):
        """ Creates debian package files
        """
        debiandir = os.path.join(self.getDestDir(), self.__pkg_dir, 'debian')
        if (os.path.exists(debiandir)):
            self.rmDir(debiandir)
        os.makedirs(debiandir)

        for template in self.getTemplates():
            if re.search("\.xslt", template):
                (head, tail) = os.path.split(template)
                (base, ext) = os.path.splitext(tail)
                self.xsltTransform(template, file, os.path.join(debiandir, base))
            else:
                shutil.copy(template, debiandir)

    def build(self):
        print "Not yet implemented"

    def getTemplates(self):
        """ Return list of files in Debian templates dir
        """
        ret = []
        for p in os.listdir(os.path.join(self.getTemplateDir(), self.__deb_dir)):
            if not re.search("\.svn", p):
                ret.append(os.path.join(self.getTemplateDir(), self.__deb_dir, p))
        return ret
