// $Id: ReportSpinEdit.C,v 1.5 2007/03/09 23:38:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Owner drawn spin edit control that allows only numeric input
// Author: George Economos


#include "stdafx.h"
#include <math.h>
#include "viatron.h"
#include "ReportSpinEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonReport;

IMPLEMENT_DYNAMIC(ReportEdit, CEdit)

BEGIN_MESSAGE_MAP(ReportEdit, CEdit)
//{{AFX_MSG_MAP(ReportEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// Report Edit

/**
 * constructor
 */
ReportEdit::ReportEdit()
{
} // constructor

//////////////////////////////////////////////////////////////////////////
// ReportNumEdit

/**
 * dynamic instantiation
 */
IMPLEMENT_DYNAMIC(ReportNumEdit, ReportEdit)

/**
 * message map
 */
BEGIN_MESSAGE_MAP(ReportNumEdit, ReportEdit)
	//{{AFX_MSG_MAP(ReportNumEdit)
  ON_WM_CHAR()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * constructor
 */
ReportNumEdit::ReportNumEdit() : m_iDigits(0)
{
} // constructor


/**
 * window receives focus
 * @param pOldWnd   previous window with focus
 */
void ReportNumEdit::OnSetFocus(CWnd* pOldWnd) 
{
  ReportEdit::OnSetFocus(pOldWnd);

  GetWindowText(m_sInitial);
  
} // OnSetFocus


/**
 * window losing focus
 * @param pNewWnd   new window with focus
 */
void ReportNumEdit::OnKillFocus(CWnd* pNewWnd) 
{
  ReportEdit::OnKillFocus(pNewWnd);

  CString s;
  GetWindowText(s);
  if(s.IsEmpty())
    SetWindowText(m_sInitial);
  
} // OnKillFocus


/**
 * set number of allowed digits
 * @param iDigits   number of allowed digits
 */
void ReportNumEdit::SetDigits(const int4 iDigits)
{
	m_iDigits = iDigits;
} // SetDigits()


/**
 * get the value
 * @return   value of control
 */
float4 ReportNumEdit::GetValue() const
{
	CString s;
	GetWindowText(s);
	return  static_cast<float>(atof(s));
} // GetValue()


/**
 * set value of control
 * @param fVal   new value
 */
void ReportNumEdit::SetValue (float4 fVal)
{
  if (fVal > m_fMax)
  {
    if(m_bWrap)
      fVal = m_fMin;
    else
      fVal = m_fMax;
  }
  else if(fVal < m_fMin)
  {
    if( m_bWrap )
      fVal = m_fMax;
    else
      fVal = m_fMin;
  }  
  
	CString sFormat;
	sFormat.Format("%%0.%df",m_iDigits);
	
  CString s;  
  s.Format(sFormat, fVal+(0.5/pow( 10.0, m_iDigits+1)));
	SetWindowText(s);
} // SetValue()


/**
 * key pressed
 * @param uChar     character pressed.
 * @param uRepCnt   Repeat count.
 * @param uFlags    Key/mouse down flags.
 */
void ReportNumEdit::OnChar (UINT uChar, UINT uRepCnt, UINT uFlags)
{
	if (isdigit (uChar) && CheckInput(uChar) )
		ReportEdit::OnChar(uChar, uRepCnt, uFlags);
	else
	{
		CString txt;
		GetWindowText(txt);
		switch (uChar)
		{
		case '+' : 
		{
			float x = (float) atof(txt);
			if (x >0)
				break;
		}
		case '-' :
		{
			float x = (float) atof(txt);
			x = -x;
			SetValue(x);
			break;
		}
		case '.' :
			if ( txt.Find('.') == -1 && CheckInput(uChar))
				CEdit::OnChar(uChar, uRepCnt, uFlags);
			break; 
		case VK_BACK:
			ReportEdit::OnChar(uChar, uRepCnt, uFlags);
			break;
		}
	}
} // OnChar()


/**
 * accepts or rejects a character entered
 * @param uChar   character to validate
 * @return        whether character is accepted
 */
bool ReportNumEdit::CheckInput(UINT uChar)
{
	int4 iPos = CharFromPos(GetCaretPos()); 

  CString s;
	GetWindowText(s);
	
  int4 iLen = s.GetLength();
	
  int4 iSign = s.Find('-');
	if ( iPos <= iSign )
		return false;

  int4 iDot = s.ReverseFind('.');
	if ( iDot == -1 ) 
  {
		if (iPos >= iLen - m_iDigits)
			goto label;
    else
			return false;
	}
	
  if ( iLen - iPos < iDot)
  {
		if ( iPos -iDot > m_iDigits )
			return false;

    if ( iLen - iDot> m_iDigits) 
			return false;
	}

label:
  if ( isdigit(uChar))
  {
    int4 iPos = CharFromPos(GetCaretPos()); 
    
    CString txt;
    GetWindowText(txt);
    
    //CString input(uChar);
    txt.Insert(iPos,uChar);

    float4 value = (float) atof(txt);
    
    if ( value > m_fMax || value < m_fMin )
      return false;
  }
	
  return true;
} // CheckInput()


