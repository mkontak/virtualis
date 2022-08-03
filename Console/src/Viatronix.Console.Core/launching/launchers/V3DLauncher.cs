// $Id: V3DLauncher.cs,v 1.11.2.7 2008/02/07 21:33:35 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Tcp;
using System.Drawing;
using System.Windows.Forms;


using Viatronix.Logging;
using Viatronix.UI;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
	/// <summary>
	/// Implements all functionality necessary to launch a v3D viewer.
	/// </summary>
	public class V3DLauncher : Launcher
	{

    #region fields


    /// <summary>
    /// Collection of datasets.
    /// </summary>
    private List<List<Series>> m_datasets = new List<List<Series>>();

 

    #endregion

    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static V3DLauncher()
    {
    } // V3DLauncher()

    #endregion

 
 
    #region methods


    ///// <summary>
    ///// Launches the viewers.
    ///// </summary>
    //private void LaunchData()
    //{
    //  try
    //  {
    //    if( this.IsSession )
    //      this.RemoteViewer.Load( this.Datasets[0][0], this.Datasets[0] );
    //    else
    //      this.RemoteViewer.New( this.Datasets.ToArray() );
    //  }
    //  catch( Exception e )
    //  {
    //    if( this.IsSession )
    //      Log.Error("The viewer failed during the load session operation. [ERROR=" + e.Message + "]", "V3DLauncher", "LaunchData");
    //    else
    //      Log.Error("The viewer failed during the new session operation. [ERROR=" + e.Message + "]", "V3DLauncher", "LaunchData");

    //    //TODO: move asynchronous launch to a common method
    //    TerminateLaunch();
    //    CleanupLaunch();

    //    // notify the user that the viewer could not start
    //    SetterDelegate< IWin32Window, string > method = new SetterDelegate< IWin32Window, string >( Viatronix.UI.MessageBox.ShowError );
    //    this.Form.Invoke( method, this.Form, "Console was unable to start the viewer." );
    //  }
    //} // LaunchData()

    #endregion

    #region IConsole Members

    /// <summary>
    /// Returns if the console supports save operations
    /// </summary>
    public virtual bool CanSave 
    { 
      get { return ( this.DataProvider != null ); }
    } // CanSave 


 

 

    /// <summary>
    /// Saves the information contained within the provided Series.
    /// </summary>
    /// <param name="session">The series being saved.</param>
    public virtual void Save( Series series )
    {
      // IDataProvider is not set
      if( this.CanSave == false )
      {
        Viatronix.Logging.Log.Debug("Cannot save the series", "V3DLuancher", "Save");
        return;
      }

      try
      {
        this.DataProvider.Update( series );
      }
      catch( NotSupportedException e )
      {
        Viatronix.Logging.Log.Warning(e.Message, "V3DLuancher", "Save");
      }

      OnUpdated();
    } // Save( session )


    /// <summary>
    /// Handles the viewer closing.
    /// </summary>
    public virtual void ViewerClosing()
    {
      try
      {       
        ViewerClosed();
      }
      catch( Exception e )
      {
        Log.Error(e.Message, "V3DLuancher", "ViewerClosing");
      }
    } // ViewerClosing()


    /// <summary>
    /// Allows the viewer to login into the system.
    /// </summary>
    /// <param name="username">user name</param>
    /// <param name="password">password</param>
    /// <returns>true if the user is allowed to login; otherwise false</returns>
    //public virtual bool Login( string username, string password )
    //{
    //  try
    //  {
    //    User user = ConsoleUtils.Login( username, password );


    //    if (user != null && ((user.Login == Global.CurrentUser.Login) || user.IsAdministratorUser || user.IsService))
    //    {
    //      m_viewerLocked = false;
    //      return true;
    //    }
    //    else
    //    {
    //      return false;
    //    }
    //  }
    //  catch( Exception e )
    //  {
    //    Log.Debug("User cannot login. [ERROR=" + e.Message + "]", "V3DLuancher", "Login");
    //    return false;
    //  }
    //} // Login( username, password )

    #endregion

    #region IWindow Members

    /// <summary>
    /// Closes the console.
    /// </summary>
    public virtual void CloseWindow()
    {
      this.Form.Close();
    } // CloseWindow()


    /// <summary>
    /// Allows a remote object to make the console visible.
    /// </summary>
    /// <param name="bounds">Restore bounds for the window</param>
    /// <param name="state">Window satte of the form</param>
    public virtual void Show( Rectangle bounds, FormWindowState state )
    {
      ShowForm( bounds, state );
    } // Show( bounds, state )


    /// <summary>
    /// Returns the process id.
    /// </summary>
    public virtual int ProcessId
    {
      get 
      { 
        return System.Diagnostics.Process.GetCurrentProcess().Id; 
      }
    } // ProcessId

    #endregion

  } // V3DLauncherLite
} // namespace Viatronix.Console

