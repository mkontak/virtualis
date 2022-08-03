// $Id: Button.cs,v 1.9 2007/10/01 14:51:43 gdavidson Exp $
//
// Copyright(c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using System.Reflection;
using System.Drawing.Text;
using System.Windows.Forms;
using Viatronix.UI.Desgin;

namespace Viatronix.UI
{
  /// <summary>
  /// Represents a "skinnable" Windows button control.
  /// </summary>
  public class Button : System.Windows.Forms.Button
  {
    #region fields
    
    /// <summary>
    ///  Indicates if the mouse is over the button.
    /// </summary>
    private bool m_mouseOver = false;

    /// <summary>
    /// Indicates if the mouse button is down.
    /// </summary>
    private bool m_mouseDown = false;       

    /// <summary>
    ///  Fill color for when the mouse is over the button.
    /// </summary>
    private Color m_hotColor = Color.FromArgb( 110, 120, 173 );

    /// <summary>
    /// Custom button images.
    /// </summary>
    private Bitmap[] m_skins = new Bitmap[ 3 ];

    /// <summary>
    ///  indicates if the old style button should be drawn
    /// </summary>
    private bool m_oldStockButton = false;

    /// <summary>
    /// indicates if the button shuld act like a toggle
    /// </summary>
    private bool m_toggleButton = false;

    /// <summary>
    /// indicates if the toggle button is checked
    /// </summary>
    private bool m_checked = false;

    /// <summary>
    /// left-part button image
    /// </summary>
    static private Bitmap m_buttonUpLeftImage;
    
    /// <summary>
    /// middle-part button image
    /// </summary>
    private static TextureBrush m_middleUpBrush;
    
    /// <summary>
    /// right-part button image
    /// </summary>
    static private Bitmap m_buttonUpRightImage;
    
    /// <summary>
    /// left-part button down image
    /// </summary>
    static private Bitmap m_buttonDownLeftImage;
    
    /// <summary>
    /// middle-part button down image
    /// </summary>
    private static TextureBrush m_middleDownBrush;
    
    /// <summary>
    /// right-part button down image
    /// </summary>    
    static private Bitmap m_buttonDownRightImage;

    /// <summary>
    /// left-part button hover image
    /// </summary>
    static private Bitmap m_buttonHoverLeftImage;
    
    /// <summary>
    /// middle-part button hover image
    /// </summary>
    private static TextureBrush m_middleHoverBrush;
    
    /// <summary>
    /// right-part button hover image
    /// </summary>    
    static private Bitmap m_buttonHoverRightImage;

    /// <summary>
    /// Checked changed event
    /// </summary>
    public event EventHandler CheckedChanged = null;

    #endregion

    #region properties
    
    /// <summary>
    /// Gets or sets the custom button images.
    /// </summary>
    [ Editor( typeof( ButtonImagesEditor ),typeof( UITypeEditor ) ) ]
    public Bitmap[] CustomImages
    {
      get{ return m_skins; }
      set{ m_skins = value; Invalidate(); }
    } // CustomImages


    /// <summary>
    /// Indicates if the stock button should be drawn in the "old style"
    /// </summary>
    public bool OldStockButton
    {
      get { return m_oldStockButton; }
      set { m_oldStockButton = value; }
    }

    /// <summary>
    /// Indicate if the button is in toggle mode
    /// </summary>
    public bool ToggleButton
    {
      get { return m_toggleButton; }
      set { m_toggleButton = value; }
    } // ToggleButton
     
    /// <summary>
    /// indicates the toggle state of the button
    /// </summary>
    public bool Checked
    {
      get { return m_checked; }
      set 
      { 
        m_checked = value; 
        OnCheckedChanged( EventArgs.Empty );      
      }
    } // Checked

    #endregion

    #region initialization
    
    /// <summary>
    /// static constructor
    /// </summary>
    static Button()
    {
      Assembly assembly = Assembly.GetExecutingAssembly();

      m_buttonUpLeftImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.button-up-left.png") );
      m_middleUpBrush = new TextureBrush( new Bitmap( assembly.GetManifestResourceStream( "Viatronix.UI.res.button-up-middle.png" )));
      m_buttonUpRightImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.button-up-right.png") );
 
      m_buttonDownLeftImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.button-down-left.png") );
      m_middleDownBrush = new TextureBrush( new Bitmap( assembly.GetManifestResourceStream( "Viatronix.UI.res.button-down-middle.png" )));
      m_buttonDownRightImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.button-down-right.png") );
 
      m_buttonHoverLeftImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.button-hover-left.png") );
      m_middleHoverBrush = new TextureBrush( new Bitmap( assembly.GetManifestResourceStream( "Viatronix.UI.res.button-hover-middle.png" )));
      m_buttonHoverRightImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.button-hover-right.png") );

    } // Button()

   
    /// <summary>
    /// Initializes the button object with default values.
    /// </summary>
    public Button()
    {     
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      
      // support transparent back colors
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      // default text color
      this.ForeColor = Color.Black;
    } // Button()

    
    #endregion
    
