###################################################################
# Copyright (c) 2007 Oak Ridge National Laboratory,
#                    Geoffroy Vallee <valleegr@ornl.gov>
#                    All rights reserved
# Copyright (c) 2007 IRISA-INRIA
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

from lxml import etree
from StringIO import StringIO
from OpkgcConfig import *

class XmlTools:
    __instance = None
    __xml_doc = None
    __xmlschema_doc = None

    def __new__ (cls):
        if cls.__instance is None:
            cls._instance = object.__new__(cls)
        return cls._instance
    
    def init (self, xml_file):
        self.__xml_doc = self.parseXml(xml_file)
        self.__xmlschema_doc = self.parseXml(Config().getValue("xsdPath"))

    def transform (self, xsl_file, output_file):
        # we parse then the XSLT file
        xsl_doc = self.parseXml(xsl_file)

        transform = etree.XSLT(xsl_doc)

        # We apply then the XSLT transformation to the XML doc
        result = transform(self.__xml_doc)
        
        # We open the output file
        output = open (output_file, "w")
        output.write(str(result))
        output.close()

    def validate (self):
        xmlschema = etree.XMLSchema(self.__xmlschema_doc)
        if not xmlschema.validate(self.__xml_doc):
            print "Input file invalid. Check against XML schema " + Config().getValue("xsdPath")
            raise SystemExit

    def parseXml (self, file):
        xml_file = open(file)
        parser_xml = etree.XMLParser()
        xml_doc = etree.parse(xml_file, parser_xml)
        xml_file.close()
        return xml_doc

    def getXmlDoc(self):
        return self.__xml_doc
