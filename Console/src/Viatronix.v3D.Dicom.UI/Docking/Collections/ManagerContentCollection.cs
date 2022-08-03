// $Id: ManagerContentCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Xml;
using System.Windows.Forms;
using System.Runtime.Serialization;
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class ManagerContentCollection : CollectionWithEvents
  {
    // Instance fields
    protected DockingManager m_manager;

    public ManagerContentCollection(DockingManager manager)
    {
      // Must provide a valid manager instance
      if (manager == null)
        throw new ArgumentNullException("DockingManager");

      // Default the state
      m_manager = manager;
    }
		
    public Content Add()
    {
      Content c = new Content(m_manager);

      // Use base class to process actual collection operation
      base.List.Add(c as object);

      return c;
    }

    // Should only ever be used by Serialization
    public Content Add(Content c)
    {
      // Assign correct docking manager to object
      c.DockingManager = m_manager;

      // Use base class to process actual collection operation
      base.List.Add(c as object);

      return c;
    }

    public Content Add(Control control)
    {
      Content c = new Content(m_manager, control);

      // Use base class to process actual collection operation
      base.List.Add(c as object);

      return c;
    }

    public Content Add(Control control, string title)
    {
      Content c = new Content(m_manager, control, title);

      // Use base class to process actual collection operation
      base.List.Add(c as object);

      return c;
    }

    public Content Add(Control control, string title, ImageList imageList, int imageIndex)
    {
      Content c = new Content(m_manager, control, title, imageList, imageIndex);

      // Use base class to process actual collection operation
      base.List.Add(c as object);

      return c;
    }

    public void Remove(Content value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public bool Contains(Content value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public Content this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as Content); }
    }

    public Content this[string title]
    {
      get 
      {
        // Search for a Content with a matching title
        foreach(Content c in base.List)
          if (c.Title == title)
            return c;

        return null;
      }
    }

    public int SetIndex(int newIndex, Content value)
    {
      base.List.Remove(value);
      base.List.Insert(newIndex, value);

      return newIndex;
    }

    public int IndexOf(Content value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }

    public ContentCollection Copy()
    {
      ContentCollection clone = new ContentCollection();

      // Copy each reference across
      foreach(Content c in base.List)
        clone.Add(c);

      return clone;
    }
  }
}


#region revision history

// $Log: ManagerContentCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/ManagerContentCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: ManagerContentCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

