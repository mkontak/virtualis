// $Id: ProgressBar2.cs,v 1.10 2007/07/27 21:30:02 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Data;
using System.Windows.Forms;
using System.Threading;

namespace Viatronix.UI
{
  /// <summary>
  /// A custom ProgressBar control.
  /// </summary>
  public class ProgressBar2 : System.Windows.Forms.Control
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
    private int m_value = 0;

    /// <summary>
    /// Step size used in Increment() and Decrement().
    /// </summary>
    private int m_step = 1;

    /// <summary>
    /// Indicates if the text should be drawn.
    /// </summary>
    private bool m_drawText = true;

    /// <summary>
    /// draws the progress as a pulse.
    /// </summary>
    private bool m_pulse = false;

    /// <summary>
    /// ProgressChanged event
    /// </summary>
    public event EventHandler ProgressChanged = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a ProgressBar control.
    /// </summary>
    public ProgressBar2()
    {
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      this.BackColor = Color.Transparent;
    } // ProgressBar2()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the minimum value of the range for the control.
    /// </summary>
    public int Minimum
    {
      get { return m_minimum; }
      set { SetMinimum( value ); }
    } // Minimum

    
    /// <summary>
    /// Gets or sets the maximum value of the range for the control.
    /// </summary>
    public int Maximum
    {
      get { return m_maximum; }
      set { SetMaximum( value ); }
    } // Maximum
    
    
    /// <summary>
    /// Gets or sets the current position of the progress bar.
    /// </summary>
    public int Value
    {
      get { return m_value; }
      set { SetValue( value ); }
    } // Value


    /// <summary>
    /// Gets or sets the text for the progress bar
    /// </summary>
    public override string Text
    {
      get { return base.Text; }
      set { SetText( value ); }
    } // Text

    
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
    /// Gets or sets the display style of the progress bar.
    /// </summary>
    public bool Pulse
    {
      get { return m_pulse; }
      set { m_pulse = value; }
    } // Pulse

    #endregion

    #region methods

    /// <summary>
    /// Increments the position of the progress bar according to the step size.
    /// </summary>
    public void Increment()
    {
      Increment( m_step );
    } // Increment()


    /// <summary>
    /// Increments the position of the progress bar according to the specified size.
    /// </summary>
    /// <param name="value">The amount of position to move the progress bar.</param>
    public void Increment( int value )
    {
      if( m_value < m_maximum )
        this.Value = Math.Min( m_maximum, m_value + value );
      
      Invalidate();
    } // Increment( value )
    

    /// <summary>
    /// Decrements the position of the progress bar according to the step size.
    /// </summary>
    public void Decrement()
    {
      Decrement( m_step );
    } // Decrement()

    
    /// <summary>
    /// Decrements the position of the progress bar according to the specified size.
    /// </summary>
    /// <param name="value">The amount of position to move the progress bar.</param>
    public void Decrement( int value )
    {
      if( m_value > m_minimum )
        this.Value = Math.Max( m_minimum, m_value - value );
      
      Invalidate();
    } // Decrement( value )


    /// <summary>
    /// Paints the progress
    /// </summary>
    /// <param name="gfx">Graphics object</param>
    private void PaintProgress( Graphics gfx )
    {
      Bitmap track = Theme.ProgressBarHorzTrackImage;
      int fillX = 3;
      int fillWidth = this.Width - 6 - 1;

      // fill information
      Bitmap fill = Theme.ProgressBarHorzFillImage;
      int fillY = (track.Height - fill.Height) / 2;
      int fillHeight = this.Height - fillY * 2;
      float fillScale = (float)this.Height/fill.Height;

      // calculate the maximum x position for the fill
      int progress = ( int )Math.Round( fillWidth * (double)(m_value - m_minimum) / (double)( m_maximum - m_minimum ));

      // draw progress fill
      Theme.ProgressBarFillBrush.ScaleTransform( 1, fillScale );
      gfx.FillRectangle( Theme.ProgressBarFillBrush, fillX, fillY, progress, fillHeight );
      Theme.ProgressBarFillBrush.ResetTransform();

      // store the current clip bounds
      RectangleF clipBounds = gfx.ClipBounds;

      try
      {
        // does the text need to be drawn
        if( m_drawText && this.Text != string.Empty )
        {
          RectangleF layout = new RectangleF( (float)fillX, (float)fillY, (float)fillWidth, (float)fillHeight );

          StringFormat format = new StringFormat();
          format.Alignment = StringAlignment.Center;
          format.LineAlignment = StringAlignment.Center;

          // set the clip bounds up to the progress location
          gfx.SetClip( new RectangleF( (float)fillX, (float)fillY, (float)progress, (float)this.Height ) );

          gfx.DrawString( this.Text, this.Font, Theme.ProgressBarFillTextBrush, layout, format );

          // set the clip bounds to the area of the text not over current fill
          gfx.SetClip( new RectangleF( (float)( progress + fillX ), (float)fillY, (float)( fillWidth - progress ), (float)( fillHeight )));

          gfx.DrawString( this.Text, this.Font, Theme.ProgressBarTrackTextBrush, layout, format );
        }
      }
      finally
      {
        // restore the clip bounds
        gfx.SetClip( clipBounds );
      }
    } // PaintProgress( gfx )


    /// <summary>
    /// Paints the pulse style
    /// </summary>
    /// <param name="gfx">Graphics object</param>
    private void PaintKnightRider( Graphics gfx )
    {
      Bitmap track = Theme.ProgressBarHorzTrackImage;
      int fillX = 3;
      int fillWidth = this.Width - 6 - 1;
      
      // fill information
      Bitmap fill = Theme.ProgressBarHorzFillImage;
      int fillY = (track.Height - fill.Height) / 2;
      int fillHeight = this.Height - fillY * 2;
      float fillScale = (float)this.Height/fill.Height;
      float progressWidth = fillWidth * 0.2f;

      // calculate the maximum x position for the fill
      int progress = ( int )Math.Round(( fillWidth - progressWidth ) * (double)(m_value - m_minimum ) / (double)( m_maximum - m_minimum ));

      // draw progress fill
      Theme.ProgressBarFillBrush.ScaleTransform( 1, fillScale );
      gfx.FillRectangle( Theme.ProgressBarFillBrush, progress, fillY, progressWidth, fillHeight );
      Theme.ProgressBarFillBrush.ResetTransform();

      // store the current clip bounds
      RectangleF clipBounds = gfx.ClipBounds;

      try
      {
        // does the text need to be drawn
        if( m_drawText && this.Text != string.Empty )
        {
          RectangleF layout = new RectangleF( (float)fillX, (float)fillY, (float)fillWidth, (float)fillHeight );

          StringFormat format = new StringFormat();
          format.Alignment = StringAlignment.Center;
          format.LineAlignment = StringAlignment.Center;

          gfx.SetClip( new RectangleF( (float)fillX, (float)fillY, (float)progress, (float)this.Height ) );

          gfx.DrawString( this.Text, this.Font, Theme.ProgressBarTrackTextBrush, layout, format );

          // set the clip bounds up to the progress location
          gfx.SetClip( new RectangleF( (float)( progress + fillX ), (float)fillY, progressWidth, (float)this.Height ) );

          gfx.DrawString( this.Text, this.Font, Theme.ProgressBarFillTextBrush, layout, format );

          // set the clip bounds to the area of the text not over current fill
          gfx.SetClip( new RectangleF( (float)( progress + progressWidth + fillX ), (float)fillY, (float)( fillWidth - progress - progressWidth ), (float)( fillHeight )));

          gfx.DrawString( this.Text, this.Font, Theme.ProgressBarTrackTextBrush, layout, format );
        }
      }
      finally
      {
        // restore the clip bounds
        gfx.SetClip( clipBounds );
      }
    } // PaintKnightRider( gfx )


    /// <summary>
    /// Sets the minimum value
    /// </summary>
    /// <param name="min">min value</param>
    private void SetMinimum( int min )
    {
      if( this.InvokeRequired )
        Invoke( new SetterDelegate< int >( this.SetMinimum ), min );
      else
      {
        m_minimum = min;
        m_maximum = Math.Max( m_minimum, m_maximum );
        SetValue( Math.Min( Math.Max( m_value, m_minimum ), m_maximum ));

        Invalidate();
      }
    } // SetMinimum( min )


