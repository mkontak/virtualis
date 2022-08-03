// $Id: SystemMenu.cs,v 1.5 2005/03/08 19:11:06 frank Exp $
//
// Copyright (c) 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (george@viatronix.com)

using System;
using System.Drawing;
using System.Windows.Forms;

namespace Viatronix.UI
{

  /// <summary>
  /// Mimics the operating system's system menu.
  /// </summary>
  public class SystemMenu : System.Windows.Forms.Control
  {

    #region enums

    /// <summary>
    /// Enumeration of a the possible button states.
    /// </summary>
    private enum ButtonState
    {
      Normal = 0,
      Hover = 1,
      Pressed = 2,
      Inactive = 3,
    } // ButtonState


    /// <summary>
    /// Enumeration of the possible button types.
    /// </summary>
    private enum ButtonType
    {
      None,
      Minimize,
      Maximize,
      Close      
    } // ButtonType

    #endregion
    
    #region fields

    /// <summary>
    /// Reference tot he parent form.
    /// </summary>
    private System.Windows.Forms.Form m_parentForm;

    /// <summary>
    /// The button size.
    /// </summary>
    private System.Drawing.Size m_buttonSize = Size.Empty;

    /// <summary>
    /// The bounds of the minimize button.
    /// </summary>
    private System.Drawing.Rectangle m_minimizeBounds = System.Drawing.Rectangle.Empty;

    /// <summary>
    /// The bounds of the maximize button.
    /// </summary>
    private System.Drawing.Rectangle m_maximizeBounds = System.Drawing.Rectangle.Empty;

    /// <summary>
    /// The bounds of the close button.
    /// </summary>
    private System.Drawing.Rectangle m_closeBounds = System.Drawing.Rectangle.Empty;
    
    /// <summary>
    /// Maintains a reference to the hover button.
    /// </summary>
    private ButtonType m_hoverButton = ButtonType.None;

    /// <summary>
    /// Determines if the minimize button should be shown.
    /// </summary>
    private bool m_showMinimize = true;

    /// <summary>
    /// Determines if the maximize button should be shown.
    /// </summary>
    private bool m_showMaximize = true;

    #endregion

    #region cosntruction

    /// <summary>
    /// Creates a new instance of a SystemMenu control.
    /// </summary>
    public SystemMenu()
    {
      // enable double buffering
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      
      // suport transparent back colors
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      this.TabStop = false;

      m_buttonSize.Width = Theme.SysmenuButtonImages.Height;
      m_buttonSize.Height = Theme.SysmenuButtonImages.Height;
    } // SystemMenu()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the parent form of the control.
    /// </summary>
    public System.Windows.Forms.Form ParentForm
    {
      get { return m_parentForm; }
      set { m_parentForm = value; }
    } // ParentForm


    /// <summary>
    /// Gets or sets if the minimize button should be shown.
    /// </summary>
    public bool ShowMinimize
    {
      get { return m_showMinimize; }
      set { m_showMinimize = value; }
    } // ShowMinimize


    /// <summary>
    /// Gets or sets if the maximize button should be shown.
    /// </summary>
    public bool ShowMaximize
    {
      get { return m_showMaximize; }
      set { m_showMaximize = value; }
    } // ShowMaximize

    #endregion

    #region methods

    /// <summary>
    /// This member overrides Control.OnPaint.
    /// </summary>
    /// <param name="args">A PaintEventArgs that contains the event data.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;

      // draw a minimize button
      if( this.ShowMinimize )
      {
        DrawImage( gfx, Theme.SysmenuButtonImages,          m_minimizeBounds.Location, GetButtonState( ButtonType.Minimize ) );
        DrawImage( gfx, Theme.SysmenuButtonMinimizeGlyphs,  m_minimizeBounds.Location, GetButtonState( ButtonType.Minimize ) );
      }
        
      // draw a maximize button
      if( this.ShowMaximize )
      {
        DrawImage( gfx, Theme.SysmenuButtonImages,  m_maximizeBounds.Location, GetButtonState( ButtonType.Maximize ) );
        if( m_parentForm.WindowState == FormWindowState.Maximized )
          DrawImage( gfx, Theme.SysmenuButtonRestoreGlyphs,  m_maximizeBounds.Location, GetButtonState( ButtonType.Maximize ) );
        else
          DrawImage( gfx, Theme.SysmenuButtonMaximizeGlyphs,  m_maximizeBounds.Location, GetButtonState( ButtonType.Maximize ) );
      }          
        
      DrawImage( gfx, Theme.SysmenuButtonCloseImages,     m_closeBounds.Location, GetButtonState( ButtonType.Close ) );
      DrawImage( gfx, Theme.SysmenuButtonCloseGlyphs,     m_closeBounds.Location, GetButtonState( ButtonType.Close ) );
    } // OnPaint( args )
        

    /// <summary>
    /// This member overrides Control.OnMouseMove.
    /// </summary>
    /// <param name="args">A MouseEventArgs that contains the event data.</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {
      base.OnMouseMove( args );

      if( this.ShowMinimize && m_minimizeBounds.Contains( args.X, args.Y ))
        m_hoverButton = ButtonType.Minimize;
      else if( this.ShowMaximize && m_maximizeBounds.Contains( args.X, args.Y ) )
        m_hoverButton = ButtonType.Maximize;
      else if( m_closeBounds.Contains( args.X, args.Y ) )
        m_hoverButton = ButtonType.Close;
      else
        m_hoverButton = ButtonType.None;

