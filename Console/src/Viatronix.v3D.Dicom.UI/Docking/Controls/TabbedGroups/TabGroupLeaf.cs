// $Id: TabGroupLeaf.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.IO;
using System.Xml;
using System.Data;
using System.Drawing;
using System.Reflection;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Menus;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public class TabGroupLeaf : TabGroupBase
  {
    // Class constants
    protected const int m_imageWidth = 16;
    protected const int m_imageHeight = 16;
    protected const int m_imageHorzSplit = 0;
    protected const int m_imageVertSplit = 1;
	
    // Class state
    protected static ImageList m_internalImages;
        
    // Instance fields
    protected MenuCommand m_mcClose;
    protected MenuCommand m_mcSep1;
    protected MenuCommand m_mcProm;
    protected MenuCommand m_mcReba;
    protected MenuCommand m_mcSep2;
    protected MenuCommand m_mcPrev;
    protected MenuCommand m_mcNext;
    protected MenuCommand m_mcVert;
    protected MenuCommand m_mcHorz;
    protected Cursor m_savedCursor;
    protected bool m_dragEntered;
    protected TargetManager m_targetManager;
    protected Controls.TabControl m_tabControl;

    static TabGroupLeaf()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_internalImages = ResourceHelper.LoadBitmapStrip(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.ImagesTabbedGroups.bmp",
        new Size(m_imageWidth, m_imageHeight),
        new Point(0,0));
    }
	
    public TabGroupLeaf(TabbedGroups tabbedGroups, TabGroupBase parent)
      : base(tabbedGroups, parent)
    {
      // Create our managed tab control instance
      m_tabControl = new Controls.TabControl();
		    
      // We need page drag to begin when mouse dragged a small distance
      m_tabControl.DragFromControl = false;
		    
      // We need to monitor attempts to drag into the tab control
      m_dragEntered = false;
      m_tabControl.AllowDrop = true;
      m_tabControl.DragDrop += new DragEventHandler(OnControlDragDrop);
      m_tabControl.DragEnter += new DragEventHandler(OnControlDragEnter);
      m_tabControl.DragLeave += new EventHandler(OnControlDragLeave);
		    
      // Need notification when page drag begins
      m_tabControl.PageDragStart += new MouseEventHandler(OnPageDragStart);
      m_tabControl.PageDragMove += new MouseEventHandler(OnPageDragMove);
      m_tabControl.PageDragEnd += new MouseEventHandler(OnPageDragEnd);
      m_tabControl.PageDragQuit += new MouseEventHandler(OnPageDragQuit);
		    
      // Hook into tab page collection events
      m_tabControl.TabPages.Cleared += new CollectionClear(OnTabPagesCleared);
      m_tabControl.TabPages.Inserted += new CollectionChange(OnTabPagesInserted);
      m_tabControl.TabPages.Removed += new CollectionChange(OnTabPagesRemoved);
            
      // Hook into page level events
      m_tabControl.GotFocus += new EventHandler(OnGainedFocus);
      m_tabControl.PageGotFocus += new EventHandler(OnGainedFocus);
      m_tabControl.ClosePressed += new EventHandler(OnClose);            
            
      // Manager only created at start of drag operation
      m_targetManager = null;
            
      DefinePopupMenuForControl(m_tabControl);

      // Setup the correct 'HideTabsMode' for the control
      Notify(TabGroupBase.NotifyCode.DisplayTabMode);

      // Define the default setup of TabControl and allow developer to customize
      m_tabbedGroups.OnTabControlCreated(m_tabControl);
    }

    protected void DefinePopupMenuForControl(Controls.TabControl tabControl)
    {
      PopupMenu pm = new PopupMenu();
            
      // Add all the standard menus we manage
      m_mcClose = new MenuCommand("", new EventHandler(OnClose));
      m_mcSep1 = new MenuCommand("-");
      m_mcProm = new MenuCommand("", new EventHandler(OnToggleProminent));
      m_mcReba = new MenuCommand("", new EventHandler(OnRebalance));
      m_mcSep2 = new MenuCommand("-");
      m_mcHorz = new MenuCommand("", m_internalImages, m_imageHorzSplit, new EventHandler(OnNewVertical));
      m_mcVert = new MenuCommand("", m_internalImages, m_imageVertSplit, new EventHandler(OnNewHorizontal));
      m_mcNext = new MenuCommand("", new EventHandler(OnMoveNext));
      m_mcPrev = new MenuCommand("", new EventHandler(OnMovePrevious));

      // Prominent is a radio checked item
      m_mcProm.RadioCheck = true;

      // Use the provided context menu
      tabControl.ContextPopupMenu = pm;

      // Update command states when shown
      tabControl.PopupMenuDisplay += new CancelEventHandler(OnPopupMenuDisplay);
    }
    
    public TabPageCollection TabPages
    {
      get { return m_tabControl.TabPages; }
    }
    
    public override void Notify(NotifyCode code)
    {
      switch(code)
      {
        case NotifyCode.ImageListChanging:
          // Are we using the group level imagelist?
          if (m_tabbedGroups.ImageList == m_tabControl.ImageList)
          {   
            // Then remove its use
            m_tabControl.ImageList = null;
          }
          break;
        case NotifyCode.ImageListChanged:
          // If no imagelist defined
          if (m_tabControl.ImageList == null)
          {   
            // Then use the group level one
            m_tabControl.ImageList = m_tabbedGroups.ImageList;
          }
          break;
        case NotifyCode.StyleChanged:
          // Update tab control with new style
          m_tabControl.Style = m_tabbedGroups.Style;
          break;
        case NotifyCode.DisplayTabMode:
          // Apply the latest mode
        switch(m_tabbedGroups.DisplayTabMode)
        {
          case Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups.DisplayTabModes.ShowAll:
            m_tabControl.HideTabsMode = Controls.TabControl.HideTabsModes.ShowAlways;
            break;
          case Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups.DisplayTabModes.HideAll:
            m_tabControl.HideTabsMode = Controls.TabControl.HideTabsModes.HideAlways;
            break;
          case Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups.DisplayTabModes.ShowActiveLeaf:
            m_tabControl.HideTabsMode = (m_tabbedGroups.ActiveLeaf == this ? Controls.TabControl.HideTabsModes.ShowAlways :
              Controls.TabControl.HideTabsModes.HideAlways);
            break;
          case Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups.DisplayTabModes.ShowMouseOver:
            m_tabControl.HideTabsMode = Controls.TabControl.HideTabsModes.HideWithoutMouse;
            break;
          case Viatronix.v3D.Dicom.UI.Docking.Controls.TabbedGroups.DisplayTabModes.ShowActiveAndMouseOver:
            m_tabControl.HideTabsMode = (m_tabbedGroups.ActiveLeaf == this ? Controls.TabControl.HideTabsModes.ShowAlways :
              Controls.TabControl.HideTabsModes.HideWithoutMouse);
            break;
        }
          break;
      }
    }

    public override int Count               { get { return m_tabControl.TabPages.Count; } }
    public override bool IsLeaf             { get { return true; } }
    public override bool IsSequence         { get { return false; } }
    public override Control GroupControl    { get { return m_tabControl; } }
        
    public override bool ContainsProminent(bool recurse)
    {
      // Cache the currently selected prominent group
      TabGroupLeaf prominent = m_tabbedGroups.ProminentLeaf;

      // Valid value to test against?            
      if (prominent != null)
        return (this == prominent);
      else
        return false;
    }

    public override void SaveToXml(XmlTextWriter xmlOut)
    {
      // Output standard values appropriate for all Sequence instances
      xmlOut.WriteStartElement("Leaf");
      xmlOut.WriteAttributeString("Count", Count.ToString());
      xmlOut.WriteAttributeString("Unique", m_unique.ToString());
      xmlOut.WriteAttributeString("Space", m_space.ToString());

      // Output each tab page
      foreach(Controls.TabPage tp in m_tabControl.TabPages)
      {
        string controlType = "null";
                
        if (tp.Control != null)
          controlType = tp.Control.GetType().AssemblyQualifiedName;

        xmlOut.WriteStartElement("Page");
        xmlOut.WriteAttributeString("Title", tp.Title);
        xmlOut.WriteAttributeString("ImageList", (tp.ImageList != null).ToString());
        xmlOut.WriteAttributeString("ImageIndex", tp.ImageIndex.ToString());
        xmlOut.WriteAttributeString("Selected", tp.Selected.ToString());
        xmlOut.WriteAttributeString("Control", controlType);

        // Give handlers chance to reconstruct the page
        xmlOut.WriteStartElement("CustomPageData");
        m_tabbedGroups.OnPageSaving(new TGPageSavingEventArgs(tp, xmlOut));
        xmlOut.WriteEndElement();

        xmlOut.WriteEndElement();
      }
                
      xmlOut.WriteEndElement();
    }

    public override void LoadFromXml(XmlTextReader xmlIn)
    {
      // Grab the expected attributes
      string rawCount = xmlIn.GetAttribute(0);
      string rawUnique = xmlIn.GetAttribute(1);
      string rawSpace = xmlIn.GetAttribute(2);

      // Convert to correct types
      int count = System.Convert.ToInt32(rawCount);
      int unique = System.Convert.ToInt32(rawUnique);
      Decimal space = System.Convert.ToDecimal(rawSpace);
            
      // Update myself with new values
      m_unique = unique;
      m_space = space;
            
      // Load each of the children
      for(int i=0; i<count; i++)
      {
        // Read to the first page element or 
        if (!xmlIn.Read())
          throw new ArgumentException("An element was expected but could not be read in");

        // Must always be a page instance
        if (xmlIn.Name == "Page")
        {
          Controls.TabPage tp = new Controls.TabPage();

          // Grab the expected attributes
          string title = xmlIn.GetAttribute(0);
          string rawImageList = xmlIn.GetAttribute(1);
          string rawImageIndex = xmlIn.GetAttribute(2);
          string rawSelected = xmlIn.GetAttribute(3);
          string controlType = xmlIn.GetAttribute(4);

          // Convert to correct types
          bool imageList = System.Convert.ToBoolean(rawImageList);
          int imageIndex = System.Convert.ToInt32(rawImageIndex);
          bool selected = System.Convert.ToBoolean(rawSelected);

          // Setup new page instance
          tp.Title = title;
          tp.ImageIndex = imageIndex;
          tp.Selected = selected;
                    
          if (imageList)
            tp.ImageList = m_tabbedGroups.ImageList;
                    
          // Is there a type description of required control?
          if (controlType != "null")
          {
            try
            {
              // Get type description, if possible
              Type t = Type.GetType(controlType);
                            
              // Was a valid, known type?
              if (t != null)
              {
                // Get the assembly this type is defined inside
                Assembly a = t.Assembly;
                                
                if (a != null)
                {
                  // Create a new instance form the assemnly
                  object newObj = a.CreateInstance(t.FullName);
                                    
                  Control newControl = newObj as Control;
                                    
                  // Must derive from Control to be of use to us
                  if (newControl != null)
                    tp.Control = newControl;
                }
              }
            }
            catch
            {
              // We ignore failure to recreate given control type
            }
          }
                    
          // Read to the custom data element
          if (!xmlIn.Read())
            throw new ArgumentException("An element was expected but could not be read in");

          if (xmlIn.Name != "CustomPageData")
            throw new ArgumentException("Expected 'CustomPageData' element was not found");

          bool finished = xmlIn.IsEmptyElement;

          TGPageLoadingEventArgs e = new TGPageLoadingEventArgs(tp, xmlIn);

          // Give handlers chance to reconstruct per-page information
          m_tabbedGroups.OnPageLoading(e);
                    
          // Add into the control unless handler cancelled add operation
          if (!e.Cancel)
            m_tabControl.TabPages.Add(tp);
                    
          // Read everything until we get the end of custom data marker
          while(!finished)
          {
            // Check it has the expected name
            if (xmlIn.NodeType == XmlNodeType.EndElement)
              finished = (xmlIn.Name == "CustomPageData");

            if (!finished)
            {
              if (!xmlIn.Read())
                throw new ArgumentException("An element was expected but could not be read in");
            }
          } 

          // Read past the end of page element                    
          if (!xmlIn.Read())
            throw new ArgumentException("An element was expected but could not be read in");

          // Check it has the expected name
          if (xmlIn.NodeType != XmlNodeType.EndElement)
            throw new ArgumentException("End of 'page' element expected but missing");
                    
        }
        else
          throw new ArgumentException("Unknown element was encountered");
      }
    }

    protected void OnGainedFocus(object sender, EventArgs e)
    {
      // This tab control has the focus, make it the active leaf
      m_tabbedGroups.ActiveLeaf = this;
    }

    protected void OnTabPagesCleared()
    {
      // All pages removed, do we need to compact?
      if (m_tabbedGroups.AutoCompact)
        m_tabbedGroups.Compact();

      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;
    }

    protected void OnTabPagesInserted(int index, object value)
    {
      // If there is no currently active leaf then make it us
      if (m_tabbedGroups.ActiveLeaf == null)
        m_tabbedGroups.ActiveLeaf = this;

      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;
    }

    protected void OnTabPagesRemoved(int index, object value)
    {
      if (m_tabControl.TabPages.Count == 0)
      {
        // All pages removed, do we need to compact?
        if (m_tabbedGroups.AutoCompact)
          m_tabbedGroups.Compact();
      }

      // Mark layout as dirty
      if (m_tabbedGroups.AutoCalculateDirty)
        m_tabbedGroups.Dirty = true;
    }
        
    protected void OnPopupMenuDisplay(object sender, CancelEventArgs e)
    {
      // Remove all existing menu items
      m_tabControl.ContextPopupMenu.MenuCommands.Clear();
            
      // Add our standard set of menus
      m_tabControl.ContextPopupMenu.MenuCommands.AddRange(new MenuCommand[]{m_mcClose, m_mcSep1, 
                                                                             m_mcProm, m_mcReba, 
                                                                             m_mcSep2, m_mcHorz, 
                                                                             m_mcVert, m_mcNext, 
                                                                             m_mcPrev});
        
      // Are any pages selected
      bool valid = (m_tabControl.SelectedIndex != -1);
        
      // Define the latest text string
      m_mcClose.Text = m_tabbedGroups.CloseMenuText;
      m_mcProm.Text = m_tabbedGroups.ProminentMenuText;
      m_mcReba.Text = m_tabbedGroups.RebalanceMenuText;
      m_mcPrev.Text = m_tabbedGroups.MovePreviousMenuText;
      m_mcNext.Text = m_tabbedGroups.MoveNextMenuText;
      m_mcVert.Text = m_tabbedGroups.NewVerticalMenuText;
      m_mcHorz.Text = m_tabbedGroups.NewHorizontalMenuText;
            
      // Only need to close option if the tab has close defined
      m_mcClose.Visible = m_tabControl.ShowClose && valid;
      m_mcSep1.Visible = m_tabControl.ShowClose && valid;
            
      // Update the radio button for prominent
      m_mcProm.Checked = (m_tabbedGroups.ProminentLeaf == this);
            
      // Can only create new group if at least two pages exist
      bool split = valid && (m_tabControl.TabPages.Count > 1);

      bool vertSplit = split;
      bool horzSplit = split;
            
      TabGroupSequence tgs = m_parent as TabGroupSequence;

      // If we are not the only leaf, then can only split in 
      // the same direction as the group we are in
      if (tgs.Count > 1)
      {
        if (tgs.Direction == Direction.Vertical)
          vertSplit = false;
        else
          horzSplit = false;
      }
            
      m_mcVert.Visible = vertSplit;
      m_mcHorz.Visible = horzSplit;

      // Can only how movement if group exists in that direction
      m_mcNext.Visible = valid && (m_tabbedGroups.NextLeaf(this) != null);
      m_mcPrev.Visible = valid && (m_tabbedGroups.PreviousLeaf(this) != null);

      TGContextMenuEventArgs tge = new TGContextMenuEventArgs(this, 
        m_tabControl, 
        m_tabControl.SelectedTab,
        m_tabControl.ContextPopupMenu);
        
      // Generate event so handlers can add/remove/cancel menu
      m_tabbedGroups.OnPageContextMenu(tge);
            
      // Pass back cancel value
      e.Cancel = tge.Cancel;
    }
        
    internal void OnClose(object sender, EventArgs e)
    {
      TGCloseRequestEventArgs tge = new TGCloseRequestEventArgs(this, m_tabControl, m_tabControl.SelectedTab);
        
      // Generate event so handlers can perform appropriate action
      m_tabbedGroups.OnPageCloseRequested(tge);
            
      // Still want to close the page?
      if (!tge.Cancel)
        m_tabControl.TabPages.Remove(m_tabControl.SelectedTab);
    }
        
    internal void OnToggleProminent(object sender, EventArgs e)
    {
      // Toggel the prominent mode
      if (m_tabbedGroups.ProminentLeaf == this)
        m_tabbedGroups.ProminentLeaf = null;
      else
        m_tabbedGroups.ProminentLeaf = this;
    }

    internal void OnRebalance(object sender, EventArgs e)
    {
      m_tabbedGroups.Rebalance();
    }
            
    internal void OnMovePrevious(object sender, EventArgs e)
    {
      // Find the previous leaf node
      TabGroupLeaf prev = m_tabbedGroups.PreviousLeaf(this);
            
      // Must always be valid!
      if (prev != null)           
        MovePageToLeaf(prev);
    }

    internal void OnMoveNext(object sender, EventArgs e)
    {
      // Find the previous leaf node
      TabGroupLeaf next = m_tabbedGroups.NextLeaf(this);
            
      // Must always be valid!
      if (next != null)           
        MovePageToLeaf(next);
    }

    internal void OnNewVertical(object sender, EventArgs e)
    {
      NewVerticalGroup(this, false);
    }

    protected void OnNewHorizontal(object sender, EventArgs e)
    {
      NewHorizontalGroup(this, false);    
    }

    internal void NewVerticalGroup(TabGroupLeaf sourceLeaf, bool before)
    {
      TabGroupSequence tgs = this.Parent as TabGroupSequence;
        
      // We must have a parent sequence!
      if (tgs != null)
      {
        tgs.Direction = Direction.Vertical;
        AddGroupToSequence(tgs, sourceLeaf, before);
      }
    }
        
    internal void NewHorizontalGroup(TabGroupLeaf sourceLeaf, bool before)
    {
      TabGroupSequence tgs = this.Parent as TabGroupSequence;
        
      // We must have a parent sequence!
      if (tgs != null)
      {
        tgs.Direction = Direction.Horizontal;
        AddGroupToSequence(tgs, sourceLeaf, before);
      }
    }
        
    internal void MovePageToLeaf(TabGroupLeaf leaf)
    {
      // Remember original auto compact mode
      bool autoCompact = m_tabbedGroups.AutoCompact;

      // Turn mode off as it interferes with reorganisation
      m_tabbedGroups.AutoCompact = false;

      // Get the requested tab page to be moved to new leaf
      TabPage tp = m_tabControl.SelectedTab;

      // Remove page from ourself
      m_tabControl.TabPages.Remove(tp);
            
      // Add into the new leaf
      leaf.TabPages.Add(tp);

      // Make new leaf the active one
      m_tabbedGroups.ActiveLeaf = leaf;
                
      TabControl tc = leaf.GroupControl as Controls.TabControl;
                
      // Select the newly added page
      tc.SelectedTab = tp;

      // Reset compacting mode as we have updated the structure
      m_tabbedGroups.AutoCompact = autoCompact;
            
      // Do we need to compact?
      if (m_tabbedGroups.AutoCompact)
        m_tabbedGroups.Compact();
    }

    protected void AddGroupToSequence(TabGroupSequence tgs, TabGroupLeaf sourceLeaf, bool before)
    {
      // Remember original auto compact mode
      bool autoCompact = m_tabbedGroups.AutoCompact;

      // Turn mode off as it interferes with reorganisation
      m_tabbedGroups.AutoCompact = false;

      // Find our index into parent collection
      int pos = tgs.IndexOf(this);
                
      TabGroupLeaf newGroup = null;

      // New group inserted before existing one?                
      if (before)
        newGroup = tgs.InsertNewLeaf(pos);
      else
      {
        // No, are we at the end of the collection?
        if (pos == (tgs.Count - 1))
          newGroup = tgs.AddNewLeaf();
        else
          newGroup = tgs.InsertNewLeaf(pos + 1);
      }
                     
      // Get tab control for source leaf
      Controls.TabControl tc = sourceLeaf.GroupControl as Controls.TabControl;
                        
      TabPage tp = tc.SelectedTab;

      // Remove page from ourself
      tc.TabPages.Remove(tp);
                    
      // Add into the new leaf
      newGroup.TabPages.Add(tp);

      // Reset compacting mode as we have updated the structure
      m_tabbedGroups.AutoCompact = autoCompact;

      // Do we need to compact?
      if (m_tabbedGroups.AutoCompact)
        m_tabbedGroups.Compact();
    }
        
    protected void OnPageDragStart(object sender, MouseEventArgs e)
    {
      // Save the current cursor value
      m_savedCursor = m_tabControl.Cursor;
            
      // Manager will create hot zones and draw dragging rectangle
      m_targetManager = new TargetManager(m_tabbedGroups, this, m_tabControl);
    }
 
    protected void OnPageDragMove(object sender, MouseEventArgs e)
    {
      // Convert from Control coordinates to screen coordinates
      Point mousePos = m_tabControl.PointToScreen(new Point(e.X, e.Y));

      // Let manager decide on drawing rectangles and setting cursor
      m_targetManager.MouseMove(mousePos);
    }

    protected void OnPageDragEnd(object sender, MouseEventArgs e)
    {
      // Give manager chance to action request and cleanup
      m_targetManager.Exit();

      // No longer need the manager
      m_targetManager = null;
            
      // Restore the original cursor
      m_tabControl.Cursor = m_savedCursor;
    }

    protected void OnPageDragQuit(object sender, MouseEventArgs e)
    {
      // Give manager chance to cleanup
      m_targetManager.Quit();
        
      // No longer need the manager
      m_targetManager = null;

      // Restore the original cursor
      m_tabControl.Cursor = m_savedCursor;
    }

        
    protected void OnControlDragEnter(object sender, DragEventArgs drgevent)
    {
      m_dragEntered = ValidFormat(drgevent);
        
      // Do we allow the drag to occur?
      if (m_dragEntered)
      {
        // Must draw a drag indicator
        DrawDragIndicator();
                
        // Update the allowed effects
        drgevent.Effect = DragDropEffects.Copy;
      }
    }

    protected void OnControlDragDrop(object sender, DragEventArgs drgevent)
    {
      // Do we allow the drop to occur?
      if (m_dragEntered)
      {
        // Must remove the drag indicator
        DrawDragIndicator();
                
        // Generate an event so caller can perform required action
        m_tabbedGroups.OnExternalDrop(this, m_tabControl, GetDragProvider(drgevent));
      }

      m_dragEntered = false;
    }

    protected void OnControlDragLeave(object sender, EventArgs e)
    {
      // Do we need to remove the drag indicator?
      if (m_dragEntered)
        DrawDragIndicator();
                
      m_dragEntered = false;
    }
        
    protected bool ValidFormat(DragEventArgs e)
    {
      return e.Data.GetDataPresent(typeof(TabbedGroups.DragProvider));
    }
        
    protected TabbedGroups.DragProvider GetDragProvider(DragEventArgs e)
    {
      return (TabbedGroups.DragProvider)e.Data.GetData(typeof(TabbedGroups.DragProvider));
    }
        
    protected void DrawDragIndicator()
    {
      // Create client rectangle
      Rectangle clientRect = new Rectangle(new Point(0,0), m_tabControl.ClientSize);
            
      // Draw drag indicator around whole control
      TargetManager.DrawDragRectangle(m_tabControl.RectangleToScreen(clientRect));
    }
  }
}


#region revision history

// $Log: TabGroupLeaf.cs,v $
// Revision 1.1  2005/10/11 14:41:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabbedGroups/TabGroupLeaf.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
// $Id: TabGroupLeaf.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $

#endregion

