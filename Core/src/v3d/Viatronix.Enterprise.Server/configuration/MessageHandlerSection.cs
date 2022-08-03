using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Xml;

namespace Viatronix.Enterprise.Configuration
{
  public class MessageHandlerSection : ConfigurationSection
  {

    #region fields

    private static MessageHandlerSection m_section = null;

    #endregion

    #region properties

    
    [ConfigurationProperty( "", IsRequired=true, IsDefaultCollection=true )]
    [ConfigurationCollection( typeof( RequestHandlerElement ), AddItemName = "handler" )]
    public RequestHandlerElementCollection Handlers
    {
      get { return (RequestHandlerElementCollection) this[""]; }
    } // Processors


    #endregion

    #region methods

    /// <summary>
    /// Gets the configuration section using the default element name.
    /// </summary>
    public static MessageHandlerSection GetSection()
    {
      return GetSection( "msgHandlers" );
    } // GetSection()


    /// <summary>
    /// Gets the configuration section using the specified element name.
    /// </summary>
    public static MessageHandlerSection GetSection( string definedName )
    {
      if( m_section == null )
      {
        m_section = ConfigurationManager.GetSection( definedName ) as MessageHandlerSection;
        if( m_section == null )
          throw new ConfigurationErrorsException( "The <" + definedName + "> section is not defined in your .config file!" );
      }

      return m_section;
    } // GetSection( definedName )

    #endregion

  } // class ProcessorSectionHandler


  public class RequestHandlerElement : ConfigurationElement
  {

    #region properties

    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty( "name", IsRequired = true )]
    public string Name
    {
      get { return (string) this["name"]; }
    } // Name



    // Add declarations for child elements and attributes like this:
    [ConfigurationProperty( "typename", IsRequired = true )]
    [TypeConverter( typeof( TypeConfigurationConverter ))]
    public Type Type
    {
      get { return (Type) this["typename"]; }
    } // Type

    #endregion

  } // class ProcessorElement

  
  [ConfigurationCollection( typeof( RequestHandlerElement ), AddItemName = "handler", CollectionType = ConfigurationElementCollectionType.BasicMap )]
  public class RequestHandlerElementCollection : ConfigurationElementCollection
  {

    #region properties

    public RequestHandlerElement this[ int index ]
    {
      get { return (RequestHandlerElement) base.BaseGet( index ); }
    }


    public new RequestHandlerElement this[ string name ]
    {
      get { return (RequestHandlerElement) base.BaseGet( name ); }
    }

    #endregion

    #region ConfigurationElementCollection Members

    protected override ConfigurationElement CreateNewElement()
    {
      return new RequestHandlerElement();
    }


    protected override object GetElementKey( ConfigurationElement element )
    {
      return ( element as RequestHandlerElement ).Name;
    }

    #endregion

  } // class RequestHandlerElementCollection

} // namespace Viatronix.Enterprise.Configuration
