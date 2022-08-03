// $Id: ControllerToolBars.cs,v 1.6 2007/03/01 17:09:18 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Windows.Forms;
using Viatronix.UI;
using System.Drawing;

namespace Viatronix.UI
{
  /// <summary>
  /// Responsible for maintaining the frame's toolbars
  /// </summary>
  public class ControllerToolBars : Control
	{

    #region methods

    /// <summary>
    /// Initilizes a ControllerToolBars instance with default values.
    /// </summary>
    public ControllerToolBars()
    {
      // enable double buffering
      SetStyle( System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( System.Windows.Forms.ControlStyles.DoubleBuffer, true );
      SetStyle( System.Windows.Forms.ControlStyles.UserPaint, true );
      
      // support transparent back colors
      SetStyle( System.Windows.Forms.ControlStyles.SupportsTransparentBackColor, true );

      // set the height of the controller to the height of the middle toolbar theme image
      this.Height = Theme.ToolbarMiddleBrush.Image.Height;
    } // ControllerToolBars()


    /// <summary>
    /// Adds the specified toolbar to the controller.
    /// </summary>
    /// <param name="toolbar">ToolBar instance to add.</param>
    public void AddToolBar( Viatronix.UI.Toolbar toolbar )
    {
      AddControl( toolbar, HorizontalAlignment.Left );
    } // AddToolBar( Viatronix.UI.Toolbar toolbar )


    /// <summary>
    /// Adds the specified toolbar to the controller.
    /// </summary>
    /// <param name="toolbar">ToolBar instance to add.</param>
    /// <param name="align">Horizontal alignment</param>
    public void AddToolBar( Viatronix.UI.Toolbar toolbar, HorizontalAlignment align )
    {
      AddControl( toolbar, align );
    } // AddToolBar( Viatronix.UI.Toolbar toolbar )


    /// <summary>
    /// Adds the specified control to the controller.
    /// </summary>
    /// <param name="newControl">Control instance to add.</param>
    /// <param name="align">Horizontal alignment</param>
    public void AddControl( Control newControl, HorizontalAlignment align )
    {
      // center the toolbar vertically
      int yPos = ( this.Height - newControl.Height ) / 2;

      // add a little padding to the initial position
      int xPos = ( align == HorizontalAlignment.Left ) ? 2 : this.Width - 5 - newControl.Width;
      
      // do we any toolbars already?
      if( this.Controls.Count > 0 )
      {
        for( int i = this.Controls.Count - 1; i >= 0; --i )
        {
          // get the last toolbar added
          Control lastControl = this.Controls[ i ];
          if( align == HorizontalAlignment.Left && (( lastControl.Anchor & AnchorStyles.Left ) == AnchorStyles.Left ))
          {
            xPos = lastControl.Right + 1;
            break;
          }
          else if( align == HorizontalAlignment.Right && (( lastControl.Anchor & AnchorStyles.Right ) == AnchorStyles.Right ))
          {
            xPos = lastControl.Left - newControl.Width - 2;
            break;
          }
        }
      } // if( this.Controls.Count > 0 )

      // set the position of the control
      newControl.Location = new Point( xPos, yPos );
      newControl.Anchor = AnchorStyles.Top | (( align == HorizontalAlignment.Left ) ? AnchorStyles.Left : AnchorStyles.Right );
    
      // add the toolbar to the controller
      this.Controls.Add( newControl );
    } // AddControl( newControl )


    /// <summary>
    /// Clears out the plugin toolbars.
    /// </summary>
    public void Clear()
    {
      this.Controls.Clear();
    } // Clear()

    
    /// <summary>
    /// Called when the control needs to paint itself
    /// </summary>
    /// <param name="args">PaintEventArgs containging data related to this event</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

      Graphics gfx = args.Graphics;

      // first draw the middle image using the texture brush
      gfx.FillRectangle( Theme.ToolbarMiddleBrush, 0, 0, this.Width, this.Height );

      // draw left and right ends
      gfx.DrawImage( Theme.ToolbarLeftImage, 0, 0 );
      gfx.DrawImage( Theme.ToolbarRightImage, this.Width - Theme.ToolbarRightImage.Width, 0 );
    } // OnPaint( PaintEventArgs args )


    /// <summary>
    /// Specifies the bounds of the control. Overriden to fix the height.
    /// </summary>
    /// <param name="x">x position</param>
    /// <param name="y">y position</param>
    /// <param name="width">width of control</param>
    /// <param name="height">height of control</param>
    /// <param name="specified">specified bounds</param>
    protected override void SetBoundsCore( int x, int y, int width, int height, BoundsSpecified specified )
    {
      // fix the height of the control to the hieght of the theme's middle image
      base.SetBoundsCore( x, y, width, Theme.ToolbarMiddleBrush.Image.Height, specified );
    } // SetBoundsCore( int x, int y, int width, int height, BoundsSpecified specified )


    /// <summary>
    /// Called when the controller's size has changed
    /// </summary>
    /// <param name="args">EventArgs containing dta related to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );
      
      // dirty the window for painting
      Invalidate();    
    } // OnResize( EventArgs args )

    
    #endregion

  } // class ControllerToolBars
} // namespace Viatronix.Viewer


#region revision history

// $Log: ControllerToolBars.cs,v $
// Revision 1.6  2007/03/01 17:09:18  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/ControllerToolBars.cs,v 1.6 2007/03/01 17:09:18 geconomos Exp $
// $Id: ControllerToolBars.cs,v 1.6 2007/03/01 17:09:18 geconomos Exp $

#endregion