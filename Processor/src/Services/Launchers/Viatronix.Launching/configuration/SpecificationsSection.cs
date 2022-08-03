using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Xml;

namespace Viatronix.Launching.Configuration
{
  public class SpecificationsSection : ConfigurationSection
  {

    #region fields

    private static SpecificationsSection m_section = null;

    #endregion

    #region properties


    /// <summary>
    /// Servers collection
    /// </summary>
    [ConfigurationProperty("", IsRequired = true, IsDefaultCollection = true)]
    [ConfigurationCollection(typeof(SpecificationElement), AddItemName = "specification")]
    public SpecificationElementCollection Specifications
    { get { return (SpecificationElementCollection)this[""]; } } 

    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static SpecificationsSection GetSection()
    {
      return GetSection("specifications");
    } // GetSection()


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static SpecificationsSection GetSection(string definedName)
    {
      if (m_section == null)
      {
        m_section = ConfigurationManager.GetSection(definedName) as SpecificationsSection;
        if (m_section == null)
          throw new NotFoundException("The <" + definedName + "> section is not defined in your .config file!");
      }

      return m_section;
    } // GetSection( definedName )

    #endregion

  } // class ServersSection



  #region class SpecificationElement

  public class SpecificationElement : ConfigurationElement
  {


    #region properties

    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty("app", IsRequired = true)]
    public string Application
    {
      get { return (string)this["app"]; }

    } // Name




    //// Add declarations for child elements and attributes like this:
    [ConfigurationProperty("max", IsRequired = true)]
    public int Maximum
    {
      get { return (int)this["max"]; }
    } // Count

    #endregion


  } // class SpecificationElement


  #endregion

 
  /// <summary>
  /// Server element collection
  /// </summary>
  [ConfigurationCollection(typeof(SpecificationsSection), AddItemName = "specification", CollectionType = ConfigurationElementCollectionType.BasicMap)]
  public class SpecificationElementCollection : ConfigurationElementCollection
  {


    #region properties

    /// <summary>
    /// Gets the server element based on the index
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    public SpecificationElement this[int index]
    {
      get { return (SpecificationElement)base.BaseGet(index); }
    }


    /// <summary>
    /// Gets the server element based on the name
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public new SpecificationElement this[string name]
    {
      get { return (SpecificationElement)base.BaseGet(name); }
    }

    #endregion

    #region ConfigurationElementCollection Members

    /// <summary>
    /// Creates a new server element
    /// </summary>
    /// <returns></returns>
    protected override ConfigurationElement CreateNewElement()
    {
      return new SpecificationElement();
    }

    /// <summary>
    /// Gets the server element object
    /// </summary>
    /// <param name="element"></param>
    /// <returns></returns>
    protected override object GetElementKey(ConfigurationElement element)
    {
      return (element as SpecificationElement).Application;
    }

    #endregion

  } // class SpecificationElementCollection

} // namespace Viatronix.Enterprise.Configuration
