// $Id: RangeSlider.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
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
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
  /// <summary>
  /// Provides a slider control that allows the user to speicfy a range of values.
  /// </summary>
  public class RangeSlider : System.Windows.Forms.Control
  {
    #region events
    
    /// <summary>
    /// Raised when the value of the slider changes.
    /// </summary>
    public event EventHandler Changed;
    
    #endregion

    #region constants

    /// <summary>
    /// Border in pixels to leave on left and right side of slider.
    /// </summary>
    static private readonly int m_padding = 10;
    
    /// <summary>
    /// Width in pixels of the button.
    /// </summary>
    static private readonly int m_buttonWidth = 8;
    
    /// <summary>
    /// Height in pixels of the button.
    /// </summary>
    static private readonly int m_buttonHeight = 24;

    #endregion

    #region fields

    /// <summary>
    /// Lower value of slider.
    /// </summary>
    private int m_min = 25;
    
    /// <summary>
    /// Upper value of slider.
    /// </summary>
    private int m_max = 75;
    
    /// <summary>
    /// Lower limit of range.
    /// </summary>
    private int m_sliderMin = 0;
    
    /// <summary>
    /// Upper limit of range.
    /// </summary>
    private int m_sliderMax = 100;
    
    /// <summary>
    /// Slider color.
    /// </summary>
    private Color m_color = Color.FromArgb( 255, 91, 61, 204 );
    
    /// <summary>
    /// Fill color.
    /// </summary>
    private Color m_barFillColor = Color.FromArgb( 255, 91, 61, 204 );

    /// <summary>
    /// Hilight color.
    /// </summary>
    private Color m_highlightColor = Color.FromArgb( 80, 0, 0, 255 );

    /// <summary>
    /// Hilight outline color.
    /// </summary>
    private Color m_highlightOutlineColor = Color.FromArgb( 0, 0, 255 );
   
    /// <summary>
    /// Position of left slider button in client coordinates.
    /// </summary>
    private int m_leftPosition = 0;
    
    /// <summary>
    /// Position of right slider button in client coordinates.
    /// </summary>
    private int m_rightPosition = 0;

    /// <summary>
    /// Minimum allowed position of left slider button in client coordinates.
    /// </summary>
    private int m_minimumXPosition = 0;
    
    /// <summary>
    /// Maximum allowed position of right slider button in client coordinates.
    /// </summary>
    private int m_maximumXPosition = 0;

    /// <summary>
    /// Points that define the Left button shape.
    /// </summary>
    private Point[] m_leftButton = new Point[ 5 ];
    
    /// <summary>
    /// Points that define the Right button shape.
    /// </summary>
    private Point[] m_rightButton = new Point[ 5 ];
    
    /// <summary>
    /// Frame shape.
    /// </summary>
    private PointF[] m_frame;

    /// <summary>
    /// Indicates if the left button is being moved.
    /// </summary>
    bool m_movingLeftButton = false;
    
    /// <summary>
    /// Indicates if the right button is being moved.
    /// </summary>
    bool m_movingRightButton = false;

    /// <summary>
    /// Indicates if the mouse is over the control
    /// </summary>
    bool m_hot = false;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the upper value of the slider.
    /// </summary>
    public int Max
    {
      get{ return m_max; }

      set
      { 
        m_max = value; 
        
        if( m_max < m_sliderMin )
          m_max = m_sliderMin;
        
        else if( m_max > m_sliderMax )
          m_max = m_sliderMax;        
        
        if( m_max < m_min )
          m_min = m_max;
        
        Range2Pos();
        
        Invalidate();         
      } // Set
    } // Max

    
    /// <summary>
    /// Gets or sets the loewer value of the slider.
    /// </summary>
    public int Min
    {
      get{ return m_min; }

      set
      { 
        m_min = value;
        if( m_min < m_sliderMin )
          m_min = m_sliderMin;
        
        else if( m_min > m_sliderMax )
          m_min = m_sliderMax;        
        
        if( m_min > m_max )
          m_max = m_min;
        
        Range2Pos();
        
        Invalidate();         
      } // set
    } // Min

    
    /// <summary>
    /// Gets or sets the upper limit of the range.
    /// </summary>
    public int SliderMax
    {
      get{ return m_sliderMax; }

      set
      { 
        m_sliderMax = value;
        
        if( m_max > m_sliderMax )
          m_max = m_sliderMax;
        
        Range2Pos();
        
        Invalidate();         
      } // Set
    } // SliderMax

    
    /// <summary>
    /// Gets or sets the upper limit of the range.
    /// </summary>
    public int SliderMin
    {
      get{ return m_sliderMin; }

      set
      { 
        m_sliderMin = value;
        
        if( m_min < m_sliderMin )
          m_min = m_sliderMin;
        
        Range2Pos();
        
        Invalidate( );         
      }
    } // SliderMin


    /// <summary>
    /// Gets or sets the slider's color
    /// </summary>
    public Color Color
    {
      get { return m_color; }
      set { m_color = value; Invalidate(); }
    }
    
    
    /// <summary>
    /// Gets or sets the slider's fill color.
    /// </summary>
    public Color FillColor
    {
      get { return m_barFillColor; }
      set { m_barFillColor = value; Invalidate(); }
    } // FillColor
    
    
    #endregion

    #region methods

    /// <summary>
    /// Sets the sliders range.
    /// </summary>
    /// <param name="min">Slider's lower value.</param>
    /// <param name="max">Slider's upper value.</param>
    public void SetRange( int min, int max )
    {
      this.Min = min;
      this.Max = max;

      Range2Pos();

      Invalidate();
    } // SetRange( min, max )

    
    /// <summary>
    /// Sets the sliders limits.
    /// </summary>
    /// <param name="min">Lower limit of slider's range.</param>
    /// <param name="max">Ipper limit of slider's range.</param>
    public void SetLimits( int min, int max )
    {
      this.SliderMin = min;
      this.SliderMax = max;

      Range2Pos();

      Invalidate();
    } // SetLimits( min, max )

    
    #endregion

    #region initialization

    /// <summary>
    /// Initializes the RangeSlider object with default values.
    /// </summary>
    public RangeSlider()
    {
      // enable double buffering
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      
      // support transparent back colors
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
      
      // set the BackColor property to transparent
      this.BackColor = Color.Transparent;
    
    } // RangeSlider()

    
    #endregion

    #region drawing

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // high quality settings
      Graphics gfx = args.Graphics;     
      gfx.SmoothingMode = SmoothingMode.HighQuality;

      DrawFrame( gfx );
      
      DrawBar( gfx );

      DrawButtons( gfx );
    
    } // OnPaint( args )

    
    /// <summary>
    /// Draws the slider's frame.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    private void DrawFrame( Graphics gfx )
    {
      using( SolidBrush fillbrush = new SolidBrush( Color.White ) ) 
        gfx.FillPolygon( fillbrush, m_frame );
      
      using( SolidBrush fillbrush2 = new SolidBrush( Color.FromArgb( 80, SystemColors.Highlight ) ) ) 
        gfx.FillPolygon( fillbrush2, m_frame  );

      // is the mouse over the slider?
      if( m_hot )
      {
        // fill in the frame with the highlight color
        using( SolidBrush hotBrush = new SolidBrush( m_highlightColor ) )
          gfx.FillPolygon( hotBrush, m_frame );
      } // if( m_hot )
       
      using( Pen pen = new Pen( m_hot? m_highlightOutlineColor : m_color  ) )
        gfx.DrawPolygon( pen, m_frame );
    } // DrawFrame(  gfx )

   
    /// <summary>
    /// Draws the slider's bar.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    private void DrawBar( Graphics gfx )
    {
      // bar width and height
      const int barHeight = 6;		
      int barWidth =  this.Width - ( 2 * m_padding ) - 1;
      
      // shadow width and height
      const int shadowWidth = 1;
      const int shadowHeight = 1;
      
      // center the bar vertically within the control
      int ypos = ( this.Height - barHeight ) / 2;      

      // draw the shadow parts of the bar
      using( SolidBrush brush = new SolidBrush( Color.FromKnownColor( KnownColor.ControlDarkDark ) ) )
      {
        // add a shadow to the top of the bar
        gfx.FillRectangle( brush, m_padding, ypos, barWidth, shadowHeight );
        
        // add a shadow to the left of the bar
        gfx.FillRectangle( brush, m_padding, ypos, shadowWidth, barHeight - 1 );
      
      } // using( SolidBrush brush = new SolidBrush( Color.FromKnownColor( KnownColor.ControlDarkDark ) ) )

      // draw the lighted parts of the bar
      using( SolidBrush brush = new SolidBrush( Color.FromKnownColor( KnownColor.ControlLightLight ) ) )
      {
        // add some lighting to the bottom of the bar
        gfx.FillRectangle( brush, m_padding, ypos + barHeight - shadowHeight - 1, this.Width - (2*m_padding) - 1, shadowHeight );
        
        // add some lighting to the left of the bar
        gfx.FillRectangle( brush, this.Width - m_padding - shadowWidth - 1, ypos, shadowWidth, barHeight - 1 );
      } // using( SolidBrush brush = new SolidBrush( Color.FromKnownColor( KnownColor.ControlLightLight ) ) )

      // fill in the area of the bar between the buttons
      using( SolidBrush brush = new SolidBrush( m_barFillColor ) )
        gfx.FillRectangle( brush, m_leftPosition , ypos + shadowHeight, m_rightPosition - m_leftPosition, barHeight - 1 - 2 * shadowHeight );			
    
    } // DrawBar( gfx )

    
    /// <summary>
    /// Draws the slider's left and right button.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    private void DrawButtons( Graphics gfx )
    {
      int height = this.Height - ( 2 * m_padding );
      int ypos = ( height - m_buttonHeight + ( 2 * m_padding ) ) / 2;

      // define the points for the left button
      m_leftButton[0].X = m_leftPosition - m_buttonWidth;	
      m_leftButton[0].Y = ypos;
      m_leftButton[1].X = m_leftPosition;				
      m_leftButton[1].Y = ypos;
      m_leftButton[2].X = m_leftPosition;				
      m_leftButton[2].Y = ypos + m_buttonHeight;
      m_leftButton[3].X = m_leftPosition - m_buttonWidth;	
      m_leftButton[3].Y = ypos + 2*m_buttonHeight/3;
      m_leftButton[4].X = m_leftPosition - m_buttonWidth;	
      m_leftButton[4].Y = ypos;
      
      // define the points for the right button
      m_rightButton[0].X = m_rightPosition + m_buttonWidth;	
      m_rightButton[0].Y = ypos;
      m_rightButton[1].X = m_rightPosition;				
      m_rightButton[1].Y = ypos;
      m_rightButton[2].X = m_rightPosition;				
      m_rightButton[2].Y = ypos + m_buttonHeight;
      m_rightButton[3].X = m_rightPosition + m_buttonWidth;	
      m_rightButton[3].Y = ypos + 2*m_buttonHeight/3;
      m_rightButton[4].X = m_rightPosition + m_buttonWidth;	
      m_rightButton[4].Y = ypos;
      
      // fill in the buttons shapes.
      using( SolidBrush brush = new SolidBrush( m_color ) )
      {
        gfx.FillPolygon( brush, m_leftButton );
        gfx.FillPolygon( brush, m_rightButton );
      } // using( SolidBrush brush = new SolidBrush( m_color ) )

      // outline the buttons shapes.
      using( Pen pen = new Pen( Color.White ) )
      {
        gfx.DrawPolygon( pen, m_leftButton );
        gfx.DrawPolygon( pen, m_rightButton );
      } // using( Pen pen = new Pen( Color.White ) )

    } // DrawButtons( gfx )
    
    #endregion

    #region mouse

    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      // did the mouse click the left button?
      if( GetLeftButtonBounds().Contains( args.X, args.Y ) )
      {
        this.Capture = true;						
        m_movingLeftButton = true;
        Invalidate();
      } // if( GetLeftButtonBounds().Contains( args.X, args.Y ) )

      // did the mouse click the right button?
      else if( GetRightButtonBounds().Contains( args.X, args.Y ) )
      {
        this.Capture = true;						
        m_movingRightButton = true;
        Invalidate();
      }
    } // OnMouseDown( args )


    /// <summary>
    /// Called when a mouse button is released.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event</param>
    protected override void OnMouseUp( MouseEventArgs args )
    {
      // release mouse capture
      this.Capture = false;

      // indicate that we are no longer moving the buttons
      m_movingLeftButton = false;
      m_movingRightButton = false;

      if( Changed != null )
        Changed( this, EventArgs.Empty );
    } // OnMouseUp( MouseEventArgs args )
		

    /// <summary>
    /// Called when a mouse is moved over the slider.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {  			
      // are we over one of the buttons?
      if( GetLeftButtonBounds().Contains( args.X, args.Y ) || GetRightButtonBounds().Contains( args.X, args.Y ) )
        this.Cursor = Cursors.SizeWE;
      else 
        this.Cursor = Cursors.Arrow;

      // are we moving the left button?
      if( m_movingLeftButton )
      {
        this.Cursor = Cursors.SizeWE;				
        
        // clamp the position
        m_leftPosition = Math.Min( Math.Max( args.X, m_minimumXPosition ), m_maximumXPosition );
        
        // is the right button less than the left?
        if( m_rightPosition < m_leftPosition ) 				
          m_rightPosition = m_leftPosition;	 // make them equal				
        
        // update the values
        Pos2Range();	
        
        // force a refresh
        Refresh();
        
        // raise the changed eent
        if( Changed != null )
          Changed( this, EventArgs.Empty );
      } // if( m_movingLeftButton )
      
      // are we moving the right button?
      else if( m_movingRightButton )
      {
        this.Cursor = Cursors.SizeWE;				
        m_rightPosition = args.X;		
		
        // clamp the position
        m_rightPosition = Math.Min( Math.Max( args.X, m_minimumXPosition ), m_maximumXPosition );

        if( m_leftPosition > m_rightPosition )				
          m_leftPosition = m_rightPosition;
        
        // update the values
        Pos2Range();	
        
        // force a refresh
        Refresh();
        
        // raise the changed eent
        if( Changed != null )
          Changed( this, EventArgs.Empty );
      } // else if( m_movingRightButton )
    } // OnMouseMove( args )


    /// <summary>
    /// Called when the mouse enters the slider.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnMouseEnter( EventArgs args )
    {
      // indicate that the mouse is over the control
      m_hot = true;
      Invalidate();
    } // OnMouseEnter( args )

    
    /// <summary>
    /// Called when the mouse leaves the slider.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnMouseLeave( EventArgs args )
    {
      // indicate that the mouse is no longer over the control
      m_hot = false;
      Invalidate();
    } // OnMouseLeave( args )
    
    
    #endregion
    
    #region layout
    
    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containg data related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      
      // control bounds
      Rectangle bounds = new Rectangle( 0, 0, this.Width - 1 , this.Height - 1 );
      
      // get the poisnt that define the frame shape
      m_frame = Shapes.BeveledEdge( bounds, Edges.All, 8 );

      // calculate the minimum and maximum allowed x locations
      m_minimumXPosition = m_buttonWidth + m_padding;
      m_maximumXPosition = this.Width - m_buttonWidth - m_padding - 1;

      // update the position of the buttons
      Range2Pos();

      // dirty the entire control
      Invalidate();

    } // OnResize( args )
	
  
    #endregion   

    #region implementation

    /// <summary>
    /// Converts the current range values to pixel locations.
    /// </summary>
    private void Range2Pos()
    {
      // get the width of the control minus the button widths and any border padding
      int available = this.Width - ( 2 * m_buttonWidth )- ( 2 * m_padding ) - 1;
			
      m_leftPosition = m_minimumXPosition + (int)Math.Round( (double)available * (double)( m_min - m_sliderMin ) / ( double )( m_sliderMax-m_sliderMin ) );
      m_rightPosition = m_minimumXPosition + (int)Math.Round( (double)available * (double)( m_max - m_sliderMin ) / ( double )( m_sliderMax-m_sliderMin ) );
    } // Range2Pos()
    

    /// <summary>
    /// Converts the current button pixel locations to range values.
    /// </summary>
    private void Pos2Range()
    {
      // get the width of the control minus the button widths and any border padding
      int available = this.Width - ( 2 * m_buttonWidth )- ( 2 * m_padding ) - 1;
			
      m_min = m_sliderMin + (int)Math.Round( ( double)( m_sliderMax-m_sliderMin) * ( double )( m_leftPosition - m_minimumXPosition ) / ( double ) available );
      m_max = m_sliderMin + (int)Math.Round( ( double )( m_sliderMax-m_sliderMin) * ( double )( m_rightPosition - m_minimumXPosition ) / ( double ) available );						
    } // Pos2Range()

    
    /// <summary>
    /// Gets the bounds in client coordinates of the left button.
    /// </summary>
    /// <returns>Bounds of left button in client coordinates</returns>
    private Rectangle GetLeftButtonBounds()
    {
      return new Rectangle
      (
        m_leftButton[0].X,
        m_leftButton[0].Y,
        m_leftButton[2].X - m_leftButton[0].X,
        m_leftButton[2].Y - m_leftButton[0].Y
      );			
    } // GetLeftButtonBounds()

    
    /// <summary>
    /// Gets the bounds in client coordinates of the right button.
    /// </summary>
    /// <returns>Bounds of right button in client coordinates</returns>
    private Rectangle GetRightButtonBounds()
    {
      return new Rectangle
      (
        m_rightButton[2].X,
        m_rightButton[0].Y,
        m_rightButton[0].X - m_rightButton[2].X,
        m_rightButton[2].Y - m_rightButton[0].Y
      );
    } // GetRightButtonBounds()

    #endregion
	
  } // class RangeSlider
} // namespace  Viatronix.UI

#region revision history

// $Log: RangeSlider.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.15.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.15  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.14  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.13  2003/05/05 14:09:00  frank
// code review
//
// Revision 1.12  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.11  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.10  2003/03/20 21:21:11  geconomos
// Issue# 2930: Control not properly initializing.
//
// Revision 1.9  2002/12/17 21:32:00  geconomos
// Forced control to refresh when setting the value.
//
// Revision 1.8  2002/12/12 15:39:19  geconomos
// Code cleanup.
//
// Revision 1.7  2002/12/09 17:06:03  geconomos
// code cleanup.
//
// Revision 1.6  2002/11/25 13:59:31  geconomos
// Initialized default colors to new values.
//
// Revision 1.5  2002/11/18 20:12:32  geconomos
// Headers and footers.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/RangeSlider.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: RangeSlider.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion