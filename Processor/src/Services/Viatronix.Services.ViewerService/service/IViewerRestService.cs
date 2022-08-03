// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: IService.cs
//
// Description: HTTP service interface defining the operation contracts.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;
using System.ServiceModel.Web;
using System.IO;

namespace Viatronix.Services
{
  // NOTE: You can use the "Rename" command on the "Re-factor" menu to change the interface name "IService" in both code and config file together.
  [ServiceContract]
  public interface IViewerRestService
  {

    /// <summary>
    /// Implements the HTTP GET.
    ///   Performs a retrieval of the specified resource
    /// </summary>
    /// <param name="resource">Resource to perform operation on</param>
    /// <returns>XML results</returns>
    [OperationContract]
    [WebGet(UriTemplate = "/launch/", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Launch();

    /// <summary>
    /// Implements an Echo which works like a ping. What ever data message is sent will be sent back to the caller.
    /// </summary>
    /// <param name="message">Message to be echoed</param>
    /// <returns>Message</returns>
    [OperationContract]
    [WebGet(UriTemplate = "/echo/{message}", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Echo(string message);

 

  } // interface IService

} // namespace Viatronix.Services
