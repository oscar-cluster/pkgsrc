<?xml version="1.0" encoding="ISO-8859-1" ?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method ="text" encoding="us-ascii" />
<xsl:template match="/">

<xsl:variable name="lcletters">abcdefghijklmnopqrstuvwxyz</xsl:variable>
<xsl:variable name="ucletters">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>

<xsl:for-each select="oscar">

<xsl:variable name="name" ><xsl:value-of select="name" /></xsl:variable>
<xsl:variable name="namelc" ><xsl:value-of select="translate($name, $ucletters, $lcletters)"/></xsl:variable>

<xsl:variable name="version" ></xsl:variable>

opkg-<xsl:value-of select="$namelc"/> (<xsl:value-of select="$version"/>) UNRELEASED; urgency=low

  * Generated from template

 -- <xsl:value-of select="packager/name" /> &lt;<xsl:value-of select="packager/email" />&gt;

</xsl:for-each>

</xsl:template>
</xsl:stylesheet>
