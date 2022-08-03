// $Id: WinGDIUtils.h,v 1.4 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: WinGDIUtils.h
// Description: 
// Created: 
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#ifndef WINDOWS_GDI_UTILS_H
#define WINDOWS_GDI_UTILS_H

#include <gdiplus.h>
#include "typedef.h"
#include "Point2D.h"

namespace WinUtils
{
  /// container/helper class
  class BrushAndPen
  {
  public:
    /// Create function, with color and pen width
    inline void Create(const COLORREF & color, const int & iPenwidth = 1);
    /// Destructor
    inline ~BrushAndPen();

    /// Create from another bitmap's characteristics
    inline void Create(CBitmap & bmp, const int & iPenwidth = 1);
    /// Select into device context
    inline void Select(CDC * pDC);

  public:
    /// The brush
    CBrush m_brush;
    /// The pen
    CPen m_pen;
  }; // BrushAndPen

  /// container/helper class
  class BitmapDC
  {
  public:
    /// Default constructor
    inline BitmapDC(CDC * pDC = NULL);
    /// Constructor with external bitmap
    inline BitmapDC(CBitmap& bmp, CDC * pDC = NULL);
    /// Destructor
    inline ~BitmapDC();

    /// Reset variables
    inline void Reset();
    /// Whether bitmap and DC has been created
    inline bool IsValid();

    /// Creates bitmap and DC compatible to DC
    inline void CreateBmpDC(CDC * pDC);
    /// Loads bitmap and stores info
    inline bool LoadBitmap(const uint4 uBitmapResID);

    /// Stores bitmap information
    inline void StoreInfo();
    /// Creates a bitmap stretched/shrunk to fill the specified window
    inline bool StretchFit(CWnd * pWnd, BitmapDC & src);
    /// Creates a bitmap stretched/shrunk to fit client rectangle
    inline bool StretchFit(CDC & winDC, const CRect & clientRect, BitmapDC & src);

  public:
    /// Reference to the bitmap used
    CBitmap &m_bmp;
    /// The device context
    CDC m_dc;
    /// Stores information on the bitmap
    BITMAP m_info;

  private:
    /// Private bitmap used if none specified in constructor
    CBitmap m_privateBmp;
    /// previous bitmap used
    CBitmap * m_previousBitmap;
  };

/// Resizes a bitmap to the given dimensions
inline void ResizeBmp(CDC * pDC, LPBYTE pBuffer, const Point2D<uint4>& origSize, const Point2D<uint4>& newSize, BitmapDC& resizeBDC);

#include "WinGDIUtils.inl"


} // namespace WinUtils

#endif // ifndef _WINDOWS_GDI_UTILS_H

// Revision History:
// $Log: WinGDIUtils.h,v $
// Revision 1.4  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.3  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.2.4.1  2006/05/09 13:56:17  geconomos
// restoring old bitmap in class BitmapDC
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.4  2003/01/03 22:40:41  jmeade
// ResizeBmp method to store/resize a bitmap buffer.
//
// Revision 3.3  2002/07/03 03:23:22  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.2.2.1  2002/07/01 19:53:45  jmeade
// Issue 2521:  Extended BitmapDC class usability by allowing use of an external
// (e.g. already initialized) bitmap.
//
// Revision 3.2  2001/12/22 00:36:10  jmeade
// StretchFit to indicate success.
//
// Revision 3.1  2001/11/20 02:57:34  jmeade
// Load bitmap convenience function.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:48   ingmar
// Initial revision.
// Revision 1.3  2001/05/11 23:13:50  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WinGDIUtils.h,v 1.4 2007/03/07 22:34:05 jmeade Exp $
// $Id: WinGDIUtils.h,v 1.4 2007/03/07 22:34:05 jmeade Exp $
