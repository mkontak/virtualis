// $Id: Structs.cs,v 1.1 2005/10/11 14:40:00 mkontak Exp $
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
using System.Runtime.InteropServices;

namespace Viatronix.v3D.Dicom.UI.Docking.Win32
{
  [StructLayout(LayoutKind.Sequential)]
  public struct MSG 
  {
    public IntPtr hwnd;
    public int message;
    public IntPtr wParam;
    public IntPtr lParam;
    public int time;
    public int pt_x;
    public int pt_y;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct PAINTSTRUCT
  {
    public IntPtr hdc;
    public int fErase;
    public Rectangle rcPaint;
    public int fRestore;
    public int fIncUpdate;
    public int Reserved1;
    public int Reserved2;
    public int Reserved3;
    public int Reserved4;
    public int Reserved5;
    public int Reserved6;
    public int Reserved7;
    public int Reserved8;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct RECT
  {
    public int left;
    public int top;
    public int right;
    public int bottom;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct POINT
  {
    public int x;
    public int y;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct SIZE
  {
    public int cx;
    public int cy;
  }

  [StructLayout(LayoutKind.Sequential, Pack=1)]
  public struct BLENDFUNCTION
  {
    public byte BlendOp;
    public byte BlendFlags;
    public byte SourceConstantAlpha;
    public byte AlphaFormat;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct TRACKMOUSEEVENTS
  {
    public uint cbSize;
    public uint dwFlags;
    public IntPtr hWnd;
    public uint dwHoverTime;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct LOGBRUSH
  {
    public uint lbStyle; 
    public uint lbColor; 
    public uint lbHatch; 
  }
}


#region revision history

// $Log: Structs.cs,v $
// Revision 1.1  2005/10/11 14:40:00  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Win32/Structs.cs,v 1.1 2005/10/11 14:40:00 mkontak Exp $
// $Id: Structs.cs,v 1.1 2005/10/11 14:40:00 mkontak Exp $

#endregion

