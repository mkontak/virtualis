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

namespace Viatronix.Enterprise
{

  /// <summary>
  /// Implements a database provider to service database calls for the resource supplied in the HTTP requests
  /// </summary>
  public class DatabaseResourceProvider : Viatronix.Enterprise.IResourceProvider
  {

    #region IResourceProvider interface

    /// <summary>
    /// Processes the request
    /// </summary>
    /// <param name="request">Http request</param>
    /// <returns>Xml results or error</returns>
    public ResponseMessage ProcessRequest(RequestMessage request)
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
          throw new System.IO.InvalidDataException("Resource/operation pair not supported");


        // Returns the results
        return ResponseMessage.Create(MessageType.Xml, results);

      }
      catch (TargetInvocationException e)
      {

        // ================================================================================================
        // We Catch the target of invocation exception since we are Invoking the method and any exceptions 
        // thrown in the Invoked method is wrapped in this exception.
        // =================================================================================================

        Log.Error("Failed to process request : " + e.InnerException.Message, "DatabaseServiceProvider", "ProcessRequest");
        throw e.InnerException;

      }
      catch (Exception e)
      {
        Log.Error("Failed to process request : " + e.Message, "DatabaseServiceProvider", "ProcessRequest");
        throw;
      }

    } // ProcessRequest(RequestMessage request)

    #endregion



    #region Study Methods

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private string GetStudy(RequestMessage request)
    { return StorageGateway.GetStudies(request.ToXml()); }

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private void InsertStudy(RequestMessage request)
    { StorageGateway.InsertStudy(request.ToXml()); }

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private void UpdateStudy(RequestMessage request)
    {  StorageGateway.UpdateStudy(request.ToXml()); }

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private void DeleteStudy(RequestMessage request)
    {  StorageGateway.DeleteStudy(request.ToXml()); }

    #endregion
 

    #region User Methods

    /// <summary>
    /// Gets the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User xml</returns>
    private string GetUser(RequestMessage request)
    {
      string login = "";
      request.TryGetValue("login", out login);

      return SecurityGateway.GetUsers(login);

    } // GetUser(RequestMessage request)

    /// <summary>
    /// Deletes the specified user
    /// </summary>
    /// <param name="request">Request</param>
    private void DeleteUser(RequestMessage request)
    {

      string login = "";
      if (!request.TryGetValue("login", out login))
        throw new System.IO.InvalidDataException("Cannot delete user, missing login");

      SecurityGateway.DeleteUser(login);
    } // DeleteUser(RequestMessage request)

    #endregion


    #region Context Methods

    /// <summary>
    /// Gets the context (GET)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Context xml</returns>
    private string GetContext(RequestMessage request)
    {   return SecurityGateway.GetContext(request.ContextId);  } 

    /// <summary>
    /// Acquires a context (POST)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Context xml with new id</returns>
    private string AcquireContext(RequestMessage request)
    { return "<context id=\"" + SecurityGateway.AcquireContext(request["user"], request["pwd"], request.GetValue("id", string.Empty)) + "\"/>";  }

    /// <summary>
    /// Deletes or Releases the context (DELETE)
    /// </summary>
    /// <param name="request">Request</param>
    private void DeleteContext(RequestMessage request)
    { SecurityGateway.ReleaseContext(request.GetValue("id", string.Empty)); }

    /// <summary>
    /// Deletes or Releases the context (POST)
    /// </summary>
    /// <param name="request">Request</param>
    private void ReleaseContext(RequestMessage request)
    { SecurityGateway.ReleaseContext(request.GetValue("id", string.Empty)); }

    /// <summary>
    /// Renews the context specified (POST)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>empty string</returns>
    private void RenewContext(RequestMessage request)
    {  SecurityGateway.RenewContext(request.GetValue("id", string.Empty)); }

    #endregion



  }

} // namespace Viatronix.Enterprise
