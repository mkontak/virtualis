// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CorsWebServiceHostFactory.cs
//
// Description: Factory for creating the CORS service
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ServiceModel;
using System.ServiceModel.Activation;

namespace Viatronix.WebHttpCors
{    

  /// <summary>
  /// Implements the HTTP CORS service factory
  /// </summary>
  public class CorsWebServiceHostFactory : WebServiceHostFactory
  {

    /// <summary>
    /// Create the service host
    /// </summary>
    /// <param name="serviceType">Service type</param>
    /// <param name="baseAddresses">Base URI address</param>
    /// <returns></returns>
    protected override ServiceHost CreateServiceHost(Type serviceType, Uri[] baseAddresses)
    {
        ServiceHost host = base.CreateServiceHost(serviceType, baseAddresses);
        host.Opening += new EventHandler(host_Opening);
        return host;
    }

    /// <summary>
    /// Event handeler for when the host is being opened
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void host_Opening(object sender, EventArgs e)
    {

      // Get the defined endpoints, only WebHttpBining is supported
      var endpoints = (sender as ServiceHost).Description.Endpoints.Where(se => se.Binding is WebHttpBinding);

      // =========================================================================
      // loop throught all the end points so that the Cors behavior can be added
      // ========================================================================
      foreach (var endpoint in endpoints)
      {
        // Add support for cross-origin resource sharing
        endpoint.Behaviors.Add(new CorsSupportBehavior());
      }
    }
  } // class CorsWebServiceHostFactory

} // namespace Viatronix.WebHttpCors