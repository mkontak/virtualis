// $Id: BorderForControl.cs,v 1.1 2005/10/11 14:41:04 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Data;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [ToolboxItem(false)]
  public class BorderForControl : UserControl 
  {
    // Instance fields
    protected int m_borderWidth = 2;

    public BorderForControl()
    {
      InternalConstruct(null, m_borderWidth);
    }

    public BorderForControl(Control control)
    {
      InternalConstruct(control, m_borderWidth);
    }

    public BorderForControl(Control control, int borderWidth)
    {
      InternalConstruct(control, borderWidth);
    }

    protected void InternalConstruct(Control control, int borderWidth)
    {
      // Remember parameter
      m_borderWidth = borderWidth;
			
      if (control != null)
      {
        // Remove any existing docking style for passed in Control
        control.Dock = DockStyle.None;

        // Add to appearance
        Controls.Add(control);
      }
    }	

    public int BorderWidth
    {
      get { return m_borderWidth; }
      set { m_borderWidth = value; }
    }

    protected override void OnResize(EventArgs e)
    {
      ResizeOnlyTheChild();
      base.OnResize(e);
    }
		
    protected override void OnLayout(LayoutEventArgs e)	
    {
      ResizeOnlyTheChild();
      base.OnLayout(e);
    }

    protected void ResizeOnlyTheChild()
    {
      // Do we have a child control to resize? 
      if (Controls.Count >= 1)
      {
        Size ourSize = this.Size;

        // Get the first child (there should not be any others)
        Control child = this.Controls[0];

        // Define new position
        child.Location = new Point(m_borderWidth, m_borderWidth);

        // Define new size
        child.Size = new Size(ourSize.Width - m_borderWidth * 2, ourSize.Height - m_borderWidth * 2);
      }
    }	
  }
}

#region revision history

// $Log: BorderForControl.cs,v $
// Revision 1.1  2005/10/11 14:41:04  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/BorderForControl/BorderForControl.cs,v 1.1 2005/10/11 14:41:04 mkontak Exp $
// $Id: BorderForControl.cs,v 1.1 2005/10/11 14:41:04 mkontak Exp $

#endregion


