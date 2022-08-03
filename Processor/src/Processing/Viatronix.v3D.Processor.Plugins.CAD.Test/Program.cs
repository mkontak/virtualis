// $Id: Program.cs,v 1.1 2006/12/21 11:27:23 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Viatronix.v3D.Processor.Plugins.CAD.Test
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault( false );
      Application.Run( new CADConsoleForm() );
    }
  }
}


#region
// $Log: Program.cs,v $
// Revision 1.1  2006/12/21 11:27:23  romy
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD.Test/Program.cs,v 1.1 2006/12/21 11:27:23 romy Exp $
// $Id: Program.cs,v 1.1 2006/12/21 11:27:23 romy Exp $
#endregion

