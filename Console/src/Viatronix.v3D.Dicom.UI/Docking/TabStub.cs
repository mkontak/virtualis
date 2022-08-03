// $Id: TabStub.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;
using Microsoft.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  [ToolboxItem(false)]
  public class TabStub : UserControl
  {
    private class DrawTab
    {
      protected int m_index;
      protected Rectangle m_drawRect;
      protected Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage m_tabPage;

      public DrawTab(Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage tabPage, Rectangle drawRect, int index)
      {
        m_index = index;
        m_tabPage = tabPage;
        m_drawRect = drawRect;
      }

      public Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage TabPage  { get { return m_tabPage; } }
      public Rectangle DrawRect                        { get { return m_drawRect; } }
      public int Index                                 { get { return m_index; } }
    }

    // Class constants
    protected static int m_imageGap = 3;
    protected static int m_imageGaps = 6;
    protected static int m_imageVector = 16;
    protected static int m_beginGap = 2;
    protected static int m_endGap = 8;
    protected static int m_sideGap = 2;
    protected static int m_hoverInterval = 500;

    // Instance fields
    protected Edge m_edge;
    protected int m_hoverOver;
    protected int m_hoverItem;
    protected int m_selectedIndex;
    protected bool m_defaultFont;
    protected bool m_defaultColor;
    protected Color m_backIDE;
    protected System.Windows.Forms.Timer m_hoverTimer;
    protected TabPageCollection m_tabPages;
    protected WindowContentTabbed m_wct;
    protected ArrayList m_drawTabs;
    protected VisualStyle m_style;

    public delegate void TabStubIndexHandler(TabStub sender, int pageIndex);
    public delegate void TabStubHandler(TabStub sender);

    // Exposed events
    public event TabStubIndexHandler PageClicked;
    public event TabStubIndexHandler PageOver;
    public event TabStubHandler PagesLeave;

    public TabStub(VisualStyle style)
    {
      // Default state
      m_wct = null;
      m_style = style;
      m_hoverOver = -1;
      m_hoverItem = -1;
      m_selectedIndex = -1;
      m_defaultFont = true;
      m_defaultColor = true;
      m_edge = Edge.None;
      m_drawTabs = new ArrayList();
      m_tabPages = new TabPageCollection();
      base.Font = SystemInformation.MenuFont;

      // Hookup to collection events
      m_tabPages.Cleared += new CollectionClear(OnClearedPages);
      m_tabPages.Inserted += new CollectionChange(OnInsertedPage);
      m_tabPages.Removing += new CollectionChange(OnRemovingPage);
      m_tabPages.Removed += new CollectionChange(OnRemovedPage);

      // Need notification when the MenuFont is changed
      Microsoft.Win32.SystemEvents.UserPreferenceChanged += new 
        UserPreferenceChangedEventHandler(OnPreferenceChanged);

      // Default default colors
      DefineBackColor(SystemColors.Control);

      // Create the Timer for handling hovering over items
      m_hoverTimer = new System.Windows.Forms.Timer();
      m_hoverTimer.Interval = m_hoverInterval;
      m_hoverTimer.Tick += new EventHandler(OnTimerExpire);
    }

    protected override void Dispose(bool disposing)
    {
      if(disposing)
      {
        // Remove notifications
        Microsoft.Win32.SystemEvents.UserPreferenceChanged -= new 
          UserPreferenceChangedEventHandler(OnPreferenceChanged);
      }
      base.Dispose(disposing);
    }

    public TabPageCollection TabPages
    {
      get { return m_tabPages; }

      set
      {
        m_tabPages.Clear();
        m_tabPages = value;
      }
    }

    public Edge Edging
    {
      get { return m_edge; }

      set
      {
        if (value != m_edge)
        {
          m_edge = value;
          ResizeControl();
          Recalculate();
          Invalidate();
        }
      }
    }

    public int SelectedIndex
    {
      get { return m_selectedIndex; }

      set
      {
        if (value != m_selectedIndex)
        {
          m_selectedIndex = value;
          Recalculate();
          Invalidate();
        }
      }
    }

    public override Font Font
    {
      get { return base.Font; }

      set
      {
        if (value != null)
        {
          if (value != base.Font)
          {
            m_defaultFont = (value == SystemInformation.MenuFont);

            base.Font = value;
            ResizeControl();
            Recalculate();
            Invalidate();
          }
        }
      }
    }

    public override Color BackColor
    {
      get { return base.BackColor; }

      set
      {
        if (this.BackColor != value)
        {
          m_defaultColor = (value == SystemColors.Control);
          DefineBackColor(value);
          Invalidate();
        }
      }
    }

    public WindowContentTabbed WindowContentTabbed
    {
      get { return m_wct; }
      set { m_wct = value; }
    }
        
    public virtual void OnPageClicked(int pageIndex)
    {
      // Has anyone registered for the event?
      if (PageClicked != null)
        PageClicked(this, pageIndex);
    }

    public virtual void OnPageOver(int pageIndex)
    {
      // Has anyone registered for the event?
      if (PageOver != null)
        PageOver(this, pageIndex);
    }

    public virtual void OnPagesLeave()
    {
      // Has anyone registered for the event?
      if (PagesLeave != null)
        PagesLeave(this);
    }
        
    public void PropogateNameValue(PropogateName name, object value)
    {
      switch(name)
      {
        case PropogateName.BackColor:
          this.BackColor = (Color)value;
          Invalidate();
          break;
        case PropogateName.InactiveTextColor:
          this.ForeColor = (Color)value;
          Invalidate();
          break;
        case PropogateName.CaptionFont:
          this.Font = (Font)value;
          break;
      }
            
      // Pass onto the contained WCT
      m_wct.PropogateNameValue(name, value);
    }
                
    protected void DefineBackColor(Color backColor)
    {
      base.BackColor = backColor;
			
      m_backIDE = ColorHelper.TabBackgroundFromBaseColor(backColor);
    }

    protected void OnPreferenceChanged(object sender, UserPreferenceChangedEventArgs e)
    {
      // Are we using the default menu or a user defined value?
      if (m_defaultFont)
      {
        base.Font = SystemInformation.MenuFont;
        ResizeControl();
        Recalculate();
        Invalidate();
      }
    }

    protected override void OnSystemColorsChanged(EventArgs e)
    {
      // If still using the Default color when we were created
      if (m_defaultColor)
      {
        this.BackColor = SystemColors.Control;
        Invalidate();
      }

      base.OnSystemColorsChanged(e);
    }

    protected void OnClearedPages()
    {
      // Cancel any hover selection
      CancelHoverItem();

      // Cancel any current selection
      m_selectedIndex = -1;

      ResizeControl();
      Recalculate();
      Invalidate();
    }
		
    protected void OnInsertedPage(int index, object value)
    {
      // If no page is currently selected
      if (m_selectedIndex == -1)
      {
        // Then make the inserted page selected
        m_selectedIndex = index;
      }

      ResizeControl();
      Recalculate();
      Invalidate();
    }

    protected void OnRemovingPage(int index, object value)
    {
      // Removed page involved in hover calculations?
      if ((m_hoverOver == index) || (m_hoverItem == index))
        CancelHoverItem();
        
      // Removing the last page?
      if (m_tabPages.Count == 1)
      {
        // Get rid of any selection
        m_selectedIndex = -1;
      }
      else
      {
        // If removing a page before the selected one...
        if (index < m_selectedIndex)
        {
          // ...then the selected index must be decremented to match
          m_selectedIndex--;
        }
        else
        {
          // If the selected page is the last one then...
          if (m_selectedIndex == (m_tabPages.Count-1))
          {
            // Must reduce selected index
            m_selectedIndex--;
          }
        }
      }
    }

    protected void OnRemovedPage(int index, object value)
    {
      ResizeControl();
      Recalculate();
      Invalidate();
    }

    protected void CancelHoverItem()
    {
      // Currently timing a hover change?
      if (m_hoverOver != -1)
      {
        // Prevent timer from expiring
        m_hoverTimer.Stop();
                
        // No item being timed
        m_hoverOver = -1;
      }

      // Any current hover item?
      if (m_hoverItem != -1)
      {
        // No item is being hovered
        m_hoverItem = -1;
		        
        // Generate event for end of hover
        OnPagesLeave();
      }
    }

    protected override void OnMouseMove(MouseEventArgs e)
    {
      // Create a point representing current mouse position
      Point mousePos = new Point(e.X, e.Y);

      int index = 0;
      int count = m_drawTabs.Count;

      // Search each draw cell
      for(; index<count; index++)
      {
        DrawTab dt = m_drawTabs[index] as DrawTab;

        // Is mouse over this cell?
        if (dt.DrawRect.Contains(mousePos))
        {
          // If the mouse is not over the hover item
          if (m_hoverItem != dt.Index)
          {
            // And we are not already timing this change in hover
            if (m_hoverOver != dt.Index)
            {
              // Start timing the hover change
              m_hoverTimer.Start();
						    
              // Remember which item we are timing
              m_hoverOver = dt.Index;
            }
          }

          break;
        }
      }

      // Failed to find an item?
      if (index == count)
      {
        // If we have a hover item or timing a hover change
        if ((m_hoverOver != -1) || (m_hoverItem != -1))
        {
          // Stop any timing
          CancelHoverItem();
        }
      }

      base.OnMouseMove(e);
    }

    protected override void OnMouseLeave(EventArgs e)
    {
      // Remove any hover state
      CancelHoverItem();
    
      base.OnMouseLeave(e);
    }

    protected void OnTimerExpire(object sender, EventArgs e)
    {
      // Prevent the timer from firing again
      m_hoverTimer.Stop();

      // A change in hover still valid?
      if (m_hoverItem != m_hoverOver)
      {
        // This item becomes the current hover item
        m_hoverItem = m_hoverOver;
                
        // No longer in a timing state
        m_hoverOver = -1;

        // Do we need a change in selection?
        if (m_selectedIndex != m_hoverItem)
        {
          // Change selection and redraw
          m_selectedIndex = m_hoverItem;

          Recalculate();
          Invalidate();
        }

        // Generate event to notify where mouse is now hovering
        OnPageOver(m_selectedIndex);
      }
    }

    protected override void OnMouseDown(MouseEventArgs e)
    {
      // Only select a button or page when using left mouse button
      if (e.Button == MouseButtons.Left)
      {
        // Create a point representing current mouse position
        Point mousePos = new Point(e.X, e.Y);

        int count = m_drawTabs.Count;

        // Search each draw cell
        for(int index=0; index<count; index++)
        {
          DrawTab dt = m_drawTabs[index] as DrawTab;

          // Is mouse pressed in this draw cell?
          if (dt.DrawRect.Contains(mousePos))
          {
            // Prevent any hover timer expiring
            m_hoverTimer.Stop();
						    
            // This becomes the current hover item
            m_hoverItem = m_selectedIndex;
						    
            // Not timing a hover change
            m_hoverOver = m_hoverItem;
                        
            // Will this cause a change in selection?
            if (m_selectedIndex != dt.Index)
            {
              // Change selection and redraw
              m_selectedIndex = dt.Index;
						
              Recalculate();
              Invalidate();
            }

            // Generate event to notify a click occurred on the selection
            OnPageClicked(m_selectedIndex);

            break;
          }
        }
      }
    }

    public static int TabStubVector(Font font)
    {
      int fixedVector = m_imageVector + m_imageGaps;

      int minFontVector = font.Height + m_imageGaps;

      // Make sure at least bit enough for the provided font
      if (fixedVector < minFontVector)
        fixedVector = minFontVector;
                
      return fixedVector + m_sideGap;
    }

    protected void ResizeControl()
    {
      int textMax = 0;

      // Find largest space needed for drawing page text
      using(Graphics g = this.CreateGraphics())
      {
        foreach(Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage page in m_tabPages)
        {
          // Find width of the requested text
          SizeF dimension = g.MeasureString(page.Title, this.Font);

          if ((int)dimension.Width > textMax)
            textMax = (int)dimension.Width;
        }
      }

      // Calculate total width/height needed
      int variableVector = m_tabPages.Count * (m_imageVector + m_imageGaps) + textMax + m_imageGap;

      // Calculate the fixed direction value
      int fixedVector = TabStubVector(this.Font);

      // Resize the control as appropriate
      switch(m_edge)
      {
        case Edge.Left:
        case Edge.Right:
          this.Size = new Size(fixedVector, variableVector + m_beginGap + m_endGap);
          break;
        case Edge.Top:
        case Edge.Bottom:
        case Edge.None:
        default:
          this.Size = new Size(variableVector + m_beginGap + m_endGap, fixedVector);
          break;
      }
    }

    protected void Recalculate()
    {
      // Create a fresh colleciton for drawing objects
      m_drawTabs = new ArrayList();

      // Need start and end position markers
      int posEnd;
      int cellVector = m_imageVector + m_imageGaps;
      int posStart = m_beginGap;

      switch(m_edge)
      {
        case Edge.Left:
        case Edge.Right:
          posEnd = this.Height - m_endGap;
          break;
        case Edge.Top:
        case Edge.Bottom:
        case Edge.None:
        default:
          posEnd = this.Width - m_endGap;
          break;
      }

      int count = m_tabPages.Count;

      // Process from start of list until we find the selected one
      for(int index=0; (index<count) && (index!=m_selectedIndex); index++)
      {
        Rectangle drawRect;

        // Drawing rectangle depends on direction
        switch(m_edge)
        {
          case Edge.Left:
            drawRect = new Rectangle(0, posStart, this.Width - m_sideGap - 1, cellVector);
            break;
          case Edge.Right:
            drawRect = new Rectangle(m_sideGap, posStart, this.Width - m_sideGap, cellVector);
            break;
          case Edge.Bottom:
            drawRect = new Rectangle(posStart, m_sideGap, cellVector, this.Height - m_sideGap);
            break;
          case Edge.Top:
          case Edge.None:
          default:
            drawRect = new Rectangle(posStart, 0, cellVector, this.Height - m_sideGap - 1);
            break;
        }

        // Move starting position
        posStart += cellVector;

        // Generate new drawing object for this tab
        m_drawTabs.Add(new DrawTab(m_tabPages[index], drawRect, index));
      }

      // Process from end of list until we find the selected one
      for(int index=count-1; (index>=0) && (index!=m_selectedIndex); index--)
      {
        Rectangle drawRect;

        // Drawing rectangle depends on direction
        switch(m_edge)
        {
          case Edge.Left:
            drawRect = new Rectangle(0, posEnd - cellVector, this.Width - m_sideGap - 1, cellVector);
            break;
          case Edge.Right:
            drawRect = new Rectangle(m_sideGap, posEnd - cellVector, this.Width - m_sideGap, cellVector);
            break;
          case Edge.Bottom:
            drawRect = new Rectangle(posEnd - cellVector, m_sideGap, cellVector, this.Height - m_sideGap);
            break;
          case Edge.Top:
          case Edge.None:
          default:
            drawRect = new Rectangle(posEnd - cellVector, 0, cellVector, this.Height - m_sideGap - 1);
            break;
        }

        // Move starting position
        posEnd -= cellVector;

        // Generate new drawing object for this tab
        m_drawTabs.Add(new DrawTab(m_tabPages[index], drawRect, index));
      }

      if (m_selectedIndex != -1)
      {
        Rectangle drawRect;

        // Drawing rectangle depends on direction
        switch(m_edge)
        {
          case Edge.Left:
            drawRect = new Rectangle(0, posStart, this.Width - m_sideGap - 1, posEnd - posStart);
            break;
          case Edge.Right:
            drawRect = new Rectangle(m_sideGap, posStart, this.Width - m_sideGap, posEnd - posStart);
            break;
          case Edge.Bottom:
            drawRect = new Rectangle(posStart, m_sideGap, posEnd - posStart, this.Height - m_sideGap);
            break;
          case Edge.Top:
          case Edge.None:
          default:
            drawRect = new Rectangle(posStart, 0, posEnd - posStart, this.Height - m_sideGap - 1);
            break;
        }

        // Generate new drawing object for this tab
        m_drawTabs.Add(new DrawTab(m_tabPages[m_selectedIndex], drawRect, m_selectedIndex));
      }
    }

    protected void AdjustRectForEdge(ref Rectangle rect)
    {
      // Adjust rectangle to exclude desired edge
      switch(m_edge)
      {
        case Edge.Left:
          rect.X--;
          rect.Width++;
          break;
        case Edge.Right:
          rect.Width++;
          break;
        case Edge.Bottom:
          rect.Height++;
          break;
        case Edge.Top:
        case Edge.None:
        default:
          rect.Y--;
          rect.Height++;
          break;
      }
    }

    protected void DrawOutline(Graphics g, bool pre)
    {
      Rectangle borderRect = new Rectangle(0, 0, this.Width - 1, this.Height - 1);

      // Adjust for drawing area
      switch(m_edge)
      {
        case Edge.Left:
          borderRect.Y += m_beginGap;
          borderRect.Height -= m_beginGap + m_endGap - 1;
          borderRect.Width -= m_sideGap;
          break;
        case Edge.Right:
          borderRect.Y += m_beginGap;
          borderRect.Height -= m_beginGap + m_endGap - 1;
          borderRect.X += m_sideGap;
          borderRect.Width -= m_sideGap;
          break;
        case Edge.Bottom:
          borderRect.Y += m_sideGap;
          borderRect.Height -= m_sideGap;
          borderRect.X += m_beginGap;
          borderRect.Width -= m_beginGap + m_endGap - 1;
          break;
        case Edge.Top:
        case Edge.None:
        default:
          borderRect.Height -= m_sideGap;
          borderRect.X += m_beginGap;
          borderRect.Width -= m_beginGap + m_endGap - 1;
          break;
      }

      // Remove unwated drawing edge
      AdjustRectForEdge(ref borderRect);

      if (pre)
      {
        if (m_style == VisualStyle.IDE)
        {
          // Fill tab area in required color
          using(SolidBrush fillBrush = new SolidBrush(this.BackColor))
            g.FillRectangle(fillBrush, borderRect);
        }
      }
      else
      {
        if (m_style == VisualStyle.Plain)
        {
          using(Pen penL = new Pen(ControlPaint.LightLight(this.BackColor)),
                  penD = new Pen(ControlPaint.Dark(this.BackColor)))
          {
            g.DrawLine(penL, borderRect.Left, borderRect.Top, borderRect.Right, borderRect.Top);
            g.DrawLine(penL, borderRect.Left, borderRect.Top, borderRect.Left, borderRect.Bottom);
            g.DrawLine(penD, borderRect.Right, borderRect.Top, borderRect.Right, borderRect.Bottom);
            g.DrawLine(penD, borderRect.Right, borderRect.Bottom, borderRect.Left, borderRect.Bottom);
          }
        }
      }
    }

    protected void DrawOutlineForCell(Graphics g, Pen pen, Rectangle rect)
    {
      // Draw border around the tab cell
      if (m_style == VisualStyle.IDE)
        g.DrawRectangle(pen, rect);
      else
      {
        switch(m_edge)
        {
          case Edge.Left:
          case Edge.Right:
            g.DrawLine(pen, rect.Left + 1, rect.Bottom, rect.Right, rect.Bottom);       
            break;                    
          case Edge.Top:
          case Edge.Bottom:
            g.DrawLine(pen, rect.Right, rect.Top + 1, rect.Right, rect.Bottom);       
            break;                    
        }
      }
    }

    protected override void OnPaintBackground(PaintEventArgs e)
    {
    }

    protected override void OnPaint(PaintEventArgs e)
    {
      // Fill background in required color
      if (m_style == VisualStyle.IDE)            
        using(SolidBrush fillBrush = new SolidBrush(m_backIDE))
          e.Graphics.FillRectangle(fillBrush, this.ClientRectangle);
      else
        using(SolidBrush fillBrush = new SolidBrush(this.BackColor))
          e.Graphics.FillRectangle(fillBrush, this.ClientRectangle);

      // Style specific outline drawing
      DrawOutline(e.Graphics, true);

      // Draw border around area
      using(Pen borderPen = new Pen(ControlPaint.LightLight(this.ForeColor)))
      {
        // Draw each of the draw objects
        foreach(DrawTab dt in m_drawTabs)
        {
          Rectangle drawRect = dt.DrawRect;

          AdjustRectForEdge(ref drawRect);

          // Style specific cell outline drawing
          DrawOutlineForCell(e.Graphics, borderPen, drawRect);

          // Draw the image in the left/top of the cell
          Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage page = dt.TabPage;

          int xDraw;
          int yDraw;

          switch(m_edge)
          {
            case Edge.Left:
            case Edge.Right:
              xDraw = drawRect.Left + (drawRect.Width - m_imageVector) / 2;
              yDraw = drawRect.Top + m_imageGap;
              break;
            case Edge.Top:
            case Edge.Bottom:
            case Edge.None:
            default:
              xDraw = drawRect.Left + m_imageGap;
              yDraw = drawRect.Top + (drawRect.Height - m_imageVector) / 2;
              break;
          }

          if ((page.ImageIndex != -1) && (page.ImageList != null))
          {
            // Draw the actual image
            e.Graphics.DrawImage(page.ImageList.Images[page.ImageIndex],
              new Rectangle(xDraw, yDraw, m_imageVector, m_imageVector));
          }

          // Is anything currently selected
          if (m_selectedIndex != -1)
          {
            // Is this page selected?
						
            if (page == m_tabPages[m_selectedIndex])
            {
              Rectangle textRect;

              StringFormat drawFormat = new StringFormat();
              drawFormat.FormatFlags = StringFormatFlags.NoClip | StringFormatFlags.NoWrap;
              drawFormat.Alignment = StringAlignment.Center;
              drawFormat.LineAlignment = StringAlignment.Center;

              // Create text drawing rectangle
              switch(m_edge)
              {
                case Edge.Left:
                case Edge.Right:
                  textRect = new Rectangle(drawRect.Left, yDraw + m_imageVector + m_imageGap, 
                    drawRect.Width, drawRect.Height - m_imageVector - m_imageGap * 2);
                  drawFormat.FormatFlags |= StringFormatFlags.DirectionVertical;
                  break;
                case Edge.Top:
                case Edge.Bottom:
                case Edge.None:
                default:
                  textRect = new Rectangle(xDraw + m_imageVector + m_imageGap, drawRect.Top, 
                    drawRect.Width - m_imageVector - m_imageGap * 2, drawRect.Height);
                  break;
              }
							
              Color brushColor = this.ForeColor;
							
              if (m_style == VisualStyle.IDE)
                brushColor = ControlPaint.Light(brushColor);

              using(SolidBrush drawBrush = new SolidBrush(brushColor))
                e.Graphics.DrawString(page.Title, this.Font, drawBrush, textRect, drawFormat);
            }
          }
        }
      }

      // Style specific outline drawing
      DrawOutline(e.Graphics, false);
            
      base.OnPaint(e);
    }
  }
}


#region revision history

// $Log: TabStub.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/TabStub.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: TabStub.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion


