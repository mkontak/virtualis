// $Id: MessageWnd.C,v 1.3 2007/03/12 20:47:11 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MessageWnd.C
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "MessageWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace WinUtils;

/////////////////////////////////////////////////////////////////////////////
// MessageWnd

IMPLEMENT_DYNCREATE(MessageWnd, CFrameWnd)

/**
 * constructor
 **/
MessageWnd::MessageWnd() : m_uDurationMS(2000), m_uDestroyWinTimerID(0)
{
} // constructor


/**
 * destructor
 **/
MessageWnd::~MessageWnd()
{
} // destructor


BEGIN_MESSAGE_MAP(MessageWnd, CFrameWnd)
	//{{AFX_MSG_MAP(MessageWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MessageWnd message handlers


/**
 * WM_CREATE, called during the creation phase of the window.
 *
 * @param   lpCreateStruct   Info on the window being created
 * @return  Return 0 to continue the creation of the CWnd object. If –1, the window will be destroyed.
 */
int MessageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_uDestroyWinTimerID = SetTimer(1, m_uDurationMS, NULL);
	return 0;
} // OnCreate()


/**
 * WM_TIMER handler
 *
 * @param uIDEvent   ID of the timer event
 */
void MessageWnd::OnTimer(UINT_PTR uIDEvent) 
{
  if (uIDEvent == m_uDestroyWinTimerID)
  {
    KillTimer(uIDEvent);
    EndMessageWnd();
    return;
  }

  CFrameWnd::OnTimer(uIDEvent);
} // OnTimer()


/**
 * WM_TIMER handler
 *
 * @param sMessage      message to be displayed
 * @param uDurationMS   length of time for a temporary window to be displayed
 * @param eStyle        type of message window
 */
void MessageWnd::Create(const CString& sMessage, const uint4 uDurationMS, const MsgStyle eStyle)
{
  m_uDurationMS = uDurationMS;
  m_sMessage = sMessage;
  m_eStyle = eStyle;

  CRect winRect(0,0,400,200);
  if (CFrameWnd::Create(NULL, AfxGetAppName(), WS_OVERLAPPED, winRect,
    NULL, NULL, 0, NULL) == FALSE)
    return;

  NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
  SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
  m_font.DeleteObject();
  m_font.CreateFontIndirect(&ncm.lfMessageFont);
  SetFont(&m_font);

  m_staticBkgnd.Create("", WS_VISIBLE | WS_CHILD, winRect, this);
  CClientDC bkgddc(&m_staticBkgnd);
  bkgddc.SetBkColor(GetSysColor(COLOR_3DFACE));

  m_staticMessage.Create(m_sMessage, WS_VISIBLE | WS_CHILD | SS_CENTER,
    CRect(10,50,390,110), this);
  m_staticMessage.SetFont(&m_font);

  CRect textRect(15,50,0,0);
  CClientDC msgdc(&m_staticMessage);
  msgdc.DrawText(m_sMessage, textRect, DT_CALCRECT);
  const int iNumTextLines((textRect.Width() / winRect.Width()) + 1);

  if (iNumTextLines > 1)
  {
    textRect.right = winRect.right - textRect.left; // give the same border on left & right
    textRect.bottom = textRect.top + int(textRect.Height() * 1.3f) * iNumTextLines;
  }
  
  textRect.InflateRect(10,10);
  m_staticMessage.MoveWindow(textRect);

  if (eStyle != MW_DEFAULT)
  {
    if (eStyle & MW_WAIT)
    {
      m_btnOK.Create("Wait", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, CRect(160,125,240,155),
        this, eWAITBTNID);
    }
    else if (eStyle & MW_OK)
    {
      m_btnOK.Create("OK", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, CRect(160,125,240,155),
        this, eOKBTNID);
    }

    m_btnOK.SetFont(&m_font);
  }

  CenterWindow();
  BringWindowToTop();
  ShowWindow(SW_SHOW);
} // Create()


/**
 * Create and display the window
 *
 * @param sMessage        message to be displayed
 * @param uDurationMS     length of window display
 * @param eStyle          style of message window
 **/
void MessageWnd::Show(const CString& sMessage, const uint4 uDurationMS, const MsgStyle eStyle)
{
  MessageWnd* wnd = new MessageWnd;
  wnd->Create(sMessage, uDurationMS, eStyle);

  if (eStyle != MW_DEFAULT)
  {
    wnd->RunModalLoop();
  }
} // Show()


/**
 * Posts resize message for property or wizard button click events
 * 
 * @param wParam   Identifies button pressed
 * @param lParam   lparam for message
 * @return         Return code from base class OnCommand() (whether command was handled)
 */
BOOL MessageWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  switch (wParam)
  {
    case eWAITBTNID:
      KillTimer(m_uDestroyWinTimerID);
      m_uDestroyWinTimerID = 0;
      m_btnOK.SetDlgCtrlID(eOKBTNID);
      m_btnOK.SetWindowText("OK");
      break;

    case eOKBTNID:
      EndMessageWnd();
      break;
    
    default:
      break;
  }
	
	return CFrameWnd::OnCommand(wParam, lParam);
} // OnCommand()


/**
 * closes message window
 */
void MessageWnd::EndMessageWnd()
{
  if (m_eStyle != MW_DEFAULT)
    EndModalLoop(0);
  PostMessage(WM_CLOSE, 0, 0);
} // EndMessageWnd()


// Revision History:
// $Log: MessageWnd.C,v $
// Revision 1.3  2007/03/12 20:47:11  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:42   ingmar
// Initial revision.
// Revision 1.1  2001/06/14 00:38:14  jmeade
// method (and accompanying class) for temp message boxes
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MessageWnd.C,v 1.3 2007/03/12 20:47:11 jmeade Exp $
// $Id: MessageWnd.C,v 1.3 2007/03/12 20:47:11 jmeade Exp $
