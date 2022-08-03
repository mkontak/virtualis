// $Id: vxMouseCursor.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)

/**
 * Define the mouse cursor
 */
#ifndef vxMouseCursor_h
#define vxMouseCursor_h


// includes



// class definition
class VX_VIEWER_DLL vxMouseCursor 
{
// functions
public:
  
  /// default constructor
  vxMouseCursor();

  /// constructor that set the mouse to the specified cursor and return to the default when the class goes out of scope
  vxMouseCursor(const std::string sCursor);

  /// destructor
  ~vxMouseCursor();
	
  /// sets the module that contains the resources
  static void SetModuleHandle(HMODULE module) { m_module = module; }

  // sets the current mouse cursor
  static HCURSOR SetCursor( const std::string & sCursor );

  // sets the current mosue cursor
  static HCURSOR SetCursor( HMODULE module, const std::string & sCursor );

// data
private:

  // indicates if mouse cursor should be restored in destructor
  HCURSOR m_hPreviousCursor;

  // base module containing cursor resources
  static HMODULE m_module;
}; // class vxMouseCursor


#endif // vxMouseCursor_h


// $Log: vxMouseCursor.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/22 16:34:27  dongqing
// code review
//
// Revision 1.7  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.6  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.5  2003/04/08 19:52:41  geconomos
// Updated for resource management.
//
// Revision 1.4  2002/11/25 13:40:34  geconomos
// No longer using cursor stack.
//
// Revision 1.3  2002/09/17 21:42:23  dmitry
// Multi HMODULE implemented.
//
// Revision 1.2  2002/09/16 19:58:07  dmitry
// HMODULE used instead of int4.
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.5  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.4  2002/06/20 02:17:37  dmitry
// cleaned up
//
// Revision 1.3  2002/06/14 15:55:12  geconomos
// Reimplemented mouse cursors.
//
// Revision 1.2  2002/06/12 21:19:46  michael
// Update method added to the vxMouseCursor.
//
// Revision 1.1  2002/05/02 15:31:10  michael
// added vxMouseCursor
//
// Revision 1.4  2002/04/23 20:49:39  dmitry
// Stack implemented.
//
// Revision 1.3  2002/01/04 21:16:50  geconomos
// Added Hand open and closed cursors.
//
// Revision 1.2  2002/01/03 21:42:37  geconomos
// Added new mouse cursors.
//
// Revision 1.1  2001/12/19 13:50:33  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMouseCursor.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxMouseCursor.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
