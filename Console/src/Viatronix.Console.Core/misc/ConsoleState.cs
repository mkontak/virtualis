// $Id: ConsoleState.cs,v 1.4 2007/03/12 19:38:35 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Text;

namespace Viatronix.Console
{
  /// <summary>
  /// Constants for Viatronix.Console objects.
  /// </summary>
  public sealed class ConsoleState
  {

    #region constants

    /// <summary>
    /// UMAS
    /// </summary>
    public const string UMAS = "UMAS";

    /// <summary>
    /// LockManager
    /// </summary>
    public const string LockManager = "LockManager";

    /// <summary>
    /// LaunchManager
    /// </summary>
    public const string LaunchManager = "LaunchManager";

    /// <summary>
    /// AETitle
    /// </summary>
    public const string AETitle = "AETitle";

    #endregion

    #region construction

    /// <summary>
    /// private constructor
    /// </summary>
    private ConsoleState()
    {
    } // ConsoleState()

    #endregion

  } // class ConsoleState
} // namespace Viatronix.Console

#region revision history

// $Log: ConsoleState.cs,v $
// Revision 1.4  2007/03/12 19:38:35  mkontak
// coding standards
//
// Revision 1.3  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.2  2007/02/21 21:22:50  gdavidson
// Commented code
//
// Revision 1.1  2006/10/30 15:42:31  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/misc/ConsoleState.cs,v 1.4 2007/03/12 19:38:35 mkontak Exp $
// $Id: ConsoleState.cs,v 1.4 2007/03/12 19:38:35 mkontak Exp $

#endregion
