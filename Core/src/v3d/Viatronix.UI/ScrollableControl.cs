// $Id: ScrollableControl.cs,v 1.5 2005/06/22 15:08:36 gdavidson Exp $
//
// Copyright(c) 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Text;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;

using Viatronix.UI;

namespace Viatronix.UI
{	
	/// <summary>
  /// Defines a base class for controls that support scrolling behavior.
	/// </summary>
  public class ScrollableControl : System.Windows.Forms.UserControl
	{
    #region events

    /// <summary>
    /// Raised when the view rectangle changes.
    /// </summary>
    public new event EventHandler Scroll;

    #endregion
    
    #region fields
    
    /// <summary>
    /// System width of vertical scrollbar.
    /// </summary> 
    private static readonly int m_scrollBarWidth = 0;

    /// <summary>
    /// Bevel for the corners. 
    /// </summary>
    private int m_bevel = 10;

    /// <summary>
    /// Fill color for the control.
    /// </summary>
    private Color m_fillColor = Theme.FillColor;

    /// <summary>
    /// Vertical scrollbar.
    /// </summary>
    private Viatronix.UI.VScrollBar m_vScrollBar = new Viatronix.UI.VScrollBar();

    /// <summary>
    /// Current view rectangle.
    /// </summary>
    private Rectangle m_viewRectangle = new Rectangle();

    /// <summary>
    /// Virtual size.
    /// </summary>
    private Size m_virtualSize = new Size();

    #endregion

    #region properties
  
    /// <summary>
    /// Gets or sets the bevel.
    /// </summary>
    public int Bevel
    {
      get { return m_bevel; }
      set { m_bevel = value; Invalidate(); }
    } // Bevel


    /// <summary>
    /// Gets or sets the fill color.
    /// </summary>
    public Color FillColor
    {
      get { return m_fillColor; }
      set { m_fillColor = value; Invalidate(); }
    } // FillColor

    
    /// <summary>
    /// Gets or sets the view rectangle.
    /// </summary>
    public Rectangle ViewRectangle
    {
      get { return m_viewRectangle; }
      set { m_viewRectangle = value; }
    } // ViewRectangle


    /// <summary>
    /// Gets the width of the vertical scrollbar in pixels.
    /// </summary>
    public int VScrollWidth
    {
      get { return m_scrollBarWidth; }
    } // VScrollWidth
    

    /// <summary>
    /// Returns a reference to the VScrollBar
    /// </summary>
    protected Viatronix.UI.VScrollBar ScrollBar
    {
      get { return m_vScrollBar; }
    } // ScrollBar


    /// <summary>
    /// Returns the virtual size for the scrolable area.
    /// </summary>
    protected Size VirtualSize
    {
      get { return m_virtualSize; }
    } // VirtualSize
    
    #endregion

    #region initialization

    /// <summary>
    /// Static initialization.
    /// </summary>
    static ScrollableControl()
    {
      // ask the system for the width of a vertical scrollbar
      m_scrollBarWidth = Win32.User32.GetSystemMetrics( ( int ) Win32.SystemMetricsFlags.SM_CXVSCROLL );
    } // ScrollableControl()
		
    
    /// <summary>
    /// Initializaes a ScrollableControl object with default values.
    /// </summary>
    public ScrollableControl()
		{
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      // wire up the event handling for the vertical scrollbar
      m_vScrollBar.ValueChanged += new EventHandler( OnVScrollChangedHandler );

      // add the scrollbar to this control
      this.Controls.Add( m_vScrollBar );
		} // ScrollableControl()

    
    #endregion

    #region methods
    
    /// <summary>
    /// Sets the virtual size for the control.
    /// </summary>
    /// <param name="size">Virtual size of control in pixels.</param>
    public void SetVirtualSize( Size size )
    {
      SetVirtualSize( size.Width, size.Height );
    } // SetVirtualSize( size )
    
    
    /// <summary>
    /// Sets the virtual size for the control.
    /// </summary>
    /// <param name="width">Virtual width in pixels</param>
    /// <param name="height">Virtual height in pixels</param>
    public void SetVirtualSize( int width, int height )
    {
      // clamp width and height
      m_virtualSize.Width = Math.Max( width, this.Width );
      m_virtualSize.Height = Math.Max( height, this.Height );

      // update the vertical scrollbar
      m_vScrollBar.Minimum = 0;
      m_vScrollBar.Maximum = m_virtualSize.Height;
      m_vScrollBar.LargeChange = this.Height;
      m_vScrollBar.SmallChange = ( int ) ( this.Height * 0.10F );
    } // SetVirtualSize( width, height )


