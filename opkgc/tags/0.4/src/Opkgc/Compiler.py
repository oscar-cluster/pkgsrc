###################################################################
# Copyright (c) 2007 Kerlabs
#                    Jean Parpaillon <jean.parpaillon@kerlabs.com>
#                    All rights reserved
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
import tempfile
from Config import *
from Tools import *
from Logger import *

from OpkgDescription import *
from PkgDescription import *
from Rpm import *
from Deb import *

class Compiler:
    """ main class for compiling for opgks
    """
    compilers = ['RPMCompiler', 'DebCompiler']
    
    config = None

    dest_dir = ''
    inputdir = ''
    dist = ''
    pkgName = None
    
    def __init__(self, inputdir, dest_dir, dist):
        self.dest_dir = dest_dir
        self.inputdir = inputdir
        self.dist = dist

    def compile (self, targets):
        """ Compile opkg
        targets: list of targets amongst: 'binary', 'source'
        """
        opkgDesc = OpkgDescription(self.inputdir)

        self.pkgName = opkgDesc.getPackageName()
        Logger().debug("Package name: %s" % self.pkgName)

        # Check if package is available on target dist
        if not opkgDesc.checkDist(self.dist):
            Logger().info("Package '%s' is not available on distribution '%s'" % (self.pkgName, self.dist))
            raise SystemExit

        tarfile = self.createTarball(opkgDesc)
        Logger().info("opkg tarball created: %s" % tarfile)

        for c in self.compilers:
            if self.dist in eval(c).supportedDist:
                dc = eval(c)(opkgDesc, self.dest_dir, self.dist)
                dc.run(tarfile, targets)
        
    def createTarball(self, opkgDesc):
        """ Create a tarball from opkg sources.
        Return: path to the tarball
        """
        tempdir = tempfile.mkdtemp('.opkgc')
        sourcedir = "opkg-%s-%s" % (self.pkgName, opkgDesc.getVersion('upstream'))
        tardir = os.path.join(tempdir, sourcedir)
        tarname = os.path.join(self.dest_dir, "%s.tar.gz" % sourcedir)
        
        os.mkdir(tardir, 0755)
        filelist = [ os.path.join(opkgDesc.opkgdir, f)
                     for f in Tools.ls(opkgDesc.opkgdir, exclude='SRPMS|distro') ]
        Tools.copy(filelist, tardir, exclude='\.svn|.*~$')

        Tools.tar(tarname, [sourcedir], tempdir)
        Logger().debug("Delete temp dir: %s" % tempdir)
        Tools.rmDir(tempdir)
        
        return tarname

    def SupportDist(cls, dist):
        """ Return true if dist is supported 'dist'
        """
        for c in cls.compilers:
            if dist in eval(c).supportedDist:
                return True
        return False
    SupportDist = classmethod(SupportDist)

class RPMCompiler:
    """ RPM Compiler
    """
    opkgDesc = None
    opkgName = None
    dist = None
    configSection = "RPM"
    supportedDist = ['fc', 'rhel', 'mdv', 'suse', 'ydl']
    buildCmd = "rpmbuild"

    def __init__(self, opkgDesc, dest_dir, dist):
        self.opkgDesc = opkgDesc
        self.opkgName = opkgDesc.getPackageName()
        self.dist = dist

    def run(self, tarfile, targets):
        # Create SOURCES dir and copy opkg tarball to it
        sourcedir = self.getMacro('%{_sourcedir}')
        if not os.path.exists(sourcedir):
            os.makedirs(sourcedir)
        Logger().debug("Copying %s to %s" % (tarfile, sourcedir))
        shutil.copy(tarfile, sourcedir)

        # Create SPECS dir and create spec file
        specdir = self.getMacro('%_specdir')
        if not os.path.exists(specdir):
            os.makedirs(specdir)

        specfile = os.path.join(specdir, "opkg-%s.spec" % self.opkgName)
        if os.path.exists(specfile):
            os.remove(specfile)

        specfile = os.path.join(self.getMacro('%_specdir'), "opkg-%s.spec" % self.opkgName)
        Tools.cheetahCompile(
            RpmSpec(self.opkgDesc, self.dist),
            os.path.join(Config().get(self.configSection, "templatedir"), "opkg.spec.tmpl"),
            specfile)

        # Build targets
        if 'source' in targets:
            ret = Tools.command("%s --clean -bs %s" % (self.buildCmd, specfile), "./")
            if ret == 0:
                Logger().info("Source package succesfully generated in %s" % self.getMacro('%_srcrpmdir'))
            else:
                Logger().error("Source package generation failed: return %d" % ret)
                raise SystemExit(1)
            
        if 'binary' in targets:
            bindir = os.path.join(self.getMacro('%_rpmdir'), "noarch")
            ret = Tools.command("%s --clean -bb %s" % (self.buildCmd, specfile), "./")
            if ret == 0:
                Logger().info("Binary package succesfully generated in %s" % bindir)
            else:
                Logger().error("Binary package generation failed: return %d" % ret)
                raise SystemExit(1)
            
    def getMacro(self, name):
        line = os.popen("rpm --eval %s" % name).readline()
        return line.strip()

class DebCompiler:
    """ Extend Compiler for Debian packaging
    """
    opkgDesc = None
    dest_dir = None
    opkgName = None
    configSection = "DEBIAN"
    buildCmd = "dpkg-buildpackage"
    supportedDist = ['debian']

    def __init__(self, opkgDesc, dest_dir, dist):
        self.opkgDesc = opkgDesc
        self.dist = dist
        self.dest_dir = dest_dir
        self.opkgName = opkgDesc.getPackageName()

    def run(self, tarfile, targets):
        sourcedir = os.path.join(self.dest_dir,
                                 "opkg-%s-%s" % (self.opkgName, self.opkgDesc.getVersion('upstream')))
        # Rename tar to follow Debian non-native package rule
        debtarfile = "opkg-%s_%s.orig.tar.gz" % (self.opkgName, self.opkgDesc.getVersion('upstream'))
        os.rename(tarfile, debtarfile)
        
        # Uncompress tar
        if os.path.exists(sourcedir):
            Tools.rmDir(sourcedir)
        Tools.untar(debtarfile, self.dest_dir)

        # Create debian dir
        debiandir = os.path.join(sourcedir, "debian")
        os.makedirs(debiandir)

        # Compile template files
        debDesc = DebDescription(self.opkgDesc, self.dist)
        templateDir = os.path.abspath(Config().get(self.configSection, "templatedir"))
        tmplList = [ os.path.join(templateDir, t)
                     for t in Tools.ls(templateDir) ]
        Logger().debug("Templates: %s" % tmplList)
        for template in tmplList:
            if re.search("\.tmpl", template):
                (head, tail) = os.path.split(template)
                (base, ext) = os.path.splitext(tail)
                Tools.cheetahCompile(debDesc, template, os.path.join(debiandir, base))
            else:
                shutil.copy(template, debiandir)
                Logger().info("Copy %s to %s" % (template, debiandir))

        for part in ['api', 'server', 'client']:
            fl = debDesc.getPackageFiles(part)
            installFile = os.path.join(debiandir, debDesc.getInstallFile(part))
            filelist = open(installFile, "a")
            for f in fl:
                filelist.write("%s /%s/\n" % (f['sourcedest'], f['dest']))
            filelist.close()

        # Build targets
        cmd = "%s -rfakeroot -sa" % self.buildCmd
        if 'source' in targets and 'binary' in targets:
            opts = ""
        elif 'source' in targets:
            opts = "-S"
        elif 'binary' in targets:
            opts = "-B"

        ret = Tools.command("%s %s" % (cmd, opts), sourcedir)
        if ret == 0:
            Logger().info("Packages succesfully generated")
        else:
            Logger().error("Packages generation failed: return %d" % ret)
            raise SystemExit(1)
