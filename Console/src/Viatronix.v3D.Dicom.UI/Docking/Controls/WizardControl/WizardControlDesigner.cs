// $Id: WizardControlDesigner.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public class WizardControlDesigner :  System.Windows.Forms.Design.ParentControlDesigner
  {
    private ISelectionService m_selectionService = null;

    public override ICollection AssociatedComponents
    {
      get 
      {
        if (base.Control is Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl)
          return ((Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl)base.Control).WizardPages;
        else
          return base.AssociatedComponents;
      }
    }

    protected override bool DrawGrid
    {
      get { return false; }
    }

    public ISelectionService SelectionService
    {
      get
      {
        // Is this the first time the accessor has been called?
        if (m_selectionService == null)
        {
          // Then grab and cache the required interface
          m_selectionService = (ISelectionService)GetService(typeof(ISelectionService));
        }

        return m_selectionService;
      }
    }

    protected override void WndProc(ref Message msg)
    {
      // Test for the left mouse down windows message
      if (msg.Msg == (int)Win32.Msgs.WM_LBUTTONDOWN)
      {
        Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl wizardControl = this.SelectionService.PrimarySelection as Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl;

        // Check we have a valid object reference
        if (wizardControl != null)
        {
          Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl tabControl = wizardControl.TabControl;

          // Check we have a valid object reference
          if (tabControl != null)
          {
            // Extract the mouse position
            int xPos = (short)((uint)msg.LParam & 0x0000FFFFU);
            int yPos = (short)(((uint)msg.LParam & 0xFFFF0000U) >> 16);

            Point screenCoord = wizardControl.PointToScreen(new Point(xPos, yPos));
            Point clientCoord = tabControl.PointToClient(screenCoord);

            // Ask the TabControl to change tabs according to mouse message
            tabControl.ExternalMouseTest(msg.HWnd, clientCoord);
          }
        }
      }
      else
      {
        if (msg.Msg == (int)Win32.Msgs.WM_LBUTTONDBLCLK)
        {
          Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl wizardControl = this.SelectionService.PrimarySelection as Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl;
                    
          // Check we have a valid object reference
          if (wizardControl != null)
          {
            Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl tabControl = wizardControl.TabControl;

            // Check we have a valid object reference
            if (tabControl != null)
            {
              // Extract the mouse position
              int xPos = (short)((uint)msg.LParam & 0x0000FFFFU);
              int yPos = (short)(((uint)msg.LParam & 0xFFFF0000U) >> 16);

              Point screenCoord = wizardControl.PointToScreen(new Point(xPos, yPos));
              Point clientCoord = tabControl.PointToClient(screenCoord);

              // Ask the TabControl to process a double click over an arrow as a simple
              // click of the arrow button. In which case we return immediately to prevent
              // the base class from using the double to generate the default event
              if (tabControl.WantDoubleClick(msg.HWnd, clientCoord))
                return;
            }
          }
        }
      }

      base.WndProc(ref msg);
    }
  }
}

#region revision history

// $Log: WizardControlDesigner.cs,v $
// Revision 1.1  2005/10/11 14:42:12  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/WizardControl/WizardControlDesigner.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $
// $Id: WizardControlDesigner.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $

#endregion

