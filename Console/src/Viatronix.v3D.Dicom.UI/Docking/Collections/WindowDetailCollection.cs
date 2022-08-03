// $Id: WindowDetailCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class WindowDetailCollection : CollectionWithEvents
  {
    public WindowDetail Add(WindowDetail value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(WindowDetail[] values)
    {
      // Use existing method to add each array entry
      foreach(WindowDetail page in values)
        Add(page);
    }

    public void Remove(WindowDetail value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, WindowDetail value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(WindowDetail value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public WindowDetail this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as WindowDetail); }
    }

    public int IndexOf(WindowDetail value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }
  }
}

#region revision history

// $Log: WindowDetailCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/WindowDetailCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: WindowDetailCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

