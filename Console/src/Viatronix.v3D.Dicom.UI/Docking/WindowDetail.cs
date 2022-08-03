// $Id: WindowDetail.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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

namespace Viatronix.v3D.Dicom.UI.Docking
{
  [ToolboxItem(false)]
  public class WindowDetail : Control
  {
    // Instance fields
    protected Zone m_parentZone;
    protected Window m_parentWindow;
    protected DockingManager m_manager;

    public WindowDetail(DockingManager manager)
    {
      // Default the state
      m_parentZone = null;
      m_parentWindow = null;
      m_manager = manager;
            
      // Get correct starting state from manager
      this.BackColor = m_manager.BackColor;            
      this.ForeColor = m_manager.InactiveTextColor;
    }
		
    public virtual Zone ParentZone
    { 
      get { return m_parentZone; }
      set { m_parentZone = value; }
    }

    public Window ParentWindow
    { 
      get { return m_parentWindow; }
			
      set 
      {
        // Call virtual function for derived classes to override
        RemovedFromParent(m_parentWindow);

        if (value == null)
        {
          if (m_parentWindow != null)
          {
            // Remove ourself from old parent window

            // Use helper method to circumvent form Close bug
            ControlHelper.Remove(m_parentWindow.Controls, this);
          }
        }
        else
        {
          if ((m_parentWindow != null) && (m_parentWindow != value))
          {
            // Call virtual function for derived classes to override
            RemovedFromParent(m_parentWindow);

            // Remove ourself from old parent window

            // Use helper method to circumvent form Close bug
            ControlHelper.Remove(m_parentWindow.Controls, this);
          }
	
          // Add ourself to the new parent window
          value.Controls.Add(this);
        }

        // Remember the new parent identity
        m_parentWindow = value;

        // Call virtual function for derived classes to override
        AddedToParent(m_parentWindow);

        if (m_parentWindow != null)
        {
          // Update to reflect new parent window state
          ParentStateChanged(m_parentWindow.State);
        }
      }
    }

    public virtual void WindowGotFocus() {}
    public virtual void WindowLostFocus() {}
    public virtual void NotifyRedockAllowed(bool redockAllowed) {}
    public virtual void NotifyAutoHideImage(bool autoHidden) {}
    public virtual void NotifyCloseButton(bool show) {}
    public virtual void NotifyHideButton(bool show) {}
    public virtual void NotifyShowCaptionBar(bool show) {}
    public virtual void NotifyFullTitleText(string title) {}
    public virtual void ParentStateChanged(State newState) {}
    public virtual void RemovedFromParent(Window parent) {}
    public virtual void AddedToParent(Window parent) {}

    public virtual void PropogateNameValue(PropogateName name, object value)
    {
      if (name == PropogateName.BackColor)
      {
        this.BackColor = (Color)value;
        Invalidate();
      }
    }

    protected override void OnGotFocus(EventArgs e)
    {
      // Inform parent window we have the focus
      if (m_parentWindow != null)
        m_parentWindow.WindowDetailGotFocus(this);

      base.OnGotFocus(e);
    }

    protected override void OnLostFocus(EventArgs e)
    {
      // Inform parent window we have lost focus
      if (m_parentWindow != null)
        m_parentWindow.WindowDetailLostFocus(this);

      base.OnLostFocus(e);
    }		
  }
}

#region revision history

// $Log: WindowDetail.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/WindowDetail.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: WindowDetail.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