    /// <summary>
    /// Scrolls the view rectangle.
    /// </summary>
    /// <param name="dx">Amount of pixels to scroll in x.</param>
    /// <param name="dy">Amount of pixels to scroll in y.</param>
    public void ScrollControl( int dx, int dy )
    {
      // convert the vertival scroll amount to a scrollbar value
      int vscrollValue = m_vScrollBar.Value + dy;

      // clamp and set the verical scrollbar value
      m_vScrollBar.Value = Math.Min( Math.Max( m_vScrollBar.Minimum, vscrollValue ), m_vScrollBar.Maximum - this.Height );
    } // ScrollControl( dx, dy )


    public void ResetScrollBar()
    {
      m_vScrollBar.Value = m_vScrollBar.Minimum;
    }
    
    
    #endregion

    #region virtuals

    /// <summary>
    /// Called when the view rectangle changes.
    /// </summary>
    /// <param name="args">EventArgs containing data releated to this event.</param>
    protected virtual void OnScroll( EventArgs args )
    {
      // raise the scroll event
      if( Scroll != null )
        Scroll( this, args );
    } // OnScroll( args )

    
    #endregion

    #region layout

    /// <summary>
    /// Called when the control needs to resize itself.
    /// </summary>
    /// <param name="args">EventArgs containing data releated to this event.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );      

      // position the scrollbar
      m_vScrollBar.SetBounds( this.Width - m_scrollBarWidth, 0, m_scrollBarWidth, this.Height );      

      // default the virtual size and view rect to the client area
      m_viewRectangle.Size = new Size( this.Width - m_scrollBarWidth, this.Height );
      
      SetVirtualSize( m_viewRectangle.Size );
      
      Invalidate();
    } // OnResize( args )

    
    #endregion

    #region overrides
    
    /// <summary>
    /// Called when the mouse wheel is rotated.
    /// </summary>
    /// <param name="args">MouseEventArgs containg data related to this event.</param>
    protected override void OnMouseWheel( MouseEventArgs args )
    {
      base.OnMouseWheel( args );

      this.ScrollControl( 0, -( m_vScrollBar.SmallChange * ( args.Delta / 120 ) ) );
    } // OnMouseWheel( MouseEventArgs args )
 
    #endregion

    #region scrollbar event handlers

    /// <summary>
    /// Event handler for the ValueChanged event of the vertical scrollbar.
    /// </summary>
    /// <param name="sender">Sender of this event.</param>
    /// <param name="args">EventArgs containing data related to this event.</param>
    private void OnVScrollChangedHandler( object sender, EventArgs args )
    {
      // offset the view rectangle
      m_viewRectangle.Location = new Point( 0, m_vScrollBar.Value );

      // notify derived classes of change
      OnScroll( EventArgs.Empty );

      Invalidate();
    } // OnVScrollChangedHandler( sender, args )

    
    #endregion

    #region drawing

    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this event.</param>
    protected override void OnPaint( PaintEventArgs args )
    {
      // cache graphics object
      Graphics gfx = args.Graphics;

      // enable high quality rendering
      gfx.SmoothingMode = SmoothingMode.HighQuality;

      // create the bounds for the background shape
      Rectangle bounds = new Rectangle( 0, 0, this.Width - m_scrollBarWidth, this.Height );

      // fill in the background shape with the fill color
      using( SolidBrush brush = new SolidBrush( m_fillColor ) )
        gfx.FillPolygon( brush, Shapes.BeveledEdge( bounds, Edges.TopLeft | Edges.BottomLeft, m_bevel ) );
    } // OnPaint(  args )

    
    #endregion
   
	} // class ScrollableControl
} // namespace Viatronix.UI

#region revision history

// $Log: ScrollableControl.cs,v $
// Revision 1.5  2005/06/22 15:08:36  gdavidson
// Added accessors for the contained VScrollBar and VirtualSize.
//
// Revision 1.4  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.2.1  2005/02/08 19:10:49  gdavidson
// Issue #3905: Added reset for the scoll control.
//
// Revision 1.3  2004/11/17 19:05:34  gdavidson
// Removed the clearing of the parent's BackColor.
//
// Revision 1.2  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.8.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.8  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.7  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.6.2.1  2003/08/17 23:49:37  geconomos
// Updated to handle the mouse wheel event.
//
// Revision 1.6  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.5  2003/04/29 14:28:35  frank
// code review
//
// Revision 1.4  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.3  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.2  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.1  2003/03/24 16:03:27  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/ScrollableControl.cs,v 1.5 2005/06/22 15:08:36 gdavidson Exp $
// $Id: ScrollableControl.cs,v 1.5 2005/06/22 15:08:36 gdavidson Exp $

#endregion
