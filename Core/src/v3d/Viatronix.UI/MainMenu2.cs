// $Id: MainMenu2.cs,v 1.3 2005/03/08 19:11:06 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;

namespace Viatronix.UI
{
	public class MainMenu2 : System.Windows.Forms.Control
	{

    #region fields

    /// <summary>
    /// the item spacing
    /// </summary>
    private const int ITEM_SPACING = 20;

    /// <summary>
    /// the menu itmes
    /// </summary>
    ArrayList m_menuItems = new ArrayList();

    /// <summary>
    /// the bounds of the control
    /// </summary>
    ArrayList m_bounds = new ArrayList();

    /// <summary>
    /// the selected item
    /// </summary>
    private int m_selected = -1;

    /// <summary>
    /// if currently displaying the menu
    /// </summary>
    private bool m_displayingMenu = false;

    /// <summary>
    /// a timer
    /// </summary>
    private System.Windows.Forms.Timer m_timer =  new System.Windows.Forms.Timer();

    #endregion

    #region properties

    /// <summary>
    /// gets the menu items
    /// </summary>
    public ArrayList MenuItems
    {
      get { return m_menuItems; }
    } // MenuItems
   
    #endregion
    
    #region methods
		
    /// <summary>
    /// initializes an instance of the MainMenu2 class
    /// </summary>
    public MainMenu2()
		{
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );

      m_timer.Interval = 100;
      m_timer.Tick +=new EventHandler(OnTimerTickHandler);

