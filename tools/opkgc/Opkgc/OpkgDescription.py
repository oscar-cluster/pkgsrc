###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import re
from OpkgcXml import *

__all__ = ['OpkgDescription', 'OpkgDescriptionDebian']

class OpkgDescription:
    """ Describe a config.xml file
    """
    xmldoc = None
    month = {"01":"Jan", "02":"Feb", "03":"Mar", "04":"Apr",
             "05":"May", "06":"Jun", "07":"Jul", "08":"Aug",
             "09":"Sep", "10":"Oct", "11":"Nov", "12":"Dec"}

    def __init__(self, xmldoc):
        self.xmldoc = xmldoc

    def date(self, date, format):
        """ Convert 'xsdDate' in xsd:dateTime format
        (cf. http://www.w3.org/TR/2004/REC-xmlschema-2-20041028/datatypes.html#dateTime)
        and return in the format specified.
        Format is one of:
        'RFC822'
        """
        p = re.compile(r'^-?(?P<year>[0-9]{4})'
                           r'-(?P<month>[0-9]{2})'
                           r'-(?P<day>[0-9]{2})'
                           r'T(?P<hour>[0-9]{2}):'
                           r'(?P<min>[0-9]{2}):'
                           r'(?P<sec>[0-9]{2})(?P<sfrac>\.[0-9]+)?'
                           r'(?P<tz>((?P<tzs>-|\+)(?P<tzh>[0-9]{2}):(?P<tzm>[0-9]{2}))|Z)?')
        m = p.search(date)
        if format == 'RFC822':
            date = "%s %s %s" % (m.group('day'), self.month[m.group('month')], m.group('year'))
            time = "%s:%s" % (m.group('hour'), m.group('min'))
            if m.group('sec'):
                time += ":%s" % m.group('sec')
            zone = ""
            if m.group('tz'):
                if m.group('tz') == "Z":
                    zone = "Z"
                else:
                    zone = "%s%s%s" % (m.group('tzs'), m.group('tzh'), m.group('tzm'))
            return "%s %s %s" % (date, time, zone)
        else:
            return date

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
        """ Return 'all'
        """
        return "all"

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
                
                closes = cEntryNode.get('closes')
                if closes:
                    for bug in closes.split():
                        items.append("closes: Bug#%s" % bug)
                
                cEntries.append({"items":items,
                                 "name":cEntryNode.get('authorName')})
            
            vEntry = {"version":vEntryNode.get('version'),
                      "uploader":(self.uploader(vEntryNode)),
                      "logs":cEntries}
            changelog.append(vEntry)

        return changelog

    def uploader(self, versionEntry):
        """ Return the version uploader
        """
        cEntry = versionEntry.find('changelogEntry')
        name = cEntry.get('authorName').strip()
        date = cEntry.get('date')
        authors = self.xmldoc.findall('authors/author')
        for a in authors:
            if a.findtext('name').strip() == name:
                return "%s  %s" % (self.author(a), self.date(date, "RFC822"))
