// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ResourceProviderFactory.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Enterprise.Configuration;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// Factory to create the resource provider to be used with the request received
  /// </summary>
  public static class ResourceProviderFactory
  {

    #region fields

    /// <summary>
    /// Dictionary containing the list of resource providers
    /// </summary>
    private static Dictionary<string, IResourceProvider> m_providers = new Dictionary<string, IResourceProvider>();

    #endregion

    /// <summary>
    /// Creates the resource provider to be used with the request
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Resource provider</returns>
    public static IResourceProvider Create(RequestMessage request)
    {
      return Create(request.Resource);
    } // Create(RequestMessage request)

    /// <summary>
    /// Creates the resource provider to be used with the request
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Resource provider</returns>
    public static IResourceProvider Create(string resource)
    {
      try
      {

        ResourceProviderElement element = ResourceProviderSection.GetSection().Providers[resource];

        return (IResourceProvider)Activator.CreateInstance(element.Type);
      }
      catch (Exception e)
      {
        throw new ArgumentException("Unknown resource.", e);
      }
    } // Create(RequestMessage request)

  } // class ResourceProviderFactry

} // namespace Viatronix.Enterprise.Providers