    #region drawing
        
    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {

      // draw background using helper
      Utilities.DrawBackground( args.Graphics, this );


      
      // NOTE: In order to draw a "skinned" button the up state image must be NON null.
      bool skinButton = m_skins[ 0 ] != null;
      if( skinButton )
        DrawSkinnedButton( args.Graphics );
      else
      {
        if( m_oldStockButton )
          DrawOldStockButton( args.Graphics );
        else
          DrawStockButton( args.Graphics );
      }
     
    } // OnPaint( args )

    
    /// <summary>
    /// Draws a skinned button using the custom images.
    /// </summary>
    /// <param name="gfx">Graphics object used for drawing.</param>
    private void DrawSkinnedButton( Graphics gfx )
    {
      // button state images
      Image normalImage, downImage, overImage = null;

      // cache the "up" state skin
      normalImage = m_skins[ 0 ];

      // cache the "down" skin if it is non null
      if( m_skins[ 1 ] != null )
        downImage = m_skins[ 1 ];
      else // otherwise use the "up" skin
        downImage = m_skins[ 0 ];

      // cache the "hover" skin if it is non null
      if( m_skins[ 2 ] != null )
        overImage = m_skins[ 2 ];
      else // otherwise use the "up" skin
        overImage = m_skins[ 0 ];
      
      // is the button enabled?
      if( this.Enabled )
      {
        // is the one of the mouse buttons down?
        if( m_mouseDown || ( m_toggleButton && m_checked ))
          gfx.DrawImage( downImage, 0, 0, downImage.Width, downImage.Height );

        else if( m_mouseOver && !m_toggleButton )
          gfx.DrawImage( overImage, 0, 0, overImage.Width, overImage.Height );

        else
          gfx.DrawImage( normalImage, 0, 0, normalImage.Width, normalImage.Height );
      } // if( this.Enabled )
      else
        ControlPaint.DrawImageDisabled( gfx, normalImage, 0, 0, this.BackColor );
    } // DrawCustomButton( gfx )
    
    
    /// <summary>
    /// Draws a stock button.
    /// </summary>
    /// <param name="gfx">Graphics object used for drawing.</param>
    private void DrawStockButton( Graphics gfx )
    {
      // highest quality rendering
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
      if ( this.BackColor != Color.Transparent ) gfx.Clear( this.BackColor );

      // get the bounding rectangle for the button
      Rectangle bounds = this.ClientRectangle;
      bounds.Inflate( -2, -2 );

      Image paneLeft           = m_mouseDown ? m_buttonDownLeftImage  : m_buttonUpLeftImage;
      TextureBrush middleBrush = m_mouseDown ? m_middleDownBrush      : m_middleUpBrush;
      Image paneRight          = m_mouseDown ? m_buttonDownRightImage : m_buttonUpRightImage;

      Pen borderTop    = m_mouseDown ? Pens.Black : Pens.White;
      Pen borderBottom = m_mouseDown ? Pens.White : Pens.Black;

      if( m_mouseDown || ( m_toggleButton && m_checked ))
      {
        paneLeft     = m_buttonDownLeftImage;
        middleBrush  = m_middleDownBrush;
        paneRight    = m_buttonDownRightImage;

      }
      else if( m_mouseOver && !m_toggleButton )
      {
        paneLeft     = m_buttonHoverLeftImage;
        middleBrush  = m_middleHoverBrush;
        paneRight    = m_buttonHoverRightImage;
      }
      else
      {
        paneLeft     = m_buttonUpLeftImage;
        middleBrush  = m_middleUpBrush;
        paneRight    = m_buttonUpRightImage;
      }
      
      middleBrush.ScaleTransform( 1, (float)this.Height / middleBrush.Image.Height ); 
      gfx.FillRectangle( middleBrush, 1, 0, this.Width-2, this.Height );
      middleBrush.ResetTransform();


      gfx.DrawImage( paneLeft, 1, 0, paneLeft.Width, this.Height );
      gfx.DrawImage( paneRight, this.Width - paneRight.Width - 1, 0, paneRight.Width, this.Height );

      gfx.DrawLine( borderTop, new Point( 0, 0 ), new Point( this.Width-1, 0 ));
      gfx.DrawLine( borderTop, new Point( 0, 0 ), new Point( 0, this.Height-1 ));
      gfx.DrawLine( borderBottom, new Point( 0, this.Height-1 ), new Point( this.Width-1, this.Height-1 ));
      gfx.DrawLine( borderBottom, new Point( this.Width-1, 0 ), new Point( this.Width-1, this.Height-1 ));

      // does the control have any associated text?
      if( this.Text != string.Empty )
      {
        RectangleF textBounds = new RectangleF( 0.0F, 0.0F, this.Width, this.Height );

        // center the text vertically and horizontally within the button's bounds
        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;

        // draw the text
        using( SolidBrush brush = new SolidBrush( this.Enabled? this.ForeColor : SystemColors.GrayText ) )
          gfx.DrawString( this.Text, this.Font, brush, textBounds, format );

        SizeF textSize = gfx.MeasureString( this.Text, this.Font );
        int textX = this.Width/2 - (int)( textSize.Width/2 );
        int textY = this.Height/2 - (int)( textSize.Height/2 );

        Rectangle focusBounds = new Rectangle( textX, textY, (int)textSize.Width, (int)textSize.Height );
        focusBounds.Inflate( 2, 2 );
        if( this.Focused )
          ControlPaint.DrawFocusRectangle( gfx, focusBounds );
        
      } // this.Text != string.Empty
    
      if ( this.Image != null || ( this.ImageList != null && this.ImageIndex >= 0 ) )
      {

        int offset = 2;
        int x = 0;
        int y = 0;


        switch ( this.ImageAlign )
        {
          case ContentAlignment.MiddleCenter:
            x = (bounds.Width / 2) - (this.Image.Width / 2) + offset;
            y = (bounds.Height / 2) - (this.Image.Height / 2) + offset;
            break;
          case ContentAlignment.MiddleLeft:
            x = offset * 2;
            y = (bounds.Height / 2) - (this.Image.Height / 2) + offset;
            break;
          case ContentAlignment.MiddleRight:
            x = bounds.Width - ( this.Image.Width );
            y = (bounds.Height / 2) - (this.Image.Height / 2) + offset;
            break;
          case ContentAlignment.TopCenter:
            x = (bounds.Width / 2) - (this.Image.Width / 2) + offset;
            y = offset;
            break;
          case ContentAlignment.TopLeft:
            x = offset * 2;
            y = offset;
            break;
          case ContentAlignment.TopRight:
            x = bounds.Width - ( this.Image.Width );
            y = offset;
            break;
          case ContentAlignment.BottomCenter:
            x = (bounds.Width / 2) - (this.Image.Width / 2) + offset;
            y = bounds.Top + bounds.Height - this.Image.Height;
            break;
          case ContentAlignment.BottomLeft:
            x = offset * 2;
            y = bounds.Top + bounds.Height - this.Image.Height;
            break;
          case ContentAlignment.BottomRight:
            x = bounds.Width - ( this.Image.Width );
            y = bounds.Top + bounds.Height - this.Image.Height;
            break;
        }

        gfx.DrawImageUnscaled(this.Image, x, y);

      }

    } // DrawStockButton( gfx )

    
    /// <summary>
    /// Draws the "old" stock button.
    /// </summary>
    /// <param name="gfx">Graphics object used for drawing.</param>
    private void DrawOldStockButton( Graphics gfx )
    {
      // highest quality rendering
      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
      
      // get the bounding rectangle for the button
      Rectangle bounds = this.ClientRectangle;
      bounds.Inflate( -2, -2 );

      // create the beveled button shape
      PointF[] border = Shapes.BeveledEdge( bounds, Edges.All, 3 );

      // is one of the mouse buttons down?
      if( m_mouseDown )
      {
        // create the gradient brush
        Color topColor = Color.FromArgb( 125, 141, 174 );
        Color bottomColor = Color.FromArgb( 199, 211, 221 );
        using ( LinearGradientBrush brush = new LinearGradientBrush( bounds, topColor, bottomColor, LinearGradientMode.Vertical ) )
        {
          // create blend function
          Blend blend = new Blend();
          blend.Factors = new float[] { 0.0F, .85F, .90F, 1.0F };
          blend.Positions = new float[] { 0.0F, .15F, .75F, 1.0F };
          brush.Blend = blend;
        
          // fill in the button
          gfx.FillPolygon( brush, border );
        }            
        
        // draw the outline of the button
        //using( Pen pen = new Pen( Color.FromArgb( 126, 143, 156 ), 1.5F ) )
        using( Pen pen = new Pen( Color.Black, 1.5F ) )
          gfx.DrawPolygon( pen, border );


      } // m_mouseDown
      else
      {
        // create the gradient brush
        Color topColor = Color.FromArgb( 246, 248, 249 );
        Color bottomColor = Color.FromArgb( 184, 192, 211 );
        using ( LinearGradientBrush brush = new LinearGradientBrush( bounds, topColor, bottomColor, LinearGradientMode.Vertical ) )
        {
          // create blend function
          Blend blend = new Blend();
          blend.Factors = new float[] { 0.0F, .30F, 1.0F };
          blend.Positions = new float[] { 0.0F, .75F, 1.0F };
          brush.Blend = blend;
        
          // fill in the button
          gfx.FillPolygon( brush, border );
        }            
        
        // is the mouse over the button?
        if( m_mouseOver )
        {
          using( SolidBrush brush = new SolidBrush( Color.FromArgb( 80, m_hotColor ) ) )
            gfx.FillPolygon( brush, border );
        }

        // draw the outline of the button
        //using( Pen pen = new Pen( m_mouseOver? m_hotColor : Color.FromArgb( 126, 143, 156 ), 1.5F ) )
        using( Pen pen = new Pen( m_mouseOver? m_hotColor : Color.Black, 1.5F ) )
          gfx.DrawPolygon( pen, border );
      } // !m_mouseDown
      
      // does the control have any associated text?
      if( this.Text != string.Empty )
      {
        RectangleF textBounds = new RectangleF( 0.0F, 0.0F, this.Width - 1, this.Height - 1 );

        // center the text vertically and horizontally within the button's bounds
        StringFormat format = new StringFormat();
        format.Alignment = StringAlignment.Center;
        format.LineAlignment = StringAlignment.Center;

        // draw the text
        using( SolidBrush brush = new SolidBrush( this.Enabled? this.ForeColor : SystemColors.GrayText ) )
          gfx.DrawString( this.Text, this.Font, brush, textBounds, format );
        
      } // this.Text != string.Empty
    
      if ( this.Image != null || ( this.ImageList != null && this.ImageIndex >= 0 ) )
      {

        int offset = 2;
        int x = 0;
        int y = 0;


        switch ( this.ImageAlign )
        {
          case ContentAlignment.MiddleCenter:
            x = (bounds.Width / 2) - (this.Image.Width / 2) + offset;
            y = (bounds.Height / 2) - (this.Image.Height / 2) + offset;
            break;
          case ContentAlignment.MiddleLeft:
            x = offset * 2;
            y = (bounds.Height / 2) - (this.Image.Height / 2) + offset;
            break;
          case ContentAlignment.MiddleRight:
            x = bounds.Width - ( this.Image.Width );
            y = (bounds.Height / 2) - (this.Image.Height / 2) + offset;
            break;
          case ContentAlignment.TopCenter:
            x = (bounds.Width / 2) - (this.Image.Width / 2) + offset;
            y = offset;
            break;
          case ContentAlignment.TopLeft:
            x = offset * 2;
            y = offset;
            break;
          case ContentAlignment.TopRight:
            x = bounds.Width - ( this.Image.Width );
            y = offset;
            break;
          case ContentAlignment.BottomCenter:
            x = (bounds.Width / 2) - (this.Image.Width / 2) + offset;
            y = bounds.Top + bounds.Height - this.Image.Height;
            break;
          case ContentAlignment.BottomLeft:
            x = offset * 2;
            y = bounds.Top + bounds.Height - this.Image.Height;
            break;
          case ContentAlignment.BottomRight:
            x = bounds.Width - ( this.Image.Width );
            y = bounds.Top + bounds.Height - this.Image.Height;
            break;
        }

        gfx.DrawImageUnscaled(this.Image, x, y);

      }

    } // DrawStockButton( gfx )


