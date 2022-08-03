// $Id: ProgressBar.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright (c) 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: steve gagliardo( sgagliardo@viatronix.net )
//         George Economos (george@viatronix.com)

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
	/// <summary>
	/// Represents a Windows bar control.
	/// </summary>
  public class ProgressBar : System.Windows.Forms.Control
	{
    #region fields

    /// <summary>
    /// Minimum value for the range of the control.
    /// </summary>
    private int m_minimum = 0;

    /// <summary>
    /// Maximum value for the range of the control.
    /// </summary>
    private int m_maximum = 100;

    /// <summary>
    /// Current value of the control.
    /// </summary>
    private int m_value = 50;

    /// <summary>
    /// Step size used in Increment() and Decrement().
    /// </summary>
    private int m_step = 1;

    /// <summary>
    /// Indicates if the text should be drawn.
    /// </summary>
    private bool m_drawText = false;

    /// <summary>
    /// Border color for the control.
    /// </summary>
    private Color m_borderColor = Color.Black;
    
    /// <summary>
    /// Back color for the control.
    /// </summary>
    private Color m_backColor = Color.White;

    /// <summary>
    /// Text color for the control.
    /// </summary>
    private Color m_textColor = Color.Black;

    /// <summary>
    /// Text highlight color for the control.
    /// </summary>
    private Color m_textHilightColor = Color.White;

    /// <summary>
    /// Indicates if the control's value should wrap around.
    /// </summary>
    private bool m_wrap = false;

    /// <summary>
    /// Timer used for animation.
    /// </summary>
    private System.Windows.Forms.Timer m_timer;
    
    /// <summary>
    /// Control's comopnents container.
    /// </summary>
    private System.ComponentModel.IContainer components;
    
    #endregion

    #region properties
    
    /// <summary>
    /// Gets or sets the minimum value of the range for the control.
    /// </summary>
    public int Minimum
    {
      get { return m_minimum; }

      set
      {
        m_minimum = value;
        m_maximum = Math.Max( m_minimum, m_maximum );
        m_value = Math.Min( Math.Max( m_value, m_minimum ), m_maximum );

        Invalidate();
      }
    } // Minimum

    
    /// <summary>
    /// Gets or sets the maximum value of the range for the control.
    /// </summary>
    public int Maximum
    {
      get { return m_maximum; }

      set
      {
        m_maximum = value;
        m_minimum = Math.Min( m_minimum, m_maximum );
        m_value = Math.Min( Math.Max( m_value, m_minimum ), m_maximum );

        Invalidate();
      }
    } // Maximum
    
    
    /// <summary>
    /// Gets or sets the current position of the progress bar.
    /// </summary>
    public int Value
    {
      get { return m_value; }

      set
      {
        m_value = Math.Min( Math.Max( value, m_minimum ), m_maximum );

        Invalidate();
      }
    } // Value

    
    /// <summary>
    /// Get or sets the amount by which a call to Increment or Decrement changes the position of the progress bar.
    /// </summary>
    public int Step
    {
      get { return m_step; }

      set { m_step = value; }
    } // Step


    /// <summary>
    /// Gets or set whether or not the text is to be displayed.
    /// </summary>
    public bool DrawText
    {
      get { return m_drawText; }
      set { m_drawText = value; Invalidate(); }
    } // DrawText
    
    
    /// <summary>
    /// Gets or sets whether or not the value of the progress bar is allowed to wrap.
    /// </summary>
    public bool Wrap
    {
      get{ return m_wrap; }
      set { m_wrap = value; }
    } // Wrap

    
    /// <summary>
    /// Gets or sets the interval for animation events in milliseconds.
    /// </summary>
    public int AnimationInterval
    {
      get { return m_timer.Interval; }
      set { m_timer.Interval = value; }
    } // AnimationInterval
    
    
    /// <summary>
    /// Gets or sets the back color for this control.
    /// </summary>
    public new Color BackColor
    {
      get { return m_backColor; }
      set { m_backColor = value; Invalidate(); }
    } // BackColor
    
    
    /// <summary>
    /// Gets or sets the border color for this control.
    /// </summary>
    public Color BorderColor
    {
      get { return m_borderColor; }
      set { m_borderColor = value; Invalidate();}
    } // BorderColor
    
    /// <summary>
    /// Gets or sets the text color for this control.
    /// </summary>
    public Color TextColor
    {
      get { return m_textColor; }
      set { m_textColor = value; Invalidate(); }
    } // TextColor
    
    
    /// <summary>
    /// Gets or sets the text highlight color for this control.
    /// </summary>
    public Color TextHilightColor
    {
      get { return m_textHilightColor; }
      set { m_textHilightColor = value; Invalidate(); }
    } // TextHighlightColor
    
    #endregion
    
    #region methods

    /// <summary>
    /// Starts animation mode for the progress bar.
    /// </summary>
    public void StartAnimation()
    {
      m_timer.Start();
    } // StartAnimation()

    
    /// <summary>
    /// Stops animation mode for the progress bar.
    /// </summary>
    public void StopAnimation()
    {
      m_timer.Stop();
    }

    /// <summary>
    /// Increments the position of the progress bar according to the step size.
    /// </summary>
    public void Increment()
    {
      if( m_value == m_maximum )
      {
        if( m_wrap )
          m_value = m_minimum;
      }
      else
        m_value = Math.Min( m_maximum,m_value + m_step );
      
      Invalidate();
    } // Increment()
    
    
    /// <summary>
    /// Decrements the position of the progress bar according to the step size.
    /// </summary>

    public void Decrement()
    {
      if( m_value == m_minimum )
      {
        if( m_wrap )
          m_value = m_maximum;
      }
      else
        m_value = Math.Max( m_minimum,m_value - m_step );
      
      Invalidate();
    } // Decrement()
    
    
    #endregion
    
    #region initialization
    
    /// <summary>
    /// Initializes the PorgressBar with default values.
    /// </summary>
    public ProgressBar()
		{
      this.components = new System.ComponentModel.Container();
      this.m_timer = new System.Windows.Forms.Timer(this.components);
      
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      
      // support transparency
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      // wire up to the timer event
      m_timer.Tick += new EventHandler( this.OnTimerHandler );

      base.BackColor = Color.Transparent;
		} // ProgressBar()

    
    #endregion

    #region drawing
    
    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
		{
      // cache the grahics object
      Graphics gfx = args.Graphics;

      // set to the highest quality settings
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      // create a bounds object for this control
      Rectangle bounds = this.ClientRectangle;
      bounds.Width -= 1;
      bounds.Height -=1;

      // create the shape for the control
      PointF[] border = Shapes.BeveledEdge( bounds, Edges.All, 5 );

      // fill the outline
      using( SolidBrush brush = new SolidBrush( m_backColor ) )
        gfx.FillPolygon( brush,  border );
      
      // calculate the maximum x position for the fill
      float x = ( int )Math.Round( this.Width * (double)(m_value - m_minimum) / (double)( m_maximum - m_minimum ) );

      // store the current clip bounds
      RectangleF clipBounds = gfx.ClipBounds;

      // set the clip bounds up to the values x location
      gfx.SetClip( new RectangleF( 0.0F, 0.0F, x, ( float ) this.Height ) );

      // fill the outline
      using( SolidBrush brush = new SolidBrush( this.ForeColor ) )
        gfx.FillPolygon( brush,  border );

      // does the text need to be drawn
      if( m_drawText )
      {
        RectangleF layout = new RectangleF( 0.0F, 0.0F, ( float )( this.Width - 1 ), ( float )( this.Height - 1 ) );

        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;

        using( SolidBrush brush = new SolidBrush( m_textHilightColor ) )
          gfx.DrawString( this.Text, this.Font, brush, layout, format );

        // set the clip bounds to the area of the text not over current fill
        // set the clip bounds up to the values x location
        gfx.SetClip( new RectangleF( x, 0.0F, ( float ) this.Width - x - 1.0F, ( float ) this.Height ) );

        using( SolidBrush brush = new SolidBrush( m_textColor ) )
          gfx.DrawString( this.Text, this.Font, brush, layout, format );
      }
      
      // restore the clip bounds
      gfx.SetClip( clipBounds );
      
      // draw the outline
      using( Pen pen = new Pen( m_borderColor ) )
        gfx.DrawPolygon( pen,  border );
    
    } // OnPaint( args )


    #endregion
    
    #region animation

    /// <summary>
    /// Event handler for timer click event.
    /// </summary>
    /// <param name="sender">Sender of the event.</param>
    /// <param name="args"> EventArgs containing data related to this event.</param>
    private void OnTimerHandler( object sender, EventArgs args )
    {
      Increment();
      
      Refresh();
    } // OnTimerHandler( sender, args )
    
    #endregion
    
    #region general implementation
    
    /// <summary>
    /// Event handler for the Resize event.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      Invalidate();
    } // OnResize( args )

    #endregion
    
	} // class ProgressBar
} // namespace Viatronix.UI

#region revision history

// $Log: ProgressBar.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.4.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.9  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.8  2003/05/05 14:02:30  frank
// code review
//
// Revision 1.7  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.6  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.5  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.4  2003/04/14 15:06:31  geconomos
// More code cleanup.
//
// Revision 1.3  2003/04/11 16:23:07  geconomos
// Code cleanup.
//
// Revision 1.2  2003/04/10 18:30:55  geconomos
// Fixed bug in the setting of "Value" property.
//
// Revision 1.1  2003/04/10 13:28:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ProgressBar.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: ProgressBar.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion