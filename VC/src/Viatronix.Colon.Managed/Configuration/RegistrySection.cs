// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Globalization;
using System.Xml;
using System.Threading;

namespace Viatronix.Colon.Configuration
{
  public class RegistrySection : ConfigurationSection
  {

    #region fields

    /// <summary>
    /// map of settings 
    /// </summary>
    private static Dictionary< string, RegistrySection > m_sections = new Dictionary< string, RegistrySection >();

    /// <summary>
    /// section name
    /// </summary>
    private string m_name = string.Empty;

    #endregion

    #region construction

    protected RegistrySection()
    {
    }


    protected RegistrySection( TextReader input )
    {
      using( XmlReader reader = XmlReader.Create( input ))
        DeserializeSection( reader );
    }

    #endregion

    #region properties

    /// <summary>
    /// Returns the version attribute
    /// </summary>
    [ConfigurationProperty( "ver", IsRequired = true )]
    public string Version
    {
      get
      {
        return (string) this["ver"];
      }
    } // Version


    /// <summary>
    /// Returns a collection of registry elements
    /// </summary>
    [ConfigurationProperty( "", IsRequired = true, IsDefaultCollection = true )]
    [ConfigurationCollection( typeof( RegistryElement ), AddItemName = "key" )]
    public RegistryElementCollection Keys
    {
      get 
      {
        return (RegistryElementCollection) this[""];
      }
    } // Keys

    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static RegistrySection GetSection( string definedName )
    {
      string key = RegistrySectionNames.GetSectionName( definedName );

      RegistrySection section = null;
      if( !m_sections.TryGetValue( key, out section ))
      {
        TextReader reader = null;
        if( RegistryProviderFactory.Create().TryLoad( key, out reader ))
        {
          using( reader )
            section = new RegistrySection( reader);
        }
        else
        {
          System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration( ConfigurationUserLevel.None );
          section = config.GetSection( key ) as RegistrySection;
          if( section == null )
            throw new ConfigurationErrorsException( "The <" + key + "> section is not defined in your .config file!" );
        }

        section.m_name = section.SectionInformation.Name;
        m_sections.Add( key, section );
      }

      return section;
    } // GetSection( definedName )


    /// <summary>
    /// Saves the registry entries
    /// </summary>
    /// <param name="definedName">Name of the registry section</param>
    public void Save( string definedName )
    {
      string key = RegistrySectionNames.GetSectionName( definedName );

      StringBuilder builder = new StringBuilder();
      using( XmlWriter writer = XmlWriter.Create( builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = true }))
        this.SerializeToXmlElement( writer, key );

      RegistryProviderFactory.Create().Save( key, builder.ToString() );
    } // Save( definedName )

 
    /// <summary>
    /// Gets the registry key associated with the specified name.
    /// </summary>
    /// <param name="name">the name of the registry key to get</param>
    /// <param name="element">When this method returns, contains the registry key associated with the specified name,</param>
    /// <returns>true if it contains an element with the specified name; otherwise, false</returns>
    public bool TryGetKey( string name, out RegistryElement element )
    {
      element = null;
      foreach( RegistryElement registryElement in this.Keys )
      {
        if( registryElement.Name == name )
        {
          element = registryElement;
          break;
        }
      }
      return element != null;
    } // TryGetKey( name, out element )

    #endregion

  } // class RegistrySection

  #region class RegistryElement

  public class RegistryElement : ConfigurationElement
  {

    #region properties

    /// <summary>
    /// Returns the value of the name attribute
    /// </summary>
    [ConfigurationProperty( "name", IsRequired = true )]
    public string Name
    {
      get { return (string) this["name"]; }
    } // Name


    /// <summary>
    /// Returns the value of the value attribute
    /// </summary>
    [UserScopedSetting]
    [ConfigurationProperty( "value", IsRequired = true )]
    public string Value
    {
      get { return (string) this["value"]; }
      set { this[ "value" ] = value; }
    } // Value

    #endregion

    #region methods

    /// <summary>
    /// Converts the type of the value attribute and returns it.
    /// </summary>
    /// <typeparam name="T">new type</typeparam>
    /// <returns>converted value</returns>
    public T As<T>()
    {
      return (T) Convert.ChangeType( this.Value, typeof( T ), CultureInfo.InvariantCulture );
    } // As<T>()


    /// <summary>
    /// Sets the value of the value attribute
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="value"></param>
    public void Set<T>( T value )
    {
      this.Value = value.ToString();
    } // Set<T>( value )

    #endregion

  } // class RegistryElement

  #endregion

  #region class RegistryElementCollection

  [ConfigurationCollection( typeof( RegistryElement ), AddItemName = "key", CollectionType = ConfigurationElementCollectionType.BasicMap )]
  public class RegistryElementCollection : ConfigurationElementCollection
  {

    #region properties

    /// <summary>
    /// Return the RegistryElement at the specified index
    /// </summary>
    /// <param name="index">index</param>
    /// <returns>RegistryElement</returns>
    public RegistryElement this[int index]
    {
      get
      {
        return (RegistryElement) base.BaseGet( index );
      }
    } // Indexer


    /// <summary>
    /// Return the RegistryElement with the specified name
    /// </summary>
    /// <param name="name">name of the RegistryElement</param>
    /// <returns>RegistryElement</returns>
    public new RegistryElement this[string name]
    {
      get
      {
        return (RegistryElement) base.BaseGet( name );
      }
    } // Indexer

    #endregion

    #region ConfigurationElementCollection Members

    /// <summary>
    /// Creates a new element
    /// </summary>
    /// <returns>new element</returns>
    protected override ConfigurationElement CreateNewElement()
    {
      return new RegistryElement();
    } // CreateNewElement()


    /// <summary>
    /// return the element key
    /// </summary>
    /// <param name="element">element</param>
    /// <returns>key</returns>
    protected override object GetElementKey( ConfigurationElement element )
    {
      return ( element as RegistryElement ).Name;
    } // GetElementKey( element )

    #endregion

  } // class RegistryElementCollection

  #endregion

} // namespace Viatronix.Colon.Configuration
