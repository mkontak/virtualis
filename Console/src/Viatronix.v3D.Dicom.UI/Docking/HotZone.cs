// $Id: HotZone.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
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
  public class HotZone
  {
    // Class constants
    protected static int m_dragWidth = 4;
		
    // Instance fields
    protected Rectangle m_hotArea;
    protected Rectangle m_newSize;

    public HotZone(Rectangle hotArea, Rectangle newSize)
    {
      // Store initial state
      m_hotArea = hotArea;
      m_newSize = newSize;
    }

    public Rectangle HotArea
    {
      get { return m_hotArea; }
    }

    public Rectangle NewSize
    {
      get { return m_newSize; }
    }

    public virtual bool ApplyChange(Point screenPos, Redocker parent) { return false; }
    public virtual void UpdateForMousePosition(Point screenPos, Redocker parent) {}

    public virtual void DrawIndicator(Point mousePos) 
    {
      DrawReversible(m_newSize);
    }
		
    public virtual void RemoveIndicator(Point mousePos) 
    {
      DrawReversible(m_newSize);
    }

    public virtual void DrawReversible(Rectangle rect)
    {
      DrawHelper.DrawDragRectangle(rect, m_dragWidth);
    }
  }
}

#region revision history

// $Log: HotZone.cs,v $
// Revision 1.1  2005/10/11 14:42:35  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/HotZone.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $
// $Id: HotZone.cs,v 1.1 2005/10/11 14:42:35 mkontak Exp $

#endregion