    #endregion
    
    #region override methods
    
    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseDown( System.Windows.Forms.MouseEventArgs args )
    {
      // indicate that the one of the mouse buttons are down
      m_mouseDown = true;    
  
      if( m_toggleButton )
        this.Checked = !m_checked;
      
      base.OnMouseDown( args );
    }  // OnMouseDown( args ) 
    
    
    /// <summary>
    /// Called when a mouse button is released.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseUp( System.Windows.Forms.MouseEventArgs args )
    {
      // indicate that the mouse button is has been released
      m_mouseDown = false;
      
      base.OnMouseUp( args );
    } // OnMouseUp( args )
   
    

    /// <summary>
    /// Called when the mouse button enters this control.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseEnter( System.EventArgs args )
    {
      base.OnMouseEnter( args );
      
      // indicate that the mouse is over the button
      m_mouseOver = true;
      
      // need to redraw
      Invalidate();
    } // OnMouseEnter( args )

    

    /// <summary>
    /// Called when the mouse leaves this control.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to this event.</param>
    protected override void OnMouseLeave( System.EventArgs args )
    {
      base.OnMouseLeave( args );
      
      // indicate that the mouse is no longer over the button
      m_mouseOver = false;
      
      // need to redraw
      Invalidate();
    } // OnMouseLeave( args )

    

