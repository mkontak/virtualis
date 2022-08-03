// $Id: PictureBox.cs,v 1.3 2007/06/21 13:45:22 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Viatronix.UI
{
	public class PictureBox : System.Windows.Forms.Control
	{

    #region fields

    /// <summary>
    /// an image
    /// </summary>
    private Image m_image = null;

    /// <summary>
    /// determines if the control should maintain the aspect ratio
    /// </summary>
    private bool m_maintainAspectRatio = true;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
		public PictureBox()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
		} // PictureBox()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the Image
    /// </summary>
    public Image Image
    {
      get { return m_image; }
      set 
      {
        m_image = value; 
        Invalidate();
      }
    } // Image


    /// <summary>
    /// Gets or sets if the apect ratio is maintained
    /// </summary>
    public bool MaintainAspectRatio
    {
      get { return m_maintainAspectRatio; }
      set 
      {
        m_maintainAspectRatio = value; 
        Invalidate();
      }
    } // MaintainAspectRatio

    #endregion

    #region override methods

    /// <summary>
    /// Resizes the image.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Draws the image.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      Graphics gfx = args.Graphics;
      
      gfx.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
      gfx.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;

      gfx.Clear( this.BackColor );

      if( this.Image != null )
      {
        Rectangle bounds = new Rectangle( 0, 0, this.Width, this.Height );
        if( this.MaintainAspectRatio )
          Utilities.DrawImage( gfx, m_image, bounds );
        else
          gfx.DrawImage( m_image, bounds );
      }

      base.OnPaint( args );
    } // OnPaint( args )


    #endregion

	} // class PictureBox
} // namespace Viatronix.UI


#region revision history

// $Log: PictureBox.cs,v $
// Revision 1.3  2007/06/21 13:45:22  gdavidson
// Adjust the bounds of the drawn image
//
// Revision 1.2  2007/02/28 19:57:04  gdavidson
// Modified the OnPaint method
//
// Revision 1.1  2006/03/02 17:11:32  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/PictureBox.cs,v 1.3 2007/06/21 13:45:22 gdavidson Exp $
// $Id: PictureBox.cs,v 1.3 2007/06/21 13:45:22 gdavidson Exp $

#endregion