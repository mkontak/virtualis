// $Id: RadioButton.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: george economos (george@viatronix.net)

using System;
using System.Reflection;
using System.Drawing;
using System.Windows.Forms;

namespace Viatronix.UI
{
	/// <summary>
	/// Represents a Windows radio button control.
	/// </summary>
  public class RadioButton : System.Windows.Forms.RadioButton
	{    
    #region fields
    
    /// <summary>
    /// Unchecked state image.
    /// </summary>
    private readonly static Bitmap m_uncheckedImage;
    
    /// <summary>
    /// Checked state image.
    /// </summary>
    private readonly static Bitmap m_checkedImage;
    
	  #endregion 		
     
    #region initialization
    
		/// <summary>
		/// Initializes the state images.
		/// </summary>
    static RadioButton()
		{
			// load the images from the assembly
      m_uncheckedImage = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.radiobutton_off.png"));
			m_checkedImage = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.radiobutton_on.png"));
		} // RadioButton()

    #endregion
    
    #region drawing

    /// <summary>
    /// Event handler for the Paint event.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // fill in the background
      Graphics gfx = args.Graphics;
      
      Utilities.DrawBackground( gfx, this );
      
      // create the checkbox image to draw
      Bitmap image = this.Checked? m_checkedImage : m_uncheckedImage;
      
      // draw the checkbox
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

    } // OnPaint()

    #endregion
 
	} // class RadioButton
} // namespace Viatronix.UI

#region revision history

// $Log: RadioButton.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.11  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.10  2003/04/22 15:27:55  geconomos
// More coding standards.
//
// Revision 1.9  2003/04/11 16:23:07  geconomos
// Code cleanup.
//
// Revision 1.8  2002/12/12 15:38:41  geconomos
// Rewrite of class.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/RadioButton.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: RadioButton.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion