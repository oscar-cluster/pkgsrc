###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# Copyright (c) 2007 Oak Ridge National Laboratory
#                    Geoffroy Vallee <valleegr@ornl.gov>
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
from OpkgDescription import *
from Cheetah.Template import Template

__all__ = ['Compiler', 'CompilerRpm', 'CompilerDebian']

class Compiler:
    """ Generic class for compiling config.xml
    """
    
    filter_xslt_file = "/tmp/opkgc/param.xsl"

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

    def getXmlDoc(self):
        return self.xml_tool.getXmlDoc()

    def xmlCompile(self, template, dest):
        """ Transform 'orig' to 'dest' with XSLT template 'template'
        
        'template' is a XSLT file
        """
        self.xml_tool.transform (template, dest)

    def cheetahCompile(self, orig, template, dest):
        """ Transform 'orig' to 'dest' with Cheetah template 'template'
        
        'template' is a XSLT file
        """
        t = Template(file=template, searchList=[orig])
        f = open(dest, 'w')
        f.write(t.respond())
        f.close()

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

    def getPackageName(self):
        return self.xml_tool.getXmlDoc().find('/name').text.lower()

    def genXSLTParam(self, distrib):
        try:
            file = open(self.filter_xslt_file,'w')
            file.write ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> \n")
            file.write ("<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n")
            file.write ("<xsl:output method =\"text\" encoding=\"us-ascii\" /><xsl:variable name=\"distrib\">")
            file.write (distrib)
            file.write ("</xsl:variable>\n")
            file.write ("</xsl:stylesheet>")
            file.close ();
        except:
            print "Impossible to generate the file for distribution specification " + self.filter_xslt_file
            sys.exit(2)

    def deleteXSLTParam(self):
        os.remove (self.filter_xslt_file)

class CompilerRpm(Compiler):
    """ Extend Compiler for RPM packaging
    """

    def compile(self, file):
        self.xmlInit (file)
        self.xmlValidate ()
        self.genXSLTParam ("rhel")

        dest = 'opkg' + '-' + self.getPackageName() + '.spec'

        self.xmlCompile(
            Config().get("RPM", "templatefile"),
            os.path.join(self.getDestDir(), dest))

        self.deleteXSLTParam ()

    def build(self):
        rpmCmd = Config().get("RPM", "buildcmd")
        rpmOpts = Config().get("RPM", "buildopts")

        ret = os.system(rpmCmd + ' ' + rpmOpts)

class CompilerDebian(Compiler):
    """ Extend Compiler for Debian packaging
    """

    pkgDir = ''

    def compile(self, file):
        """ Creates debian package files
        """
        self.xmlInit (file)
        self.xmlValidate ()

        self.genXSLTParam ("debian")

        desc = OpkgDescriptionDebian(self.xml_tool.getXmlDoc())

        self.pkgDir = 'opkg' + '-' + self.getPackageName()

        debiandir = os.path.join(self.getDestDir(), self.pkgDir, 'debian')
        if (os.path.exists(debiandir)):
            self.rmDir(debiandir)
        os.makedirs(debiandir)

        for template in self.getTemplates():
            if re.search("\.tmpl", template):
                (head, tail) = os.path.split(template)
                (base, ext) = os.path.splitext(tail)
                self.cheetahCompile(desc, template, os.path.join(debiandir, base))
            else:
                shutil.copy(template, debiandir)

        self.deleteXSLTParam ()

    def build(self):
        cdCmd = 'cd ' + self.pkgDir
        dpkgCmd = Config().get("DEBIAN", "buildcmd")
        dpkgOpts = Config().get("DEBIAN", "buildopts")

        ret = os.system(cdCmd + ';' + dpkgCmd + ' ' + dpkgOpts)

    def getTemplates(self):
        """ Return list of files in Debian templates dir
        """
        ret = []
        for p in os.listdir(Config().get("DEBIAN", "templatedir")):
            if not re.search("\.svn", p):
                ret.append(os.path.join(Config().get("DEBIAN", "templatedir"), p))
        return ret
