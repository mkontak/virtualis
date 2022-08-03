// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 

//
// File: FileSystemSection.cs
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


  public class FileSystemSection : ConfigurationSection
  {

    #region fields

    private static FileSystemSection m_section = null;

    #endregion

    #region properties


    [ConfigurationProperty("", IsRequired = true, IsDefaultCollection = true)]
    [ConfigurationCollection(typeof(FileSystemElement), AddItemName = "file")]
    public FileSystemElementCollection FileSystems
    {
      get { return (FileSystemElementCollection)this[""]; }
    } // Processors


    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static FileSystemSection GetSection()
    {
      return GetSection("fileSystem");
    } // GetSection()


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static FileSystemSection GetSection(string definedName)
    {
      if (m_section == null)
      {
        m_section = ConfigurationManager.GetSection(definedName) as FileSystemSection;
        if (m_section == null)
          throw new ConfigurationErrorsException("The <" + definedName + "> section is not defined in your .config file!");
      }

      return m_section;
    } // GetSection( definedName )

    #endregion

  } // class FileSystemSection


  public class FileSystemElement : ConfigurationElement
  {

    #region properties

    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty("name", IsRequired = true)]
    public string Name
    {
      get { return (string)this["name"]; }
    } // Name

    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty("type", IsRequired = true)]
    public string Type
    {
      get { return (string)this["type"]; }
    } // Type


    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty("loc", IsRequired = true)]
     public string Location
    {
      get { return (string)this["loc"]; }
    } // Type

    #endregion

    #region method
    /// <summary>
    /// Converts the element back out to the xml
    /// </summary>
    /// <returns></returns>
    public string ToXml()
    {
      StringBuilder builder = new StringBuilder();

      builder.Append("<file ");

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

  } // class ProcessorElement


  [ConfigurationCollection(typeof(FileSystemElement), AddItemName = "file", CollectionType = ConfigurationElementCollectionType.BasicMap)]
  public class FileSystemElementCollection : ConfigurationElementCollection
  {

    #region properties

    public FileSystemElement this[int index]
    {
      get { return (FileSystemElement)base.BaseGet(index); }
    }


    public new FileSystemElement this[string name]
    {
      get { return (FileSystemElement)base.BaseGet(name); }
    }

    #endregion

    #region ConfigurationElementCollection Members

    protected override ConfigurationElement CreateNewElement()
    {
      return new FileSystemElement();
    }


    protected override object GetElementKey(ConfigurationElement element)
    {
      return (element as FileSystemElement).Name;
    }

    #endregion

  } // class FileSystemElementElementCollection

} // namespace Viatronix.Enterprise.Configuration
