// $Id: Target.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public class Target
  {
    public enum TargetActions
    {
      Transfer,
      GroupLeft,
      GroupRight,
      GroupTop,
      GroupBottom
    }
	    
    // Instance fields
    protected Rectangle m_hotRect;
    protected Rectangle m_drawRect;
    protected TabGroupLeaf m_leaf;
    protected TargetActions m_action;

    public Target(Rectangle hotRect, Rectangle drawRect, TabGroupLeaf leaf, TargetActions action)
    {
      // Define state
      m_hotRect = hotRect;
      m_drawRect = drawRect;
      m_leaf = leaf;
      m_action = action;
    }

    public Rectangle HotRect
    {
      get { return m_hotRect; }
    }	    
        
    public Rectangle DrawRect
    {
      get { return m_drawRect; }
    }	    

    public TabGroupLeaf Leaf
    {
      get { return m_leaf; }
    }

    public TargetActions Action
    {
      get { return m_action; }
    }
  }
	
  public class TargetManager
  {
    // Class fields
    protected const int m_rectWidth = 4;
    protected static Cursor m_validCursor;
    protected static Cursor m_invalidCursor;
	
    // Instance fields
    protected TabbedGroups m_host;
    protected TabGroupLeaf m_leaf;
    protected Controls.TabControl m_source;
    protected Target m_lastTarget;
    protected TargetCollection m_targets;
	    
    static TargetManager()
    {
      m_validCursor = ResourceHelper.LoadCursor(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.TabbedValid.cur");

      m_invalidCursor = ResourceHelper.LoadCursor(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.TabbedInvalid.cur");
    }
	    
    public TargetManager(TabbedGroups host, TabGroupLeaf leaf, Controls.TabControl source)
    {
      // Define state
      m_host = host;
      m_leaf = leaf;
      m_source = source;
      m_lastTarget = null;
	    
      // Create collection to hold generated targets
      m_targets = new TargetCollection();
	        
      // Process each potential leaf in turn
      TabGroupLeaf tgl = host.FirstLeaf();
	        
      while(tgl != null)
      {
        // Create all possible targets for this leaf
        CreateTargets(tgl);
	        
        // Enumerate all leafs
        tgl = host.NextLeaf(tgl);
      }
    }
	    
    protected void CreateTargets(TabGroupLeaf leaf)
    {
      // Grab the underlying tab control
      Controls.TabControl tc = leaf.GroupControl as Controls.TabControl;

      // Get the total size of the tab control itself in screen coordinates
      Rectangle totalSize = tc.RectangleToScreen(tc.ClientRectangle);

      // We do not allow a page to be transfered to its own leaf!
      if (leaf != m_leaf)
      {
        Rectangle tabsSize = tc.RectangleToScreen(tc.TabsAreaRect);

        // Give priority to the tabs area being used to transfer page
        m_targets.Add(new Target(tabsSize, totalSize, leaf, Target.TargetActions.Transfer));
      }
	        
      // Can only create new groups if moving relative to a new group 
      // or we have more than one page in the originating group
      if ((leaf != m_leaf) || ((leaf == m_leaf) && m_leaf.TabPages.Count > 1))
      {
        int horzThird = totalSize.Width / 3;
        int vertThird = totalSize.Height / 3;
	        
        // Create the four spacing rectangle
        Rectangle leftRect = new Rectangle(totalSize.X, totalSize.Y, horzThird, totalSize.Height);
        Rectangle rightRect = new Rectangle(totalSize.Right - horzThird, totalSize.Y, horzThird, totalSize.Height);
        Rectangle topRect = new Rectangle(totalSize.X, totalSize.Y, totalSize.Width, vertThird);
        Rectangle bottomRect = new Rectangle(totalSize.X, totalSize.Bottom - vertThird, totalSize.Width, vertThird);

        TabGroupSequence tgs = m_leaf.Parent as TabGroupSequence;

        // Can only create new groups in same direction, unless this is the only leaf
        if (tgs.Count <= 1)
        {
          // Add each new target
          m_targets.Add(new Target(leftRect, leftRect, leaf, Target.TargetActions.GroupLeft));
          m_targets.Add(new Target(rightRect, rightRect, leaf, Target.TargetActions.GroupRight));
          m_targets.Add(new Target(topRect, topRect, leaf, Target.TargetActions.GroupTop));
          m_targets.Add(new Target(bottomRect, bottomRect, leaf, Target.TargetActions.GroupBottom));
        }
        else
        {
          if (tgs.Direction == Direction.Vertical)
          {
            m_targets.Add(new Target(topRect, topRect, leaf, Target.TargetActions.GroupTop));
            m_targets.Add(new Target(bottomRect, bottomRect, leaf, Target.TargetActions.GroupBottom));
          }
          else
          {
            m_targets.Add(new Target(leftRect, leftRect, leaf, Target.TargetActions.GroupLeft));
            m_targets.Add(new Target(rightRect, rightRect, leaf, Target.TargetActions.GroupRight));
          }
        }
      }
	        
      // We do not allow a page to be transfered to its own leaf!
      if (leaf != m_leaf)
      {
        // Any remaining space is used to 
        m_targets.Add(new Target(totalSize, totalSize, leaf, Target.TargetActions.Transfer));
      }
    }
	    
    public void MouseMove(Point mousePos)
    {
      // Find the Target the mouse is currently over (if any)
      Target t = m_targets.Contains(mousePos);
	    
      // Set appropriate cursor
      if (t != null)
        m_source.Cursor = m_validCursor;
      else
        m_source.Cursor = m_invalidCursor;
                
      if (t != m_lastTarget)
      {
        // Remove the old indicator
        if (m_lastTarget != null)
          DrawHelper.DrawDragRectangle(m_lastTarget.DrawRect, m_rectWidth);
                
        // Draw the new indicator
        if (t != null)
          DrawHelper.DrawDragRectangle(t.DrawRect, m_rectWidth);
                
        // Remember for next time around
        m_lastTarget = t;
      }
    }
        
    public void Exit()
    {
      // Remove any showing indicator
      Quit();

      if (m_lastTarget != null)
      {
        // Perform action specific operation
        switch(m_lastTarget.Action)
        {
          case Target.TargetActions.Transfer:
            // Transfer selecte page from source to destination
            m_leaf.MovePageToLeaf(m_lastTarget.Leaf);
            break;
          case Target.TargetActions.GroupLeft:                        
            m_lastTarget.Leaf.NewHorizontalGroup(m_leaf, true);
            break;
          case Target.TargetActions.GroupRight:
            m_lastTarget.Leaf.NewHorizontalGroup(m_leaf, false);
            break;
          case Target.TargetActions.GroupTop:
            m_lastTarget.Leaf.NewVerticalGroup(m_leaf, true);
            break;
          case Target.TargetActions.GroupBottom:
            m_lastTarget.Leaf.NewVerticalGroup(m_leaf, false);
            break;
        }
      }
    }
            
    public void Quit()
    {
      // Remove drawing of any indicator
      if (m_lastTarget != null)
        DrawHelper.DrawDragRectangle(m_lastTarget.DrawRect, m_rectWidth);
    }
        
    public static void DrawDragRectangle(Rectangle rect)
    {
      DrawHelper.DrawDragRectangle(rect, m_rectWidth);
    }
  }
}


#region revision history

// $Log: Target.cs,v $
// Revision 1.1  2005/10/11 14:41:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabbedGroups/Target.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
// $Id: Target.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $

#endregion

