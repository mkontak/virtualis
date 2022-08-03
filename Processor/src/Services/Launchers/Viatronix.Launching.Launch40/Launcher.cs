// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File:  Launcher.cs
//
// Description: 
//
// Owner: (mkontak@viatronix.com)

using System;
using System.Xml;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Collections.Generic;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Launching
{
  /// <summary>
  /// Launcher used for Enterprise (4.0.x) viewers
  /// </summary>
  public class Launcher : ILauncher
  {

    #region fields

    /// <summary>
    /// Version of the launching
    /// </summary>
    private const string m_launchVersion = "4.0";


    protected readonly string m_context = Viatronix.Encryption.Crypto.Decrypt("cbHmZgzHtMUP0s69dPhPei5bYZHW1k7QFyx0oEYkyE8Vqgeb4rY/lpbtIw23fbJJ");


    #endregion

    #region properties

    /// <summary>
    /// Gets the launch version that this launcher supports
    /// </summary>
    public string Version
    { get { return m_launchVersion; } }


    #endregion

    #region methods

    public void CloseSession(Session session)
    {
      try
      {
        Logging.Log.Information(string.Format("Attempting to close session ({0})", session.Uid), "Launcher", "CloseSession");

        if (!string.IsNullOrEmpty(session.Uid))
        {
          Logging.Log.Information("Removing session locks", "Launcher", "CloseSession");

          StorageGateway.Delete(session.ContextId, "lock", XmlBuilder.Create("lock", new Dictionary<string, string>() { { "tid", session.TransactionId } }));

        }

        if (!session.IsSuppliedContext && session.HasContext )
        {
          Logging.Log.Information("Release context", "Launcher", "CloseSession");

          SecurityGateway.ReleaseContext(session.ContextId);
        }
      }
      catch ( Exception ex)
      {
        Logging.Log.Error(string.Format("Failed to close session ({0}) : {1}", session.ContextId, ex.Message), "Launcher", "CloseSession");
        throw;
      }
    }


    /// <summary>
    /// Launch the application specified in the request using the 4.0 version of the launching
    /// </summary>
    /// <param name="request">Request</param>
    /// <param name="user">User to launch under</param>
    /// <returns></returns>
    public Session Launch(ILaunchRequest request, Viatronix.Launching.Configuration.SpecificationsSection section, WindowsSession windowsSession)
    {

      // Get the specifications item for the application
      Viatronix.Launching.Configuration.SpecificationElement element = section.Specifications[request.Application];

      // Set the maximum number of instances allowed for the application
      int max = (element != null ? element.Maximum : 1);


      string application = request.Application;

      try
      {


        Viatronix.Logging.Log.Information(string.Format("Attempting to launch {0} application", application), "Launcher", "Launch");

        // Create the session
        Session session = new Session(request, this);

        // Validate that there were UIDs specified
        if ( string.IsNullOrEmpty(request.Uids) )
          throw new InvalidDataException("UIDS were not specified");

        // Validate that there was a user specified
        if (string.IsNullOrEmpty(request.UserName) )
          throw new InvalidDataException("User authorization information was not specified");

        // query for the applications
        XmlBuilder query = XmlBuilder.Create("application", new Dictionary<string, string>() { { "id", application } });

        List<Application> applications = new List<Application>();

        // Get the application list from the database
        EntityCollection.Load<Application>(applications, SystemGateway.Get(m_context, "application", query.ToXml()));

        // ==================================================
        // Check if the application specified is installed
        // =================================================
        if (applications.Count == 0 || !applications[0].Installed)
          throw new NotInstallException(string.Format("{0} not installed", application));

        // Set the executable path in the session
        session.Executable = applications[0].Url;


        // =============================================================
        // Make sure the max number of viewers is not already running
        // ==============================================================
        if (!Utilities.CanLaunch(session.Name, max))
          throw new AlreadyRunningException(string.Format("The maximum of {0} instance(s) of {1} is already running", max, request.Application));


        // =============================================================
        // Make sure the Console is not running
        // ==============================================================
        if (!Utilities.CanLaunch("Console"))
          throw new LaunchException("Cannot launch viewer remotely while the console is up");


        // ==============================================================================
        // If the context was not supplied then we need to log into the V3D System to
        // acquire a context.
        // =============================================================================
        if ( string.IsNullOrEmpty(request.ContextId))
        {

          Logging.Log.Debug(string.Format("Acquiring context for {0}", request.UserName), "Launcher", "Launch");

          // Acquire a context
          session.ContextId = SecurityGateway.AcquireContext(request.ContextId, request.UserName, Viatronix.Encryption.Crypto.Encrypt(request.Password));

          // Make sure it is not blank
          if (string.IsNullOrEmpty(session.ContextId))
            throw new SecurityException("Failed to login into V3D System!");

        } // END ... If the context was not supplied


        // ==============================================================
        // Create the query that will be used for the sp_getLaunchList
        // ==============================================================
        query = XmlBuilder.Create("launchlist");
        query.AddAttribute<string>("uids", request.Uids);
        query.AddAttribute<string>("app", application);
        query.AddAttribute<string>("desc", request.Description);
        query.AddAttribute<Boolean>("new", request.NewSession);
        query.AddAttribute<string>("sys", System.Environment.MachineName);

        // Get the launch list so that if any error occur they can be returned.
        string launchList = StorageGateway.Get(session.ContextId, "launchlist", query.ToXml());

        // ==============================================================================================================
        // We need to parse the session uid from the launch list since that will be used when launching the viewer.
        // ==============================================================================================================
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(launchList);
            
        session.Uid           = doc.DocumentElement.Attributes["uid"].Value;          // Session UID
        session.TransactionId = doc.DocumentElement.Attributes["tid"].Value;          // Transaction Id (Used to unlock)

        // Delete the current locks so that we can launch the viewer
        StorageGateway.Delete(session.ContextId, "lock", XmlBuilder.Create("lock", new Dictionary<string, string>() { { "tid", session.TransactionId } }));

        // Create the viewer arguments
        session.Arguments = string.Format("-uid \"{0}\" -user \"{1}||{2}\" ", session.Uid, request.UserName, session.ContextId);

        // launch the session
        session.Launch(windowsSession);

        // Returns the session
        return session;

      }
      catch (Exception ex)
      {
        Logging.Log.Error(string.Format("Failed to launch {0} : {1}", application, ex.Message), "Launcher", "Launch");
        throw;
      }

    }

    #endregion

  } // class Launcher
} // namespace Viatronix.Launching
