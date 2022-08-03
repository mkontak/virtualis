// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DatabaseServiceProvider.cs
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
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// Implements a database provider to service database calls for the resource supplied in the HTTP requests
  /// </summary>
  public class ResourceProviderBase : IResourceProvider
  {


    #region IResourceProvider interface

    /// <summary>
    /// Processes the request
    /// </summary>
    /// <param name="request">Http request</param>
    /// <returns>Xml results or error</returns>
    public virtual ResponseMessage ProcessRequest(RequestMessage request)
    {
      try
      {
        string results = string.Empty;

        bool bSupported = false;

        // =======================================================================
        // Standard methods names consist of operation + resource
        //   example:    
        //            operation = get
        //            resource  = study
        //            method    = getstudy() or GetStudy()
        // ====================================================================
        string method = request.Operation + request.Resource;

        // =====================================================================================
        // Loop through all the non-public instance methods for a operational matching method
        // =====================================================================================
        foreach (MethodInfo info in this.GetType().GetMethods(BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.DeclaredOnly))
        {
          if (info.MemberType == MemberTypes.Method)
          {
            if (string.Compare(info.Name, method, true) == 0)
            {

              bSupported = true;

              // ==============================================================================
              // Check for a return code of string so we can catch the returning information
              // ==============================================================================
              if (info.ReturnType == typeof(string))
                results = (string)info.Invoke(this, new object[] { request });
              else
                info.Invoke(this, new object[] { request });

              break;
            }
          }

        }

        // If no method was found we have an unsupported resource/operation pair
        if (!bSupported)
          throw new System.NotSupportedException(string.Format("{0}-{1} is not supported", request.Operation, request.Resource));


        // Returns the results
        return ResponseMessage.Create(MessageFormat.Xml, results);

      }
      catch (TargetInvocationException e)
      {

        // ================================================================================================
        // We Catch the target of invocation exception since we are Invoking the method and any exceptions 
        // thrown in the Invoked method is wrapped in this exception.
        // =================================================================================================

        Log.Error("Failed to process request : " + e.InnerException.Message, "ResourceProviderBase", "ProcessRequest");
        throw e.InnerException;

      }
      catch (Exception e)
      {
        Log.Error("Failed to process request : " + e.Message, "ResourceProviderBase", "ProcessRequest");
        throw;
      }

    } // ProcessRequest(RequestMessage request)

    #endregion


 
  } // class ResourceProviderBase

} // namespace Viatronix.Enterprise
