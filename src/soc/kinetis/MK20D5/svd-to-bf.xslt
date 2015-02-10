<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text"/>
  <xsl:template match="/device">
    <xsl:for-each select="peripherals/peripheral">
      <xsl:variable name="periph" select="name"/> <xsl:text>struct </xsl:text><xsl:value-of select="$periph"/><xsl:text> {
</xsl:text>
      <xsl:for-each select="registers/register">
        <xsl:text>	uint</xsl:text><xsl:value-of select="size"/><xsl:text>_t </xsl:text><xsl:value-of select="name"/>
        <xsl:text>;
</xsl:text>
      </xsl:for-each>
      <xsl:text>};
CTASSERT_SIZE_BYTE(struct </xsl:text><xsl:value-of select="$periph"/><xsl:text>, </xsl:text><xsl:value-of select="addressBlock/size"/><xsl:text>);

</xsl:text>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>
