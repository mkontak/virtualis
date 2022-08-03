//////////////////////////////////////////////////////////////////////
// $Id: ViatronDialog.C,v 1.7.2.2 2010/04/21 14:05:04 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: ViatronDialog dialog helper class (e.g. for skinned-looking dialogs)
// Owner: Jeff Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "viatron.h"
#include "ViatronDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonViews::Dialogs;

#define FILE_REVISION "$Revision: 1.7.2.2 $"

/**
 * Constructor 
 *
 * @param uIDTemplate                Dialog resource ID.
 * @param pParent                    Parent window.
 * @param uDlgBackgroundResourceID   Background image ID.
 */
ViatronDialog::ViatronDialog(UINT uIDTemplate, CWnd* pParent /*=NULL*/, const uint4 uDlgBackgroundResourceID /*=IDB_BLUEMETAL*/)
  : CDialog(uIDTemplate, pParent), m_uDlgBackgroundResourceID(uDlgBackgroundResourceID), m_bResetMouseTrackingEvents(false), m_pBackgroundBrush( NULL ),
  m_iSizeCount( 0 )
{
} // default constructor


/**
 * Constructor 
 */
ViatronDialog::ViatronDialog() : CDialog()
{
} // constructor


BEGIN_MESSAGE_MAP(ViatronDialog, CDialog)
  ON_WM_SIZE()
  ON_WM_CTLCOLOR()
  ON_WM_MOUSEMOVE()
  ON_WM_NCLBUTTONDBLCLK()
  ON_WM_DESTROY()
  //ON_WM_ERASEBKGND()
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
  ON_MESSAGE(ViatronDialog::WM_USER_LAYOUT_CHANGE, OnMessageLayoutChangeEvent)
END_MESSAGE_MAP()


/**
 * WM_DESTROY handler
 */
void ViatronDialog::OnDestroy()
{
	CDialog::OnDestroy();
  if( m_pBackgroundBrush != NULL )
  {
    delete m_pBackgroundBrush;
    m_pBackgroundBrush = NULL;
  }
} // OnDestroy()


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param pdc   Device context for window
 * @return      Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL ViatronDialog::OnEraseBkgnd( CDC * pdc ) 
{
  return TRUE;
} // OnEraseBkgnd()


/***
 * Moves a single child window during enumeration of CenterChildControls method
 *
 * @param hWndChild   enumerated child
 * @param lParam      pointer to move window information structure
 */
BOOL CALLBACK ViatronDialog::MoveChildWinEnumProc(HWND hWndChild, LPARAM lParam)
{
  const MoveChildWinStruct& rMCWS = *((MoveChildWinStruct*)lParam);

  if (::GetDlgCtrlID(hWndChild) == rMCWS.uContainerID)
    return TRUE;

  CRect rtChild;
  ::GetWindowRect(hWndChild, rtChild);
  WinUtils::ScreenToClient(rMCWS.hParent, rtChild);

  ::SetWindowPos(hWndChild, NULL, rtChild.left+rMCWS.szDiff.cx, rtChild.top+rMCWS.szDiff.cy, 0,0, SWP_NOSIZE | SWP_NOZORDER);

  return TRUE;
}


/** centers all children of the dialog box within the specified groupbox
 *
 * @param uControlContainerBoxID   a groupbox that, within the resource file, encapsulates all children
 */
void ViatronDialog::CenterChildControls(const uint4 uControlContainerBoxID)
{
  CWnd *pWndContainer;
  if ( (pWndContainer = GetDlgItem(uControlContainerBoxID)) != NULL )
  {
    CRect rtBefore;  pWndContainer->GetWindowRect(rtBefore);
    pWndContainer->CenterWindow();
    CRect rtAfter;   pWndContainer->GetWindowRect(rtAfter);

    MoveChildWinStruct mcws;
    mcws.uContainerID = uControlContainerBoxID;
    mcws.szDiff = rtAfter.TopLeft() - rtBefore.TopLeft();
    mcws.hParent = GetSafeHwnd();
    EnumChildWindows(mcws.hParent, ViatronDialog::MoveChildWinEnumProc, LPARAM(&mcws));

    pWndContainer->ShowWindow(SW_HIDE);
  }
} // CenterChildControls(const uint4 uControlContainerBoxID)


/**
 * Colors dialog's controls.
 *
 * @param pDC         Device context for painting controls.
 * @param pWnd        Handle to the control being drawn.
 * @param uCtlColor   Identifies the type of control being drawn.
 * @return            Brush to paint control.
 */
