// $Id: GroupBox.cs,v 1.5.28.1 2009/01/26 21:20:09 kchalupa Exp $
//
// Copyright(c) 2002, Viatronix Inc., All Rights Reserved.
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
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
  /// <summary>
  /// Represents a Window's group box control.
  /// </summary>
	public class GroupBox : System.Windows.Forms.GroupBox
	{
    #region fields
    
    /// <summary>
    /// Border color.
    /// </summary>
    private Color m_borderColor = Color.FromArgb( 89,116, 153 );
		
    /// <summary>
    /// Tab color.
    /// </summary>
    private Color m_tabColor = Color.FromArgb( 128,143,180 );
		
    /// <summary>
    /// Fill color.
    /// </summary>
    private Color m_fillColor = Color.FromArgb( 240, 240, 240 );

    /// <summary>
    /// Outline color.
    /// </summary>
    private Color m_outlineColor =  Color.FromArgb( 51, 51, 102 );
		
    /// <summary>
    /// Bevel for rounded edges.
    /// </summary>
    private int m_bevel = 6;

    #endregion
		
    #region properties

    /// <summary>
    /// Gets or sets the fill color.
    /// </summary>
    public Color FillColor
		{
		  get { return m_fillColor; }
		  set { m_fillColor = value; Invalidate(); }
		} // FillColor
		
		
    /// <summary>
    /// Gets or sets the border color.
    /// </summary>
    public Color BorderColor
		{
		  get { return m_borderColor; }
		  set { m_borderColor = value; Invalidate(); }
		} // BorderColor
		
    
    /// <summary>
    /// Gets or sets the tab color.
    /// </summary>
    public Color TabColor
    {
      get { return m_tabColor; }
      set { m_tabColor = value; Invalidate(); }
    } // TabColor
		
    
    #endregion
		
    #region initialization
    
    /// <summary>
    /// Initializes a GroupBox object with default values.
    /// </summary>
    public GroupBox()
		{
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
    } // GroupBox()
		

    #endregion
		
    #region drawing
    
    /// <summary>
    /// Called when the control needs to repaint itself.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this override.</param>
    protected override void OnPaint( PaintEventArgs args )
		{
      // bounding rectangle for control
      Rectangle bounds = new Rectangle( 0, 0, this.Width - 1, this.Height - 1 );
		  
      // high quality settings
      Graphics gfx = args.Graphics;		  
		  gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      gfx.Clear( this.Parent.BackColor );

		  // fill the border
      PointF[] border = Shapes.BeveledEdge( bounds, Edges.All, m_bevel );
      using( SolidBrush brush = new SolidBrush( m_borderColor ) )
		    args.Graphics.FillPolygon( brush, border );

      // draw an outline around the border
      using( Pen pen = new Pen( m_outlineColor ) )
        gfx.DrawPolygon( pen, border );

		  // shrink the bounds for the tab points
      bounds.Inflate( -m_bevel, -m_bevel );

      // fill the tab
      PointF[] tab = Shapes.BeveledEdge( bounds, Edges.All, m_bevel );		  
      using( SolidBrush brush = new SolidBrush( m_tabColor ) )
		    args.Graphics.FillPolygon( brush, tab );
		  
      // set up the bounds for the inner shape
      int tabHeight = this.Font.Height +  m_bevel;	
      bounds.Inflate( 0, -( tabHeight / 2 ) );
      bounds.Offset( 0,   ( tabHeight / 2 ) );
	
      // fill the inside
      PointF[] fill = Shapes.BeveledEdge( bounds, Edges.All, m_bevel );		  
      using( SolidBrush brush = new SolidBrush( m_fillColor ) )
		    args.Graphics.FillPolygon( brush, fill );

      // draw an outline around the tab
      using( Pen pen = new Pen( m_outlineColor ) )
        gfx.DrawPolygon( pen, tab );		    

		  // draw the text
		  using( SolidBrush brush = new SolidBrush( this.ForeColor ) )
      {
        args.Graphics.DrawString(this.Text, this.Font, brush, new RectangleF(m_bevel + 5, m_bevel + 3, Width - (m_bevel + 7), tabHeight));  
      }
		} // OnPaint( args )
		

    #endregion
		
    #region overrides
    
    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this override.</param>
    protected override void OnResize( EventArgs args )
		{
      base.OnResize( args );
      Invalidate();
		} // OnResize( args )
		
    
    /// <summary>
    /// Called when the control's Font property is changed.
    /// </summary>
    /// <param name="args">EventArgs containing data related to this override.</param>
    protected override void OnFontChanged( EventArgs args )
    {
      base.OnFontChanged( args );
      Invalidate();
    } // OnFontChanged( args )
		

    #endregion

	} // class GroupBox
} // namespace Viatronix.UI

#region revision history

// $Log: GroupBox.cs,v $
// Revision 1.5.28.1  2009/01/26 21:20:09  kchalupa
// Issue #6227
//
// Revision 1.5  2004/11/22 20:03:21  gdavidson
// Moved the location of the text.
//
// Revision 1.4  2004/10/18 14:51:05  geconomos
// reverted back to old style
//
// Revision 1.3  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.2  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.13.2.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.13  2003/08/20 14:22:59  michael
// added "mailto:"
//
// Revision 1.12  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.11  2003/04/28 19:25:11  frank
// code review
//
// Revision 1.10  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.9  2003/04/23 15:02:35  geconomos
// Updated for PolygonGenerator to Shapes change.
//
// Revision 1.8  2003/04/22 14:48:33  geconomos
// More coding standards.
//
// Revision 1.7  2003/04/07 18:07:04  geconomos
// Changed color White to RGB( 250, 250, 250 ).
//
// Revision 1.6  2003/01/15 17:12:18  geconomos
// Added call to base class in OnResize()
//
// Revision 1.5  2002/12/12 15:39:19  geconomos
// Code cleanup.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/GroupBox.cs,v 1.5.28.1 2009/01/26 21:20:09 kchalupa Exp $
// $Id: GroupBox.cs,v 1.5.28.1 2009/01/26 21:20:09 kchalupa Exp $

#endregion
