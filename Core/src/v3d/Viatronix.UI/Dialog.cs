// $Id: Dialog.cs,v 1.6 2005/10/05 13:05:36 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Drawing;
using System.Drawing.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using Viatronix.UI;

namespace Viatronix.UI
{
  /// <summary>
  /// Represents a window or dialog box that makes up an application's user interface.
  /// </summary>
  public class Dialog : System.Windows.Forms.Form
  {

    #region constants
    
    /// <summary>
    /// Dialog's caption height.
    /// </summary>
    private static readonly int m_captionHeight = Theme.MenuMiddleImage.Height;
    
    /// <summary>
    /// Dialog's frame width.
    /// </summary>
    private static readonly int m_frameWidth = 10;
    
    /// <summary>
    /// Dialog's frame height.
    /// </summary>
    private static readonly int m_frameHeight = 10;

    /// <summary>
    /// Bevel value for corners.
    /// </summary>
    private static readonly int m_bevel = 5;

    /// <summary>
    /// Caption's font object.
    /// </summary>
    private static readonly Font m_captionFont = new Font( "Arial", 10, FontStyle.Bold );

    /// <summary>
    /// Scale factor used to resize logo image.
    /// </summary>
    private static readonly float m_logoScaleFactor = .70F;

    #endregion

    #region fields
    
    /// <summary>
    /// Last mouse position in screen coordinates.
    /// </summary>
    private Point m_lastMouse = new Point();    
    
    /// <summary>
    /// Indicates if the user is repositioning the dialog.
    /// </summary>
    private bool m_moving = false;    
      
    /// <summary>
    /// Logo image.
    /// </summary>
    private static Bitmap m_logoImage = null;
    
    /// <summary>
    /// system menu
    /// </summary>
    private SystemMenu m_sysMenu = new SystemMenu();

    #endregion

    #region properties

    /// <summary>
    /// Gets the caption height.
    /// </summary>
    static public int CaptionHeight
    {
      get { return m_captionHeight; }
    } // CaptionHeight


    /// <summary>
    /// Gets the frame width.
    /// </summary>
    static public int FrameWidth
    {
      get { return m_frameWidth; }
    } // FrameWidth


    /// <summary>
    /// Gets the frame height height.
    /// </summary>
    static public int FrameHeight
    {
      get { return m_frameHeight; }
    } // FrameHeight
    
    
    /// <summary>
    /// Gets the client offset.
    /// </summary>
    public Point Offset
    {
      get{ return new Point( m_frameWidth, m_captionHeight ); }
    } // Offset
    
    #endregion

    #region methods
   
    /// <summary>
    /// Static initialization for Dialog class.
    /// </summary>
    static Dialog()
    {
      // load the default images from the assembly
      Assembly assembly = Assembly.GetExecutingAssembly();
      
      m_logoImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.logo_icon.png") );
    } // Dialog()
    
    
    /// <summary>
    /// Initializes the Dialog object with default values.
    /// </summary>
    public Dialog()
    {
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      
      this.FormBorderStyle = FormBorderStyle.FixedDialog;

      this.BackColor = Theme.PaneBackColor;
      this.Font = Theme.FormFont;

      this.Controls.Add( m_sysMenu ); 
      m_sysMenu.BackColor = Color.Transparent;
      m_sysMenu.ParentForm = this;
      m_sysMenu.ShowMaximize = false;
      m_sysMenu.ShowMinimize = false;
    } // Dialog()

    
    /// <summary>
    /// Called before the dialog is displayed.
    /// </summary>
    /// <param name="args">EventArgs containing data specific to this event.</param>
    protected override void OnLoad( EventArgs args )
    {
      base.OnLoad( args );

      if( this.DesignMode == false )
      {      
        this.FormBorderStyle = FormBorderStyle.None;

        // move each child into place
        foreach( Control control in this.Controls )
          control.Location = new Point( control.Location.X + this.Offset.X, control.Location.Y + this.Offset.Y );

        SetSize( this.Width, this.Height );
      } // if( this.DesignMode == false )
      
    } // OnLoad( args )


