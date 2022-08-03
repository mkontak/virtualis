// $Id: MenuFile.cs,v 1.5.2.1 2007/05/25 14:00:57 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using Viatronix.UI;

namespace Viatronix.Console
{
  /// <summary>
  /// Provides the "File" menu.
  /// </summary>
  public class MenuFile : Viatronix.UI.FrameMenu
  {  

    #region fields

    /// <summary>
    /// "Exit" menu item image
    /// </summary>
    private static readonly System.Drawing.Bitmap MENU_IMAGE_EXIT = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.Console.res.toolbar-standard-exit.png" ) );

    /// <summary>
    /// "Exit" menu text 
    /// </summary>
    private static readonly string MENU_TEXT_EXIT = "Exit";

    /// <summary>
    /// "Exit" menu text 
    /// </summary>
    private static readonly string MENU_TEXT_LOCK = "Lock";
    
    /// <summary>
    /// Context menu object
    /// </summary>
    private static Viatronix.UI.ContextMenu m_contextMenu = new Viatronix.UI.ContextMenu ( );

    /// <summary>
    /// Event handler for the menu item click
    /// </summary>
    private static EventHandler m_handler = null;

    /// <summary>
    /// Lock menu item
    /// </summary>
    private static MenuItemEx m_lockMenuItem = null;

    /// <summary>
    /// Exit menu item
    /// </summary>
    private static MenuItemEx m_exitMenuItem = null;


    #endregion

    #region initialization

    /// <summary>
    /// Initializes a MenuFile instance with default values.
    /// </summary>
    public MenuFile()
    {
      this.Text = "File";
      m_handler = new EventHandler ( this.OnMenuItemClickedHandler );

      m_lockMenuItem = new MenuItemEx ( MENU_TEXT_LOCK, m_handler );
      m_exitMenuItem = new MenuItemEx ( MENU_TEXT_EXIT, m_handler, MENU_IMAGE_EXIT );

    } // MenuFile()

    
    #endregion

    #region methods
    
    /// <summary>
    /// Returns a context menu.
    /// </summary>
    /// <returns>context menu</returns>
    public override Viatronix.UI.ContextMenu GetContextMenu()
    {
      m_contextMenu.MenuItems.Clear ( );
      m_contextMenu.MenuItems.Add ( m_lockMenuItem );
      m_contextMenu.MenuItems.Add ( m_exitMenuItem );

      return m_contextMenu;
    } // GetContextMenu()

    
    /// <summary>
    /// Handles the menu item clicked event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnMenuItemClickedHandler( object sender, EventArgs args )
    {
      MenuItemEx item = ( MenuItemEx ) sender;

      if ( item.Text == MENU_TEXT_EXIT )
        MenuFile.Exit ( );

      else if ( item.Text == MENU_TEXT_LOCK )
        Global.Frame.ActivateAutoLogin();
    } // OnMenuItemClickedHandler( sender, args )   
    

    /// <summary>
    /// Exits the application
    /// </summary>
    public static void Exit()
    {
      Global.Frame.Close();
      //Application.Exit();
    } // Exit()
    
    
    #endregion
	
  } // class MenuFile
} // namespace Viatronix.Console

#region revision history

// $Log: MenuFile.cs,v $
// Revision 1.5.2.1  2007/05/25 14:00:57  gdavidson
// Issue #5661: Use the frame's lock method for the lock menu item
//
// Revision 1.5  2007/03/07 22:12:16  gdavidson
// Changed ownership
//
// Revision 1.4  2006/08/17 20:41:51  romy
// Fixed crash among menu and timer
//
// Revision 1.3  2006/02/23 21:05:55  gdavidson
// Added a lock console menu item.
//
// Revision 1.2  2005/11/22 21:46:14  romy
// added License Release via Frame close
//
// Revision 1.1  2005/01/28 16:34:31  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/menus/MenuFile.cs,v 1.5.2.1 2007/05/25 14:00:57 gdavidson Exp $
// $Id: MenuFile.cs,v 1.5.2.1 2007/05/25 14:00:57 gdavidson Exp $

#endregion