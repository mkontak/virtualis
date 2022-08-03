// $Id: ColorAdjustor.cs,v 1.3 2007/06/05 16:39:01 romy Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: gayathri srikant( gai@viatronix.net )
//         George Economos (george@viatronix.com)

using System;
using System.Resources;
using System.Reflection;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Design;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using Viatronix.UI;

namespace Viatronix.UI
{
	/// <summary>
	/// Repesents a control that allows color selection as well as alpha adjustment.
	/// </summary>
  public class ColorAdjustor : System.Windows.Forms.Control
	{
    #region events

		/// <summary>
		/// Raised when the color value of the adjustor changes.
		/// </summary>
    public event EventHandler Changed;

    /// <summary>
    /// Raised when the user starst an alpha adjustment using the slider
    /// </summary>
    public event EventHandler AlphaAdjustBegin;

    /// <summary>
    /// Raised when the user ends an alpha adjustment using the slider
    /// </summary>
    public event EventHandler AlphaAdjustEnd;

    #endregion

    #region fields
    
    /// <summary>
    /// Minimum allowed alpha value.
    /// </summary>
    static private readonly int m_minAlpha = 0;
		
    /// <summary>
    /// Maximum allowed alpha value.
    /// </summary>
    static private readonly int m_maxAlpha = 255;

    /// <summary>
    /// Button up image.
    /// </summary>
    static private readonly Bitmap m_buttonImage;
    
    /// <summary>
    /// Button down image.
    /// </summary>
    static private readonly Bitmap m_buttonDownImage;
    
    /// <summary>
    /// Arrow image.
    /// </summary>
    static private readonly Bitmap m_arrowImage;

    /// <summary>
    /// Current color.
    /// </summary>
    private Color m_color = Color.FromArgb( 255, 0, 255, 255 );
    
    /// <summary>
    /// Indicates if the alpha value is being adjusted.
    /// </summary>
    private bool m_adjusting = false;
		
    /// <summary>
    /// Alpha adjustor's bounding rectangle.
    /// </summary>
    private Rectangle m_arrowBounds = new Rectangle();


    
    /// <summary>
    /// Fill image that is blended with current color.
    /// </summary>
    private Bitmap m_fillImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.fill_button.png" ) );

    /// <summary>
    ///  Raises alpha change events while sdjusting with the slider
    /// </summary>
    private bool m_raiseAlphaChangedEvents = true;

    private int m_xmin;

    private int m_xmax;
    
    #endregion
		
    #region properties

	/// <summary>
	/// Gets the arrow bounds.
	/// </summary>
    public Rectangle ArrowBounds
    {
      get { return m_arrowBounds; }
    }

 
		/// <summary>
		/// Gets or sets the color.
		/// </summary>
    public Color Color
		{
			get{return m_color;}
			set
			{
				m_color = value;
 				UpdateColorFill();
				Invalidate();
			} // set
		}	// Color


    /// <summary>
    /// Gets or sets the RaiseAlphaChangedEvents flags
    /// </summary>
    public bool RaiseAlphaChangedEvents
    {
      get { return m_raiseAlphaChangedEvents; }
      set { m_raiseAlphaChangedEvents = value; }
    } // RaiseAlphaChangedEvents

    
    #endregion

    #region methods

    /// <summary>
    /// Static initialization.
    /// </summary>
    static ColorAdjustor()
    {
      // load images from the assembly
      m_buttonImage = new Bitmap( Assembly.GetExecutingAssembly( ).GetManifestResourceStream( "Viatronix.UI.res.transparent_button.png" ) );
      m_buttonDownImage = new Bitmap( Assembly.GetExecutingAssembly( ).GetManifestResourceStream( "Viatronix.UI.res.transparent_button_down.png" ) );
      m_arrowImage = new Bitmap( Assembly.GetExecutingAssembly( ).GetManifestResourceStream( "Viatronix.UI.res.sm_arrow.png" ) );
    } // ColorAdjustor()


