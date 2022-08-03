// $Id: HotZoneNull.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Common;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class HotZoneNull : HotZone
  {
    public HotZoneNull(Rectangle hotArea)
      : base(hotArea, hotArea)
    {
    }

    public override void DrawIndicator(Point mousePos) {}
    public override void RemoveIndicator(Point mousePos) {}
  }
}

#region revision history

// $Log: HotZoneNull.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/HotZoneNull.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: HotZoneNull.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

