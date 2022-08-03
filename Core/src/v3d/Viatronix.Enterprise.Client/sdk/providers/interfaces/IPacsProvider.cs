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
  /// <summary>
  /// Provides Pacs specific features to enable browsing datasets on a pacs system.
  /// </summary>
  public interface IPacsProvider : IBrowserProvider
  {

    #region properties

    /// <summary>
    /// Gets/Sets the PACS server to work against
    /// </summary>
    DicomHost Server
    {
      get;
      set;
    } // Server

    #endregion

  } // interface IPacsProvider
} // namespace Viatronix.Enterprise.SDK.Providers
