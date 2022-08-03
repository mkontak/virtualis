// $Id: StudyBrowserView.C,v 1.6 2007/03/01 01:18:56 jmeade Exp $
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StudyBrowserView.C  
// Description: implementation of the StudyBrowserView class
// Owner: Jeffrey Cohen  jcohen@viatronix.net, Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "StudyBrowserView.h"
#include "Viatron.h"

#include "MainAppFrame.h"
#include "V1KMessages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ColonViews::Layouts;

//////////////////////////////////////////////////////////////////////
// StudyBrowserFrame

IMPLEMENT_DYNCREATE(StudyBrowserFrame, CMDIChildWnd)


/**
 * Constructor 
 */
StudyBrowserFrame::StudyBrowserFrame() : ChildLayoutFrame(new StudyBrowserFrame::ViewWnd)
{

} // constructor


/**
 * Destructor 
 */
StudyBrowserFrame::~StudyBrowserFrame()
{

} // destructor

//////////////////////////////////////////////////////////////////////
// StudyBrowserFrame::ViewWnd


/**
 * Constructor 
 */
StudyBrowserFrame::ViewWnd::ViewWnd() : GLWnd(IDB_BLUEMETAL)
{

} // constructor


/**
 * Destructor
 */
StudyBrowserFrame::ViewWnd::~ViewWnd()
{

} // destructor

BEGIN_MESSAGE_MAP(StudyBrowserFrame::ViewWnd, GLWnd)
END_MESSAGE_MAP()


/**
 * Create handler for the view
 *
 * @param hWndThis       Handle to this window
 */
void StudyBrowserFrame::ViewWnd::CreateChildViews(HWND hWndThis)
{
  CWnd* pWndThis = CWnd::FromHandle(hWndThis);
} // CreateChildViews()


/**
 * Effectively, the WM_SIZE handler
 *
 * @param iWidth    Width of view
 * @param iHeight   Height of view
 */
void StudyBrowserFrame::ViewWnd::CalculateViewRects(const int4 iWidth, const int4 iHeight)
{
} // CalculateViewRects()


// Revision History:
// 
// $Log: StudyBrowserView.C,v $
// Revision 1.6  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.5  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.4  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/10/03 13:01:01  geconomos
// new gui build
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.9.2.1.12.1  2005/06/17 13:16:30  geconomos
// set default backgound resource id to IDB_BLUEMETAL
//
// Revision 3.9.2.1  2003/02/22 12:20:28  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.9  2002/11/15 00:40:39  jmeade
// No hard-coding of # of child views.
//
// Revision 3.8  2002/09/25 19:31:40  jmeade
// Unused study open accelerator message.
//
// Revision 3.7  2002/09/18 20:06:16  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 3.6  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.4  2001/12/28 20:40:33  jmeade
// return of v3DConsole main frame.
//
// Revision 3.3  2001/12/17 19:59:47  jmeade
// No background for the study browser view (dialog).
//
// Revision 3.2  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1.2.2  2001/10/30 23:11:24  jmeade
// Now able to detect and report study file access errors
//
// Revision 3.1.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.1  2001/10/18 16:43:09  jmeade
// Coding standards (removing unused resource items, obsolete code)
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 02 2001 18:43:02   jmeade
// File system location (for service) searches for local studies
// 
//    Rev 2.0   Sep 16 2001 23:42:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:00   ingmar
// Initial revision.
// Revision 1.29  2001/08/30 19:06:05  jmeade
// ID 120:  Show Deleted studies menu option
//
// Revision 1.28  2001/08/24 15:55:57  jmeade
// Handle removed FileExtensions const char * operator
//
// Revision 1.27  2001/08/21 02:25:57  jmeade
// Enums for view window positions (no hardcoded values)
//
// Revision 1.26  2001/08/01 20:37:28  jmeade
// intermediate checkin for new database changes
//
// Revision 1.25  2001/07/27 22:59:17  jmeade
// Dataset state (instead of status) and study id types
//
// Revision 1.24  2001/06/14 00:36:16  jmeade
// coding conventions
//
// Revision 1.23  2001/06/08 01:14:00  jmeade
// changes for new database waaaaay too numerous to mention
//
// Revision 1.22  2001/04/13 02:34:36  jmeade
// headers and footers
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/StudyBrowserView.C,v 1.6 2007/03/01 01:18:56 jmeade Exp $
// $Id: StudyBrowserView.C,v 1.6 2007/03/01 01:18:56 jmeade Exp $
