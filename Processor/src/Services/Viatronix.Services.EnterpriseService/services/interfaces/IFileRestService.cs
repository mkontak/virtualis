// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: IFileService.cs
//
// Description: FILE SERVER HTTP service interface defining the operation contracts.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;
using System.ServiceModel.Web;
using System.IO;

namespace Viatronix.Services
{

  /// <summary>
  /// Defines the FILE SERVER REST interface
  /// </summary>
  [ServiceContract]
  public interface IFileRestService
  {

    /// <summary>
    /// Implements the HTTP POST operation for the FILE SERVER interface
    /// </summary>
    /// <param name="body">XML request stream</param>
    /// <returns>XML response</returns>
    [OperationContract]
    [WebInvoke(Method = "POST", UriTemplate = "/fileserver", BodyStyle = WebMessageBodyStyle.Bare )]
    Stream PostFileServer(Stream body);



  } // interface IService

} // namespace Viatronix.Enterprise
