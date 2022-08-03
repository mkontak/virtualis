// $Id: EnterpriseClientLauncher.cs,v 1.8 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using Viatronix.v3D.Provider;
using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{

  /// <summary>
  /// Implements a launcher for enterprise systems.
  /// </summary>
  public class EnterpriseClientLauncher : Launcher
  {

    #region fields

    /// <summary>
    /// client settings
    /// </summary>
    private EnterpriseClientSettings m_settings = null;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public EnterpriseClientLauncher()
    {
      this.CanNavigate = false;
    } // V3DLauncherClient()

    #endregion

    #region override methods

    /// <summary>
    /// initializes the launch
    /// </summary>
    protected override void OnInitializeLaunch()
    {
      m_settings = new EnterpriseClientSettings( Guid.NewGuid(), this.LauncherArgs.User, this.ID, this.IsSession, this.Datasets ); 
      Database.AddEnterpriseClientSettings( Database.GetConnectionString("storage"), m_settings );
    } // OnInitializeLaunch()


    /// <summary>
    /// Creates a viewer process
    /// </summary>
    /// <returns>viewer's process</returns>
    protected override Process OnCreateViewerProcess()
    {
      try
      {
        // creates the process which executes the viewer
        ProcessStartInfo info = new ProcessStartInfo( this.Executable, this.CommandLine + " -r -launchID " + m_settings.LaunchID.ToString() );
        info.WorkingDirectory = Path.GetDirectoryName( this.Executable );
        info.ErrorDialog = false;
        info.UseShellExecute = true;
        info.WindowStyle = ProcessWindowStyle.Hidden;

        Process process = Process.Start( info );
        return process;
      }
      catch( Exception e )
      {
        throw new LaunchException( LaunchStage.StartProcess, "Unable to start the viewer's process. [PLUGIN=" + this.ID + "] [ERROR=" + e.Message + "]", e );
      }
    } // OnCreateViewerProcess()


    /// <summary>
    /// Displays the form
    /// </summary>
    /// <param name="bounds">Windows bounds</param>
    /// <param name="state">Windows state</param>
    protected override void OnShowForm( System.Drawing.Rectangle bounds, System.Windows.Forms.FormWindowState state )
    {
      this.Form.Show();
    } // OnShowForm( bounds, state )


    /// <summary>
    /// Shows the viewer
    /// </summary>
    protected override void OnShowViewer()
    {
      this.Form.Hide();
    } // OnShowViewer()


    /// <summary>
    /// Starts the viewer
    /// </summary>
    protected override void OnStartViewer()
    {
    }


    /// <summary>
    /// Closes the viewer
    /// </summary>
    protected override void OnCloseViewer()
    {
    }


    /// <summary>
    /// Cleans up resources
    /// </summary>
    protected override void OnCleanupLaunch()
    {
      base.OnCleanupLaunch();

      // delete data from database
      Database.DeleteEnterpriseClientSettings( Database.GetConnectionString("storage"), m_settings );
    } // OnCleanupLaunch()


    /// <summary>
    /// Handles the viewer closed.
    /// </summary>
    protected override void OnViewerClosed()
    {
      base.OnViewerClosed();

      // delete data from database
      Database.DeleteEnterpriseClientSettings( Database.GetConnectionString("storage"), m_settings );
    } // OnViewerClosed()

    #endregion

  } // class EnterpriseClientLauncher
} // namespace Viatronix.v3D.Console


#region revision history

// $Log: EnterpriseClientLauncher.cs,v $
// Revision 1.8  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.7  2007/03/05 16:17:30  gdavidson
// Issue #5338: Perform all launch operations on the main thread
//
// Revision 1.6  2007/02/21 20:55:58  gdavidson
// Commented code
//
// Revision 1.5  2007/01/23 15:19:36  gdavidson
// Fixed a problem with maintaing window bounds and state when switching applications
//
// Revision 1.4  2006/11/11 01:34:35  gdavidson
// Modified the OnCreateViewerProcess method
//
// Revision 1.3  2006/11/02 22:37:42  gdavidson
// Execute database cleanup when viewer closes
//
// Revision 1.2  2006/10/30 17:17:04  gdavidson
// Added EnterpriseClientSettings to Viatronix.v3D.Core
//
// Revision 1.1  2006/10/30 15:41:34  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/launching/launchers/EnterpriseClientLauncher.cs,v 1.8 2007/03/06 19:14:59 gdavidson Exp $
// $Id: EnterpriseClientLauncher.cs,v 1.8 2007/03/06 19:14:59 gdavidson Exp $

#endregion
