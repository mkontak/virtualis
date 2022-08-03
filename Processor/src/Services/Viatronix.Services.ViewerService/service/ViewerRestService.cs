// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Service.cs
//
// Description: Implements the HTTP Rest service.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;
using System.ServiceModel;
using System.ServiceModel.Web;
using System.Text;
using System.Xml;
using System.IO;
using System.Reflection;
using Viatronix;
using Viatronix.Services.Messages;
using Viatronix.Launching;
using System.Configuration;
using System.Diagnostics;


namespace Viatronix.Services
{

  /// <summary>
  /// Implements the HTTP rest service. This service needs to be implemented as a singleton so that it is not disposed of by the ServiceHost after it has processed 
  /// the request.
  /// </summary>
  public class ViewerRestService : IViewerRestService, IDisposable
  {

 
    #region fields

    /// <summary>
    /// Launcher type (Version based launcher)
    /// </summary>
    private static Type m_launcher = null;

 
    /// <summary>
    /// Authorize flag
    /// </summary>
    private static bool m_authorize = (ConfigurationManager.AppSettings["authorize"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["authorize"]) : true);

    /// <summary>
    /// Session list
    /// </summary>
    private Dictionary<string, Session> m_sessions = new Dictionary<string, Session>();

    /// <summary>
    /// authentication string (should be encrypted)
    /// </summary>
    private string m_authentication = (ConfigurationManager.AppSettings["authentication"] != null ? ConfigurationManager.AppSettings["authentication"] : string.Empty);

    /// <summary>
    /// windows session user for launching processes
    /// </summary>
    private List<string> m_windowsSessionUsers = new List<string>();

    /// <summary>
    /// Specification section
    /// </summary>
    private Viatronix.Launching.Configuration.SpecificationsSection m_specificationSection = null;

    /// <summary>
    /// Session for displaying error message
    /// </summary>
    private Win32.PROCESS_INFORMATION m_errorViewerProcess = new Win32.PROCESS_INFORMATION();

    /// <summary>
    /// Error Viewer Title
    /// </summary>
    private string m_errorViewerTitle = (ConfigurationManager.AppSettings["errortitle"] != null ? ConfigurationManager.AppSettings["errortitle"] : "Launch Error");

    private string m_errorViewer = (ConfigurationManager.AppSettings["errorviewer"] != null ? ConfigurationManager.AppSettings["errorviewer"] : "ErrorViewer.exe");

    /// <summary>
    /// Dispose flag
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    public ViewerRestService()
    {
      if ( ConfigurationManager.AppSettings["launcher"] == null )
        throw new ConfigurationErrorsException("No launcher specified!");

      m_launcher = Type.GetType((string)ConfigurationManager.AppSettings["launcher"]);

      m_specificationSection = Viatronix.Launching.Configuration.SpecificationsSection.GetSection();

      string users = (ConfigurationManager.AppSettings["sessionusers"] != null ? ConfigurationManager.AppSettings["sessionusers"] : string.Empty);
    
      string [] list = users.Split(new char [] { ',' });

      foreach( string user in list )
      {
        m_windowsSessionUsers.Add(user);
      }

    } // ViewerRestService()

    /// <summary>
    /// Disposes of the service
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Disposes of the service, terminate all sessions.
    /// </summary>
    /// <param name="disposing"></param>
    protected void Dispose(bool disposing)
    {

      if (!m_disposed)
      {
        if (disposing)
        {

          Logging.Log.Debug("Disposing of Viewer Rest Service", "ViewerRestService", "Dispose");

          // ========================================================================================================================
          // Create and copy the session list to another array so that we can iterate through it. This is done since the session
          // is removed from the main list when terminated and would interfere with the enumeration.
          // ========================================================================================================================
          Session[] sessions = new Session[m_sessions.Keys.Count];
          m_sessions.Values.CopyTo(sessions, 0);

          // =================================
          // Loop through all the sessions
          // ===============================
          foreach (Session session in sessions)
          { 
            session.Terminate();                    // Terminate Session
            System.Threading.Thread.Sleep(500);     // Sleep a bit to allow the session to terminate
          } // END ... For each session
        }

        m_disposed = true;

      } // END ... If not already disposed
    }

    #endregion


    #region properties

    protected static Type LauncherType
    {
      get
      {
        if (ConfigurationManager.AppSettings["launcher"] == null)
          throw new ConfigurationErrorsException("No launcher specified!");

        string typename = (string)ConfigurationManager.AppSettings["launcher"];

        return Type.GetType(typename);


      }
    }

    #endregion

    #region Processing Methods

    /// <summary>
    /// Handles ECHO command which is like a PING. Will respond back to the calling entity with the message sent.
    /// </summary>
    /// <returns>Response XML</returns>
    public string Echo(string message)
    {
      return message;
    } // Echo(string message)

 



