// $Id: LaunchManager.cs,v 1.7.2.6 2011/05/18 01:57:00 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mkontak@viatronix.com )


using System;
using System.IO;
using System.Diagnostics;
using System.ComponentModel;
using System.Configuration;
using System.Collections.Generic;
using System.Xml;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Enterprise.Launching
{
  /// <summary>
  /// Implements functionality for launching from a Console type application.
  /// </summary>
  public sealed class LaunchManager
  {


 
    #region fields

    /// <summary>
    /// Source of the launch (Always console)
    /// </summary>
    private string m_source = "console";

    /// <summary>
    /// Applications in system (Installed only)
    /// </summary>
    List<Viatronix.Enterprise.Entities.Application> m_applications;

    /// <summary>
    /// Application being launched
    /// </summary>
    Viatronix.Enterprise.Entities.Application m_application;

    /// <summary>
    /// Process for launching
    /// </summary>
    private System.Diagnostics.Process m_process = null;


    /// <summary>
    /// Reference to an IDataProvider.
    /// </summary>
    private IDataProvider m_dataProvider = null;

     /// <summary>
    /// determines if the launch manager is enabled
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// application exited handler
    /// </summary>
    private EventHandler m_applicationExitedHandler = null;

    /// <summary>
    /// User
    /// </summary>
    private User m_user = null;

    #endregion

    #region contruction

    /// <summary>
    /// default constructor
    /// </summary>
    public LaunchManager(IDataProvider provider, User user, string source = "console")
    {
      m_dataProvider = provider;
      m_user = user;
      m_source = "console";

      /// Gets the instaled applications 
      m_applications = m_dataProvider.GetApplications();

      m_applicationExitedHandler = new EventHandler(this.OnApplicationExitedHandler);

    } // LaunchManager()

    #endregion

    #region events


    /// <summary>
    /// Fired when the application has been closed
    /// </summary>
    public event EventHandler ApplicationClosed = null;

    /// <summary>
    /// Fired when the appliucation has actuially been launched
    /// </summary>
    public event EventHandler ApplicationLaunched = null;


    #endregion


    #region properties


    /// <summary>
    /// Gets the installed application list
    /// </summary>
    public List<Viatronix.Enterprise.Entities.Application> Applications
    { get { return m_applications;  } }
      
    /// <summary>
    /// Returns a reference to the Provider.
    /// </summary>
    public IDataProvider DataProvider
    {
      get { return m_dataProvider; }
    } // DataProvider


    /// <summary>
    /// Gets or sets if the launch manager is enabled.
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      set { m_enabled = value; }
    } // Enabled


    /// <summary>
    /// Gets currently launched application
    /// </summary>
    public Viatronix.Enterprise.Entities.Application Application
    {
      get { return m_application; }
    } // Application 

    /// <summary>
    /// Returns true if we are launched
    /// </summary>
    public bool IsApplicationLaunched
    { get { return (m_process != null ? true : false); } }

    #endregion

    #region methods


    #region Study Launching

    /// <summary>
    /// Launches  the specified study using the supplied application.
    /// </summary>
    /// <param name="application">Application to launch</param>
    /// <param name="study">Study to launch</param>
    /// <param name="newSession">New session flag</param>
    public void Launch(Viatronix.Enterprise.Entities.Application application, Study study, bool newSession = false)
    {
      // Launching is enabled
      if (this.Enabled == false)
        return;

      // can only have one viewer open
      if (m_process != null)
        throw new LaunchException("Cannot launch application");

      // Make sure the application is installed
      if (!application.Installed)
        throw new NotInstalledException(string.Format("Cannot launch {0} application, not installed"));

      // Set the current launched application
      m_application = application;

      // Build the UID list
      string commandLine = string.Format(" -uid {0} {1}", study.Uid, (newSession ? " -new" : string.Empty));

      // Launch the application
      LaunchApplication(commandLine);

    }

    /// <summary>
    /// Launches  the specified study
    /// </summary>
    /// <param name="applicationId">Application to launch</param>
    /// <param name="study">Study to launch</param>
    /// <param name="newSession">New session flag</param>
    public void Launch(string applicationId, Study study, bool newSession = false)
    {

      Viatronix.Enterprise.Entities.Application application = new Enterprise.Entities.Application();

      // Make sure the application is in the installed applications list
      if (!m_applications.TryFind(applicationId, ref application))
        throw new NotInstalledException(string.Format("Cannot launch {0} application, not installed"));

     
      Launch(application, study, newSession);

    } // Launch( string applicationId, Study study, bool newSession = false )

    /// <summary>
    /// Launches  the specified study determining what application to use.
    /// </summary>
    /// <param name="study">Study to launch</param>
    /// <param name="newSession">New session flag</param>
    public void Launch(Study study, bool newSession = false)
    {

      string applicationId = string.Empty;

      List<Series> sessions = study.Series.FindAll(SeriesTypes.Session);

      // =====================================================
      // loop through applications which are in priority order
      // to see if there is a session for the application.
      // ====================================================
      foreach (Application app in m_applications)
      {
        // ==========================================================================
        // loop through sessions to see if there is one for the current application
        // ==========================================================================
        foreach (Series session in sessions)
        {
          if (session.Application == app.Id)
          {
            applicationId = app.Id;
            break;
          }
        }
      } // END ... For each session


      // ====================================================================================================
      // If no session was found for the installed applications then look for an application in the study list
      // ====================================================================================================
      if (string.IsNullOrEmpty(applicationId))
      {

        // ==================================================================================================================
        // For each application which is in priority if the study has a series marked as that then launch that application
        // ==================================================================================================================
        foreach (Application app in m_applications)
        {
          if (study.Applications.Contains(app.Id))
          {
            applicationId = app.Id;
            break;
          }
        } // END ... For each application


        // ===================================================
        // If no launchable data was found then throw exception
        // ===================================================
        if (string.IsNullOrEmpty(applicationId))
          throw new NotFoundException("No application was found to launch");
      }


      Launch(applicationId, study, newSession);

    } // Launch( Study study, bool newSession = false )



    #endregion


    #region Series Launching

    /// <summary>
    /// Launches  the specified series list using the application supplied.
    /// </summary>
    /// <param name="application">Application to be launched</param>
    /// <param name="series">List of series to be launched</param>
    /// <param name="newSession">Flag indicating we want a new session</param>
    public void Launch(Viatronix.Enterprise.Entities.Application application, List<Series> seriesList, bool newSession = false)
    {
      // Make sure Launching is enabled
      if (this.Enabled == false)
        return;

      // can only have one veiwer open
      if (m_process != null)
        throw new LaunchException("Cannot launch application");

      // Make sure the application is installed
      if (!application.Installed)
        throw new NotInstalledException(string.Format("Cannot launch {0} application, not installed"));

      // Set the current application launched
      m_application = application;

      // ================================
      // Build the -uid list 
      // ===============================
      string commandLine = " -uid ";

      foreach (Series series in seriesList)
      {

        commandLine += series.Uid + "|";
      }

      // Remove the trailing '|'
      commandLine = commandLine.TrimEnd(new char[] { '|' });


      commandLine += (newSession ? " -new" : string.Empty);

      // Actually launch the application
      LaunchApplication(commandLine);


    } // LaunchViewer( controller )


    /// <summary>
    /// Launches  the specified list of series using the application id supplied. Will make sure the application is 
    /// installed.
    /// </summary>
    /// <param name="applicationId">Application to launch</param>
    /// <param name="study">Study to launch</param>
    /// <param name="newSession">New session flag</param>
    public void Launch(string applicationId, List<Series> seriesList, bool newSession = false)
    {

      Viatronix.Enterprise.Entities.Application application = new Enterprise.Entities.Application();

      // Try to find the application in the installed application list
      if (!m_applications.TryFind(applicationId, ref application))
        throw new NotInstalledException(string.Format("Cannot launch {0} application, not installed"));

      // Launch the application
      Launch(application, seriesList, newSession);

    } // Launch(string applicationId, List<Series> seriesList, bool newSession = false)


    /// <summary>
    /// Launches  the specified series list determining the application to be launched.
    /// </summary>
    /// <param name="seriesList">List of series</param>
    /// <param name="newSession">New session flag</param>
    public void Launch( List<Series> seriesList, bool newSession = false)
    {
      
      string applicationId = string.Empty;


      Series session = new Series();

      // =======================================================================
      // Make sure if a session is supplied then that is all that in the list
      // ========================================================================
      if (seriesList.TryFindType(SeriesTypes.Session, ref session))
      {
        if (seriesList.Count > 1)
          throw new InvalidDataException("Cannot specify multi-series to launch if one is a session");

        applicationId = session.Application;
      }
      else
      {



        foreach (Application app in m_applications)
        {

          // ===============================================================
          // Loop through the series to make sure we have an application 
          // ==============================================================
          foreach (Series series in seriesList)
          {
            // ====================================
            // Make sure the series is not failed
            // =====================================
            if (series.IsFailed)
              throw new InvalidDataException("Cannot launched a failed series");

            if (series.Type == SeriesTypes.Dicom )
            {
              foreach (Volume volume in series.Volumes)
              {
                if (volume.Application == app.Id)
                {
                  applicationId = app.Id;
                  break;
                }
              }


            }
            else
            {
              if (series.Application == app.Id)
              {
                applicationId = app.Id;
                break;
              }
            }

          } // END ... For each series

          // If the application id is set then break out of the loop
          if (!string.IsNullOrEmpty(applicationId))
            break;

        } // END ... For each application

      } // END ... Else a session was not supplied

      if (string.IsNullOrEmpty(applicationId))
        throw new NotFoundException("No application was found to launch");


      Launch(applicationId, seriesList, newSession);

    } // Launch( List<Series> seriesList, bool newSession = false)

    #endregion

    /// <summary>
    /// Launches the application.
    /// </summary>
    /// <param name="commandLine">Command line containing the UID's</param>
    private void LaunchApplication(string commandLine)
    {

      try
      {

        // Creates the application process
        m_process = CreateApplicationProcess(commandLine);

        // We want the process to fire an exited event
        m_process.EnableRaisingEvents = true;

        // Hook up the exit handler
        m_process.Exited += m_applicationExitedHandler;

        // Start the process and return immediately
        m_process.Start();

      }
      catch (LaunchException e)
      {
        // Any exception reset the process to null for re-launch
        m_process = null;

        // Rethow the exception
        throw e;
      }

    } // LaunchApplication()


    #endregion




    #region event handlers

    /// <summary>
    /// Handles the exited event of the application process. 
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnApplicationExitedHandler(object sender, EventArgs args)
    {

      // Sets the process so that a news launch can be performed
      m_process = null;

      // Log information that the application has exited
      Logging.Log.Information(string.Format("{0} application has exited", m_application.Id), "LuaunchManager", "OnApplicationExitedHandler");

      // Fire any hooked up events to indicate that the application has closed.
      if (ApplicationClosed != null)
        ApplicationClosed(this, EventArgs.Empty);

    } // OnApplicationExitedHandler( sender, args )

    /// <summary>
    /// Creates the applications process to be executed
    /// </summary>
    /// <param name="commandLine">Initial command line containing UID list</param>
    /// <returns>application process</returns>
    private Process CreateApplicationProcess(string commandLine)
    {
      try
      {

        string parameters = "-" + m_source.ToString().ToLower() + " " + (m_user != null ? "-user " + m_user.Login + "||" + m_user.ContextId : "") + " " + commandLine;

        Logging.Log.Information("Launching application [PATH=" + m_application.Url + "] [PARAMETERS=" + parameters + "]", "Launcher", "CreateApplicationProcess");

        // creates the process which executes the viewer
        ProcessStartInfo info = new ProcessStartInfo(m_application.Url, parameters);
        info.WorkingDirectory = m_application.Url;
        info.ErrorDialog = false;
        info.UseShellExecute = true;

        Process process = Process.Start(info);

        System.Threading.Thread.Sleep(1000);


        return process;

      }
      catch (Exception e)
      {
        throw new LaunchException("Unable to start the application's process. [APP=" + m_application.Id + "] [ERROR=" + e.Message + "]", e);
      }
    }



    #endregion

  } // class LaunchManager

} // namespace Viatronix.Enterprise.Launching

