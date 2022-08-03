// $Id: HotZoneCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  public class HotZoneCollection : CollectionWithEvents
  {
    public HotZone Add(HotZone value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(HotZone[] values)
    {
      // Use existing method to add each array entry
      foreach(HotZone page in values)
        Add(page);
    }

    public void Remove(HotZone value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, HotZone value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(HotZone value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public HotZone this[int index]
    {
      // Use base class to process actual collection operation
      get { return (base.List[index] as HotZone); }
    }

    public int IndexOf(HotZone value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }

    public HotZone Contains(Point pt)
    {
      foreach(HotZone hz in base.List)
      {
        if (hz.HotArea.Contains(pt))
          return hz;
      }

      return null;
    }
  }
}


#region revision history

// $Log: HotZoneCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/HotZoneCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: HotZoneCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

