// $Id: CollectionWithEvents.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Collections
{
  // Declare the event signatures
  public delegate void CollectionClear();
  public delegate void CollectionChange(int index, object value);

  public class CollectionWithEvents : CollectionBase
  {
    // Collection change events
    public event CollectionClear Clearing;
    public event CollectionClear Cleared;
    public event CollectionChange Inserting;
    public event CollectionChange Inserted;
    public event CollectionChange Removing;
    public event CollectionChange Removed;
	
    // Overrides for generating events
    protected override void OnClear()
    {
      // Any attached event handlers?
      if (Clearing != null)
        Clearing();
    }	

    protected override void OnClearComplete()
    {
      // Any attached event handlers?
      if (Cleared != null)
        Cleared();
    }	

    protected override void OnInsert(int index, object value)
    {
      // Any attached event handlers?
      if (Inserting != null)
        Inserting(index, value);
    }

    protected override void OnInsertComplete(int index, object value)
    {
      // Any attached event handlers?
      if (Inserted != null)
        Inserted(index, value);
    }

    protected override void OnRemove(int index, object value)
    {
      // Any attached event handlers?
      if (Removing != null)
        Removing(index, value);
    }

    protected override void OnRemoveComplete(int index, object value)
    {
      // Any attached event handlers?
      if (Removed != null)
        Removed(index, value);
    }

    protected int IndexOf(object value)
    {
      // Find the 0 based index of the requested entry
      return base.List.IndexOf(value);
    }
  }
}

#region revision history

// $Log: CollectionWithEvents.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/CollectionWithEvents.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: CollectionWithEvents.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion


