// $Id: UserLabel.cs,v 1.2 2007/02/26 17:59:51 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Drawing;
using System.Windows.Forms;
using Viatronix.UI;

namespace Viatronix.Console
{
	/// <summary>
	/// Draws the logged in users name over a toolbar background.
	/// </summary>
	public class UserLabel : Control
	{

    #region construction

    /// <summary>
    /// Creates a new instance of the UserLabel control
    /// </summary>
		public UserLabel()
		{
      // enable double buffering
      SetStyle( System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( System.Windows.Forms.ControlStyles.DoubleBuffer, true );
      SetStyle( System.Windows.Forms.ControlStyles.UserPaint, true );
      
      // support transparent back colors
      SetStyle( System.Windows.Forms.ControlStyles.SupportsTransparentBackColor, true );
      
      this.TabStop = false;

      // set the height of the controller to the height of the middle toolbar theme image
      this.Height = Theme.ToolbarMiddleBrush.Image.Height;


      if( Global.LoggedOn )
      {
        using( Graphics gfx = CreateGraphics() )
          this.Width = (int) gfx.MeasureString( Global.CurrentUser.Login, Theme.FormFont ).Width + 20;
      }
		} // UserLabel()

    #endregion

    #region override methods

    /// <summary>
    /// Called when the control needs to paint itself
    /// </summary>
    /// <param name="args">PaintEventArgs containging data related to this event</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;

      Rectangle bounds = new Rectangle( 0, 0, this.Width, this.Height );

      // first draw the middle image using the texture brush
      gfx.FillRectangle( Theme.ToolbarMiddleBrush, bounds );

      StringFormat format = new StringFormat();
      format.LineAlignment = StringAlignment.Center;
      format.Alignment = StringAlignment.Far;

      // paint the user text
      gfx.DrawString( Global.CurrentUser.Login, Theme.FormFont, Brushes.Black, bounds, format );
    } // OnPaint( args )

    #endregion

	} // class UserLabel
} // namespace Viatronix.Console


#region revision history

// $Log: UserLabel.cs,v $
// Revision 1.2  2007/02/26 17:59:51  gdavidson
// Set the TabStop to false
//
// Revision 1.1  2006/10/30 15:43:19  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.3  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.2  2005/11/17 23:11:10  gdavidson
// Added a check for a null user
//
// Revision 1.1  2005/11/02 14:17:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ui/UserLabel.cs,v 1.2 2007/02/26 17:59:51 gdavidson Exp $
// $Id: UserLabel.cs,v 1.2 2007/02/26 17:59:51 gdavidson Exp $

#endregion