// $Id: Program.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// Contains the main entry point for the program.
  /// </summary>
  static class Program
  {

    #region "main"

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      Viatronix.Logging.Log.Initialize();

      Application.EnableVisualStyles();

      Application.Run(new CadForm());
    } // Main()

    #endregion

  } // Program
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/Program.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: Program.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
