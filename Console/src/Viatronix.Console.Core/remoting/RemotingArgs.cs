// $Id: RemotingArgs.cs,v 1.6 2007/03/12 17:46:31 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using Viatronix.Enterprise.Entities;


namespace Viatronix.v3D.Core
{
	/// <summary>
	/// Implements the arguements to be used in our remoting
	/// </summary>
	public class RemotingArgs : MarshalByRefObject
	{

    #region fields

    /// <summary>
    /// Console process id.
    /// </summary>
    private int m_processID = -1;

    /// <summary>
    /// Reference to an IConsole
    /// </summary>
    private IConsole m_console = null;

    /// <summary>
    /// plugin id
    /// </summary>
    private string m_pluginID = string.Empty;

    /// <summary>
    /// Reference to the current user.
    /// </summary>
    private User m_user = null;

    /// <summary>
    /// Amount of idle time before the application auto locks.
    /// </summary>
    private int m_idleInterval;

    /// <summary>
    /// The site id
    /// </summary>
    private string m_siteID = string.Empty;

    /// <summary>
    /// determines if the viewer can make console visible
    /// </summary>
    private bool m_canShowConsole = true;

    #endregion

    #region construction
		
    /// <summary>
    /// Creates a new instance of a RemotingArgs object.
    /// </summary>
    /// <param name="processID">Console process id</param>
    /// <param name="console">The IConsole object.</param>
    /// <param name="user">Logged in user.</param>
    /// <param name="pluginID">The pluginID of the viewer.</param>
    /// <param name="intervalMinutes">The idle interval</param>
    /// <param name="siteID">The site id</param>
    public RemotingArgs( int processID, IConsole console, User user, string pluginID, int intervalMinutes, string siteID ) 
      : this( processID, console, user, pluginID, intervalMinutes, siteID, true )
		{
		} // RemotingArgs( processID, console, user, pluginID, intervalMinutes, siteID )


    /// <summary>
    /// Creates a new instance of a RemotingArgs object.
    /// </summary>
    /// <param name="processID">Console process id</param>
    /// <param name="console">The IConsole object.</param>
    /// <param name="user">Logged in user.</param>
    /// <param name="pluginID">The pluginID of the viewer.</param>
    /// <param name="intervalMinutes">The idle interval</param>
    /// <param name="siteID">The site id</param>
    public RemotingArgs( int processID, IConsole console, User user, string pluginID, int intervalMinutes, string siteID, bool canShowConsole )
		{
      m_processID = processID;
			m_console = console;
      m_user = user;
      m_pluginID = pluginID;
      m_idleInterval = intervalMinutes;
      m_siteID = siteID;
      m_canShowConsole = canShowConsole;
		} // RemotingArgs( processID, console, user, pluginID, intervalMinutes, siteID )


    /// <summary>
    /// Default constructor
    /// </summary>
    public RemotingArgs()
    {
    }

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the process id
    /// </summary>
    public int ProcessID
    {
      get { return m_processID; }
      set { m_processID = value; }
    } // ProcessID


    /// <summary>
    /// Gets or sets the IConsole.
    /// </summary>
    public IConsole Console
    {
      get { return m_console; }
      set { m_console = value; }
    } // Console


    /// <summary>
    /// Gets or sets the User.
    /// </summary>
    public User User
    {
      get { return m_user; }
      set { m_user = value; }
    } // User


    /// <summary>
    /// Gets or sets the plugin id.
    /// </summary>
    public string PluginID
    {
      get { return m_pluginID; }
      set { m_pluginID = value; }
    } // PluginID


    /// <summary>
    /// Gets or sets the idle time interval.
    /// </summary>
    public int IdleInterval
    {
      get { return m_idleInterval; }
      set { m_idleInterval = value; }
    } // IdleInterval


    /// <summary>
    /// Gets or sets the site id.
    /// </summary>
    public string SiteID
    {
      get { return m_siteID; }
      set { m_siteID = value; }
    } // SiteID


    /// <summary>
    /// Gets or sets if console can be shown by the viewer
    /// </summary>
    public bool CanShowConsole
    {
      get { return m_canShowConsole; }
      set { m_canShowConsole = value; }
    } // CanShowConsole

    #endregion

	} // class RemotingArgs
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: RemotingArgs.cs,v $
// Revision 1.6  2007/03/12 17:46:31  mkontak
// coding standards
//
// Revision 1.5  2006/10/30 16:05:30  gdavidson
// Added a property that determines if the viewer can navigate back to the console
//
// Revision 1.4  2006/05/09 20:18:01  gdavidson
// Added site id property
//
// Revision 1.3  2006/03/09 21:22:20  gdavidson
// Removed ProcessID
//
// Revision 1.2  2006/01/24 19:53:34  mkontak
// Chnage the name in the constructor from interval to intervalMinutes
//
// Revision 1.1  2006/01/18 16:05:03  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/remoting/RemotingArgs.cs,v 1.6 2007/03/12 17:46:31 mkontak Exp $
// $Id: RemotingArgs.cs,v 1.6 2007/03/12 17:46:31 mkontak Exp $

#endregion