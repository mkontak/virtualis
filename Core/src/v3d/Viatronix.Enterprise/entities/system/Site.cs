// $Id: Site.cs,v 1.1 2006/05/09 17:34:41 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Xml;
using System.Runtime.Serialization;



namespace Viatronix.Enterprise.Entities
{

  #region enumerations

  [Serializable]
  public enum SiteFields
  {
    ID,
    NAME,
    ADDRESS1,
    ADDRESS2,
    CITY,
    STATE,
    ZIP,
    STATUS,
    COUNTRY,
    INTERNATIONAL
  }




  #endregion


  /// <summary>
  /// Site
  /// </summary>
  public class Site : Entity
  {

    #region Attributes class

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id                  = "id";
      public const string Name                = "name";
      public const string AddressLine1        = "addr1";
      public const string AddressLine2        = "addr2";
      public const string City                = "city";
      public const string State               = "state";
      public const string ZipCode             = "zip";
      public const string Country             = "cntry";
      public const string International       = "int";
    } // class Attributes

    #endregion

    #region fields

 
    /// <summary>
    /// XML file name 
    /// </summary>
    private const string m_xmltag = "site";

    /// <summary>
    /// Id
    /// </summary>
    private int m_id = 0;

    /// <summary>
    /// Name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Address 1
    /// </summary>
    private string m_address1 = string.Empty;


    /// <summary>
    /// Address 2
    /// </summary>
    private string m_address2 = string.Empty;


    /// <summary>
    /// City
    /// </summary>
    private string m_city = string.Empty;

    /// <summary>
    /// State
    /// </summary>
    private string m_state = string.Empty;

    /// <summary>
    /// Zip
    /// </summary>
    private string m_zip = string.Empty;

    /// <summary>
    /// Country
    /// </summary>
    private string m_country = string.Empty;

    /// <summary>
    /// International flag
    /// </summary>
    private bool m_international = false;








    #endregion

    #region construction and destruction

    /// <summary>
    /// Constructor
    /// </summary>
    public Site()
    {
    }


    #endregion

    #region properties

    /// <summary>
    /// Sets/Gets the Id
    /// </summary>
    public int Id
    {
      get { return m_id; }
      set { m_id = value; }
    } // Id

    /// <summary>
    /// Sets/Gets the Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    } // Name

    /// <summary>
    /// Sets/Gets the Country
    /// </summary>
    public string Country
    {
      get { return m_country; }
      set { m_country = value; }
    } // Country


    /// <summary>
    /// Gets the Address 1
    /// </summary>
    public string Address1
    {
      get { return m_address1; }
      set { m_address1 = value; }
    } // Address1

    /// <summary>
    /// Gets the Address 2
    /// </summary>
    public string Address2
    {
      get { return m_address2; }
      set { m_address2 = value; }
    } // Address2


    /// <summary>
    /// Sets/Gets the city
    /// </summary>
    public string City
    {
      get { return m_city; }
      set { m_city = value; }
    } // City


    /// <summary>
    /// Sets/Gets the state
    /// </summary>
    public string State
    {
      get { return m_state; }
      set { m_state = value; }
    } // State



    /// <summary>
    /// Sets/Gets the zip code
    /// </summary>
    public string Zip
    {
      get { return m_zip; }
      set { m_zip = value; }
    } // Zip


    /// <summary>
    /// Gets/Sets the International flag
    /// </summary>
    public bool International
    {
      get { return m_international; }
      set { m_international = value; }
    } // International


    #endregion

    #region methods

    /// <summary>
    /// Creates the site from the xml string
    /// </summary>
    /// <param name="xml"></param>
    /// <returns></returns>
    public static Site Create(string xml)
    {
      if (string.IsNullOrEmpty(xml))
        throw new InvalidDataException("Site xml is missing or invalid");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);
    } //Create(string xml)

    /// <summary>
    /// Creates the site from the xml node
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static Site Create(XmlNode node)
    {
      Site site = new Site();

      if (site == null)
        throw new OutOfMemoryException("Failed to allocate Site");

      site.m_id = Int32.Parse(node.Attributes[Attributes.Id].Value);
      site.m_name = node.Attributes[Attributes.Name].Value;
      site.m_address1 = node.Attributes[Attributes.AddressLine1].Value;
      site.m_address2 = node.Attributes[Attributes.AddressLine2].Value;
      site.m_city = node.Attributes[Attributes.City].Value;
      site.m_state = node.Attributes[Attributes.State].Value;
      site.m_zip = node.Attributes[Attributes.ZipCode].Value;
      site.m_country = node.Attributes[Attributes.Country].Value;
      site.m_international = (node.Attributes[Attributes.International].Value == "1" ? true : false);


      return site;

    }

    /// <summary>
    /// Returns String Representation of this object (NAME)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_name;
    } // ToString()


    /// <summary>
    /// Get hash code
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    } // GetHashCode()


    /// <summary>
    /// Determines equality
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    public override bool Equals(object obj)
    {
      return Equals((Site)obj);
    } // Equals()

    /// <summary>
    /// Determines equality
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    public bool Equals(Site Site)
    {
      return (this.Id == Site.Id);
    } // Equals()


 
    #endregion


    #region IEntity

    /// <summary>
    /// Creates an xml representation of a Site object.
    /// </summary>
    /// <returns>Xlm string representing the Site object.</returns>
    public override string ToXml()
    {
      using (MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter(new StreamWriter(stream));

        writer.WriteStartElement("Site");

        writer.WriteAttributeString(Attributes.Id, m_id.ToString());
        writer.WriteAttributeString(Attributes.Name, m_name);
        writer.WriteAttributeString(Attributes.AddressLine1, m_address1);
        writer.WriteAttributeString(Attributes.AddressLine2, m_address2);
        writer.WriteAttributeString(Attributes.City, m_city);
        writer.WriteAttributeString(Attributes.State, m_state);
        writer.WriteAttributeString(Attributes.ZipCode, m_zip);
        writer.WriteAttributeString(Attributes.Country, m_country);
        writer.WriteAttributeString(Attributes.International, m_international.ToString());

        writer.WriteEndElement();
        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString(stream.ToArray());
      }
    } // ToXml()


    #endregion

  } // class Site


} // namespace Viatronix.Enterprise.Entities
