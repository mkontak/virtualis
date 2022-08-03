// $Id: RedockerContent.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class RedockerContent : Redocker
  {
    public enum Source
    {
      RawContent,
      ContentInsideWindow,
      WindowContent,
      FloatingForm
    }

    // Class constants
    protected static int m_hotVectorFromEdge = 2;
    protected static int m_hotVectorBeforeControl = 5;

    // Instance fields
    protected ContainerControl m_container;
    protected Source m_source;
    protected Content m_content;
    protected HotZone m_currentHotZone;
    protected Control m_callingControl;
    protected FloatingForm m_floatingForm;
    protected HotZoneCollection m_hotZones;
    protected WindowContent m_windowContent;
    protected DockingManager m_dockingManager;
    protected Rectangle m_insideRect;
    protected Rectangle m_outsideRect;
    protected Point m_offset;

    public RedockerContent(Control callingControl, Content c, Point offset)
    {
      InternalConstruct(callingControl, Source.RawContent, c, null, null, c.DockingManager, offset);
    }

    public RedockerContent(Control callingControl, Content c, WindowContent wc, Point offset)
    {
      InternalConstruct(callingControl, Source.ContentInsideWindow, c, wc, null, c.DockingManager, offset);
    }

    public RedockerContent(Control callingControl, WindowContent wc, Point offset)
    {
      InternalConstruct(callingControl, Source.WindowContent, null, wc, null, wc.DockingManager, offset);
    }

    public RedockerContent(FloatingForm ff, Point offset)
    {
      InternalConstruct(ff, Source.FloatingForm, null, null, ff, ff.DockingManager, offset);
    }

    protected void InternalConstruct(Control callingControl, 
      Source source, 
      Content c, 
      WindowContent wc, 
      FloatingForm ff,
      DockingManager dm,
      Point offset)
    {
      // Store the starting state
      m_callingControl = callingControl;
      m_source = source;
      m_content = c;
      m_windowContent = wc;
      m_dockingManager = dm;
      m_container = m_dockingManager.Container;
      m_floatingForm = ff;
      m_hotZones = null;
      m_currentHotZone = null;
      m_insideRect = new Rectangle();
      m_outsideRect = new Rectangle();
      m_offset = offset;

      // Begin tracking straight away
      EnterTrackingMode();
    }

    public override void EnterTrackingMode()
    {
      // Have we entered tracking mode?
      if (!m_tracking)
      {
        base.EnterTrackingMode();

        // Source must provide a valid manager instance
        if (m_dockingManager == null)
          throw new ArgumentNullException("DockingManager");

        // Generate the hot spots that represent actions
        GenerateHotZones();
      }
    }

    public override bool ExitTrackingMode(MouseEventArgs e)
    {
      // Have we exited tracking mode?
      if (m_tracking)
      {
        base.ExitTrackingMode(e);
	
        // Is there a current HotZone active?
        if (m_currentHotZone != null)
        {
          // Convert from Control coordinates to screen coordinates
          Point mousePos = m_callingControl.PointToScreen(new Point(e.X, e.Y));
					
          // Let the zone apply whatever change it represents
          return m_currentHotZone.ApplyChange(mousePos, this);
        }
      }

      return false;
    }

    public override void QuitTrackingMode(MouseEventArgs e)
    {
      // Have we quit tracking mode?
      if (m_tracking)
      {
        if (m_callingControl.Handle != IntPtr.Zero)
        {
          // Remove any visible tracking indicator
          if (m_currentHotZone != null)
            m_currentHotZone.RemoveIndicator(new Point(0,0));
        }
                                    
        base.QuitTrackingMode(e);
      }
    }

    public override void OnMouseMove(MouseEventArgs e)
    {
      if (m_callingControl.Handle != IntPtr.Zero)
      {
        // Convert from Control coordinates to screen coordinates
        Point mousePos = m_callingControl.PointToScreen(new Point(e.X, e.Y));

        // Find HotZone this position is inside
        HotZone hz = m_hotZones.Contains(mousePos);

        if (hz != m_currentHotZone)
        {
          if (m_currentHotZone != null)
            m_currentHotZone.RemoveIndicator(mousePos);

          m_currentHotZone = hz;

          if (m_currentHotZone != null)
            m_currentHotZone.DrawIndicator(mousePos);
        }
        else
        {
          if (m_currentHotZone != null)
            m_currentHotZone.UpdateForMousePosition(mousePos, this);
        }
      }

      base.OnMouseMove(e);
    }

    public override bool OnMouseUp(MouseEventArgs e)
    {
      if (m_callingControl.Handle != IntPtr.Zero)
      {
        if (m_currentHotZone != null)
          m_currentHotZone.RemoveIndicator(m_callingControl.PointToScreen(new Point(e.X, e.Y)));
      }

      return base.OnMouseUp(e);
    }

    public Source DockingSource
    {
      get { return m_source; }
    }

    public Content Content
    {
      get { return m_content; }
    }

    public WindowContent WindowContent
    {
      get { return m_windowContent; }
    }

    public DockingManager DockingManager
    {
      get { return m_dockingManager; }
    }

    public FloatingForm FloatingForm
    {
      get { return m_floatingForm; }
    }

    protected void GenerateHotZones()
    {
      // Need the client rectangle for the whole Form
      Rectangle formClient = m_container.RectangleToScreen(m_container.ClientRectangle);

      // Create a fresh collection for HotZones
      m_hotZones = new HotZoneCollection();

      // We do not allow docking in front of the outer index entry
      int outerIndex = FindOuterIndex();

      // Create lists of Controls which are docked against each edge	
      ArrayList topList = new ArrayList();
      ArrayList leftList = new ArrayList();
      ArrayList rightList = new ArrayList();
      ArrayList bottomList = new ArrayList();
      ArrayList fillList = new ArrayList();

      PreProcessControlsCollection(topList, leftList, rightList, bottomList, fillList, outerIndex);

      int vectorH;
      int vectorV;

      // Find the vectors required for calculating new sizes
      VectorDependsOnSourceAndState(out vectorH, out vectorV);

      GenerateHotZonesForTop(topList, formClient, vectorV, outerIndex);
      GenerateHotZonesForLeft(leftList, formClient, vectorH, outerIndex);
      GenerateHotZonesForRight(rightList, formClient, vectorH, outerIndex);
      GenerateHotZonesForBottom(bottomList, formClient, vectorV, outerIndex);
      GenerateHotZonesForFill(fillList, outerIndex);
      GenerateHotZonesForFloating(SizeDependsOnSource());
    }

    protected void PreProcessControlsCollection(ArrayList topList, ArrayList leftList, ArrayList rightList, ArrayList bottomList, ArrayList fillList, int outerIndex)
    {
      // Find space left after all docking windows has been positioned
      m_insideRect = m_container.ClientRectangle; 
      m_outsideRect = m_insideRect; 

      // We want lists of docked controls grouped by docking style
      foreach(Control item in m_container.Controls)
      {
        bool ignoreType = (item is AutoHidePanel);

        int controlIndex = m_container.Controls.IndexOf(item);

        bool outer = (controlIndex >= outerIndex);

        if (item.Visible)
        {
          if (item.Dock == DockStyle.Top)
          {
            topList.Insert(0, item);

            if (m_insideRect.Y < item.Bottom)
            {
              m_insideRect.Height -= item.Bottom - m_insideRect.Y;
              m_insideRect.Y = item.Bottom;
            }

            if (outer || ignoreType)
            {
              if (m_outsideRect.Y < item.Bottom)
              {
                m_outsideRect.Height -= item.Bottom - m_outsideRect.Y;
                m_outsideRect.Y = item.Bottom;
              }
            }
          }

          if (item.Dock == DockStyle.Left)
          {
            leftList.Insert(0, item);

            if (m_insideRect.X < item.Right)
            {
              m_insideRect.Width -= item.Right - m_insideRect.X;
              m_insideRect.X = item.Right;
            }

            if (outer || ignoreType)
            {
              if (m_outsideRect.X < item.Right)
              {
                m_outsideRect.Width -= item.Right - m_outsideRect.X;
                m_outsideRect.X = item.Right;
              }
            }
          }

          if (item.Dock == DockStyle.Bottom)
          {
            bottomList.Insert(0, item);

            if (m_insideRect.Bottom > item.Top)
              m_insideRect.Height -= m_insideRect.Bottom - item.Top;

            if (outer || ignoreType)
            {
              if (m_outsideRect.Bottom > item.Top)
                m_outsideRect.Height -= m_outsideRect.Bottom - item.Top;
            }
          }

          if (item.Dock == DockStyle.Right)
          {
            rightList.Insert(0, item);

            if (m_insideRect.Right > item.Left)
              m_insideRect.Width -= m_insideRect.Right - item.Left;

            if (outer || ignoreType)
            {
              if (m_outsideRect.Right > item.Left)
                m_outsideRect.Width -= m_outsideRect.Right - item.Left;
            }
          }

          if (item.Dock == DockStyle.Fill)
            fillList.Insert(0, item);
        }
      }

      // Convert to screen coordinates
      m_insideRect = m_container.RectangleToScreen(m_insideRect);
      m_outsideRect = m_container.RectangleToScreen(m_outsideRect);
    }

    protected int FindOuterIndex()
    {
      // We do not allow docking in front of the outer index entry
      int outerIndex = m_container.Controls.Count;
			
      Control outerControl = m_dockingManager.OuterControl;

      // If an outer control has been specified then use this as the limit
      if (outerControl != null)
        outerIndex = m_container.Controls.IndexOf(outerControl);

      return outerIndex;
    }

    protected void GenerateHotZonesForLeft(ArrayList leftList, Rectangle formClient, int vector, int outerIndex)
    {
      foreach(Control c in leftList)
      {	
        bool ignoreType = (c is AutoHidePanel);
            
        int controlIndex = m_container.Controls.IndexOf(c);

        if (!ignoreType && (controlIndex < outerIndex))
        {
          // Grab the screen rectangle of the Control being processed
          Rectangle hotArea = c.RectangleToScreen(c.ClientRectangle);

          // Create the rectangle for the hot area
          hotArea.Width = m_hotVectorBeforeControl;

          // Create the rectangle for the insertion indicator
          Rectangle newSize = new Rectangle(hotArea.X, hotArea.Y, vector, hotArea.Height);
					
          hotArea.X += m_hotVectorFromEdge;

          // Create the new HotZone used to reposition a docking content/windowcontent
          m_hotZones.Add(new HotZoneReposition(hotArea, newSize, State.DockLeft, controlIndex));

          IHotZoneSource ag = c as IHotZoneSource;

          // Does this control expose an interface for its own HotZones?
          if (ag != null)
            ag.AddHotZones(this, m_hotZones);
        }
      }

      // Grab the screen rectangle of the Control being processed
      Rectangle fullArea = m_outsideRect;

      // Create the rectangle for the hot area
      fullArea.Width = m_hotVectorFromEdge;

      // Create the rectangle for the insertion indicator
      Rectangle fillSize = new Rectangle(fullArea.X, fullArea.Y, vector, fullArea.Height);

      m_hotZones.Add(new HotZoneReposition(fullArea, fillSize, State.DockLeft, false));

      // If performing our own InsideFill then do not dock at inner positions
      if (!m_dockingManager.InsideFill)
      {
        // Create the HotArea at the left side of the inner rectangle
        Rectangle innerHotArea = new Rectangle(m_insideRect.X, m_insideRect.Y, m_hotVectorBeforeControl, m_insideRect.Height);

        // Create the rectangle for tgqhe insertion indicator
        Rectangle innerNewSize = new Rectangle(innerHotArea.X, innerHotArea.Y, vector, innerHotArea.Height);

        // Create a HotZone for docking to the Left at the innermost position
        m_hotZones.Add(new HotZoneReposition(innerHotArea, innerNewSize, State.DockLeft, true));
      }
    }

    protected void GenerateHotZonesForRight(ArrayList rightList, Rectangle formClient, int vector, int outerIndex)
    {
      foreach(Control c in rightList)
      {	
        bool ignoreType = (c is AutoHidePanel);
            
        int controlIndex = m_container.Controls.IndexOf(c);

        if (!ignoreType && (controlIndex < outerIndex))
        {
          // Grab the screen rectangle of the Control being processed
          Rectangle hotArea = c.RectangleToScreen(c.ClientRectangle);

          // Create the rectangle for the hot area
          hotArea.X = hotArea.Right - m_hotVectorBeforeControl;
          hotArea.Width = m_hotVectorBeforeControl;

          // Create the rectangle for the insertion indicator
          Rectangle newSize = new Rectangle(hotArea.Right - vector, hotArea.Y, vector, hotArea.Height);
					
          hotArea.X -= m_hotVectorFromEdge;

          // Create the new HotZone used to reposition a docking content/windowcontent
          m_hotZones.Add(new HotZoneReposition(hotArea, newSize, State.DockRight, controlIndex));

          IHotZoneSource ag = c as IHotZoneSource;

          // Does this control expose an interface for its own HotZones?
          if (ag != null)
            ag.AddHotZones(this, m_hotZones);
        }
      }

      // Grab the screen rectangle of the Control being processed
      Rectangle fullArea = m_outsideRect;

      // Create the rectangle for the hot area
      fullArea.X = fullArea.Right - m_hotVectorFromEdge;
      fullArea.Width = m_hotVectorFromEdge;

      // Create the rectangle for the insertion indicator
      Rectangle fillSize = new Rectangle(fullArea.Right - vector, fullArea.Y, vector, fullArea.Height);

      m_hotZones.Add(new HotZoneReposition(fullArea, fillSize, State.DockRight, false));

      // If performing our own InsideFill then do not dock at inner positions
      if (!m_dockingManager.InsideFill)
      {
        // Create the HotArea at the right side of the inner rectangle
        Rectangle innerHotArea = new Rectangle(m_insideRect.Right - m_hotVectorBeforeControl, m_insideRect.Y, m_hotVectorBeforeControl, m_insideRect.Height);

        // Create the rectangle for the insertion indicator
        Rectangle innerNewSize = new Rectangle(innerHotArea.Right - vector, innerHotArea.Y, vector, innerHotArea.Height);

        // Create a HotZone for docking to the Left at the innermost position
        m_hotZones.Add(new HotZoneReposition(innerHotArea, innerNewSize, State.DockRight, true));
      }
    }

    protected void GenerateHotZonesForTop(ArrayList topList, Rectangle formClient, int vector, int outerIndex)
    {
      foreach(Control c in topList)
      {	
        bool ignoreType = (c is AutoHidePanel);
            
        int controlIndex = m_container.Controls.IndexOf(c);

        if (!ignoreType && (controlIndex < outerIndex))
        {
          // Grab the screen rectangle of the Control being processed
          Rectangle hotArea = c.RectangleToScreen(c.ClientRectangle);

          // Create the rectangle for the hot area
          hotArea.Height = m_hotVectorBeforeControl;

          // Create the rectangle for the insertion indicator
          Rectangle newSize = new Rectangle(hotArea.X, hotArea.Y, hotArea.Width, vector);
					
          hotArea.Y += m_hotVectorFromEdge;

          // Create the new HotZone used to reposition a docking content/windowcontent
          m_hotZones.Add(new HotZoneReposition(hotArea, newSize, State.DockTop, controlIndex));

          IHotZoneSource ag = c as IHotZoneSource;

          // Does this control expose an interface for its own HotZones?
          if (ag != null)
            ag.AddHotZones(this, m_hotZones);
        }
      }

      // Grab the screen rectangle of the Control being processed
      Rectangle fullArea = m_outsideRect;

      // Create the rectangle for the hot area
      fullArea.Height = m_hotVectorFromEdge;

      // Create the rectangle for the insertion indicator
      Rectangle fillSize = new Rectangle(fullArea.X, fullArea.Y, fullArea.Width, vector);

      m_hotZones.Add(new HotZoneReposition(fullArea, fillSize, State.DockTop, false));

      // If performing our own InsideFill then do not dock at inner positions
      if (!m_dockingManager.InsideFill)
      {
        // Create the HotArea at the left side of the inner rectangle
        Rectangle innerHotArea = new Rectangle(m_insideRect.X, m_insideRect.Y, m_insideRect.Width, m_hotVectorBeforeControl);

        // Create the rectangle for the insertion indicator
        Rectangle innerNewSize = new Rectangle(innerHotArea.X, innerHotArea.Y, innerHotArea.Width, vector);

        // Create a HotZone for docking to the Left at the innermost position
        m_hotZones.Add(new HotZoneReposition(innerHotArea, innerNewSize, State.DockTop, true));
      }
    }

    protected void GenerateHotZonesForBottom(ArrayList bottomList, Rectangle formClient, int vector, int outerIndex)
    {
      foreach(Control c in bottomList)
      {	
        bool ignoreType = (c is AutoHidePanel);
            
        int controlIndex = m_container.Controls.IndexOf(c);

        if (!ignoreType && (controlIndex < outerIndex))
        {
          // Grab the screen rectangle of the Control being processed
          Rectangle hotArea = c.RectangleToScreen(c.ClientRectangle);

          // Create the rectangle for the hot area
          hotArea.Y = hotArea.Bottom - m_hotVectorBeforeControl;
          hotArea.Height = m_hotVectorBeforeControl;

          // Create the rectangle for the insertion indicator
          Rectangle newSize = new Rectangle(hotArea.X, hotArea.Bottom - vector, hotArea.Width, vector);
					
          hotArea.Y -= m_hotVectorFromEdge;

          // Create the new HotZone used to reposition a docking content/windowcontent
          m_hotZones.Add(new HotZoneReposition(hotArea, newSize, State.DockBottom, controlIndex));

          IHotZoneSource ag = c as IHotZoneSource;

          // Does this control expose an interface for its own HotZones?
          if (ag != null)
            ag.AddHotZones(this, m_hotZones);
        }
      }

      // Grab the screen rectangle of the Control being processed
      Rectangle fullArea = m_outsideRect;

      // Create the rectangle for the hot area
      fullArea.Y = fullArea.Bottom - m_hotVectorFromEdge;
      fullArea.Height = m_hotVectorFromEdge;

      // Create the rectangle for the insertion indicator
      Rectangle fillSize = new Rectangle(fullArea.X, fullArea.Bottom - vector, fullArea.Width, vector);

      m_hotZones.Add(new HotZoneReposition(fullArea, fillSize, State.DockBottom, false));

      // If performing our own InsideFill then do not dock at inner positions
      if (!m_dockingManager.InsideFill)
      {
        // Create the HotArea at the bottom of the inner rectangle
        Rectangle innerHotArea = new Rectangle(m_insideRect.X, m_insideRect.Bottom - m_hotVectorBeforeControl, m_insideRect.Width, m_hotVectorBeforeControl);

        // Create the rectangle for the insertion indicator
        Rectangle innerNewSize = new Rectangle(innerHotArea.X, innerHotArea.Bottom - vector, innerHotArea.Width, vector);

        // Create a HotZone for docking to the Left at the innermost position
        m_hotZones.Add(new HotZoneReposition(innerHotArea, innerNewSize, State.DockBottom, true));
      }
    }

    protected void GenerateHotZonesForFill(ArrayList fillList, int outerIndex)
    {
      foreach(Control c in fillList)
      {	
        bool ignoreType = (c is AutoHidePanel);
            
        int controlIndex = m_container.Controls.IndexOf(c);

        if (controlIndex < outerIndex)
        {
          IHotZoneSource ag = c as IHotZoneSource;

          // Does this control expose an interface for its own HotZones?
          if (ag != null)
            ag.AddHotZones(this, m_hotZones);
        }
      }
    }

    protected void GenerateHotZonesForFloating(Size sourceSize)
    {
      ContainerControl main = m_dockingManager.Container;
            
      foreach(Form f in main.FindForm().OwnedForms)
      {
        // Cannot redock entire Floatin form onto itself
        if (f != m_floatingForm)
        {
          IHotZoneSource ag = f as IHotZoneSource;

          // Does this Form expose an interface for its own HotZones?
          if (ag != null)
            ag.AddHotZones(this, m_hotZones);
        }
      }
             
      // Applies to the entire desktop area
      Rectangle hotArea = new Rectangle(0, 0, 
        SystemInformation.MaxWindowTrackSize.Width,
        SystemInformation.MaxWindowTrackSize.Height);

      // Position is determined by HotZone dynamically but the size is defined now
      Rectangle newSize = new Rectangle(0, 0, sourceSize.Width, sourceSize.Height);

      // Generate a catch all HotZone for floating a Content
      m_hotZones.Add(new HotZoneFloating(hotArea, newSize, m_offset, this)); 
    }

    protected void VectorDependsOnSourceAndState(out int vectorH, out int vectorV)
    {
      Size sourceSize = SizeDependsOnSource();

      switch(m_source)
      {
        case Source.FloatingForm:
          // Make sure the vector is the smaller of the two dimensions
          if (sourceSize.Width > sourceSize.Height)
            sourceSize.Width = sourceSize.Height;

          if (sourceSize.Height > sourceSize.Width)
            sourceSize.Height = sourceSize.Width;

          // Do not let the new vector extend beyond halfway
          if (sourceSize.Width > (m_container.Width / 2))
            sourceSize.Width = m_container.Width / 2;

          if (sourceSize.Height > (m_container.Height / 2))
            sourceSize.Height = m_container.Height / 2;
          break;
        case Source.WindowContent:
        case Source.ContentInsideWindow:
        switch(m_windowContent.State)
        {
          case State.DockLeft:
          case State.DockRight:
            vectorH = sourceSize.Width;
            vectorV = vectorH;
            return;
          case State.DockTop:
          case State.DockBottom:
            vectorH = sourceSize.Height;
            vectorV = vectorH;
            return;
        }
          break;
      }

      vectorV = sourceSize.Height;
      vectorH = sourceSize.Width;
    }

    protected Size SizeDependsOnSource()
    {
      switch(m_source)
      {
        case Source.WindowContent:
          return m_windowContent.Size;
        case Source.FloatingForm:
          return m_floatingForm.Size;
        case Source.RawContent:
        case Source.ContentInsideWindow:
        default:
          return m_content.DisplaySize;
      }
    }
  }
}

#region revision history

// $Log: RedockerContent.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/RedockerContent.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: RedockerContent.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