HBRUSH ViatronDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor) 
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, uCtlColor);

  // TODO: Change any attributes of the DC here
  pDC->SetBkMode(TRANSPARENT);

  // TODO: Return a different brush if the default is not desired
  if (uCtlColor != CTLCOLOR_EDIT)
  {
    hbr = HBRUSH(*m_pBackgroundBrush);
  }

  if (pWnd->GetStyle() & WS_CHILD)
  {
    CRect ctlRect; pWnd->GetWindowRect(ctlRect);  ScreenToClient(ctlRect);
    switch (uCtlColor)
    {
      case CTLCOLOR_DLG:
      {
        pDC->SetBrushOrg(0, 0);
        break;
      }
      case CTLCOLOR_LISTBOX:
      {
        pDC->SetBrushOrg(-(ctlRect.left+2)-3, -(ctlRect.top+2)-3);
        break;
      }
      case CTLCOLOR_STATIC:
      {
        pDC->SetBrushOrg(-ctlRect.left, -ctlRect.top);
        break;
      }
      default:
      {
        break;
      }
    }
  }

  return hbr;
} // OnCtlColor()


/**
 * Gets tool tip text
 *
 * @param uID          Control ID
 * @param pTTTStruct   Pointer to tool tip structure
 * @param pResult      Result
 * @return             TRUE if text is set
 */
BOOL ViatronDialog::OnToolTipNeedText(UINT uID, NMHDR *pTTTStruct, LRESULT *pResult)
{
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
  UINT uIDFrom = pTTTStruct->idFrom;
  if (pTTT->uFlags & TTF_IDISHWND)
  {
    // idFrom is actually the HWND of the tool
    uIDFrom = ::GetDlgCtrlID((HWND)uIDFrom);
    if (uIDFrom == 0)
    {
      return FALSE;
    }

    pTTT->lpszText = MAKEINTRESOURCE(uIDFrom);
    pTTT->hinst = AfxGetResourceHandle();
    return TRUE;
  }

  return FALSE;
} // OnToolTipNeedText()


/**
 * Called when dialog is resized.
 *
 * @param uType   Identifies the resize event.
 * @param iCX     Width of dialog.
 * @param iCY     Height of dialog.
 */
void ViatronDialog::OnSize(UINT uType, int iCX, int iCY) 
{
  CDialog::OnSize(uType, iCX, iCY);

  // Add message handler code here
  if( iCX > 0 && iCY > 0 )
  {

    if( m_pBackgroundBrush != NULL )
    {
      delete m_pBackgroundBrush;
      m_pBackgroundBrush = NULL;
    }

    m_pBackgroundBrush = new WinUtils::BackgroundBrush();
    m_pBackgroundBrush->Create( this, m_uDlgBackgroundResourceID );

  }
} // OnSize()


/**
 * Mouse movement handler.
 *
 * @param uFlags   Indicates which virtual keys are currently held down.
 * @param point    Location of mouse.
 */
void ViatronDialog::OnMouseMove(UINT uFlags, CPoint point) 
{
	if (m_bResetMouseTrackingEvents)
  {
    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_HOVER | TME_LEAVE, GetSafeHwnd(), HOVER_DEFAULT };
    _TrackMouseEvent(&tme);
    m_bResetMouseTrackingEvents = false;
  }
  //GetParentOwner()->SetFocus();
  CDialog::OnMouseMove(uFlags, point);
} // OnMouseMove()


/**
 * Dispatches window messages.
 *
 * @param uMessage   Message ID.
 * @param wParam     Message-dependent parameter.
 * @param lParam     Message-dependent parameter.
 * @return           Message-dependent.
 */
LRESULT ViatronDialog::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam) 
{
  switch (uMessage)
  {
    case WM_MOUSEHOVER:
    {
      OnMouseHover( UINT(wParam), CPoint(LOWORD(lParam), HIWORD(lParam)) );

      // Hover tracking must be reset after handling
      TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_HOVER, GetSafeHwnd(), HOVER_DEFAULT };
      _TrackMouseEvent(&tme);
      
      break;
    }
    
    case WM_MOUSELEAVE:
      OnMouseLeave();
      EnableMouseTracking();
      break;
  }

  return CDialog::WindowProc(uMessage, wParam, lParam);
} // WindowProc()


/**
 * Performs operation specified in lParam parameter on the indicated window.
 * 
 * @param hwnd     Handle to child window.
 * @param lParam   eEnumChildrenProc, indicates operation to perform.
 * @return         To continue enumeration, return TRUE; to stop enumeration, return FALSE. 
 */
