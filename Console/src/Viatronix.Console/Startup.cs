// $Id: Startup.cs,v 1.42.2.3 2009/02/06 19:19:12 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Configuration;
using System.Windows.Forms;
using Viatronix.UI;
using System.Xml;
using System.Threading;
using Viatronix.v3D.Core;

using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{	
  /// <summary>
  /// Provides the main function for the application
  /// </summary>
  public class Startup
  {
    [STAThread]
    static void Main(string[] args) 
    {

      // Need to add a check such that multiple instances
      // of console can not be run.  If this is the case
      // it will return at that point.

      string strModName = Process.GetCurrentProcess().MainModule.ModuleName;
      string strProcName = System.IO.Path.GetFileNameWithoutExtension(strModName);

      System.Windows.Forms.Application.EnableVisualStyles();

      Process[] appProc = System.Diagnostics.Process.GetProcessesByName(strProcName);
      if( appProc.Length > 1 )
      {
        // Log a warning.
        Viatronix.Logging.Log.Error("Attempted to run multiple instances of Console, bailing out.", "Startup", "Main");

        // Inform the user that the open process should be closed.
        Viatronix.UI.MessageBox.Show("Viatronix.Console is already running.\nPlease close the " +
          "running program and try again.\nIf the program is not visible please ensure that it doesn't " +
          "appear in the process table.", "Multiple instances running.", MessageBoxButtons.OK, 
          MessageBoxIcon.Error);
        
        return;
      } // if( appProc.Length > 1 )


      try
      {
        #if ( DEBUG )
            Thread.GetDomain().UnhandledException += new UnhandledExceptionEventHandler( UnhandledException );
        #endif

        //GetConnectionStrings();

        InitializeLog();


        //if (!Global.SecurityProvider.IsAvailable || !Global.DataProvider.IsAvailable)
        //{
        //  Viatronix.UI.MessageBox.ShowError("V3D Enterprise Server is unavailable. Please contact Viatronix customer support.");
        //  return;
        //}
        //if (!Global.DataProvider.IsAvailable)
        //{
        //  Viatronix.UI.MessageBox.ShowError("V3D Enterprise Server is unavailable. Please contact Viatronix customer support.");
        //  return;
        //}





        InitializeTheme();


        // Create the global frame
        Global.Frame = new FrameConsole();
        //Global.Frame.Version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;

        // =======================================================================
        // Attempt login, but if no User is returned the user canceled the login
        // operation so we just return.
        // ======================================================================
        if (Global.Frame.Login() == null)
          return;

        // Start the authentication renewl process
        Global.Frame.StartRenewalTimer();
       
        // comment out this line to the thread exception handler.
        Global.Frame.EnableThreadExceptionHandler();

        
        // run with the main form
        System.Windows.Forms.Application.Run( Global.Frame );

      }
      catch ( Exception ex )
      {

        Viatronix.Logging.Log.Error(ex.Message, "Startup", "Main");
        Viatronix.UI.MessageBox.ShowError ( "Failed v3D Console initialization. Please contact Viatronix customer support."  );

//#if !DEBUG
        try
        {

          if (Global.Frame != null )
          { 
            Global.Frame.Logout();
            //user.LoginServer = string.Empty;
            //Global.UMAS.UpdateUser(user);
          }
        }
        catch ( System.Exception )
        {
          Viatronix.Logging.Log.Error("Failed to clear the login server for the user", "Startup", "Main");
        }
//#endif

      }
    } // Main( args )



    /// <summary>
    /// 
    /// </summary>
    private static void GetConnectionStrings()
    {

      /// Connection string source file
      string sourceFile = ConfigurationManager.AppSettings["ConnectionStrings"];

      if (string.IsNullOrEmpty(sourceFile))
        throw new System.ApplicationException("No connection strings specified");


      XmlDocument doc = new XmlDocument();
      doc.Load(sourceFile);

      string server = (doc.DocumentElement.Attributes["server"] == null ? string.Empty : doc.DocumentElement.Attributes["server"].Value);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("add"))
      {


        string name = node.Attributes["name"].Value;

        typeof(ConfigurationElementCollection).GetField("bReadOnly", System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.NonPublic).SetValue(ConfigurationManager.ConnectionStrings, false);

        var connectionStrings = ConfigurationManager.ConnectionStrings;

        string connectionString = node.Attributes["connectionString"].Value;

        if (!string.IsNullOrEmpty(server))
          connectionString = Viatronix.Encryption.Crypto.Encrypt(Viatronix.Encryption.Crypto.Decrypt(connectionString).Replace("(local)", server));

        ConnectionStringSettings settings = new ConnectionStringSettings();
        settings.Name = name;
        settings.ConnectionString = connectionString;

        ConfigurationManager.ConnectionStrings.Add(settings);        

      }

      

    }

    /// <summary>
    /// Unhandled exception to a message box.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">UnhandledExceptionEventArgs</param>
    public static void UnhandledException( object sender, UnhandledExceptionEventArgs e)
    {
        Viatronix.UI.MessageBox.ShowError( "Sender: " + sender.ToString() + " " +  e.ExceptionObject.ToString() );
     // The exception object is contained in e.ExceptionObject.
    }//UnhandledException( object sender, UnhandledExceptionEventArgs e)

    /// <summary>
    /// Initialize the Log 
    /// </summary>
    private static void InitializeLog()
    {
      Logging.Log.Initialize();
    }//InitializeLog()


    /// <summary>
    /// Initialize the theme
    /// </summary>
    private static void InitializeTheme()
    {
      try
      {
        Viatronix.UI.Theme.LoadThemeFromXml( (XmlNode) ConfigurationManager.GetSection( "theme" ) );
      }
      catch ( Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to Initialize the theme" + ex.Message, "Startup", "InitializeTheme");
        throw ex;
      }
    } //InitializeTheme()


  } // class Startup
} // namespace Viatronix.Console

