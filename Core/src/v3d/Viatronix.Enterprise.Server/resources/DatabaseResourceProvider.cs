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
  /// Implements a database provider to service database calls for the resource supplied in the HTTP requests
  /// </summary>
  public class DatabaseResourceProvider : ResourceProviderBase
  {


    #region Processing

    /// <summary>
    /// Gets the messages
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetMessage(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }


    /// <summary>
    /// Gets the job status 
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetJob(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }


    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Pending jobs</returns>
    private string GetPendingJob(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Deletes the job
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private void DeleteJob(RequestMessage request)
    { StorageGateway.Delete(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Deletes the messages
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private void DeleteMessage(RequestMessage request)
    { StorageGateway.Delete(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets tasks
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Tasks</returns>
    private string UpdateJob(RequestMessage request)
    { return StorageGateway.Update(request.ContextId, request.Resource, request.Payload); }

    #endregion


    #region Receiving


    /// <summary>
    /// Gets the receiver list 
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetReceiver(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

 
    #endregion

    #region Locking

    /// <summary>
    /// Gets the locking information for the data in the request
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetLock(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload);  }


    /// <summary>
    /// Gets the locking information for the data in the request
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private void DeleteLock(RequestMessage request)
    { StorageGateway.Delete(request.ContextId, request.Resource, request.Payload); }

    #endregion

    #region Storage


    /// <summary>
    /// Completes the session
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private void CompleteSession(RequestMessage request)
    {
      string uid = string.Empty;

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(request.Payload);
      doc.DocumentElement.TryGetAttribute<string>("uid", ref uid);
      StorageGateway.CloseSession(request.ContextId, uid);

      StorageGateway.CompleteSession(request.ContextId, uid); 
    }

    /// <summary>
    /// Close the session
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private void CloseSession(RequestMessage request)
    {
      string uid = string.Empty;

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(request.Payload);
      doc.DocumentElement.TryGetAttribute<string>("uid",ref uid);
      StorageGateway.CloseSession(request.ContextId, uid); 
    }

    #region Session


    #endregion


    /// <summary>
    /// Gets the reference list
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetReference(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the dependent list
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetDependent(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the list of applications (Only if installed will they be returned)
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetProduct(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the list of dicom servers
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetDicomServer(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

 
    #region Header

    /// <summary>
    /// Gets the dicom header(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private string GetDicomHeader(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    #endregion


    #region Study Methods

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private string GetStudy(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private void InsertStudy(RequestMessage request)
    { StorageGateway.Insert(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private void UpdateStudy(RequestMessage request)
    { StorageGateway.Update(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Deletes the study(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Study xml</returns>
    private void DeleteStudy(RequestMessage request)
    { StorageGateway.Delete(request.ContextId, request.Resource, request.Payload); }
    #endregion

    #region Series Methods

    /// <summary>
    /// Gets the Series path
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private string GetSeriesPath(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the Series(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private string GetSeries(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the Series(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private void InsertSeries(RequestMessage request)
    { StorageGateway.Insert(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the Series(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private void UpdateSeries(RequestMessage request)
    { StorageGateway.Update(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the Series(s)
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Series xml</returns>
    private void DeleteSeries(RequestMessage request)
    { StorageGateway.Delete(request.ContextId, request.Resource, request.Payload); }

    #endregion

    #endregion

    #region System

    /// <summary>
    /// Gets the template list
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetTemplate(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the systemsetting list
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetSystemSetting(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the studygroup list
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetStudyGroup(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the application list
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetApplication(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the DICOM hosts
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetDicomHost(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the modality
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetModality(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }
 
    /// <summary>
    /// Gets the setting
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetSetting(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }
 

    /// <summary>
    /// Gets the site
    /// </summary>
    /// <param name="message"></param>
    /// <returns></returns>
    private string GetSite(RequestMessage request)
   { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }
 

    /// <summary>
    /// Gets the list of study groups
    /// </summary>
    /// <param name="request"></param>
    /// <returns>Xml groups</returns>
    private string GetGroup(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }


    /// <summary>
    /// Gets the system setting or all system settings
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetSystem(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Get store information from the database and appends the capacity, total usage and 
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetStore(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, "store", request.Payload);  } // GetStorageCapacity(RequestMessage request)


    #endregion

    #region SystemConfig

    /// <summary>
    /// Gets the system configuration
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>User config xml</returns>
    private string GetSystemConfig(RequestMessage request)
    { return SystemGateway.Get(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Gets the system configuration
    /// </summary>
    /// <param name="request">Request</param>
    private void DeleteSystemConfig(RequestMessage request)
    { SystemGateway.Delete(request.ContextId, request.Resource, request.Payload); }

    /// <summary>
    /// Inserts the system configuration
    /// </summary>
    /// <param name="request">Request</param>
    private void InsertSystemConfig(RequestMessage request)
    { SystemGateway.Insert(request.ContextId, request.Resource, request.Payload); }

    #endregion

    #region Launching

    /// <summary>
    /// Gets the launch list for the viewer
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetLaunchList(RequestMessage request)
    { return StorageGateway.Get(request.ContextId, request.Resource, request.Payload); }

    #endregion


  } // class DatabaseResourceProvider

} // namespace Viatronix.Enterprise
