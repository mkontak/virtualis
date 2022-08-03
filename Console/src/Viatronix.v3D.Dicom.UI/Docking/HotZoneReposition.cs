// $Id: HotZoneReposition.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
  public class HotZoneReposition : HotZone
  {
    protected enum Position
    {
      Inner,
      Index,
      Outer
    }

    // Instance fields
    protected int m_newIndex;
    protected Position m_position;
    protected State m_state;

    public HotZoneReposition(Rectangle hotArea, Rectangle newSize, State state, bool inner)
      : base(hotArea, newSize)
    {
      // Minus one means create at the innermost position
      InternalConstruct((inner ? Position.Inner : Position.Outer), state, -1);
    }

    public HotZoneReposition(Rectangle hotArea, Rectangle newSize, State state, int newIndex)
      : base(hotArea, newSize)
    {
      InternalConstruct(Position.Index, state, newIndex);
    }

    protected void InternalConstruct(Position position, State state, int newIndex)
    {
      // Store initial state
      m_position = position;
      m_newIndex = newIndex;
      m_state = state;
    }

    public override bool ApplyChange(Point screenPos, Redocker parent)
    {
      // We are only called from the RedockerContent class
      RedockerContent redock = parent as RedockerContent;

      DockingManager dockingManager = redock.DockingManager;

      // Reduce flicker during transition
      dockingManager.Container.SuspendLayout();

      // Need to create a new Zone
      Zone zone;
            
      if (redock.DockingSource == RedockerContent.Source.FloatingForm)
      {
        // Make every Content object in the Floating Zone 
        // record its current state as the Floating state 
        redock.FloatingForm.ExitFloating();

        zone = redock.FloatingForm.Zone;
      }
      else
        zone = dockingManager.CreateZoneForContent(m_state);

      // Insert Zone at end of Controls collection
      dockingManager.Container.Controls.Add(zone);

      // Adjust ordering
      switch(m_position)
      {
        case Position.Inner:
          dockingManager.ReorderZoneToInnerMost(zone);
          break;
        case Position.Index:
          // Manageing Zones should remove display AutoHide windows
          dockingManager.RemoveShowingAutoHideWindows();
                
          // Place new Zone AFTER the one given, so need to increase index by one
          dockingManager.Container.Controls.SetChildIndex(zone, m_newIndex + 1);
          break;
        case Position.Outer:
          dockingManager.ReorderZoneToOuterMost(zone);
          break;
      }

      switch(redock.DockingSource)
      {
        case RedockerContent.Source.RawContent:
        {
          // Create a new Window to host Content
          Window w = dockingManager.CreateWindowForContent(redock.Content);
                        
          // Add into Zone
          zone.Windows.Add(w);
        }
          break;
        case RedockerContent.Source.WindowContent:
          // Remove WindowContent from old Zone
          if (redock.WindowContent.ParentZone != null)
          {
            // If the source is leaving the Floating state then need to record Restore positions
            if (redock.WindowContent.State == State.Floating)
            {
              foreach(Content c in redock.WindowContent.Contents)
                c.ContentLeavesFloating();
            }
                        
            redock.WindowContent.ParentZone.Windows.Remove(redock.WindowContent);
          }

          // Add into new Zone
          zone.Windows.Add(redock.WindowContent);
          break;
        case RedockerContent.Source.ContentInsideWindow:
        {
          // Remove Content from existing WindowContent
          if (redock.Content.ParentWindowContent != null)
          {
            // If the source is leaving the Floating state then need to record Restore position
            if (redock.Content.ParentWindowContent.State == State.Floating)
              redock.Content.ContentLeavesFloating();

            redock.Content.ParentWindowContent.Contents.Remove(redock.Content);
          }
    				
          // Create a new WindowContent to host Content
          Window w = dockingManager.CreateWindowForContent(redock.Content);

          // Add into Zone
          zone.Windows.Add(w);
        }
          break;
        case RedockerContent.Source.FloatingForm:
          DockStyle ds;
          Direction direction;
                
          dockingManager.ValuesFromState(m_state, out ds, out direction);

          // Define correct docking style to match state
          zone.Dock = ds;
                    
          ZoneSequence zs = zone as ZoneSequence;
                    
          // Define correct display direction to match state
          if (zs != null)
            zs.Direction = direction;

          // Ensure the Zone recalculates contents according to new state
          zone.State = m_state;
          break;
      }

      // Define correct size of the new Zone
      switch(m_state)
      {
        case State.DockLeft:
        case State.DockRight:
          zone.Width = m_newSize.Width;
          break;
        case State.DockTop:
        case State.DockBottom:
          zone.Height = m_newSize.Height;
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

// $Log: HotZoneReposition.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/HotZoneReposition.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: HotZoneReposition.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