#region revision history

// $Log: Startup.cs,v $
// Revision 1.42.2.3  2009/02/06 19:19:12  kchalupa
// Region crashproofing.
//
// Revision 1.42.2.2  2008/09/16 17:13:00  kchalupa
// Issue
// 6150  No Movie Preview Console Crashes
// VCSessionViewer
// 6122 VC Sessions Not Showing Completed
// VCLauncher
// 6073 Each Movie (VC) Is Listed Twice On Patient CD
// VCSessionProvider
// 6153 Movies that have no preview do not show up in Patient CD HTML Report
// RemovableMediaForPatient
// ReportTags
//
// Revision 1.43  2008/09/04 14:11:32  kchalupa
// Issue #6137
// Needed to add a check for multiple console insances
// in the process table.
//
// Revision 1.42  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.41  2007/01/08 12:54:46  vxconfig
// no message
//
// Revision 1.40  2007/01/05 13:11:10  vxconfig
// Fix build
//
// Revision 1.39  2007/01/04 20:37:20  mkontak
// Issue 5272
//
// Revision 1.38  2007/01/04 20:36:26  mkontak
// Issue 5272
//
// Revision 1.37  2006/10/30 16:09:24  gdavidson
// Moved loading of the user and connections tring to global.
//
// Revision 1.36  2006/08/22 17:43:38  mkontak
// Fix abort when doing PACS search. Added new flag to disable the
// imbedded image query within the series query to save time.
//
// Revision 1.35  2006/07/28 20:03:27  mkontak
// Added code to check if the cache server is available before starting
//
// Revision 1.34  2006/07/19 12:50:23  romy
// extended debug message for unhandles exception
//
// Revision 1.33  2006/07/06 18:55:05  romy
// exception message box for Debug builds
//
// Revision 1.32  2006/05/05 20:39:46  gdavidson
// Issue #4679: Changed the Idle timer
//
// Revision 1.31  2006/04/10 19:13:11  mkontak
// Grab the conbsole AETitle from the config file
//
// Revision 1.30  2006/03/28 15:02:59  romy
// lockmanager reads the refresh rate from the db
//
// Revision 1.29  2006/02/21 14:56:38  gdavidson
// New version of licensing
//
// Revision 1.28  2006/01/18 16:10:50  gdavidson
// Added functionality to start an idle timer.
//
// Revision 1.27  2006/01/17 03:20:21  romy
// added auditlog ref
//
// Revision 1.26  2005/11/22 21:31:23  romy
// better  error handling.
//
// Revision 1.25  2005/11/18 20:52:22  romy
// added comment
//
// Revision 1.24  2005/11/17 23:11:31  gdavidson
// Added a check for a null user
//
// Revision 1.23  2005/11/16 19:53:06  romy
// exception handling added
//
// Revision 1.22  2005/11/16 18:48:45  romy
// added Theme Node
//
// Revision 1.21  2005/11/10 19:49:51  gdavidson
// Removed IConsoleHost
//
// Revision 1.20  2005/11/09 16:10:43  gdavidson
// Removed RemotingConsole and replace with ConsoleHost class.
//
// Revision 1.19  2005/10/12 15:49:42  gdavidson
// Changed the namespace for the FormLogin
//
// Revision 1.18  2005/10/06 15:38:10  gdavidson
// Removed ConsoleServer and replaced it with RemotingConsole.
//
// Revision 1.17  2005/08/02 12:38:12  gdavidson
// Modified section handler to acquire license before returning an instance of a frame.
//
// Revision 1.16  2005/06/14 18:47:45  mkontak
// Minor changes
//
// Revision 1.15  2005/05/26 20:45:36  mkontak
// UMAS changes
//
// Revision 1.14  2005/05/26 18:03:21  mkontak
// Changes for UMAS
//
// Revision 1.13  2005/05/25 13:32:07  mkontak
// Changes to UMAS
//
// Revision 1.12  2005/05/16 14:25:28  gdavidson
// Set the global reference to the ConsoleServer.
//
// Revision 1.11  2005/05/09 17:19:54  romy
// Added FrameConsole frame
//
// Revision 1.10  2005/05/05 20:11:05  gdavidson
// Added creation of IConsole class.
//
// Revision 1.9  2005/05/05 17:07:56  romy
// Added the User Logging details to the database.
//
// Revision 1.8  2005/05/05 12:01:22  romy
// Added all Rights to the vxService user
//
// Revision 1.7  2005/05/05 11:46:01  romy
// Added Connection String extraction from the Config File.
//
// Revision 1.6  2005/04/28 14:13:25  romy
// Changed the Role permission from bit to long
//
// Revision 1.5  2005/04/18 13:49:15  romy
// Added the vxService User in the Debug Mode.
//
// Revision 1.4  2005/03/28 22:28:06  gdavidson
// Added ConsoleFrameConfigSectionHandler class.
//
// Revision 1.3  2001/01/02 12:55:29  gdavidson
// Added manifest for WindowsXP theme.
//
// Revision 1.2  2005/02/24 19:14:36  gdavidson
// Set the active frame for the global object.
//
// Revision 1.1  2005/01/28 16:34:22  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Startup.cs,v 1.42.2.3 2009/02/06 19:19:12 kchalupa Exp $
// $Id: Startup.cs,v 1.42.2.3 2009/02/06 19:19:12 kchalupa Exp $

#endregion