/**
 * set range of allowed values
 * @param fMin   minimum value
 * @param fMax   max value
 */
void ReportNumEdit::SetRange (const float fMin, const float fMax)
{
  m_fMax = fMax;
  m_fMin = fMin;
} // SetRange()


/**
 * whether to wrap text
 * @param bWrap   true to wrap text
 */
void ReportNumEdit::SetWrap (bool bWrap)
{
  m_bWrap = bWrap;
} // SetWrap()

//////////////////////////////////////////////////////////////////////////
// CSpin

IMPLEMENT_DYNAMIC(CSpin,CSpinButtonCtrl)
BEGIN_MESSAGE_MAP(CSpin,CSpinButtonCtrl)
//{{AFX_MSG_MAP(CSpin)
//}}AFX_MSG_MAP
ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltaPos)
END_MESSAGE_MAP()


/**
 * constructor
 */
CSpin::CSpin () : m_fDelta(1)
{
  // no code
} // constructor


/**
 * change in value
 * @param iSteps   value change
 */
void CSpin::Change (const int4 iSteps)
{
  CWnd* b = GetBuddy();
  ReportNumEdit* pEdit = dynamic_cast<ReportNumEdit*>(b);
  ASSERT(pEdit);
  
  float fDelta = iSteps*m_fDelta;
  float f = pEdit->GetValue();
  if( f == -fDelta )
    f = 0;
  else
    f += fDelta;

  pEdit->SetValue(f);
} // Change()


/**
 * on pos changed
 * @param pNMHDR    notification message handler
 * @param pResult   return value
 */
void CSpin::OnDeltaPos (NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
  DWORD bWrap = (GetStyle() & UDS_WRAP);
  
  if ((pNMUpDown->iPos <= pNMUpDown->iDelta) || (pNMUpDown->iPos >= 1000 - pNMUpDown->iDelta)) 
    pNMUpDown->iPos = 500;

  Change(pNMUpDown->iDelta);

  *pResult = 0;
} // OnDeltaPos()


/**
 * set the spin's delta
 * @param fX   the new delta
 */
void CSpin::SetDelta (const float4 fX)
{
  m_fDelta = fX;
} // SetDelta()


//////////////////////////////////////////////////////////////////////////
// ReportSpinEdit

IMPLEMENT_DYNAMIC(ReportSpinEdit, CWnd )
BEGIN_MESSAGE_MAP(ReportSpinEdit, CWnd)
	//{{AFX_MSG_MAP(ReportSpinEdit )
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  ON_WM_SETFOCUS()
  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * constructor
 */
ReportSpinEdit::ReportSpinEdit ()
{
  m_pSpin = new CSpin();
  m_pEdit = new ReportNumEdit();
  m_clrBrush = RGB( 255, 255, 255 );
} // constructor


/**
 * destructor
 */
ReportSpinEdit::~ReportSpinEdit ()
{
  delete m_pSpin;
  delete m_pEdit;
} // destructor


/**
 * create window validation
 * @param cs   parameters
 * @return     success/failure
 */
int ReportSpinEdit::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.dwExStyle |= WS_EX_CLIENTEDGE;

  return CWnd::PreCreateWindow(cs);
} // PreCreateWindow()


/**
 * create routine
 * @param lpcs   parameters
 * @return       success/failure
 */
int ReportSpinEdit::OnCreate (LPCREATESTRUCT lpcs)
{
  CWnd::OnCreate(lpcs);
  int4 cx = lpcs->cx;
  int4 cy = lpcs->cy;
  int4 x = lpcs->x;
  int4 y = lpcs->y;

  int4 v = m_pEdit->Create(WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE| ES_AUTOHSCROLL |ES_RIGHT,CRect(x,y,cx-15,cy),this,1);
  m_pSpin->Create(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|UDS_ARROWKEYS,CRect(x+cx,y,15,y+cy),this,2);
  m_pSpin->SetBuddy(m_pEdit);

  m_bkBrush.CreateSolidBrush(m_clrBrush);
  return 0;
} // OnCreate()


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void ReportSpinEdit::OnSize (UINT uType, int iCX, int iCY)
{
  CWnd::OnSize(uType,iCX,iCY);
  if ( iCX < 60 )
  {
    m_pEdit->MoveWindow(0,0,iCX,iCY);
    m_pSpin->MoveWindow(iCX-15,0,0,iCY);
  }
  else 
  {
    m_pEdit->MoveWindow(0,0,iCX-15,iCY);
    m_pSpin->MoveWindow(iCX-15,0,15,iCY);
  }
} // OnSize()


/**
 * WM_KEYDOWN handler
 *
 * @param uChar     Key code identifier.
 * @param uRepCnt   Key repeat count.
 * @param uFlags    Specifies the scan code, key-transition code, previous key state, and context code.
 */
void ReportSpinEdit::OnKeyDown (UINT uChar, UINT uRepCnt, UINT uFlags)
{
  switch ( uChar )
  {
  case VK_PRIOR :	 
    m_pSpin->Change(10);
    break;
  case VK_NEXT :	
    m_pSpin->Change(-10);
    break;
  default :
    CWnd::OnKeyDown(uChar,uRepCnt,uFlags);
  }
} // OnKeyDown()


