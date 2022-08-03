// $Id: MenuControlDesigner.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
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
using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using Viatronix.v3D.Dicom.UI.Docking.Controls;

namespace Viatronix.v3D.Dicom.UI.Docking.Menus
{
  public class MenuControlDesigner :  System.Windows.Forms.Design.ParentControlDesigner
  {
    public override ICollection AssociatedComponents
    {
      get 
      {
        if (base.Control is Viatronix.v3D.Dicom.UI.Docking.Menus.MenuControl)
          return ((Viatronix.v3D.Dicom.UI.Docking.Menus.MenuControl)base.Control).MenuCommands;
        else
          return base.AssociatedComponents;
      }
    }

    protected override bool DrawGrid
    {
      get { return false; }
    }
  }
}


#region revision history

// $Log: MenuControlDesigner.cs,v $
// Revision 1.1  2005/10/11 14:40:23  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Menus/MenuControlDesigner.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $
// $Id: MenuControlDesigner.cs,v 1.1 2005/10/11 14:40:23 mkontak Exp $

#endregion

