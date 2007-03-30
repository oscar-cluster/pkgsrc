###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

from OpkgcXml import *

__all__ = ['OpkgDescription', 'OpkgDescriptionDebian']

class OpkgDescription:
    """ Describe a config.xml file
    """
    xmldoc = None

    def __init__(self, xmldoc):
        self.xmldoc = xmldoc

    def node(self, path, capitalize=''):
        s = self.xmldoc.findtext(path)
        if capitalize == 'lower':
            return s.lower()
        elif capitalize == 'upper':
            return s.upper()
        else:
            return s

class OpkgDescriptionDebian(OpkgDescription):
    """ Filters out some fields in a opkg description,
        for Debian templates
    """

    archName = {"i386":"i386",
                "amd64":"amd64",
                "x86_64":"ia64"}

    dependsName = {"requires":"Depends",
                   "conflicts":"Conflicts",
                   "provides":"Provides",
                   "suggests":"Suggests"}

    relName = {"<":"<<",
               "<=":">=",
               "=":"=",
               ">=":">=",
               ">":">>"}

    def __init__(self, xmldoc):
        OpkgDescription.__init__(self, xmldoc)

    def description(self):
        """ Return the description in Debian format:
            each line begin with a space
        """
        desc = ''
        t = self.xmldoc.findtext('/description')
        for line in t.split('\n'):
            desc += ' ' + line.strip() + '\n'
        return desc.strip()

    def arch(self):
        """ Return comma-separated list of architectures,
        or 'all' if none.
        """
        alist = self.xmldoc.findall('/filters/arch')
        if len(alist) == 0:
            arch = 'all'
        else:
            i = 0
            arch = ''
            for a in alist:
                if i != 0:
                    arch += ', '
                arch += self.archName[a.text]
                i += 1
        return arch

    def authors(self, type):
        """ Return comma separated list of authors of type 'type'
        """

        # Get filtered list of authors
        alist = [a for a in self.xmldoc.findall("authors/author") if a.get('cat') == type]
        authors = ''
        i = 0
        for a in alist:
            if i != 0:
                authors += ', '
            authors += self.author(a)
            i += 1
        return authors

    def author(self, etree):
        """ Format an author node:
            Name (nickname) <email@site.ext>
        """
        author = etree.findtext('name')
        nickname = etree.findtext('nickname')
        if nickname != None:
            author += ' (%s)' % nickname
        author += ' <%s>' % etree.findtext('email')
        return author
        
    def depends(self, part, relation):
        """ Return list of dependencies of type 'relation' for
        the 'part' package part.
        Relation is one of: requires, conflicts, provides, suggests
        Part is one of: apiDeps, serverDeps, clientDeps
        """
        dlist = [d
                 for d in self.xmldoc.findall(part + '/' + relation)
                 if self.filterDist(d, 'Debian')]
        pkglistlist = [d.findall('pkg') for d in dlist]
        depends = ''
        i = 0
        for pkglist in pkglistlist:
            for pkg in pkglist:
                if i != 0:
                    depends += ', '
                depends += self.pkgDep(pkg)
                i += 1
        ret = ''
        if i == 0:
            return ''
        else:
            return self.dependsName[relation] + ': ' + depends + '\n'

    def filterDist(self, elem, dist):
        """ Return true if 'elem' contains a filter for distribution 'dist'
        or no filters at all.
        """
        distFilters = [d.text.strip() for d in elem.findall('filters/dist')]
        return len(distFilters) == 0 or dist in distFilters

    def pkgDep(self, e):
        """ Return formatted package name plus dependancy relation
        """
        ret = e.text.strip()
        version = e.get('version')
        rel = e.get('rel')
        if version:
            ret += ' (%s %s)' % (self.relName[rel], version)
        return ret

    def changelog(self):
        """ Return a list of versionEntries
        """
        changelog = []
        vEntryNodes = self.xmldoc.findall('/changelog/versionEntry')
        for vEntryNode in vEntryNodes:

            cEntries = []
            cEntryNodes = vEntryNode.findall('changelogEntry')
            for cEntryNode in cEntryNodes:
                items = [i.text.strip() for i in cEntryNode.findall('item')]
                cEntries.append({"items":items,
                                 "name":cEntryNode.get('authorName')})
            
            vEntry = {"version":vEntryNode.get('version'),
                      "uploader":self.uploader(vEntryNode),
                      "logs":cEntries}
            changelog.append(vEntry)

        return changelog

    def uploader(self, versionEntry):
        """ Return the version uploader
        """ 
        name = versionEntry.find('changelogEntry').get('authorName').strip()
        authors = self.xmldoc.findall('authors/author')
        for a in authors:
            if a.findtext('name').strip() == name:
                return self.author(a)
