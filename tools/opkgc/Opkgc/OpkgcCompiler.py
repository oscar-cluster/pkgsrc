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
    config = None

    dest_dir = ''
    validate = True

    xml_tool = XmlTools()

    def __init__(self, dest_dir, validate):
        self.dest_dir = dest_dir
        self.validate = validate

    def getDestDir(self):
        return self.dest_dir

    def xmlInit(self, orig):
        self.xml_tool.init (orig)

    def xmlValidate(self):
        if self.validate:
            self.xml_tool.validate()

    def xmlCompile(self, template, dest):
        """ Transform 'orig' to 'dest' with XSLT template 'template'
        
        'template' is a XSLT file
        """
        self.xml_tool.transform (template, dest)

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

    template = 'opkg-core-spec.xsl'
    dest = 'test.spec'

    def compile(self, file):
        self.xmlInit (file)
        self.xmlValidate ()
        self.xmlCompile(
            os.path.join(Config().getValue("templateDir"), self.template),
            os.path.join(self.getDestDir(), self.dest))

    def build(self):
        print "Not yet implemented"

class CompilerDebian(Compiler):
    """ Extend Compiler for Debian packaging
    """

    debDir = 'debian'
    pkgDir = 'opkg'

    def compile(self, file):
        """ Creates debian package files
        """
        self.xmlInit (file)
        self.xmlValidate ()

        pkgName = self.xml_tool.getXmlDoc().find('/name').text.lower()
        pkgDir = self.pkgDir+'-'+pkgName

        debiandir = os.path.join(self.getDestDir(), pkgDir, 'debian')
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
        for p in os.listdir(os.path.join(Config().getValue("templateDir"), self.debDir)):
            if not re.search("\.svn", p):
                ret.append(os.path.join(Config().getValue("templateDir"), self.debDir, p))
        return ret
