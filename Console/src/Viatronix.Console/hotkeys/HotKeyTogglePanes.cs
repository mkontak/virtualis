// $Id: HotKeyTogglePanes.cs,v 1.2 2005/06/16 13:18:31 gdavidson Exp $
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
  /// Plugin "Hot Key" responsible toggling the fixed palette.
  /// </summary>
  public class HotKeyTogglePanes : Viatronix.UI.FrameHotKey
  {
    /// <summary>
    /// Toggles the "visibilty" of the fixed palette
    /// </summary>
    public override void Execute()
    {
      if( Global.Frame.ActiveViewTemplate.Panes.Count > 0 )
        Global.Frame.Panes.Visible = !Global.Frame.Panes.Visible;
    } // Execute()

  } // class HotKeyMaximize
} // namespace Viatronix.Console

#region revision history

// $Log: HotKeyTogglePanes.cs,v $
// Revision 1.2  2005/06/16 13:18:31  gdavidson
// Added check for panes before toggling the Rollout's visibility.
//
// Revision 1.1  2001/01/02 12:59:12  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/hotkeys/HotKeyTogglePanes.cs,v 1.2 2005/06/16 13:18:31 gdavidson Exp $
// $Id: HotKeyTogglePanes.cs,v 1.2 2005/06/16 13:18:31 gdavidson Exp $

#endregion