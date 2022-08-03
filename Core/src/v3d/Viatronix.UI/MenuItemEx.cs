// $Id: MenuItemEx.cs,v 1.4 2005/11/18 21:08:03 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Drawing;

namespace Viatronix.UI
{
  /// <summary>
  /// Extends the System.Windows.Forms.MenuItem class.
  /// </summary>  
  public class MenuItemEx : System.Windows.Forms.MenuItem
  {

    #region fields

    /// <summary>
    /// menu item image
    /// </summary>
    System.Drawing.Image m_image = null ;

    /// <summary>
    /// shortcut item text
    /// </summary>
    string m_shortcutText = string.Empty;

    /// <summary>
    /// user-specific data
    /// </summary>
    object m_data = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the image.
    /// </summary>
    public System.Drawing.Image Image
    {
      get { return m_image; }
      set { m_image = value; }
    } // Image

    /// <summary>
    /// Gets or sets the shortcut text
    /// </summary>
    public string ShortcutText
    {
      get { return m_shortcutText; }
      set { m_shortcutText = value; }
    } // ShortcutText

    
    /// <summary>
    /// Gets or sets the user-specific data
    /// </summary>
    public object Data
    {
      get { return m_data; }
      set { m_data = value; }
    } // Data

    
    #endregion

    #region events

    /// <summary>
    /// Occurs when a Popup menu is requested.
    /// </summary>
    public new event EventHandler Popup;

    #endregion

    #region initialization

    /// <summary>
    /// Initializes a MenuItemEx instance with default values
    /// </summary>
    public MenuItemEx()
    {
    } // MenuItemEx()

    
    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text.
    /// </summary>
    /// <param name="text">Item text.</param>
    public MenuItemEx( string text )
    {
      this.Text = text;
    } // MenuItemEx( text )

    
    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text and click handler.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="handler">Item click handler.</param>
    public MenuItemEx( string text, EventHandler handler )
    {
      this.Text = text;
      this.Click += handler;
    } // MenuItemEx( text, handler )

    
    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text and click handler.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="shortcutText">Shortcut item text.</param>
    /// <param name="handler">Item click handler.</param>
    public MenuItemEx( string text, string shortcutText, EventHandler handler )
    {
      this.Text = text;
      this.ShortcutText = shortcutText;
      this.Click += handler;
    } // MenuItemEx( text, handler )


    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text, shortcut, click handler, and user-specific data.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="shortcutText">Shortcut item text.</param>
    /// <param name="handler">Item click handler.</param>
    /// <param name="data">User-specific data</param>
    public MenuItemEx( string text, string shortcutText, EventHandler handler, object data )
    {
      this.Text = text;
      this.ShortcutText = shortcutText;
      this.Click += handler;
      this.Data = data;
    } // MenuItemEx( text, handler )


    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text, click handler and user-specific data.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="handler">Item click handler.</param>
    /// <param name="data">User-specific data.</param>
    public MenuItemEx( string text, EventHandler handler, object data )
    {
      this.Text = text;
      this.Click += handler;
      this.Data = data;
    } // MenuItemEx( text, handler, data )


    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text, click handler and image.
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="handler">Item click handler.</param>
    /// <param name="image">Item image.</param>
    public MenuItemEx( string text, EventHandler handler, Image image )
    {
      this.Text = text;
      this.Click += handler;
      this.Image = image;
    } // MenuItemEx( text, handler, image )

    
    /// <summary>
    /// Initializes a MenuItemEx instance with the specified text, click handler, image and user-specific data
    /// </summary>
    /// <param name="text">Item text.</param>
    /// <param name="handler">Item click handler.</param>
    /// <param name="image">Item image.</param>
    /// <param name="data">User-specific data</param>
    public MenuItemEx( string text, EventHandler handler, Image image, object data )
    {
      this.Text = text;
      this.Click += handler;
      this.Image = image;
      this.Data = data;
    } // MenuItemEx( text, handler, image, data )
    
    
    /// <summary>
    /// Raises a Popup event.
    /// </summary>
    public void PerformPopup()
    {
      if( Popup != null )
      {
        Popup( this, EventArgs.Empty );
      }
    } // PerformPopup()
    
    
    #endregion
  
  } // class MenuItemEx
} // namespace Viatronix.UI

#region revision history

// $Log: MenuItemEx.cs,v $
// Revision 1.4  2005/11/18 21:08:03  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/08 17:58:32  frank
// Issue #4331: Fixed selecting colormap and cameras from button
//
// Revision 1.3  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.2  2005/02/28 18:57:56  gdavidson
// Code review
//
// Revision 1.2.2.1  2005/02/11 20:17:31  geconomos
// Added ShortcutText property
//
// Revision 1.2  2004/11/17 13:18:21  geconomos
// Added popup handling.
//
// Revision 1.1  2004/10/11 23:59:15  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/MenuItemEx.cs,v 1.4 2005/11/18 21:08:03 frank Exp $
// $Id: MenuItemEx.cs,v 1.4 2005/11/18 21:08:03 frank Exp $

#endregion
