// $Id: Redocker.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
using Viatronix.v3D.Dicom.UI.Docking;

namespace Viatronix.v3D.Dicom.UI.Docking
{
  public class Redocker
  {
    // Instance fields
    protected bool m_tracking;

    public Redocker()
    {
      // Default the state
      m_tracking = false;
    }

    public bool Tracking
    {
      get { return m_tracking; }
    }

    public virtual void EnterTrackingMode()
    {
      if (!m_tracking)
        m_tracking = true;
    }

    public virtual bool ExitTrackingMode(MouseEventArgs e)
    {
      if (m_tracking)
        m_tracking = false;

      return false;
    }

    public virtual void QuitTrackingMode(MouseEventArgs e)
    {
      if (m_tracking)
        m_tracking = false;
    }

    public virtual void OnMouseMove(MouseEventArgs e) {}

    public virtual bool OnMouseUp(MouseEventArgs e)
    {
      if (m_tracking)
      {
        if (e.Button == MouseButtons.Left)
          return ExitTrackingMode(e);
        else
          QuitTrackingMode(e);
      }

      return false;
    }
  }
}

#region revision history

// $Log: Redocker.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Redocker.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: Redocker.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

