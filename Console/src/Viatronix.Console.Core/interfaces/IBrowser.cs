// $Id: IBrowser.cs,v 1.3 2005/10/18 17:34:23 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
  /// <summary>
  /// Defines methods that a browser type view will be required to implement
  /// </summary>
  public interface IBrowser
  {

    #region properties

    /// <summary>
    /// Data Provider (who is providing data to the browser)
    /// </summary>
    IBrowserProvider BrowserProvider
    { 
      get;
    } // BrowserProvider

    #endregion

  } // interface IBrowser
} // namespace Viatronix.Console


#region revision history

// $Log: IBrowser.cs,v $
// Revision 1.3  2005/10/18 17:34:23  gdavidson
// Removed the RefreshBrower method.
//
// Revision 1.2  2005/10/11 14:12:32  gdavidson
// Added a RefreshBrowser method.
//
// Revision 1.1  2005/10/05 12:30:52  mkontak
// Added to support CreateSeries() in the IConsole interface
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/ui/IBrowser.cs,v 1.3 2005/10/18 17:34:23 gdavidson Exp $
// $Id: IBrowser.cs,v 1.3 2005/10/18 17:34:23 gdavidson Exp $

#endregion

