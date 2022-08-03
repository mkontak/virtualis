// $Id: WindowContentTabbed.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  [ToolboxItem(false)]
  public class WindowContentTabbed : WindowContent, IHotZoneSource, IMessageFilter
  {
    // Class constants
    protected static int m_plainBorder = 3;
    protected static int m_hotAreaInflate = -3;

    // Instance fields
    protected int m_dragPageIndex;
    protected Content m_activeContent;
    protected RedockerContent m_redocker;
    protected Controls.TabControl m_tabControl;

    public WindowContentTabbed(DockingManager manager, VisualStyle vs)
      : base(manager, vs)
    {
      m_redocker = null;
      m_activeContent = null;
            
      // Create the TabControl used for viewing the Content windows
      m_tabControl = new Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl();

      // It should always occupy the remaining space after all details
      m_tabControl.Dock = DockStyle.Fill;

      // Show tabs only if two or more tab pages exist
      m_tabControl.HideTabsMode = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.HideTabsModes.HideUsingLogic;
            
      // Hook into the TabControl notifications
      m_tabControl.GotFocus += new EventHandler(OnTabControlGotFocus);
      m_tabControl.LostFocus += new EventHandler(OnTabControlLostFocus);
      m_tabControl.PageGotFocus += new EventHandler(OnTabControlGotFocus);
      m_tabControl.PageLostFocus += new EventHandler(OnTabControlLostFocus);
      m_tabControl.SelectionChanged += new EventHandler(OnSelectionChanged);
      m_tabControl.PageDragStart += new MouseEventHandler(OnPageDragStart);
      m_tabControl.PageDragMove += new MouseEventHandler(OnPageDragMove);
      m_tabControl.PageDragEnd += new MouseEventHandler(OnPageDragEnd);
      m_tabControl.PageDragQuit += new MouseEventHandler(OnPageDragQuit);
      m_tabControl.DoubleClickTab += new Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.DoubleClickTabHandler(OnDoubleClickTab);
      m_tabControl.Font = manager.TabControlFont;
      m_tabControl.BackColor = manager.BackColor;
      m_tabControl.ForeColor = manager.InactiveTextColor;

      // Define the visual style required
      m_tabControl.Style = vs;

      // Allow developers a chance to override default settings
      manager.OnTabControlCreated(m_tabControl);

      switch(vs)
      {
        case VisualStyle.IDE:
          Controls.Add(m_tabControl);
          break;
        case VisualStyle.Plain:
          // Only the border at the pages edge and not around the whole control
          m_tabControl.InsetBorderPagesOnly = !m_manager.PlainTabBorder;

          // We want a border around the TabControl so it is indented and looks consistent
          // with the Plain look and feel, so use the helper Control 'BorderForControl'
          BorderForControl bfc = new BorderForControl(m_tabControl, m_plainBorder);

          // It should always occupy the remaining space after all details
          bfc.Dock = DockStyle.Fill;

          // Define the default border border
          bfc.BackColor = m_manager.BackColor;

          // When in 'VisualStyle.Plain' we need to 
          Controls.Add(bfc);
          break;
      }

      // Need to hook into message pump so that the ESCAPE key can be 
      // intercepted when in redocking mode
      Application.AddMessageFilter(this);
    }

    public Content CurrentContent
    {
      get
      {
        Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage tp = m_tabControl.SelectedTab;
                
        if (tp != null)
          return (Content)tp.Tag;
        else
          return null;
      }
    }

    public Controls.TabControl TabControl
    {
      get { return m_tabControl; } 
    }

    public void HideCurrentContent()
    {
      Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage tp = m_tabControl.SelectedTab;
            
      int count = m_tabControl.TabPages.Count;

      // Find currently selected tab
      int index = m_tabControl.SelectedIndex;

      // Decide which other tab to make selected instead
      if (count > 1)
      {
        // Move to the next control along
        int newSelect = index + 1;

        // Wrap around to first tab if at end
        if (newSelect == count)
          newSelect = 0;

        // Change selection
        m_tabControl.SelectedIndex = newSelect;
      }
      else
      {
        // Hide myself as am about to die
        this.Hide();

        // Ensure the focus goes somewhere else
        m_manager.Container.Focus();
      }

      if (tp != null)
      {
        // Have the manager perform the Hide operation for us
        m_manager.HideContent(tp.Tag as Content);
      }
    }
        
    public override void BringContentToFront(Content c)
    {
      // Find the matching Page and select it
      foreach(Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage page in m_tabControl.TabPages)
        if (page.Tag == c)
        {
          m_tabControl.SelectedTab = page;
          break;
        }
    }

    public override void PropogateNameValue(PropogateName name, object value)
    {
      base.PropogateNameValue(name, value);
        
      switch(name)
      {
        case PropogateName.BackColor:
          Color newColor = (Color)value;
            
          // In Plain style we need to color the intermidiate window as well    
          if (m_style == VisualStyle.Plain)
          {
            BorderForControl bfc = this.Controls[0] as BorderForControl;
            bfc.BackColor = newColor;                               
          }

          m_tabControl.BackColor = newColor;
          this.BackColor = newColor;
                    
          Invalidate();
          break;
        case PropogateName.InactiveTextColor:
          m_tabControl.ForeColor = (Color)value;
          break;
        case PropogateName.PlainTabBorder:
          m_tabControl.InsetBorderPagesOnly = !(bool)value;
          break;
        case PropogateName.TabControlFont:
          m_tabControl.Font = (Font)value;
          break;
      }
    }

    protected override void OnContentsClearing()
    {
      m_tabControl.TabPages.Clear();

      base.OnContentsClearing();

      if (!this.AutoDispose)
      {
        // Inform each detail of the change in title text
        NotifyFullTitleText("");
      }
    }

    protected override void OnContentInserted(int index, object value)
    {
      base.OnContentInserted(index, value);

      Content content = value as Content;

      // Create TabPage to represent the Content
      Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage newPage = new Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage();

      // Reflect the Content properties int the TabPage
      newPage.Title = content.Title;
      newPage.ImageList = content.ImageList;
      newPage.ImageIndex = content.ImageIndex;
      newPage.Control = content.Control;
      newPage.Tag = content;
			
      // Reflect same order in TabPages collection as Content collection
      m_tabControl.TabPages.Insert(index, newPage);
    }

    protected override void OnContentRemoving(int index, object value)
    {
      base.OnContentRemoving(index, value);

      Content c = value as Content;

      // Find the matching Page and remove it
      foreach(Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage page in m_tabControl.TabPages)
        if (page.Tag == c)
        {
          m_tabControl.TabPages.Remove(page);
          break;
        }
    }

    public override void WindowDetailGotFocus(WindowDetail wd)
    {
      // Transfer focus from WindowDetail to the TabControl
      m_tabControl.Focus();
    }

    protected void OnSelectionChanged(object sender, EventArgs e)
    {
      if (m_tabControl.TabPages.Count == 0)
      {
        // Inform each detail of the change in title text
        NotifyFullTitleText("");
      }
      else
      {
        // Inform each detail of the new title text
        if (m_tabControl.SelectedIndex != -1)
        {
          Content selectedContent = m_tabControl.SelectedTab.Tag as Content;
                    
          NotifyAutoHideImage(selectedContent.AutoHidden);
          NotifyCloseButton(selectedContent.CloseButton);
          NotifyHideButton(selectedContent.HideButton);
          NotifyFullTitleText(selectedContent.FullTitle);
          NotifyShowCaptionBar(selectedContent.CaptionBar);
        }
      }
    }

    protected void OnTabControlGotFocus(object sender, EventArgs e)
    {
      NotifyContentGotFocus();
    }

    protected void OnTabControlLostFocus(object sender, EventArgs e)
    {
      NotifyContentLostFocus();
    }

    public void AddHotZones(Redocker redock, HotZoneCollection collection)
    {
      RedockerContent redocker = redock as RedockerContent;

      bool itself = false;
      bool nullZone = false;

      // We process differently for WindowContent to redock into itself!
      if ((redocker.WindowContent != null) && (redocker.WindowContent == this))
        itself = true;

      // We do not allow a Content to redock into its existing container
      if (itself && !m_contents.Contains(redocker.Content))
        nullZone = true;

      Rectangle newSize = this.RectangleToScreen(this.ClientRectangle);
      Rectangle hotArea = m_tabControl.RectangleToScreen(m_tabControl.ClientRectangle);;

      // Find any caption detail and use that area as the hot area
      foreach(WindowDetail wd in m_windowDetails)
      {
        WindowDetailCaption wdc = wd as WindowDetailCaption;

        if (wdc != null)
        {
          hotArea = wdc.RectangleToScreen(wdc.ClientRectangle);
          hotArea.Inflate(m_hotAreaInflate, m_hotAreaInflate);
          break;
        }
      }

      if (nullZone)
        collection.Add(new HotZoneNull(hotArea));
      else
        collection.Add(new HotZoneTabbed(hotArea, newSize, this, itself));				
    }

    protected void OnDoubleClickTab(Controls.TabControl tc, Controls.TabPage page)
    {
      Content c = (Content)page.Tag;

      // Make Content record its current position and remember it for the future 
      c.RecordRestore();

      // Invert docked status
      c.Docked = (c.Docked == false);

      // Remove from current WindowContent and restore its position
      m_manager.HideContent(c, false, true);
      m_manager.ShowContent(c);
    }
		
    protected void OnPageDragStart(object sender, MouseEventArgs e)
    {
      if (this.RedockAllowed)
      {
        // There must be a selected page for this event to occur
        Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage page = m_tabControl.SelectedTab;

        // Event page must specify its Content object
        Content c = page.Tag as Content;

        // Remember the position of the tab before it is removed
        m_dragPageIndex = m_tabControl.TabPages.IndexOf(page);

        // Remove page from TabControl
        m_tabControl.TabPages.Remove(page);

        // Force the entire window to redraw to ensure the Redocker does not start drawing
        // the XOR indicator before the window repaints itself. Otherwise the repainted
        // control will interfere with the XOR indicator.
        this.Refresh();

        // Start redocking activity for the single Content of this WindowContent
        m_redocker = new RedockerContent(m_tabControl, c, this, new Point(e.X, e.Y));
      }
    }

    protected void OnPageDragMove(object sender, MouseEventArgs e)
    {
      // Redocker object needs mouse movements
      if (m_redocker != null)
        m_redocker.OnMouseMove(e);
    }

    protected void OnPageDragEnd(object sender, MouseEventArgs e)
    {
      // Are we currently in a redocking state?
      if (m_redocker != null)
      {
        // Let the redocker finish off
        bool moved = m_redocker.OnMouseUp(e);

        // If the tab was not positioned somewhere else
        if (!moved)
        {
          // Put back the page that was removed when dragging started
          RestoreDraggingPage();
        }

        // No longer need the object
        m_redocker = null;
      }
    }

    protected void OnPageDragQuit(object sender, MouseEventArgs e)
    {
      // Are we currently in a redocking state?
      if (m_redocker != null)
      {
        // Put back the page that was removed when dragging started
        RestoreDraggingPage();

        // No longer need the object
        m_redocker = null;
      }
    }

    protected override void OnContentChanged(Content obj, Content.Property prop)
    {
      // Find the matching TabPage entry
      foreach(Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage page in m_tabControl.TabPages)
      {
        // Does this page manage our Content?
        if (page.Tag == obj)
        {
          // Property specific processing
          switch(prop)
          {
            case Content.Property.Control:
              page.Control = obj.Control;
              break;
            case Content.Property.Title:
              page.Title = obj.Title;
              break;
            case Content.Property.FullTitle:
              // Title changed for the current page?
              if (m_tabControl.SelectedTab == page)
              {
                // Update displayed caption text
                NotifyFullTitleText(obj.FullTitle);
              }								
              break;
            case Content.Property.ImageList:
              page.ImageList = obj.ImageList;
              break;
            case Content.Property.ImageIndex:
              page.ImageIndex = obj.ImageIndex;
              break;
            case Content.Property.CaptionBar:
              // Property changed for the current page?
              if (m_tabControl.SelectedTab == page)
              {
                Content target = page.Tag as Content;
                        
                // TODO
                NotifyShowCaptionBar(target.CaptionBar);                                
              }
              break;
            case Content.Property.CloseButton:
              // Property changed for the current page?
              if (m_tabControl.SelectedTab == page)
              {
                Content target = page.Tag as Content;
                        
                NotifyCloseButton(target.CloseButton);   
              }
              break;
            case Content.Property.HideButton:
              // Property changed for the current page?
              if (m_tabControl.SelectedTab == page)
              {
                Content target = page.Tag as Content;
                        
                NotifyHideButton(target.HideButton);   
              }
              break;
          }

          break;
        }
      }
    }

    protected override void OnResize(EventArgs e)
    {
      // Inform each Content of its actual displayed size
      foreach(Content c in m_contents)
      {
        switch(m_state)
        {
          case State.DockLeft:
          case State.DockRight:
            if (this.Dock != DockStyle.Fill)
            {
              // Only update the remembered width
              c.DisplaySize = new Size(this.ClientSize.Width, c.DisplaySize.Height);
            }
            break;
          case State.DockTop:
          case State.DockBottom:
            if (this.Dock != DockStyle.Fill)
            {
              // Only update the remembered height
              c.DisplaySize = new Size(c.DisplaySize.Width, this.ClientSize.Height);
            }
            break;
          case State.Floating:
          {
            Form f = this.FindForm();

            // Update width and height
            if (f == null)
              c.DisplaySize = this.ClientSize;
            else
              c.DisplaySize = f.Size;
          }
            break;
        }
      }

      base.OnResize(e);
    }
        
    public override Restore RecordRestore(object child) 
    {
      // Child of a WindowContent must be a Content object
      Content c = child as Content;

      StringCollection next = new StringCollection();
      StringCollection previous = new StringCollection();

      bool before = true;

      // Fill collections with list of Content before and after parameter
      foreach(Content content in m_contents)
      {
        if (content == c)
          before = false;
        else
        {
          if (before)
            previous.Add(content.Title);
          else
            next.Add(content.Title);
        }
      }

      bool selected = false;

      // Is there a selected tab?
      if (m_tabControl.SelectedIndex != -1)
      {
        // Get access to the selected Content
        Content selectedContent = m_tabControl.SelectedTab.Tag as Content;

        // Need to record if it is selected
        selected = (selectedContent == c);
      }

      // Create a Restore object to handle this WindowContent
      Restore thisRestore = new RestoreWindowContent(null, c, next, previous, selected);

      // Do we have a Zone as our parent?
      if (m_parentZone != null)
      {
        // Get the Zone to prepend its own Restore knowledge
        thisRestore = m_parentZone.RecordRestore(this, child, thisRestore);
      }

      return thisRestore;
    }

    public bool PreFilterMessage(ref Message m)
    {
      // Has a key been pressed?
      if (m.Msg == (int)Win32.Msgs.WM_KEYDOWN)
      {
        // Is it the ESCAPE key?
        if ((int)m.WParam == (int)Win32.VirtualKeys.VK_ESCAPE)
        {                   
          // Are we in redocking mode?
          if (m_redocker != null)
          {
            // Cancel the redocking activity
            m_redocker.QuitTrackingMode(null);

            // Put back the page that was removed when dragging started
            RestoreDraggingPage();
                        
            // No longer need the object
            m_redocker = null;
                        
            return true;
          }
        }
      }
            
      return false;
    }
        
    protected void RestoreDraggingPage()
    {
      // Create TabPage to represent the Content
      Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage newPage = new Viatronix.v3D.Dicom.UI.Docking.Controls.TabPage();

      Content content = m_redocker.Content;

      // Reflect the Content properties int the TabPage
      newPage.Title = content.Title;

      newPage.ImageList = content.ImageList;
      newPage.ImageIndex = content.ImageIndex;
      newPage.Control = content.Control;
      newPage.Tag = content;
      newPage.Selected = true;
		
      // Put it back where it came from
      m_tabControl.TabPages.Insert(m_dragPageIndex, newPage);

      // Update the title displayed
      NotifyFullTitleText(content.FullTitle);
    }
  }
}

#region revision history

// $Log: WindowContentTabbed.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/WindowContentTabbed.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: WindowContentTabbed.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

