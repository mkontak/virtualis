// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ProcessingResourceProvider.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Viatronix.Enterprise.Data;
using System.Reflection;
using System.IO;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// Implements a database provider to service database calls for the resource supplied in the HTTP requests
  /// </summary>
  public class ProcessingResourceProvider : ResourceProviderBase
  {

  
    /// <summary>
    /// Gets the templates
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetTemplate(RequestMessage request)
    { return ProcessingGateway.GetHandler(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the messages
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetMessage(RequestMessage request)
    { return ProcessingGateway.GetHandler(request.ContextId, request.Resource, request.Payload); }

 
    /// <summary>
    /// Gets the job status 
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetJob(RequestMessage request)
    { return ProcessingGateway.GetHandler(request.ContextId, request.Resource, request.Payload); }

    
    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Pending jobs</returns>
    private string GetPendingJob(RequestMessage request)
    { return ProcessingGateway.GetHandler(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets tasks
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Tasks</returns>
    private string GetTask(RequestMessage request)
    { return ProcessingGateway.GetHandler(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets tasks
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Tasks</returns>
    private string UpdateTask(RequestMessage request)
    { return ProcessingGateway.UpdateHandler(request.ContextId, request.Resource, request.Payload); }


    /// <summary>
    /// Deletes the job
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private void DeleteJob(RequestMessage request)
    { ProcessingGateway.DeleteHandler(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Deletes the messages
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private void DeleteMessage(RequestMessage request)
    { ProcessingGateway.DeleteHandler(request.ContextId, request.Resource, request.Payload); }


  }

} // namespace Viatronix.Enterprise
