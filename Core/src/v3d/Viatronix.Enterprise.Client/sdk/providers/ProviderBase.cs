using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Enterprise.Entities;



namespace Viatronix.Enterprise.SDK.Providers
{


  /// <summary>
  /// Base implementation of a provider
  /// </summary>
  public class ProviderBase : IProvider
  {

    #region fields

    /// <summary>
    /// Seperator
    /// </summary>
    protected readonly char[] m_seperator = { '|' };

    /// <summary>
    /// Source Name to be sent to determine source of requests
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// The name of the provider.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Version
    /// </summary>
    private string m_version = string.Empty;


    #endregion


    #region constructors

    /// <summary>
    /// Initializes the provider using only xml configutation. This is used for the 
    /// SecurityProvders
    /// </summary>
    /// <param name="xml"></param>
    protected ProviderBase(string xml)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      if (!doc.DocumentElement.TryGetAttributeValue<string>("name", ref m_name))
        throw new ConfigurationException("Invalid configuration missing name");


      if (!doc.DocumentElement.TryGetAttributeValue<string>("ver", ref m_version))
        throw new ConfigurationException("Invalid configuration missing version");

      doc.DocumentElement.TryGetAttributeValue<string>("source", ref m_source);

    } // ProviderBase(string xml)


    #endregion

    #region properties

    /// <summary>
    /// Gets the source
    /// </summary>
    public string Source
    { get { return m_source; } }

    /// <summary>
    /// Returns the authenticated flag
    /// </summary>
    public virtual bool IsAuthenticated
    { get { return false; } }

    /// <summary>
    /// Returns the available flag
    /// </summary>
    public virtual bool IsAvailable
    { get { return false; } }

    /// <summary>
    /// Returns the name of the provider.
    /// </summary>
    public string Name
    { get { return m_name; } } // Name


    /// <summary>
    /// Gets the version
    /// </summary>
    public string Version
    { get { return m_version; } }

    /// <summary>
    /// Returns false since by default no asynchronous operations are supported
    /// </summary>
    public virtual bool SupportsAsynchronousOperations
    { get { return false; } }

    #endregion


    #region methods

    /// <summary>
    /// Case-insensitve comparison
    /// </summary>
    /// <param name="name">Name of provider</param>
    /// <returns></returns>
    public bool IsName(string name)
    { return (string.Compare(m_name, name, true) == 0); }


 
 
    /// <summary>
    /// Determines if the provider is of the type specified. A provider can be multiple types
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    public bool IsType(ProviderType type)
    {
      bool isType = false;

       switch (type)
        {
          case ProviderType.Security:
            if ( this is ISecurityProvider )
              isType = true;
            break;
          case ProviderType.Data:
            if (this is IDataProvider)
              isType = true;
            break;
          case ProviderType.Renewal:
            if ( this is IRenewalProvider )
              isType = true;
            break;
          case ProviderType.Pacs:
            if ( this is IPacsProvider )
              isType = true;
            break;
          case ProviderType.Audit:
            if ( this is IAuditProvider )
              isType = true;
            break;
          case ProviderType.Browser:
            if (this is IAuditProvider)
              isType = true;
            break;

         case ProviderType.Service:
            if (this is IServiceProvider)
              isType = true;
            break;
        };

      return isType;

    }

    #endregion



  }
}
