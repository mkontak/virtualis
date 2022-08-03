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
  public interface IResourceProvider
  {

    // Process the request message
    ResponseMessage ProcessRequest(RequestMessage request);


  } // interface IResourceProvider

} // namespace Viatronix.Enterprise.Providers
