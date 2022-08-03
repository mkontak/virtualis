// Copyright© 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File:  LaunchedViewer.cs
//
// Description: 
//
// Owner: (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Configuration;



namespace Viatronix.Launching
{


  public class Session : IDisposable
  {

    #region fields

    /// <summary>
    /// Flag indicating the context was the supllied context
    /// </summary>
    private bool m_suppliedContext = false;

     /// <summary>
    /// Audit file name
    /// </summary>
    private static string m_auditFileName = (ConfigurationManager.AppSettings["audit"] != null ? ConfigurationManager.AppSettings["audit"] : string.Empty);


    /// <summary>
    /// Start time
    /// </summary>
    private DateTime? m_started = null;

    /// <summary>
    /// Date Ended
    /// </summary>
    private DateTime? m_ended = null;

    /// <summary>
    /// Session Uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Viewers argument list
    /// </summary>
    private string m_arguements = string.Empty;

    /// <summary>
    /// Viewer executable and path
    /// </summary>
    private string m_executable = string.Empty;

    /// <summary>
    /// Viewer name (Executable without directory and extension)
    /// </summary>
    private string m_name = string.Empty;


    /// <summary>
    /// Exit code
    /// </summary>
    private int m_exitCode = 0;


    /// <summary>
    /// Context Id
    /// </summary>
    private string m_context = string.Empty;


    /// <summary>
    /// Version specific launcher
    /// </summary>
    private ILauncher m_launcher = null;


    /// <summary>
    ///  Transaction id used inf 4.0 for locks
    /// </summary>
    private string m_transaction = string.Empty;


    /// <summary>
    ///  Initial process information
    /// </summary>
    private Win32.PROCESS_INFORMATION m_processInfo = new Win32.PROCESS_INFORMATION();


    /// <summary>
    /// Task to wait for process to end
    /// </summary>
    private System.Threading.Tasks.Task m_processWaitTask;

    /// <summary>
    /// Application launch request
    /// </summary>
    private ILaunchRequest m_request;

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// Windows Session
    /// </summary>
    private WindowsSession m_windowsSession = null;



    private string m_user = string.Empty;

    /// <summary>
    /// Users password
    /// </summary>
    private string m_password = string.Empty;

    #endregion


    #region constructors

    /// <summary>
    /// Constructs a viewer object with the specified application
    /// </summary>
    /// <param name="application">Application id</param>
    /// <param name="user">User</param>
    /// <param name="launcher">Launcher object</param>
    public Session(ILaunchRequest request,  ILauncher launcher)
    {

      // initialize the process information
      m_processInfo.dwProcessId = 0;
      m_processInfo.dwThreadId = 0;
      m_processInfo.hProcess = IntPtr.Zero;
      m_processInfo.hThread = IntPtr.Zero;


      m_launcher = launcher;
      m_request = request;

      m_context = m_request.ContextId;
      m_suppliedContext = (!string.IsNullOrEmpty(m_context));



    }

    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    }
    
    /// <summary>
    /// Disposes of the session
    /// </summary>
    /// <param name="disposing"></param>
    protected void Dispose(bool disposing)
    {

      if (!m_disposed)
      {
        if (disposing)
        {

          if ( m_windowsSession != null )
            m_windowsSession.Dispose();

          // close the session
          Close();

          Utilities.CloseHandle(m_processInfo, "session");

        }

      } // END ... If not already disposed
    }

    #endregion

    #region properties

    /// <summary>
    /// Flag indicating that the context was supplied
    /// </summary>
    public bool IsSuppliedContext
    { get {  return m_suppliedContext;} }

    /// <summary>
    /// Flag indicating that the session has a context
    /// </summary>
    public bool HasContext
    { get { return ( ! string.IsNullOrEmpty(m_context) ); } }

    /// <summary>
    /// Gets the sessions process id
    /// </summary>
    public uint Pid
    { get { return m_processInfo.dwProcessId;  } }

    /// <summary>
    /// Sets/Gets the session uid
    /// </summary>
    public string Uid
    { 
      get { return m_uid;  }
      set { m_uid = value; }

    }

    /// <summary>
    /// Transaction id
    /// </summary>
    public string TransactionId
    {
      get { return m_transaction;  }
      set { m_transaction = value;  }
    }

    /// <summary>
    /// Sets/Gets the context id
    /// </summary>
    public string ContextId
    {
      get { return m_context; }
      set { m_context = value; m_suppliedContext = false; }

    }

    /// <summary>
    /// Gets the exit code
    /// </summary>
    public int ExitCode
    { get { return m_exitCode;  } }


    /// <summary>
    /// Determines in the session is running
    /// </summary>
    public bool IsRunning
    {  
      get 
      {
        bool running = false;

        if (m_processInfo.dwProcessId > 0)
        {
          try
          {
            Process process = Process.GetProcessById((int)m_processInfo.dwProcessId);

            if (!process.HasExited)
              running = true;
          }
          catch ( Exception )
          {
            // Ignore
          }
        }

        return running; 
      }
    }

    /// <summary>
    /// Gets/Sets the arguments
    /// </summary>
    public string Arguments
    {
      set { m_arguements = value;  }
      get { return m_arguements;  }
    }

    /// <summary>
    /// Gets/Sets the executable
    /// </summary>
    public string Executable
    {
      set { m_executable = value; m_name = System.IO.Path.GetFileNameWithoutExtension(m_executable);  }
      get { return m_executable; }
    }

