// $Id: ContentCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
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
  public class ContentCollection : CollectionWithEvents
  {
    public Content Add(Content value)
    {
      // Use base class to process actual collection operation
      base.List.Add(value as object);

      return value;
    }

    public void AddRange(Content[] values)
    {
      // Use existing method to add each array entry
      foreach(Content page in values)
        Add(page);
    }

    public void Remove(Content value)
    {
      // Use base class to process actual collection operation
      base.List.Remove(value as object);
    }

    public void Insert(int index, Content value)
    {
      // Use base class to process actual collection operation
      base.List.Insert(index, value as object);
    }

    public bool Contains(Content value)
    {
      // Use base class to process actual collection operation
      return base.List.Contains(value as object);
    }

    public bool Contains(ContentCollection values)
    {
      foreach(Content c in values)
      {
        // Use base class to process actual collection operation
        if (Contains(c))
          return true;
      }

      return false;
    }

    public bool Contains(String value)
    {
      foreach(Content c in base.List)
        if (c.Title.Equals(value))
          return true;
					
      return false;			
    }

    public bool Contains(StringCollection values)
    {
      foreach(String s in values)
        if (Contains(s))
          return true;

      return false;
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

// $Log: ContentCollection.cs,v $
// Revision 1.1  2005/10/11 14:39:32  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Collections/ContentCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $
// $Id: ContentCollection.cs,v 1.1 2005/10/11 14:39:32 mkontak Exp $

#endregion