      this.Height = Theme.MenuMiddleImage.Height;
		} // MainMenu2()


    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );

      if( m_selected != -1 )
      {
        ShowMenuItem( ( Viatronix.UI.MenuItemEx ) m_menuItems[ m_selected ] );
      }
    } // OnMouseDown( MouseEventArgs args )


    /// <summary>
    /// Called when the mouse leaves the control.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this override.</param>
    protected override void OnMouseLeave( EventArgs args )
    {
      base.OnMouseLeave( args );
      
      if( !m_displayingMenu )
      {
        m_selected = -1;
        Invalidate();
      }
    } // OnMouseLeave( EventArgs args )

    
    /// <summary>
    /// Called when the mouse is moved within the control.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this override.</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {
      base.OnMouseMove( args );

      int selected = -1;
      foreach( RectangleF bounds in m_bounds )
      {
        if( bounds.Contains( args.X, args.Y ) )
        {
          selected = m_bounds.IndexOf( bounds );
        }
      }

      if( selected != m_selected )
      {
        m_selected = selected;
        Invalidate();
      }
    } // OnMouseMove( MouseEventArgs args )


    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( System.Windows.Forms.PaintEventArgs args )
    {
      base.OnPaint( args );

      UpdatePositions();

      Graphics gfx = args.Graphics;

      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      gfx.FillRectangle( Theme.MenuMiddleBrush, this.ClientRectangle );

      gfx.DrawImage( Theme.MenuLeftImage, 0, 0 );

      gfx.DrawImage( Theme.MenuRightImage, this.Width - Theme.MenuRightImage.Width, 0 );

      System.Drawing.StringFormat format = new StringFormat();
      format.Alignment = StringAlignment.Center;
      format.LineAlignment = StringAlignment.Center;

      foreach( MenuItem item in this.MenuItems )
      {
        int index =  m_menuItems.IndexOf( item );

        RectangleF layout = ( RectangleF ) ( m_bounds[ index ] );

        if( m_selected == index )
        {
          gfx.FillRectangle( Theme.SelectedBrush, layout );
          gfx.DrawRectangle( Theme.SelectedPen, layout.X, layout.Y, layout.Width, layout.Height );
        }
        gfx.DrawString( item.Text, Theme.MenuFont, Theme.MenuTextBrush, layout, format );
        
        if( index <  ( m_menuItems.Count - 1)  )
        {
          float x1 = layout.Right + ( ITEM_SPACING / 2.0F );          
          gfx.DrawLine( Pens.Black, x1, layout.Top + 4, x1, layout.Bottom - 4 );
          
          float x2 = x1 + 1.0F;
          gfx.DrawLine( Pens.Gainsboro, x2, layout.Top + 4, x2, layout.Bottom - 4 );
        }
      } // foreach( MenuItem item in this.MenuItems )

    } // OnPaint( System.Windows.Forms.PaintEventArgs args )


    /// <summary>
    /// Called when the control is resized
    /// </summary>
    /// <param name="args"></param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( EventArgs args )



    /// <summary>
    /// Performs the work of setting the specified bounds of this control.
    /// </summary>
    /// <param name="x">The new Left property value of the control.</param>
    /// <param name="y">The new Top property value of the control.</param>
    /// <param name="width">The new Width property value of the control.</param>
    /// <param name="height">The new Height property value of the control.</param>
    /// <param name="specified">A bitwise combination of the BoundsSpecified values.</param>
    protected override void SetBoundsCore( int x, int y, int width, int height, BoundsSpecified specified )
    { 
      base.SetBoundsCore (x, y, width, Viatronix.UI.Theme.MenuMiddleImage.Height, specified ); 
    } // SetBoundsCore( int x, int y, int width, int height, BoundsSpecified specified )
 

    /// <summary>
    /// Handles the tick event
    /// </summary>
    /// <param name="sender">Sender of event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void OnTimerTickHandler( object sender, EventArgs args )
    {
      
      if( ! m_displayingMenu )
      {
        m_timer.Stop();
        return;
      }

      Point pos = this.PointToClient( Control.MousePosition );
      foreach( RectangleF bounds in m_bounds )
      {
        if( bounds.Contains( pos.X, pos.Y ) )
        {
          int selected = m_bounds.IndexOf( bounds );

          if( selected != m_selected )
          {
            m_timer.Stop();

            Win32.User32.EndMenu();

            m_selected = selected;
            
            Win32.User32.PostMessage( this.Handle, (int) Win32.Msgs.WM_COMMAND, (uint) selected, 0 );

            return;
          }
        }
      } // foreach( RectangleF bounds in m_bounds )

    } // OnTimerTickHandler( object sender, EventArgs args )


    /// <summary>
    /// Handles window messages for this control
    /// </summary>
    /// <param name="m">the message</param>
    protected override void WndProc( ref Message m )
    {
      base.WndProc (ref m);

      if( m.Msg == (int)Win32.Msgs.WM_COMMAND )
      {
        int param  = m.WParam.ToInt32();
        if( param >= 0 && param < m_menuItems.Count )
        {
          Refresh();
          ShowMenuItem( ( Viatronix.UI.MenuItemEx ) m_menuItems[ m.WParam.ToInt32() ] );
        }
      }
    } // WndProc( ref Message m )


    /// <summary>
    /// Shows a menu item
    /// </summary>
    /// <param name="item">the item to show</param>
    private void ShowMenuItem( Viatronix.UI.MenuItemEx item )
    {
      item.PerformPopup();
        
      Viatronix.UI.ContextMenu context =  new Viatronix.UI.ContextMenu();
      
      context.SetOwnerDraw();

      Viatronix.UI.MenuItemEx[] items = new Viatronix.UI.MenuItemEx[ item.MenuItems.Count ];
      for( int index = 0; index < items.Length; ++index )
        items[ index ] = (Viatronix.UI.MenuItemEx)item.MenuItems[ index ];
      
      context.MenuItems.AddRange( items );

      RectangleF bounds = ( RectangleF ) m_bounds[ m_selected ];
        
      m_displayingMenu = true;

      m_timer.Start();
      
      context.Show( this, new Point( (int)bounds.Left, (int)bounds.Bottom ) );
      
      m_timer.Stop();
      
      m_displayingMenu = false;
    } // ShowMenuItem( Viatronix.UI.MenuItemEx item )
    
    
    /// <summary>
    /// Update the positions of the menu itmes
    /// </summary>
    private void UpdatePositions()
    {
      m_bounds.Clear();
      
      float x = ITEM_SPACING;

      using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
      {
        foreach( MenuItem item in this.MenuItems )
        {
          SizeF size = gfx.MeasureString( item.Text, Theme.MenuFont );

          RectangleF bounds = new RectangleF( x, 0,  size.Width, this.Height );
          bounds.Inflate( 0.0F, -4.0F );

          m_bounds.Add( bounds );

          x +=  size.Width + ITEM_SPACING;
        } // foreach( MenuItem item in this.MenuItems )
      } // using( Graphics gfx = Graphics.FromHwnd( this.Handle ) )
    } // UpdatePositions()
    
    #endregion

  } // class MainMenu2
} // namespace Viatronix.UI

#region revision history

// $Log: MainMenu2.cs,v $
// Revision 1.3  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.1  2005/02/28 21:09:35  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/MainMenu2.cs,v 1.3 2005/03/08 19:11:06 frank Exp $
// $Id: MainMenu2.cs,v 1.3 2005/03/08 19:11:06 frank Exp $

#endregion
