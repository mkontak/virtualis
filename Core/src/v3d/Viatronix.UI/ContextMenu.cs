// $Id: ContextMenu.cs,v 1.5 2005/03/08 19:11:06 frank Exp $
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
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;

namespace Viatronix.UI
{

  #region ContextMenuAlign

  /// <summary>
  /// the alignment of the context menu
  /// </summary>
  [ Flags ]
  public enum ContextMenuAlignment
  {
    Left            = 0x0000,
    Center          = 0x0004,
    Right           = 0x0008,
    Top             = 0x0000,
    VerticalCenter  = 0x0010,
    Bottom          = 0x0020,
  } // enum ContextMenuAlignment

  #endregion

  /// <summary>
  /// Represents an "owner-drawn" context menu.
  /// </summary>
  public class ContextMenu : System.Windows.Forms.ContextMenu
	{

    #region fields
    
    /// <summary>
    /// draw item event handler
    /// </summary>
    private DrawItemEventHandler m_drawItemHandler;
    
    /// <summary>
    /// measure item draw handler
    /// </summary>
    private MeasureItemEventHandler m_measureItemHandler;

    /// <summary>
    /// indicates if we owner drawing is enabled
    /// </summary>
    private bool m_ownerDraw = true;

    /// <summary>
    /// offset from left side of menu where text is displayed
    /// </summary>
    private static readonly int HORIZONTAL_TEXT_OFFSET = 22;

    /// <summary>
    /// padding to add to the right side of the menu item
    /// </summary>
    private static readonly int RIGHT_PADDING = 5;

    /// <summary>
    /// padding to add to the height of the menu item
    /// </summary>
    private static readonly int HEIGHT_PADDING = 6;

    /// <summary>
    /// size of the checkmark glyph
    /// </summary>
    private static readonly Size CHECKMARK_SIZE = new Size( 5, 5 );

    #endregion

    #region initialization

    /// <summary>
    /// Initializer
    /// </summary>
    public ContextMenu()
		{
      m_drawItemHandler = new DrawItemEventHandler( this.OnDrawItemHandler );

      m_measureItemHandler = new MeasureItemEventHandler( this.OnMeasureItemHandler );
		} // ContextMenu()

    #endregion

    #region methods
    
    /// <summary>
    /// Shows the context menu at the specified position.
    /// </summary>
    /// <param name="control">Owner of context menu</param>
    /// <param name="pos">Position to display context menu.</param>
    public new void Show( Control control, Point pos ) 
    {
      Show( control, pos, ContextMenuAlignment.Top | ContextMenuAlignment.Left );
    } // Show( Control control, Point pos )   

  
    /// <summary>
    /// Shows the context menu at the specified position.
    /// </summary>
    /// <param name="control">Owner of context menu</param>
    /// <param name="pos">Position to display context menu.</param>
    /// <param name="alignment">Alignment relative to the mouse position.</param>
    public void Show( Control control, Point pos, ContextMenuAlignment alignment ) 
    {
      // make sure menu is created
      if( this.Handle == IntPtr.Zero )
        return;

      // raise the "Popup" event
      OnPopup( EventArgs.Empty );

      // enable owner drawing for all the menu items
      if( m_ownerDraw )
        SetOwnerDraw();

      // convert the supplied position to screen coordinates
      pos = control.PointToScreen( pos );
      
      // allow the context menu to respong to the right mouse menu
      int flags = ( int ) ( Win32.TPM.TPM_RIGHTBUTTON );

      // and the aligment flags
      flags |= ( int ) alignment;

      // show the context menu
      Win32.User32.TrackPopupMenu( this.Handle, flags, pos.X, pos.Y, 0, control.Handle, IntPtr.Zero );
    
    } // Show( Control control, Point pos, ContextMenuAlignment alignment ) 

    
    /// <summary>
    /// Enables owner-drawing for all the menu items assigned to the context menu.
    /// </summary>
    public void SetOwnerDraw()
    {
      SetOwnerDraw( this.MenuItems );
    } // SetOwnerDraw()
    
    
    /// <summary>
    /// Enables owner-drawing for all the menu items in the specified collection,
    /// </summary>
    /// <param name="items">MenuItem collection</param>
    public void SetOwnerDraw( Menu.MenuItemCollection items )
    {
      foreach( MenuItem item in items )
      {
        // is the item a separator?
        if( item.Text != "-" )
        {
          // enable owner-drawing for the item
          item.OwnerDraw = true;
          item.DrawItem += m_drawItemHandler;
          item.MeasureItem += m_measureItemHandler;

          // pass along any of the item's children
          SetOwnerDraw( item.MenuItems );
        }
      } // foreach( MenuItem item in items )
    } // SetOwnerDraw( Menu.MenuItemCollection items )

    
    /// <summary>
    /// DrawItem handler.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">DrawItemEventArgs containg data related to this event.</param>
    private void OnDrawItemHandler( object sender, DrawItemEventArgs args )
    { 
      // convert the sender to the MenuItem that sent event
      MenuItem item = ( MenuItem ) sender;
      
      DrawBackground( args.Graphics, item, args.Bounds, args.State );
   
      if( item.Checked )
      {
        DrawCheckMark( args.Graphics, item, args.Bounds );
      }
      else
      {
        DrawImage( args.Graphics, item, args.Bounds );
      }

      DrawText( args.Graphics, item, args.Bounds );

    } // OnDrawItemHandler( object sender, DrawItemEventArgs args )


