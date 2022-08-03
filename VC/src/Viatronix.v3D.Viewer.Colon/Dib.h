// $Id: Dib.h,v 1.3 2007/03/01 21:40:08 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (mailto:george@viatronix.com)

// pragmas
#pragma once

namespace WinUtils
{

  // class declaration
  class Dib
  {

  // member functions
  public:

    /// default constructor
    Dib();

    /// virtual destructor
    virtual ~Dib();
    
    /// creates a dib with the specified width and height
    void Create( uint4 uWidth, uint4 uHeight );

    /// gets the associated memory device context
	  HDC GetHDC() const { return m_hdc; }

    /// gets the handle to the underlying HBITMAP
	  HBITMAP GetHandle() const { return m_hBitmap; }
  	
	  /// gets a pointer the the pixel data
	  uint1* GetPixelData() const { return m_puBits; }

    /// gets the bitmap width
    int4 GetWidth() const { return m_iWidth; }

    /// gets the bitmap height
    int4 GetHeight() const { return m_iHeight; }

  private:

    /// frees all resources associated with the dib
	  void Destroy();

  // member variables  
  private:

    /// underlying HBITMAP
	  HBITMAP m_hBitmap;
  	
	  /// previous bitmap associated with device context
	  HBITMAP m_hBitmapOld;
  	
	  /// pointer to the pixel data
	  uint1 * m_puBits;
  	
	  /// underlying memory device context
	  HDC m_hdc;

    /// dib width
    int4 m_iWidth;

    /// dib height
    int4 m_iHeight;

  }; // class Dib

} // namespace WinUtils

// $Log: Dib.h,v $
// Revision 1.3  2007/03/01 21:40:08  geconomos
// code review preparation
//
// Revision 1.2  2005/08/16 22:02:28  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2005/06/23 18:50:30  geconomos
// Issue #4189: Added width and height properties
//
// Revision 1.1.2.1  2005/06/21 15:12:53  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Dib.h,v 1.3 2007/03/01 21:40:08 geconomos Exp $
// $Id: Dib.h,v 1.3 2007/03/01 21:40:08 geconomos Exp $
