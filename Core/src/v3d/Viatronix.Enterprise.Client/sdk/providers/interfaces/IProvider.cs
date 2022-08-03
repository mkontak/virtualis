using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{

  #region enum ProviderType

  /// <summary>
  /// SDK provider type
  /// </summary>
  public enum ProviderType
  {
    Security,
    Data,
    Pacs,
    Service,
    Renewal,
    Audit,
    Browser
  } // ProviderType

  #endregion



  /// <summary>
  /// Base provider interfaces. All SDK providers shall use this interface
  /// </summary>
  public interface IProvider
  {



    #region properties


    /// <summary>
    /// Source
    /// </summary>
    string Source
    { get; }

    /// <summary>
    /// Returns the available flag
    /// </summary>
    bool IsAvailable
    { get; }

    /// <summary>
    /// Returns the name of the provider.
    /// </summary>
    string Name
    { get; } // Name


    /// <summary>
    /// Gets the version
    /// </summary>
    string Version
    { get; }

    /// <summary>
    /// Gets the flag indicating is asynchronous operations are supported
    /// </summary>
    bool SupportsAsynchronousOperations
    { get; }

    #endregion


    #region methods

    /// <summary>
    /// Case-INsensitive compare
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    bool IsName(string name);

    /// <summary>
    /// Returs true if the type of the provider is specified 
    /// </summary>
    /// <param name="type">Provider type</param>
    /// <returns></returns>
    bool IsType(ProviderType type);


    #endregion


    
  }
}