    /// <summary>
    /// Initializes the ColorAdjustor object with default values.
    /// </summary>
    public ColorAdjustor()
    {
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );

      UpdateColorFill( );


      m_xmin = ( int ) ( m_arrowImage.Width / 2.0F + 0.5F );
      m_xmax = m_fillImage.Width + m_xmin;

    } // ColorAdjustor()

    
    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // high quality settings
      Graphics gfx = args.Graphics;

      // is the button enabled?
      if( this.Enabled )
      {
        // draw the fill color image
        if( m_color.A > 0 )
          gfx.DrawImage( m_fillImage, m_xmin, 0, m_fillImage.Width, m_fillImage.Height );
 
        // is the left mouse button down and we are NOT adjusting the alpha value?
        if( Control.MouseButtons == MouseButtons.Left && !m_adjusting )
          gfx.DrawImage( m_buttonDownImage, m_xmin, 0, m_fillImage.Width, m_fillImage.Height );
        else
          gfx.DrawImage( m_buttonImage, m_xmin, 0, m_fillImage.Width, m_fillImage.Height );
 				
      } // if( this.Enabled )
      else
        ControlPaint.DrawImageDisabled( gfx, m_buttonImage, m_xmin, 0, this.BackColor );

      // draw the arrow image
      m_arrowBounds = GetArrowBounds();
      if ( this.Enabled )
        gfx.DrawImage(m_arrowImage, m_arrowBounds);
      else
        ControlPaint.DrawImageDisabled(gfx, m_arrowImage, m_arrowBounds.X, m_arrowBounds.Height, this.BackColor);
    } // OnPaint( args )

    
    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEvntArgs containg data specific to this event.</param>
    protected override void OnMouseMove( MouseEventArgs args )
		{
      base.OnMouseMove( args );

      // are we over the alpha adjustor button?
      Rectangle bounds = GetArrowBounds();
      //bounds.Inflate( 3, 3 );
      if ( bounds.Contains( args.X, args.Y ) || m_adjusting )
				this.Cursor = Cursors.SizeWE;
      else
        this.Cursor = Cursors.Hand;
				
	    if( m_adjusting )
			{
        int x = Math.Max( Math.Min( m_xmin, args.X ), m_xmax );
				
        // calculate new alpha value
        int alpha = ( int ) Utilities.Interpolate( m_xmin, args.X, m_xmax, m_minAlpha, m_maxAlpha );

        // clamp value
        alpha = Math.Max( m_minAlpha, Math.Min( alpha, m_maxAlpha ) );
        
			  // set the new color
        this.Color = Color.FromArgb( alpha, m_color.R, m_color.G, m_color.B );
        
        // force a repaint
        Refresh();
        
        // raise the Changed event
        if ( this.Changed != null && m_raiseAlphaChangedEvents )
          Changed( this, EventArgs.Empty );
			} // if( ( m_adjusting ) && ( args.X >= 0 ) && ( args.X <= ( m_fillImage.Width + halfArrow ) ) )
		} // OnMouseMove( args )

		
    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEvntArgs containg data specific to this event.</param>
    protected override void OnMouseDown( MouseEventArgs args )
		{
      // call parent class
      base.OnMouseDown( args );

			// is the left mouse button down?
      if( args.Button == MouseButtons.Left )
			{
        // did we click the alpha adjustor button?
        if( GetArrowBounds().Contains( args.X, args.Y ) )
				{
					// indicate that we are adjusting the alpha value
          m_adjusting = true;

          if( this.AlphaAdjustBegin != null )
            AlphaAdjustBegin( this, EventArgs.Empty );
					
          // capture the mouse
          this.Capture = true;				
        } // if( m_arrowBounds.Contains( args.X, args.Y ) )
				
        // did we click the button?
        else if( ( args.X < m_fillImage.Width ) && ( args.Y < m_fillImage.Height ) )
				{
					// force a redraw
          Refresh();
          
          // show the color picker dialog
          ColorDialog dialog = new ColorDialog();					
          if( dialog.ShowDialog( this ) == DialogResult.OK )
          {
            // set this color
            this.Color = dialog.Color;					
            
            // force a redraw
            Refresh();
            
            // raise the Changed event
            if( Changed != null )
              Changed( this, EventArgs.Empty );
          
          } // if( dlg.ShowDialog(this) == DialogResult.OK )				
        } // else if( (args.X < m_fillImage.Width ) && ( args.Y < m_fillImage.Height ) )
				else
					this.Cursor = Cursors.Default;
			
      } // if( args.Button == MouseButtons.Left  )
		} // OnMouseDown( args )

		
    /// <summary>
    /// Called when a mouse button is released.
    /// </summary>
    /// <param name="args">MouseEvntArgs containg data specific to this event.</param>
    protected override void OnMouseUp( MouseEventArgs args )
		{
      // call parent class
      base.OnMouseUp( args );
      
      // were we adjusting the alpha value?
      if( m_adjusting )
			{
				// indicate that we are not adjusting the alpha value anymore
        m_adjusting = false;

        if ( this.AlphaAdjustEnd != null )
          AlphaAdjustEnd( this, EventArgs.Empty );

        // release the mouse capture
        Capture = false;
			} // if( m_adjusting )
	
			// Invalidate entire window
      Invalidate();
		} // OnMouseUp( args )


    /// <summary>
    /// Updates the bounds for the alpha adjustor button.
    /// </summary>
    private Rectangle GetArrowBounds()
    {
      // half the width of the arrow image in pixels
      int halfArrow = ( int ) ( m_arrowImage.Width / 2.0F + 0.5F );

      // minimum and maximum allowed x positions
      int xmin = halfArrow;
      int xmax  = halfArrow + m_buttonDownImage.Width;

      // calculate the position of the arrow button
      int x = ( int ) Utilities.Interpolate( m_minAlpha, m_color.A, m_maxAlpha, xmin, xmax ) - xmin;
      int y = m_buttonImage.Height;

      // set the arrow bounds
      return new Rectangle( x, y, m_arrowImage.Width, m_arrowImage.Height );
    } // GetArrowBounds()

    /// <summary>
    /// Updates the fill image with the current color.
    /// </summary>
    private void UpdateColorFill()
    {
      // is the current color's opacity greater than zero?
      if ( m_color.A > 0 )
      {
        // for each column
        for ( int x = 0; x < m_fillImage.Width; ++x )
        {
          // for each row
          for ( int y = 0; y < m_fillImage.Height; ++y )
          {
            // get the pixel 
            Color pixel = m_fillImage.GetPixel( x, y );

            // set the pixel to the color
            if ( pixel.A > 0 )
              m_fillImage.SetPixel( x, y, m_color );

          } // for( int x=0; x < m_fillImage.Width; ++x )
        } // for( int y=0; y < m_fillImage.Height; ++y )
      } // if( m_color.A  > 0)
    } // UpdateColorFill()

    #endregion    
    
	} // class ColorAdjustor
} // namespace Viatronix.UI

#region revision history

// $Log: ColorAdjustor.cs,v $
// Revision 1.3  2007/06/05 16:39:01  romy
// added arrowound property
//
// Revision 1.2  2007/01/18 19:24:35  geconomos
// Issue# 5231: color slider not working as expected
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2003/05/25 18:12:06  michael
// .net --> .com
//
// Revision 1.13  2003/05/06 11:55:35  geconomos
// Issue # 3154: Adjustor not drawing properly when color has alpha set to 0
//
// Revision 1.12  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.11  2003/04/22 15:22:22  geconomos
// More coding standards.
//
// Revision 1.10  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.9  2002/12/12 15:39:47  geconomos
// Code cleanup.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ColorAdjustor.cs,v 1.3 2007/06/05 16:39:01 romy Exp $
// $Id: ColorAdjustor.cs,v 1.3 2007/06/05 16:39:01 romy Exp $

#endregion
