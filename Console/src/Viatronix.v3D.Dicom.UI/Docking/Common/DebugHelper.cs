// $Id: DebugHelper.cs,v 1.2 2006/10/30 16:30:38 gdavidson Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Windows.Forms;

namespace Viatronix.v3D.Dicom.UI.Docking.Common
{
  public class DebugHelper
  {
    public static void ListControls(Control.ControlCollection controls)
    {
      ListControls("Control Collection", controls, false);
    }

    public static void ListControls(string title, Control.ControlCollection controls)
    {
      ListControls(title, controls, false);
    }

    public static void ListControls(string title, Control.ControlCollection controls, bool fullName)
    {
      // Output title first
      System.Console.WriteLine("\n{0}", title);

      // Find number of controls in the collection
      int count = controls.Count;

      // Output details for each 
      for(int index=0; index<count; index++)
      {
        Control c = controls[index];

        string typeName;
				
        if (fullName)
          typeName = c.GetType().FullName;
        else
          typeName = c.GetType().Name;

        System.Console.WriteLine("{0} V:{1} T:{2} N:{3}", index, c.Visible, typeName, c.Name);
      }
    }
  }
}



#region revision history

// $Log: DebugHelper.cs,v $
// Revision 1.2  2006/10/30 16:30:38  gdavidson
// Added the namespace when accessing the System.Console
//
// Revision 1.1  2005/10/11 14:39:47  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Common/DebugHelper.cs,v 1.2 2006/10/30 16:30:38 gdavidson Exp $
// $Id: DebugHelper.cs,v 1.2 2006/10/30 16:30:38 gdavidson Exp $

#endregion

