using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Xml;

namespace Viatronix.Enterprise.Configuration
{

  public class ConfigurationTextElement<T> : ConfigurationElement
  {
    private T _value;
    protected override void DeserializeElement(XmlReader reader,
                            bool serializeCollectionKey)
    {
      _value = (T)reader.ReadElementContentAs(typeof(T), null);
    }

    protected override bool SerializeElement(XmlWriter writer, bool serializeCollectionKey)
    {
      return base.SerializeElement(writer, serializeCollectionKey);
    }

    public T Value
    {
      get { return _value; }
    }
  }
  public class ServersSection : ConfigurationSection
  {

    #region fields

    private static ServersSection m_section = null;

    #endregion

    #region properties


    /// <summary>
    /// Servers collection
    /// </summary>
    [ConfigurationProperty("", IsRequired = true, IsDefaultCollection = true)]
    [ConfigurationCollection(typeof(ServerElement), AddItemName = "server")]
    public ServerElementCollection Servers
    { get { return (ServerElementCollection)this[""]; } } 

    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static ServersSection GetSection()
    {
      return GetSection("servers");
    } // GetSection()


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static ServersSection GetSection(string definedName)
    {
      if (m_section == null)
      {
        m_section = ConfigurationManager.GetSection(definedName) as ServersSection;
        if (m_section == null)
          throw new NotFoundException("The <" + definedName + "> section is not defined in your .config file!");
      }

      return m_section;
    } // GetSection( definedName )

    #endregion

  } // class ServersSection



  public class ServerElement : ConfigurationElement
  {


    #region properties

    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty("name", IsRequired = true)]
    public string Name
    {
      get { return (string)this["name"]; }

    } // Name

    /// <summary>
    /// Gets the provider type
    /// </summary>
    [ConfigurationProperty("type", IsRequired = true)]
    [TypeConverter(typeof(TypeConfigurationConverter))]
    public Type Type
    {
      get { return (Type)this["type"]; }
    } // Type



    //// Add declarations for child elements and attributes like this:
    [ConfigurationProperty("settings")]
    public SettingsElement Settings
    {
      get { return (SettingsElement)this["settings"]; }
    } // Settings

    #endregion


  } // class ServerElement


  /// <summary>
  /// Settings element
  /// </summary>
  public class SettingsElement : ConfigurationElement
  {

    #region fields

    /// <summary>
    /// Settings
    /// </summary>
    private string m_settings = string.Empty;

    #endregion


    #region methods


    /// <summary>
    /// Override the Deserialization so that we can get the xml only
    /// </summary>
    /// <param name="reader"></param>
    /// <param name="serializeCollectionKey"></param>
    protected override void DeserializeElement(XmlReader reader, bool serializeCollectionKey)
    {
      m_settings = (string)reader.ReadOuterXml();
    } 

    #endregion


    #region properties

    /// <summary>
    /// Gets the settings
    /// </summary>
    public string Settings
    {
      get { return m_settings;  }
    }

      #endregion


  } // class SetingsElement


  /// <summary>
  /// Server element collection
  /// </summary>
  [ConfigurationCollection(typeof(ServerElement), AddItemName = "server", CollectionType = ConfigurationElementCollectionType.BasicMap)]
  public class ServerElementCollection : ConfigurationElementCollection
  {


    #region properties

    /// <summary>
    /// Gets the server element based on the index
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    public ServerElement this[int index]
    {
      get { return (ServerElement)base.BaseGet(index); }
    }


    /// <summary>
    /// Gets the server element based on the name
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public new ServerElement this[string name]
    {
      get { return (ServerElement)base.BaseGet(name); }
    }

    #endregion

    #region ConfigurationElementCollection Members

    /// <summary>
    /// Creates a new server element
    /// </summary>
    /// <returns></returns>
    protected override ConfigurationElement CreateNewElement()
    {
      return new ServerElement();
    }

    /// <summary>
    /// Gets the server element object
    /// </summary>
    /// <param name="element"></param>
    /// <returns></returns>
    protected override object GetElementKey(ConfigurationElement element)
    {
      return (element as ServerElement).Name;
    }

    #endregion

  } // class ServerElementCollection

} // namespace Viatronix.Enterprise.Configuration
