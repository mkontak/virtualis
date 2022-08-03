// $Id: ZoneHelper.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class ZoneHelper
  {
    public static ContentCollection Contents(Zone z)
    {
      // Container for returned group of found Content objects
      ContentCollection cc = new ContentCollection();

      // Process each Window in the Zone
      foreach(Window w in z.Windows)
      {
        WindowContent wc = w as WindowContent;

        // Is the Zone a Content derived variation?
        if (wc != null)
        {
          // Add each Content into the collection
          foreach(Content c in wc.Contents)
            cc.Add(c);
        }
      }

      return cc;
    }

    public static StringCollection ContentNames(Zone z)
    {
      // Container for returned group of found String objects
      StringCollection sc = new StringCollection();

      // Process each Window in the Zone
      foreach(Window w in z.Windows)
      {
        WindowContent wc = w as WindowContent;

        // Is the Zone a Content derived variation?
        if (wc != null)
        {
          // Add each Content into the collection
          foreach(Content c in wc.Contents)
            sc.Add(c.Title);
        }
      }

      return sc;
    }

    public static StringCollection ContentNamesInPriority(Zone z, Content c)
    {
      // Container for returned group of found Content objects
      StringCollection sc = new StringCollection();

      // Process each Window in the Zone
      foreach(Window w in z.Windows)
      {
        WindowContent wc = w as WindowContent;

        // Is the Zone a Content derived variation?
        if (wc != null)
        {
          // Does this contain the interesting Content?
          if (wc.Contents.Contains(c))
          {
            // All Content of this Window are given priority and 
            // added into the start of the collection
            foreach(Content content in wc.Contents)
              sc.Insert(0, content.Title);
          }
          else
          {
            // Lower priority Window and so contents are always
            // added to the end of the collection
            foreach(Content content in wc.Contents)
              sc.Add(content.Title);
          }
        }
      }

      return sc;
    }
  }
}

#region revision history

// $Log: ZoneHelper.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/ZoneHelper.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: ZoneHelper.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

