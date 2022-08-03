// $Id: Gdi32.cs,v 1.1 2005/10/11 14:40:00 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Runtime.InteropServices;

namespace Viatronix.v3D.Dicom.UI.Docking.Win32
{
  public class Gdi32
  {
    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern int CombineRgn(IntPtr dest, IntPtr src1, IntPtr src2, int flags);

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr CreateRectRgnIndirect(ref Win32.RECT rect); 

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern int GetClipBox(IntPtr hDC, ref Win32.RECT rectBox); 

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern int SelectClipRgn(IntPtr hDC, IntPtr hRgn); 

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr CreateBrushIndirect(ref LOGBRUSH brush); 

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern bool PatBlt(IntPtr hDC, int x, int y, int width, int height, uint flags); 

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr DeleteObject(IntPtr hObject);

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern bool DeleteDC(IntPtr hDC);

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr CreateCompatibleDC(IntPtr hDC);
  }
}

#region revision history

// $Log: Gdi32.cs,v $
// Revision 1.1  2005/10/11 14:40:00  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Win32/Gdi32.cs,v 1.1 2005/10/11 14:40:00 mkontak Exp $
// $Id: Gdi32.cs,v 1.1 2005/10/11 14:40:00 mkontak Exp $

#endregion

