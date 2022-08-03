// $Id: IViewerLauncher.cs,v 1.4.2.1 2011/05/18 01:57:00 mkontak Exp $
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
using System.Drawing;
using System.Windows.Forms;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
	/// <summary>
	/// Defines methods that launch a viewer.
	/// </summary>
	public interface IViewerLauncher : IComparable< IViewerLauncher >
	{
		  
    /// <summary>
    /// Closed event
    /// </summary>
    event EventHandler Closed;


    /// <summary>
    /// Updated event.
    /// </summary>
    event EventHandler Updated;
    

    /// <summary>
    /// Gets or sets the plugin id for the viewer.
    /// </summary>
    string ID
    {
      get;
      set;
    } // ID
    

    /// <summary>
    /// Gets or sets the display text.
    /// </summary>
    string Text
    {
      get;
      set;
    } // Text


    /// <summary>
    /// Gets or sets the logo.
    /// </summary>
    Image Logo
    {
      get;
      set;
    } // Logo


    /// <summary>
    /// Gets or sets the associated viewer's description.
    /// </summary>
    string Description
    {
      get;
      set;
    } // Description


    /// <summary>
    /// Gets or sets the full path to the viewer's executable.
    /// </summary>
    string Executable
    {
      get;
      set;
    } // Executable


    /// <summary>
    /// Gets or sets the command line arguments
    /// </summary>
    string CommandLine
    {
      get;
      set;
    } // CommandLine


    /// <summary>
    /// Gets or sets if the viewer is launching an existing session.
    /// </summary>
    bool IsSession
    {
      get;
      set;
    } // HasExistingSession


    /// <summary>
    /// Gets or sets if the console can navigate to the viewer
    /// </summary>
    bool CanNavigate
    {
      get;
      set;
    } // CanNavigate


    /// <summary>
    /// Additional information stored in an xml
    /// </summary>
    string AdditionalInfo
    {
      get;
      set;
    } // AdditionalInfo


    /// <summary>
    /// Returns the series being launched.
    /// </summary>
    List<List<Series>> Datasets 
    { 
      get; 
      set;
    } // Datasets


    /// <summary>
    /// Returns the Viewer's process
    /// </summary>
    System.Diagnostics.Process ViewerProcess
    {
      get;
      set;
    } // ViewerProcess


    /// <summary>
    /// Initializes the IViewerController.
    /// </summary>
    /// <param name="collection">A collection of series being launched.</param>
    void Initialize( LauncherArgs collection, User user );


    /// <summary>
    /// Launches the viewer.
    /// </summary>
    void LaunchViewer();



	} // interface IViewerController
} // namespace Viatronix.Console

#region revision history

// $Log: IViewerLauncher.cs,v $
// Revision 1.4.2.1  2011/05/18 01:57:00  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.3  2006/12/18 16:10:30  gdavidson
// Added functionality to intialize with additional xml data
//
// Revision 1.2  2006/11/02 22:41:09  gdavidson
// Added configurable command line arguments
//
// Revision 1.1  2006/10/30 15:35:22  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.3  2006/03/09 21:06:20  gdavidson
// Removed ViewerBounds property
//
// Revision 1.2  2006/01/09 19:48:46  gdavidson
// Added Logo property.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/interfaces/IViewerLauncher.cs,v 1.4.2.1 2011/05/18 01:57:00 mkontak Exp $
// $Id: IViewerLauncher.cs,v 1.4.2.1 2011/05/18 01:57:00 mkontak Exp $

#endregion
