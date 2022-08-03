// $Id: Panel.cs,v 1.2 2005/06/22 15:07:54 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
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
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
	/// <summary>
	/// A panel with configurable border and curvature.
	/// </summary>
	public class Panel : System.Windows.Forms.Panel
	{

    #region fields

    /// <summary>
    /// Top padding.
    /// </summary>
		private int m_topPadding = 10;

    /// <summary>
    /// Bottom padding.
    /// </summary>
    private int m_bottomPadding = 10;

    /// <summary>
    /// Left padding.
    /// </summary>
    private int m_leftPadding = 10;

    /// <summary>
    /// Right padding.
    /// </summary>
    private int m_rightPadding = 10;

    /// <summary>
    /// The amount of curve on the corners.
    /// </summary>
    private int m_curve = 10;

    /// <summary>
    /// The edges being curved.
    /// </summary>
    private Edges m_edges = Edges.All;

    /// <summary>
    /// The border color.
    /// </summary>
    private Color m_borderColor = Color.White;

    /// <summary>
    /// The fill color.
    /// </summary>
    private Color m_fillColor = Theme.PaneBackColor;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a Panel control.
    /// </summary>
		public Panel()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );

      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      this.BackColor = Theme.BackColor;
		} // Panel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the top padding.
    /// </summary>
    public int TopPadding
    {
      get { return m_topPadding; }
      set 
      { 
        m_topPadding = value; 
        Invalidate();
      }
    } // TopPadding


    /// <summary>
    /// Gets or sets the bottom padding.
    /// </summary>
    public int BottomPadding
    {
      get { return m_bottomPadding; }
      set 
      { 
        m_bottomPadding = value; 
        Invalidate();
      }
    } // BottomPadding


    /// <summary>
    /// Gets or sets the left padding.
    /// </summary>
    public int LeftPadding
    {
      get { return m_leftPadding; }
      set 
      { 
        m_leftPadding = value; 
        Invalidate();
      }
    } // LeftPadding


    /// <summary>
    /// Gets or sets the right padding.
    /// </summary>
    public int RightPadding
    {
      get { return m_rightPadding; }
      set 
      { 
        m_rightPadding = value; 
        Invalidate();
      }
    } // RightPadding


    /// <summary>
    /// Gets or sets the border color.
    /// </summary>
    public Color BorderColor
    {
      get { return m_borderColor; }
      set 
      { 
        m_borderColor = value; 
        Invalidate();
      }
    } // BorderColor


    /// <summary>
    /// Gets or set the fill color.
    /// </summary>
    public Color FillColor
    {
      get { return m_fillColor; }
      set 
      { 
        m_fillColor = value; 
        Invalidate();
      }
    } // FillColor


    /// <summary>
    /// Gets or sets the curve of the corners.
    /// </summary>
    public int Curve
    {
      get { return m_curve; }
      set 
      { 
        m_curve = value;
        Invalidate();
      }
    } // Curve


    /// <summary>
    /// Gets or sets the edges being curved.
    /// </summary>
    public Edges Edges
    {
      get { return m_edges; }
      set
      {
        m_edges = value;
        Invalidate();
      }
    } // Edges

    #endregion

    #region override methods

    /// <summary>
    /// Paints the background and border for the panel.
    /// </summary>
    /// <param name="args">A PaintEventArgs that contains the event data.</param>
		protected override void OnPaint( PaintEventArgs args )
		{
      base.OnPaint( args );

      Graphics gfx = args.Graphics;
      gfx.SmoothingMode = SmoothingMode.HighQuality;

      int x = this.ClientRectangle.X + this.LeftPadding;
      int y = this.ClientRectangle.Y + this.TopPadding;

      int width = this.ClientRectangle.Width - x - this.RightPadding - 1;
      int height = this.ClientRectangle.Height - y - this.BottomPadding - 1;

      Rectangle bounds = new Rectangle( x, y, width, height );
      PointF[] points = Shapes.BeveledEdge( bounds, Edges.All, this.Curve );

      using( SolidBrush brush = new SolidBrush( this.FillColor ))
        gfx.FillPolygon( brush, points );

      using( Pen pen = new Pen( this.BorderColor ))
        gfx.DrawPolygon( pen, points );
		} // OnPaint( args )


    /// <summary>
    /// Resizes the control.
    /// </summary>
    /// <param name="args">A EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Processes Windows messages.
    /// </summary>
    /// <param name="m">The Windows Message to process.</param>
    protected override void WndProc( ref Message m ) 
    {
      if( m.Msg == (int) Win32.WindowMessages.WM_PRINT ) 
      {
        if( !this.Visible ) 
          this.Visible = true;
      }

      base.WndProc(ref m);
    } // WndProc( m ) 


    #endregion

	} // class Panel
} // namespace Viatronix.UI

#region revision history

// $Log: Panel.cs,v $
// Revision 1.2  2005/06/22 15:07:54  gdavidson
// Overrided the WndProc method.
//
// Revision 1.1  2005/05/26 17:27:18  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Panel.cs,v 1.2 2005/06/22 15:07:54 gdavidson Exp $
// $Id: Panel.cs,v 1.2 2005/06/22 15:07:54 gdavidson Exp $

#endregion
