// $Id: GLPaintDevice.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)

/*
  Encapsulation of window handle and window context
*/

#ifndef GLPaintDevice_h
#define GLPaintDevice_h

// class definition
class VX_VIEWER_DLL GLPaintDevice
{

// member functions
public:

  /// returns the HWND for the paint device
  HWND GetHWND() const { return m_hWnd; }
  
  /// sets the HWND for the paint device
  void SetHWND( HWND hWnd ) { m_hWnd = hWnd; }
  
  /// returns the HDC for the paint device
  HDC GetHDC() const { return m_hDC; }

  /// sets the HDC for the paint device
  void SetHDC( HDC hDC ) { m_hDC = hDC; }

// member variables
private:
  
  /// Win32 specific window handle
  HWND m_hWnd;
  
  /// Win32 specific device context
  HDC m_hDC;
}; // GLPaintDevice


#endif // GLPaintDevice_h


// $Log: GLPaintDevice.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 3.3  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.2  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 3.1  2001/12/18 17:16:46  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:49:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:36   ingmar
// Initial revision.
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.1  2001/06/22 18:22:07  geconomos
// Initial OpenGL implementation.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/GLPaintDevice.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: GLPaintDevice.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $

