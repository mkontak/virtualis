// $Id: ChildViewFrame.C,v 1.7 2007/03/01 01:18:55 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ChildViewFrame.C  
// Description: Implementation of MDI Child Wnd encapsulating a GLWnd
// Owner: Jeff Meade  @jmeade@viatronix.net


#include "stdafx.h"
#include "ChildViewFrame.h"
#include "Theme.h"

#define FILE_REVISION "$Revision: 1.7 $"

using namespace ColonViews::Layouts;
using namespace WinUtils;

BEGIN_MESSAGE_MAP(ChildLayoutFrame, CMDIChildWnd)
  ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
  ON_WM_SETFOCUS()
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_ERASEBKGND()
  ON_WM_DESTROY()
END_MESSAGE_MAP()


/**
 * Constructor 
 * 
 * @param pWnd   Child View Window
 */
ChildLayoutFrame::ChildLayoutFrame(GLWnd *pWnd)
{
  // TODO: add member initialization code here
  m_pWndView = pWnd;  
} // constructor


/**
 * Destructor 
 */
ChildLayoutFrame::~ChildLayoutFrame()
{
  LogDbg ( "ChildLayoutFrame::~ChildLayoutFrame()", "", "" );
} // destructor


/**
 * WM_DESTROY handler
 */
void ChildLayoutFrame::OnDestroy()
{
  CMDIChildWnd::OnDestroy();
  LogDbg ( "ChildLayoutFrame::OnDestroy()", "", "" );

  if ( m_pWndView )
  {
    m_pWndView->DestroyWindow();
    delete m_pWndView;
    m_pWndView = NULL;
  }
} // OnDestroy()

/**
 * Called to set window creation parameters
 * 
 * @param cs   Create parameter struct
 * @return     Whether or not to continue on to window creation
 */
BOOL ChildLayoutFrame::PreCreateWindow( CREATESTRUCT & cs )
{
  if( !CMDIChildWnd::PreCreateWindow(cs) )
    return FALSE;

  // make sure view is maximum size to eliminate flicker when switching tabs
  cs.x = cs.y = 0;
  cs.cx = cs.cy = 32767;

  cs.style &= ~WS_SYSMENU;
  cs.style |= WS_CLIPCHILDREN;
  cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
  return TRUE;
} // PreCreateWindow()


/**
 * Activate and restores the frame window so that it is visible and available to the user
 */
void ChildLayoutFrame::ActivateFrame( int uShowCmd )
{ 
  CMDIChildWnd::ActivateFrame( SW_MAXIMIZE ); 
} // ActivateFrame()


/**
 * File menu Close item handler
 */
void ChildLayoutFrame::OnFileClose()
{
  // To close the frame, just send a WM_CLOSE, which is the equivalent
  // choosing close from the system menu.

  SendMessage(WM_CLOSE);
} // OnFileClose()


/**
 * Creates child view window on frame creation
 * 
 * @param lpCreateStruct   Create parameters (for frame window)
 * @return                 Success of creation
 */
int ChildLayoutFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if( CMDIChildWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;

  // create a view to occupy the client area of the frame
  if ( ( !m_pWndView->Create( NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, 1000, NULL ) ) )
    return -1;

  return 0;
} // OnCreate()


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pDC     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL ChildLayoutFrame::OnEraseBkgnd( CDC * pDC ) 
{
  return TRUE;
} // OnEraseBkgnd()


/**
 * Called when frame is resized
 *
 * @param uType   Type of sizing (max/min/restore/etc)
 * @param cx      Width
 * @param cy      Height
 */
void ChildLayoutFrame::OnSize( UINT uType, int cx, int cy ) 
{
  CMDIChildWnd::OnSize( uType, cx, cy );

  if( m_pWndView )
    m_pWndView->MoveWindow( 0, 0, cx, cy );
} // OnSize()


/**
 * Set focus to child window
 * 
 * @param pOldWnd   Previous focus window
 */
void ChildLayoutFrame::OnSetFocus(CWnd* pOldWnd) 
{
  CMDIChildWnd::OnSetFocus(pOldWnd);

  m_pWndView->SetFocus();
} // OnSetFocus()


/**
 * Redirecting window messages
 * 
 * @param uID            Message ID
 * @param iCode          message code??           
 * @param pExtra         extra message-dependent info
 * @param pHandlerInfo   handling info
 * @return               Whether command was handled
 */
