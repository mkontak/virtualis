// $Id: Global.cs,v 1.5.2.4 2011/05/18 01:57:00 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.UI;
using Viatronix.Enterprise.Configuration;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
  /// <summary>
  /// Global data and operations.
  /// </summary>
  public sealed class Global
  {

    #region fields

 
    /// <summary>
    /// Frame console object
    /// </summary>
    private static Viatronix.UI.Frame m_frame = null;

    /// <summary>
    /// AE Title of the console
    /// </summary>
    private static string m_aeTitle = string.Empty;
      
 
    /// <summary>
    /// Idle interval (default 10 min)
    /// </summary>
    private static int m_idleInterval = -1;

    /// <summary>
    /// lock manager object
    /// </summary>
    //private static LockManager m_lockManager = null;

    /// <summary>
    /// Site object.
    /// </summary>
    private static Site m_site = null;

    /// <summary>
    /// Application data
    /// </summary>
    private static Dictionary< string, object > m_applicationState = new Dictionary< string, object >();


    /// <summary>
    /// Study groups
    /// </summary>
    private static List<StudyGroup> m_studyGroups = null;



     #endregion

    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static Global()
    {
      if( System.Configuration.ConfigurationManager.AppSettings[ ConsoleState.AETitle ] != null )
        m_aeTitle = System.Configuration.ConfigurationManager.AppSettings[ ConsoleState.AETitle ];
    } // Global()


    /// <summary>
    /// private constructor
    /// </summary>
    private Global()
    {
    }

    #endregion

    #region properties



     /// <summary>
    /// Gets the security provider 
    /// </summary>
    public static ISecurityProvider SecurityProvider
    {
      get { return m_frame.SecurityProvider; }
    }


    /// <summary>
    /// Gets the security provider 
    /// </summary>
    public static IDataProvider DataProvider
    {
      get { return m_frame.DataProvider; }
    }



    /// <summary>
    /// Study groups 
    /// </summary>
    public static List<StudyGroup> StudyGroups
    {
      get
      {
        if (m_studyGroups == null)
        {
          m_studyGroups = Global.DataProvider.GetStudyGroups();
        }
        return m_studyGroups;
      }
    }

    ///// <summary>
    ///// Gets or sets the LockManager
    ///// </summary>
    //public static LockManager LockManager
    //{
    //  get 
    //  { 
    //    if( m_lockManager == null )
    //      m_lockManager = LockManager.CreateInstance();

    //    return m_lockManager; 
    //  }
    //} // LockManager


 

    /// <summary>
    /// Gets or Sets the Frame Object.
    /// </summary>
    public static Viatronix.UI.Frame Frame
    {
      get { return m_frame; }
      set 
      { 
        m_frame = value; 
      }
    } // Frame


    /// <summary>
    /// True if logged on
    /// </summary>
    public static bool LoggedOn
    {
      get { return ( m_frame != null ? m_frame.LoggedOn : false ); }
    } // LoggedOn

    /// <summary>
    /// Current Logged in User to Console.
    /// </summary>
    public static User CurrentUser
    {
      get { return m_frame.CurrentUser; }
     } // CurrentUser


    /// <summary>
    /// Returns the idle interval
    /// </summary>
    public static int IdleInterval
    {
      get 
      {
        if( m_idleInterval == -1 )
        {
          m_idleInterval = int.Parse( System.Configuration.ConfigurationManager.AppSettings[ CoreState.IdleInterval ]);
          m_idleInterval *= ( 60 * 1000 );
        }

        return m_idleInterval; 
      }
    } // IdleInterval


    /// <summary>
    /// Gets/Sets the aetitle
    /// </summary>
    public static string AETitle
    {
      get { return m_aeTitle; }
      set { m_aeTitle = value; }
    } // AETitle


    /// <summary>
    /// Returns the site object.
    /// </summary>
    public static Site Site
    {
      get
      {
        if (m_site == null)
        {
          m_site = Global.DataProvider.GetSite();
        }

        return m_site;
      }
    } // Site


    /// <summary>
    /// Returns a reference to any additional data.
    /// </summary>
    public static Dictionary< string, object > ApplicationState
    {
      get 
      {
        try
        {
          m_applicationState[CoreState.User] = Global.CurrentUser;
        }
        catch ( System.Security.SecurityException )
        {
          m_applicationState[CoreState.User] = null;
        }

        return m_applicationState; 
      }
    } // ApplicationState

    #endregion

    #region methods

   /// <summary>
    /// Loads the current user.
    /// </summary>
    /// <returns>The current user.</returns>
    //public static User GetUser()
    //{
    //  try
    //  {
    //    // Release mode
    //    Viatronix.Console.FormLogin login = new Viatronix.Console.FormLogin();
    //    if( login.ShowDialog() == DialogResult.Cancel )
    //      return null;
               

    //    return Global.CurrentUser;
    //  }
    //  catch ( Exception ex )
    //  {
    //    Viatronix.Logging.Log.Error ( "Failed to create a user. [ERROR="  + ex.Message + "]", "Global", "GetUser" );
    //    throw ex;
    //  }
    //} // GetUser()




    #endregion 

  } // class Global
} // namespace Viatronix.Console

