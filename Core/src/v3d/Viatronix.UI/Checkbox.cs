// $Id: Checkbox.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright (c) 2002, Viatronix Inc., All Rights Reserved.
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
using System.Windows.Forms;
using System.Reflection;
using Viatronix.UI.Desgin;

namespace Viatronix.UI
{
	/// <summary>
	/// Represents a "skinnable" Windows check box
	/// </summary>
  public class CheckBox : System.Windows.Forms.CheckBox
	{ 
    #region fields
    
		/// <summary>
		/// Default "unchecked" image.
		/// </summary>
    static private Bitmap m_uncheckedImage;
    
    /// <summary>
    /// Default "checked" image.
    /// </summary>
    static private Bitmap m_checkedImage;

    /// <summary>
    /// Custom button images.
    /// </summary>
    private Bitmap[] m_skins = new Bitmap[ 3 ];

    #endregion 

    #region properties
    
    /// <summary>
    /// Gets or sets the custom images.
    /// </summary>
    [ Editor( typeof( ButtonImagesEditor ),typeof( UITypeEditor ) ) ]
    public Bitmap[] CustomImages
    {
      get{ return m_skins; }
      set{ m_skins = value; Invalidate(); }
    } // CustomImages

    
    #endregion
		
    #region initialization
		
    /// <summary>
    /// Static constructor loads resources.
    /// </summary>
    static CheckBox()
		{
			// load the default images from the assembly
      Assembly assembly = Assembly.GetExecutingAssembly();
			m_uncheckedImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.checkbox_off.png") );
			m_checkedImage = new Bitmap( assembly.GetManifestResourceStream("Viatronix.UI.res.checkbox_on.png") );
		} // Checkbox()
    
    #endregion

    #region drawing

           
    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      Graphics gfx = args.Graphics;

      // use helper to draw background of control
      Utilities.DrawBackground( gfx, this );

      // NOTE: In order to draw a "skinned" button the up state image must be NON null.
      bool skinButton = m_skins[ 0 ] != null;
      if( skinButton )
        DrawSkinnedButton( args.Graphics );
      else
        DrawStockButton( args.Graphics );
      
    } // OnPaint( args )


    /// <summary>
    /// Draws a skinned checkbox using the custom images.
    /// </summary>
    /// <param name="gfx">Graphics object used for drawing.</param>
    private void DrawSkinnedButton( Graphics gfx )
    {
      // get the button state image
      Image image = this.Checked? ( m_skins[ 1 ] != null? m_skins[ 1 ] : m_skins[ 0 ] ) : m_skins[ 0 ];

      // is the button enabled?
      if( this.Enabled )
         gfx.DrawImageUnscaled( image, 0, 0 );      
      else
         ControlPaint.DrawImageDisabled( gfx, image, 0, 0, this.BackColor );

        
    } // DrawSkinnedButton( gfx )
    

    /// <summary>
    /// Draws a stock button.
    /// </summary>
    /// <param name="gfx">Graphics object used for drawing.</param>
    private void DrawStockButton( Graphics gfx )
    {
      // decide which image to display from the "Checked" property
      Bitmap image = this.Checked? m_checkedImage : m_uncheckedImage;
      
      // draw the checkbox centered vertically with the control
      int y = ( this.Height - image.Height ) / 2;
      
      if( this.Enabled )      
        gfx.DrawImageUnscaled( image, 0, y );
      else
        ControlPaint.DrawImageDisabled( gfx, image, 0, y, this.BackColor );

      // is there any text to draw?
      if( this.Text != String.Empty )
      {
        // calculate the bounding rectangle for the text
        SizeF textSize = gfx.MeasureString( this.Text, this.Font );
        PointF textLocation = new PointF( ( float ) image.Width + 3.0F, ( this.Height - this.Font.Height ) / 2 );
        RectangleF textBounds = new RectangleF( textLocation, textSize );

        // draw the text
        using( SolidBrush brush = new SolidBrush( this.Enabled? this.ForeColor : SystemColors.GrayText ) )
          gfx.DrawString( this.Text, this.Font, brush, textBounds );

        // should we draw the focus rectangle
        if( this.Focused )
        {
          Rectangle focusBounds = new Rectangle( ( int )textBounds.X, ( int )textBounds.Y, ( int )textBounds.Width, ( int )textBounds.Height );
          focusBounds.Inflate( 1, 1 );
          ControlPaint.DrawFocusRectangle( gfx, focusBounds );
        }
      } // this.Text != String.Empty
    } // DrawStockButton( gfx )
 
    
    #endregion
    
  } // class CheckBox
} // namespace Viatronix.UI

#region revision history

// $Log: Checkbox.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/08/20 14:22:59  michael
// added "mailto:"
//
// Revision 1.16  2003/05/25 18:12:06  michael
// .net --> .com
//
// Revision 1.15  2003/05/06 12:32:53  geconomos
// Fixed issue with drawing disabled button state.
//
// Revision 1.14  2003/05/01 17:16:50  frank
// code review in progress
//
// Revision 1.13  2003/04/28 01:25:24  geconomos
// A little more coding standards.
//
// Revision 1.12  2003/04/27 23:51:48  geconomos
// More code cleanup.
//
// Revision 1.11  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.10  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.9  2003/04/14 15:06:02  geconomos
// More code cleanup.
//
// Revision 1.8  2003/02/21 15:32:58  geconomos
// Now allowing for skins.
//
// Revision 1.7  2003/01/09 19:01:45  geconomos
// ??
//
// Revision 1.6  2001/01/09 12:54:49  geconomos
// Coding conventions.
//
// Revision 1.5  2002/12/12 15:38:41  geconomos
// Rewrite of class.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Checkbox.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: Checkbox.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion