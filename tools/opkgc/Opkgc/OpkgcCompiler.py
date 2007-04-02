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
    config = None

    dest_dir = ''
    validate = True
    inputdir = ''

    xml_tool = XmlTools()

    def __init__(self, inputdir, dest_dir, validate):
        self.dest_dir = dest_dir
        self.validate = validate
        self.inputdir = inputdir

    def getDestDir(self):
        return self.dest_dir

    def xmlInit(self, orig):
        self.xml_tool.init (orig)

    def xmlValidate(self):
        if self.validate:
            self.xml_tool.validate()

    def getXmlDoc(self):
        return self.xml_tool.getXmlDoc()

    def xmlCompile(self, template, dest, params):
        """ Transform 'orig' to 'dest' with XSLT template 'template'
        'template' is a XSLT file
        'params' is a dictionnary with params to give to the template
        """
        self.xml_tool.transform (template, dest, params)

    def cheetahCompile(self, orig, template, dest):
        """ Transform 'orig' to 'dest' with Cheetah template 'template'
        
        'template' is a XSLT file
        """
        t = Template(file=template, searchList=[orig])
        f = open(dest, 'w')
        f.write(t.respond())
        f.close()

    def compile(self):
        """ Abstract method to generate packaging files
        """
        raise NotImplementedError

    def build(self):
        """ Abstract method to build 
        """
        raise NotImplementedError

    def getPackageName(self):
        return self.xml_tool.getXmlDoc().find('/name').text.lower()

    def getConfigFile(self):
        """ Return path of config.xml file
        Raise exception if not found
        """
        path = os.path.join(self.inputdir, "config.xml")
        if os.path.exists(path):
            return path
        else:
            print "No config.xml file found. Either:"
            print "* specify the --input=dir option"
            print "* run opkgc from the opkg directory"
            raise SystemExit

class CompilerRpm(Compiler):
    """ Extend Compiler for RPM packaging
    """

    def compile (self):
        self.xmlInit (self.getConfigFile())
        self.xmlValidate ()

        dest = 'opkg' + '-' + self.getPackageName() + '.spec'

        self.xmlCompile(
            Config().get("RPM", "templatefile"),
            os.path.join(self.getDestDir(), dest),
            {"distrib":"rhel"})

    def build(self):
        rpmCmd = Config().get("RPM", "buildcmd")
        rpmOpts = Config().get("RPM", "buildopts")

        ret = os.system(rpmCmd + ' ' + rpmOpts)

class CompilerDebian(Compiler):
    """ Extend Compiler for Debian packaging
    """

    pkgDir = ''

    def compile (self):
        """ Creates debian package files
        """
        self.xmlInit (self.getConfigFile())
        self.xmlValidate ()

        desc = OpkgDescriptionDebian(self.xml_tool.getXmlDoc())

        self.pkgDir = 'opkg' + '-' + self.getPackageName()

        debiandir = os.path.join(self.getDestDir(), self.pkgDir, 'debian')
        if (os.path.exists(debiandir)):
            Config().rmDir(debiandir)
        os.makedirs(debiandir)

        for template in self.getTemplates():
            if re.search("\.tmpl", template):
                (head, tail) = os.path.split(template)
                (base, ext) = os.path.splitext(tail)
                self.cheetahCompile(desc, template, os.path.join(debiandir, base))
            else:
                shutil.copy(template, debiandir)

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
