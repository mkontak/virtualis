using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;



namespace Viatronix.Enterprise.SDK.Providers
{

  /// <summary>
  /// Interface for implementing an asynchronous PACS data provider
  /// </summary>
  interface IAsynchronousPacsProvider : IPacsProvider, IAsynchronousBrowserProvider
  {


  } // interface IAsynchronousPacsProvider

} // namespace Viatronix.Enterprise.SDK.Providers