    /// <summary>
    /// Sets the size of the dialog
    /// </summary>
    /// <param name="width">Width in pixels of dialog.</param>
    /// <param name="height">Height in pixels of the dialog.</param>
    public void SetSize( int width, int height )
    {
      if( this.DesignMode == false )
      {
        // calculate how big we need to make the form to draw our titlebar and frame
        height +=  m_captionHeight + m_frameHeight;
        width += m_frameWidth + m_frameWidth;

        // create the region for this form
        Rectangle bounds = new Rectangle( 0, 0 , width, height );
        CreateRegion( bounds );
      		  
        // set the size of this form
        this.SetBounds( this.Bounds.X, this.Bounds.Y, width, height );

        m_sysMenu.SetBounds( width - 103, 0, 100, m_captionHeight );
		  
        CreateSkin();
      } // if( this.DesignMode == false )
    } // SetSize( width, height )
    
    #endregion

    #region skin stuff

    /// <summary>
    /// Create the window's region based.
    /// </summary>
    /// <param name="bounds">Window's bounds to create region for.</param>
    private void CreateRegion( Rectangle bounds )
    {
      // create a grahics path object to create region from
      GraphicsPath path = new GraphicsPath();
      
      // add the outer shape to the path
      path.AddPolygon( Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.TopRight, m_bevel ) );

      // create the region
      this.Region = new Region( path );
    } // CreateRegion( bounds )

    
    /// <summary>
    /// Creates the background skin for the dialog.
    /// </summary>
    private void CreateSkin()
    {
      // create a bitmap to render the skin into
      Bitmap bitmap = new Bitmap( this.Width - 1, this.Height - 1, PixelFormat.Format24bppRgb );
      
      // create a graphics object based on the bitmap
      using( Graphics gfx = Graphics.FromImage( bitmap ) )
      {
      
        // use highest quailty settings
        gfx.SmoothingMode = SmoothingMode.HighQuality;
        gfx.InterpolationMode = InterpolationMode.HighQualityBicubic;
        gfx.TextRenderingHint = TextRenderingHint.AntiAliasGridFit;

        gfx.Clear( Theme.PaneBackColor );

        // create a bounding rectangle for creating the shapes
        Rectangle bounds = new Rectangle( 0, 0, this.Width - 1, this.Height - 1 );

        // brushes
        TextureBrush sideBrush = Theme.FormSideBrush;
        TextureBrush topBrush = Theme.FormTopBrush;
        TextureBrush bottomBrush = Theme.FormBottomBrush;

        // left
        gfx.FillRectangle( sideBrush, 0, topBrush.Image.Height, sideBrush.Image.Width, this.Height );
        
        // right
        sideBrush.TranslateTransform( ( this.Width - sideBrush.Image.Width - 1) % sideBrush.Image.Width, 0 );
        gfx.FillRectangle( sideBrush, this.Width - sideBrush.Image.Width - 1, topBrush.Image.Height, sideBrush.Image.Width, this.Height );
        sideBrush.ResetTransform();

        // top
        gfx.FillRectangle( topBrush, 0, 0, this.Width, topBrush.Image.Height );
        gfx.DrawImage( Theme.FormCornerTopLeftImage, 0, 0 );
        gfx.DrawImage( Theme.FormCornerTopRightImage, this.Width - Theme.FormCornerTopRightImage.Width - 1, 0 );

        // bottom
        bottomBrush.TranslateTransform( 0, ( this.Height - bottomBrush.Image.Height - 1) % bottomBrush.Image.Height, 0 );
        gfx.FillRectangle( bottomBrush, 0, this.Height - bottomBrush.Image.Height - 1, this.Width, bottomBrush.Image.Height );
        bottomBrush.ResetTransform();

        gfx.DrawImage( Theme.FormCornerBottomLeftImage, 0, this.Height - Theme.FormCornerBottomLeftImage.Height-1 );
        gfx.DrawImage( Theme.FormCornerBottomRightImage, this.Width - Theme.FormCornerBottomRightImage.Width - 1, this.Height - Theme.FormCornerBottomRightImage.Height-1 );

        DrawCaptionAndLogo( gfx );
      
      } // using( Graphics gfx = Graphics.FromImage( bitmap ) )

      // set the form background image to the skin image
      this.BackgroundImage = bitmap;
    
    } // CreateSkin()

     
    /// <summary>
    /// Draws the logo image and the caption.
    /// </summary>
    /// <param name="gfx">Graphics object used for drawing.</param>
    private void DrawCaptionAndLogo( Graphics gfx )
    {
      // scale down the size of the logo image
      Rectangle logoBounds = new Rectangle( 0, 0, ( int )( m_logoImage.Width * m_logoScaleFactor ), ( int )( m_logoImage.Height * m_logoScaleFactor ) );
      
      // draw the logo image
      gfx.DrawImage( m_logoImage, m_bevel + 4, ( int ) ( m_captionHeight - logoBounds.Height ) / 2.0F, logoBounds.Width, logoBounds.Height );

      // draw the caption for the dialog      
      using( SolidBrush brush = new SolidBrush( Color.White ) )
        gfx.DrawString( this.Text, m_captionFont, brush, ( float ) m_bevel + 5.0F + logoBounds.Width, ( m_captionHeight + m_frameHeight - m_captionFont.Height ) * 0.3f  );
    
    } // DrawLogo( gfx )
    