/**
 * get pointer to spin control
 * @return   spin control pointer
 */
CSpin* ReportSpinEdit::GetSpinner () const
{
  return m_pSpin;
} // GetSpinner()


/**
 * get pointer to edit control
 * @return   edit control pointer
 */
ReportNumEdit* ReportSpinEdit::GetEdit () const
{
  return m_pEdit;
} // GetEdit()


/**
 * Set focus to child window
 * 
 * @param pOldWnd   Previous focus window
 */
void ReportSpinEdit::OnSetFocus (CWnd* pOldWnd)
{
  m_pEdit->SetFocus();
} // OnSetFocus()


/**
 * Override this function to filter window messages before they are dispatched to the Windows functions .
 *
 * @param pMsg   message to translate
 * @return       Nonzero if the message was fully processed and should not be processed further
 */
BOOL ReportSpinEdit::PreTranslateMessage (MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN)
  {
    if (pMsg->wParam == VK_PRIOR  || pMsg->wParam == VK_NEXT ) 
    {
      OnKeyDown(pMsg->wParam , LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
      return TRUE;
    }
  }
  return FALSE;
} // PreTranslateMessage()


/**
 * set the max number of allowed digits
 * @param iDigCount   max num of allowed digits
 */
void ReportSpinEdit::SetDigits(const int4 iDigCount)
{
  ASSERT(m_pEdit);
  m_pEdit->SetDigits(iDigCount);
} // SetDigits()


/**
 * set the value range
 * @param fMin    minimum value
 * @param fMax    maximum value
 * @param bWrap   true to wrap values on change
 */
void ReportSpinEdit::SetRange (const float4 fMin, const float4 fMax, const bool bWrap)
{
  ASSERT(m_pEdit);
  m_pEdit->SetRange(fMin, fMax);
  m_pEdit->SetWrap(bWrap);
  m_pSpin->SetRange((int4)fMin,(int4)fMax);
} // SetRange()


/**
 * set the spin increment step
 * @param fDX   step size
 */
void ReportSpinEdit::SetStepSize (const float4 fDX)
{
  ASSERT(m_pSpin);
  m_pSpin->SetDelta(fDX);
} // SetStepSize()


/**
 * set font
 * @param pFont     the font
 * @param bRedraw   whether to redraw after setting font
 */
void ReportSpinEdit::SetFont (CFont* pFont, BOOL bRedraw)
{
  CWnd::SetFont(pFont, bRedraw);
  m_pEdit->SetFont(pFont,bRedraw);
} // SetFont()


/**
 * Colors dialog's controls.
 *
 * @param pDC         Device context for painting controls.
 * @param pWnd        Handle to the control being drawn.
 * @param uCtlColor   Identifies the type of control being drawn.
 * @return            Brush to paint control.
 */
HBRUSH ReportSpinEdit::OnCtlColor (CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hBrush;
  hBrush = CWnd::OnCtlColor (pDC,pWnd,nCtlColor);
	
  if (pWnd == m_pEdit)
  {
    pDC->SetBkColor(m_clrBrush);
    hBrush = m_bkBrush;
  }

  return hBrush;
} // OnCtlColor()


/**
 * set text colour
 *
 * @param r   red
 * @param b   blue
 * @param g   green
 */
void ReportSpinEdit::SetTextColor (const int4 r, const int4 b, const int4 g)
{
  m_clrText = RGB(r,g,b);
} // SetTextColor()


/**
 * get the current value
 *
 * @return   the value
 */
float ReportSpinEdit::GetValue () const
{
  return m_pEdit->GetValue();
} // GetValue()


/**
 * set the current value
 *
 * @param fVal   the value
 */
void ReportSpinEdit::SetValue (const float4 fVal)
{
  m_pEdit->SetValue(fVal);
} // SetValue()


/**
 * set text colour
 *
 * @param clr   new colour
 */
void ReportSpinEdit::SetColor(const COLORREF clr)
{
  m_clrBrush = clr; 
  m_bkBrush.DeleteObject();
  m_bkBrush.CreateSolidBrush(m_clrBrush);
  if(m_pEdit)
    m_pEdit->SetColor(clr);
} // SetColor()


// Revision History:
// $Log: ReportSpinEdit.C,v $
// Revision 1.5  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.4  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.3  2006/06/01 20:22:52  frank
// updated to visual studio 2005
//
// Revision 1.2.2.1  2006/05/09 13:51:51  geconomos
// initialization of clear color
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// Revision 1.7  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.6  2001/03/30 18:13:20  geconomos
// Partially fixed bug with reportspinedit control
//
// Revision 1.5  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:48   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:40   ingmar
// Initial revision.
// Revision 1.7  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.6  2001/03/30 18:13:20  geconomos
// Partially fixed bug with reportspinedit control
//
// Revision 1.5  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportSpinEdit.C,v 1.5 2007/03/09 23:38:54 jmeade Exp $
// $Id: ReportSpinEdit.C,v 1.5 2007/03/09 23:38:54 jmeade Exp $
