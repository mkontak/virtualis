// $Id: TargetCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class TargetCollection : CollectionWithEvents
  {
    public Target Add(Target value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(Target[] values)
    {
      // Use existing method to add each array entry
      foreach(Target page in values)
        Add(page);
    }

    public void Remove(Target value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, Target value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(Target value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public Target this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as Target); }
    }

    public int IndexOf(Target value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }

    public Target Contains(Point pt)
    {
      foreach(Target t in base.List)
      {
        if (t.HotRect.Contains(pt))
          return t;
      }

      return null;
    }
  }
}


#region revision history

// $Log: TargetCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/TargetCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: TargetCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

