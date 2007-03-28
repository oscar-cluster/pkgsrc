<?xml version="1.0" encoding="ISO-8859-1" ?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method ="text" encoding="us-ascii" />

<xsl:template name="depends" >
  <xsl:param name="path" />
  <xsl:for-each select="$path/pkg[../filters/dist = 'Debian' or not(../filters/dist)]" >
    <xsl:call-template name="addpkg" />
  </xsl:for-each>
</xsl:template>

<xsl:template name="addpkg" >
  <xsl:value-of select="." />
  <xsl:if test="@version" >
    <xsl:text> (</xsl:text>
    <xsl:call-template name="versionrelation" />
    <xsl:text> </xsl:text>
    <xsl:value-of select="@version" />
    <xsl:text>)</xsl:text>
  </xsl:if>
  <xsl:if test="position() != last()" >
    <xsl:text>, </xsl:text>
  </xsl:if>  
</xsl:template>

<xsl:template name="versionrelation" >
  <xsl:choose>
    <xsl:when test="@rel = '&gt;'" >
      <xsl:text>&gt;&gt;</xsl:text>
    </xsl:when>
    <xsl:when test="@rel = '&lt;'" >
      <xsl:text>&lt;&lt;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@rel" />
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="authorslist" >
  <xsl:param name="cat" />

  <xsl:for-each select="authors/author[cat=$cat]" >
    <xsl:value-of select="name" />
    <xsl:if test="nickname" >
      <xsl:text> (</xsl:text><xsl:value-of select="nickname" /><xsl:text>)</xsl:text>
    </xsl:if>
    <xsl:text> &lt;</xsl:text><xsl:value-of select="email" /><xsl:text>&gt;</xsl:text>
    <xsl:if test="position() != last()" >
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template name="arch" >
  <xsl:choose>
    <xsl:when test="filters/arch" >
      <xsl:for-each select="filters/arch" >
	<xsl:value-of select="." />
	<xsl:if test="position() != last()" >
	  <xsl:text>, </xsl:text>
	</xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>all</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
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
Maintainer: <xsl:call-template name="authorslist" ><xsl:with-param name="cat" >maintainer</xsl:with-param></xsl:call-template>
Uploaders: <xsl:call-template name="authorslist" ><xsl:with-param name="cat" >uploader</xsl:with-param></xsl:call-template>
Build-Depends: debhelper (>= 5)
Standards-Version: 3.7.2

Package: opkg-api-<xsl:value-of select="$namelc"/>
Architecture: <xsl:call-template name="arch" />
Depends: <xsl:call-template name="depends" ><xsl:with-param name="path" select="apiDeps/requires" /></xsl:call-template>
Conflicts: <xsl:call-template name="depends" ><xsl:with-param name="path" select="apiDeps/conflicts" /></xsl:call-template>
Provides: <xsl:call-template name="depends" ><xsl:with-param name="path" select="apiDeps/provides" /></xsl:call-template>
Suggests: <xsl:call-template name="depends" ><xsl:with-param name="path" select="apiDeps/suggests" /></xsl:call-template>
Description: <xsl:value-of select="summary"/>, API part
 <xsl:value-of select="description" />
 .
 This package contains the API part of the package.
 .
  <xsl:value-of select="uri" />

Package: opkg-server-<xsl:value-of select="$namelc"/>
Architecture: <xsl:call-template name="arch" />
Depends: <xsl:call-template name="depends" ><xsl:with-param name="path" select="serverDeps/requires" /></xsl:call-template>
Conflicts: <xsl:call-template name="depends" ><xsl:with-param name="path" select="serverDeps/conflicts" /></xsl:call-template>
Provides: <xsl:call-template name="depends" ><xsl:with-param name="path" select="serverDeps/provides" /></xsl:call-template>
Suggests: <xsl:call-template name="depends" ><xsl:with-param name="path" select="serverDeps/suggests" /></xsl:call-template>
Description: <xsl:value-of select="summary"/>, server part
 <xsl:value-of select="description" />
 .
 This package contains the server part of the package.
 .
  <xsl:value-of select="uri" />

Package: opkg-client-<xsl:value-of select="$namelc"/>
Architecture: <xsl:call-template name="arch" />
Depends: <xsl:call-template name="depends" ><xsl:with-param name="path" select="clientDeps/requires" /></xsl:call-template>
Conflicts: <xsl:call-template name="depends" ><xsl:with-param name="path" select="clientDeps/conflicts" /></xsl:call-template>
Provides: <xsl:call-template name="depends" ><xsl:with-param name="path" select="clientDeps/provides" /></xsl:call-template>
Suggests: <xsl:call-template name="depends" ><xsl:with-param name="path" select="clientDeps/suggests" /></xsl:call-template>
Description: <xsl:value-of select="summary"/>, client part
 <xsl:value-of select="description" />
 .
 This package contains the client part of the package.
 .
  <xsl:value-of select="uri" />

</xsl:for-each>

</xsl:template>
</xsl:stylesheet>
