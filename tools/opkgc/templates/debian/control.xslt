<?xml version="1.0" encoding="ISO-8859-1" ?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method ="text" encoding="us-ascii" />

<xsl:template name="depends" >
  <xsl:param name="group" select="oscar_api" />
  
  <xsl:for-each select="binary-package-list/pkg">
    <xsl:if test="../filter/distribution/name = 'debian' or not(../filter/distribution)" >
      <xsl:choose>
	<xsl:when test="../filter/group = $group" >
	  <xsl:value-of select="." /><xsl:text>, </xsl:text> 
	</xsl:when>
      </xsl:choose>
    </xsl:if>
  </xsl:for-each>

</xsl:template>

<xsl:template match="/">

<xsl:variable name="lcletters">abcdefghijklmnopqrstuvwxyz</xsl:variable>
<xsl:variable name="ucletters">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>

<xsl:for-each select="oscar">

<xsl:variable name="name" ><xsl:value-of select="name" /></xsl:variable>
<xsl:variable name="namelc" ><xsl:value-of select="translate($name, $ucletters, $lcletters)"/></xsl:variable>

Source: opkg-<xsl:value-of select="$namelc"/>
Section: admin
Priority: optional
Maintainer: Open Cluster Group &lt;http://OSCAR.OpenClusterGroup.org/&gt;
Uploaders: <xsl:value-of select="packager/name" /> &lt;<xsl:value-of select="packager/email" />&gt;
Build-Depends: debhelper (>= 5)
Standards-Version: 3.7.2

Package: opkg-api-<xsl:value-of select="$namelc"/>
Architecture: all
Depends: <xsl:call-template name="depends" />
Description: <xsl:value-of select="summary"/>, API part

Package: opkg-server-<xsl:value-of select="$namelc"/>
Architecture: all
Depends: <xsl:call-template name="depends" ><xsl:with-param name="group" >oscar_server</xsl:with-param></xsl:call-template>
Description: <xsl:value-of select="summary"/>, server part

Package: opkg-client-<xsl:value-of select="$namelc"/>
Architecture: all
Depends: <xsl:call-template name="depends" ><xsl:with-param name="group" >oscar_clients</xsl:with-param></xsl:call-template>
Description: <xsl:value-of select="summary"/>, client part

</xsl:for-each>

</xsl:template>
</xsl:stylesheet>
