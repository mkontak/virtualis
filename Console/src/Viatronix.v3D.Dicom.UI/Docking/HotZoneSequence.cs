// $Id: HotZoneSequence.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Common;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class HotZoneSequence : HotZone
  {
    // Instance fields
    protected int m_index;
    protected ZoneSequence m_zs;

    public HotZoneSequence(Rectangle hotArea, Rectangle newSize, ZoneSequence zs, int index)
      : base(hotArea, newSize)
    {
      m_index = index;
      m_zs = zs;
    }

    public override bool ApplyChange(Point screenPos, Redocker parent)
    {
      // We are only called from the RedockerContent class
      RedockerContent redock = parent as RedockerContent;

      DockingManager dockingManager = redock.DockingManager;

      bool becomeFloating = (m_zs.State == State.Floating);

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

          // Create a new Window to host Content
          Window w = dockingManager.CreateWindowForContent(redock.Content);

          // Add into Zone
          m_zs.Windows.Insert(m_index, w);
        }
          break;
        case RedockerContent.Source.WindowContent:
        {
          // Is the destination Zone in the Floating state?
          if (becomeFloating)
          {
            foreach(Content c in redock.WindowContent.Contents)
              c.ContentBecomesFloating();
          }
          else
          {
            if (redock.WindowContent.State == State.Floating)
            {
              foreach(Content c in redock.WindowContent.Contents)
                c.ContentLeavesFloating();
            }
          }

          // Check if the WindowContent source is in same Zone
          if (redock.WindowContent.ParentZone == m_zs)
          {
            // Find current position of source WindowContent
            int currPos = m_zs.Windows.IndexOf(redock.WindowContent);
                            
            // If current window is before the new position then the current 
            // window will disappear before the new one is inserted,so need to 
            // adjust down the new insertion point
            if (currPos < m_index)
              m_index--;
          }

          // Create a new Window to host Content
          WindowContent wc = dockingManager.CreateWindowForContent(null) as WindowContent;

          // Transfer content across
          int count = redock.WindowContent.Contents.Count;
                        
          for(int index=0; index<count; index++)
          {
            Content c = redock.WindowContent.Contents[0];

            // Remove from existing location                            
            redock.WindowContent.Contents.RemoveAt(0);

            // Add into new WindowContent host
            wc.Contents.Add(c);  
          }

          // Add into host into Zone
          m_zs.Windows.Insert(m_index, wc);
        }
          break;
        case RedockerContent.Source.ContentInsideWindow:
        {
          // Perform State specific Restore actions
          if (becomeFloating)
            redock.Content.ContentBecomesFloating();
          else
          {
            if (redock.Content.ParentWindowContent.State == State.Floating)
              redock.Content.ContentLeavesFloating();
          }

          // Remove Content from existing WindowContent
          if (redock.Content.ParentWindowContent != null)
          {
            // Will removing the Content cause the WindowContent to die?
            if (redock.Content.ParentWindowContent.Contents.Count == 1)
            {
              // Check if the WindowContent source is in same Zone
              if (redock.Content.ParentWindowContent.ParentZone == m_zs)
              {
                // Find current position of source WindowContent
                int currPos = m_zs.Windows.IndexOf(redock.Content.ParentWindowContent);
                            
                // If current window is before the new position then the current 
                // window will disappear before the new one is inserted,so need to 
                // adjust down the new insertion point
                if (currPos < m_index)
                  m_index--;
              }
            }

            redock.Content.ParentWindowContent.Contents.Remove(redock.Content);
          }
    				
          // Create a new Window to host Content
          Window w = dockingManager.CreateWindowForContent(redock.Content);

          // Add into Zone
          m_zs.Windows.Insert(m_index, w);
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

          int count = redock.FloatingForm.Zone.Windows.Count;
                        
          for(int index=count-1; index>=0; index--)
          {
            // Remember the Window reference
            Window w = redock.FloatingForm.Zone.Windows[index];
                        
            // Remove from floating collection
            redock.FloatingForm.Zone.Windows.RemoveAt(index);

            // Add into new ZoneSequence destination
            m_zs.Windows.Insert(m_index, w);
          }
        }
          break;
      }

      dockingManager.UpdateInsideFill();

      // Reduce flicker during transition
      dockingManager.Container.ResumeLayout();

      return true;
    }
  }
}

#region revision history

// $Log: HotZoneSequence.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/HotZoneSequence.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: HotZoneSequence.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

