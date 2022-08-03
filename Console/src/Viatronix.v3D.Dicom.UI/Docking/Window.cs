// $Id: Window.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  [ToolboxItem(false)]
  public class Window : ContainerControl
  {
    // Instance fields
    protected State m_state;
    protected Zone m_parentZone;
    protected WindowDetailCollection m_windowDetails;
    protected Decimal m_zoneArea;
    protected Size m_minimalSize;
    protected DockingManager m_manager;
    protected bool m_autoDispose;
    protected bool m_redockAllowed;
    protected bool m_floatingCaption;
    protected bool m_contentCaption;
    protected string m_fullTitle;

    // Instance events
    public event EventHandler FullTitleChanged; 

    public Window(DockingManager manager)
    {
      // Must provide a valid manager instance
      if (manager == null)
        throw new ArgumentNullException("DockingManager");

      // Default object state
      m_state = State.Floating;
      m_parentZone = null;
      m_zoneArea = 100m;
      m_minimalSize = new Size(0,0);
      m_manager = manager;
      m_autoDispose = true;
      m_fullTitle = "";
      m_redockAllowed = true;
      m_floatingCaption = true;
      m_contentCaption = true;

      // Create collection of window details
      m_windowDetails = new WindowDetailCollection();

      // We want notification when window details are added/removed/cleared
      m_windowDetails.Clearing += new CollectionClear(OnDetailsClearing);
      m_windowDetails.Inserted += new CollectionChange(OnDetailInserted);
      m_windowDetails.Removing += new CollectionChange(OnDetailRemoving);
    }

    public DockingManager DockingManager
    {
      get { return m_manager; }
    }

    public State State
    {
      get { return m_state; }
			
      set 
      {
        if (m_state != value)
        {
          m_state = value;

          // Inform each window detail of the change in state
          foreach(WindowDetail wd in m_windowDetails)
            wd.ParentStateChanged(m_state);
        }
      }
    }

    public Zone ParentZone
    {
      get { return m_parentZone; }
			
      set 
      { 
        if (m_parentZone != value)
        {
          m_parentZone = value; 

          // Inform each window detail of the change in zone
          foreach(WindowDetail wd in m_windowDetails)
            wd.ParentZone = m_parentZone;
        }
      }
    }

    public WindowDetailCollection WindowDetails
    {
      get { return m_windowDetails; }
			
      set
      {
        m_windowDetails.Clear();
        m_windowDetails = value;
      }
    }

    public Decimal ZoneArea
    {
      get { return m_zoneArea; }
      set { m_zoneArea = value; }
    }

    public Size MinimalSize
    {
      get { return m_minimalSize; }
      set { m_minimalSize = value; }
    }

    public bool AutoDispose
    {
      get { return m_autoDispose; }
      set { m_autoDispose = value; }
    }

    public string FullTitle
    {
      get { return m_fullTitle; }
    }

    public bool RedockAllowed
    {
      get { return m_redockAllowed; }
      set { m_redockAllowed = value; }
    }

    protected void OnDetailsClearing()
    {
      // Inform each detail it no longer has a parent
      foreach(WindowDetail wd in m_windowDetails)
      {
        // Inform each detail it no longer has a parent
        wd.ParentWindow = null;

        // Inform object that it is no longer in a Zone
        wd.ParentZone = null;
      }
    }

    protected void OnDetailInserted(int index, object value)
    {
      WindowDetail wd = value as WindowDetail;

      // Inform object we are the new parent
      wd.ParentWindow = this;

      // Inform object that it is in a Zone
      wd.ParentZone = m_parentZone;
    }

    protected void OnDetailRemoving(int index, object value)
    {
      WindowDetail wd = value as WindowDetail;

      // Inform object it no longer has a parent
      wd.ParentWindow = null;
			
      // Inform object that it is no longer in a Zone
      wd.ParentZone = null;
    }
		
    public virtual void NotifyFullTitleText(string title)
    {
      // Inform each detail of change in focus
      foreach(WindowDetail wd in m_windowDetails)
        wd.NotifyFullTitleText(title);
                
      OnFullTitleChanged(title);
    }

    public virtual void NotifyAutoHideImage(bool autoHidden)
    {
      // Inform each detail of change in caption bar
      foreach(WindowDetail wd in m_windowDetails)
        wd.NotifyAutoHideImage(autoHidden);
    }

    public virtual void NotifyShowCaptionBar(bool show)
    {
      // Remember the per-content requested caption
      m_contentCaption = show;
        
      // If priority value always showing then we can let the
      // individual content decide on visibility. Otherwise
      // the priority forces it to remain hidden
      if (m_floatingCaption)
      {
        // Inform each detail of change in caption bar
        foreach(WindowDetail wd in m_windowDetails)
          wd.NotifyShowCaptionBar(show);
      }
    }

    public virtual void NotifyCloseButton(bool show)
    {
      // Inform each detail of change close button
      foreach(WindowDetail wd in m_windowDetails)
        wd.NotifyCloseButton(show);
    }

    public virtual void NotifyHideButton(bool show)
    {
      // Inform each detail of change close button
      foreach(WindowDetail wd in m_windowDetails)
        wd.NotifyHideButton(show);
    }

    public virtual void NotifyContentGotFocus()
    {
      // Inform each detail of change in focus
      foreach(WindowDetail wd in m_windowDetails)
        wd.WindowGotFocus();
    }

    public virtual void NotifyContentLostFocus()
    {
      // Inform each detail of change in focus
      foreach(WindowDetail wd in m_windowDetails)
        wd.WindowLostFocus();
    }

    public virtual void WindowDetailGotFocus(WindowDetail wd)
    {
      NotifyContentGotFocus();
    }
		
    public virtual void WindowDetailLostFocus(WindowDetail wd)
    {
      NotifyContentLostFocus();
    }
        
    public void HideDetails()
    {
      // Inform each detail of change in visibility
      foreach(WindowDetail wd in m_windowDetails)
        wd.Hide();
                
      // Remember priority state for caption
      m_floatingCaption = false;
    }

    public void ShowDetails()
    {
      // Inform each detail of change in visibility
      foreach(WindowDetail wd in m_windowDetails)
        wd.Show();

      // Remember priority state for caption
      m_floatingCaption = true;
            
      // If the content requested the caption be hidden
      if (!m_contentCaption)
        NotifyShowCaptionBar(m_contentCaption);
    }
        
    public virtual void OnFullTitleChanged(String fullTitle)
    {
      m_fullTitle = fullTitle;
            
      if (FullTitleChanged != null)
        FullTitleChanged((object)fullTitle, EventArgs.Empty);
    }

    public virtual Restore RecordRestore(object child) 
    {
      // Do we have a Zone as our parent?
      if (m_parentZone != null)
      {
        // Delegate to the Zone as we cannot help out
        return m_parentZone.RecordRestore(this, child, null);
      }

      return null;
    }

    public virtual void PropogateNameValue(PropogateName name, object value)
    {
      if (name == PropogateName.BackColor)
      {
        this.BackColor = (Color)value;
        Invalidate();
      }

      // Pass onto each of our child Windows
      foreach(WindowDetail wd in m_windowDetails)
        wd.PropogateNameValue(name, value);
    }
  }
}

#region revision history

// $Log: Window.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Window.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: Window.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

