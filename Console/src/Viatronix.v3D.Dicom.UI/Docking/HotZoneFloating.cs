// $Id: HotZoneFloating.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
  public class HotZoneFloating : HotZone
  {
    // Instance fields
    protected Point m_offset;
    protected Point m_drawPos;
    protected Rectangle m_drawRect;
    protected RedockerContent m_redocker;

    public HotZoneFloating(Rectangle hotArea, Rectangle newSize, Point offset, RedockerContent redocker)
      : base(hotArea, newSize)
    {
      // Store initial state
      m_offset = offset;
      m_redocker = redocker;

      Size floatSize = CalculateFloatingSize();
      float widthPercentage = (float)floatSize.Width / (float)m_newSize.Width;
      float heightPercentage = (float)floatSize.Height / (float)m_newSize.Height;

      m_newSize.Width = floatSize.Width;
      m_newSize.Height = floatSize.Height + SystemInformation.ToolWindowCaptionHeight;
            
      m_offset.X = (int)((float) m_offset.X * widthPercentage);
      m_offset.Y = (int)((float) m_offset.Y * heightPercentage);

      // We do not want the indicator to be too far away from the cursor, so limit check the offset
      if (m_offset.X > newSize.Width)
        m_offset.X = newSize.Width;

      if (m_offset.Y > newSize.Height)
        m_offset.Y = newSize.Height;
    }

    public override bool ApplyChange(Point screenPos, Redocker parent)
    {
      // Should always be the appropriate type
      RedockerContent redock = parent as RedockerContent;
        
      DockingManager dockingManager = redock.DockingManager;

      Zone newZone = null;
            
      // Manageing Zones should remove display AutoHide windows
      dockingManager.RemoveShowingAutoHideWindows();

      switch(redock.DockingSource)
      {
        case RedockerContent.Source.RawContent:
        {
          // Perform State specific Restore actions
          redock.Content.ContentBecomesFloating();

          // Create a new Window to host Content
          Window w = dockingManager.CreateWindowForContent(redock.Content);

          // We need to create a Zone for containing the transfered content
          newZone = dockingManager.CreateZoneForContent(State.Floating);
                        
          // Add into Zone
          newZone.Windows.Add(w);
        }
          break;
        case RedockerContent.Source.WindowContent:
          // Perform State specific Restore actions
          foreach(Content c in redock.WindowContent.Contents)
            c.ContentBecomesFloating();

          // Remove WindowContent from old Zone
          if (redock.WindowContent.ParentZone != null)
            redock.WindowContent.ParentZone.Windows.Remove(redock.WindowContent);

          // We need to create a Zone for containing the transfered content
          newZone = dockingManager.CreateZoneForContent(State.Floating);
                    
          // Add into new Zone
          newZone.Windows.Add(redock.WindowContent);
          break;
        case RedockerContent.Source.ContentInsideWindow:
        {
          // Perform State specific Restore actions
          redock.Content.ContentBecomesFloating();

          // Remove Content from existing WindowContent
          if (redock.Content.ParentWindowContent != null)
            redock.Content.ParentWindowContent.Contents.Remove(redock.Content);
    				
          // Create a new Window to host Content
          Window w = dockingManager.CreateWindowForContent(redock.Content);

          // We need to create a Zone for containing the transfered content
          newZone = dockingManager.CreateZoneForContent(State.Floating);
                        
          // Add into Zone
          newZone.Windows.Add(w);
        }
          break;
        case RedockerContent.Source.FloatingForm:
          redock.FloatingForm.Location = new Point(screenPos.X - m_offset.X,
            screenPos.Y - m_offset.Y);

          return false;
      }
        
      dockingManager.UpdateInsideFill();

      // Create a new floating form
      FloatingForm floating = new FloatingForm(redock.DockingManager, newZone,
        new ContextHandler(dockingManager.OnShowContextMenu));

      // Find screen location/size            
      m_drawRect = new Rectangle(screenPos.X, screenPos.Y, m_newSize.Width, m_newSize.Height);

      // Adjust for mouse starting position relative to source control
      m_drawRect.X -= m_offset.X;
      m_drawRect.Y -= m_offset.Y;

      // Define its location/size
      floating.Location = new Point(m_drawRect.Left, m_drawRect.Top);
      floating.Size = new Size(m_drawRect.Width, m_drawRect.Height);

      // Show it!
      floating.Show();

      return true;
    }

    public override void UpdateForMousePosition(Point screenPos, Redocker parent)
    {
      // Remember the current mouse pos
      Point newPos = screenPos;

      // Calculate the new drawing rectangle
      Rectangle newRect = new Rectangle(newPos.X, newPos.Y, m_newSize.Width, m_newSize.Height);

      // Adjust for mouse starting position relative to source control
      newRect.X -= m_offset.X;
      newRect.Y -= m_offset.Y;

      // Draw both the old rectangle and the new one, that will remove flicker as the
      // draw method will only actually draw areas that differ between the two rectangles
      DrawHelper.DrawDragRectangles(new Rectangle[]{m_drawRect,newRect}, m_dragWidth);

      // Remember new values
      m_drawPos = newPos;
      m_drawRect = newRect;
    }

    public override void DrawIndicator(Point mousePos) 
    {
      // Remember the current mouse pos
      m_drawPos = mousePos;

      // Calculate the new drawing rectangle
      m_drawRect = new Rectangle(m_drawPos.X, m_drawPos.Y, m_newSize.Width, m_newSize.Height);

      // Adjust for mouse starting position relative to source control
      m_drawRect.X -= m_offset.X;
      m_drawRect.Y -= m_offset.Y;

      DrawReversible(m_drawRect);
    }
		
    public override void RemoveIndicator(Point mousePos) 
    {			
      DrawReversible(m_drawRect);
    }
        
    protected Size CalculateFloatingSize()
    {
      Size floatingSize = new Size(0,0);

      // Get specific redocker type
      RedockerContent redock = m_redocker as RedockerContent;

      switch(redock.DockingSource)
      {
        case RedockerContent.Source.RawContent:
          // Whole Form is size requested by single Content
          floatingSize = redock.Content.FloatingSize;
          break;
        case RedockerContent.Source.WindowContent:
          // Find the largest requested floating size
          foreach(Content c in redock.WindowContent.Contents)
          {
            if (c.FloatingSize.Width > floatingSize.Width)
              floatingSize.Width = c.FloatingSize.Width;
                    
            if (c.FloatingSize.Height > floatingSize.Height)
              floatingSize.Height = c.FloatingSize.Height;
          }

          // Apply same size to all Content objects
          foreach(Content c in redock.WindowContent.Contents)
            c.FloatingSize = floatingSize;
          break;
        case RedockerContent.Source.ContentInsideWindow:
          // Whole Form is size requested by single Content
          floatingSize = redock.Content.FloatingSize;
          break;
        case RedockerContent.Source.FloatingForm:
          // Use the requested size
          floatingSize.Width = m_newSize.Width;
          floatingSize.Height = m_newSize.Height;
          break;
      }

      return floatingSize;
    }
  }
}


#region revision history

// $Log: HotZoneFloating.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/HotZoneFloating.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: HotZoneFloating.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

