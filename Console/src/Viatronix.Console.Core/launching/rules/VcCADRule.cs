// $Id: VcCADRule.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using Viatronix.v3D.Core;

namespace Viatronix.Console
{
  /// <summary>
	/// Launch rule for VC CAD
	/// </summary>
	public class VcCADRule : VcRule
	{

    #region properties

    /// <summary>
    /// Determines if the associated viewer supports CAD.
    /// </summary>
    public override bool SupportsCAD
    {
      get { return true; }
    } // SupportsCAD

    #endregion

	} // class VcCADRule
} // namespace Viatronix.Console

#region revision history

// $Log: VcCADRule.cs,v $
// Revision 1.2  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:40:29  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.1  2005/12/14 20:02:18  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/VcCADRule.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
// $Id: VcCADRule.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $

#endregion
