// $Id: FrameMenu.cs,v 1.2.2.1 2009/01/14 15:01:57 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;

namespace Viatronix.UI
{
  /// <summary>
  /// Represents a menu item that is merged into the viewer's menubar when a plugin tab
  /// is selected.
  /// </summary>
	public abstract class FrameMenu
	{   
    #region fields

    /// <summary>
    /// current xml version
    /// </summary>
    private const string FRAME_MENU_XML_VERSION = "1.0";

    /// <summary>
    /// menu item text
    /// </summary>
    private string m_text = string.Empty;

    #endregion

    #region properties
 

    /// <summary>
    /// Gets or sets the menu item text
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text
    
    #endregion

    #region methods

    /// <summary>
    /// Gets a context menu for the frame
    /// </summary>
    /// <returns>context menu</returns>
    public abstract Viatronix.UI.ContextMenu GetContextMenu();
    
    #endregion
  
    #region serialization

    /// <summary>
    /// Creates a new FrameMenu instance from the specified xml node.
    /// </summary>
    /// <param name="parent">XmlNode containing initialization data</param>
    /// <returns>New FrameMenu instance.</returns>
    public static FrameMenu Create(  XmlNode parent )
    {
    

      // version checking
      string version = parent.Attributes[ "version" ].Value;
      if( version != FRAME_MENU_XML_VERSION )
      {
        throw new ArgumentException( "Invalid xml version for class FrameMenu." );
      }

      FrameMenu menu = ( FrameMenu )System.Activator.CreateInstance( System.Type.GetType(  parent.Attributes[ "typename" ].Value ) );

      menu.Text = parent.Attributes[ "text" ].Value;

      return menu;
    } // Create( XmlNode parent )

    
    #endregion
  } // class FrameMenu
} // Viatronix.Viewer.Plugins

#region revision history

// $Log: FrameMenu.cs,v $
// Revision 1.2.2.1  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.2  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.1  2005/01/12 17:10:08  geconomos
// moved from viatronix.v3d.viewer.plugins
//
// Revision 1.2  2004/10/12 00:07:03  geconomos
// Updated for changes to IPluginMenu
//
// Revision 1.1  2004/09/15 17:49:38  geconomos
// moved files
//
// Revision 1.1  2004/07/09 18:47:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/FrameMenu.cs,v 1.2.2.1 2009/01/14 15:01:57 kchalupa Exp $
// $Id: FrameMenu.cs,v 1.2.2.1 2009/01/14 15:01:57 kchalupa Exp $

#endregion