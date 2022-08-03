using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Viatronix.Launching
{

  /// <summary>
  /// Utilities class contains utility functions
  /// </summary>
  public static class Utilities
  {

    private struct ProcessInfo
    {
      public ProcessInfo(string executable, string arguments, string commandLine, string workingDircetory)
      {
        Executable = executable;
        Arguments = arguments;
        CommandLine = commandLine;
        WorkingDirectory = workingDircetory;
      }

      public string Executable;
      public string CommandLine;
      public string Arguments;
      public string WorkingDirectory;
    }

    /// <summary>
    /// Determines if we can launch a new process of the name specified. 
    /// </summary>
    /// <param name="name">Process name</param>
    /// <param name="max">Maximum number of instances allowed</param>
    /// <returns>True if we can launch, false if we cannot</returns>
    public static bool CanLaunch(string name, int max = 1)
    {
      bool canLaunch = true;

      // =====================================
      // Make sure the application is not running
      // ======================================
      Process [] processes = Process.GetProcessesByName(name);
      if (processes.Length >= max)
        canLaunch = false;


      return canLaunch;

    }   // CanLaunch()


    /// <summary>
    /// Gets the active session id ignoring session 0 which is special. If there are no active session besides 0 then an exception is thrown. If the 
    /// user is specified then the owner of the session is validated against this user to make sure we launch on the specified users session.
    /// </summary>
    /// <param name="user">User to launch under</param>
    /// <returns></returns>
    public static WindowsSession GetActiveWindowsSession(List<string> users)
    {
 
      string userList = users.ToDelimtedString(',');

      Logging.Log.Debug(string.Format("Attempting to find active windows terminal session for {0}", (string.IsNullOrEmpty(userList) ? " any user" : " " + userList)), "Utilities", "GetActiveWindowsSession");

      WindowsSession session = null;

      IntPtr ppSessionInfo = IntPtr.Zero;
      Int32 count   = 0;
      Int32 retval  = Win32.WTSEnumerateSessions(IntPtr.Zero, ref ppSessionInfo, ref count);
      Int32 dataSize = Marshal.SizeOf(typeof(Win32.WTS_SESSION_INFO));

      Int64 current = (Int64)ppSessionInfo;

      try
      {

        // ==============================
        // Make sure we have sessions
        // ==============================
        if (count > 0)
        {

          Logging.Log.Debug(string.Format("Found {0} windows terminal sessions", count), "Utilities", "GetActiveWindowsSession");

          int i = 0;

          do
          {

            // Get the session info
            Win32.WTS_SESSION_INFO si = (Win32.WTS_SESSION_INFO)Marshal.PtrToStructure((System.IntPtr)current, typeof(Win32.WTS_SESSION_INFO));
            current += dataSize;

            // =======================
            // We ignore session 0
            // =====================
            if (si.SessionId != 0)
            {

              Logging.Log.Debug(string.Format("Found windows terminal session {0} : ({1}) {2}",
                                              si.SessionId,
                                              (uint)si.State,
                                              (si.State == Win32.WTS_CONNECTSTATE_CLASS.WTSActive ? "Active" : "Not Active")),
                                              "Utilities", "GetActiveWindowsSession");


              WindowsSession wts = WindowsSession.Create(si);

              if (wts.CanUse(users))
                session = wts;

            } // END .. If we are not session id 0
            else
              Logging.Log.Debug("Ignoring session 0", "Utilities", "GetActiveWindowsSession");

            i++;

          } while (session == null && i < count);


        } // END .. If there are active sessions
        else
          throw new NoActiveSessionsException("No active sessions found!");

        if (session != null)
          Logging.Log.Information(String.Format("Found active session Id: {0}  User: {1}", session.Id, session.UserName), "Utilities", "GetActiveWindowsSession");
        else
          throw new NoActiveSessionsException(string.Format("No usable active sessions found {0}", (string.IsNullOrEmpty(userList) ? "" : string.Format(", {0} needs to be logged in and active", userList))));

      }
      finally
      {

        // Must free the memory return be WTS for the session information
        Win32.WTSFreeMemory(ppSessionInfo);

      }

      return session;

    } // GetActiveWindowsSession(string user)


    /// <summary>
    /// Launches the  executable on the supplied windows session with the arguments
    /// </summary>
    /// <param name="windowsSession">Windows session to launch on</param>
    /// <param name="session">Associated session</param>
    /// <returns></returns>
    public static Win32.PROCESS_INFORMATION Launch(WindowsSession windowsSession, Session session)
    {
      // Get the working directory
      string workingDirectory = System.IO.Path.GetDirectoryName(session.Executable);

      // Need to combine the executable and the arguments
      string commandLine = string.Format("{0} {1}", session.Executable, session.Arguments);



      Logging.Log.Information(string.Format("Executable .......... {0}", session.Executable), "Utilities", "Launch");
      Logging.Log.Information(string.Format("Arguments ........... {0}", session.Scrub(session.Arguments)), "Utilities", "Launch");
      Logging.Log.Information(string.Format("Working Directory ... {0}", workingDirectory), "Utilities", "Launch");
      Logging.Log.Information(string.Format("Command Line ........ {0}", session.Scrub(commandLine)), "Utilities", "Launch");


      return Launch(windowsSession, new ProcessInfo(session.Executable, session.Arguments, commandLine, workingDirectory));

    }

        /// <summary>
    /// Launches the  executable on the supplied windows session with the arguments
    /// </summary>
    /// <param name="windowsSession">Windows session to launch on</param>
    /// <param name="executable">Executable (Full Path)</param>
    /// <param name="arguments">Arguments</param>
    /// <param name="session">Associated session</param>
    /// <returns></returns>
    public static Win32.PROCESS_INFORMATION Launch(WindowsSession windowsSession, string executable, string arguments)
    {
      // Get the working directory
      string workingDirectory = System.IO.Path.GetDirectoryName(executable);

      // Need to combine the executable and the arguments
      string commandLine = string.Format("{0} {1}", executable, arguments);



      Logging.Log.Information(string.Format("Executable .......... {0}", executable), "Utilities", "Launch");
      Logging.Log.Information(string.Format("Arguments ........... {0}", arguments), "Utilities", "Launch");
      Logging.Log.Information(string.Format("Working Directory ... {0}", workingDirectory), "Utilities", "Launch");
      Logging.Log.Information(string.Format("Command Line ........ {0}", commandLine), "Utilities", "Launch");


      return Launch(windowsSession, new ProcessInfo(executable, arguments, commandLine, workingDirectory));
    }

    /// <summary>
    /// Launches the  executable on the supplied windows session with the arguments
    /// </summary>
    /// <param name="windowsSession">Windows session to launch on</param>
    /// <param name="commandLine">Command line</param>
    /// <param name="workingDirectory">Working directory</param>
    /// <returns></returns>
    private static Win32.PROCESS_INFORMATION Launch(WindowsSession windowsSession, ProcessInfo info )
    {

      // Duplicate user token
      IntPtr hDuplicateUserToken = IntPtr.Zero;

      Win32.PROCESS_INFORMATION processInfo = new Win32.PROCESS_INFORMATION();

      try
      {

        Win32.SECURITY_ATTRIBUTES sa = new Win32.SECURITY_ATTRIBUTES();
        sa.Length = Marshal.SizeOf(sa);


        // copy the access token of the winlogon process; 
        // the newly created token will be a primary token
        if (!Win32.DuplicateTokenEx(windowsSession.UserToken, (uint)Win32.TOKEN_ACCESS.TOKEN_ALL_ACCESS, ref sa,
                                    (int)Win32.SECURITY_IMPERSONATION_LEVEL.SecurityIdentification,
                                    (int)Win32.TOKEN_TYPE.TokenPrimary, out hDuplicateUserToken))
          throw new LaunchException(String.Format("Failed to duplicate access token, error {0}", Marshal.GetLastWin32Error()));


        Win32.STARTUPINFO si = new Win32.STARTUPINFO();
        si.cb = (int)Marshal.SizeOf(si);

        // interactive window station parameter; basically this indicates 
        // that the process created can display a GUI on the desktop
        si.lpDesktop = @"winsta0\default";



        Logging.Log.Information(string.Format("Desktop ............. {0}", si.lpDesktop), "Utilities", "Launch");


        // flags that specify the priority and creation method of the process
        Win32.CreationFlags dwCreationFlags = Win32.CreationFlags.NORMAL_PRIORITY_CLASS | Win32.CreationFlags.CREATE_NEW_CONSOLE;






        // create a new process in the current User's login session
        bool result = Win32.CreateProcessAsUser(hDuplicateUserToken,      // user access token
                                                null,                     // file to execute
                                                info.CommandLine,              // command line
                                                ref sa,                   // pointer to process SECURITY_ATTRIBUTES
                                                ref sa,                   // pointer to thread SECURITY_ATTRIBUTES
                                                false,                    // handles are not inheritable
                                                (uint)dwCreationFlags,    // creation flags
                                                IntPtr.Zero,              // pointer to new environment block 
                                                info.WorkingDirectory,         // name of current directory 
                                                ref si,                   // pointer to STARTUPINFO structure
                                                out processInfo  // receives information about new process
                                                );

        if (!result)
          throw new LaunchException(String.Format("Failed to create a user process, error {0}", Marshal.GetLastWin32Error()));


        return processInfo;

      }
      finally
      {
        Win32.CloseHandle(hDuplicateUserToken, "Duplicate User Token");
      }

    } // Launch()


    /// <summary>
    /// Closes the handles associated with the process info
    /// </summary>
    /// <param name="processInfo"></param>
    public static  void CloseHandle(Win32.PROCESS_INFORMATION processInfo, string type)
    {

      Logging.Log.Debug(string.Format("Closing {0} process information handles", type), "Utilities", "CloseHandle");

      /// Cleanup the process information
      if (processInfo.hProcess != IntPtr.Zero)
      {
        Win32.CloseHandle(processInfo.hProcess, string.Format("{0} process",type) );
        processInfo.dwProcessId = 0;
      }

      if (processInfo.hThread != IntPtr.Zero)
      {
        Win32.CloseHandle(processInfo.hThread, string.Format("{0} thread", type));
        processInfo.dwThreadId = 0;
      }

    }


    #region Extension Methods

    /// <summary>
    /// Generates a delimited list of string values ( value1|value2|...)
    /// </summary>
    /// <param name="list"></param>
    public static string ToDelimtedString(this List<string> list, char delimiter)
    {

      string delimitedList = string.Empty;

      foreach (string str in list)
      {
        delimitedList = delimitedList + str + delimiter;
      }

      return delimitedList.TrimEnd(new char[] { delimiter });
    }

    /// <summary>
    /// Determines if the value is contained in the list
    /// </summary>
    /// <param name="list">lust</param>
    /// <param name="ignoreCase">Ignore the case</param>
    /// <param name="value">Values to compare against</param>
    public static bool Contains(this List<string> list, string value, bool ignoreCase = false)
    {
      bool contains = false;

      foreach (string str in list)
      {
        if (string.Compare(str, value, ignoreCase) == 0)
        {
          contains = true;
          break;
        }
      }

      return contains;
    }

    #endregion

  } // class Utilities




} //namespace  Viatronix.Launching
