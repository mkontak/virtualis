// $Id: TabPageCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]
// $Id: TabPageCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using Viatronix.v3D.Dicom.UI.Docking.Collections;
using Viatronix.v3D.Dicom.UI.Docking.Controls;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class TabPageCollection : CollectionWithEvents
  {
    public TabPage Add(TabPage value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(TabPage[] values)
    {
      // Use existing method to add each array entry
      foreach(TabPage page in values)
        Add(page);
    }

    public void Remove(TabPage value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, TabPage value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(TabPage value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public TabPage this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as TabPage); }
    }

    public TabPage this[string title]
    {
      get 
      {
        // Search for a Page with a matching title
        foreach(TabPage page in base.List)
          if (page.Title == title)
            return page;

        return null;
      }
    }

    public int IndexOf(TabPage value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }
  }
}


#region revision history

// $Log: TabPageCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/TabPageCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: TabPageCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

