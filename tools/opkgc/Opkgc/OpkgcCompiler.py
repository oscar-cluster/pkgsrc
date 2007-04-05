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
from OpkgcTools import *
from Cheetah.Template import Template

__all__ = ['Compiler', 'CompilerRpm', 'CompilerDebian']

class Compiler:
    """ Generic class for compiling config.xml
    """
    config = None

    dest_dir = ''
    validate = True
    inputdir = ''
    dist = ''
    
    xml_tool = XmlTools()

    def __init__(self, inputdir, dest_dir, dist, validate):
        self.dest_dir = dest_dir
        self.validate = validate
        self.inputdir = inputdir
        self.dist = dist

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

    def getScripts(self):
        """ Return list of files in scripts/ dir
        """
        ret = []
        scriptdir = os.path.join(self.inputdir, "scripts")
        if os.path.isdir(scriptdir):
            for p in os.listdir(scriptdir):
                if not re.search("\.svn|.*~", p) and not os.path.isdir(p):
                    ret.append(os.path.join(self.inputdir, "scripts", p))
        return ret

    def SupportedDist(cls):
        """ Return a list of supported dist
        """
        return cls.supportedDist
    SupportedDist = classmethod(SupportedDist)

    def SupportDist(cls, dist):
        """ Return true if the class support 'dist'
        """
        return dist in cls.supportedDist
    SupportDist = classmethod(SupportDist)
        
class CompilerRpm(Compiler):
    """ Extend Compiler for RPM packaging
    """
    supportedDist = ['fc', 'rhel']

    def compile (self):
        self.xmlInit (self.getConfigFile())
        self.xmlValidate ()

        dest = 'opkg' + '-' + self.getPackageName() + '.spec'

        self.xmlCompile(
            Config().get("RPM", "templatefile"),
            os.path.join(self.getDestDir(), dest),
            {"distrib":self.dist})

    def build(self):
        rpmCmd = Config().get("RPM", "buildcmd")
        rpmOpts = Config().get("RPM", "buildopts")

        ret = os.system(rpmCmd + ' ' + rpmOpts)

class CompilerDebian(Compiler):
    """ Extend Compiler for Debian packaging
    """
    supportedDist = ['debian']
    pkgDir = ''
    scriptsOrigDest = {'api-pre-install':       'opkg-api-%s.preinst',
                       'server-post-install':   'opkg-server-%s.postinst',
                       'server-post-uninstall': 'opkg-server-%s.postrm',
                       'client-post-install':   'opkg-client-%s.postinst',
                       'client-post-uninstall': 'opkg-client-%s.postrm'}

    def compile (self):
        """ Creates debian package files
        """
        self.xmlInit (self.getConfigFile())
        self.xmlValidate ()

        desc = OpkgDescriptionDebian(self.xml_tool.getXmlDoc())

        pkgName = self.filterPackageName(self.getPackageName())
        self.pkgDir = os.path.join(self.getDestDir(), "opkg-%s" % pkgName)

        if (os.path.exists(self.pkgDir)):
            Tools.rmDir(self.pkgDir)

        debiandir = os.path.join(self.pkgDir, 'debian')
        os.makedirs(debiandir)

        # Compile template files
        for template in self.getTemplates():
            if re.search("\.tmpl", template):
                (head, tail) = os.path.split(template)
                (base, ext) = os.path.splitext(tail)
                self.cheetahCompile(desc, template, os.path.join(debiandir, base))
            else:
                shutil.copy(template, debiandir)

        # Manage [pre|post]-scripts
        for orig in self.getScripts():
            basename = os.path.basename(orig)
            try:
                # If script is one of scripts included as
                # {pre|post}{inst|rm} scripts, copy with appropriate filename
                # (see Debian Policy for it)
                dest = self.scriptsOrigDest[basename] % pkgName
                shutil.copy(orig, os.path.join(debiandir, dest))
            except(KeyError):
                # else, file is packaged in /var/lib/oscar/packages/<packages>/
                filelist = open(os.path.join(debiandir, "opkg-api-%s.install" % pkgName), "a")
                filelist.write("%s /var/lib/oscar/packages/%s\n" % (basename, pkgName))
                filelist.close()
                shutil.copy(orig, self.pkgDir)

        # Copy doc
        docdir = os.path.join(self.inputdir, "doc")
        if os.path.isdir(docdir):
            Tools.copy(docdir,
                       self.pkgDir,
                       True,
                       '\.svn|.*~')
            filelist = open(os.path.join(debiandir, "opkg-api-%s.install" % pkgName), "a")
            filelist.write("doc/* /usr/share/doc/opkg-api-%s\n" % pkgName)
            filelist.close()

        # Copy testing scripts
        testdir = os.path.join(self.inputdir, "testing")
        if os.path.isdir(testdir):
            Tools.copy(testdir,
                       self.pkgDir,
                       True,
                       '\.svn|.*~')
            filelist = open(os.path.join(debiandir, "opkg-api-%s.install" % pkgName), "a")
            filelist.write("testing/* /var/lib/oscar/testing/%s\n" % pkgName)
            filelist.close()

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
            f = os.path.join(Config().get("DEBIAN", "templatedir"), p)
            if not re.search("\.svn|.*~", p) and not os.path.isdir(f):
                ret.append(f)
        return ret

    def filterPackageName(self, s):
        """ Filter s to comply with Debian package name syntax
        """
        p = re.compile(r'[^a-zA-Z0-9-]')
        return p.sub('-', s)
