// $Id: ResizeBar.cs,v 1.1 2005/10/11 14:41:31 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public interface IResizeSource
  {
    bool CanResize(ResizeBar bar);
    bool StartResizeOperation(ResizeBar bar, ref Rectangle screenBoundary);
    void EndResizeOperation(ResizeBar bar, int delta);
    Color ResizeBarColor { get; }
    int ResizeBarVector { get; }
    VisualStyle Style { get; }
    Color BackgroundColor { get; }
  }

  [ToolboxItem(false)]
  public class ResizeBar : Control
  {
    // Class constants
    protected static int m_ideLength = 4;
    protected static int m_plainLength = 6;

    // Instance fields
    protected VisualStyle m_style;
    protected Direction m_direction;
    protected bool m_resizing;
    protected Point m_pointStart;
    protected Point m_pointCurrent;
    protected Rectangle m_boundary;
    protected IResizeSource m_resizeSource;

    public ResizeBar(Direction direction, IResizeSource resizeSource)
    {
      // Define initial state
      m_direction = direction;
      m_resizing = false;
      m_resizeSource = resizeSource;

      // Always default to Control color
      this.BackColor = m_resizeSource.ResizeBarColor;
      this.ForeColor = SystemColors.ControlText;

      UpdateStyle(m_resizeSource.Style);
    }

    public VisualStyle Style
    {
      get { return m_style; }

      set
      {
        if (m_style != value)
          UpdateStyle(value);
      }
    }

    public Direction Direction
    {
      get { return m_direction; }

      set
      {
        if (m_direction != value)
        {
          m_direction = value;
          UpdateStyle(m_style);
        }
      }
    }

    public int Length
    {
      get
      {
        int vector = m_resizeSource.ResizeBarVector;
                
        if (vector == -1)
        {
          if (m_style == VisualStyle.IDE)
            vector = m_ideLength;
          else 
            vector = m_plainLength;
        }
               
        return vector;
      }
            
      set
      {
        // If a change in vector...
        if (value != this.Length)
        {
          // Force update of the height/width
          UpdateStyle(m_resizeSource.Style);
        }
      }
    }

    public virtual void PropogateNameValue(PropogateName name, object value)
    {
      if (name == PropogateName.ResizeBarVector)
        this.Length = (int)value;

      if (name == PropogateName.ResizeBarColor)
      {
        this.BackColor = (Color)value;
        Invalidate();
      }
    }
        
    protected void UpdateStyle(VisualStyle newStyle)
    {
      m_style = newStyle;

      int vector = this.Length;

      if (m_direction == Direction.Vertical)
        this.Height = vector;
      else
        this.Width = vector;

      Invalidate();
    }

    protected bool StartResizeOperation(MouseEventArgs e)
    {
      if (m_resizeSource != null)
      {
        if (m_resizeSource.CanResize(this))
        {
          if (m_resizeSource.StartResizeOperation(this, ref m_boundary))
          {
            // Record the starting screen position
            m_pointStart = PointToScreen(new Point(e.X, e.Y));

            // Record the current position being drawn
            m_pointCurrent = m_pointStart;

            // Draw the starting position
            DrawResizeIndicator(m_pointCurrent);

            return true;
          }
        }
      }

      return false;
    }

    protected void EndResizeOperation(MouseEventArgs e)
    {
      if (m_resizeSource != null)
      {
        // Undraw the current position
        DrawResizeIndicator(m_pointCurrent);

        // Find new screen position
        Point newCurrent = PointToScreen(new Point(e.X, e.Y));

        // Limit the extend the bar can be moved
        ApplyBoundaryToPoint(ref newCurrent);

        // Calculate delta from initial resize
        Point delta = new Point(newCurrent.X - m_pointStart.X, 
          newCurrent.Y - m_pointStart.Y);

        // Inform the Zone of requested change
        if (m_direction == Direction.Horizontal)
          m_resizeSource.EndResizeOperation(this, delta.X);
        else
          m_resizeSource.EndResizeOperation(this, delta.Y);
      }

      m_resizing = false;
    }

    protected void UpdateResizePosition(MouseEventArgs e)
    {
      // Find new screen position
      Point newCurrent = PointToScreen(new Point(e.X, e.Y));

      // Limit the extend the bar can be moved
      ApplyBoundaryToPoint(ref newCurrent);

      // Has change in position occurred?
      if (newCurrent != m_pointCurrent)
      {
        // Undraw the old position
        DrawResizeIndicator(m_pointCurrent);

        // Record the new screen position
        m_pointCurrent = newCurrent;

        // Draw the new position
        DrawResizeIndicator(m_pointCurrent);
      }
    }
	
    protected void ApplyBoundaryToPoint(ref Point newCurrent)
    {
      // Calculate mouse position delta from mouse down
      Point delta = new Point(newCurrent.X - m_pointStart.X, 
        newCurrent.Y - m_pointStart.Y);
			
      // Get our dimensions in screen coordinates
      Rectangle client = RectangleToScreen(this.ClientRectangle);

      if (m_direction == Direction.Horizontal)
      {
        client.Offset(delta.X, 0);

        // Test against left hand edge
        if (client.X < m_boundary.X)
          newCurrent.X += m_boundary.X - client.X;
				
        // Test against right hand edge
        if (client.Right > m_boundary.Right)
          newCurrent.X -= client.Right - m_boundary.Right;
      }
      else
      {
        client.Offset(0, delta.Y);

        // Test against top edge
        if (client.Y < m_boundary.Y)
          newCurrent.Y += m_boundary.Y - client.Y;
				
        // Test against bottom edge
        if (client.Bottom > m_boundary.Bottom)
          newCurrent.Y -= client.Bottom - m_boundary.Bottom;
      }		
    }

    protected void DrawResizeIndicator(Point screenPosition)
    {
      // Calculate mouse position delta from mouse down
      Point delta = new Point(screenPosition.X - m_pointStart.X, 
        screenPosition.Y - m_pointStart.Y);

      // Get our dimensions in screen coordinates
      Rectangle client = RectangleToScreen(this.ClientRectangle);

      if (m_direction == Direction.Horizontal)
        client.Offset(delta.X, 0);
      else
        client.Offset(0, delta.Y);

      DrawHelper.DrawDragRectangle(client, 0);
    }

    protected override void OnMouseDown(MouseEventArgs e)
    {
      // Mouse down occurred inside control
      m_resizing = StartResizeOperation(e);

      base.OnMouseDown(e);
    }

    protected override void OnMouseUp(MouseEventArgs e)
    {
      // Currently in a resizing operation?
      if (m_resizing)
      {
        // Reset resizing state
        EndResizeOperation(e);
      }

      base.OnMouseUp(e);
    }

    protected override void OnMouseMove(MouseEventArgs e)
    {
      if ((m_resizeSource != null) && (m_resizeSource.CanResize(this)))
      {
        // Display the correct mouse shape
        if (m_direction == Direction.Vertical)
          this.Cursor = Cursors.SizeNS;
        else
          this.Cursor = Cursors.SizeWE;
      }
      else
        this.Cursor = Cursors.Arrow;

      // Currently in a resizing operation?
      if (m_resizing)
      {
        UpdateResizePosition(e);
      }

      base.OnMouseMove(e);
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      // Plain style draws a 3D effect around edges
      if (m_style == VisualStyle.Plain)
      {
        // Drawing is relative to client area
        Size ourSize = this.ClientSize;

        Point[] light = new Point[2];
        Point[] dark = new Point[2];
        Point[] black = new Point[2];

        // Painting depends on orientation
        if (m_direction == Direction.Vertical)
        {
          // Draw as a horizontal bar
          dark[0].Y = dark[1].Y = ourSize.Height - 2;
          black[0].Y = black[1].Y = ourSize.Height - 1;
          light[1].X = dark[1].X = black[1].X = ourSize.Width;
        }
        else
        {
          // Draw as a vertical bar
          dark[0].X = dark[1].X = ourSize.Width - 2;
          black[0].X = black[1].X = ourSize.Width - 1;
          light[1].Y = dark[1].Y = black[1].Y = ourSize.Height;
        }

        using (Pen penLightLight = new Pen(ControlPaint.LightLight(m_resizeSource.BackgroundColor)),
                 penDark = new Pen(ControlPaint.Dark(m_resizeSource.BackgroundColor)),
                 penBlack = new Pen(ControlPaint.DarkDark(m_resizeSource.BackgroundColor)))
        {
          e.Graphics.DrawLine(penLightLight, light[0], light[1]);
          e.Graphics.DrawLine(penDark, dark[0], dark[1]);
          e.Graphics.DrawLine(penBlack, black[0], black[1]);
        }	
      }
				
      // Let delegates fire through base
      base.OnPaint(e);
    }
  }
    
  [ToolboxItem(false)]
  public class ResizeAutoBar : ResizeBar
  {
    public ResizeAutoBar(Direction direction, IResizeSource resizeSource)
      : base(direction, resizeSource) 
    {
    }    
            
    protected override void OnPaint(PaintEventArgs e)
    {
      Color backColor = this.BackColor;
        
      switch(this.Dock)
      {
        case DockStyle.Right:
          using(Pen penD = new Pen(ControlPaint.Dark(backColor)),
                  penDD = new Pen(ControlPaint.DarkDark(backColor)))
          {
            e.Graphics.DrawLine(penD, this.Width - 2, 0, this.Width - 2, this.Height);
            e.Graphics.DrawLine(penDD, this.Width - 1, 0, this.Width - 1, this.Height);
          }
          break;
        case DockStyle.Left:
          using(Pen penLL = new Pen(ControlPaint.LightLight(backColor)))
            e.Graphics.DrawLine(penLL, 1, 0, 1, this.Height);
          break;
        case DockStyle.Bottom:
          using(Pen penD = new Pen(ControlPaint.Dark(backColor)),
                  penDD = new Pen(ControlPaint.DarkDark(backColor)))
          {
            e.Graphics.DrawLine(penD, 0, this.Height - 2, this.Width, this.Height - 2);
            e.Graphics.DrawLine(penDD, 0, this.Height - 1, this.Width, this.Height - 1);
          }
          break;
        case DockStyle.Top:
          using(Pen penLL = new Pen(ControlPaint.LightLight(backColor)))
            e.Graphics.DrawLine(penLL, 0, 1, this.Width, 1);
          break;
      }
    }
  }
}

#region revision history

// $Log: ResizeBar.cs,v $
// Revision 1.1  2005/10/11 14:41:31  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/ResizeBar/ResizeBar.cs,v 1.1 2005/10/11 14:41:31 mkontak Exp $
// $Id: ResizeBar.cs,v 1.1 2005/10/11 14:41:31 mkontak Exp $

#endregion

