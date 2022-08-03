// $Id: vxDib.h,v 1.2 2007/10/24 20:10:50 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

#ifndef vxDib_h
#define vxDib_h

// class declaration
class VX_VIEWER_DLL vxDib
{

// member functions
public:

  /// default constructor
  vxDib();

  /// virtual destructor
  virtual ~vxDib();
  
  /// creates a dib with the specified width and height
  void Create( uint4 uWidth, uint4 uHeight );

  /// gets the width of the dib
  inline uint4 GetWidth() const { return m_uWidth; }

  /// gets the height of the dib
  inline uint4 GetHeight() const { return m_uHeight; }

  /// gets the associated memory device context
	HDC GetHDC() const { return m_hdc; }

  /// gets the handle to the underlying HBITMAP
	HBITMAP GetHandle() const { return m_hBitmap; }
	
	/// gets a pointer the the pixel data
	uint1* GetPixelData() const { return m_puBits; }

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
	uint1* m_puBits;
	
	/// underlying memory device context
	HDC m_hdc;

  /// width of the dib
  uint4 m_uWidth;

  /// height of the dib
  uint4 m_uHeight;

}; // class vxDib

#endif // #ifndef vxDib_h

// $Log: vxDib.h,v $
// Revision 1.2  2007/10/24 20:10:50  gdavidson
// Added witdth and height accessor methods
//
// Revision 1.1  2004/03/30 17:23:03  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDib.h,v 1.2 2007/10/24 20:10:50 gdavidson Exp $
// $Id: vxDib.h,v 1.2 2007/10/24 20:10:50 gdavidson Exp $
