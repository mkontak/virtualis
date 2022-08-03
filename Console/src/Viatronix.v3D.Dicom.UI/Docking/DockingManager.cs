// $Id: DockingManager.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.Xml.Serialization;
using Microsoft.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Menus;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public enum PropogateName
  {
    BackColor,
    ActiveColor,
    ActiveTextColor,
    InactiveTextColor,
    ResizeBarColor,
    ResizeBarVector,
    CaptionFont,
    TabControlFont,
    ZoneMinMax,
    PlainTabBorder
  }

  public class DockingManager
  {
    // Instance fields
    protected bool m_zoneMinMax;
    protected bool m_insideFill;
    protected bool m_autoResize;
    protected bool m_firstHalfWidth;
    protected bool m_firstHalfHeight;
    protected int m_surpressVisibleEvents;
    protected int m_resizeBarVector;
    protected Size m_innerMinimum;
    protected Color m_backColor;
    protected Color m_activeColor;
    protected Color m_activeTextColor;
    protected Color m_inactiveTextColor;
    protected Color m_resizeBarColor;
    protected Font m_captionFont;
    protected Font m_tabControlFont;
    protected bool m_defaultBackColor;
    protected bool m_defaultActiveColor;
    protected bool m_defaultActiveTextColor;
    protected bool m_defaultInactiveTextColor;
    protected bool m_defaultResizeBarColor;
    protected bool m_defaultCaptionFont;
    protected bool m_defaultTabControlFont;
    protected bool m_plainTabBorder;
    protected Control m_innerControl;
    protected Control m_outerControl;
    protected AutoHidePanel m_ahpTop;
    protected AutoHidePanel m_ahpLeft;
    protected AutoHidePanel m_ahpBottom;
    protected AutoHidePanel m_ahpRight;
    protected VisualStyle m_visualStyle;
    protected ContainerControl m_container;
    protected ManagerContentCollection m_contents;

    public delegate void ContentHandler(Content c, EventArgs cea);
    public delegate void ContentHidingHandler(Content c, CancelEventArgs cea);
    public delegate void ContextMenuHandler(PopupMenu pm, CancelEventArgs cea);
    public delegate void TabControlCreatedHandler(Controls.TabControl tabControl);
    public delegate void SaveCustomConfigHandler(XmlTextWriter xmlOut);
    public delegate void LoadCustomConfigHandler(XmlTextReader xmlIn);

    // Exposed events
    public event ContentHandler ContentShown;
    public event ContentHandler ContentHidden;
    public event ContentHidingHandler ContentHiding;
    public event ContextMenuHandler ContextMenu;
    public event TabControlCreatedHandler TabControlCreated;
    public event SaveCustomConfigHandler SaveCustomConfig;
    public event LoadCustomConfigHandler LoadCustomConfig;

    public DockingManager(ContainerControl container, VisualStyle vs)
    {
      // Must provide a valid container instance
      if (container == null)
        throw new ArgumentNullException("Container");

      // Default state
      m_container = container;
      m_visualStyle = vs;
      m_innerControl = null;
      m_zoneMinMax = true;
      m_insideFill = false;
      m_autoResize = true;
      m_firstHalfWidth = true;
      m_firstHalfHeight = true;
      m_plainTabBorder = false;
      m_surpressVisibleEvents = 0;
      m_innerMinimum = new Size(20, 20);

      // Default font/resize
      m_resizeBarVector = -1;
      m_captionFont = SystemInformation.MenuFont;
      m_tabControlFont = SystemInformation.MenuFont;
      m_defaultCaptionFont = true;
      m_defaultTabControlFont = true;

      // Create and add hidden auto hide panels
      AddAutoHidePanels();

      // Define initial colors
      ResetColors();

      // Create an object to manage the collection of Content
      m_contents = new ManagerContentCollection(this);

      // We want notification when contents are removed/cleared
      m_contents.Clearing += new CollectionClear(OnContentsClearing);
      m_contents.Removed += new CollectionChange(OnContentRemoved);

      // We want to perform special action when container is resized
      m_container.Resize += new EventHandler(OnContainerResized);
    			
      // A Form can cause the child controls to be reordered after the initialisation
      // but before the Form.Load event. To handle this we hook into the event and force
      // the auto hide panels to be ordered back into their proper place.
      if (m_container is Form)
      {   
        Form formContainer = m_container as Form;			    
        formContainer.Load += new EventHandler(OnFormLoaded);
      }

      // Need notification when colors change
      Microsoft.Win32.SystemEvents.UserPreferenceChanged += new UserPreferenceChangedEventHandler(OnPreferenceChanged);

    }

    public ContainerControl Container
    {
      get { return m_container; }
    }

    public Control InnerControl
    {
      get { return m_innerControl; }
      set { m_innerControl = value; }
    }

    public Control OuterControl
    {
      get { return m_outerControl; }
      set 
      {
        if (m_outerControl != value)
        {
          m_outerControl = value;
    				    
          // Use helper routine to ensure panels are in correct positions
          ReorderAutoHidePanels();
        }
      }
    }

    public ManagerContentCollection Contents
    {
      get { return m_contents; }
			
      set 
      {
        m_contents.Clear();
        m_contents = value;	
      }
    }

    public bool ZoneMinMax
    {
      get { return m_zoneMinMax; }

      set 
      { 
        if (value != m_zoneMinMax)
        {
          m_zoneMinMax = value;
                
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.ZoneMinMax, (object)m_zoneMinMax);
        } 
      }
    }

    public bool InsideFill
    {
      get { return m_insideFill; }

      set
      {
        if (m_insideFill != value)
        {
          m_insideFill = value;

          if (m_insideFill)
          {
            // Add Fill style to innermost docking window
            AddInnerFillStyle();
          }
          else
          {
            // Remove Fill style from innermost docking window
            RemoveAnyFillStyle();
						
            // Ensure that inner control can be seen
            OnContainerResized(null, EventArgs.Empty);
          }
        }
      }
    }

    public bool AutoResize
    {
      get { return m_autoResize; }
      set { m_autoResize = value; }
    }

    public Size InnerMinimum
    {
      get { return m_innerMinimum; }
      set { m_innerMinimum = value; }
    }

    public VisualStyle Style
    {
      get { return m_visualStyle; }
    }

    public int ResizeBarVector
    {
      get { return m_resizeBarVector; }
            
      set 
      {
        if (value != m_resizeBarVector)
        {
          m_resizeBarVector = value;
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.ResizeBarVector, (object)m_resizeBarVector);
        }
      }
    }

    public Color BackColor
    {
      get { return m_backColor; }
            
      set 
      {
        if (value != m_backColor)
        {
          m_backColor = value;
          m_defaultBackColor = (m_backColor == SystemColors.Control);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.BackColor, (object)m_backColor);
        }
      }
    }
    
    public Color ActiveColor
    {
      get { return m_activeColor; }
            
      set 
      {
        if (value != m_activeColor)
        {
          m_activeColor = value;
          m_defaultActiveColor = (m_activeColor == SystemColors.ActiveCaption);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.ActiveColor, (object)m_activeColor);
        }
      }
    }
        
    public Color ActiveTextColor
    {
      get { return m_activeTextColor; }
            
      set 
      {
        if (value != m_activeTextColor)
        {
          m_activeTextColor = value;
          m_defaultActiveTextColor = (m_activeTextColor == SystemColors.ActiveCaptionText);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.ActiveTextColor, (object)m_activeTextColor);
        }
      }
    }

    public Color InactiveTextColor
    {
      get { return m_inactiveTextColor; }
            
      set 
      {
        if (value != m_inactiveTextColor)
        {
          m_inactiveTextColor = value;
          m_defaultInactiveTextColor = (m_inactiveTextColor == SystemColors.ControlText);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.InactiveTextColor, (object)m_inactiveTextColor);
        }
      }
    }

    public Color ResizeBarColor
    {
      get { return m_resizeBarColor; }
            
      set 
      {
        if (value != m_resizeBarColor)
        {
          m_resizeBarColor = value;
          m_defaultResizeBarColor = (m_resizeBarColor == SystemColors.Control);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.ResizeBarColor, (object)m_resizeBarColor);
        }
      }
    }
        
    public Font CaptionFont
    {
      get { return m_captionFont; }
            
      set 
      {
        if (value != m_captionFont)
        {
          m_captionFont = value;
          m_defaultCaptionFont = (m_captionFont == SystemInformation.MenuFont);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.CaptionFont, (object)m_captionFont);
        }
      }
    }

    public Font TabControlFont
    {
      get { return m_tabControlFont; }
            
      set 
      {
        if (value != m_tabControlFont)
        {
          m_tabControlFont = value;
          m_defaultTabControlFont = (m_captionFont == SystemInformation.MenuFont);
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.TabControlFont, (object)m_tabControlFont);
        }
      }
    }

    public bool PlainTabBorder
    {
      get { return m_plainTabBorder; }
            
      set 
      {
        if (value != m_plainTabBorder)
        {
          m_plainTabBorder = value;
                    
          // Notify each object in docking hierarchy in case they need to know new value
          PropogateNameValue(PropogateName.PlainTabBorder, (object)m_plainTabBorder);
        }
      }
    }

    public void ResetColors()
    {
      m_backColor = SystemColors.Control;
      m_inactiveTextColor = SystemColors.ControlText;
      m_activeColor = SystemColors.ActiveCaption;
      m_activeTextColor = SystemColors.ActiveCaptionText;
      m_resizeBarColor = SystemColors.Control;
      m_defaultBackColor = true;
      m_defaultActiveColor = true;
      m_defaultActiveTextColor = true;
      m_defaultInactiveTextColor = true;
      m_defaultResizeBarColor = true;

      PropogateNameValue(PropogateName.BackColor, (object)m_backColor);
      PropogateNameValue(PropogateName.ActiveColor, (object)m_activeColor);
      PropogateNameValue(PropogateName.ActiveTextColor, (object)m_activeTextColor);
      PropogateNameValue(PropogateName.InactiveTextColor, (object)m_inactiveTextColor);
      PropogateNameValue(PropogateName.ResizeBarColor, (object)m_resizeBarColor);
    }

    public void UpdateInsideFill()
    {
      // Is inside fill ability enabled?
      if (m_insideFill)
      {
        // Reduce flicker
        m_container.SuspendLayout();
				
        // Ensure correct zone has the Fill style
        RemoveAnyFillStyle();
        AddInnerFillStyle();

        m_container.ResumeLayout();
      }
    }

    public virtual bool ShowContent(Content c)
    {
      // Validate the incoming Content instance is a valid reference
      // and is a current instance within our internal collection
      if ((c == null) || !m_contents.Contains(c))
        return false;
		
      // Remove it from view by removing from current WindowContent container
      if (!c.Visible)
      {
        // Do not generate hiding/hidden/shown events
        m_surpressVisibleEvents++;

        // Manageing Zones should remove display AutoHide windows
        RemoveShowingAutoHideWindows();
                               
        // Use the assigned restore object to position the Content appropriately
        if (c.Docked)
        {
          if (c.AutoHidden)
            c.AutoHideRestore.PerformRestore(this);
          else
            c.DockingRestore.PerformRestore(this);
        }
        else
          c.FloatingRestore.PerformRestore(this);

        // Enable generation hiding/hidden/shown events
        m_surpressVisibleEvents--;

        // Generate event
        OnContentShown(c);

        return true;
      }
      else
        return false;
    }

    public virtual void ShowAllContents()
    {
      m_container.SuspendLayout();

      foreach(Content c in m_contents)
        ShowContent(c);

      UpdateInsideFill();

      m_container.ResumeLayout();
    }

    public virtual void HideContent(Content c)
    {
      HideContent(c, true, true);
    }

    public virtual void HideContent(Content c, bool record, bool reorder)
    {
      // Remove it from view by removing from current WindowContent container
      if (c.Visible)
      {
        // Do not generate hiding/hidden/shown events
        m_surpressVisibleEvents++;

        // Manageing Zones should remove display AutoHide windows
        RemoveShowingAutoHideWindows();
                
        if (record)
        {
          // Tell the Content to create a new Restore object to record its current location
          c.RecordRestore();
        }

        if (c.AutoHidden)
        {
          // Remove it from its current AutoHidePanel
          c.AutoHidePanel.RemoveContent(c);
        }
        else
        {
          // Remove the Content from its current WindowContent
          c.ParentWindowContent.Contents.Remove(c);
        }
                
        if (reorder)
        {
          // Move the Content to the start of the list
          m_contents.SetIndex(0, c); 
        }

        UpdateInsideFill();

        // Enable generation hiding/hidden/shown events
        m_surpressVisibleEvents--;
                
        // Generate event
        OnContentHidden(c);
      }
    }

    public virtual void HideAllContents()
    {
      m_container.SuspendLayout();

      int count = m_contents.Count;

      // Hide in reverse order so that a ShowAll in forward order gives accurate restore
      for(int index=count-1; index>=0; index--)
      {
        // Cannot hide something already hidden
        if (m_contents[index].Visible)
        {
          // Generate event
          if (!OnContentHiding(m_contents[index]))
          {
            HideContent(m_contents[index], true, false);
          }
        }
      }

      UpdateInsideFill();

      m_container.ResumeLayout();
    }

    public virtual Window CreateWindowForContent(Content c)
    {
      return CreateWindowForContent(c, new EventHandler(OnContentClose), 
        new EventHandler(OnRestore),
        new EventHandler(OnInvertAutoHide),
        new ContextHandler(OnShowContextMenu));
    }

    public virtual Window CreateWindowForContent(Content c,
      EventHandler contentClose,
      EventHandler restore,
      EventHandler invertAutoHide,
      ContextHandler showContextMenu)
    {
      // Create new instance with correct style
      WindowContent wc = new WindowContentTabbed(this, m_visualStyle);

      WindowDetailCaption wdc;

      // Create a style specific caption detail
      if (m_visualStyle == VisualStyle.IDE)
        wdc = new WindowDetailCaptionIDE(this, contentClose, restore,
          invertAutoHide, showContextMenu);
      else
        wdc = new WindowDetailCaptionPlain(this, contentClose, restore,
          invertAutoHide, showContextMenu);

      // Add the caption to the window display
      wc.WindowDetails.Add(wdc);

      if (c != null)
      {
        // Add provided Content to this instance
        wc.Contents.Add(c);
      }

      return wc;
    }    
            
    public virtual Zone CreateZoneForContent(State zoneState)
    {
      return CreateZoneForContent(zoneState, m_container);
    }

    protected virtual Zone CreateZoneForContent(State zoneState, ContainerControl destination)
    {
      DockStyle ds;
      Direction direction;

      // Find relevant values dependant on required state
      ValuesFromState(zoneState, out ds, out direction);

      // Create a new ZoneSequence which can host Content
      ZoneSequence zs = new ZoneSequence(this, zoneState, m_visualStyle, direction, m_zoneMinMax);

      // Set the appropriate docking style
      zs.Dock = ds;

      if (destination != null)
      {
        // Add this Zone to the display
        destination.Controls.Add(zs);
      }

      return zs;
    }

    public WindowContent AddContentWithState(Content c, State newState)
    {
      // Validate the incoming Content instance is a valid reference
      // and is a current instance within our internal collection
      if ((c == null) || !m_contents.Contains(c))
        return null;
		
      // Do not generate hiding/hidden/shown events
      m_surpressVisibleEvents++;

      // Manageing Zones should remove display AutoHide windows
      RemoveShowingAutoHideWindows();
                
      // Is the window already part of a WindowContent?
      if (c.ParentWindowContent != null)
      {
        // If it used to be in a floating mode, then record state change
        if (c.ParentWindowContent.ParentZone.State == State.Floating)
          c.ContentLeavesFloating();

        // Remove the Content from its current WindowContent
        c.ParentWindowContent.Contents.Remove(c);
      }

      // Create a new Window instance appropriate for hosting a Content object
      Window w = CreateWindowForContent(c);

      ContainerControl destination = null;

      if (newState != State.Floating)
      {
        destination = m_container;
        destination.SuspendLayout();
      }

      // Create a new Zone capable of hosting a WindowContent
      Zone z = CreateZoneForContent(newState, destination);

      if (newState == State.Floating)
      {
        // Content is not in the docked state
        c.Docked = false;
			
        // destination a new floating form
        destination = new FloatingForm(this, z, new ContextHandler(OnShowContextMenu));

        // Define its location
        destination.Location = c.DisplayLocation;
				
        // ...and its size, add the height of the caption bar to the requested content size
        destination.Size = new Size(c.FloatingSize.Width, 
          c.FloatingSize.Height + SystemInformation.ToolWindowCaptionHeight);
      }
			
      // Add the Window to the Zone
      z.Windows.Add(w);

      if (newState != State.Floating)
      {
        // Set the Zone to be the least important of our Zones
        ReorderZoneToInnerMost(z);

        UpdateInsideFill();

        destination.ResumeLayout();
      }
      else
        destination.Show();

      // Enable generation hiding/hidden/shown events
      m_surpressVisibleEvents--;

      // Generate event to indicate content is now visible
      OnContentShown(c);

      return w as WindowContent;
    }

    public WindowContent AddContentToWindowContent(Content c, WindowContent wc)
    {
      // Validate the incoming Content instance is a valid reference
      // and is a current instance within our internal collection
      if ((c == null) || !m_contents.Contains(c))
        return null;

      // Validate the incoming WindowContent instance is a valid reference
      if (wc == null)
        return null;

      // Is Content already part of given Window then nothing to do
      if (c.ParentWindowContent == wc)
        return wc;
      else
      {
        // Do not generate hiding/hidden/shown events
        m_surpressVisibleEvents++;

        // Manageing Zones should remove display AutoHide windows
        RemoveShowingAutoHideWindows();
                
        if (c.ParentWindowContent != null)
        {
          // Is there a change in docking state?
          if (c.ParentWindowContent.ParentZone.State != wc.ParentZone.State)
          {
            // If it used to be in a floating mode, then record state change
            if (c.ParentWindowContent.ParentZone.State == State.Floating)
              c.ContentLeavesFloating();
            else
              c.ContentBecomesFloating();
          }

          // Remove the Content from its current WindowContent
          c.ParentWindowContent.Contents.Remove(c);
        }
        else
        {
          // If adding to a floating window then it is not docked
          if (wc.ParentZone.State == State.Floating)
            c.Docked = false;
        }

        // Add the existing Content to this instance
        wc.Contents.Add(c);

        // Enable generation hiding/hidden/shown events
        m_surpressVisibleEvents--;

        // Generate event to indicate content is now visible
        OnContentShown(c);

        return wc;
      }
    }

    public Window AddContentToZone(Content c, Zone z, int index)
    {
      // Validate the incoming Content instance is a valid reference
      // and is a current instance within our internal collection
      if ((c == null) || !m_contents.Contains(c))
        return null;

      // Validate the incoming Zone instance is a valid reference
      if (z == null) 
        return null;

      // Do not generate hiding/hidden/shown events
      m_surpressVisibleEvents++;

      // Manageing Zones should remove display AutoHide windows
      RemoveShowingAutoHideWindows();
                
      // Is the window already part of a WindowContent?
      if (c.ParentWindowContent != null)
      {
        // Is there a change in docking state?
        if (c.ParentWindowContent.ParentZone.State != z.State)
        {
          // If it used to be in a floating mode, then record state change
          if (c.ParentWindowContent.ParentZone.State == State.Floating)
            c.ContentLeavesFloating();
          else
            c.ContentBecomesFloating();
        }

        // Remove the Content from its current WindowContent
        c.ParentWindowContent.Contents.Remove(c);
      }
      else
      {
        // If target zone is floating window then we are no longer docked
        if (z.State == State.Floating)
          c.Docked = false;
      }

      // Create a new WindowContent instance according to our style
      Window w = CreateWindowForContent(c);

      // Add the Window to the Zone at given position
      z.Windows.Insert(index, w);

      // Enable generation hiding/hidden/shown events
      m_surpressVisibleEvents--;

      // Generate event to indicate content is now visible
      OnContentShown(c);

      return w;
    }

    public Rectangle InnerResizeRectangle(Control source)
    {
      // Start with a rectangle that represents the entire client area
      Rectangle client = m_container.ClientRectangle;

      int count = m_container.Controls.Count;
      int inner = m_container.Controls.IndexOf(m_innerControl);
      int sourceIndex = m_container.Controls.IndexOf(source);

      // Process each control outside the inner control
      for(int index=count-1; index>inner; index--)
      {
        Control item = m_container.Controls[index];

        bool insideSource = (index < sourceIndex);

        switch(item.Dock)
        {
          case DockStyle.Left:
            client.Width -= item.Width;
            client.X += item.Width;

            if (insideSource)
              client.Width -= item.Width;
            break;
          case DockStyle.Right:
            client.Width -= item.Width;

            if (insideSource)
            {
              client.Width -= item.Width;
              client.X += item.Width;
            }
            break;
          case DockStyle.Top:
            client.Height -= item.Height;
            client.Y += item.Height;

            if (insideSource)
              client.Height -= item.Height;
            break;
          case DockStyle.Bottom:
            client.Height -= item.Height;

            if (insideSource)
            {
              client.Height -= item.Height;
              client.Y += item.Height;
            }
            break;
          case DockStyle.Fill:
          case DockStyle.None:
            break;
        }
      }

      return client;
    }

    public void ReorderZoneToInnerMost(Zone zone)
    {
      int index = 0;

      // If there is no control specified as the one for all Zones to be placed
      // in front of then simply add the Zone at the start of the list so it is
      // in front of all controls.
      if (m_innerControl != null)
      {
        // Find position of specified control and place after it in the list 
        // (hence adding one to the returned value)
        index = m_container.Controls.IndexOf(m_innerControl) + 1;
      }

      // Find current position of the Zone to be repositioned
      int current = m_container.Controls.IndexOf(zone);

      // If the old position is before the new position then we need to 
      // subtract one. As the collection will remove the Control from the
      // old position before inserting it in the new, thus reducing the index
      // by 1 before the insert occurs.
      if (current < index)
        index--;

      // Found a Control that is not a Zone, so need to insert straight it
      m_container.Controls.SetChildIndex(zone, index);
            
      // Manageing Zones should remove display AutoHide windows
      RemoveShowingAutoHideWindows();
    }

    public void ReorderZoneToOuterMost(Zone zone)
    {
      // Get index of the outer control (minus AutoHidePanel's)
      int index = OuterControlIndex();

      // Find current position of the Zone to be repositioned
      int current = m_container.Controls.IndexOf(zone);

      // If the old position is before the new position then we need to 
      // subtract one. As the collection will remove the Control from the
      // old position before inserting it in the new, thus reducing the index
      // by 1 before the insert occurs.
      if (current < index)
        index--;

      // Found a Control that is not a Zone, so need to insert straight it
      m_container.Controls.SetChildIndex(zone, index);

      // Manageing Zones should remove display AutoHide windows
      RemoveShowingAutoHideWindows();
    }
        
    public int OuterControlIndex()
    {
      int index = m_container.Controls.Count;

      // If there is no control specified as the one for all Zones to be placed behind 
      // then simply add the Zone at the end of the list so it is behind all controls.
      if (m_outerControl != null)
      {
        // Find position of specified control and place before it in the list 
        index = m_container.Controls.IndexOf(m_outerControl);
      }

      // Adjust backwards to prevent being after any AutoHidePanels
      for(; index>0; index--)
        if (!(m_container.Controls[index-1] is AutoHidePanel))
          break;
                    
      return index;
    }

    public void RemoveShowingAutoHideWindows()
    {
      m_ahpLeft.RemoveShowingWindow();
      m_ahpRight.RemoveShowingWindow();
      m_ahpTop.RemoveShowingWindow();
      m_ahpBottom.RemoveShowingWindow();
    }
        
    internal void RemoveShowingAutoHideWindowsExcept(AutoHidePanel except)
    {
      if (except != m_ahpLeft)
        m_ahpLeft.RemoveShowingWindow();

      if (except != m_ahpRight)
        m_ahpRight.RemoveShowingWindow();
            
      if (except != m_ahpTop)
        m_ahpTop.RemoveShowingWindow();
            
      if (except != m_ahpBottom)
        m_ahpBottom.RemoveShowingWindow();
    }

    public void BringAutoHideIntoView(Content c)
    {
      if (m_ahpLeft.ContainsContent(c))
        m_ahpLeft.BringContentIntoView(c);     

      if (m_ahpRight.ContainsContent(c))
        m_ahpRight.BringContentIntoView(c);     

      if (m_ahpTop.ContainsContent(c))
        m_ahpTop.BringContentIntoView(c);     

      if (m_ahpBottom.ContainsContent(c))
        m_ahpBottom.BringContentIntoView(c);     
    }            
        
    public void ValuesFromState(State newState, out DockStyle dockState, out Direction direction)
    {
      switch(newState)
      {
        case State.Floating:
          dockState = DockStyle.Fill;
          direction = Direction.Vertical;
          break;
        case State.DockTop:
          dockState = DockStyle.Top;
          direction = Direction.Horizontal;
          break;
        case State.DockBottom:
          dockState = DockStyle.Bottom;
          direction = Direction.Horizontal;
          break;
        case State.DockRight:
          dockState = DockStyle.Right;
          direction = Direction.Vertical;
          break;
        case State.DockLeft:
        default:
          dockState = DockStyle.Left;
          direction = Direction.Vertical;
          break;
      }
    }

    public byte[] SaveConfigToArray()
    {
      return SaveConfigToArray(Encoding.Unicode);	
    }

    public byte[] SaveConfigToArray(Encoding encoding)
    {
      // Create a memory based stream
      MemoryStream ms = new MemoryStream();
			
      // Save into the file stream
      SaveConfigToStream(ms, encoding);

      // Must remember to close
      ms.Close();

      // Return an array of bytes that contain the streamed XML
      return ms.GetBuffer();
    }

    public void SaveConfigToFile(string filename)
    {
      SaveConfigToFile(filename, Encoding.Unicode);
    }

    public void SaveConfigToFile(string filename, Encoding encoding)
    {
      // Create/Overwrite existing file
      FileStream fs = new FileStream(filename, FileMode.Create);
			
      // Save into the file stream
      SaveConfigToStream(fs, encoding);		

      // Must remember to close
      fs.Close();
    }

    public void SaveConfigToStream(Stream stream, Encoding encoding)
    {
      XmlTextWriter xmlOut = new XmlTextWriter(stream, encoding); 

      // Use indenting for readability
      xmlOut.Formatting = Formatting.Indented;
			
      // Always begin file with identification and warning
      xmlOut.WriteStartDocument();
      xmlOut.WriteComment(" Magic, The User Interface library for .NET (www.dotnetmagic.com) ");
      xmlOut.WriteComment(" Modifying this generated file will probably render it invalid ");

      // Associate a version number with the root element so that future version of the code
      // will be able to be backwards compatible or at least recognise out of date versions
      xmlOut.WriteStartElement("DockingConfig");
      xmlOut.WriteAttributeString("FormatVersion", "5");
      xmlOut.WriteAttributeString("InsideFill", m_insideFill.ToString());
      xmlOut.WriteAttributeString("InnerMinimum", ConversionHelper.SizeToString(m_innerMinimum));

      // We need to hide all content during the saving process, but then restore
      // them back again before leaving so the user does not see any change
      m_container.SuspendLayout();

      // Store a list of those content hidden during processing
      ContentCollection hideContent = new ContentCollection();

      // Let create a copy of the current contents in current order, because
      // we cannot 'foreach' a collection that is going to be altered during its
      // processing by the 'HideContent'.
      ContentCollection origContents = m_contents.Copy();

      // Do not generate hiding/hidden/shown events
      m_surpressVisibleEvents++;
            
      int count = origContents.Count;

      // Hide in reverse order so that a ShowAll in forward order gives accurate restore
      for(int index=count-1; index>=0; index--)
      {
        Content c = origContents[index];
            
        c.RecordRestore();
        c.SaveToXml(xmlOut);

        // If visible then need to hide so that subsequent attempts to 
        // RecordRestore will not take its position into account
        if (c.Visible)
        {
          hideContent.Insert(0, c);
          HideContent(c);
        }
      }
			
      // Allow an event handler a chance to add custom information after ours
      OnSaveCustomConfig(xmlOut);

      // Put content we hide back again
      foreach(Content c in hideContent)
        ShowContent(c);

      // Enable generation of hiding/hidden/shown events
      m_surpressVisibleEvents--;
            
      // Reapply any fill style required
      AddInnerFillStyle();

      m_container.ResumeLayout();

      // Terminate the root element and document        
      xmlOut.WriteEndElement();
      xmlOut.WriteEndDocument();

      // This should flush all actions and close the file
      xmlOut.Close();			
    }

    public void LoadConfigFromArray(byte[] buffer)
    {
      // Create a memory based stream
      MemoryStream ms = new MemoryStream(buffer);
			
      // Save into the file stream
      LoadConfigFromStream(ms);

      // Must remember to close
      ms.Close();
    }

    public void LoadConfigFromFile(string filename)
    {
      // Open existing file
      FileStream fs = new FileStream(filename, FileMode.Open);
			
      // Load from the file stream
      LoadConfigFromStream(fs);		

      // Must remember to close
      fs.Close();
    }

    public void LoadConfigFromStream(Stream stream)
    {
      XmlTextReader xmlIn = new XmlTextReader(stream); 

      // Ignore whitespace, not interested
      xmlIn.WhitespaceHandling = WhitespaceHandling.None;

      // Moves the reader to the root element.
      xmlIn.MoveToContent();

      // Double check this has the correct element name
      if (xmlIn.Name != "DockingConfig")
        throw new ArgumentException("Root element must be 'DockingConfig'");

      // Load the format version number
      string version = xmlIn.GetAttribute(0);
      string insideFill = xmlIn.GetAttribute(1);
      string innerSize = xmlIn.GetAttribute(2);

      // Convert format version from string to double
      int formatVersion = (int)System.Convert.ToDouble(version);
            
      // We can only load 3 upward version formats
      if (formatVersion < 3)
        throw new ArgumentException("Can only load Version 3 and upwards Docking Configuration files");

      // Convert from string to proper types
      m_insideFill = (bool)System.Convert.ToBoolean(insideFill);
      m_innerMinimum = ConversionHelper.StringToSize(innerSize);

      ContentCollection cc = new ContentCollection();

      do
      {
        // Read the next Element
        if (!xmlIn.Read())
          throw new ArgumentException("An element was expected but could not be read in");

        // Have we reached the end of root element?
        if ((xmlIn.NodeType == XmlNodeType.EndElement) && (xmlIn.Name == "DockingConfig"))
          break;

        // Is the element name 'Content'
        if (xmlIn.Name == "Content")
        {
          // Process this Content element
          cc.Insert(0, new Content(xmlIn, formatVersion));
        }
        else
        {
          // Must have reached end of our code, let the custom handler deal with this
          OnLoadCustomConfig(xmlIn);

          // Ignore anything else that might be in the XML
          xmlIn.Close();			
                   
          // Exit
          break;
        }

      } while(!xmlIn.EOF);

      xmlIn.Close();			

      // Reduce flicker during window operations
      m_container.SuspendLayout();

      // Hide all the current content items
      HideAllContents();

      // Attempt to apply loaded settings
      foreach(Content loaded in cc)
      {
        Content c = m_contents[loaded.Title];

        // Do we have any loaded information for this item?
        if (c != null)
        {
          // Copy across the loaded values of interest
          c.Docked = loaded.Docked;
          c.AutoHidden = loaded.AutoHidden;
          c.CaptionBar = loaded.CaptionBar;
          c.CloseButton = loaded.CloseButton;
          c.DisplaySize = loaded.DisplaySize;
          c.DisplayLocation = loaded.DisplayLocation;
          c.AutoHideSize = loaded.AutoHideSize;
          c.FloatingSize = loaded.FloatingSize;
          c.DefaultRestore = loaded.DefaultRestore;
          c.AutoHideRestore = loaded.AutoHideRestore;
          c.DockingRestore = loaded.DockingRestore;
          c.FloatingRestore = loaded.FloatingRestore;

          // Allow the Restore objects a chance to rehook into object instances
          c.ReconnectRestore();					

          // Was the loaded item visible?
          if (loaded.Visible)
          {
            // Make it visible now
            ShowContent(c);
          }
        }
      }

      // Reapply any fill style required
      AddInnerFillStyle();

      // Reduce flicker during window operations
      m_container.ResumeLayout();
			
      // If any AutoHostPanel's have become visible we need to force a repaint otherwise
      // the area not occupied by the TabStub instances will be painted the correct color
      m_ahpLeft.Invalidate();
      m_ahpRight.Invalidate();
      m_ahpTop.Invalidate();
      m_ahpBottom.Invalidate();
    }
        
    public void PropogateNameValue(PropogateName name, object value)
    {
      foreach(Control c in m_container.Controls)
      {
        Zone z = c as Zone;

        // Only interested in our Zones
        if (z != null)
          z.PropogateNameValue(name, value);
      }

      // If the docking manager is created for a Container that does not
      // yet have a parent control then we need to double check before
      // trying to enumerate the owned forms.
      if (m_container.FindForm() != null)
      {
        foreach(Form f in m_container.FindForm().OwnedForms)
        {
          FloatingForm ff = f as FloatingForm;
                    
          // Only interested in our FloatingForms
          if (ff != null)
            ff.PropogateNameValue(name, value);
        }
      }
            
      // Propogate into the AutoHidePanel objects
      m_ahpTop.PropogateNameValue(name, value);
      m_ahpLeft.PropogateNameValue(name, value);
      m_ahpRight.PropogateNameValue(name, value);
      m_ahpBottom.PropogateNameValue(name, value);
    }

    public virtual bool OnContentHiding(Content c)
    {
      CancelEventArgs cea = new CancelEventArgs();

      if (m_surpressVisibleEvents == 0)
      {
        // Allow user to prevent hide operation                
        if (ContentHiding != null)
          ContentHiding(c, cea);
      }
            
      // Was action cancelled?                        
      return cea.Cancel;
    }

    public virtual void OnContentHidden(Content c)
    {
      if (m_surpressVisibleEvents == 0)
      {
        // Notify operation has completed
        if (ContentHidden != null)
          ContentHidden(c, EventArgs.Empty);
      }
    }

    public virtual void OnContentShown(Content c)
    {
      if (m_surpressVisibleEvents == 0)
      {
        // Notify operation has completed
        if (ContentShown != null)
          ContentShown(c, EventArgs.Empty);
      }
    }

    public virtual void OnTabControlCreated(Controls.TabControl tabControl)
    { 
      // Notify interested parties about creation of a new TabControl instance
      if (TabControlCreated != null)
        TabControlCreated(tabControl);
    }
		
    public virtual void OnSaveCustomConfig(XmlTextWriter xmlOut)
    {
      // Notify interested parties that they can add their own custom data
      if (SaveCustomConfig != null)
        SaveCustomConfig(xmlOut);
    }

    public virtual void OnLoadCustomConfig(XmlTextReader xmlIn)
    {
      // Notify interested parties that they can add their own custom data
      if (LoadCustomConfig != null)
        LoadCustomConfig(xmlIn);
    }
        
    protected virtual void OnContentsClearing()
    {
      m_container.SuspendLayout();

      // Remove each Content from any WindowContent it is inside
      foreach(Content c in m_contents)
      {
        // Is the Content inside a WindowContent?
        if (c.ParentWindowContent != null)
          c.ParentWindowContent.Contents.Remove(c);
      }

      m_container.ResumeLayout();
    }

    protected virtual void OnContentRemoved(int index, object value)
    {
      m_container.SuspendLayout();

      Content c = value as Content;

      if (c != null)
      {
        // Is the Content inside a WindowContent?
        if (c.ParentWindowContent != null)
          c.ParentWindowContent.Contents.Remove(c);
      }

      m_container.ResumeLayout();
    }

    protected virtual void OnContentClose(object sender, EventArgs e)
    {
      WindowDetailCaption wdc = sender as WindowDetailCaption;
            
      // Was Close generated by a Caption detail?
      if (wdc != null)
      {
        WindowContentTabbed wct = wdc.ParentWindow as WindowContentTabbed;
                
        // Is the Caption part of a WindowContentTabbed object?
        if (wct != null)
        {
          // Find the Content object that is the target
          Content c = wct.CurrentContent;
                    
          if (c != null)
          {
            // Was action cancelled?                        
            if (!OnContentHiding(c))
              wct.HideCurrentContent();
          }
        }
      }
    }
        
    protected virtual void OnInvertAutoHide(object sender, EventArgs e)
    {
      // Do not generate hiding/hidden/shown events
      m_surpressVisibleEvents++;
        
      WindowDetail detail = sender as WindowDetail;

      // Get access to Content that initiated AutoHide for its Window
      WindowContent wc = detail.ParentWindow as WindowContent;
                        
      // Create a collection of the Content in the same window
      ContentCollection cc = new ContentCollection();
            
      // Add all Content into collection
      foreach(Content c in wc.Contents)
        cc.Add(c);

      // Add to the correct AutoHidePanel
      AutoHideContents(cc, wc.State);

      // Enable generate hiding/hidden/shown events
      m_surpressVisibleEvents--;
    }
        
    internal AutoHidePanel AutoHidePanelForState(State state)
    {
      AutoHidePanel ahp = null;

      // Grab the correct hosting panel
      switch(state)
      {
        case State.DockLeft:
          ahp = m_ahpLeft;
          break;
        case State.DockRight:
          ahp = m_ahpRight;
          break;
        case State.DockTop:
          ahp = m_ahpTop;
          break;
        case State.DockBottom:
          ahp = m_ahpBottom;
          break;
      }

      return ahp;
    }
        
    internal void AutoHideContents(ContentCollection cc, State state)
    {
      // Hide all the Content instances. This will cause the restore objects to be 
      // created and so remember the docking positions for when they are restored
      foreach(Content c in cc)
        HideContent(c);

      AutoHidePanel ahp = AutoHidePanelForState(state);

      // Pass management of Contents into the panel            
      ahp.AddContentsAsGroup(cc);
    }

    internal AutoHidePanel AutoHidePanelForContent(Content c)
    {
      if (m_ahpLeft.ContainsContent(c))
        return m_ahpLeft;     

      if (m_ahpRight.ContainsContent(c))
        return m_ahpRight;     

      if (m_ahpTop.ContainsContent(c))
        return m_ahpTop;     

      if (m_ahpBottom.ContainsContent(c))
        return m_ahpBottom;     
                
      return null;
    }

    internal int SurpressVisibleEvents
    {
      get { return m_surpressVisibleEvents; }
      set { m_surpressVisibleEvents = value; }
    }

    protected void AddAutoHidePanels()
    {
      // Create an instance for each container edge (they default to being hidden)
      m_ahpTop = new AutoHidePanel(this, DockStyle.Top);
      m_ahpLeft = new AutoHidePanel(this, DockStyle.Left);
      m_ahpBottom = new AutoHidePanel(this, DockStyle.Bottom);
      m_ahpRight = new AutoHidePanel(this, DockStyle.Right);
        
      m_ahpTop.Name = "Top";
      m_ahpLeft.Name = "Left";
      m_ahpBottom.Name = "Bottom";
      m_ahpRight.Name = "Right";
		    
      // Add to the end of the container we manage
      m_container.Controls.AddRange(new Control[]{m_ahpBottom, m_ahpTop, m_ahpRight, m_ahpLeft});
    }
		            
    protected void RepositionControlBefore(Control target, Control source)
    {
      // Find indexs of the two controls
      int targetPos = m_container.Controls.IndexOf(target);
      int sourcePos = m_container.Controls.IndexOf(source);

      // If the source is being moved further up the list then we must decrement the target index 
      // as the move is carried out in two phases. First the source control is removed from the 
      // collection and then added at the given requested index. So when insertion point needs 
      // ahjusting to reflec the fact the control has been removed before being inserted.
      if (targetPos >= sourcePos)
        targetPos--;

      m_container.Controls.SetChildIndex(source, targetPos);			
    }

    protected virtual void OnRestore(object sender, EventArgs e)
    {
      WindowDetailCaption wdc = sender as WindowDetailCaption;

      // Was Restore generated by a Caption detail?
      if (wdc != null)
      {
        RemoveAnyFillStyle();

        WindowContent wc = wdc.ParentWindow as WindowContent;

        // Is the Caption part of a WindowContent object?
        if (wc != null)
        {
          ContentCollection copy = new ContentCollection();

          // Make every Content of the WindowContent record its
          // current position and remember it for when the future
          foreach(Content c in wc.Contents)
          {
            c.RecordRestore();

            // Invert docked status
            c.Docked = (c.Docked == false);

            copy.Add(c);
          }

          int copyCount = copy.Count;

          // Must have at least one!
          if (copyCount >= 1)
          {
            // Remove from current WindowContent and restore its position
            HideContent(copy[0], false, true);
            ShowContent(copy[0]);

            // Any other content to be moved along with it?
            if (copyCount >= 2)
            {
              WindowContent newWC = copy[0].ParentWindowContent;

              if (newWC != null)
              {
                // Transfer each one to its new location
                for(int index=1; index<copyCount; index++)
                {
                  HideContent(copy[index], false, true);
                  newWC.Contents.Add(copy[index]);
                }
              }
            }
          }
        }

        AddInnerFillStyle();
      }
    }

    protected void AddInnerFillStyle()
    {
      if (m_insideFill)
      {
        // Find the innermost Zone which must be the first one in the collection
        foreach(Control c in m_container.Controls)
        {
          Zone z = c as Zone;

          // Only interested in our Zones
          if (z != null)
          {
            // Make it fill all remaining space
            z.Dock = DockStyle.Fill;

            // Exit
            break;
          }
        }
      }
    }

    protected void RemoveAnyFillStyle()
    {
      // Check each Zone in the container
      foreach(Control c in m_container.Controls)
      {
        Zone z = c as Zone;

        if (z != null)
        {
          // Only interested in ones with the Fill dock style
          if (z.Dock == DockStyle.Fill)
          {
            DockStyle ds;
            Direction direction;

            // Find relevant values dependant on required state
            ValuesFromState(z.State, out ds, out direction);
			
            // Reassign its correct Dock style
            z.Dock = ds;
          }
        }
      }
    }

    protected void OnFormLoaded(object sender, EventArgs e)
    {
      // A Form can cause the child controls to be reordered after the initialisation
      // but before the Form.Load event. To handle this we reorder the auto hide panels
      // on the Form.Load event to ensure they are correctly positioned.
      ReorderAutoHidePanels();
    }

    protected void ReorderAutoHidePanels()
    {
      if (m_outerControl == null)
      {
        int count = m_container.Controls.Count;

        // Position the AutoHidePanel's at end of controls
        m_container.Controls.SetChildIndex(m_ahpLeft, count - 1);
        m_container.Controls.SetChildIndex(m_ahpRight, count - 1);
        m_container.Controls.SetChildIndex(m_ahpTop, count - 1);
        m_container.Controls.SetChildIndex(m_ahpBottom, count - 1);
      }
      else
      {
        // Position the AutoHidePanel's as last items before OuterControl
        RepositionControlBefore(m_outerControl, m_ahpBottom);
        RepositionControlBefore(m_outerControl, m_ahpTop);
        RepositionControlBefore(m_outerControl, m_ahpRight);
        RepositionControlBefore(m_outerControl, m_ahpLeft);
      }
    }
        
    protected void OnContainerResized(object sender, EventArgs e)
    {
      if (m_autoResize)
      {
        Rectangle inner = InnerResizeRectangle(null);			

        // Shrink by the minimum size
        inner.Width -= m_innerMinimum.Width;
        inner.Height -= m_innerMinimum.Height;
    			
        Form f = m_container as Form;

        // If the container is a Form then ignore resizing because of becoming Minimized
        if ((f == null) || ((f != null) && (f.WindowState != FormWindowState.Minimized)))
        {
          if ((inner.Width < 0) || (inner.Height < 0))
          {
            m_container.SuspendLayout();

            ZoneCollection zcLeft = new ZoneCollection();
            ZoneCollection zcRight = new ZoneCollection();
            ZoneCollection zcTop = new ZoneCollection();
            ZoneCollection zcBottom = new ZoneCollection();

            // Construct a list of the docking windows on the left and right edges
            foreach(Control c in m_container.Controls)
            {
              Zone z = c as Zone;

              if (z != null)
              {
                switch(z.State)
                {
                  case State.DockLeft:
                    zcLeft.Add(z);
                    break;
                  case State.DockRight:
                    zcRight.Add(z);
                    break;
                  case State.DockTop:
                    zcTop.Add(z);
                    break;
                  case State.DockBottom:
                    zcBottom.Add(z);
                    break;
                }
              }
            }

            if (inner.Width < 0)
              ResizeDirection(-inner.Width, zcLeft, zcRight, Direction.Horizontal);

            if (inner.Height < 0)
              ResizeDirection(-inner.Height, zcTop, zcBottom, Direction.Vertical);

            m_container.ResumeLayout();
          }
        }
      }
    }

    protected void ResizeDirection(int remainder, ZoneCollection zcAlpha, ZoneCollection zcBeta, Direction dir)
    {
      bool alter;
      int available;
      int half1, half2;

      // Keep going till all space found or nowhere to get it from
      while((remainder > 0) && ((zcAlpha.Count > 0) || (zcBeta.Count > 0)))
      {
        if (dir == Direction.Horizontal)
        {
          m_firstHalfWidth = (m_firstHalfWidth != true);
          alter = m_firstHalfWidth;
        }
        else
        {
          m_firstHalfHeight = (m_firstHalfHeight != true);
          alter = m_firstHalfHeight;
        }

        // Alternate between left and right getting the remainder
        if (alter)
        {
          half1 = (remainder / 2) + 1;
          half2 = remainder - half1;
        }
        else
        {
          half2 = (remainder / 2) + 1;
          half1 = remainder - half2;
        }

        // Any Zone of the left to use?
        if (zcAlpha.Count > 0)
        {
          Zone z = zcAlpha[0];

          // Find how much space it can offer up
          if (dir == Direction.Horizontal)
            available = z.Width - z.MinimumWidth;
          else
            available = z.Height - z.MinimumHeight;

          if (available > 0)
          {
            // Only take away the maximum we need
            if (available > half1)
              available = half1;
            else
              zcAlpha.Remove(z);

            // Resize the control accordingly
            if (dir == Direction.Horizontal)
              z.Width = z.Width - available;
            else
              z.Height = z.Height - available;

            // Reduce total amount left to allocate
            remainder -= available;
          }
          else
            zcAlpha.Remove(z);
        }

        // Any Zone of the left to use?
        if (zcBeta.Count > 0)
        {
          Zone z = zcBeta[0];

          // Find how much space it can offer up
          if (dir == Direction.Horizontal)
            available = z.Width - z.MinimumWidth;
          else
            available = z.Height - z.MinimumHeight;

          if (available > 0)
          {
            // Only take away the maximum we need
            if (available > half2)
              available = half2;
            else
              zcBeta.Remove(z);

            // Resize the control accordingly
            if (dir == Direction.Horizontal)
              z.Width = z.Width - available;
            else
              z.Height = z.Height - available;

            // Reduce total amount left to allocate
            remainder -= available;
          }
          else
            zcBeta.Remove(z);
        }
      }
    }

    protected void OnPreferenceChanged(object sender, UserPreferenceChangedEventArgs e)
    {
      if (m_defaultBackColor)
      {
        m_backColor = SystemColors.Control;
        PropogateNameValue(PropogateName.BackColor, (object)SystemColors.Control);
      }

      if (m_defaultActiveColor)
      {
        m_activeColor = SystemColors.ActiveCaption;
        PropogateNameValue(PropogateName.ActiveColor, (object)SystemColors.ActiveCaption);
      }
            
      if (m_defaultActiveTextColor)
      {
        m_activeTextColor = SystemColors.ActiveCaptionText;
        PropogateNameValue(PropogateName.ActiveTextColor, (object)SystemColors.ActiveCaptionText);
      }

      if (m_defaultInactiveTextColor)
      {
        m_inactiveTextColor = SystemColors.ControlText;
        PropogateNameValue(PropogateName.InactiveTextColor, (object)SystemColors.ControlText);
      }

      if (m_defaultResizeBarColor)
      {
        m_resizeBarColor = SystemColors.Control;
        PropogateNameValue(PropogateName.ResizeBarColor, (object)SystemColors.Control);
      }

      if (m_defaultCaptionFont)
      {
        m_captionFont = SystemInformation.MenuFont;
        PropogateNameValue(PropogateName.CaptionFont, (object)SystemInformation.MenuFont);
      }

      if (m_defaultTabControlFont)
      {
        m_tabControlFont = SystemInformation.MenuFont;
        PropogateNameValue(PropogateName.TabControlFont, (object)SystemInformation.MenuFont);
      }
    }

    public virtual void OnShowContextMenu(Point screenPos)
    {
      PopupMenu context = new PopupMenu();

      // The order of Content displayed in the context menu is not the same as
      // the order of Content in the m_contents collection. The latter has its
      // ordering changed to enable Restore functionality to work.
      ContentCollection temp = new ContentCollection();

      foreach(Content c in m_contents)
      {
        int count = temp.Count;
        int index = 0;

        // Find best place to add into the temp collection
        for(; index<count; index++)
        {
          if (c.Order < temp[index].Order)
            break;
        }

        temp.Insert(index, c);
      }

      // Create a context menu entry per Content
      foreach(Content t in temp)
      {
        MenuCommand mc = new MenuCommand(t.Title, new EventHandler(OnToggleContentVisibility));
        mc.Checked = t.Visible;
        mc.Tag = t;

        context.MenuCommands.Add(mc);
      }

      // Add a separator 
      context.MenuCommands.Add(new MenuCommand("-"));

      // Add fixed entries to end to effect all content objects
      context.MenuCommands.Add(new MenuCommand("Show All", new EventHandler(OnShowAll)));
      context.MenuCommands.Add(new MenuCommand("Hide All", new EventHandler(OnHideAll)));

      // Ensure menu has same style as the docking windows
      context.Style = m_visualStyle;

      if (OnContextMenu(context))
      {
        // Show it!
        context.TrackPopup(screenPos);
      }
    }

    protected bool OnContextMenu(PopupMenu context)
    {
      CancelEventArgs cea = new CancelEventArgs();
        
      if (ContextMenu != null)
        ContextMenu(context, cea);
                
      return !cea.Cancel;
    }

    protected void OnToggleContentVisibility(object sender, EventArgs e)
    {
      MenuCommand mc = sender as MenuCommand;

      if (mc != null)
      {
        Content c = mc.Tag as Content;

        if (c != null)
        {
          if (c.Visible)
            HideContent(c);
          else
            ShowContent(c);
        }
      }
    }

    protected void OnShowAll(object sender, EventArgs e)
    {
      ShowAllContents();
    }

    protected void OnHideAll(object sender, EventArgs e)
    {
      HideAllContents();
    }
  }
}

#region revision history

// $Log: DockingManager.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/DockingManager.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: DockingManager.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

