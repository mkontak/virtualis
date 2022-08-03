// $Id: SeriesDetailsPanel.cs,v 1.8.2.1 2008/09/08 18:24:23 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Configuration;
using System.Collections;
using System.Windows.Forms;
using System.Xml;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
	/// <summary>
	/// Parent panel control used to display series details.
	/// </summary>
	public class SeriesDetailsPanel : Panel
	{

    #region fields

    /// <summary>
    /// A collection of ISeriesDetails.
    /// </summary>
    //private static Hashtable m_viewerCache = new Hashtable();

    /// <summary>
    /// Reference to the currently displayed ISeriesDetails.
    /// </summary>
    private SessionViewer m_sessionViewer = null;
    
    /// <summary>
    /// Reference to the launch manager object.
    /// </summary>
    private LaunchManager m_launchManager = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the LaunchManager
    /// </summary>
    public LaunchManager Launcher
    {
      get { return m_launchManager; }
      set { m_launchManager = value; }
    } // Launcher

    #endregion

    #region methods

    /// <summary>
    /// Displays the content of the provided series.
    /// </summary>
    /// <param name="series">Series</param>
    public void Display( Series series )
    {
      this.Controls.Clear();

      // Dispose of the previous session viewer.
      if( m_sessionViewer != null )
      {
        m_sessionViewer.Dispose();
      } // if(m_sessionViewer != null)
      
      // find the associated ISeriesDetails
      m_sessionViewer = CreateSessionViewer( series );
      if( m_sessionViewer == null )
        this.Visible = false;
      else
      {
        this.Visible = true;
        this.Controls.Add( m_sessionViewer );
      }
    } // Display( series )


    /// <summary>
    /// Returns the SeesionViewer object assocaited with the provided series.
    /// </summary>
    /// <param name="series">Series</param>
    /// <returns>ISeriesDetails or null</returns>
    private SessionViewer CreateSessionViewer( Series series )
    {
      if (series.Type == SeriesTypes.Session)
      {
        return new DefaultSessionViewer() { Series = series };
      }
      else
      {
        return null;
      }
    } // CreateDetails( series )

    #endregion

  } // class SeriesDetailsPanel
} // namespace Viatronix.Console

#region revision history

// $Log: SeriesDetailsPanel.cs,v $
// Revision 1.8.2.1  2008/09/08 18:24:23  kchalupa
// Fixes For:
// Issue 6138 Parameters column on the Console does not always display the parameters.  Sometims the parameters column is blank.
// Issue 6137 Multiple instances of Consoles can be opened on the same computer.
// Issue 6133 Check the proper share permissions.
// Issue 6081 Temp Directories are not being cleaned up after use.
//
//
// Revision 1.9  2008/09/08 10:05:32  kchalupa
// Called dispose on m_sessionViewer before creating new one.
// Created dispose method.
//
// Revision 1.8  2007/03/06 19:00:44  gdavidson
// Added class comments
//
// Revision 1.7  2006/07/11 18:13:57  gdavidson
// Reworked SessionViewer
//
// Revision 1.6  2006/06/06 19:47:39  mkontak
// changed deprecated code
//
// Revision 1.5  2006/01/03 16:14:03  gdavidson
// Modified how ISeriesDetails objects are displayed.
//
// Revision 1.4  2005/12/14 19:58:32  gdavidson
// Load the ISeriesDetails asynchronously.
//
// Revision 1.3  2005/11/02 14:24:38  gdavidson
// Don't display ISessionDetails if an error ocurrs.
//
// Revision 1.2  2005/10/27 15:18:20  gdavidson
// Added a check for a series's type of session.
//
// Revision 1.1  2005/10/26 19:52:32  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/Browser Plugins/SeriesDetailsPanel.cs,v 1.8.2.1 2008/09/08 18:24:23 kchalupa Exp $
// $Id: SeriesDetailsPanel.cs,v 1.8.2.1 2008/09/08 18:24:23 kchalupa Exp $

#endregion