    #endregion

    #region mouse handlers

    /// <summary>
    /// Called when a mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event.</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      base.OnMouseDown( args );
      
      // is the left buton down
      if( args.Button == MouseButtons.Left )
      {
        // did we click in the caption area?
        if( args.Y < m_captionHeight )
        {
          m_moving = true;
          m_lastMouse = Control.MousePosition;
        } // if( args.Y < m_captionHeight )
      
      } // else if( args.Button == MouseButtons.Left )
    } // OnMouseDown( args )

    
    /// <summary>
    /// Called when a mouse button is released.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event.</param>
    protected override void OnMouseUp( MouseEventArgs args )
    {
      // we are not moving anymore
      m_moving = false;

    } // OnMouseUp( args )
    
    
    /// <summary>
    /// Called when the mouse moves.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data specific to this event.</param>
    protected override void OnMouseMove( MouseEventArgs args )
    {
      // are we repositioning the dialog?
      if( m_moving )
      {
        // get the current location
        Point location =  this.Location;

        // offset the current location 
        location.Offset( Control.MousePosition.X - m_lastMouse.X, Control.MousePosition.Y - m_lastMouse.Y );
        
        // update the dialog's location
        this.Location = location;
        
        // store the last mouse position
        m_lastMouse = Control.MousePosition;
      } // if( m_moving )
      
    } // OnMouseMove( MouseEventArgs args )
    
    #endregion
  
  } // class Dialog
} // namespace Viatronix.UI

#region revision history

// $Log: Dialog.cs,v $
// Revision 1.6  2005/10/05 13:05:36  gdavidson
// Added font property for the form.
//
// Revision 1.5  2005/05/24 12:43:07  romy
// Adjusted  the Caption Location
//
// Revision 1.4  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.2.1  2005/02/28 21:11:04  frank
// code review
//
// Revision 1.3  2005/01/07 22:20:07  gdavidson
// Changed the backgound color.
//
// Revision 1.2  2004/11/19 22:30:27  gdavidson
// Added new skin.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.16.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.16  2003/08/20 14:22:59  michael
// added "mailto:"
//
// Revision 1.15  2003/07/02 15:05:18  geconomos
// Called down to base class on OnLoad( ... ).
//
// Revision 1.14  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.13  2003/05/05 13:58:37  frank
// code review
//
// Revision 1.12  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.11  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.10  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.9  2003/04/22 15:22:22  geconomos
// More coding standards.
//
// Revision 1.8  2003/04/17 19:41:19  geconomos
// Some more code cleanup.
//
// Revision 1.7  2003/04/14 13:08:15  geconomos
// Code cleanup.
//
// Revision 1.6  2003/03/14 14:22:44  geconomos
// Positioning of appication icon.
//
// Revision 1.5  2003/03/10 16:13:14  geconomos
// Initial version application icon. Needs some work.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Dialog.cs,v 1.6 2005/10/05 13:05:36 gdavidson Exp $
// $Id: Dialog.cs,v 1.6 2005/10/05 13:05:36 gdavidson Exp $

#endregion