    /// <summary>
    /// Sets the maximum value
    /// </summary>
    /// <param name="max">max value</param>
    private void SetMaximum( int max )
    {
      if( this.InvokeRequired )
        Invoke( new SetterDelegate< int >( this.SetMaximum ), max );
      else
      {
        m_maximum = max;
        m_minimum = Math.Min( m_minimum, m_maximum );
        SetValue( Math.Min( Math.Max( m_value, m_minimum ), m_maximum ));

        Invalidate();
      }
    } // SetMaximum( max )


    /// <summary>
    /// Sets the current value
    /// </summary>
    /// <param name="value">current value</param>
    private void SetValue( int value )
    {
      if( this.InvokeRequired )
        Invoke( new SetterDelegate<int>( this.SetValue ), value );
      else
      {
        int newValue = Math.Min( Math.Max( value, m_minimum ), m_maximum );
        if( m_value != newValue )
        {
          m_value = newValue;

          if( this.ProgressChanged != null )
            this.ProgressChanged( this, EventArgs.Empty );

          Invalidate();
        }
      }
    } // SetValue( value )


    /// <summary>
    /// Sets the text
    /// </summary>
    /// <param name="text">text</param>
    private void SetText( string text )
    {
      if( this.InvokeRequired )
        Invoke( new SetterDelegate<string>( this.SetText ), text );
      else
      {
        base.Text = text;
        Invalidate();
      }
    } // SetText( text )

    #endregion

    #region override methods

    /// <summary>
    /// Called when the control resizes itself
    /// </summary>
    /// <param name="args">EventArgs containing data related to this override.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
    {

      Graphics gfx = args.Graphics;

      // set to the highest quality settings
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      Bitmap track = Theme.ProgressBarHorzTrackImage;
      int fillX = 2;
      int fillWidth = this.Width - 6;
      float trackScale = (float)this.Height/track.Height;
      
      // draw left track
      gfx.DrawImage( track, new Rectangle( 0, 0, 3, this.Height ), new Rectangle( 0, 0, 3, track.Height ), GraphicsUnit.Pixel );

      // draw center track
      Theme.ProgressBarTrackBrush.ScaleTransform( 1, trackScale );
      gfx.FillRectangle( Theme.ProgressBarTrackBrush, fillX, 0, fillWidth, this.Height );
      Theme.ProgressBarTrackBrush.ResetTransform();

      // draw right track
      gfx.DrawImage( track, new Rectangle( this.Width-4, 0, 3, this.Height ), new Rectangle( 6, 0, 3, track.Height ), GraphicsUnit.Pixel ); 

      // fill information
      Bitmap fill = Theme.ProgressBarHorzFillImage;
      int fillY = (track.Height - fill.Height) / 2;
      int fillHeight = this.Height - fillY * 2;
      float fillScale = (float)this.Height/fill.Height;
      float progressWidth = ( this.Pulse ) ? fillWidth * 0.2f : 0;

      float totalWidth = ( this.Pulse ) ? fillWidth - progressWidth + 3 : fillWidth - progressWidth;
      int progress = ( int )Math.Round( totalWidth * (double)(m_value - m_minimum ) / (double)( m_maximum - m_minimum ));

      // draw progress fill
      Theme.ProgressBarFillBrush.ScaleTransform( 1, fillScale );

      if( this.Pulse )
        gfx.FillRectangle( Theme.ProgressBarFillBrush, progress, fillY, progressWidth, fillHeight );
      else
        gfx.FillRectangle( Theme.ProgressBarFillBrush, fillX, fillY, progress, fillHeight );

      Theme.ProgressBarFillBrush.ResetTransform();

      // store the current clip bounds
      RectangleF clipBounds = gfx.ClipBounds;

      try
      {
        // does the text need to be drawn
        if( m_drawText && this.Text != string.Empty )
        {
          RectangleF layout = new RectangleF( (float)fillX, (float)fillY, (float)fillWidth, (float)fillHeight );

          StringFormat format = new StringFormat();
          format.Alignment = StringAlignment.Center;
          format.LineAlignment = StringAlignment.Center;

          gfx.SetClip( new RectangleF( (float)fillX, (float)fillY, (float)progress, (float)this.Height ) );

          gfx.DrawString( this.Text, this.Font, (this.Pulse ) ? Theme.ProgressBarTrackTextBrush : Theme.ProgressBarFillTextBrush, layout, format );

          if( this.Pulse )
          {
            // set the clip bounds up to the progress location
            gfx.SetClip( new RectangleF( (float)( progress + fillX ), (float)fillY, progressWidth, (float)this.Height ) );

            gfx.DrawString( this.Text, this.Font, Theme.ProgressBarFillTextBrush, layout, format );
          }

          // set the clip bounds to the area of the text not over current fill
          gfx.SetClip( new RectangleF( (float)( progress + progressWidth + fillX ), (float)fillY, (float)( fillWidth - progress - progressWidth ), (float)( fillHeight )));

          gfx.DrawString( this.Text, this.Font, Theme.ProgressBarTrackTextBrush, layout, format );
        }
      }
      finally
      {
        // restore the clip bounds
        gfx.SetClip( clipBounds );
      }
    } // OnPaint( args )

