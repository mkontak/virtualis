// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FileSystemServiceProvider.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using Viatronix.Enterprise.Data;
using System.Reflection;
using Viatronix.Enterprise.Configuration;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// System Resource Provider
  /// 
  /// Provides interface to system resources
  /// </summary>
  public class SystemResourceProvider : ResourceProviderBase
  {

    /// <summary>
    /// Gets the templates
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetTemplate(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>


    /// <summary>
    /// Gets the current enterprise server date 
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetDate(RequestMessage request)
    {
      return string.Format("<date current=\"{0}\" ver=\"3.0\" />",GetDate());

    } // GetDate()


    /// <summary>
    /// Gets the current enterprise server date 
    /// </summary>
    /// <returns></returns>
    public static string GetDate()
    {
      return DateTimeOffset.Now.ToString();

    } // GetDate()


    /// <summary>
    /// Gets the joined domain of the enterprise server
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetDomain(RequestMessage request)
    {
      return string.Format("<domain name=\"{0}\" ver=\"3.0\" />",GetDomain());
    }  //  GetDomain(RequestMessage request)

    /// <summary>
    /// Gets the joined domain of the enterprise server
    /// </summary>
    /// <returns>Domain if joined, empty string if not</returns>
    public static string GetDomain()
    {

      string domainName = string.Empty;

      try
      {
        DomainInfo info = Viatronix.Enterprise.DomainInformation.GetDomainInfo();

        domainName = (info.Status == NetJoinStatus.NetSetupDomainName ? info.Name : string.Empty);
      }
      catch (Exception ex)
      {
        Logging.Log.Warning("Failed to get the computer domain : " + ex.Message, "SystemResourceProvider", "GetDomain");
      }


      return domainName;

    }  //  GetDomain(RequestMessage request)


 
  } // class SystemResourceProvider

} // namespace Viatronix.Enterprise.Providers
