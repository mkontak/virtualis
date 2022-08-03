// $Id: Zone.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
  public class Zone : Panel
  {
    // Instance fields
    protected State m_state;
    protected bool m_autoDispose;
    protected DockingManager m_manager;
    protected WindowCollection m_windows;

    public Zone(DockingManager manager)
    {
      InternalConstruct(manager, State.DockLeft);
    }

    public Zone(DockingManager manager, State state)
    {
      InternalConstruct(manager, state);
    }

    protected void InternalConstruct(DockingManager manager, State state)
    {
      // Must provide a valid manager instance
      if (manager == null)
        throw new ArgumentNullException("DockingManager");

      // Remember initial state
      m_state = state;
      m_manager = manager;
      m_autoDispose = true;

      // Get correct starting state from manager
      this.BackColor = m_manager.BackColor;
      this.ForeColor = m_manager.InactiveTextColor;

      // Create collection of windows
      m_windows = new WindowCollection();

      // We want notification when contents are added/removed/cleared
      m_windows.Clearing += new CollectionClear(OnWindowsClearing);
      m_windows.Inserted += new CollectionChange(OnWindowInserted);
      m_windows.Removing += new CollectionChange(OnWindowRemoving);
      m_windows.Removed += new CollectionChange(OnWindowRemoved);
    }

    public virtual State State
    {
      get { return m_state; }
      set { m_state = value; }
    }

    public bool AutoDispose
    {
      get { return m_autoDispose; }
      set { m_autoDispose = value; }
    }

    public DockingManager DockingManager
    {
      get { return m_manager; }
    }

    public WindowCollection Windows
    {
      get { return m_windows; }

      set
      {
        m_windows.Clear();
        m_windows = value;
      }
    }

    public virtual Restore RecordRestore(Window w, object child, Restore childRestore) { return null; }
    public virtual int MinimumWidth { get { return 0; } }
    public virtual int MinimumHeight { get { return 0; } }

    public virtual void PropogateNameValue(PropogateName name, object value)
    {
      if (name == PropogateName.BackColor)
      {
        this.BackColor = (Color)value;
        Invalidate();
      }

      // Pass onto each of our child Windows
      foreach(Window w in m_windows)
        w.PropogateNameValue(name, value);
    }

    protected virtual void OnWindowsClearing()
    {
      foreach(Control c in Controls)
      {
        Window w = c as Window;

        // Remove back pointers from Windows to this Zone
        if (w != null)
          w.ParentZone = null;
      }

      // Should we kill ourself?
      if (this.AutoDispose)
      {
        // Remove notification when contents are added/removed/cleared
        m_windows.Clearing -= new CollectionClear(OnWindowsClearing);
        m_windows.Inserted -= new CollectionChange(OnWindowInserted);
        m_windows.Removing -= new CollectionChange(OnWindowRemoving);
        m_windows.Removed -= new CollectionChange(OnWindowRemoved);

        this.Dispose();
      }
    }

    protected virtual void OnWindowInserted(int index, object value)
    {
      Window w = value as Window;

      // Define back pointer from Window to its new Zone
      w.ParentZone = this;

      // Define the State the Window should adopt
      w.State = m_state;
    }

    protected virtual void OnWindowRemoving(int index, object value)
    {
      Window w = value as Window;

      // Remove back pointer from Window to this Zone
      w.ParentZone = null;
    }

    protected virtual void OnWindowRemoved(int index, object value)
    {
      // Removed the last entry?
      if (m_windows.Count == 0)
      {
        // Should we kill ourself?
        if (this.AutoDispose)
          this.Dispose();
      }
    }
  }
}


#region revision history

// $Log: Zone.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Zone.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: Zone.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

