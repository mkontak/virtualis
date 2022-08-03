using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// Interface defining what a resource provider must support
  /// </summary>
  public interface IFileServerProvider
  {

    // Process the request message
    byte [] ProcessRequest(byte [] request);


  } // interface IFileServerProvider

} // namespace Viatronix.Enterprise.Providers