#region revision history

// $Log: Global.cs,v $
// Revision 1.5.2.4  2011/05/18 01:57:00  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.3  2009/02/20 20:13:57  kchalupa
// Release mode did not display login, debug did.  Reversed to provide proper functionality on console launch.
//
// Revision 1.5.2.2  2009/02/06 19:19:13  kchalupa
// Region crashproofing.
//
// Revision 1.5.2.1  2008/09/08 18:24:24  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
// Revision 1.5  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.4  2007/02/21 21:24:25  gdavidson
// Commented code
//
// Revision 1.3  2006/11/03 14:39:36  romy
// fixed console login issue
//
// Revision 1.2  2006/11/02 20:35:36  gdavidson
// Modified the type initializer
//
// Revision 1.1  2006/10/30 15:42:55  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.24  2006/08/22 17:43:38  mkontak
// Fix abort when doing PACS search. Added new flag to disable the
// imbedded image query within the series query to save time.
//
// Revision 1.23  2006/07/28 20:03:26  mkontak
// Added code to check if the cache server is available before starting
//
// Revision 1.22  2006/06/06 19:46:29  mkontak
// changed deprecated code
//
// Revision 1.21  2006/05/09 20:17:23  gdavidson
// Added site property
//
// Revision 1.20  2006/04/10 19:13:11  mkontak
// Grab the conbsole AETitle from the config file
//
// Revision 1.19  2006/03/28 15:02:56  romy
// lockmanager reads the refresh rate from the db
//
// Revision 1.18  2006/03/22 05:34:39  romy
// lockmanager modified
//
// Revision 1.17  2006/03/22 05:16:03  romy
// lockmanager modified
//
// Revision 1.16  2006/02/10 21:34:35  gdavidson
// Changed the type for the Frame property to FrameConsole.
//
// Revision 1.15  2006/01/18 16:09:12  gdavidson
// Added an IdleInterval property.
//
// Revision 1.14  2005/12/14 19:54:51  gdavidson
// Added a ViewerController property.
//
// Revision 1.13  2005/12/12 20:54:07  romy
// added application State caching mechanism
//
// Revision 1.12  2005/11/22 21:31:30  romy
// better  error handling.
//
// Revision 1.11  2005/11/10 19:49:38  gdavidson
// Removed IConsoleHost
//
// Revision 1.10  2005/11/09 16:10:12  gdavidson
// Removed RemoteObject and replace with RemoteViewer class.
//
// Revision 1.9  2005/10/06 15:41:02  gdavidson
// Removed ConsoleServer and RemoteViewer. Added RemotingConsole and RemotingViewer.
//
// Revision 1.8  2005/05/26 18:03:21  mkontak
// Changes for UMAS
//
// Revision 1.7  2005/05/25 13:32:07  mkontak
// Changes to UMAS
//
// Revision 1.6  2005/05/16 14:22:50  gdavidson
// Added comments for ConsoleServer and RemoteViewer properties.
//
// Revision 1.5  2005/05/05 17:42:27  gdavidson
// Added ConsoleServer and RemoteViewer properties.
//
// Revision 1.4  2005/05/05 11:43:32  romy
// Added the Connection String. Temporary Solution is reading from Config file.
//
// Revision 1.3  2005/04/28 14:13:25  romy
// Changed the Role permission from bit to long
//
// Revision 1.2  2005/03/29 21:38:53  romy
// Added the Current user member for logged in users
//
// Revision 1.1  2005/02/24 19:14:06  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/misc/Global.cs,v 1.5.2.4 2011/05/18 01:57:00 mkontak Exp $
// $Id: Global.cs,v 1.5.2.4 2011/05/18 01:57:00 mkontak Exp $

#endregion