    protected override void OnKeyDown( KeyEventArgs args )
    {
      if( args.KeyCode == Keys.Space || args.KeyCode == Keys.Enter )
      {
        m_mouseDown = true;    

        if( m_toggleButton )
          this.Checked = !m_checked;

        Invalidate();
      }

      base.OnKeyDown( args );
    }


    protected override void OnKeyUp( KeyEventArgs args )
    {
      if( args.KeyCode == Keys.Space || args.KeyCode == Keys.Enter )
      {
        m_mouseDown = false;
        Invalidate();
      }

      base.OnKeyUp( args );
    }
    
    
    protected override void OnGotFocus( EventArgs args )
    {
      base.OnGotFocus( args );
      Invalidate();
    }


    protected override void OnLostFocus( EventArgs args )
    {
      base.OnLostFocus( args );
      Invalidate();
    }


    /// <summary>
    /// Called when the visibility for the control changeds
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected override void OnVisibleChanged( EventArgs args )
    {
      base.OnVisibleChanged( args );

      m_mouseDown = false;
      m_mouseOver = false;
    } // OnVisibleChanged( args )


    /// <summary>
    /// Handles the check changed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this event.</param>
    protected virtual void OnCheckedChanged( EventArgs args )
    {
      if( this.CheckedChanged != null )
        CheckedChanged( this, args );
    } // OnCheckedChanged( args )

