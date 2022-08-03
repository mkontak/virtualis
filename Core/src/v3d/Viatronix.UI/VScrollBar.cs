// $Id: VScrollBar.cs,v 1.4 2006/04/27 21:00:06 romy Exp $
//
// Copyright (c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace Viatronix.UI
{	
  /// <summary>
  /// Represents a "windows" type vertical scroll bar.
  /// </summary>
  public class VScrollBar : System.Windows.Forms.Control
  {
    #region event

    /// <summary>
    /// Raised when the scrollbar's value changes.
    /// </summary>
    public event EventHandler ValueChanged;

    #endregion
    
    #region fields
    
    /// <summary>
    /// Underlying standard window's vertical scrollbar ( shhhhh! )
    /// </summary>
    private System.Windows.Forms.VScrollBar m_scroll = new System.Windows.Forms.VScrollBar();
    
    /// <summary>
    /// Scrollbar color.
    /// </summary>
    private Color m_scrollbarColor = Color.FromArgb( 141, 149, 167 );
    
    /// <summary>
    /// Scrollbar outline color.
    /// </summary>
    private Color m_scrollbarOutlineColor = Color.FromArgb( 93, 101, 119 );
    
    /// <summary>
    /// "Up Arrow" button bounding box in client coordinates
    /// </summary>
    private Rectangle m_upArrowBounds = new Rectangle( 0, 0, 0, 0 );
    
    /// <summary>
    /// "Down arrow" button bounding box in client coordinates
    /// </summary>
    private Rectangle m_downArrowBounds = new Rectangle( 0, 0, 0, 0 );

    private EventHandler m_valueChangedHandler = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the lower limit of the scrollbar range.
    /// </summary>
    public int Minimum
    {
      get { return m_scroll.Minimum; }
      set { m_scroll.Minimum = value; Invalidate(); }
    } // Minimum

    
    /// <summary>
    /// Gets or sets the upper limit of the scrollbar range.
    /// </summary>
    public int Maximum
    {
      get { return m_scroll.Maximum; }
      set { m_scroll.Maximum = value; Invalidate(); }
    } // Maximum
    
    
    /// <summary>
    /// Gets or sets the value of the scrollbar.
    /// </summary>
    public int Value
    {
      get { return m_scroll.Value; }
      set { m_scroll.Value = value; Invalidate(); }
    } //Value

    
    /// <summary>
    /// Gets or sets the amount used to change the scrollbar value when using the arrow buttons. 
    /// </summary>
    public int SmallChange
    {
      get { return m_scroll.SmallChange; }
      set { m_scroll.SmallChange = value; Invalidate(); }
    } // SmallChange
    
    
    /// <summary>
    /// Gets or sets the amount used to change the scrollbar value 
    /// when scroll box is moved a large distance. 
    /// </summary>
    public int LargeChange
    {
      get { return m_scroll.LargeChange; }
      set { m_scroll.LargeChange = value; Invalidate(); }
    } // LargeChange

    
    #endregion

    #region initialization
    
    /// <summary>
    /// Initializes a VScrollBar object with default values.
    /// </summary>
    public VScrollBar()
    {
      m_valueChangedHandler = new EventHandler( this.OnValueChangedHandler );

      if( !Utilities.WinXpOrGreater )
      {
        // enable double buffering
        SetStyle( ControlStyles.AllPaintingInWmPaint , true );
        SetStyle( ControlStyles.UserPaint , true );
        SetStyle( ControlStyles.DoubleBuffer , true );

        // support transparent back color
        SetStyle( ControlStyles.SupportsTransparentBackColor , true );
      }
      else
        m_scroll.ValueChanged += m_valueChangedHandler; //new EventHandler( this.OnValueChangedHandler );

        
      // make the real scrollbar not visible
      m_scroll.Visible = Utilities.WinXpOrGreater;
      
      // add this secret scrollbar to the control
      this.Controls.Add( m_scroll );
    } // VScrollBar()

    
    #endregion
    
    #region drawing

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    { 
      if( ! Utilities.WinXpOrGreater )
      {
        // use high quality settings
        Graphics gfx = args.Graphics;
        gfx.SmoothingMode =  SmoothingMode.HighQuality;

        DrawBackground( gfx );
        
        DrawUpArrow( gfx );
        
        DrawDownArrow( gfx );
        
        DrawThumb( gfx ); 
      }

    } // OnPaint( args )
    
    
    /// <summary>
    /// Draws the background image for the scrollbar.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    void DrawBackground( Graphics gfx )
    {
      Utilities.DrawBackground( gfx, this );

      // get the control's client bounds
      Rectangle bounds = new Rectangle( 0, 0, this.Width, this.Height );
      
      // create the background shape
      PointF[] background = Shapes.BeveledEdge( bounds, Edges.TopRight | Edges.BottomRight, 10 );
      
      // fill in the shape with the scrollbar color
      using( SolidBrush brush = new SolidBrush( m_scrollbarColor ) )
        gfx.FillPolygon( brush, background );
    
    } // DrawBackground( gfx )


    /// <summary>
    /// Draws the up arrow.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    void DrawUpArrow( Graphics gfx )
    {
      // width and height of arrow
      const int height = 3;
      const int width = 3;
 
      // draw the outline of the button
      PointF[] upButton = Shapes.BeveledEdge( m_upArrowBounds, Edges.TopRight, 10 );
      using( Pen pen = new Pen( m_scrollbarOutlineColor, 2.0F ) )
        gfx.DrawPolygon( pen, upButton );

      // add some shading to the top of the button
      Rectangle top = new Rectangle( ( int )( upButton[ 0 ].X + 1.0F ), ( int ) ( upButton[ 0 ].Y ),  (int ) ( upButton[ 1 ].X - upButton[ 0 ].X  + 1.0F ) , height );
      top.Inflate( 1, 1 );
      using( LinearGradientBrush brush = new LinearGradientBrush( top, m_scrollbarOutlineColor, m_scrollbarColor, LinearGradientMode.Vertical ) )
      {
        top.Inflate( -1, -1 );
        //gfx.FillRectangle( brush, top );
      } // using ( LinearGradientBrush brush )

      // add some light to the bottom of the button
      Rectangle bottom = new Rectangle( m_upArrowBounds.X + 1, m_upArrowBounds.Bottom - height - 1, m_upArrowBounds.Width - 2, height );
      bottom.Inflate( 1, 1 );
      using( LinearGradientBrush brush = new LinearGradientBrush( bottom, m_scrollbarColor,  Color.FromArgb( 202, 209, 218 ), LinearGradientMode.Vertical ) )
      {
        bottom.Inflate( -1, -1 );
        gfx.FillRectangle( brush, bottom );
      } // using ( LinearGradientBrush brush )

      // add some shading to the right side of the button
      Rectangle right = new Rectangle( m_upArrowBounds.Right - width - 1, (int)(upButton[ upButton.Length - 4].Y - 1.0F ), width, (int)( m_upArrowBounds.Height - upButton[ upButton.Length - 4].Y ));
      right.Inflate( 1, 1 );
      using( LinearGradientBrush brush = new LinearGradientBrush( right, m_scrollbarColor, m_scrollbarOutlineColor, LinearGradientMode.Horizontal ) )
      {
        right.Inflate( -1, -1 );
        gfx.FillRectangle( brush, right );
      } // using ( LinearGradientBrush brush )

      // add some more light to the left side of the button
      Rectangle left = new Rectangle( m_upArrowBounds.X + 1, top.Bottom - 1 , width, m_upArrowBounds.Height - top.Height - 1 );
      using( LinearGradientBrush brush = new LinearGradientBrush( left,  Color.FromArgb( 202, 209, 218 ), m_scrollbarColor, LinearGradientMode.Horizontal ) )
      {
        gfx.FillRectangle( brush, left );
      } // using ( LinearGradientBrush brush )

    } // DrawUpArrow( gfx )

    
    /// <summary>
    /// Draws the down arrow.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    void DrawDownArrow( Graphics gfx )
    {
      // width of a horizonal effect
      const int width = 3;
      
      // height of a vertical effect
      const int height = 3;
 
      // draw a border around the button
      PointF[] downButton = Shapes.BeveledEdge( m_downArrowBounds, Edges.BottomRight, 10 );
      using( Pen pen = new Pen( m_scrollbarOutlineColor, 2.0F ) )
        gfx.DrawPolygon( pen, downButton );

      // add some shading to the top of the button
      Rectangle top = new Rectangle( ( int )( downButton[ 0 ].X + 1.0F ), m_downArrowBounds.Y,  (int ) ( downButton[ 1 ].X - downButton[ 0 ].X - 1.0F   ) , height - 1 );
      top.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( top, m_scrollbarOutlineColor, m_scrollbarColor, LinearGradientMode.Vertical ) )
      {
        top.Inflate( -1, -1 );
        gfx.FillRectangle( brush, top );
      } // using ( LinearGradientBrush brush )
      
      // add some light to the bottom of the button
      Rectangle bottom = new Rectangle( m_downArrowBounds.X + width, m_downArrowBounds.Bottom - height - 1, m_downArrowBounds.Width - 8, height );
      bottom.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( bottom, m_scrollbarColor,  Color.FromArgb( 202, 209, 218 ), LinearGradientMode.Vertical ) )
      {
        bottom.Inflate( -1, -1 );
        gfx.FillRectangle( brush, bottom );
      } // using ( LinearGradientBrush brush )

      // add some shading to the right side of the button
      Rectangle right = new Rectangle( ( int ) downButton[ 1 ].X - width , ( int ) downButton[ 0 ].Y + height, width, height );
      right.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( right, m_scrollbarColor, m_scrollbarOutlineColor, LinearGradientMode.Horizontal ) )
      {
        right.Inflate( -1, -1 );
        gfx.FillRectangle( brush, right );
      } // using ( LinearGradientBrush brush )

      // add some more light to the left side of the button
      Rectangle left = new Rectangle( m_downArrowBounds.X + 1, ( int ) ( downButton[ 0 ].Y + height ), width, m_upArrowBounds.Height - height - 1 );
      left.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( left,  Color.FromArgb( 202, 209, 218 ), m_scrollbarColor, LinearGradientMode.Horizontal ) )
      {
        left.Inflate( -1, -1 );
        gfx.FillRectangle( brush, left );
      } // using ( LinearGradientBrush brush )

    } // DrawDownArrow( gfx )
   
    
    /// <summary>
    /// Draws the thumb portion of the scrollbar.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    void DrawThumb( Graphics gfx )
    {
      // width and height of effect
      const int height = 3;
      const int width = 3;

      // get the bounds of the thumb from the undelying scrollbar
      Win32.SCROLLBARINFO sbi = new Win32.SCROLLBARINFO();
      sbi.cbSize =  System.Runtime.InteropServices.Marshal.SizeOf( typeof( Win32.SCROLLBARINFO ) );
      Win32.User32.GetScrollBarInfo( m_scroll.Handle, ( int ) unchecked ( ( int )Win32.CommonObjectIDs.OBJID_CLIENT ), ref sbi );

      Rectangle thumbBounds = new Rectangle( 1, sbi.xyThumbTop, this.Width - 2, sbi.xyThumbBottom - sbi.xyThumbTop );

      // create a shape with rounded corners from the thumb's bounds
      PointF[] thumbShape = Shapes.BeveledEdge( thumbBounds, Edges.All, 2 );
      
      // fill in the thumb with the scrollbar's color
      using( SolidBrush brush = new SolidBrush( m_scrollbarColor ) )
        gfx.FillPolygon( brush, thumbShape );

      // outline the thumb with the outline color
      using( Pen pen = new Pen( m_scrollbarOutlineColor ) )
        gfx.DrawPolygon( pen, thumbShape );

      
      // add some shading to the top of the button
      Rectangle top = new Rectangle( thumbBounds.X + 1, thumbBounds.Y, thumbBounds.Width - 2, height );
      PointF[] topShape = Shapes.BeveledEdge( top, Edges.TopLeft | Edges.TopRight, 2 );
      top.Inflate( 1, 1 );
      using( LinearGradientBrush brush = new LinearGradientBrush( top, m_scrollbarOutlineColor, m_scrollbarColor, LinearGradientMode.Vertical ) )
        gfx.FillPolygon( brush, topShape );
 
      // add some light to the bottom of the button
      Rectangle bottom = new Rectangle( thumbBounds.X + 1, thumbBounds.Bottom - height - 1, thumbBounds.Width - 2, height );
      PointF[] bottomShape = Shapes.BeveledEdge( bottom, Edges.BottomLeft | Edges.BottomRight, 2 );
      bottom.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( bottom, m_scrollbarColor,  Color.FromArgb( 202, 209, 218 ), LinearGradientMode.Vertical ) )
        gfx.FillPolygon( brush, bottomShape );

      // add some shading to the right side of the button
      Rectangle right = new Rectangle( thumbBounds.Right - width - 1, thumbBounds.Y + height - 1, width, thumbBounds.Height - top.Height - 1 );
      PointF[] rightShape = Shapes.BeveledEdge( right, Edges.BottomRight, 2 );
      right.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( right, m_scrollbarColor, m_scrollbarOutlineColor, LinearGradientMode.Horizontal ) )
        gfx.FillPolygon( brush, rightShape );

      // add some more light to the left side of the button
      Rectangle left = new Rectangle( 2, top.Bottom - 1 , width, thumbBounds.Height - top.Height - 1 );
      PointF[] leftShape = Shapes.BeveledEdge( left, Edges.BottomLeft, 2 );
      left.Inflate( 1, 1 );
      using ( LinearGradientBrush brush = new LinearGradientBrush( left,  Color.FromArgb( 202, 209, 218 ), m_scrollbarColor, LinearGradientMode.Horizontal ) )
        gfx.FillPolygon( brush, leftShape );
    } // DrawThumb( gfx ) 
    
    
    #endregion

    #region overrides
    
    /// <summary>
    /// Called when control is resized.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      if( !Utilities.WinXpOrGreater )
      {
        // disconnect from the ValueChanged event
        m_scroll.ValueChanged -= m_valueChangedHandler;//new EventHandler( this.OnValueChangedHandler );

        // set the bounds of the underlying scrollbar to the entire client area
        m_scroll.SetBounds( 0, 0, this.Width, this.Height );

        // store the current values of the scrollbar
        int min = m_scroll.Minimum;
        int max = m_scroll.Maximum;
        int current = m_scroll.Value;

        // reset  the scrollbar
        m_scroll.Minimum = 0;
        m_scroll.Maximum = 100;      
        
        // set the value of the underlying scrollbar to the lower limit of the range. This will allow us to use the
        // SCROLLBARINFO.xyThumbTop value to determine the size of the up arrow.
        m_scroll.Value = 0;
        Win32.SCROLLBARINFO sbi = new Win32.SCROLLBARINFO();
        sbi.cbSize =  System.Runtime.InteropServices.Marshal.SizeOf( typeof( Win32.SCROLLBARINFO ) );
        Win32.User32.GetScrollBarInfo( m_scroll.Handle, ( int ) unchecked ( ( int )Win32.CommonObjectIDs.OBJID_CLIENT ), ref sbi );
        
        m_upArrowBounds = new Rectangle( 0, 0, this.Width, sbi.xyThumbTop - 1 );
        m_upArrowBounds.Inflate( -1, -1 );

        // set the value of the underlying scrollbar to the upper limit of the range. This will allow us to use the
        // SCROLLBARINFO.xyThumbBZottom value to determine the size of the down arrow.
        m_scroll.Value = 100;
        sbi = new Win32.SCROLLBARINFO();
        sbi.cbSize =  System.Runtime.InteropServices.Marshal.SizeOf( typeof( Win32.SCROLLBARINFO ) );
        Win32.User32.GetScrollBarInfo( m_scroll.Handle, ( int ) unchecked ( ( int )Win32.CommonObjectIDs.OBJID_CLIENT ), ref sbi );

        m_downArrowBounds = new Rectangle( 0, sbi.xyThumbBottom + 1, this.Width, this.Height - sbi.xyThumbBottom - 1 );
        m_downArrowBounds.Inflate( -1, -1 );

        // restore the scrollbar's values
        m_scroll.Minimum = min;
        m_scroll.Maximum = max;
        m_scroll.Value = current;
        
        // wire back up to the ValueChanged event.
        m_scroll.ValueChanged += m_valueChangedHandler;//new EventHandler( this.OnValueChangedHandler );
      }
      else
        m_scroll.SetBounds( 0, 0, this.Width, this.Height );


      Invalidate();
    } // OnResize( args )

    
    /// <summary>
    /// Processes Windows messages.
    /// </summary>
    /// <param name="m">The Windows Message to process. </param>
    protected override void WndProc( ref Message m )
    {
      if( !Utilities.WinXpOrGreater )
      {
        // allow the underlying scrollbar handle WM_LBUTTONDOWN
        if( m.Msg == ( int ) Win32.Msgs.WM_LBUTTONDOWN  )
        {
          this.Capture = true;
          Win32.User32.SendMessage( m_scroll.Handle, m.Msg, ( uint ) m.WParam, ( uint ) m.LParam );        
        } // if( m.Msg == ( int ) Win32.Msgs.WM_LBUTTONDOWN  )
        
        else if( m.Msg == ( int ) Win32.Msgs.WM_LBUTTONUP  )
        {
          this.Capture = false;
          Win32.User32.SendMessage( m_scroll.Handle, m.Msg, ( uint ) m.WParam, ( uint ) m.LParam );        
        } // else if( m.Msg == ( int ) Win32.Msgs.WM_LBUTTONUP  )
        
        else if( m.Msg == ( int ) Win32.Msgs.WM_MOUSEMOVE  )
          Win32.User32.SendMessage( m_scroll.Handle, m.Msg, ( uint ) m.WParam, ( uint ) m.LParam );        
        
        else
          base.WndProc( ref m );
      }
      else
          base.WndProc( ref m );
    } // WndProc( m )

    
    #endregion

    #region "Secret" scrollbar event handler
    
    /// <summary>
    /// Event handler for the ValueChanged event for the underlying scroll bar.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void OnValueChangedHandler( object sender, EventArgs args )
    {
      // force the gui to redraw
      Refresh();
      
      // raise the ValueChanged event.
      if( this.ValueChanged != null )
        ValueChanged( this, System.EventArgs.Empty );
    
    } // OnValueChangedHandler( sender, args )
 
    
    #endregion
  
  } // class VScollBar
} // namespace Viatronix.UI

#region revision history

// $Log: VScrollBar.cs,v $
// Revision 1.4  2006/04/27 21:00:06  romy
// event handler fix
//
// Revision 1.3  2004/08/05 14:16:02  geconomos
// Moved code to check version of windows to utilities.
//
// Revision 1.2  2004/07/09 18:31:53  geconomos
// Added a mechansim to default to the default windows scrollbar when running on windows xp.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.7  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.6  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.5  2003/05/07 17:10:14  frank
// code review
//
// Revision 1.4  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.3  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.2  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.1  2002/12/17 21:30:14  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/VScrollBar.cs,v 1.4 2006/04/27 21:00:06 romy Exp $
// $Id: VScrollBar.cs,v 1.4 2006/04/27 21:00:06 romy Exp $

#endregion
