// $Id: HotKeySearch.cs,v 1.4 2006/07/10 18:39:36 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using Viatronix.UI;

namespace Viatronix.Console
{
  /// <summary>
  /// Plugin "Hot Key" responsible for performing a search.
  /// </summary>
  public class HotKeySearch : Viatronix.UI.FrameHotKey
  {
    /// <summary>
    /// Performs a search .
    /// </summary>
    public override void Execute()
    {
      if( Global.Frame.ActiveView is ISearchable )
      {
        ISearchable view = (ISearchable) ( Global.Frame.ActiveView );
        view.Searcher.Reset();
        view.Searcher.Search();
      }
    } // Execute()

  } // class HotKeySearch
} // namespace Viatronix.Console

#region revision history

// $Log: HotKeySearch.cs,v $
// Revision 1.4  2006/07/10 18:39:36  gdavidson
// Modified SearchManager
//
// Revision 1.3  2006/07/06 18:16:27  gdavidson
// Issue #4766: Disable search when on is active
//
// Revision 1.2  2006/06/30 13:32:52  gdavidson
// Issue #4766: Made search synchronous
//
// Revision 1.1  2006/05/08 18:18:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/hotkeys/HotKeySearch.cs,v 1.4 2006/07/10 18:39:36 gdavidson Exp $
// $Id: HotKeySearch.cs,v 1.4 2006/07/10 18:39:36 gdavidson Exp $

#endregion