    #endregion

  } // class Button
} // namespace Viatronix.UI.Control

#region revision history

// $Log: Button.cs,v $
// Revision 1.9  2007/10/01 14:51:43  gdavidson
// Added toggle capability
//
// Revision 1.8  2006/06/30 14:36:34  gdavidson
// Issue #4835: Reset the button state when visibility changes
//
// Revision 1.7  2004/11/19 16:11:39  gdavidson
// Modified focus rectangle.
//
// Revision 1.6  2004/11/19 15:50:59  gdavidson
// Added focus rectangle and borders.
//
// Revision 1.5  2004/11/16 19:29:39  mkontak
// Fixed transparent background problem
//
// Revision 1.4  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.3  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.2  2004/07/15 11:36:52  mkontak
// Added ability to set the image on the button
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.18  2004/03/02 01:10:41  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.17.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.17  2003/08/20 14:22:59  michael
// added "mailto:"
//
// Revision 1.16  2003/05/25 18:12:06  michael
// .net --> .com
//
// Revision 1.15  2003/04/28 01:25:24  geconomos
// A little more coding standards.
//
// Revision 1.14  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.13  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.12  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.11  2003/04/22 15:22:21  geconomos
// More coding standards.
//
// Revision 1.10  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.9  2003/04/15 17:22:52  geconomos
// Added code to draw a skinned button disabled.
//
// Revision 1.8  2003/04/14 15:06:02  geconomos
// More code cleanup.
//
// Revision 1.7  2003/04/10 13:28:25  geconomos
// Defaulted ForeColor to black.
//
// Revision 1.6  2003/01/15 17:11:48  geconomos
// Fixed bug with drawing of button after button has been released.
//
// Revision 1.5  2003/01/03 12:54:49  geconomos
// Coding conventions.
//
// Revision 1.4  2002/12/12 15:38:41  geconomos
// Rewrite of class.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Button.cs,v 1.9 2007/10/01 14:51:43 gdavidson Exp $
// $Id: Button.cs,v 1.9 2007/10/01 14:51:43 gdavidson Exp $

#endregion

