// $Id: ConsoleUtils.cs,v 1.5 2007/03/12 19:38:35 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Windows.Forms;
using Viatronix.v3D.Core;


namespace Viatronix.Console
{
	/// <summary>
	/// Utility functions for console operations.
	/// </summary>
  public sealed class ConsoleUtils
  {


    #region XmlBuilderKeyValidator

    /// <summary>
    /// Validates the query key
    /// </summary>
    /// <param name="args">KeyEventArgs</param>
    /// <returns>bool</returns>
    public static bool IsKeyValid( KeyEventArgs args )
    {
      if ( args.KeyValue == 8 ||
         ( args.KeyValue >= 33 && args.KeyValue <= 36 ) ||
         ( args.KeyValue >= 41 && args.KeyValue <= 45 ) ||
         ( args.KeyValue >= 48 && args.KeyValue <= 57 ) ||
         ( args.KeyValue >= 65 && args.KeyValue <= 90 ) ||
         ( args.KeyValue >= 96 && args.KeyValue <= 105 ) )    
        return true;
      else
        return false;
    }//IsKeyValid( KeyEventArgs args )

  #endregion

  } // class ConsoleUtils
} // namespace Viatronix.Console

#region revision history

// $Log: ConsoleUtils.cs,v $
// Revision 1.5  2007/03/12 19:38:35  mkontak
// coding standards
//
// Revision 1.4  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.3  2007/02/20 15:20:51  romy
// Added QueryKeyValidator for keypress.
//
// Revision 1.2  2006/11/03 22:21:34  gdavidson
// Issue #5092: Uncommented code
//
// Revision 1.1  2006/10/30 15:42:48  gdavidson
// Renamed from LoginUtilities
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/misc/ConsoleUtils.cs,v 1.5 2007/03/12 19:38:35 mkontak Exp $
// $Id: ConsoleUtils.cs,v 1.5 2007/03/12 19:38:35 mkontak Exp $

#endregion