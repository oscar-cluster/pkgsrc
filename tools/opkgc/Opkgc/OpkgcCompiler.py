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
from OpkgcXml import *
from OpkgcConfig import *

__all__ = ['Compiler', 'CompilerRpm', 'CompilerDebian']

class Compiler:
    """ Generic class for compiling config.xml
    """
    __config = None

    __xml_tool = None
    __dest_dir = ''
    __validate = True

    def __init__(self, dest_dir, validate):
        self.__dest_dir = dest_dir
        self.__xml_tool = XmlTools()
        self.__validate = validate

    def getDestDir(self):
        return self.__dest_dir

    def xmlInit(self, orig):
        self.__xml_tool.init (orig)

    def xmlValidate(self):
        if self.__validate:
            self.__xml_tool.validate()

    def xmlCompile(self, template, dest):
        """ Transform 'orig' to 'dest' with XSLT template 'template'
        
        'template' is a XSLT file
        """
        self.__xml_tool.transform (os.path.join (Config().getValue("templateDir"), template), dest)

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
        self.xmlInit (file)
        self.xmlValidate ()
        self.xmlCompile(self.__template, os.path.join(self.getDestDir(), self.__dest))

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
        self.xmlInit (file)
        self.xmlValidate ()

        debiandir = os.path.join(self.getDestDir(), self.__pkg_dir, 'debian')
        if (os.path.exists(debiandir)):
            self.rmDir(debiandir)
        os.makedirs(debiandir)

        for template in self.getTemplates():
            if re.search("\.xslt", template):
                (head, tail) = os.path.split(template)
                (base, ext) = os.path.splitext(tail)
                self.xmlCompile(template, os.path.join(debiandir, base))
            else:
                shutil.copy(template, debiandir)

    def build(self):
        print "Not yet implemented"

    def getTemplates(self):
        """ Return list of files in Debian templates dir
        """
        ret = []
        for p in os.listdir(os.path.join(Config().getValue("templateDir"), self.__deb_dir)):
            if not re.search("\.svn", p):
                ret.append(os.path.join(Config().getValue("templateDir"), self.__deb_dir, p))
        return ret
