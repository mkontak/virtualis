// $Id: vxMouseCursor.C,v 1.1.1.1.34.1 2008/11/18 19:33:44 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)


// include declarations
#include "stdafx.h"
#include "vxMouseCursor.h"


// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1.34.1 $"


// static member initialization
HMODULE vxMouseCursor::m_module = NULL;


/**
 * Default constructor
 */
vxMouseCursor::vxMouseCursor() :
m_hPreviousCursor(NULL)
//*******************************************************************
{
} // vxMouseCursor()


/**
 * Constructor that sets the mouse to the specified cursor. 
 * The destructor will return the cursor to the default.
 *
 * @param eCursor cursor type
 */
vxMouseCursor::vxMouseCursor(const std::string sCursor) 
//*******************************************************************
{
  m_hPreviousCursor = SetCursor(sCursor);
} // vxMouseCursor()


/**
 * Destructor
 */
vxMouseCursor::~vxMouseCursor()
//*******************************************************************
{
  if (m_hPreviousCursor != NULL)
  {
    ::SetCursor(m_hPreviousCursor);
  }
} // ~vxMouseCursor()


/**
 * Set the mouse to the specified cursor
 * @param sCursor Name of Cursor
 * @return previous cursor
 */
HCURSOR vxMouseCursor::SetCursor(const std::string & sCursor)
//*******************************************************************
{
  // update the mouse cursor
  return SetCursor(m_module, sCursor);
} // SetCursor()


/**
 * Set the mouse to the specified cursor
 * @param module containing mouse cursor
 * @param name of Cursor
 * @return previous cursor
 */
HCURSOR vxMouseCursor::SetCursor(HMODULE hModule, const std::string & sCursor)
//*******************************************************************
{
  hModule = hModule == NULL? m_module : hModule;

  HCURSOR hCursor = NULL;

  // special case for default cursor
  if(sCursor == "CURSOR_DEFAULT")
  {
    hCursor = ::LoadCursor(NULL, IDC_ARROW);
  } 
    
  // special case for wait cursor
  else if(sCursor == "CURSOR_WAIT")
  {
    hCursor = ::LoadCursor(NULL, IDC_WAIT);
  } 

  // special case for cross hair cursor
  else if(sCursor == "CURSOR_CROSSHAIR")
  {
    hCursor = ::LoadCursor(NULL, IDC_CROSS);
  } 
    
  // all other cursors
  else 
  {
    hCursor = ::LoadCursor(hModule, sCursor.c_str());
    
    // if the cursor was unabled to be loaed, load the default arrow
    if(hCursor == NULL)
    {
      hCursor = ::LoadCursor(NULL, IDC_ARROW);
    }
  }
  
  // update the mouse cursor
  return ::SetCursor(hCursor);
} // SetCursor()


#undef FILE_REVISION

  
// $Log: vxMouseCursor.C,v $
// Revision 1.1.1.1.34.1  2008/11/18 19:33:44  gdavidson
// Issue #6203: Added center selection for obliques
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.8  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.7  2003/04/08 19:52:41  geconomos
// Updated for resource management.
//
// Revision 1.6  2002/11/25 13:40:34  geconomos
// No longer using cursor stack.
//
// Revision 1.5  2002/09/18 15:26:57  dmitry
// Ooops, ! bug fixed.
//
// Revision 1.4  2002/09/17 21:42:23  dmitry
// Multi HMODULE implemented.
//
// Revision 1.3  2002/09/16 19:58:07  dmitry
// HMODULE used instead of int4.
//
// Revision 1.2  2002/08/01 18:59:33  frank
// spelling
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
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
// Revision 1.7  2002/04/24 21:26:23  dmitry
// pop after top push
//
// Revision 1.6  2002/04/23 20:49:39  dmitry
// Stack implemented.
//
// Revision 1.5  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
//
// Revision 1.4  2002/01/09 19:00:01  frank
// Fixed cursors.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxMouseCursor.C,v 1.1.1.1.34.1 2008/11/18 19:33:44 gdavidson Exp $
// $Id: vxMouseCursor.C,v 1.1.1.1.34.1 2008/11/18 19:33:44 gdavidson Exp $


