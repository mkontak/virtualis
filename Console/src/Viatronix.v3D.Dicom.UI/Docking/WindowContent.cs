// $Id: WindowContent.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
  public class WindowContent : Window
  {
    // Instance fields
    protected ContentCollection m_contents;
    protected VisualStyle m_style;

    public WindowContent(DockingManager manager, VisualStyle vs)
      : base(manager)
    {
      // Remember state
      m_style = vs;
        
      // Create collection of window details
      m_contents = new ContentCollection();

      // We want notification when contents are added/removed/cleared
      m_contents.Clearing += new CollectionClear(OnContentsClearing);
      m_contents.Inserted += new CollectionChange(OnContentInserted);
      m_contents.Removing += new CollectionChange(OnContentRemoving);
      m_contents.Removed += new CollectionChange(OnContentRemoved);
    }

    public ContentCollection Contents
    {
      get { return m_contents; }
			
      set
      {
        m_contents.Clear();
        m_contents = value;
      }
    }

    public virtual void BringContentToFront(Content c) {}

    protected virtual void OnContentsClearing()
    {
      foreach(Content c in m_contents)
      {
        // Inform content of new parent content window
        c.ParentWindowContent = null;

        // Unhook from property change notification
        c.PropertyChanged -= new Content.PropChangeHandler(OnContentChanged);
      }

      // Should we kill ourself?
      if (this.AutoDispose)
        Suicide();
    }

    protected virtual void OnContentInserted(int index, object value)
    {
      Content content = value as Content;

      // Is this the first Content added?
      if (m_contents.Count == 1)
      {
        // Use size of the Content to determine our size
        this.Size = content.DisplaySize;
      }

      // Inform content where it now resides
      content.ParentWindowContent = this;

      // Monitor changes in Content properties
      content.PropertyChanged += new Content.PropChangeHandler(OnContentChanged);
    }

    protected virtual void OnContentRemoving(int index, object value)
    {
      Content content = value as Content;

      // Inform content of new parent content window
      content.ParentWindowContent = null;

      // Unhook from monitoring changes in Content properties
      content.PropertyChanged -= new Content.PropChangeHandler(OnContentChanged);
    }

    protected virtual void OnContentRemoved(int index, object value)
    {
      // Removed the last entry?
      if (m_contents.Count == 0)
      {
        // Should we kill ourself?
        if (this.AutoDispose)
          Suicide();
      }
    }

    protected virtual void OnContentChanged(Content obj, Content.Property prop) {}

    protected void Suicide()
    {
      // Are we inside a Zone object?
      if (this.ParentZone != null)
        this.ParentZone.Windows.Remove(this);

      // Remover monitoring of events
      m_contents.Clearing -= new CollectionClear(OnContentsClearing);
      m_contents.Inserted -= new CollectionChange(OnContentInserted);
      m_contents.Removing -= new CollectionChange(OnContentRemoving);
      m_contents.Removed -= new CollectionChange(OnContentRemoved);

      this.Dispose();
    }
  }
}

#region revision history

// $Log: WindowContent.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/WindowContent.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: WindowContent.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

