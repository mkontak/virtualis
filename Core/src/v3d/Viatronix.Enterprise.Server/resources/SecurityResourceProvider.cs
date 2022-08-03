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
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  /// <summary>
  /// Security Resource Provider
  /// 
  /// Provider interface to security resource in the V3D system.
  /// 
  /// </summary>
  public class SecurityResourceProvider : ResourceProviderBase
  {


    #region Rights Methpds

    /// <summary>
    /// Gets the rights(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User xml</returns>
    private string GetRight(RequestMessage request)
    { return SecurityGateway.Get(request.ContextId, request.Resource, request.Payload); }


    #endregion

    #region Role Methods

    /// <summary>
    /// Gets the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User xml</returns>
    private string GetRole(RequestMessage request)
    { return SecurityGateway.Get(request.ContextId, request.Resource, request.Payload); }


    #endregion

    #region User Methods

    /// <summary>
    /// Gets the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User xml</returns>
    private string GetUser(RequestMessage request)
    { return SecurityGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Deletes the specified user
    /// </summary>
    /// <param name="request">Request</param>
    private void DeleteUser(RequestMessage request)
    {

      string login = "";
      if (!request.TryGetValue("login", out login))
        throw new System.IO.InvalidDataException("Cannot delete user, missing login");

      SecurityGateway.DeleteUser(request.ContextId, login);
    } // DeleteUser(RequestMessage request)

    /// <summary>
    /// Inserts the specified user
    /// </summary>
    /// <param name="request">Request</param>
    private void InsertUser(RequestMessage request)
    { SecurityGateway.Insert(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Updates the specified user
    /// </summary>
    /// <param name="request">Request</param>
    private void UpdateUser(RequestMessage request)
    { SecurityGateway.Update(request.ContextId, request.Resource, request.Payload); }

    #endregion

    #region Context Methods

    /// <summary>
    /// Gets the context (GET)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Context xml</returns>
    private string GetContext(RequestMessage request)
    { return SecurityGateway.Get(request.ContextId, request.Resource, string.Format("<context id=\"{0}\"/>", request.ContextId)); }

    /// <summary>
    /// Acquires a context (POST)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Context xml with new id</returns>
    private string AcquireContext(RequestMessage request)
    {
      return "<context id=\"" + SecurityGateway.AcquireContext(request.GetValue("id", string.Empty), request["login"], request["pwd"] ) + "\" ver=\"1.0\" />"; 
 
      //return "<context id=\"" + SecurityGateway.AcquireContext(request["login"], Viatronix.Encryption.Crypto.Decrypt(request["pwd"], Helper.GetKey()), request.GetValue("id", string.Empty)) + "\" ver=\"1.0\" />"; 
    }

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
    { SecurityGateway.ReleaseContext(request.GetValue("id", string.Empty), request.GetValue("login", string.Empty)); }

    /// <summary>
    /// Renews the context specified (POST)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>empty string</returns>
    private void RenewContext(RequestMessage request)
    { SecurityGateway.RenewContext(request.GetValue("id", string.Empty)); }

    #endregion

    #region UserProfile Methods

    /// <summary>
    /// Gets the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User config xml</returns>
    private string GetUserProfile(RequestMessage request)
    { return SecurityGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    private void DeleteUserProfile(RequestMessage request)
    { SecurityGateway.Delete(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Updates the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    private void UpdateUserProfile(RequestMessage request)
    { SecurityGateway.Update(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Inserts the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    private void InsertUserProfile(RequestMessage request)
    { SecurityGateway.Insert(request.ContextId, request.Resource, request.Payload); }


    #endregion

    #region AuthentcationTypes Methods

    /// <summary>
    /// Gets the user(s) specified
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User xml</returns>
    private string GetAuthenticationType(RequestMessage request)
    { return SecurityGateway.Get(request.ContextId, request.Resource, request.Payload); }


    #endregion


  }

} // namespace Viatronix.Enterprise
