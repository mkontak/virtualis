// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 

//
// File: ServiceProviderSection.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Xml;

namespace Viatronix.Enterprise.Configuration
{


  #region class ProviderSection


  /// <summary>
  /// Implements the resource provider section 
  /// </summary>
  public class ProviderSection : ConfigurationSection
  {

    #region fields

    // Section
    private static ProviderSection m_section = null;

    #endregion

    #region properties


    /// <summary>
    /// Gets the list of resource providers from the section
    /// </summary>
    [ConfigurationProperty("", IsRequired = true, IsDefaultCollection = true)]
    [ConfigurationCollection(typeof(ProviderElement), AddItemName = "provider")]
    public ProviderElementCollection Providers
    { get { return (ProviderElementCollection)this[""]; } }


    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static ProviderSection GetSection()
    { return GetSection("providers"); }


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static ProviderSection GetSection(string definedName)
    {
      if (m_section == null)
      {
        m_section = ConfigurationManager.GetSection(definedName) as ProviderSection;
        if (m_section == null)
          throw new ConfigurationErrorsException("The <" + definedName + "> section is not defined in your .config file!");
      }

      return m_section;
    } // GetSection( definedName )

    #endregion

  } // class ProviderSection

  #endregion

  #region class ProviderElement


  /// <summary>
  /// Resource provider element 
  /// </summary>
  public class ProviderElement : ConfigurationElement
  {

    #region properties


    /// <summary>
    /// Gets the name 
    /// </summary>
    [ConfigurationProperty("name", IsRequired = true)]
    public string Name
    { get { return (string)this["name"]; } }

    /// <summary>
    /// Gets the version 
    /// </summary>
    [ConfigurationProperty("ver", IsRequired = true)]
    public string Version
    { get { return (string)this["ver"]; } }

    /// <summary>
    /// Gets the provider type
    /// </summary>
    [ConfigurationProperty("url", IsRequired = false)]
    public string Url
    {
      get { return (string)this["url"]; }
    } // Url

    /// <summary>
    /// Gets the aetitle used in a PACS provider
    /// </summary>
    [ConfigurationProperty("aetitle", IsRequired = false)]
    public string AETitle
    {
      get { return (string)this["aetitle"]; }
    } // aetitle

    /// <summary>
    /// Gets the security provider name for this provider
    /// </summary>
    [ConfigurationProperty("security", IsRequired = false)]
    public string SecurityProviderName
    {
      get { return (string)this["security"]; }
    } // SecurityProviderName

    /// <summary>
    /// Gets the provider type
    /// </summary>
    [ConfigurationProperty("type", IsRequired = true)]
    [TypeConverter(typeof(Viatronix.Enterprise.Configuration.TypeConfigurationConverter))]
    public Type Type
    {
      get { return (Type)this["type"]; }
    } // Type

    /// <summary>
    /// Gets the provider renewal setting (only security)
    /// </summary>
    [ConfigurationProperty("renewal", IsRequired = false)]
    public Int64 Renewal
    {
      get { return Int64.Parse(this["renewal"].ToString()); }

    } // Renewal

    #endregion


    #region methods

    /// <summary>
    /// Converts the element back out to the xml
    /// </summary>
    /// <returns></returns>
    public string ToXml()
    {
      StringBuilder builder = new StringBuilder();

      builder.Append("<provider ");

      foreach (ConfigurationProperty property in this.Properties)
      {
        if (property.Type == typeof(Type))
        {
          if (this[property.Name] != null)
            builder.Append(string.Format("{0}=\"{1}\" ", property.Name, ((Type)this[property.Name]).AssemblyQualifiedName));
        }
        else
          builder.Append(string.Format("{0}=\"{1}\" ", property.Name, this[property.Name]));
      }

      builder.Append(" />");


      return builder.ToString();
    } // ToXml()

    #endregion

  } // class ProviderElement

  #endregion


  #region class ProviderElementCollection


  /// <summary>
  /// Collection of resource providers
  /// </summary>
  [ConfigurationCollection(typeof(ProviderElement), AddItemName = "provider", CollectionType = ConfigurationElementCollectionType.BasicMap)]
  public class ProviderElementCollection : ConfigurationElementCollection
  {

    #region properties

    /// <summary>
    /// Get the provider by index
    /// </summary>
    /// <param name="index">Index number</param>
    /// <returns></returns>
    public ProviderElement this[int index]
    { get { return (ProviderElement)base.BaseGet(index); } }


    /// <summary>
    /// Gets the data provider by name
    /// </summary>
    /// <param name="name">Name</param>
    /// <returns></returns>
    public new ProviderElement this[string name]
    { get { return (ProviderElement)base.BaseGet(name); } }

    #endregion

    #region ConfigurationElementCollection Members

    /// <summary>
    /// Creates a new element
    /// </summary>
    /// <returns></returns>
    protected override ConfigurationElement CreateNewElement()
    { return new ProviderElement(); }


    /// <summary>
    /// Gets the element key (Name of the provider)
    /// </summary>
    /// <param name="element">Element to get key for</param>
    /// <returns></returns>
    protected override object GetElementKey(ConfigurationElement element)
    { return (element as ProviderElement).Name; }

    #endregion

  } // class ProviderElementCollection

  #endregion

} // namespace Viatronix.Enterprise.Configuration
