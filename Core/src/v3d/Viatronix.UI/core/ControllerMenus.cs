// $Id: ControllerMenus.cs,v 1.1 2005/01/26 19:07:05 geconomos Exp $
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

namespace Viatronix.UI
{
  /// <summary>
  /// Responsible for maintaining the frame's top-level menus
  /// </summary>
	public class ControllerMenus : Viatronix.UI.MainMenu2
	{
    #region fields

    /// <summary>
    /// event handler for popup event for the top level menu item
    /// </summary>
    private EventHandler m_menuHandler = null;

    #endregion
		
    #region  methods

    /// <summary>
    /// Initializes a ControllerMenus instance with default values.
    /// </summary>
    public ControllerMenus()
		{
      m_menuHandler = new EventHandler( this.OnMenuHandler );
    } // ControllerMenus()

 
    /// <summary>
    /// Adds the specified menu to the controller.
    /// </summary>
    /// <param name="menu">FrameMenu instance.</param>
    public void AddMenu( FrameMenu menu )
    {
      // create the menu item associating the FrameMenu instance in the data field
      Viatronix.UI.MenuItemEx item = new Viatronix.UI.MenuItemEx();
      item.Text = menu.Text;
      item.Data = menu;      
      
      // we will add the menu items when the menu item is clicked
      item.Popup += m_menuHandler;
      
      // add a dummy item so that the popup event gets raised
      item.MenuItems.Add( "" );
      
      this.MenuItems.Add( item );
    } // AddMenu( FrameMenu menu )

    /// <summary>
    /// Resets the controller for the specified template.
    /// </summary>
    /// <param name="template">ViewTemplate instance</param>
    public void Reset( TemplateView template )
    {
      this.MenuItems.Clear();
      foreach( FrameMenu menu in template.Menus )
        AddMenu( menu );
      Refresh();
    } // Reset( ViewTemplate template )

    
    /// <summary>
    /// Called before a top level menu item is displayed.
    /// </summary>
    /// <param name="sender">Sender of this event</param>
    /// <param name="args">EventArgs containing data related to this event</param>
    private void OnMenuHandler( object sender, EventArgs args )
    {
      // the sender is the menu item that is to be shown
      Viatronix.UI.MenuItemEx item = ( Viatronix.UI.MenuItemEx ) sender;

      // the item's data contains an instance to the actual frame menu to be displayed
      FrameMenu menu = ( FrameMenu ) item.Data;

      // get the context and menu
      Viatronix.UI.ContextMenu context = menu.GetContextMenu();
      
      // enable owner drawing
      context.SetOwnerDraw();
      
      // clear out
      item.MenuItems.Clear();

      // create an array containg references to the menu (a little foobar)
      Viatronix.UI.MenuItemEx[] items = new Viatronix.UI.MenuItemEx[ context.MenuItems.Count ];
      for( int index = 0; index < items.Length; ++index )
        items[ index ] = (Viatronix.UI.MenuItemEx)context.MenuItems[ index ];
      
      // add menu items to the top level item
      item.MenuItems.AddRange( items );
    
    } // OnMenuHandler( object sender, EventArgs args )

    
    #endregion

  } // class ControllerMenus
} // namespace Viatronix.Viewer

#region revision history

// $Log: ControllerMenus.cs,v $
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.8  2004/12/10 16:14:43  geconomos
// temporarily removed "view" menu
//
// Revision 1.7  2004/11/17 22:39:17  geconomos
// initial implmentation for system menu
//
// Revision 1.6  2004/11/17 13:27:59  geconomos
// working on maximize / restore ability
//
// Revision 1.5  2004/11/16 13:49:27  geconomos
// creating new menus
//
// Revision 1.4  2004/11/05 16:31:44  michael
// checking whether the image has a valid size (> 0)
//
// Revision 1.3  2004/11/01 15:20:14  geconomos
// updated to reflect new Global events
//
// Revision 1.2  2004/10/18 02:41:21  geconomos
// cleaned up controller code
//
// Revision 1.1  2004/10/12 00:25:25  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/ControllerMenus.cs,v 1.1 2005/01/26 19:07:05 geconomos Exp $
// $Id: ControllerMenus.cs,v 1.1 2005/01/26 19:07:05 geconomos Exp $

#endregion