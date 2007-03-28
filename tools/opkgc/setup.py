#!/usr/bin/python

###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@inria.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

###################################################################
#
# Description:
#   Install opkgc
#
###################################################################

from distutils.core import setup

setup(name="opkgc",
      version="1.0",
      description="OSCAR package compiler.",
      long_description="""
opkgc transform an OSCAR package descriptor in a XML file into a
set of RPM or Debian packages.""",
      license="GNU GPL",
      author="Jean Parpaillon",
      author_email="jean.parpaillon@irisa.fr",
      url="http://oscar.openclustergroup.org/comp_opkgc",
      scripts = ['opkgc'],
      packages = ['Opkgc'],
      data_files = [('share/opkgc', ['doc/opkg.xsd']),
                    ('share/opkgc/tmpl/debian',
                     ['templates/debian/changelog.xslt',
                      'templates/debian/compat',
                      'templates/debian/control.xslt',
                      'templates/debian/copyright',
                      'templates/debian/rules',
                      'templates/opkg-core-spec.xsl']),
                    ('share/opkgc/tmpl', ['templates/opkg-core-spec.xsl']),
                    ('/etc', ['conf/opkgc.conf'])]
     )
