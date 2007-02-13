###################################################################
# Copyright (c) 2007 Oak Ridge National Laboratory,
#                    Geoffroy Vallee <valleegr@ornl.gov>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

from lxml import etree
from StringIO import StringIO

class XSLT_transform:
    def __init__ (self, xml_file, xsl_file, output_file):
        # we parse first the XML file
        xml_doc = self.parse_xml(xml_file)
        # we parse then the XSLT file
        xsl_doc = self.parse_xml(xsl_file)
        transform = etree.XSLT(xsl_doc)
        # We apply then the XSLT transformation to the XML doc
        result = transform(xml_doc)
        # We open the output file
        output = open (output_file, "w")
        output.write(str(result))
        output.close()
    def parse_xml (self, file):
        xml_file = open(file)
        parser_xml = etree.XMLParser()
        xml_doc = etree.parse(xml_file, parser_xml)
        xml_file.close()
        return xml_doc

