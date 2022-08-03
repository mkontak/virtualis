// $Id: Slider.cs,v 1.1.1.1.34.3 2009/03/26 14:46:18 gdavidson Exp $
//
// Copyright (c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace Viatronix.UI
{
  #region SliderType
  
  /// <summary>
  /// Specifies the type of slider.
  /// </summary>
  public enum SliderType
  {
    /// <summary>
    /// Standard slider type.
    /// </summary>
    Standard = 0,
    
    /// <summary>
    /// Draws a wedge in the background.
    /// </summary>
    Wedge = 1,
  } // enum SliderType

  #endregion  

  #region TextType
  
  /// <summary>
  /// Specifies the amount of text displayed
  /// </summary>
  public enum TextType
  {
    /// <summary>
    /// Standard text.
    /// </summary>
    Standard = 0,
    
    /// <summary>
    /// Minimal text
    /// </summary>
    Minimal = 1,
  } // enum TextType

  #endregion  
  
  #region Slider
  
  /// <summary>
  /// Represents a standard Windows track bar.
  /// </summary>
  public class Slider : System.Windows.Forms.Control
	{
    #region events

    /// <summary>
    /// Raised when the value of the slider changes.
    /// </summary>
    public event EventHandler Changed;

    #endregion
    
    #region constants
    
    /// <summary>
    /// Bevel amount for round corners.
    /// </summary>
    private static readonly int m_bevel = 5;
    
    /// <summary>
    /// Padding in pixels to add to borders of control.
    /// </summary>
    private static readonly int m_padding = 2 * m_bevel;
    
    #endregion
 
		# region fields

    /// <summary>
    /// Indicates if the text should be drawn.
    /// </summary>
    private bool m_showText = false;
    
    /// <summary>
    /// Text to append to values.
    /// </summary>
    private String m_suffix = String.Empty;
    
    /// <summary>
    /// Color of background and fill color of button.
    /// </summary>
    private Color m_sliderColor = Color.FromArgb( 255, 89, 116, 153 );
		
    /// <summary>
    /// Highlight color.
    /// </summary>
    private Color m_highlightColor = Color.FromArgb( 80, 0, 0, 255 );
   
    /// <summary>
    /// Indicates if the slider is being adjusted.
    /// </summary>
    private bool m_adjusting = false;
    
    /// <summary>
    /// Indicates if the mouse is over the control.
    /// </summary>
    private bool m_mouseOver = false;    

    /// <summary>
    /// Lower limit of slider range.
    /// </summary>
    private int m_sliderMin = 0;
    
    /// <summary>
    /// Upper limit of scrollbar range.
    /// </summary>
    private int m_sliderMax = 100;
    
    /// <summary>
    /// Current value of slider.
    /// </summary>
    private int m_value = 50;
    
    /// <summary>
    /// Polygon that defines the background shape.
    /// </summary>
    private PointF[] m_borderShape = new PointF[ 0 ];

    /// <summary>
    /// bounds of the slider
    /// </summary>
    private Rectangle m_sliderBounds = Rectangle.Empty;
    
    /// <summary>
    /// Current pixel position of button.
    /// </summary>
    private int m_position = 0;

    /// <summary>
    /// Minimum allowable x position for the button.
    /// </summary>
    private int m_minimumPosition = 0;
    
    /// <summary>
    /// Maximum allowable x position for the button.
    /// </summary>
    private int m_maximumPosition = 0;

    /// <summary>
    /// Slider background type.
    /// </summary>
    private SliderType m_sliderType = SliderType.Wedge;

    /// <summary>
    /// Text display type
    /// </summary>
    private TextType m_textType = TextType.Standard;

    /// <summary>
    /// Indicates if events should be fired while adjusting the slider.
    /// </summary>
    private bool m_eventsWhileAdjusting = false;

    /// <summary>
    /// Should the border be displayed?
    /// </summary>
    private bool m_displayBorder = true;

		# endregion
		
    # region properties
    
    /// <summary>
    /// Gets or sets the slider color.
    /// </summary>
    public Color SliderColor
    {
      get { return m_sliderColor; }
      set { m_sliderColor = value; Invalidate(); }
    } // SliderColor
    
    
    /// <summary>
    /// Gets or sets the slider's type.
    /// </summary>
    public SliderType SliderType
    {
      get { return m_sliderType; }
      set 
      { 
        m_sliderType = value; 
        OnResize( EventArgs.Empty );
      }
    } // SliderType


    /// <summary>
    /// Gets or sets the slider's text display type.
    /// </summary>
    public TextType TextType
    {
      get { return m_textType; }
      set 
      { 
        m_textType = value; 
        OnResize( EventArgs.Empty );
      }
    } // TextType
		
    
    /// <summary>
    /// Gets or sets the upper limit of the slider's range.
    /// </summary>
    public int Max
    {
      get{return m_sliderMax;}
      set
      {
        m_sliderMax = value;
        if( m_sliderMin > m_sliderMax )
          m_sliderMin = m_sliderMax;

        m_value = Math.Min( Math.Max( m_value, m_sliderMin ), m_sliderMax );

        Value2Pos();
        Invalidate();
      } // set
    } // Max

		
    /// <summary>
    /// Gets or sets the lower limit of the slider's range
    /// </summary>
    public int Min
    {
      get{ return m_sliderMin; }
      set
      {
        m_sliderMin = value;
        if( m_sliderMax < m_sliderMin )
          m_sliderMax = m_sliderMin;

        m_value = Math.Min( Math.Max( m_value, m_sliderMin ), m_sliderMax );

        Value2Pos();
        Invalidate();
      } // set
    } // Min

		
    /// <summary>
    /// Gets or sets the value of the scrollbar.
    /// </summary>
    public int Value
    {
      get{ return m_value; }
			
      set
      {
        m_value = Math.Min( Math.Max( value, m_sliderMin ), m_sliderMax );
        Value2Pos();
        Invalidate();
      } // set
    } // Value

    
    /// <summary>
    /// Gets or set if the text should be displayed on the slider.
    /// </summary>
    public bool ShowText
    {
      get { return m_showText; }
      set 
      { 
        m_showText = value; 
        OnResize( EventArgs.Empty );
      }
    } // ShowText

    
    /// <summary>
    /// Gets or sets the string that will be appended to the values.
    /// </summary>
    public String Suffix
    {
      get { return m_suffix; }
      set { m_suffix = value; Invalidate(); }
    } // Suffix

    /// <summary>
    /// Indicates if events should be raised when adjusting the slider.
    /// </summary>
    public bool EventsWhildAdjusting
    {
      get { return m_eventsWhileAdjusting; }
      set { m_eventsWhileAdjusting = value; }
    } // EventsWhildAdjusting


    /// <summary>
    /// Should the border be displayed.
    /// </summary>
    public bool DisplayBorder
    {
      get { return m_displayBorder;  }
      set { m_displayBorder = value; }
    } // DisplayBorder

    # endregion
		
    # region initialization
		
    /// <summary>
    /// Initializes a Slider object with default values.
    /// </summary>
    public Slider()
		{
      // enable double buffering
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
    } // Slider()
		
    # endregion
		
    #region painting

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {	
      // calculate the maximum and minimum allowed x positions
      m_minimumPosition = m_bevel;
      m_maximumPosition = m_sliderBounds.Width - m_bevel - 1;

      // check that the cuurent position is in range
      if( m_position < m_minimumPosition ) 
        m_position = m_minimumPosition;
      
      if( m_position > m_maximumPosition ) 
        m_position = m_maximumPosition;
      
      // highest rendering quality
      Graphics gfx = args.Graphics;
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      
      // fill brush
      Brush brush = new SolidBrush( m_sliderColor );

      // Should the border be displayed?
      if (m_displayBorder)
      {
        // outline the background shape
        using (Pen pen = new Pen(m_sliderColor))
        {
          gfx.DrawPolygon(pen, m_borderShape);
        }
      }
      
      if( m_sliderType == SliderType.Wedge )
      {

        PointF[] wedge = new PointF[ 3 ];
        wedge[ 0 ] = new PointF( m_bevel, m_sliderBounds.Height - m_bevel );
        wedge[ 1 ] = new PointF( m_sliderBounds.Width - m_bevel, m_sliderBounds.Height - m_bevel );
        wedge[ 2 ] = new PointF( m_sliderBounds.Width - m_bevel, m_bevel );

        gfx.FillPolygon( brush, wedge );

        using( Pen pen = new Pen( Color.White ) )
          gfx.DrawPolygon( pen,  wedge );
      } // if( m_sliderType == SliderType.Wedge )

      // fill the button
      PointF[] poly = Shapes.BeveledEdge( GetButtonBounds(), Edges.All, 5 );
      gfx.FillPolygon( brush, poly );
      
      // outline the button
      using( Pen pen = new Pen( Color.White ) )
        gfx.DrawPolygon( pen,  poly );

      // if we are showing text display it
      if( m_showText )
      {
        using( SolidBrush textBrush = new SolidBrush( this.ForeColor ) )
        {
          StringFormat format = new StringFormat();
          if( m_textType == TextType.Standard )
          {
            int y = this.Height - this.Font.Height;
            
            // print out the allowable min value
            format.Alignment = StringAlignment.Near;
            gfx.DrawString( string.Format( "{0} {1}", m_sliderMin, m_suffix ), this.Font, textBrush, 0, y, format );

            // print out the allowable max value
            format.Alignment = StringAlignment.Far;
            gfx.DrawString( string.Format( "{0} {1}", m_sliderMax, m_suffix ), this.Font, textBrush, this.Width, y, format );

            // print out the current value
            format.Alignment = StringAlignment.Center;
            gfx.DrawString( string.Format( "{0} {1}", m_value, m_suffix ), this.Font, textBrush, this.Width / 2, y, format );
          }
          else
          {
            format.LineAlignment = StringAlignment.Center;
            format.Alignment = StringAlignment.Center;
            Rectangle textBounds = new Rectangle( m_sliderBounds.Right, 1, this.Width - m_sliderBounds.Width, this.Height - 1 );
            gfx.DrawString( string.Format( "{0} {1}", m_value, m_suffix ), this.Font, textBrush, textBounds, format );
          }
        }
      } // if( m_showText )
    } // OnPaint( args )

    #endregion

    #region implementation

    /// <summary>
    /// Raises the changed event.
    /// </summary>
    /// <param name="args">EventArgs containing data releated to this event.</param>
    protected virtual void OnChanged( EventArgs args )
    {
      if( this.Changed != null )
        this.Changed( this, args );
    } // OnChange( args )
		

    /// <summary>
    /// Converts the current value to a pixel location.
    /// </summary>
    private void Value2Pos()
    {
      // convert the current value to a position
      int available = this.Width - m_padding - 1;			
      m_position = m_minimumPosition + (int)Math.Round( (double)available * (double)( m_value - m_sliderMin ) / (double)( m_sliderMax - m_sliderMin ) );
    } // Value2Pos()
    

    /// <summary>
    /// Converts the current button pixel location to a value.
    /// </summary>
    private void Pos2Value()
    {
      // convert the current position to a value
      int available = this.Width - m_padding - 1;			
      m_value = m_sliderMin + (int)Math.Round( ( double )( m_sliderMax-m_sliderMin) * ( double )( m_position - m_minimumPosition ) / (double) available );
    } // Pos2Value()
		

    /// <summary>
    /// Get the buttons bounds in client coordinates.
    /// </summary>
    /// <returns></returns>
    private Rectangle GetButtonBounds()
    {
      // return the button's bounding box in client coordinates
      Size buttonSize = GetButtonSize();
      return new Rectangle( m_position - ( buttonSize.Width / 2 ), 1, buttonSize.Width, buttonSize.Height );
    } // GetButtonBounds()
    
    
    /// <summary>
    /// Gets the size of the button.
    /// </summary>
    /// <returns>Size of button in pixels.</returns>
    private Size GetButtonSize()
    {
      // compute the size of the slider button
      int height = m_sliderBounds.Height - 4;
      int width  = Math.Min( 10, this.Width / 16 );

      // return the size
      return new Size( width, height );
    } // GetButtonSize()
    
    
    #endregion

    #region mouse

    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data releated to this event.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      this.Focus();

      // did the mouse hit the button?
      if( GetButtonBounds().Contains( args.X, args.Y ) )
      {
        // capture the mouse and indicate that we are adjusting the slider
        this.Capture = true;	
        m_adjusting = true;
        Invalidate( true );
      } // if( GetButtonBounds().Contains( args.X, args.Y ) )
    } // OnMouseDown( args )
		
    
    /// <summary>
    /// Called when a mouse button is released.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data releated to this event.</param>
    protected override void OnMouseUp( MouseEventArgs args )
    {
      // are we currently adjusting the slider?
      if( m_adjusting )
      {
        // release the mouse capture and indicate we aren't adjusting anymore.
        this.Capture = false;	
        m_adjusting = false;
      } // if( m_adjusting )

      // fire event to indicate change
      OnChanged( EventArgs.Empty );
    } // OnMouseUp( args )

    
    /// <summary>
    /// Called when the mouse is moved within the control.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data releated to this event.</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {
      // is the mouse over the button?
      if( GetButtonBounds().Contains( args.X, args.Y ) )
        this.Cursor = Cursors.SizeWE;
      else 
        this.Cursor = Cursors.Arrow;

      // is the slider being adjusted?
      if( m_adjusting )
      {
        // reapply the mouse cursor
        this.Cursor = Cursors.SizeWE;				
        
        // store the current mouse position
        m_position = args.X;		
        
        // ensure that the mouse is within the allowed x positions
        if( m_position < m_minimumPosition ) 
          m_position = m_minimumPosition;
        
        if( m_position > m_maximumPosition ) 
          m_position = m_maximumPosition;							
        
        // convert the position to a value
        Pos2Value();	
        
        Invalidate();
        
        // fire the changed event
        if( m_eventsWhileAdjusting )
          OnChanged( EventArgs.Empty );
      } // if( m_adjusting )
    } // OnMouseMove( args )

    
    /// <summary>
    /// Called when the mouse enters the slider.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnMouseEnter( EventArgs args )
    {
      // indicate that the mouse is over the control
      m_mouseOver = true;
      Invalidate();
    } // OnMouseEnter( args )
    
    
    /// <summary>
    /// Called when the mouse leaves the slider.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnMouseLeave( EventArgs args )
    {
      // indicate that the mouse has left the control
      m_mouseOver = false;
      Invalidate();
    } // OnMouseLeave( args )    

    #endregion

		#region overrides

    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnResize( EventArgs args )
		{
      // get the background shaped based on the size of the control
      if( m_showText )
      {
        int width = 0;
        int height = this.Font.Height;
        if( m_textType == TextType.Minimal )
        {
          SizeF size = this.CreateGraphics().MeasureString( string.Format( "{0} {1}", m_sliderMax, m_suffix ), this.Font );
          width = (int) ( size.Width + 0.5F );
          height = 0;
        }     
        
        m_sliderBounds = new Rectangle( 0, 0, this.Width - width - 1, this.Height - height - 1 );
        m_borderShape = Shapes.BeveledEdge( new Rectangle( 0, 0, this.Width - 1, this.Height - height - 1 ), Edges.All, m_bevel );
      }
      else
      {
        m_sliderBounds = new Rectangle( 0, 0, this.Width - 1, this.Height - 1 );
        m_borderShape = Shapes.BeveledEdge( m_sliderBounds, Edges.All, m_bevel );
      }

      // Update the position with the result of any changes that may have occurred.
      Value2Pos();

      Invalidate();
		} // OnResize( args )

		# endregion
	
  } // class Slider

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: Slider.cs,v $
// Revision 1.1.1.1.34.3  2009/03/26 14:46:18  gdavidson
// Added a minimal text display mode
//
// Revision 1.1.1.1.34.2  2009/03/24 20:44:30  kchalupa
// Added code to turn on or off the border and a property to control the behavior.
//
// Revision 1.1.1.1.34.1  2008/11/14 16:50:38  kchalupa
// Fix for slider having the wrong position on startup.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.8.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.8  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.7  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.6  2003/05/07 17:19:57  geconomos
// Fixed the setting of min and max properties for slider.
//
// Revision 1.5  2003/05/05 14:14:09  frank
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
// Revision 1.1  2002/12/09 17:05:32  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Slider.cs,v 1.1.1.1.34.3 2009/03/26 14:46:18 gdavidson Exp $
// $Id: Slider.cs,v 1.1.1.1.34.3 2009/03/26 14:46:18 gdavidson Exp $

#endregion