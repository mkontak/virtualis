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
  public interface IRestService
  {

    /// <summary>
    /// Implements the HTTP GET.
    ///   Performs a retrieval of the specified resource
    /// </summary>
    /// <param name="resource">Resource to perform operation on</param>
    /// <returns>XML results</returns>
    [OperationContract]
    [WebGet(UriTemplate = "/{resource}", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Get(string resource);

    /// <summary>
    /// Implements an Echo which works like a ping. What ever data message is sent will be sent back to the caller.
    /// </summary>
    /// <param name="message">Message to be echoed</param>
    /// <returns>Message</returns>
    [OperationContract]
    [WebGet(UriTemplate = "/echo/{message}", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Echo(string message);

    /// <summary>
    /// Returns the flag indicating if the service is expecting encrypted text or not 
    /// </summary>
    /// <returns>Returns true if the service expects the data to be encrypted, false if not</returns>
    [OperationContract]
    [WebGet(UriTemplate = "/encrypt", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Encrypt();

    /// <summary>
    /// Implements an HTTP DELETE
    ///   Deletes the specified resource item
    /// </summary>
    /// <param name="resource">Resource to be deleted</param>
    /// <returns>Nothing id success, or error message if failure</returns>
    [OperationContract]
    [WebInvoke(Method = "DELETE", UriTemplate = "/{resource}", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Delete(string resource);

    /// <summary>
    /// Implements an HTTP PUT
    ///   Inserts the specified resource
    /// </summary>
    /// <param name="body">resource XML</param>
    /// <returns>Nothing is success, error is failure</returns>
    [OperationContract]
    [WebInvoke(Method = "PUT", UriTemplate = "/", BodyStyle = WebMessageBodyStyle.Bare, ResponseFormat = WebMessageFormat.Xml)]
    string Put(Stream body);


    /// <summary>
    /// Implements an HTTP POST
    ///   Updates the specified resource
    /// </summary>
    /// <param name="body">resource XML</param>
    /// <returns>Nothing is success, error is failure</returns>
    [OperationContract]
    [WebInvoke(Method = "POST", UriTemplate = "/", BodyStyle = WebMessageBodyStyle.Bare, RequestFormat = WebMessageFormat.Xml, ResponseFormat = WebMessageFormat.Xml)]
    string Post(Stream body);


 

  } // interface IService

} // namespace Viatronix.Enterprise
