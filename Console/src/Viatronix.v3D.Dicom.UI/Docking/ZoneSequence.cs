// $Id: ZoneSequence.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  [ToolboxItem(false)]
  public class ZoneSequence : Zone, IHotZoneSource, IZoneMaximizeWindow, IResizeSource
  {
    protected struct Position
    {
      public int length;
    }

    // Class constants
    protected const int m_spacePrecision = 3;
    protected const int m_hotVectorBeforeControl = 5;

    // Instance fields
    protected VisualStyle m_style;
    protected Direction m_direction;
    protected ResizeBar m_resizeBar;
    protected Window m_maximizedWindow;
    protected bool m_suppressReposition;
    protected bool m_zoneMinMax;
	
    // Instance events
    public event EventHandler RefreshMaximize;

    public ZoneSequence(DockingManager manager)
      : base(manager)
    {
      InternalConstruct(VisualStyle.IDE, Direction.Vertical, true);
    }

    public ZoneSequence(DockingManager manager, State state, VisualStyle style, Direction direction, bool zoneMinMax)
      : base(manager, state)
    {
      InternalConstruct(style, direction, zoneMinMax);
    }

    protected void InternalConstruct(VisualStyle style, Direction direction, bool zoneMinMax)
    {
      // Remember initial state
      m_style = style;
      m_direction = direction;
      m_maximizedWindow = null;
      m_suppressReposition = false;
      m_zoneMinMax = zoneMinMax;

      // Create the control used to resize the whole Zone
      m_resizeBar = new ResizeBar(m_direction, this);

      // Place last in the list of child Controls
      Controls.Add(m_resizeBar);

      // Start of very small and let first content determine new size
      this.Size = new Size(0,0);		

      // Do not inherit the parent BackColor, we want the .Control color as 
      // this blends in with the way all the docking windows are drawn
      this.BackColor = SystemColors.Control;
      this.ForeColor = SystemColors.ControlText;
    }

    public override int MinimumWidth 
    { 
      get { return m_resizeBar.Width * 5; }
    }
		
    public override int MinimumHeight
    { 
      get { return m_resizeBar.Height * 6; } 
    }

    public override DockStyle Dock
    {
      get { return base.Dock; }

      set
      {
        base.Dock = value;

        RepositionControls();
      }
    }

    public Direction Direction
    {
      get { return m_direction; }

      set
      {
        if (m_direction != value)
          m_direction = value;
      }
    }

    public override State State
    {
      set 
      {
        base.State = value;

        // Inform each Window of the new requried state
        foreach(Window w in m_windows)
          w.State = value;

        RepositionControls();
      }
    }

    public void SuppressReposition()
    {
      m_suppressReposition = true;
    }

    public bool IsMaximizeAvailable()
    {
      return (m_windows.Count > 1) && m_zoneMinMax;
    }

    public bool IsWindowMaximized(Window w)
    {
      return (w == m_maximizedWindow);
    }

    public void MaximizeWindow(Window w)
    {
      // Remember the newly maximized Window
      m_maximizedWindow = w;

      // Inform all interested parties of change
      OnRefreshMaximize(EventArgs.Empty);

      RepositionControls();
    }

    public void RestoreWindow()
    {
      // Remember the newly maximized Window
      m_maximizedWindow = null;

      // Inform all interested parties of change
      OnRefreshMaximize(EventArgs.Empty);

      RepositionControls();
    }

    public virtual void OnRefreshMaximize(EventArgs e)
    {
      // Any attached event handlers?
      if (RefreshMaximize != null)
        RefreshMaximize(this, e);
    }

    public Color ResizeBarColor
    {
      get { return m_manager.ResizeBarColor; }
    }
            
    public int ResizeBarVector
    {
      get { return m_manager.ResizeBarVector; }
    }
            
    public VisualStyle Style 
    { 
      get { return m_manager.Style; }
    }

    public Color BackgroundColor
    {
      get { return m_manager.BackColor; }
    }

    public bool CanResize(ResizeBar bar)
    {
      // Is this the Window resize bar?
      if (bar != m_resizeBar)
      {
        // Find position of this ResizeBar in the Controls collection
        int barIndex = Controls.IndexOf(bar);

        // The Window before this bar must be the one before it in the collection
        Window wBefore = Controls[barIndex - 1] as Window;

        // The Window after this bar must be the one after it in the Window collection
        Window wAfter = m_windows[m_windows.IndexOf(wBefore) + 1];

        // If Windows either side of the bar have no space then cannot resize there relative positions
        if (((wBefore.ZoneArea <= 0m) && (wAfter.ZoneArea <= 0m)))
          return false;

        // If in maximized state and the bar is not connected to the maximized window
        if ((m_maximizedWindow != null) && (m_maximizedWindow != wBefore) && (m_maximizedWindow != wAfter))
          return false;
      }

      return true;
    }

    public bool StartResizeOperation(ResizeBar bar, ref Rectangle screenBoundary)
    {
      // Is this the Zone level resize bar?
      if (bar == m_resizeBar)
      {
        // Define resize boundary as the inner area of the Form containing the Zone
        screenBoundary = this.Parent.RectangleToScreen(m_manager.InnerResizeRectangle(this));

        // Find the screen limits of this Zone
        Rectangle zoneBoundary = RectangleToScreen(this.ClientRectangle);

        int minHeight = this.MinimumHeight;
        int minWidth = this.MinimumWidth;

        // Restrict resize based on which edge we are attached against
        switch(m_state)
        {
          case State.DockTop:
          {
            // Restrict Zone being made smaller than its minimum height
            int diff = zoneBoundary.Top - screenBoundary.Top + minHeight;
            screenBoundary.Y += diff;
            screenBoundary.Height -= diff;					

            // Restrict Zone from making inner control smaller than minimum allowed
            int innerMinimumWidth = m_manager.InnerMinimum.Height;
            screenBoundary.Height -= innerMinimumWidth;
          }
            break;
          case State.DockBottom:
          {
            // Restrict Zone being made smaller than its minimum height
            int diff = zoneBoundary.Bottom - screenBoundary.Bottom - minHeight;
            screenBoundary.Height += diff;					

            // Restrict Zone from making inner control smaller than minimum allowed
            int innerMinimumWidth = m_manager.InnerMinimum.Height;
            screenBoundary.Y += innerMinimumWidth;
            screenBoundary.Height -= innerMinimumWidth;
          }
            break;
          case State.DockLeft:
          {
            // Restrict Zone being made smaller than its minimum width
            int diff = zoneBoundary.Left - screenBoundary.Left + minWidth;
            screenBoundary.X += diff;
            screenBoundary.Width -= diff;					

            // Restrict Zone from making inner control smaller than minimum allowed
            int innerMinimumWidth = m_manager.InnerMinimum.Width;
            screenBoundary.Width -=	innerMinimumWidth;
          }
            break;
          case State.DockRight:
          {
            // Restrict Zone being made smaller than its minimum width
            int diff = zoneBoundary.Right - screenBoundary.Right - minWidth;
            screenBoundary.Width += diff;

            // Restrict Zone from making inner control smaller than minimum allowed
            int innerMinimumWidth = m_manager.InnerMinimum.Width;
            screenBoundary.X += innerMinimumWidth;
            screenBoundary.Width -=	innerMinimumWidth;
          }
            break;
        }
      }
      else
      {
        // Find position of this ResizeBar in the Controls collection
        int barIndex = Controls.IndexOf(bar);

        // Define resize boundary as the client area of the Zone
        screenBoundary = RectangleToScreen(this.ClientRectangle);

        // The Window before this bar must be the one before it in the collection
        Window wBefore = Controls[barIndex - 1] as Window;

        // The Window after this bar must be the one after it in the Window collection
        Window wAfter = m_windows[m_windows.IndexOf(wBefore) + 1];
				
        // Find screen rectangle for the Windows either side of the bar
        Rectangle rectBefore = wBefore.RectangleToScreen(wBefore.ClientRectangle);
        Rectangle rectAfter = wAfter.RectangleToScreen(wAfter.ClientRectangle);

        // Reduce the boundary in the appropriate direction
        if (m_direction == Direction.Vertical)
        {
          screenBoundary.Y = rectBefore.Y + wBefore.MinimalSize.Height;
          screenBoundary.Height -= screenBoundary.Bottom - rectAfter.Bottom;
          screenBoundary.Height -= wAfter.MinimalSize.Height;
        }
        else
        {
          screenBoundary.X = rectBefore.X + wBefore.MinimalSize.Width;
          screenBoundary.Width -= screenBoundary.Right - rectAfter.Right;
          screenBoundary.Width -= wAfter.MinimalSize.Width;
        }
      }

      // Allow resize operation to occur
      return true;
    }

    public void EndResizeOperation(ResizeBar bar, int delta)
    {
      // Is this the Zone level resize bar?
      if (bar == m_resizeBar)
      {
        switch(this.Dock)
        {
          case DockStyle.Left:
            this.Width += delta;
            break;
          case DockStyle.Right:
            this.Width -= delta;
            break;
          case DockStyle.Top:
            this.Height += delta;
            break;
          case DockStyle.Bottom:
            this.Height -= delta;
            break;
        }
      }
      else
      {
        // Find position of this ResizeBar in the Controls collection
        int barIndex = Controls.IndexOf(bar);

        // The Window relating to this bar must be the one before it in the collection
        Window w = Controls[barIndex - 1] as Window;

        // Is the Window being expanded
        ModifyWindowSpace(w, delta);
      }
    }

    public override Restore RecordRestore(Window w, object child, Restore childRestore)
    {
      Content c = child as Content;

      // We currently only understand Windows that have Content as children
      if (c != null)
      {
        StringCollection best;
        StringCollection next;
        StringCollection previous;

        GetWindowContentFriends(w, out best, out next, out previous); 

        // Create a restore object that will find the correct WindowContent to 
        // place a Content in within a specified Zone, or it will create a new 
        // WindowContent in an appropriate relative ordering
        Restore zoneRestore = new RestoreZoneAffinity(childRestore, c, best, next, previous); 

        if (m_state == State.Floating)
        {
          // Create a restore object to find the correct Floating Form to restore inside
          // or it will create a new Floating Form as appropriate
          return new RestoreContentFloatingAffinity(zoneRestore, m_state, c, best, ZoneHelper.ContentNames(this));
        }
        else
        {
          StringCollection zoneBest;
          StringCollection zoneNext;
          StringCollection zonePrevious;
          StringCollection zoneNextAll;
          StringCollection zonePreviousAll;

          GetZoneContentFriends(c, out zoneBest, out zoneNext, out zonePrevious, 
            out zoneNextAll, out zonePreviousAll); 

          // Create a restore object able to find the correct Zone in the appropriate 
          // docking direction and then restore into that Zone. If no appropriate Zone 
          // found then create a new one
          return new RestoreContentDockingAffinity(zoneRestore, m_state, c, zoneBest, 
            zoneNext, zonePrevious,
            zoneNextAll, zonePreviousAll);
        }
      }

      return null;
    }
	
    public override void PropogateNameValue(PropogateName name, object value)
    {
      base.PropogateNameValue(name, value);
            
      // Reduce flicker during update
      SuspendLayout();

      if (name == PropogateName.ZoneMinMax)
      {
        if (m_zoneMinMax != (bool)value)
        {
          // Remember the new value
          m_zoneMinMax = (bool)value;
                    
          // If turning off the min/max ability
          if (!m_zoneMinMax)
            m_maximizedWindow = null;  // no window can be currently maximized
        
          // Get child windows to retest the maximize capability
          OnRefreshMaximize(EventArgs.Empty);
        }           
      }
            
      // Update each resize bar control
      foreach(Control c in this.Controls)
      {
        ResizeBar rb = c as ResizeBar;
                
        if (rb != null)
          rb.PropogateNameValue(name, value);
      }
            
      // Recalculate positions using new values
      RepositionControls();
            
      ResumeLayout();
    }
    protected void GetZoneContentFriends(Content c,
      out StringCollection zoneBest,
      out StringCollection zoneNext,
      out StringCollection zonePrevious,
      out StringCollection zoneNextAll,
      out StringCollection zonePreviousAll)
    {

      // Out best friends are all those Content inside this Zone but with the ones
      // in the same Window as the first in list and so the highest priority
      zoneBest = ZoneHelper.ContentNamesInPriority(this,c);

      zoneNext = new StringCollection();
      zonePrevious = new StringCollection();
      zoneNextAll = new StringCollection();
      zonePreviousAll = new StringCollection();

      bool before = true;

      foreach(Control control in m_manager.Container.Controls)
      {
        Zone z = control as Zone;

        if (z != null)
        {
          if (z == this)
            before = false;
          else
          {
            ContentCollection newContent = ZoneHelper.Contents(z);

            foreach(Content content in newContent)
            {
              if (before)
              {
                if (z.State == this.State)
                  zonePrevious.Add(content.Title);

                zonePreviousAll.Add(content.Title);
              }
              else
              {
                if (z.State == this.State)
                  zoneNext.Add(content.Title);

                zoneNextAll.Add(content.Title);
              }
            }

            newContent.Clear();
          }
        }
      }
    }

    protected void GetWindowContentFriends(Window match, 
      out StringCollection best,
      out StringCollection next,
      out StringCollection previous)
    {
      best = new StringCollection();
      next = new StringCollection();
      previous = new StringCollection();

      bool before = true;

      foreach(Window w in m_windows)
      {
        WindowContent wc = w as WindowContent;

        // Is this the Window we are searching for?
        if (w == match)
        {
          if (wc != null)
          {
            // Best friends are those in the matching Window
            foreach(Content content in wc.Contents)
              best.Add(content.Title);
          }

          before = false;
        }
        else
        {
          if (wc != null)
          {
            // Remember all found Content in appropriate next/previous collection
            foreach(Content content in wc.Contents)
            {
              if (before)
                previous.Add(content.Title);
              else
                next.Add(content.Title);
            }
          }
        }
      }

    }
        
    public void AddHotZones(Redocker redock, HotZoneCollection collection)
    {
      RedockerContent redocker = redock as RedockerContent;

      // Allow all the Window objects a chance to add HotZones
      foreach(Window w in m_windows)
      {
        IHotZoneSource ag = w as IHotZoneSource;

        // Does this control expose an interface for its own HotZones?
        if (ag != null)
          ag.AddHotZones(redock, collection);
      }

      // Check for situations that need extra attention...
      //
      //  (1) Do not allow a WindowContent from a ZoneSequence to be redocked into the
      //      same ZoneSequence. As removing it will cause the Zone to be destroyed and
      //      so it cannot be added back again. Is not logical anyway.
      //
      //  (2) If the source is in this ZoneSequence we might need to adjust the insertion
      //      index because the item being removed will reduce the count for when the insert
      //		takes place.

      bool indexAdjustTest = false;
      WindowContent redockWC = redocker.WindowContent;

      if (m_windows.Count == 1)
      {
        if (redockWC != null)
          if (redockWC == m_windows[0])
            if ((redocker.Content == null) || (redockWC.Contents.Count == 1))
              return;
      }
      else
      {
        if (m_windows.Contains(redockWC))
        {
          if ((redocker.Content == null) || (redockWC.Contents.Count == 1))
            indexAdjustTest = true;
        }
      }

      // Find the Zone client area in screen coordinates
      Rectangle zoneArea = this.RectangleToScreen(this.ClientRectangle);

      int length;

      // Give a rough idea of the new window size
      if (m_direction == Direction.Vertical)
        length = zoneArea.Height / (m_windows.Count + 1);
      else
        length = zoneArea.Width / (m_windows.Count + 1);

      AddHotZoneWithIndex(collection, zoneArea, length, 0);

      int addative = 0;		
      int count = m_windows.Count;
			
      for(int index=1; index<count; index++)
      {
        // Grab the Window for this index
        WindowContent wc = m_windows[index] as WindowContent;

        if (indexAdjustTest)
        {	
          if (wc == redockWC)
            --addative;
        }

        AddHotZoneWithIndex(collection,
          wc.RectangleToScreen(wc.ClientRectangle), 
          length, 
          index + addative);
      }

      if (m_windows.Count > 0)
      {
        // Find hot area and new size for last docking position
        Rectangle lastArea = zoneArea;
        Rectangle lastSize = zoneArea;

        if (m_direction == Direction.Vertical)
        {
          lastArea.Y = lastArea.Bottom - m_hotVectorBeforeControl;
          lastArea.Height = m_hotVectorBeforeControl;
          lastSize.Y = lastSize.Bottom - length;
          lastSize.Height = length;
        }
        else
        {
          lastArea.X = lastArea.Right - m_hotVectorBeforeControl;
          lastArea.Width = m_hotVectorBeforeControl;
          lastSize.X = lastSize.Right - length;
          lastSize.Width = length;
        }

        collection.Add(new HotZoneSequence(lastArea, lastSize, this, m_windows.Count + addative));
      }
    }

    public void ModifyWindowSpace(Window w, Decimal newSpace)
    {
      // Double check this Window is a member of the collection
      if (m_windows.Contains(w))
      {
        // Cannot reallocate space if it is the only element
        if (m_windows.Count > 1)
        {
          int otherWindows = m_windows.Count - 1; 

          // Limit the resize allowed
          if (newSpace > 100m)
            newSpace = 100m;
						
          if (newSpace <= 0m)
            newSpace = 0m;		

          if (newSpace != w.ZoneArea)
          {
            // How much needs to be reallocated to other windows
            Decimal diff = w.ZoneArea - newSpace;					

            // Reducing the amount of space?
            if (diff > 0m)
            {
              // How much to give each of the other windows
              Decimal extra = diff / otherWindows;

              // Looping counters
              Decimal allocated = 0m;
              int found = 0;

              foreach(Window target in m_windows)
              {
                // We only process the other windows
                if (target != w)
                {
                  // Allocate it extra space
                  target.ZoneArea += extra;

                  // Keep count of total extra allocated
                  allocated += extra;

                  // Count number of others processed
                  found++;

                  // The last window to be allocated needs to also be given any rouding 
                  // errors that occur from previous division, to ensure that the total 
                  // space it always exactly equal to 100.
                  if (found == otherWindows)
                    target.ZoneArea += (diff - allocated);
                }
              }
            }
            else
            {
              // Easier to work with positive than negative numbers
              diff = -diff;

              while(diff > 0m)
              {
                // How much to grab from each of the other windows
                Decimal extra = diff / otherWindows;

                foreach(Window target in m_windows)
                {
                  // We only process the other windows
                  if (target != w)
                  {
                    if (target.ZoneArea > 0m)
                    {
                      if (target.ZoneArea < extra)
                      {
                        // Keep count of total left to grab
                        diff -= target.ZoneArea;
		
                        // Window no longer has any ZoneArea											
                        target.ZoneArea = 0m;
                      }
                      else
                      {
                        // Allocate it extra space
                        target.ZoneArea -= extra;

                        // Keep count of total left to grab
                        diff -= extra;
                      }
                    }
                  }
                }
              }
            }

            w.ZoneArea = newSpace;
          }
        }
      }

      // Recalculate the size and position of each Window and resize bar
      RepositionControls();
    }

    protected void AddHotZoneWithIndex(HotZoneCollection collection, Rectangle zoneArea, int length, int index)
    {
      // Find hot area and new size for first docking position
      Rectangle hotArea = zoneArea;
      Rectangle newSize = zoneArea;

      if (m_direction == Direction.Vertical)
      {
        hotArea.Height = m_hotVectorBeforeControl;
        newSize.Height = length;
      }
      else
      {
        hotArea.Width = m_hotVectorBeforeControl;
        newSize.Width = length;
      }

      collection.Add(new HotZoneSequence(hotArea, newSize, this, index));				
    }
		
    protected override void OnWindowsClearing()
    {
      base.OnWindowsClearing();

      // Make sure no Window is recorded as maximized
      m_maximizedWindow = null;

      // Remove all child controls
      Controls.Clear();

      if (!this.AutoDispose)
      {
        // Add back the Zone resize bar
        Controls.Add(m_resizeBar);

        Invalidate();
      }
    }

    protected override void OnWindowInserted(int index, object value)
    {
      base.OnWindowInserted(index, value);

      Window w = value as Window;

      // Is this the first Window entry?
      if (m_windows.Count == 1)
      {
        // Use size of the Window to determine our new size
        Size wSize = w.Size;

        // Adjust to account for the ResizeBar
        switch(this.Dock)
        {
          case DockStyle.Left:
          case DockStyle.Right:
            wSize.Width += m_resizeBar.Width;
            break;
          case DockStyle.Top:
          case DockStyle.Bottom:
            wSize.Height += m_resizeBar.Height;
            break;
        }

        this.Size = wSize;

        // Add the Window to the appearance
        Controls.Add(w);

        // Reposition to the start of the collection
        Controls.SetChildIndex(w, 0);
      }
      else
      {
        ResizeBar bar = new ResizeBar(m_direction, this);

        // Add the bar and Window
        Controls.Add(bar);
        Controls.Add(w);

        // Adding at start of collection?
        if (index == 0)
        {
          // Reposition the bar and Window to start of collection
          Controls.SetChildIndex(bar, 0);
          Controls.SetChildIndex(w, 0);
        }
        else
        {
          int	pos = index * 2 - 1;

          // Reposition the bar and Window to correct relative ordering
          Controls.SetChildIndex(bar, pos++);
          Controls.SetChildIndex(w, pos);
        }
      }

      // Allocate space for the new Window from other Windows
      AllocateWindowSpace(w);

      // Recalculate the size and position of each Window and resize bar
      RepositionControls();

      // Inform all interested parties of possible change in maximized state
      OnRefreshMaximize(EventArgs.Empty);
    }

    protected override void OnWindowRemoving(int index, object value)
    {
      base.OnWindowRemoving(index, value);

      Window w = value as Window;

      // If the Window being removed the maximized one?
      if (m_maximizedWindow == w)
        m_maximizedWindow = null;

      // Is this the only Window entry?
      if (m_windows.Count == 1)
      {
        // Remove Window from appearance

        // Use helper method to circumvent form Close bug
        ControlHelper.RemoveAt(this.Controls, 0);
      }
      else
      {
        int pos = 0;

        // Calculate position of Window to remove				
        if (index != 0)
          pos = index * 2 - 1;

        // Remove Window and bar 

        // Use helper method to circumvent form Close bug
        ControlHelper.RemoveAt(this.Controls, pos);
        ControlHelper.RemoveAt(this.Controls, pos);
      }

      // Redistribute space taken up by Window to other windows
      RemoveWindowSpace(w);
    }

    protected override void OnWindowRemoved(int index, object value)
    {
      base.OnWindowRemoved(index, value);

      // Recalculate the size and position of each Window and resize bar
      RepositionControls();

      // Inform all interested parties of possible change in maximized state
      OnRefreshMaximize(EventArgs.Empty);
    }

    protected void AllocateWindowSpace(Window w)
    {
      // Is this the only Window?
      if (m_windows.Count == 1)
      {
        // Give it all the space
        w.ZoneArea = 100m;
      }
      else
      {
        // Calculate how much space it should have
        Decimal newSpace = 100m / m_windows.Count;

        // How much space should we steal from each of the others
        Decimal reduceSpace = newSpace / (m_windows.Count - 1);

        // Actual space acquired
        Decimal allocatedSpace = 0m;

        foreach(Window entry in m_windows)
        {
          if (entry != w)
          {
            // How much space the entry currently has
            Decimal currentSpace = entry.ZoneArea;

            // How much space to steal from it
            Decimal xferSpace = reduceSpace;

            // Does it have at least the requested amount of space?
            if (currentSpace < xferSpace)
              xferSpace = currentSpace;

            // Transfer the space across
            currentSpace -= xferSpace;

            // Round the sensible number of decimal places
            currentSpace = Decimal.Round(currentSpace, m_spacePrecision);

            // Update window with new space allocation
            entry.ZoneArea = currentSpace;

            allocatedSpace += currentSpace;
          }
        }

        // Assign allocated space to new window
        w.ZoneArea = 100m - allocatedSpace;
      }
    }

    protected void ModifyWindowSpace(Window w, int vector)
    {
      // Remove any maximized state
      if (m_maximizedWindow != null)
      {
        // Make the maximized Window have all the space
        foreach(Window entry in m_windows)
        {
          if (entry == m_maximizedWindow)
            entry.ZoneArea = 100m;
          else
            entry.ZoneArea = 0m; 
        }

        // Remove maximized state
        m_maximizedWindow = null;

        // Inform all interested parties of change
        OnRefreshMaximize(EventArgs.Empty);
      }

      Rectangle clientRect = this.ClientRectangle;

      RepositionZoneBar(ref clientRect);

      // Space available for allocation
      int space;

      // New pixel length of the modified Window
      int newLength = vector;
			
      if (m_direction == Direction.Vertical)
      {
        space = clientRect.Height;

        // New pixel size is requested change plus original 
        // height minus the minimal size that is always added
        newLength += w.Height;
        newLength -= w.MinimalSize.Height;
      }
      else
      {
        space = clientRect.Width;

        // New pixel size is requested change plus original 
        // width minus the minimal size that is always added
        newLength += w.Width;
        newLength -= w.MinimalSize.Width;
      }

      int barSpace = 0;

      // Create temporary array of working values
      Position[] positions = new Position[Controls.Count - 1];

      // Pass 1, allocate all the space needed for each ResizeBar and the 
      //         minimal amount of space that each Window requests. 
      AllocateMandatorySizes(ref positions, ref barSpace, ref space);

      // What is the new percentage it needs?
      Decimal newPercent = 0m;

      // Is there any room to allow a percentage calculation
      if ((newLength > 0) && (space > 0))
        newPercent = (Decimal)newLength / (Decimal)space * 100;

      // What is the change in area
      Decimal reallocate = newPercent - w.ZoneArea;

      // Find the Window after this one
      Window nextWindow = m_windows[m_windows.IndexOf(w) + 1];

      if ((nextWindow.ZoneArea - reallocate) < 0m)
        reallocate = nextWindow.ZoneArea;
	
      // Modify the Window in question
      w.ZoneArea += reallocate;

      // Reverse modify the Window afterwards
      nextWindow.ZoneArea -= reallocate;
			
      // Update the visual appearance
      RepositionControls();
    }

    protected Decimal ReduceAreaEvenly(int thisIndex, Decimal windowAllocation)
    {
      Decimal removed = 0m;

      // Process each Window after this one in the collection
      for(int index=thisIndex + 1; index<m_windows.Count; index++)
      {
        Decimal zoneArea = m_windows[index].ZoneArea;

        if (zoneArea > 0m)
        {
          if (zoneArea >= windowAllocation)
          {
            // Reduce the area available for this Window
            m_windows[index].ZoneArea -= windowAllocation;

            // Keep total of all area removed
            removed += windowAllocation;
          }
          else
          {
            // Remove all the area from this Window
            m_windows[index].ZoneArea = 0m;

            // Keep total of all area removed
            removed += zoneArea;
          }
        }
      }

      return removed;
    }
		
    protected void RemoveWindowSpace(Window w)
    {
      // Is there only a single Window left?
      if (m_windows.Count == 1)
      {
        // Give it all the space
        m_windows[0].ZoneArea = 100m;
      }
      else
      {
        // Is there any space to reallocate?
        if (w.ZoneArea > 0)
        {
          // Total up all the values
          Decimal totalAllocated = 0m;

          // How much space should we add to each of the others
          Decimal freedSpace = w.ZoneArea / (m_windows.Count - 1);

          foreach(Window entry in m_windows)
          {
            if (entry != w)
            {
              // We only retain a sensible level of precision
              Decimal newSpace = Decimal.Round(entry.ZoneArea + freedSpace, m_spacePrecision);                            

              // Assign back new space
              entry.ZoneArea = newSpace;
                            
              // Total up all space so far 
              totalAllocated += newSpace;
            }
          }

          // Look for minor errors due not all fractions can be accurately represented in binary!
          if (totalAllocated > 100m)
          {
            Decimal correction = totalAllocated - 100m;

            // Remove from first entry
            foreach(Window entry in m_windows)
            {
              if (entry != w)
              {
                // Apply correction to this window
                entry.ZoneArea = totalAllocated - 100m;
                break;
              }
            }
          }
          else if (totalAllocated < 100m)
          {
            Decimal correction = 100m - totalAllocated;

            // Remove from first entry
            foreach(Window entry in m_windows)
            {
              if (entry != w)
              {
                // Apply correction to this window
                entry.ZoneArea += 100m - totalAllocated;
                break;
              }
            }
          }

          // Window no longer has any space
          w.ZoneArea = 0m;
        }
      }
    }

    protected void RepositionControls()
    {
      // Caller has requested that this call be ignored
      if (m_suppressReposition)
      {
        m_suppressReposition = false;
        return;
      }

      Rectangle clientRect = this.ClientRectangle;

      RepositionZoneBar(ref clientRect);

      if (m_windows.Count > 0)
      {
        // Space available for allocation
        int space;
				
        // Starting position of first control 
        int delta;

        if (m_direction == Direction.Vertical)
        {
          space = clientRect.Height;
          delta = clientRect.Top;
        }
        else
        {
          space = clientRect.Width;
          delta = clientRect.Left;
        }

        // Ensure this is not negative
        if (space < 0)
          space = 0;

        int barSpace = 0;
        int allocation = space;

        // Create temporary array of working values
        Position[] positions = new Position[Controls.Count - 1];

        // Pass 1, allocate all the space needed for each ResizeBar and the 
        //         minimal amount of space that each Window requests. 
        AllocateMandatorySizes(ref positions, ref barSpace, ref space);

        // If there any more space left
        if (space > 0)
        {
          // Pass 2, allocate any space left over according to the request percent 
          //         area that each Window would like to achieve.
          AllocateRemainingSpace(ref positions, space);
        }

        // Pass 3, reposition the controls according to allocated values.
        RepositionControls(ref positions, clientRect, delta);
      }
    }

    protected void AllocateMandatorySizes(ref Position[] positions, ref int barSpace, ref int space)
    {
      // Process each control (except last which is the Zone level ResizeBar)
      for(int index=0; index<(Controls.Count - 1); index++)
      {
        ResizeBar bar = Controls[index] as ResizeBar;

        if (bar != null)
        {
          // Length needed is depends on direction 
          positions[index].length = bar.Length;

          // Add up how much space was allocated to ResizeBars
          barSpace += positions[index].length;
        }
        else
        {
          Window w = Controls[index] as Window;

          if (w != null)
          {
            Size minimal = w.MinimalSize;

            // Length needed is depends on direction 
            if (m_direction == Direction.Vertical)
              positions[index].length = minimal.Height;
            else
              positions[index].length = minimal.Width;
          }
        }

        // Reduce available space by that just allocated
        space -= positions[index].length;
      }			
    }

    protected void AllocateRemainingSpace(ref Position[] positions, int windowSpace)
    {
      // Space allocated so far
      int allocated = 0;

      // Process each control (except last which is the Zone level ResizeBar)
      for(int index=0; index<(Controls.Count - 1); index++)
      {
        Window w = Controls[index] as Window;

        // If no window is maximized then as long as the control is a window we enter the if statement,
        // If a window is maximized then we only enter the if statement if this is the maximzied one
        if ((w != null) && ((m_maximizedWindow == null) || ((m_maximizedWindow != null) && (m_maximizedWindow == w))))
        {
          // How much of remaining space does the Window request to have?
          int extra;
					
          if (m_maximizedWindow == null)				
          {
            extra = (int)(windowSpace / 100m * w.ZoneArea);

            // Is this the last Window to be positioned?
            if (index == (Controls.Count - 1))
            {
              // Use up all the remaining space, this handles the case of 
              // the above vector calculation giving rounding errors so that
              // the last element needs adusting to fill exactly all the 
              // available space
              extra = windowSpace - allocated;
            }
          }
          else
          {
            extra = windowSpace - allocated;
          }

          // Add the extra space to any existing space it has
          positions[index].length += extra;

          // Keep count of all allocated so far
          allocated += extra;
        }
      }
    }

    protected void RepositionControls(ref Position[] positions, Rectangle clientRect, int delta)
    {
      // Process each control (except last which is the Zone level ResizeBar)
      for(int index=0; index<(Controls.Count - 1); index++)
      {
        int newDelta = positions[index].length;

        ResizeBar bar = Controls[index] as ResizeBar;

        if (bar != null)
        {
          if (m_direction == Direction.Vertical)
          {
            // Set new position
            bar.Location = new Point(clientRect.X, delta);
            bar.Width = clientRect.Width;
            bar.Height = newDelta;

            // Move delta down to next position
            delta += newDelta;
          }
          else
          {
            // Set new position
            bar.Location = new Point(delta, clientRect.Y);
            bar.Height = clientRect.Height;
            bar.Width = newDelta;

            // Move delta across to next position
            delta += newDelta;
          }
        }
        else
        {
          Window w = Controls[index] as Window;

          if (w != null)
          {
            if (newDelta == 0)
              w.Hide();
            else
            {
              // Set new position/size based on direction
              if (m_direction == Direction.Vertical)
              {
                w.Location = new Point(clientRect.X, delta);
                w.Width = clientRect.Width;
                w.Height = newDelta;
              }
              else
              {
                w.Location = new Point(delta, clientRect.Y);
                w.Height = clientRect.Height;
                w.Width = newDelta;
              }

              if (!w.Visible)
                w.Show();

              // Move delta to next position
              delta += newDelta;
            }
          }
        }
      }			
    }

    protected void RepositionZoneBar(ref Rectangle clientRect)
    {
      Rectangle barRect;

      int length = m_resizeBar.Length;
			
      // We only want a resizeBar when actually docked against an edge
      bool resizeBarPresent = ((this.Dock != DockStyle.Fill) && (this.Dock != DockStyle.None));

      // Define the correct direction for the resize bar and new Zone position
      switch(m_state)
      {
        case State.DockLeft:
          m_resizeBar.Direction = Direction.Horizontal;
          barRect = new Rectangle(this.Width - length, 0, length, this.Height);
					
          if (resizeBarPresent)
            clientRect.Width -= length;
          break;
        case State.DockTop:
          m_resizeBar.Direction = Direction.Vertical;
          barRect = new Rectangle(0, this.Height - length, this.Width, length);

          if (resizeBarPresent)
            clientRect.Height -= length;
          break;
        case State.DockRight:
          m_resizeBar.Direction = Direction.Horizontal;
          barRect = new Rectangle(0, 0, length, this.Height);

          if (resizeBarPresent)
          {
            clientRect.X += length;
            clientRect.Width -= length;
          }
          break;
        case State.DockBottom:
          m_resizeBar.Direction = Direction.Vertical;
          barRect = new Rectangle(0, 0, this.Width, length);

          if (resizeBarPresent)
          {
            clientRect.Y += length;
            clientRect.Height -= length;
          }
          break;
        case State.Floating:
        default:
          m_resizeBar.Direction = Direction.Horizontal;
          barRect = new Rectangle(0, 0, 0, 0);
          break;
      }

      if (resizeBarPresent)
      {
        // Reposition the Zone level resize bar control
        m_resizeBar.Location = new Point(barRect.X, barRect.Y);
        m_resizeBar.Size = new Size(barRect.Width, barRect.Height);
				
        if (!m_resizeBar.Visible)
          m_resizeBar.Show();
      }
      else
      {
        if (m_resizeBar.Visible)
          m_resizeBar.Hide();
      }
    }

    protected override void OnResize(EventArgs e)
    {
      // Need to recalculate based on new window space
      RepositionControls();

      base.OnResize(e);
    }
  }
}

#region revision history

// $Log: ZoneSequence.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/ZoneSequence.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: ZoneSequence.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion


