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


  #region class ResourceProviderSection


  /// <summary>
  /// Implements the resource provider section 
  /// </summary>
  public class ResourceProviderSection : ConfigurationSection
  {

    #region fields

    // Section
    private static ResourceProviderSection m_section = null;

    #endregion

    #region properties


    /// <summary>
    /// Gets the list of resource providers from the section
    /// </summary>
    [ConfigurationProperty("", IsRequired = true, IsDefaultCollection = true)]
    [ConfigurationCollection(typeof(ResourceProviderElement), AddItemName = "provider")]
    public ResourceProviderElementCollection Providers
    {   get { return (ResourceProviderElementCollection)this[""]; }  } 


    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static ResourceProviderSection GetSection()
    { return GetSection("resourceProviders"); } 


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static ResourceProviderSection GetSection(string definedName)
    {
      if (m_section == null)
      {
        m_section = ConfigurationManager.GetSection(definedName) as ResourceProviderSection;
        if (m_section == null)
          throw new ConfigurationErrorsException("The <" + definedName + "> section is not defined in your .config file!");
      }

      return m_section;
    } // GetSection( definedName )

    #endregion

  } // class ResourceProviderSection

  #endregion

  #region class ResourceProviderElement


  /// <summary>
  /// Resource provider element 
  /// </summary>
  public class ResourceProviderElement : ConfigurationElement
  {

    #region properties


    /// <summary>
    /// Gets the resource 
    /// </summary>
    [ConfigurationProperty("resource", IsRequired = true)]
    public string Resource
    {  get { return (string)this["resource"]; } } 



    /// <summary>
    /// Gets the provider type
    /// </summary>
    [ConfigurationProperty("type", IsRequired = true)]
    [TypeConverter(typeof(TypeConfigurationConverter))]
    public Type Type
    {
      get { return (Type)this["type"]; }
    } // Type


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
          builder.Append(string.Format("{0}=\"{1}\" ", property.Name, ((Type)this[property.Name]).AssemblyQualifiedName));
        else
          builder.Append(string.Format("{0}=\"{1}\" ", property.Name, (string)this[property.Name]));
      }

      builder.Append(" />");


      return builder.ToString();
    } // ToXml()

    #endregion

  } // class ResourceProviderElement

  #endregion


  #region class ResourceProviderElementCollection


  /// <summary>
  /// Collection of resource providers
  /// </summary>
  [ConfigurationCollection(typeof(ResourceProviderElement), AddItemName = "provider", CollectionType = ConfigurationElementCollectionType.BasicMap)]
  public class ResourceProviderElementCollection : ConfigurationElementCollection
  {

    #region properties

    /// <summary>
    /// Get the provider by index
    /// </summary>
    /// <param name="index">Index number</param>
    /// <returns></returns>
    public ResourceProviderElement this[int index]
    { get { return (ResourceProviderElement)base.BaseGet(index); } }


    /// <summary>
    /// Gets the resource provider by name
    /// </summary>
    /// <param name="name">Name</param>
    /// <returns></returns>
    public new ResourceProviderElement this[string name]
    { get { return (ResourceProviderElement)base.BaseGet(name); } }

    #endregion

    #region ConfigurationElementCollection Members

    /// <summary>
    /// Creates a new element
    /// </summary>
    /// <returns></returns>
    protected override ConfigurationElement CreateNewElement()
    {  return new ResourceProviderElement(); }


    /// <summary>
    /// Gets the element key (Name of the provider)
    /// </summary>
    /// <param name="element">Element to get key for</param>
    /// <returns></returns>
    protected override object GetElementKey(ConfigurationElement element)
    { return (element as ResourceProviderElement).Resource; }

    #endregion

  } // class ResourceProviderElementCollection

  #endregion

} // namespace Viatronix.Enterprise.Configuration
