// $Id: ReportViewFrame.C,v 1.6 2007/03/09 15:59:01 jmeade Exp $
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos


#include "stdafx.h"
#include "ReportViewFrame.h"
#include "ReportViewWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ColonViews::Layouts;
using namespace ColonReport;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(ReportLayoutFrame, CMDIChildWnd)

/**
 * constructor
 **/
ReportLayoutFrame::ReportLayoutFrame() : ChildLayoutFrame(new ReportLayout)
{

} // constructor


/**
 * destructor
 **/
ReportLayoutFrame::~ReportLayoutFrame()
{
} // destructor


IMPLEMENT_DYNCREATE(ReportPreviewLayoutFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(ReportPreviewLayoutFrame, ChildLayoutFrame)
  //{{AFX_MSG_MAP(ReportPreviewLayoutFrame)
  ON_WM_SETFOCUS()
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_COMMAND(IDM_OPEN_REPORT, OnOpenPreview)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * constructor
 */
ReportPreviewLayoutFrame::ReportPreviewLayoutFrame() : ChildLayoutFrame(NULL)
{
  
} // constructor


/**
 * destructor
 */
ReportPreviewLayoutFrame::~ReportPreviewLayoutFrame()
{
  SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
  AfxGetApp()->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
} // destructor


/**
 * WM_CREATE
 *
 * @param lpCreateStruct   creation parameters
 * @return                 success of creation
 */
int ReportPreviewLayoutFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
  AfxGetApp()->SetThreadPriority(THREAD_PRIORITY_NORMAL);
  return 0;
} // OnCreate()


/**
 * create client routine
 *
 * @param lpcs       creation parameters
 * @param pContext   context parameters
 */
BOOL ReportPreviewLayoutFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
   pContext->m_pNewViewClass = RUNTIME_CLASS(ReportPreview);

   if(!CMDIChildWnd::OnCreateClient(lpcs, pContext))
     return FALSE;

 //  InitialUpdateFrame(NULL, TRUE);

   return TRUE;
} // OnCreateClient()


/**
 * WM_SIZE handler
 *
 * @param uType   identifies size event
 * @param iCX     width
 * @param iCY     height
 */
void ReportPreviewLayoutFrame::OnSize(UINT uType, int iCX, int iCY) 
{
  CMDIChildWnd::OnSize(uType, iCX, iCY);
} // OnSize()


/**
 * WM_SETFOCUS handler
 * @param pOldWnd   the previous window that had the focus
 */
void ReportPreviewLayoutFrame::OnSetFocus(CWnd* pOldWnd) 
{
  CMDIChildWnd::OnSetFocus(pOldWnd);
} // OnSetFocus()


/**
 * OnCmdMsg handler
 * @param uID            message id
 * @param uCode          message code
 * @param pExtra         message parameter
 * @param pHandlerInfo   message parameter
 */
BOOL ReportPreviewLayoutFrame::OnCmdMsg(UINT uID, int uCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
  return CMDIChildWnd::OnCmdMsg(uID, uCode, pExtra, pHandlerInfo);
} // OnCmdMsg()


/**
 * message processing procedure
 * @param uMessage   message id
 * @param wParam     message-dependent parameter
 * @param lParam     message-dependent parameter
 * @return           message-dependent
 */
LRESULT ReportPreviewLayoutFrame::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam) 
{
   return CMDIChildWnd::WindowProc(uMessage, wParam, lParam);
} // WindowProc()


/**
 * gets the active layout
 */
GLWnd *ReportPreviewLayoutFrame::GetActiveLayout() const
{
  return NULL;
} // GetActiveGLWnd()


/**
 * opens report preview
 */
void ReportPreviewLayoutFrame::OnOpenPreview()
{
} // OnOpenPreview()


// Revision History:
// $Log: ReportViewFrame.C,v $
// Revision 1.6  2007/03/09 15:59:01  jmeade
// code standards.
//
// Revision 1.5  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.4  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/09/13 17:27:19  jmeade
// ColonReport namespace.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.1.4.1  2003/02/22 11:59:03  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.1  2002/07/26 15:04:27  jmeade
// Fixes for Visual Studio class wizard database.
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:48   ingmar
// Initial revision.
// Revision 1.7  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.6  2001/02/01 17:07:14  geconomos
// Added comment headers and footers
//
// Revision 1.5  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportViewFrame.C,v 1.6 2007/03/09 15:59:01 jmeade Exp $
// $Id: ReportViewFrame.C,v 1.6 2007/03/09 15:59:01 jmeade Exp $