#region revision history

// $Log: V3DLauncher.cs,v $
// Revision 1.11.2.7  2008/02/07 21:33:35  mkontak
// Account for missing field during serialization
//
// Revision 1.11.2.6  2007/12/14 19:04:02  mkontak
// Upon return from the viewer launch the Console will be refreshed
//
// Revision 1.11.2.5  2007/09/10 17:17:35  mkontak
// Issue 5774
//
// Revision 1.11.2.4  2007/06/15 18:39:07  mkontak
// Issue 5686: Check the login for a valid user admin or service user
//
// Revision 1.11.2.3  2007/05/07 15:22:55  gdavidson
// Issue #5641: Invoke on main thread for TerminateLaunch and Cleanup launch methods
//
// Revision 1.11.2.2  2007/05/04 14:09:35  gdavidson
// Issue #5641: Close viewer when asynchronous load operation fails
//
// Revision 1.11.2.1  2007/04/27 18:36:44  gdavidson
// Issue #5624: Load data asynchronously
//
// Revision 1.11  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.10  2007/03/05 16:17:30  gdavidson
// Issue #5338: Perform all launch operations on the main thread
//
// Revision 1.9  2007/03/02 22:09:09  gdavidson
// Modified log messages
//
// Revision 1.8  2007/02/21 15:41:42  gdavidson
// Added additional logging for tracking down crashes
//
// Revision 1.7  2007/02/13 18:48:27  gdavidson
// Added log messages in the remoting methods
//
// Revision 1.6  2007/01/23 15:19:36  gdavidson
// Fixed a problem with maintaing window bounds and state when switching applications
//
// Revision 1.5  2007/01/03 22:25:09  gdavidson
// Issue #5261: Added a flag to track when license is not available
//
// Revision 1.4  2006/12/21 16:43:44  gdavidson
// Issue #5251: Fixed user login
//
// Revision 1.3  2006/12/19 15:33:42  gdavidson
// Implemented Login method
//
// Revision 1.2  2006/11/06 14:53:08  mkontak
// Issue 5081
//
// Revision 1.1  2006/10/30 15:41:21  gdavidson
// Moved from Viatronix.Console (IVewerController renamed ILauncher)
//
// Revision 1.14  2006/09/21 20:11:40  mkontak
// Sets the series date to the current date when created
//
// Revision 1.13  2006/07/10 18:39:10  gdavidson
// Modified SearchManager
//
// Revision 1.12  2006/06/06 19:48:00  mkontak
// changed deprecated code
//
// Revision 1.11  2006/05/10 12:50:45  vxconfig
// build fix
//
// Revision 1.10  2006/04/27 19:45:19  mkontak
// Make field non case sensitive
//
// Revision 1.9  2006/04/20 19:46:59  gdavidson
// Modified the IConsole interface
//
// Revision 1.8  2006/04/04 20:22:25  gdavidson
// Modified the timeout interval
//
// Revision 1.7  2006/03/09 21:07:41  gdavidson
// Moved window show methods to WindowsUtilities
//
// Revision 1.6  2006/02/13 16:29:56  gdavidson
// Added checks that the remoting channels have been initialized.
//
// Revision 1.5  2006/01/18 20:29:24  gdavidson
// Added LoginUtilities
//
// Revision 1.4  2006/01/18 16:11:55  gdavidson
// Modified the parameters used in IViewer's Initialize method.
//
// Revision 1.3  2006/01/16 14:27:24  gdavidson
// Added a check for duplicates in the SetupLaunch method
//
// Revision 1.2  2006/01/03 16:06:28  gdavidson
// Removed DataProvider's RetrieveData calls.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/launchers/V3DLauncher.cs,v 1.11.2.7 2008/02/07 21:33:35 mkontak Exp $
// $Id: V3DLauncher.cs,v 1.11.2.7 2008/02/07 21:33:35 mkontak Exp $

#endregion
