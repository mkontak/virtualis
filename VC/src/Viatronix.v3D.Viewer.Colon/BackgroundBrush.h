// $Id: BackgroundBrush.h,v 1.3 2005/08/23 16:31:45 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


// pragmas
#pragma once


// includes
#include "WinGLUtils.h"

namespace WinUtils
{

  // class declaration
  class BackgroundBrush
  {
  // member functions
  public:

    /// constructor
    BackgroundBrush();

    /// destructor
    virtual ~BackgroundBrush();

    /// create the background brush for the specified CWnd and resource id
    bool Create( CWnd * pWnd, int4 uResourceId );

    /// resets the brush to a default state
    void Reset();

    /// conversion operator to HBRUSH
    operator HBRUSH() const { return m_hBrush; }

    /// returns the associated HBITMAP
    HBITMAP GetHBITMAP() const { return m_hBitmap; }

  private:

    /// resource image map typedef
    typedef std::map< uint4, Gdiplus::Bitmap * > ResourceImageCache;

    /// returns the resource image cache
    static ResourceImageCache & GetResourceImageCache();

  // member variables
  private:

    // background image
    HBITMAP m_hBitmap;

    // pattern brush
    HBRUSH m_hBrush;

  }; // class BackgroundBrush

} // namespace WinUtils

// $Log: BackgroundBrush.h,v $
// Revision 1.3  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.2  2005/08/16 22:56:12  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2005/06/22 19:46:57  geconomos
// reimplemented using HBITMAP and HBRUSH (no MFC)
//
// Revision 1.1.2.1  2005/06/17 13:02:09  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/BackgroundBrush.h,v 1.3 2005/08/23 16:31:45 geconomos Exp $
// $Id: BackgroundBrush.h,v 1.3 2005/08/23 16:31:45 geconomos Exp $