BOOL CALLBACK ViatronDialog::ChildWindowEnumProc(HWND hwnd, LPARAM lParam)
{
  switch (eEnumChildrenProc(lParam))
  {
    case eShow:
      ::ShowWindow(hwnd, SW_SHOW);
      break;

    case eHide:
      ::ShowWindow(hwnd, SW_HIDE);
      break;

    case eEnable:
      ::EnableWindow(hwnd, TRUE);
      break;

    case eDisable:
      ::EnableWindow(hwnd, FALSE);
     break;

    default:
     break;
  }

  return TRUE;
} // ChildWindowEnumProc


/**
 * Performs specified function on all children of *this window
 *
 * @param eProc   Function to perform on child windows
 */
void ViatronDialog::ProcAllChildWindows(const eEnumChildrenProc eProc)
{
  EnumChildWindows(*this, ChildWindowEnumProc, LPARAM(eProc));
} // ProcAllChildWindows


/**
 * Enables mouse tracking events (hover and leave).
 */
void ViatronDialog::EnableMouseTracking()
{
  m_bResetMouseTrackingEvents = true;
} // EnableMouseTracking


/**
 * Mouse hovering over view for a prescibed time period
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    Coordinates of the cursor.
 */
void ViatronDialog::OnMouseHover(UINT uFlags, CPoint origPt)
{
} // OnMouseHover


/**
 * Mouse leaving client area.
 */
void ViatronDialog::OnMouseLeave()
{
} // OnMouseLeave()


/**
 * Double click in non-client area
 *
 * @param uHitTest   hit test data
 * @param point      click point
 */
void ViatronDialog::OnNcLButtonDblClk(UINT uHitTest, CPoint point) 
{
  GetParent()->PostMessage(WM_NCLBUTTONDBLCLK, WPARAM(HWND(*this)), MAKELPARAM(point.x, point.y));
} // OnNcLButtonDblClk()


/**
 * layout changing or layout changed event
 * @param wParam   specifies layout change event
 */
LRESULT ViatronDialog::OnMessageLayoutChangeEvent(WPARAM wParam, LPARAM)
{
  if (wParam == ViatronDialog::eWMUserWPLayoutChanging)
  {
    OnLayoutChanging();
  }
  else
  {
    OnLayoutChanged();
  }

  return 1;
} // OnMessageLayoutChangeEvent()


// Revision History:
// $Log: ViatronDialog.C,v $
// Revision 1.7.2.2  2010/04/21 14:05:04  dongqing
// set focus to parent to ensure proper application message routing (e.g. problem with F12 stuck in missed regions dialog)
//
// Revision 1.7.2.1  2010/04/11 11:48:06  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7  2007/03/07 20:19:17  jmeade
// comments.
//
// Revision 1.6  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5  2007/01/09 19:16:43  jmeade
// layout changed/changing events for dialogs.
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.4.3  2006/05/05 12:06:47  geconomos
// correctly freeing brushes when windows ins destroyed
//
// Revision 1.3.4.2  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.3.4.1  2005/12/20 17:28:28  jmeade
// comments.
//
// Revision 1.3  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.6.4.1.4.1.2.1  2005/06/17 13:05:52  geconomos
// using new BackgroundBrush class
//
// Revision 3.6.4.1.4.1  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.6.4.1.8.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 3.6.4.1  2003/02/27 23:14:19  jmeade
// Put common tooltip text get method in base class for use by all derivatives.
//
// Revision 3.6  2002/07/26 22:43:25  jmeade
// Pass non-client area double-click (i.e. activate/deactivate) message to parent for handling.
//
// Revision 3.5  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.4.2.1  2002/04/10 20:14:44  jmeade
// Implemented mouse tracking.
//
// Revision 3.4  2002/01/29 03:02:54  jmeade
// Function to perform operations on all child windows.
//
// Revision 3.3  2001/12/04 17:47:32  jmeade
// Removed debug code.
//
// Revision 3.2  2001/11/18 01:10:40  jmeade
// Variable naming.
//
// Revision 3.1  2001/11/17 03:47:12  jmeade
// Threw out the old carpet.
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 26 2001 23:56:48   jmeade
// Default CDialog-derived constructor
// 
//    Rev 2.0   Sep 16 2001 23:43:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:20   ingmar
// Initial revision.
// Revision 1.2  2001/04/13 02:35:22  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronDialog.C,v 1.7.2.2 2010/04/21 14:05:04 dongqing Exp $
// $Id: ViatronDialog.C,v 1.7.2.2 2010/04/21 14:05:04 dongqing Exp $
