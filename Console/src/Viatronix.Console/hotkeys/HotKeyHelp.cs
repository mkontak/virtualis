// $Id: HotKeyHelp.cs,v 1.1.2.3 2007/04/19 14:21:06 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using Viatronix.UI;

namespace Viatronix.Console
{
  /// <summary>
  /// Plugin "Hot Key" responsible for performing a search.
  /// </summary>
  public class HotKeyHelp : Viatronix.UI.FrameHotKey
  {
    /// <summary>
    /// bring up the help file  .
    /// </summary>
    public override void Execute()
    {
      MenuHelp.ShowHelp();
    } // Execute()

  } // class HotKeySearch
} // namespace Viatronix.Console

#region revision history

// $Log: HotKeyHelp.cs,v $
// Revision 1.1.2.3  2007/04/19 14:21:06  romy
// code hot keys for Help file
//
// Revision 1.1.2.1  2007/04/19 13:51:31  romy
// hotkey for Help added
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/hotkeys/HotKeyHelp.cs,v 1.1.2.3 2007/04/19 14:21:06 romy Exp $
// $Id: HotKeyHelp.cs,v 1.1.2.3 2007/04/19 14:21:06 romy Exp $

#endregion