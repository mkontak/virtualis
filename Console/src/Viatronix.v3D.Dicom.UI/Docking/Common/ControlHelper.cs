// $Id: ControlHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Windows.Forms;

namespace Viatronix.v3D.Dicom.UI.Docking.Common
{
  public class ControlHelper
  {
    public static void RemoveAll(Control control)
    {
      if ((control != null) && (control.Controls.Count > 0))
      {
        Button tempButton = null;
        Form parentForm = control.FindForm();
                
        if (parentForm != null)
        {
          // Create a hidden, temporary button
          tempButton = new Button();
          tempButton.Visible = false;

          // Add this temporary button to the parent form
          parentForm.Controls.Add(tempButton);

          // Must ensure that temp button is the active one
          parentForm.ActiveControl = tempButton;
        }

        // Remove all entries from target
        control.Controls.Clear();

        if (parentForm != null)
        {
          // Remove the temporary button
          tempButton.Dispose();
          parentForm.Controls.Remove(tempButton);
        }
      }
    }

    public static void Remove(Control.ControlCollection coll, Control item)
    {
      if ((coll != null) && (item != null))
      {
        Button tempButton = null;
        Form parentForm = item.FindForm();

        if (parentForm != null)
        {
          // Create a hidden, temporary button
          tempButton = new Button();
          tempButton.Visible = false;

          // Add this temporary button to the parent form
          parentForm.Controls.Add(tempButton);

          // Must ensure that temp button is the active one
          parentForm.ActiveControl = tempButton;
        }
                
        // Remove our target control
        coll.Remove(item);

        if (parentForm != null)
        {
          // Remove the temporary button
          tempButton.Dispose();
          parentForm.Controls.Remove(tempButton);
        }
      }
    }

    public static void RemoveAt(Control.ControlCollection coll, int index)
    {
      if (coll != null)
      {
        if ((index >= 0) && (index < coll.Count))
        {
          Remove(coll, coll[index]);
        }
      }
    }
    
    public static void RemoveForm(Control source, Form form)
    {
      ContainerControl container = source.FindForm() as ContainerControl;
            
      if (container == null)
        container = source as ContainerControl;

      Button tempButton = new Button();
      tempButton.Visible = false;

      // Add this temporary button to the parent form
      container.Controls.Add(tempButton);

      // Must ensure that temp button is the active one
      container.ActiveControl = tempButton;

      // Remove Form parent
      form.Parent = null;
            
      // Remove the temporary button
      tempButton.Dispose();
      container.Controls.Remove(tempButton);
    }
  }
}

#region revision history

// $Log: ControlHelper.cs,v $
// Revision 1.1  2005/10/11 14:39:47  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Common/ControlHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $
// $Id: ControlHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $

#endregion