    #endregion

  } // class ProgressBar2

  #region PulseGenerator class

  /// <summary>
  /// Generates increment and decrement pulses for a ProgressBar.
  /// </summary>
  public class PulseGenerator
  {
    
    #region fields

    /// <summary>
    /// Reference to a progress bar.
    /// </summary>
    private ProgressBar2 m_progress = null;

    /// <summary>
    /// Controls if an increment or a decrement operation should be performed
    /// </summary>
    private bool m_incrementPulse = true;


    /// <summary>
    /// creates the pulse
    /// </summary>
    private System.Windows.Forms.Timer m_timer = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a PulseGenerator.
    /// </summary>
    /// <param name="progress"></param>
    public PulseGenerator( ProgressBar2 progress )
    {
      m_progress = progress;
      m_timer = new System.Windows.Forms.Timer();
      m_timer.Tick += new EventHandler( this.OnTickHandler );
    } // PulseGenerator( progress )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the interval between pulses.
    /// </summary>
    public int Interval
    {
      get { return m_timer.Interval; }
      set { m_timer.Interval = value; }
    } // Interval

    #endregion

    #region methods

    /// <summary>
    /// Starts the pulses.
    /// </summary>
    public void StartPulse()
    {
      StopPulse();
      m_timer.Start();
    } // StartPulse()


    /// <summary>
    /// Stops the pulses.
    /// </summary>
    public void StopPulse()
    {
      m_timer.Stop();
    } // StopPulse()


    /// <summary>
    /// Receives the pulse from the timer.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnTickHandler( object sender, EventArgs args )
    {
      Pulse();
    } // Pulse()


    /// <summary>
    /// Performs the increment/decrement operations on the progress bar.
    /// </summary>
    private void Pulse()
    {
      if( m_progress.Value == m_progress.Maximum )
        m_incrementPulse = false;
      else if( m_progress.Value == m_progress.Minimum )
        m_incrementPulse = true;

      if( m_incrementPulse )
        m_progress.Increment();
      else
        m_progress.Decrement();
    } // Pulse()

    #endregion

  } // class PulseGenerator

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: ProgressBar2.cs,v $
// Revision 1.10  2007/07/27 21:30:02  gdavidson
// Added invokes to remove cross threading problems
//
// Revision 1.9  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.8.2.1  2007/03/26 18:18:53  gdavidson
// Issue #5516: Changed the threading timer to a winforms timer
//
// Revision 1.8  2007/03/01 14:49:42  gdavidson
// Comment code
//
// Revision 1.7  2006/07/19 13:52:30  gdavidson
// Modified how the pulse is created.
//
// Revision 1.6  2006/03/02 17:11:01  gdavidson
// Tweaked the frame draw by the progress bar
//
// Revision 1.5  2006/01/03 16:34:37  gdavidson
// Added a pulse style to the progress bar
//
// Revision 1.4  2005/09/26 13:42:50  gdavidson
// Added ProgressChanged event.
//
// Revision 1.3  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.1  2005/02/28 21:10:32  frank
// code review
//
// Revision 1.2  2004/11/12 19:03:15  gdavidson
// Set the background to transparent.
//
// Revision 1.1  2004/11/11 20:22:09  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ProgressBar2.cs,v 1.10 2007/07/27 21:30:02 gdavidson Exp $
// $Id: ProgressBar2.cs,v 1.10 2007/07/27 21:30:02 gdavidson Exp $

#endregion
