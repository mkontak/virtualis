// $Id: V3DReportLauncher.cs,v 1.4 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.Configuration;
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
	/// Implements all functionality necessary to launch a v3D viewer's report.
	/// </summary>
	public class V3DReportLauncher : V3DLauncher
	{

    #region fields

    /// <summary>
    /// The index of the report page.
    /// </summary>
    private int m_pageIndex = 0;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a V3DReportLauncher
    /// </summary>
    /// <param name="launcher">parent IViewerLauncher</param>
    public V3DReportLauncher( IViewerLauncher launcher )
    {
      this.ID = launcher.ID;
      this.Text = launcher.Text + " Report";
      this.Description = launcher.Description;
      this.Executable = launcher.Executable;
      this.Datasets = launcher.Datasets;
      this.IsSession = true;
    } // V3DReportLauncher( launcher )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the page index of the report.
    /// </summary>
    public int PageIndex
    {
      get { return m_pageIndex; }
      set { m_pageIndex = value; }
    } // PageIndex

    #endregion

    #region override methods

    /// <summary>
    /// Launches the viewer.
    /// </summary>
    public override void LaunchViewer()
    {
      if( this.LauncherArgs.User.IsAllowed( "OpenReport" ))
        base.LaunchViewer();
    } // LaunchViewer()


    /// <summary>
    /// Groups the series into series collections based on datasets.
    /// </summary>
    protected override void OnInitializeLaunch()
    {
      Series session = null;


      if ( this.Datasets[0].TryFind("session", ref session ) )
      {
        // TODO: New
        //this.DataProvider.RetrieveData( session, false);
      }
    } // SetupLaunch()


 
    #endregion

  } // V3DReportLauncher
} // namespace Viatronix.Console

#region revision history

// $Log: V3DReportLauncher.cs,v $
// Revision 1.4  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.3  2007/02/21 20:57:44  gdavidson
// Commented code
//
// Revision 1.2  2006/11/15 22:04:46  gdavidson
// Issue #5164: Set the loaded datasets
//
// Revision 1.1  2006/10/30 15:41:21  gdavidson
// Moved from Viatronix.Console (IVewerController renamed ILauncher)
//
// Revision 1.3  2006/07/24 21:57:56  gdavidson
// Issue #4878: Added permission checks
//
// Revision 1.2  2006/01/03 15:58:29  gdavidson
// Added information to the log messages in the StartViewer method.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/launchers/V3DReportLauncher.cs,v 1.4 2007/03/06 19:14:59 gdavidson Exp $
// $Id: V3DReportLauncher.cs,v 1.4 2007/03/06 19:14:59 gdavidson Exp $

#endregion