    /// <summary>
    /// Draws the specified menu item bvackground.
    /// </summary>
    /// <param name="gfx">Graphics instance</param>
    /// <param name="item">MenuItem instance</param>
    /// <param name="bounds">MenuItem bounds</param>
    /// <param name="state">Draw item state</param>
    private void DrawBackground( Graphics gfx, MenuItem item, Rectangle bounds, DrawItemState state  )
    {
      // fill in the item's bounds with the systems' back color for a menu
      using( SolidBrush brush = new SolidBrush( SystemColors.Menu ) )
        gfx.FillRectangle( brush, bounds );
      
      // is the item selected.
      if( ( state & DrawItemState.Selected ) == DrawItemState.Selected )
      {
        // adjust the item's bounds for drawing the selection frame
        bounds.Width -= 1;
        bounds.Height -= 1;

        gfx.FillRectangle( Theme.SelectedBrush, bounds );
        gfx.DrawRectangle( Theme.SelectedPen, bounds );
      }
    } // DrawBackground(  Graphics gfx, MenuItem item, Rectangle bounds )
    
    
    /// <summary>
    /// Draws the specified menu item image.
    /// </summary>
    /// <param name="gfx">Graphics instance</param>
    /// <param name="item">MenuItem instance</param>
    /// <param name="bounds">MenuItem bounds</param>
    private void DrawImage( Graphics gfx, MenuItem item, Rectangle bounds )
    {
      if( item is MenuItemEx )
      {
        MenuItemEx itemex = ( MenuItemEx ) item;
        if( itemex.Image != null )
          gfx.DrawImage( itemex.Image, bounds.X + 2, bounds.Y + 2 );
      }
    } // DrawImage( Graphics gfx, MenuItem item, Rectangle bounds )
    
    
    /// <summary>
    /// Draws the specified menu item text.
    /// </summary>
    /// <param name="gfx">Graphics instance</param>
    /// <param name="item">MenuItem instance</param>
    /// <param name="bounds">MenuItem bounds</param>
    private void DrawText( Graphics gfx, MenuItem item, Rectangle bounds )
    {
      SolidBrush brush = item.Enabled? Theme.ContextMenuTextBrush : Theme.ContextMenuTextDisabledBrush;
      
      StringFormat format = new StringFormat();      
      format.LineAlignment = StringAlignment.Center;
      
      // layout for the item's text
      PointF point = new PointF();
      point.X = bounds.X + HORIZONTAL_TEXT_OFFSET + 2;
      point.Y = bounds.Y + ( bounds.Height / 2.0F );

      gfx.DrawString( item.Text, Theme.ContextMenuFont, brush, point, format );
    
      // is this an extended menu item?
      if( item is MenuItemEx )
      {
        MenuItemEx itemex = ( MenuItemEx ) item;
        if(  itemex.ShortcutText != string.Empty )
        {
          point.X = ( bounds.X + bounds.Width ) - (2*RIGHT_PADDING);
          format.Alignment = StringAlignment.Far;
          gfx.DrawString( itemex.ShortcutText, Theme.ContextMenuFont, brush, point, format );
        }
      }
    } // DrawText( Graphics gfx, MenuItem item, Rectangle bounds )
        
  
    /// <summary>
    /// Draws the checkmark menu glyph.
    /// </summary>
    /// <param name="gfx">Graphics object to render to</param>
    /// <param name="itemBounds">Bounds to draw checkmark into</param>
    /// <param name="grayed">Indicates if the checkmark is grayed out</param>
    private void DrawCheckMark( Graphics gfx, MenuItem item, Rectangle bounds )
    {
      bounds.Width = HORIZONTAL_TEXT_OFFSET;
      bounds.Height -= 1;
      bounds.Inflate( -2, -2 );
        
      // create bounding rectangle for checkbox
      Rectangle checkBounds = new Rectangle();
      checkBounds.X = bounds.X + ( int )( ( ( bounds.Width  - CHECKMARK_SIZE.Width  ) / 2.0F ) + 0.5F );
      checkBounds.Y = bounds.Y + ( int )( ( ( bounds.Height - CHECKMARK_SIZE.Height ) / 2.0F ) + 0.5F );
      checkBounds.Size = CHECKMARK_SIZE;
      
      // calculate checkmark points
      PointF[ ] points = new PointF[ 3 ];      
      points[ 0 ].X = checkBounds.Left + ( ( int )( checkBounds.Width  / 4.0F ) + 0.5F );
      points[ 0 ].Y = checkBounds.Top  + ( ( int )( checkBounds.Height / 4.0F ) + 0.5F );
      points[ 1 ].X = checkBounds.Left + ( ( int )( checkBounds.Width  / 3.0F ) + 0.5F );
      points[ 1 ].Y = checkBounds.Bottom;
      points[ 2 ].X = checkBounds.Right;
      points[ 2 ].Y = checkBounds.Top;
    
      // connect the points
      using( Pen pen = new Pen( item.Enabled ?  Color.Black : Color.Gray) )
      {
        gfx.DrawLine( pen, points[ 0 ], points[ 1 ] );
        gfx.DrawLine( pen, points[ 1 ], points[ 2 ] );
      }
    } // DrawCheckMark( gfx, bounds )

 
    /// <summary>
    /// Handler for the MeasureItem event. Returns the required size for the specified menu item.
    /// </summary>
    /// <param name="sender">Sender of this even</param>
    /// <param name="args"></param>
    private void OnMeasureItemHandler( object sender, MeasureItemEventArgs args )
    {
      MenuItem item = ( MenuItem ) sender;
      
      // mesaure the item's text
      SizeF size = args.Graphics.MeasureString( item.Text, Theme.ContextMenuFont );
      
      // set the required width for the item adding all padding
      args.ItemWidth = HORIZONTAL_TEXT_OFFSET + ( int )( size.Width ) + RIGHT_PADDING;

      // set the required height for the item
      args.ItemHeight = ( int )( size.Height + 0.5F );

      // is this an extended menu item?
      if( item is MenuItemEx )
      {
        MenuItemEx itemex = ( MenuItemEx ) item;

        if( itemex.ShortcutText != string.Empty )
        {
          SizeF shortcutSize = args.Graphics.MeasureString( itemex.ShortcutText, Theme.ContextMenuFont );
          args.ItemWidth += ( int )( shortcutSize.Width ) + RIGHT_PADDING;
        }

        // does the item have an associated image?
        if( itemex.Image != null )
        {
          args.ItemHeight = Math.Max( args.ItemHeight, itemex.Image.Height );
          args.ItemHeight += 4;
        }
      }

      args.ItemHeight += HEIGHT_PADDING;

    } // OnMeasureItemHandler( object sender, MeasureItemEventArgs args )

    #endregion

  } // class ContextMenu
} // namespace Viatronix.UI

#region revision history

// $Log: ContextMenu.cs,v $
// Revision 1.5  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.4.2.3  2005/03/01 21:18:13  frank
// code review
//
// Revision 1.4.2.2  2005/02/11 20:15:38  geconomos
// reworked drawing code
//
// Revision 1.4.2.1  2005/02/10 21:22:57  geconomos
// Issue #3932: Ability to disable load session if data does not exist
//
// Revision 1.4  2004/11/17 13:18:03  geconomos
// Adjusted the drawing of the menu items
//
// Revision 1.3  2004/10/18 02:15:43  geconomos
// reduced the vertical padding for a menu item
//
// Revision 1.2  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.1  2004/10/11 23:59:15  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ContextMenu.cs,v 1.5 2005/03/08 19:11:06 frank Exp $
// $Id: ContextMenu.cs,v 1.5 2005/03/08 19:11:06 frank Exp $

#endregion
