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
  /// Interface for an asynchronous enabled data provider. Only includes
  /// </summary>
  public interface IAsynchronousDataProvider : IDataProvider, IAsynchronousBrowserProvider, IDisposable
  {

  } // interface IAsynchronousDataProvider

} // namespace Viatronix.Enterprise.SDK.Providers