      Invalidate();
      
    } // OnMouseMove( args )

    
    /// <summary>
    /// This member overrides Control.OnMouseDown.
    /// </summary>
    /// <param name="args">A MouseEventArgs that contains the event data.</param>
    protected override void OnMouseDown( MouseEventArgs  args )
    {
      base.OnMouseDown( args );
      if( m_hoverButton != ButtonType.None )
        Invalidate();
    } // OnMouseDown(  args )

    
    /// <summary>
    /// This member overrides Control.OnMouseUp.
    /// </summary>
    /// <param name="args">A MouseEventArgs that contains the event data.</param>
    protected override void OnMouseUp( MouseEventArgs  args )
    {
      base.OnMouseDown( args );
      if( m_hoverButton != ButtonType.None )
      {
        if( m_hoverButton == ButtonType.Minimize )
        {
          Win32.User32.PostMessage( m_parentForm.Handle, (int)Win32.Msgs.WM_SYSCOMMAND, (int)Win32.SysCommands.SC_MINIMIZE, 0 );
        }
        else if( m_hoverButton == ButtonType.Maximize )
        {
          if( m_parentForm.WindowState == FormWindowState.Maximized )
          {
            Win32.User32.PostMessage( m_parentForm.Handle, (int)Win32.Msgs.WM_SYSCOMMAND, (int)Win32.SysCommands.SC_RESTORE, 0 );
          }
          else
          {
            Win32.User32.PostMessage( m_parentForm.Handle, (int)Win32.Msgs.WM_SYSCOMMAND, (int)Win32.SysCommands.SC_MAXIMIZE, 0 );
          }
        }
        else if( m_hoverButton == ButtonType.Close )
        {
          Win32.User32.PostMessage( m_parentForm.Handle, (int)Win32.Msgs.WM_SYSCOMMAND, (int)Win32.SysCommands.SC_CLOSE, 0 );
        }
      }
    } // OnMouseUp( args )


    /// <summary>
    /// This member overrides Control.OnMouseLeave.
    /// </summary>
    /// <param name="args">A EventArgs that contains the event data.</param>
    protected override void OnMouseLeave( EventArgs  args )
    {
      base.OnMouseLeave( args );
      m_hoverButton = ButtonType.None;
      Invalidate();
    } // OnMouseLeave( args )


    /// <summary>
    /// Draws the specified image at the specified location.
    /// </summary>
    /// <param name="gfx">The Graphics object.</param>
    /// <param name="image">The image being drawn.</param>
    /// <param name="location">The location to draw the image.</param>
    /// <param name="state">The state of the image.</param>
    private void DrawImage( Graphics gfx, Image image, Point location, ButtonState state )
    {
      Rectangle src = new Rectangle();
      src.X = image.Height * (int)state;
      src.Y = 0;
      src.Width = image.Height;
      src.Height = image.Height;
      
      gfx.DrawImage( image, location.X, location.Y, src, GraphicsUnit.Pixel );
    } // DrawImage( gfx, image, location, state )



    /// <summary>
    /// This member overrides Control.OnResize.
    /// </summary>
    /// <param name="args">A EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      UpdatePositions();
    } // OnResize( args )


    /// <summary>
    /// Update the bounds of the minimize, maximize and close buttons.
    /// </summary>
    private void UpdatePositions()
    {
      int m_padding = 2;

      int x3 = this.Width - m_buttonSize.Width - m_padding - 5;
      int x2 = x3 - m_buttonSize.Width - m_padding;
      int x1 = x2 - m_buttonSize.Width - m_padding;

      int y = ( this.Height - m_buttonSize.Height ) / 2;

      m_minimizeBounds  = new Rectangle( x1, y, m_buttonSize.Width, m_buttonSize.Height );
      m_maximizeBounds  = new Rectangle( x2, y, m_buttonSize.Width, m_buttonSize.Height );
      m_closeBounds     = new Rectangle( x3, y, m_buttonSize.Width, m_buttonSize.Height );
    } // UpdatePositions()


    /// <summary>
    /// Determines the button state based on the button type.
    /// </summary>
    /// <param name="type">The type of the button.</param>
    /// <returns>The button state for the specified button type.</returns>
    private ButtonState GetButtonState( ButtonType type )
    {
      ButtonState state = ButtonState.Normal;
      if( m_hoverButton == type )
      {
        if( Control.MouseButtons == MouseButtons.Left )
          state = ButtonState.Pressed;
        else
          state = ButtonState.Hover;
      }
      return state;
    } // GetButtonState( type )

    #endregion

  } // class SystemMenu
} // namespace Viatronix.UI

#region revision history

// $Log: SystemMenu.cs,v $
// Revision 1.5  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.4  2005/02/04 20:14:58  gdavidson
// SystemMenu cannot be accessed by the tab key.
//
// Revision 1.3.2.2  2005/02/28 18:25:07  gdavidson
// Code review
//
// Revision 1.3.2.1  2005/02/04 20:15:28  gdavidson
// SystemMenu cannot be accessed by the tab key.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/SystemMenu.cs,v 1.5 2005/03/08 19:11:06 frank Exp $
// $Id: SystemMenu.cs,v 1.5 2005/03/08 19:11:06 frank Exp $

#endregion