    /// <summary>
    /// Gets the request associated with this session
    /// </summary>
    public ILaunchRequest Request
    { get { return m_request;  } }

    /// <summary>
    /// Name of the application (FileName without the extension)
    /// </summary>
    public string Name
    { get { return m_name; } }

    /// <summary>
    /// Returns the time the session took
    /// </summary>
    public TimeSpan Time
    {
      get
      {
        if (m_ended == null)
          m_ended = DateTime.Now;

        return m_ended.Value.Subtract(m_started.Value);

      }
    } // Time

    #endregion

    #region events

    /// <summary>
    /// Exited event get raised when the viewer exits
    /// </summary>
    public event EventHandler Exited;

    #endregion

    #region methods

    /// <summary>
    /// Scrubs the password from the input string
    /// </summary>
    /// <param name="input"></param>
    /// <returns></returns>
    public string Scrub(string input)
    {
      return input.Replace(Request.Password, "**********");
    }


    /// <summary>
    /// Write the audit entry to the file specified
    /// </summary>
    /// <param name="filePath"></param>
    public void WriteAuditEntry()
    {

      Logging.Log.Information(string.Format("Application: {0}   User: {1}   Alias: {2}   Time: {3}", m_request.Application, m_request.UserName, m_request.Alias, Time), "Session", "WriteSuditEntry");


      if (!string.IsNullOrEmpty(m_auditFileName))
      {

        string filePath = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), Path.Combine(@"Viatronix\Logs", m_auditFileName));

        string entry = string.Format("{0}|{1}|{2}|{3}|{4}|{5}|{6}", m_request.Application, m_request.UserName, m_request.Alias, m_started.Value.ToString("G"), m_ended.Value.ToString("G"), Time, m_exitCode);


        using (FileStream fs = File.Open(filePath, FileMode.Append, FileAccess.Write))
        {

          ASCIIEncoding encoding = new ASCIIEncoding();


          fs.Write(encoding.GetBytes(entry), 0, entry.Length);

          fs.Write(encoding.GetBytes(System.Environment.NewLine), 0, System.Environment.NewLine.Length);

          fs.Close();
        }

      }

    }

 
    /// <summary>
    /// Terminates the session
    /// </summary>
    public void Terminate()
    {

      uint exitCode = 999;

      // ==================================
      // Make sure the session is running
      // ==================================
      if (IsRunning)
      {
      
        Logging.Log.Information(string.Format("Terminating {0} session PID: {1}", m_name, m_processInfo.dwProcessId), "Session", "Terminate");
       
        // ======================================
        // Make sure the process handle is valid
        // ======================================
        if (m_processInfo.hProcess != IntPtr.Zero)
        {

          // ==================
          // Terminate process
          // ===================
          if (!Win32.TerminateProcess(m_processInfo.hProcess, exitCode))
            Logging.Log.Error(string.Format("Failed to terminate process {0} : {1}", m_processInfo.dwProcessId, Marshal.GetLastWin32Error()), "Session", "Terminate");

        } // END ... If there is a process handle

      } // END ... If the process is running

    }


    /// <summary>
    /// Launches the application session
    /// </summary>
    /// <param name="sessionUser">Session user to look for</param>
    public void Launch(WindowsSession windowsSession)
    {


      /// Start date/time
      m_started = DateTime.Now;

      Logging.Log.Information(string.Format("Session ............. {0}", m_uid), "Session", "Launch");
      Logging.Log.Information(string.Format("Executable .......... {0}", m_executable), "Session", "Launch");
      Logging.Log.Information(string.Format("Arguments ........... {0}", m_arguements), "Session", "Launch");

      m_processInfo = Utilities.Launch(windowsSession, this);

      // ==============================
      // Set up a process wait task
      // =============================
      m_processWaitTask = System.Threading.Tasks.Task.Factory.StartNew(() =>
      {

        Logging.Log.Debug(String.Format("Waiting for {0} process to completed", m_name), "Session", "Launch");

        if (Win32.WaitForSingleObject(m_processInfo.hProcess, Win32.INFINITE) == Win32.WAIT_FAILED)
          Logging.Log.Error(String.Format("Failed while waiting for {0} to complete, error {1}", m_name, Marshal.GetLastWin32Error()), "Session", "Launch");

        Logging.Log.Debug("Firing the Exit handler", "Session", "Launch");

        /// Gets the exit code from the process
        Win32.GetExitCodeProcess(m_processInfo.hProcess, ref m_exitCode);

        /// Close the session
        Close();

        WriteAuditEntry();

        // Fire Exited events
        if (Exited != null)
          Exited(this, EventArgs.Empty);


      });
        

    }

    /// <summary>
    /// Closes the session
    /// </summary>
    private void Close()
    {
      // =====================================
      // We only close if we are not running
      // =====================================
      if ( ! IsRunning )
      {
        Logging.Log.Information(String.Format("{0}: session {1} has exited {2}", m_name, m_uid, m_exitCode), "Session", "ExitHandler"); 

        try
        {
          m_launcher.CloseSession(this);
        }
        catch (Exception )
        {
          // Do Nothing
        }
        finally
        {
          m_ended = DateTime.Now;
        }

      } // END ... If not running

    }


 

 
    #endregion

  }
}
