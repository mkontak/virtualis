// $Id: TabGroupBaseCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class TabGroupBaseCollection : CollectionWithEvents
  {
    public TabGroupBase Add(TabGroupBase value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(TabGroupBase[] values)
    {
      // Use existing method to add each array entry
      foreach(TabGroupBase item in values)
        Add(item);
    }

    public void Remove(TabGroupBase value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, TabGroupBase value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(TabGroupBase value)
    {
      // Value comparison
      foreach(String s in base.List)
        if (value.Equals(s))
          return true;

      return false;
    }

    public bool Contains(TabGroupBaseCollection values)
    {
      foreach(TabGroupBase c in values)
      {
        // Use base class to process actual collection operation
        if (Contains(c))
          return true;
      }

      return false;
    }

    public TabGroupBase this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as TabGroupBase); }
      set { base.List[index] = value; }
    }

    public int IndexOf(TabGroupBase value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }
  }
}


#region revision history

// $Log: TabGroupBaseCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/TabGroupBaseCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: TabGroupBaseCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

