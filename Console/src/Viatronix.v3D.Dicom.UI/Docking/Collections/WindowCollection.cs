// $Id: WindowCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
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
  public class WindowCollection : CollectionWithEvents
  {
    public Window Add(Window value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(Window[] values)
    {
      // Use existing method to add each array entry
      foreach(Window page in values)
        Add(page);
    }

    public void Remove(Window value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, Window value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(Window value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public Window this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as Window); }
    }

    public int IndexOf(Window value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }
  }
}

#region revision history

// $Log: WindowCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/WindowCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: WindowCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

