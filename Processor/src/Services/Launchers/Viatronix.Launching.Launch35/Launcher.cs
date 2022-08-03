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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Threading.Tasks;
using Viatronix.Launching.Database;
using System.Diagnostics;
using System.Configuration;

namespace Viatronix.Launching
{
  class Launcher : ILauncher
  {
    #region fields



    /// <summary>
    /// Version of the launching
    /// </summary>
    private const string m_launchVersion = "3.5";


    private string m_configDirectory = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.CommonApplicationData), "Viatronix\\Config");

    private string m_connectionsConfig = string.Empty;

    private Dictionary<string, string> m_systemSettings = new Dictionary<string, string>();


    #endregion


    #region properties
   /// <summary>
    /// Gets the launch version that this launcher supports
    /// </summary>
    public string Version
    { get { return m_launchVersion; } }

 
    #endregion

    #region constructor

    public Launcher()
    {


 
    }

    #endregion

    #region methods

    /// <summary>
    /// Gets the setting from the SystemSettings.config file
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    private string GetSetting(string name)
    {

      if (m_systemSettings.Count == 0)
      {
        string systemSettingsFile = System.IO.Path.Combine(m_configDirectory, "SystemSettings.config");

        XmlDocument doc = new XmlDocument();

        doc.Load(systemSettingsFile);

        foreach (XmlNode node in doc.DocumentElement.ChildNodes)
        {
          m_systemSettings.Add(node.Name, node.InnerXml);
        }


      }


      if (m_systemSettings.ContainsKey(name))
        return m_systemSettings[name];
      else
        throw new NotFoundException(string.Format("Specified setting {0} was not found in file", name));


    }

 
    /// <summary>
    /// Parse the directories macros
    /// </summary>
    /// <param name="directory"></param>
    /// <returns></returns>
    private string ParseDirectory(string directory)
    {
      string parsed = directory;

      parsed = parsed.Replace("$(ViewersDirectory)", GetSetting("viewersDirectory"));
      parsed = parsed.Replace("$(ProcessorDirectory)", GetSetting("processorDirectory"));
      parsed = parsed.Replace("$(ConfigDirectory)", GetSetting("configDirectory"));
      parsed = parsed.Replace("$(ComDirectory)", GetSetting("comDirectory"));
      parsed = parsed.Replace("$(UtilitiesDirectory)", GetSetting("utilitiesDirectory"));
      parsed = parsed.Replace("$(ServicesDirectory)", GetSetting("servicesDirectory"));
      parsed = parsed.Replace("$(UsersDirectory)", GetSetting("usersDirectory"));

      parsed = parsed.Replace("$(InstallDirectory)", GetSetting("installDirectory"));

      return parsed;
    }

    /// <summary>
    /// Closes a session using 3.5.X version tables and functions
    /// </summary>
    /// <param name="session">Session to be closed</param>
    public void CloseSession(Session session)
    {
      try
      {
        Logging.Log.Information(string.Format("Attempting to close session ({0}), exited with {1}", session.Uid, session.ExitCode), "Launcher", "CloseSession");

        if (!string.IsNullOrEmpty(session.Uid))
        {
          Logging.Log.Information("Removing session locks", "Launcher", "CloseSession");

          StorageGateway.UnlockSession(session);

        }

        SecurityGateway.ReleaseContext(session);
      }
      catch (Exception ex)
      {
        Logging.Log.Error(string.Format("Failed to close session ({0}) : {1}", session.ContextId, ex.Message), "Launcher", "CloseSession");
        throw;
      }



    } // CloseSession()


    /// <summary>
    /// Launch the application
    /// </summary>
    /// <param name="request"></param>
    /// <param name="user"></param>
    /// <param name="max"></param>
    /// <returns></returns>
    public Session Launch(ILaunchRequest request, Viatronix.Launching.Configuration.SpecificationsSection section, WindowsSession windowsSession)
    {
      

      // Get the specifications item for the application
      Viatronix.Launching.Configuration.SpecificationElement element = section.Specifications[request.Application];

      // Set the maximum number of instances allowed for the application
      int max = (element != null ? element.Maximum : 1);

      try
      {
        Viatronix.Logging.Log.Information(string.Format("Attempting to launch {0} application", request.Application), "Launcher", "Launch");

        // ================================
        // Make sure that SP4 is installed
        // =================================
        if (!SystemGateway.IsServicePack4Installed())
          throw new LaunchException("Service Pack 4 is not installed");


        Session session = new Session(request, this);

        if (string.IsNullOrEmpty(request.Uids))
          throw new InvalidDataException("UIDS were not specified");

        if (string.IsNullOrEmpty(request.UserName))
          throw new InvalidDataException("User authorization information was not specified");


        string launchRulesConfigFile = System.IO.Path.Combine(m_configDirectory, "launchrules.config");

        XmlDocument doc = new XmlDocument();

        doc.Load(launchRulesConfigFile);

        bool installed = false;

        foreach (XmlNode node in doc.DocumentElement.SelectNodes("rule"))
        {
          string id = node.Attributes["id"].Value;

          if (string.Compare(id, request.Application, true) == 0)
          {
            Viatronix.Logging.Log.Information(string.Format("Found launch rule for {0}", request.Application), "Launcher", "Launch");
            session.Executable = ParseDirectory(node.Attributes["executable"].Value);
            Viatronix.Logging.Log.Information(string.Format("{0} executables {1}", request.Application, session.Executable), "Launcher", "Launch");

            installed = true;

            break;
          }
        }

        // ==================================================
        // Check if the application specified is installed
        // =================================================
        if (!installed)
          throw new NotInstallException(string.Format("{0} not installed", request.Application));

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



        // ================================================================
        // If no context specified in the request then acquire the context
        // ===============================================================
        if (string.IsNullOrEmpty(request.ContextId))
        {
          Logging.Log.Information(string.Format("Attempting to log into the V3D System ({0})", request.UserName), "Launcher", "Launch");

          session.ContextId = SecurityGateway.Login(request.UserName, request.Password);


          if (string.IsNullOrEmpty(session.ContextId))
            throw new SecurityException("Failed to log into V3D System");


          SecurityGateway.ReleaseContext(session);

          session.ContextId = string.Empty;

        }
        else
          throw new UnsupportedException("Supplying a context id on URL is not supported in 3.5 launching");

        if (StorageGateway.IsLocked(request.Uids))
          throw new LaunchException("The requested dataset or one of it's dependencies are locked");

        Logging.Log.Information(string.Format("Retrieving the launch list to check for errors ({0})", request.Uids), "Launcher", "Launch");

        // Get the launch list to make sure there are no errors
        string launchList = StorageGateway.GetLaunchList(request.Application, request.Uids, request.UserName, request.Description, request.NewSession);


        // Parse the launch list to ge the session UID
        doc.LoadXml(launchList);

        session.Uid         = doc.DocumentElement.Attributes["uid"].Value;
        session.Arguments = string.Format("-uid \"{0}\" -user \"{1}|{2}\" ", session.Uid, request.UserName, request.Password);

        /// Lock the session 
        StorageGateway.LockSession(session, request.UserName);

        /// launch
        session.Launch(windowsSession);

        return session;

      }
      catch (Exception ex)
      {
        Logging.Log.Error(string.Format("Failed to launch {0} : {1}", request.Application, ex.Message), "Launcher", "Launch");
        throw;
      }
    }



    #endregion
  }
}