BOOL ChildLayoutFrame::OnCmdMsg(UINT uID, int iCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
  // let the view have first crack at the command
  if( m_pWndView->OnCmdMsg(uID, iCode, pExtra, pHandlerInfo) )
    return TRUE;
  
  // otherwise, do default handling
  return CMDIChildWnd::OnCmdMsg(uID, iCode, pExtra, pHandlerInfo);
} // OnCmdMsg()


/**
 * Process window messages
 * 
 * @param uMessage   Message ID
 * @param wParam     Message parameter
 * @param lParam     Message parameter
 * @return LRESULT   Whether message was handled
 */
LRESULT ChildLayoutFrame::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam) 
{
  if (m_pWndView && m_pWndView->GetSafeHwnd())
  {
    if ((uMessage >= WM_APP) && (uMessage <= 0xBFFF))
    {
      m_pWndView->PostMessage(uMessage, wParam, lParam);
      return TRUE;
    }
  }
  return CMDIChildWnd::WindowProc(uMessage, wParam, lParam);
} // WindowProc()

// Revision History:
// $Log: ChildViewFrame.C,v $
// Revision 1.7  2007/03/01 01:18:55  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.6  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5.2.1  2006/05/16 12:39:46  geconomos
// proper destruction of contained view window in WM_DESTROY handler
//
// Revision 1.5  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/10/05 15:23:49  geconomos
// Modified child frame such that it is always maximized within the parent frame
//
// Revision 1.3  2005/10/03 13:01:01  geconomos
// new gui build
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.20.1.4.1  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.0.20.1.8.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 3.0.20.1  2003/02/22 11:54:47  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:00   ingmar
// Initial revision.
// Revision 1.10  2001/05/03 20:59:35  jmeade
// doxygen comments
//
// Revision 1.9  2001/05/02 20:25:28  ingmar
// .
//
// Revision 1.8.2.1  2001/05/02 00:02:03  jmeade
// tabs to spaces; bracketing (April 26 - jmeade)
//
// Revision 1.9  2001/04/26 15:52:48  jmeade
// tabs to spaces, bracketing
//
// Revision 1.8  2001/04/13 02:31:49  jmeade
// headers and footers
//
// Revision 1.7  2000/12/27 23:06:06  jeff
// global GetViatronMainFrame() ==> static MainAppFrame::GetMainFrame()
//
// Revision 1.6  2000/12/20 19:21:18  jeff
// OnSize() handler to resize view window
//
// Revision 1.5  2000/12/15 18:51:30  jeff
// better (yet static) calculation of viewframe client rectangle
//
// Revision 1.4  2000/10/26 20:47:41  jeff
// Correcting class name reference in code comments
//
// Revision 1.3  2000/10/02 21:56:46  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.2  2000/09/30 02:34:51  jeff
// Added common view frame code to PreCreateWindow()
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 10    8/28/00 1:02p Antonio
// updated header and footer
//
//*****************  Version 9  *****************
//User: Jeff            Date:  8/23/00  Time:  2:50p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added GetActiveViewWnd() function (returns the GLViewWnd * member)
//
//*****************  Version 8  *****************
//User: Jeff            Date:  8/23/00  Time: 12:57a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Only reroute WM_APP messages to the view window (otherwise, endless
//loops abound!)
//
//*****************  Version 7  *****************
//User: Jeff            Date:  8/22/00  Time: 11:35p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  WindowProc messages (e.g. WM_USER, WM_APP) redirected through the
//child frame window to its child view window
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 6  *****************
//User: Jeff            Date:  8/14/00  Time:  6:16p
//Checked in $/ViatronMDI/src/Viatron
//
//*****************  Version 5  *****************
//User: Jeff            Date:  8/12/00  Time: 10:45p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added the Endoscopic-Axial view window (first beta)
//Added a small edit field to dialog bar to test that it doesn't "steal" the mouse
//(it doesn't)
//Many other class organizational changes for (multiple view windows)
//
//*****************  Version 4  *****************
//User: Jeff            Date:  8/12/00  Time:  8:01p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  First errorless compile after changes to class architecture
//
//*****************  Version 3  *****************
//User: Jeff            Date:  8/12/00  Time:  6:47p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Changing class architecture
//
//*****************  Version 2  *****************
//User: Jeff            Date:  8/12/00  Time:  5:53p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  
//
//*****************  Version 1  *****************
//User: Jeff            Date:  8/11/00  Time:  3:44p
//Created MainViewFrame.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ChildViewFrame.C,v 1.7 2007/03/01 01:18:55 jmeade Exp $
// $Id: ChildViewFrame.C,v 1.7 2007/03/01 01:18:55 jmeade Exp $
