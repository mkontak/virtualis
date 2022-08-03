// $Id: HotZoneTabbed.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Common;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class HotZoneTabbed : HotZone
  {
    // Class constants
    protected static int m_tabPageLeft = 9;
    protected static int m_tabPageHeight = 25;
    protected static int m_tabPageWidth= 45;

    // Instance fields
    protected bool m_itself;
    protected Rectangle m_tabRect;
    protected Rectangle m_tabRectTL;
    protected Rectangle m_tabRectTR;
    protected WindowContentTabbed m_wct;

    public HotZoneTabbed(Rectangle hotArea, Rectangle newSize, WindowContentTabbed wct, bool itself)
      : base(hotArea, newSize)
    {
      // Remember state
      m_wct = wct;
      m_itself = itself;

      // Instead of a single rectangle for the dragging indicator we want to provide
      // two rectangles. One for the main area and another to show a tab extending
      // below it. This ensures the user can tell that it will be added as a new tab
      // page of the control.

      int tabHeight = m_tabPageHeight;

      // Make sure the tab rectangle does not extend past end of control
      if (newSize.Height < (tabHeight + m_dragWidth))
        tabHeight = newSize.Height -  m_dragWidth * 3;

      // Create the tab page extension
      m_tabRect = new Rectangle(newSize.X + m_tabPageLeft,
        newSize.Bottom - tabHeight,
        m_tabPageWidth,
        tabHeight);

      // Make sure tab rectangle does not draw off right side of control
      if (m_tabRect.Right > newSize.Right)
        m_tabRect.Width -= m_tabRect.Right - newSize.Right;

      // We want the intersection between the top left and top right corners to be displayed
      m_tabRectTL = new Rectangle(m_tabRect.X, m_tabRect.Y, m_dragWidth, m_dragWidth);
      m_tabRectTR = new Rectangle(m_tabRect.Right - m_dragWidth, m_tabRect.Y, m_dragWidth, m_dragWidth);

      // Reduce the main area by the height of the above item
      m_newSize.Height -= tabHeight - m_dragWidth;
    }

    public override bool ApplyChange(Point screenPos, Redocker parent)
    {
      // If docking back to itself then refuse to apply the change, this will cause the
      // WindowContentTabbed object to put back the content which is the desired effect
      if (m_itself)
        return false;

      // We are only called from the RedockerContent class
      RedockerContent redock = parent as RedockerContent;

      DockingManager dockingManager = redock.DockingManager;

      bool becomeFloating = (m_wct.ParentZone.State == State.Floating);

      // Reduce flicker during transition
      dockingManager.Container.SuspendLayout();

      // Manageing Zones should remove display AutoHide windows
      dockingManager.RemoveShowingAutoHideWindows();

      switch(redock.DockingSource)
      {
        case RedockerContent.Source.RawContent:
        {
          // Perform State specific Restore actions
          if (becomeFloating)
            redock.Content.ContentBecomesFloating();

          m_wct.Contents.Add(redock.Content);
        }
          break;
        case RedockerContent.Source.WindowContent:
        {
          // Perform State specific Restore actions
          if (becomeFloating)
          {
            foreach(Content c in redock.WindowContent.Contents)
              c.ContentBecomesFloating();
          }
          else
          {
            // If the source is leaving the Floating state then need to record Restore positions
            if (redock.WindowContent.State == State.Floating)
            {
              foreach(Content c in redock.WindowContent.Contents)
                c.ContentLeavesFloating();
            }
          }

          int count = redock.WindowContent.Contents.Count;

          for(int index=0; index<count; index++)
          {
            Content c = redock.WindowContent.Contents[0];

            // Remove Content from previous WindowContent
            redock.WindowContent.Contents.RemoveAt(0);

            // Add into new WindowContent
            m_wct.Contents.Add(c);
          }
        }
          break;
        case RedockerContent.Source.ContentInsideWindow:
        {
          // Perform State specific Restore actions
          if (becomeFloating)
            redock.Content.ContentBecomesFloating();
          else
          {
            // If the source is leaving the Floating state then need to record Restore position
            if (redock.Content.ParentWindowContent.State == State.Floating)
              redock.Content.ContentLeavesFloating();
          }

          // Remove Content from existing WindowContent
          if (redock.Content.ParentWindowContent != null)
            redock.Content.ParentWindowContent.Contents.Remove(redock.Content);

          m_wct.Contents.Add(redock.Content);
        }
          break;
        case RedockerContent.Source.FloatingForm:
        {
          // Perform State specific Restore actions
          if (!becomeFloating)
          {
            // Make every Content object in the Floating Zone 
            // record its current state as the Floating state 
            redock.FloatingForm.ExitFloating();
          }

          int wCount = redock.FloatingForm.Zone.Windows.Count;
                    
          for(int wIndex=0; wIndex<wCount; wIndex++)
          {
            WindowContent wc = redock.FloatingForm.Zone.Windows[0] as WindowContent;

            if (wc != null)
            {
              int cCount = wc.Contents.Count;
                            
              for(int cIndex=0; cIndex<cCount; cIndex++)
              {
                // Get reference to first content in collection
                Content c = wc.Contents[0];

                // Remove from old WindowContent
                wc.Contents.RemoveAt(0);

                // Add into new WindowContentTabbed
                m_wct.Contents.Add(c);
              }
            }
          }
        }
          break;
      }

      dockingManager.UpdateInsideFill();

      // Reduce flicker during transition
      dockingManager.Container.ResumeLayout();

      return true;
    }

    public override void DrawReversible(Rectangle rect)
    {
      DrawHelper.DrawDragRectangles(new Rectangle[]{rect, m_tabRect, m_tabRectTL, m_tabRectTR}, m_dragWidth);
    }
  }
}

#region revision history

// $Log: HotZoneTabbed.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/HotZoneTabbed.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: HotZoneTabbed.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