    /// <summary>
    /// Process the HTTP GET REQUEST which launches the specified viewer
    /// </summary>
    /// <returns>Returns the XML result data or error</returns>
    public string Launch()
    {

      WindowsSession windowsSession = null;
    
      try
      {

        /// Terminates the error viewer prior to processing the request
        TerminateErrorViewer();

        /// Get the launch request (URL)
        LaunchRequest request = LaunchRequest.Create(WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters);


        List<string> windowsSessionUsers = new List<string>();
        
        // ================================================================
        // If the windows session user was supplied then override the list 
        // ================================================================
        if (!string.IsNullOrEmpty(request.WindowsSessionUser))
          windowsSessionUsers.Add(request.WindowsSessionUser);
        else
        {
          foreach(string user in m_windowsSessionUsers)
          {
            windowsSessionUsers.Add(user);
          }
        }

        /// Get the windows session for launching
        windowsSession = Viatronix.Launching.Utilities.GetActiveWindowsSession(windowsSessionUsers);

    

        // Create the launcher specified in the app.config
        ILauncher launcher = (ILauncher)System.Activator.CreateInstance(LauncherType);


        // ======================================================================================================
        // If the original request does not has authentication information (username/password) then we will
        // use the authentication information in the server.
        // =======================================================================================================
        if (!request.HasAuthenticationInformation)
        {
          if (!string.IsNullOrEmpty(m_authentication))
          {
            string decrypt = Viatronix.Encryption.Crypto.Decrypt(m_authentication);

            string[] fields = decrypt.Split(new char[] { '|' });

            if (fields.Length != 2)
              throw new System.Configuration.ConfigurationErrorsException("Authentication string is invalid!");

            request.UserName = fields[0];
            request.Password = fields[1];
          }
          else
            throw new Viatronix.Launching.InvalidDataException("Missing authentication information, cannot log into the V3D System!");
        } // END ... If there is not authentication information



        Logging.Log.Information(string.Format("Launching {0} using version {1} of the launcher under {2}'s session",
                                      request.Application,
                                      launcher.Version,
                                      windowsSession.UserName), "ViewerRestService", "Launch");

        /// Launch the viewer
        Session session = launcher.Launch(request, m_specificationSection, windowsSession);

        // Hook up the exited event
        session.Exited += new EventHandler(this.SessionExitHandler);

        Logging.Log.Debug(string.Format("Adding session {0} to the list", session.Uid), "ViewerRestSerive", "Launch");

        // Add to the list
        m_sessions.Add(session.Uid, session);

        // Return the session UID
        return string.Format("<rsp code=\"0\" fmt=\"xml\"><session uid=\"{0}\"/></rsp>", session.Uid);


      }
      catch ( AlreadyRunningException e )
      {
        Logging.Log.Error(e.Message, "ViewerRestService", "LaunchViewer");

        // We already have a viewer don't display error viewer
        //LaunchErrorViewer(windowsSession, e.Message);

        return string.Format("<rsp code=\"-1\" fmt=\"text\">{0}</rsp>", e.Message);

      }
      catch (Exception e)
      {
        

        Logging.Log.Error(e.Message, "ViewerRestService", "LaunchViewer");

        LaunchErrorViewer(windowsSession, e.Message);

        return string.Format("<rsp code=\"1\" fmt=\"text\" >{0}</rsp>", e.Message);

       
      }
      finally
      {
        if (windowsSession != null)
          windowsSession.Dispose();

      }



    } // ProcessRequest(string resource, string operation)

    /// <summary>
    /// Terminate the old error viewer  if it is up
    /// </summary>
    private void TerminateErrorViewer()
    {
      // If the error viewer 
      if (m_errorViewerProcess.hProcess != IntPtr.Zero)
      {
        try 
        {
          // Make sure the process is still running
          Process process = Process.GetProcessById((int)m_errorViewerProcess.dwProcessId);

           Win32.TerminateProcess(m_errorViewerProcess.hProcess, 999);

        }
        catch ( Exception )
        {
          Utilities.CloseHandle(m_errorViewerProcess, "Error View");
        }
      }
    }

    private void LaunchErrorViewer(WindowsSession windowsSession, string errorMessage)
    {

      if (windowsSession != null)
      {
        Logging.Log.Error(string.Format("Launching Error Viewer [{0}]", m_errorViewer), "ViewerRestServices", "LaunchErrorViewer");

        string executable = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), m_errorViewer);

        if (System.IO.File.Exists(executable))
        {

          string arguments = string.Format("\"{0}\" \"{1}\"", m_errorViewerTitle, errorMessage);

          m_errorViewerProcess = Utilities.Launch(windowsSession, executable, arguments);
        }
        else
          Logging.Log.Error(string.Format("Error Viewer [{0}] was not found", executable), "ViewerRestServices", "LaunchErrorViewer");

      }
    }

    /// <summary>
    /// Session Exit handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void SessionExitHandler(object sender, EventArgs args)
    {

      Session session = (Session)sender;

      Logging.Log.Information(string.Format("Session ({0}) exited {1}, closing session", session.Uid, session.ExitCode), "ViewerRestService", "SessionExitHandler");

      m_sessions.Remove(session.Uid);


    }

 
    #endregion

  } //

}


