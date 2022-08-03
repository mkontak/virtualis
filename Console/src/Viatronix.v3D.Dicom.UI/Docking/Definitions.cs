// $Id: Definitions.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Drawing;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  /// <summary>
  /// State Enumerator
  /// </summary>
  public enum State
  {
    Floating,
    DockTop,
    DockBottom,
    DockLeft,
    DockRight
  }

  public interface IHotZoneSource
  {
    void AddHotZones(Redocker redock, HotZoneCollection collection);
  }

  public interface IZoneMaximizeWindow
  {
    Direction Direction { get; }
    bool IsMaximizeAvailable();
    bool IsWindowMaximized(Window w);
    void MaximizeWindow(Window w);
    void RestoreWindow();
    event EventHandler RefreshMaximize;
  }

  // Delegate signatures
  public delegate void ContextHandler(Point screenPos);

}

#region revision history

// $Log: Definitions.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Definitions.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: Definitions.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

