// $Id: GradientCtrl.C,v 1.4.2.1 2007/11/02 03:07:16 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GradientCtrl.C
// Description: 
// Owner: Jeff Meade [jmeade@viatronix.com]

#include "stdafx.h"
#include "GradientCtrl.h"
#include "GradientCtrlImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GRADIENTCTRL_CLASSNAME _T("Viatronix.v3D.Colon.Controls.CGradientCtrl")

using namespace WinUtils;

/////////////////////////////////////////////////////////////////////////////
// GradientCtrl

GradientCtrl::GradientCtrl() : m_iWidth(GCW_AUTO), m_iSelected(GradientList::NONE), m_bShowToolTip(true), m_eOrientation(AUTO)
{
  RegisterWindowClass();
  m_pGradImpl = new GradientCtrlImpl(*this);
  m_sToolTipFormat = _T("&SELPOS\nPosition: &SELPOS Colour: R &R G &G B &B\nColour: R &R G &G B &B\nColour: R &R G &G B &B\nDouble Click to Add a New Peg");
}

GradientCtrl::~GradientCtrl()
{
  delete m_pGradImpl;
}

BEGIN_MESSAGE_MAP(GradientCtrl, CWnd)
  //{{AFX_MSG_MAP(GradientCtrl)
  ON_WM_PAINT()
  ON_WM_ERASEBKGND()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_KEYDOWN()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_GETDLGCODE()
  ON_WM_KILLFOCUS()
  ON_WM_SETCURSOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL GradientCtrl::RegisterWindowClass()
{
  WNDCLASS wndcls;
  HINSTANCE hInst = AfxGetInstanceHandle();
  HBRUSH background;
  background = ::CreateSolidBrush(0x00FFFFFF);

  if (!(::GetClassInfo(hInst, GRADIENTCTRL_CLASSNAME, &wndcls)))
  {
    // otherwise we need to register a new class
    wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc      = ::DefWindowProc;
    wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
    wndcls.hInstance        = hInst;
    wndcls.hIcon            = NULL;
    wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.hbrBackground    = background;
    wndcls.lpszMenuName     = NULL;
    wndcls.lpszClassName    = GRADIENTCTRL_CLASSNAME;

    if (!AfxRegisterClass(&wndcls))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// GradientCtrl message handlers

BOOL GradientCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
  return CWnd::CreateEx(WS_EX_CLIENTEDGE, GRADIENTCTRL_CLASSNAME, _T(""),
    0x50010000, rect, pParentWnd, nID);
}

void GradientCtrl::OnPaint()
{
  CPaintDC dc(this);

  m_pGradImpl->Draw(&dc, IsWindowEnabled()==TRUE);

  //----- Refresh -----//
  if (m_iSelected > m_gradient.GetPegCount())
    m_iSelected = GradientList::NONE;

  if(m_bShowToolTip)
    m_pGradImpl->SynchronizeTooltips();
}

BOOL GradientCtrl::OnEraseBkgnd(CDC* pDC)
{
  CRgn rgnPeg, rgnClient, rgnGradient, rgnErase;
  CRect rectClient;
  bool bLeftdown = m_pGradImpl->m_bLeftDownSide == TRUE;
  bool bRightup = m_pGradImpl->m_bRightUpSide == TRUE;

  //----- The area of the window -----//
  GetClientRect(&rectClient);
  rgnClient.CreateRectRgn(0, 0, rectClient.right, rectClient.bottom);

  //----- The area of the gradient -----//
  if (m_pGradImpl->IsVertical())
    rgnGradient.CreateRectRgn(bLeftdown ? 23 : 4, 4, m_pGradImpl->GetDrawWidth() - (bRightup ? 23 : 3), rectClient.bottom-4);
  else
    rgnGradient.CreateRectRgn(4, rectClient.bottom-m_pGradImpl->GetDrawWidth() + (bRightup ? 23 : 4), 
                              rectClient.right-4, rectClient.bottom- (bLeftdown ? 23 : 4));

  //----- The area of the pegs -----//
  GetPegRgn(&rgnPeg);

  //----- Create a region outside all of these -----//
  rgnErase.CreateRectRgn(0, 0, 0, 0); // Create a dummy rgn
  rgnErase.CombineRgn(&rgnClient, &rgnPeg, RGN_DIFF);
  rgnErase.CombineRgn(&rgnErase, &rgnGradient, RGN_DIFF);

  //----- Fill the result in -----//
  CBrush brushBkgd(GetSysColor((IsWindowEnabled()==TRUE) ? COLOR_WINDOW : COLOR_INACTIVEBORDER));
  pDC->FillRgn(&rgnErase, &brushBkgd);

  CBrush brushBorder(COLORREF(0x0));
  pDC->FrameRect(rectClient, &brushBorder);

  return 1;
}

void GradientCtrl::PreSubclassWindow() 
{
  CWnd::PreSubclassWindow();
}

int GradientCtrl::GetPegAtPoint(const CPoint& point)
{
  CRect pegrect;

  //----- Check if the user is selecting a marker peg -----//
  for (int i = 0; i < m_gradient.GetPegCount(); i++)
  {
    if (m_pGradImpl->m_bLeftDownSide)
    {
      m_pGradImpl->GetPegRect(i, &pegrect, false);
      if (pegrect.PtInRect(point))
      {
        return i;
      }
    }

    if (m_pGradImpl->m_bRightUpSide)
    {
      m_pGradImpl->GetPegRect(i, &pegrect, true);
      if(pegrect.PtInRect(point))
      {
        return i;
      }
    }
  }

  //----- Check if the user is trying to select the first or last peg -----//
  if (m_pGradImpl->m_bRightUpSide)
  {
    m_pGradImpl->GetPegRect(GradientList::STARTPEG, &pegrect, true);
    if (pegrect.PtInRect(point))
    {
      return GradientList::STARTPEG;
    }

    m_pGradImpl->GetPegRect(GradientList::ENDPEG, &pegrect, true);
    if (pegrect.PtInRect(point))
    {
      return GradientList::ENDPEG;
    }
  }

  if (m_pGradImpl->m_bLeftDownSide)
  {
    m_pGradImpl->GetPegRect(GradientList::STARTPEG, &pegrect, false);
    if (pegrect.PtInRect(point))
    {
      return GradientList::STARTPEG;
    }

    m_pGradImpl->GetPegRect(GradientList::ENDPEG, &pegrect, false);
    if (pegrect.PtInRect(point))
    {
      return GradientList::ENDPEG;
    }
  }

  return GradientList::NONE;
}

void GradientCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CClientDC dc(this);
  CRect pegrect;

  m_iLastPos = -1;

  //----- Just in case the user starts dragging -----//
  SetFocus();
  SetCapture();
  m_ptMouseDown = point;

  int iSelected = GetPegAtPoint(point);
  SetSelIndex(iSelected);

  if (iSelected == GradientList::NONE)
  {
    m_pGradImpl->DrawSelPeg(&dc, m_iSelected);
    m_iSelected = GradientList::NONE;
    m_iLastPos = -1;
  }

  CWnd *pParent = GetParent();

  if (pParent)
  {
    PegNMHDR nmhdr;

    nmhdr.nmhdr.code = GC_SELCHANGE;
    nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
    nmhdr.nmhdr.idFrom = GetDlgCtrlID();
    if(m_iSelected != GradientList::NONE)
      nmhdr.peg = m_gradient.GetPeg(m_iSelected);
    else
      nmhdr.peg = m_pGradImpl->m_pegNull;
    nmhdr.iIndex = m_iSelected;
    pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));
  }

  SetFocus();

  CWnd::OnLButtonDown(nFlags, point);
}

void GradientCtrl::OnMouseMove(UINT uFlags, CPoint point) 
{
  bool bSendPegMoveNotify = false;

  if (uFlags & MK_LBUTTON)
  {
    if (m_iSelected > -1)
    {
      CRgn oldrgn, newrgn, erasergn;
      CBrush brush;
      POINT tippoint;
      CString tiptext;
      bool vertical = m_pGradImpl->IsVertical();
      int selpegpos;

      //----- Prepare -----//
      CClientDC dc(this);

      //----- Checks to see if the mouse is far enough away to "drop out" -----//
      if(vertical)
      {
        if(max(point.x - m_ptMouseDown.x, m_ptMouseDown.x - point.x) <= 200)
          selpegpos = point.y;
        else
          selpegpos = m_ptMouseDown.y;
      }
      else
      {
        if(max(point.y - m_ptMouseDown.y, m_ptMouseDown.y - point.y) <= 200)
          selpegpos = point.x;
        else
          selpegpos = m_ptMouseDown.x;
      }


      //----- A brush for thebackground -----//
      brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));

      //----- Get the orginal erase area -----//
      GetPegRgn(&oldrgn);

      //----- Continue -----//
      Peg peg = m_gradient.GetPeg(m_iSelected);
      peg.SetPosition(m_pGradImpl->PosFromPoint(selpegpos));
      //"The id of the selection may change"//
      m_iSelected = m_gradient.SetPeg(m_iSelected, peg);

      //----- Draw the peg -----//
      m_pGradImpl->DrawPegs(&dc);
      m_pGradImpl->DrawEndPegs(&dc);
      m_pGradImpl->DrawSelPeg(&dc, m_iSelected);
      m_pGradImpl->DrawGradient(&dc);
      GetPegRgn(&newrgn);

      erasergn.CreateRectRgn(0,0,0,0); //Dummy rgn
      erasergn.CombineRgn(&oldrgn, &newrgn, RGN_DIFF);

      dc.FillRgn(&erasergn, &brush);

      m_iLastPos = selpegpos;

      //----- Free up stuff -----//
      oldrgn.DeleteObject();
      newrgn.DeleteObject();
      erasergn.DeleteObject();

      //----- Show tooltip -----//
      if (m_bShowToolTip)
      {
        tiptext = m_pGradImpl->ExtractLine(m_sToolTipFormat, 0);
        m_pGradImpl->ParseToolTipLine(tiptext, peg);

        if(m_pGradImpl->m_hToolTip == NULL)
        {
          if(m_pGradImpl->IsVertical())
          {
            tippoint.y = point.y - 8;
            tippoint.x = m_pGradImpl->GetDrawWidth() + m_pGradImpl->GetPegIndent(GetSelIndex())*11 - 7;
          }
          else
          {
            CRect clientrect;
            GetClientRect(&clientrect);
            tippoint.y = clientrect.bottom - m_pGradImpl->GetDrawWidth() - m_pGradImpl->GetPegIndent(GetSelIndex())*11 - 7;
            tippoint.x = point.x - 8;
          }

          ClientToScreen(&tippoint);

          m_pGradImpl->ShowTooltip(tippoint, tiptext);
        }
        else
          m_pGradImpl->SetTooltipText(tiptext);
      }

      bSendPegMoveNotify = true;

    } // end if (m_iSelected > -1)
  } // end if (uFlags & MK_LBUTTON)
  else
  {
  }

  //----- Send parent messages -----//
  CWnd *pParent = GetParent();
  if (pParent)
  {
    PegNMHDR nmhdr;
    nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
    nmhdr.nmhdr.idFrom = GetDlgCtrlID();
    if (m_iSelected != GradientList::NONE)
      nmhdr.peg = GetSelPeg();
    else
      nmhdr.peg = m_pGradImpl->m_pegNull;
    nmhdr.iIndex = m_iSelected;

    if (bSendPegMoveNotify)
    {
      nmhdr.nmhdr.code = GC_PEGMOVE;
      pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));
    }

    if (bSendPegMoveNotify)
    {
      nmhdr.nmhdr.code = GC_CHANGE;
      pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&(nmhdr.nmhdr)));
    }

    nmhdr.nmhdr.code = WM_MOUSEMOVE;
    pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&(nmhdr.nmhdr)));
  }

  CWnd::OnMouseMove(uFlags, point);
}

void GradientCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
  if (m_iSelected >= -4 && m_iSelected != GradientList::NONE)
  {
    bool vertical = m_pGradImpl->IsVertical();
    CRect clientrect;
    GetClientRect(&clientrect);
    //int selpegpos = vertical ? point.y-5 : point.x-5;

    //----- Update the pegs -----//
    CClientDC dc(this);
    if(vertical)
      dc.FillSolidRect(CRect(m_pGradImpl->GetDrawWidth(), 0, clientrect.right,
      clientrect.bottom), 0x00FFFFFF); //Erase the old ones
    else
      dc.FillSolidRect(CRect(0, 0, clientrect.right,
      clientrect.bottom - m_pGradImpl->GetDrawWidth()), 0x00FFFFFF); //Erase the old ones

    m_pGradImpl->DrawPegs(&dc);
    m_pGradImpl->DrawEndPegs(&dc); //Draw the new ones
    m_pGradImpl->DrawSelPeg(&dc, m_iSelected);

    m_pGradImpl->DestroyTooltip();

    CWnd *pParent = GetParent();
    if (pParent)
    {
      PegNMHDR nmhdr;
      nmhdr.nmhdr.code = GC_PEGMOVED;
      nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
      nmhdr.nmhdr.idFrom = GetDlgCtrlID();
      if (m_iSelected != GradientList::NONE)
        nmhdr.peg = m_gradient.GetPeg(m_iSelected);
      else
        nmhdr.peg = m_pGradImpl->m_pegNull;
      nmhdr.iIndex = m_iSelected;
      pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));

      nmhdr.nmhdr.code = GC_CHANGE;
      pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&(nmhdr.nmhdr)));
    }
  }

  ReleaseCapture();

  CWnd::OnLButtonUp(nFlags, point);
}

void GradientCtrl::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  switch(uChar)
  {
  case VK_TAB:
    if(m_iSelected > -4 && m_iSelected < m_gradient.GetPegCount())
    {
      CClientDC dc(this);
      m_pGradImpl->DrawSelPeg(&dc, m_iSelected);
      switch(m_iSelected)
      {
      case GradientList::NONE:
        m_iSelected = GradientList::STARTPEG;
        break;
      case GradientList::STARTPEG:
        if(m_gradient.GetPegCount() > 0)
          m_iSelected = 0;
        else m_iSelected = GradientList::ENDPEG;
        break;
      case GradientList::ENDPEG:
        m_iSelected = GradientList::STARTPEG;
        break;
      default:
        if(m_iSelected+1 >= m_gradient.GetPegCount())
          m_iSelected = GradientList::ENDPEG;
        else m_iSelected++;
      }

      m_pGradImpl->DrawSelPeg(&dc, m_iSelected);
    }
    break;

  case VK_UP:
  case VK_LEFT:
    if(m_iSelected >= 0 && m_iSelected < m_gradient.GetPegCount())
    {
      Peg selpeg = GetSelPeg();
      selpeg.m_fPosition -= 0.025f;
      //Make sure that the m_fPosition does not stray bellow zero
      selpeg.m_fPosition = (selpeg.m_fPosition <= 1.0f) ?  selpeg.m_fPosition : 1.0f;
      MoveSelected(selpeg.m_fPosition, true);

      //Send parent messages
      SendBasicNotification(GC_PEGMOVED, selpeg, m_iSelected);
    }
    break;

  case VK_DOWN:
  case VK_RIGHT:
    if(m_iSelected >= 0 && m_iSelected < m_gradient.GetPegCount())
    {
      Peg selpeg = GetSelPeg();
      selpeg.m_fPosition += 0.025f;
      //Make sure that the m_fPosition does not stray above 1
      selpeg.m_fPosition = (selpeg.m_fPosition <= 1.0f) ?  selpeg.m_fPosition : 1.0f;
      MoveSelected(selpeg.m_fPosition, true);

      //Send parent messages
      SendBasicNotification(GC_PEGMOVED, selpeg, m_iSelected);
    }

    break;

  case VK_HOME:
    if(m_iSelected >= 0 && m_iSelected < m_gradient.GetPegCount())
    {
      Peg selpeg = GetSelPeg();
      selpeg.m_fPosition = 0.0f;
      MoveSelected(selpeg.m_fPosition, true);
      //Send parent messages
      SendBasicNotification(GC_PEGMOVED, selpeg, m_iSelected);
    }

    break;

  case VK_END:
    if(m_iSelected >= 0 && m_iSelected < m_gradient.GetPegCount())
    {
      Peg selpeg = GetSelPeg();
      selpeg.m_fPosition = 1.0f;
      MoveSelected(selpeg.m_fPosition, true);
      //Send parent messages
      SendBasicNotification(GC_PEGMOVED, selpeg, m_iSelected);
    }

    break;

  case VK_DELETE:
  case VK_BACK:
    if (IsEnabledKeyboardEdit() && (m_iSelected >= 0) && (m_iSelected < m_gradient.GetPegCount()))
      DeleteSelected(TRUE);

    break;
  case VK_PRIOR: // Shift the peg up a big jump
    if(m_iSelected >= 0 && m_iSelected < m_gradient.GetPegCount())
    {
      Peg selpeg = GetSelPeg();
      selpeg.m_fPosition -= 0.1f;
      //Make sure that the m_fPosition does not stray bellow zero
      selpeg.m_fPosition = (selpeg.m_fPosition >= 0.0f) ?  selpeg.m_fPosition : 0.0f;
      MoveSelected(selpeg.m_fPosition, true);

      //Send parent messages
      SendBasicNotification(GC_PEGMOVED, selpeg, m_iSelected);
    }

    break;

  case VK_NEXT:
    if(m_iSelected >= 0 && m_iSelected < m_gradient.GetPegCount())
    {
      Peg selpeg = GetSelPeg();
      selpeg.m_fPosition += 0.1f;
      //Make sure that the m_fPosition does not stray above 1
      selpeg.m_fPosition = (selpeg.m_fPosition <= 1.0f) ?  selpeg.m_fPosition : 1.0f;
      MoveSelected(selpeg.m_fPosition, true);

      //Send parent messages
      SendBasicNotification(GC_PEGMOVED, selpeg, m_iSelected);
    }
    break;

  case VK_RETURN:
  case VK_SPACE:
    if (IsEnabledKeyboardEdit() && (m_iSelected > -4) && (m_iSelected < m_gradient.GetPegCount())
      && (m_iSelected != GradientList::NONE))
    {
      PegNMHDR nmhdr;

      nmhdr.nmhdr.code = GC_EDITPEG;
      nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
      nmhdr.nmhdr.idFrom = GetDlgCtrlID();
      nmhdr.peg = GetSelPeg();
      nmhdr.iIndex = m_iSelected;
      GetParent()->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));
    }

    break;

  case VK_INSERT:
    if (IsEnabledKeyboardEdit() && (m_iSelected > -4) && (m_iSelected < m_gradient.GetPegCount())
      && (m_iSelected != GradientList::NONE))
    {
      PegCreateNMHDR nmhdr;
      Peg peg = GetSelPeg();

      nmhdr.nmhdr.code = GC_CREATEPEG;
      nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
      nmhdr.nmhdr.idFrom = GetDlgCtrlID();
      nmhdr.fPosition = peg.m_fPosition;
      nmhdr.colour = peg.m_colour;
      GetParent()->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));
    }
    break;
  }

  CWnd::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()

void GradientCtrl::DeleteSelected(bool bUpdate) 
{
  Peg peg;
  int oldsel;
  float oldpos;

  if(m_iSelected == GradientList::NONE)
    return;
  if(m_iSelected <= -4)
    return;
  if(m_iSelected >= m_gradient.GetPegCount())
    return;

  oldsel = m_iSelected;
  peg = m_gradient.GetPeg(m_iSelected);
  oldpos = peg.m_fPosition;

  m_gradient.RemovePeg(m_iSelected);

  //Select the previous peg
  if(m_iSelected == GradientList::STARTPEG) m_iSelected = GradientList::STARTPEG;
  else m_iSelected--;
  if(m_iSelected < 0) m_iSelected = GradientList::STARTPEG;

  if(bUpdate)
    Invalidate();

  CWnd *pParent = GetParent();

  if (pParent)
  { 
    PegNMHDR nmhdr, nmhdr2, nmhdr3;

    nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
    nmhdr.nmhdr.idFrom = GetDlgCtrlID();
    nmhdr.nmhdr.code = GC_PEGREMOVED;
    if (m_iSelected != GradientList::NONE)
      nmhdr.peg = peg;
    else
      nmhdr.peg = m_pGradImpl->m_pegNull;
    nmhdr.iIndex = oldsel;
    pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));

    nmhdr2 = nmhdr;
    nmhdr2.nmhdr.code = GC_SELCHANGE;
    if (m_iSelected != GradientList::NONE)
      nmhdr2.peg = m_gradient.GetPeg(m_iSelected);
    else
      nmhdr2.peg = m_pGradImpl->m_pegNull;
    nmhdr2.iIndex = m_iSelected;
    pParent->SendMessage(WM_NOTIFY, nmhdr2.nmhdr.idFrom, (DWORD)(&nmhdr2));

    nmhdr3 = nmhdr2;
    nmhdr3.nmhdr.code = GC_CHANGE;
    nmhdr3.peg = m_pGradImpl->m_pegNull;
    nmhdr3.iIndex = GradientList::NONE;
    pParent->SendMessage(WM_NOTIFY, nmhdr3.nmhdr.idFrom, (DWORD)(&nmhdr3));
  }
} // DeleteSelected()

int GradientCtrl::SetSelIndex(int iSel)
{
  int oldsel = m_iSelected;
  ASSERT(iSel > -4); //Nothing smaller than -4 ok?
  ASSERT(iSel != GradientList::BACKGROUND); //You can't select the background
  ASSERT(iSel < m_gradient.GetPegCount()); //Make sure things are in range

  m_iSelected = iSel;

  return oldsel;
}

int GradientCtrl::MoveSelected(float newpos, bool bUpdate)
{
  Peg peg;

  if (m_iSelected < 0)
  {
    return GradientList::NONE;
  }

  peg = GetSelPeg();
  peg.SetPosition(newpos);
  m_iSelected = m_gradient.SetPeg(m_iSelected, peg);

  if (bUpdate)
  {
    Invalidate();
  }

  return m_iSelected;
}

COLORREF GradientCtrl::SetColourSelected(COLORREF crNewColour, bool bUpdate)
{
  Peg peg;

  if(m_iSelected < 0)
    return 0x00000000;

  peg = GetSelPeg();
  peg.SetColor(crNewColour);
  m_gradient.SetPeg(m_iSelected, peg);

  if(bUpdate) Invalidate();

  return peg.m_colour;
}

void GradientCtrl::OnLButtonDblClk(UINT uFlags, CPoint point) 
{
  CWnd *pParent = GetParent();
  float pos;
  CRect clientrect;
  Peg peg;
  CClientDC dc(this);
  int iDrawWidth = m_pGradImpl->GetDrawWidth();

  if(m_pGradImpl->IsVertical())
    pos = m_pGradImpl->PosFromPoint(point.y);
  else
    pos = m_pGradImpl->PosFromPoint(point.x);

  int iSelected = GetPegAtPoint(point);
  m_pGradImpl->DrawSelPeg(&dc, m_iSelected); //Erase the last m_iSelected peg
  SetSelIndex(iSelected);
  m_pGradImpl->DrawSelPeg(&dc, m_iSelected); //Erase the last m_iSelected peg

  if (pParent)
  {
    if (iSelected != GradientList::NONE)
    {
      PegNMHDR nmhdr;

      nmhdr.nmhdr.code = GC_EDITPEG;
      nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
      nmhdr.nmhdr.idFrom = GetDlgCtrlID();
      if(m_iSelected != GradientList::NONE) nmhdr.peg = peg;
      else nmhdr.peg = m_pGradImpl->m_pegNull;
      nmhdr.iIndex = m_iSelected;
      pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));
    }
    else
    {
      PegCreateNMHDR nmhdr;

      nmhdr.nmhdr.code = GC_CREATEPEG;
      nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
      nmhdr.nmhdr.idFrom = GetDlgCtrlID();
      nmhdr.fPosition = pos;
      nmhdr.colour = m_gradient.ColourFromPosition(pos);
      pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));
    }
  }

  CWnd::OnLButtonDblClk(uFlags, point);
}

void GradientCtrl::GetPegRgn(CRgn *rgn)
{
  CRect clientrect;
  POINT pegpoint;
  int j;
  int iDrawWidth = m_pGradImpl->GetDrawWidth();
  bool bVertical = m_pGradImpl->IsVertical();
  int iColcount = (int)m_pGradImpl->m_bLeftDownSide+(int)m_pGradImpl->m_bRightUpSide;

  if (iColcount == 0)
  {
    rgn->CreateRectRgn(0,0,0,0); // A dummy
    return;
  }

  //----- Carefully do some preparation -----//
  int iPegCount = m_gradient.GetPegCount();
  POINT *points = new POINT[iPegCount*3*iColcount + 8*iColcount];
  ASSERT(points);
  int *polycounts = new int[iPegCount*iColcount + 2*iColcount];
  ASSERT(polycounts);

  GetClientRect(&clientrect);

  //----- End pegs -----//
  j = 0;
  if (bVertical)
  {
    if (m_pGradImpl->m_bRightUpSide)
    {
      polycounts[j/4]=4;
      points[j].x = iDrawWidth-15, points[j].y = 4;
      j++;
      points[j].x = iDrawWidth-15, points[j].y = 11;
      j++;
      points[j].x = iDrawWidth-8,  points[j].y = 11;
      j++;
      points[j].x = iDrawWidth-8,  points[j].y = 4;
      j++;

      polycounts[j/4]=4;
      points[j].x = iDrawWidth-15, points[j].y = clientrect.bottom-4;
      j++;
      points[j].x = iDrawWidth-15, points[j].y = clientrect.bottom-11;
      j++;
      points[j].x = iDrawWidth-8,  points[j].y = clientrect.bottom-11;
      j++;
      points[j].x = iDrawWidth-8,  points[j].y = clientrect.bottom-4;
      j++;
    }

    if (m_pGradImpl->m_bLeftDownSide)
    {
      polycounts[j/4]=4;
      points[j].x = 8,  points[j].y = 4;
      j++;
      points[j].x = 8,  points[j].y = 11;
      j++;      
      points[j].x = 15, points[j].y = 11;
      j++;      
      points[j].x = 15, points[j].y = 4;
      j++;      

      polycounts[j/4]=4;
      points[j].x = 8,  points[j].y = clientrect.bottom-4;
      j++;  
      points[j].x = 8,  points[j].y = clientrect.bottom-11;
      j++;  
      points[j].x = 15, points[j].y = clientrect.bottom-11;
      j++;      
      points[j].x = 15, points[j].y = clientrect.bottom-4;
      j++;      
    }
  }
  else
  {
    if (m_pGradImpl->m_bRightUpSide)
    {
      polycounts[j/4]=4;
      points[j].x = 4,  points[j].y = clientrect.bottom-iDrawWidth+8;
      j++;
      points[j].x = 11, points[j].y = clientrect.bottom-iDrawWidth+8;
      j++;
      points[j].x = 11, points[j].y = clientrect.bottom-iDrawWidth+15;
      j++;
      points[j].x = 4,  points[j].y = clientrect.bottom-iDrawWidth+15;
      j++;

      polycounts[j/4]=4;
      points[j].x = clientrect.right-4, points[j].y = clientrect.bottom-iDrawWidth+8;
      j++;
      points[j].x = clientrect.right-11,  points[j].y = clientrect.bottom-iDrawWidth+8;
      j++;
      points[j].x = clientrect.right-11,  points[j].y = clientrect.bottom-iDrawWidth+15;
      j++;
      points[j].x = clientrect.right-4, points[j].y = clientrect.bottom-iDrawWidth+15;
      j++;
    }

    if (m_pGradImpl->m_bLeftDownSide)
    {
      polycounts[j/4]=4;
      points[j].x = 4,  points[j].y = clientrect.bottom-8;
      j++;
      points[j].x = 11, points[j].y = clientrect.bottom-8;
      j++;
      points[j].x = 11, points[j].y = clientrect.bottom-15;
      j++;
      points[j].x = 4,  points[j].y = clientrect.bottom-15;
      j++;

      polycounts[j/4]=4;
      points[j].x = clientrect.right-4, points[j].y = clientrect.bottom-8;
      j++;
      points[j].x = clientrect.right-11,  points[j].y = clientrect.bottom-8;
      j++;
      points[j].x = clientrect.right-11,  points[j].y = clientrect.bottom-15;
      j++;
      points[j].x = clientrect.right-4, points[j].y = clientrect.bottom-15;
      j++;
    }
  }

  j=0;

  //----- Main pegs -----//
  for (int i = 0; i < iPegCount; i++)
  {
    if (bVertical)
    {
      pegpoint.y = m_pGradImpl->PointFromPos(m_gradient.GetPeg(i).m_fPosition);

      if (m_pGradImpl->m_bLeftDownSide)
      {
        pegpoint.x = 23 - m_pGradImpl->GetPegIndent(i)*11;

        points[j*3+8*iColcount].x = pegpoint.x;
        points[j*3+8*iColcount].y = pegpoint.y+1;
        points[j*3+8*iColcount+1].x = pegpoint.x-10;
        points[j*3+8*iColcount+1].y = pegpoint.y-4;
        points[j*3+8*iColcount+2].x = pegpoint.x-10;
        points[j*3+8*iColcount+2].y = pegpoint.y+6;
        polycounts[j + 2*iColcount] = 3;
        j++;
      }

      if (m_pGradImpl->m_bRightUpSide)
      {
        pegpoint.x = m_pGradImpl->GetPegIndent(i)*11 + iDrawWidth - 23;

        points[j*3+8*iColcount].x = pegpoint.x;
        points[j*3+8*iColcount].y = pegpoint.y+1;
        points[j*3+8*iColcount+1].x = pegpoint.x+10;
        points[j*3+8*iColcount+1].y = pegpoint.y-4;
        points[j*3+8*iColcount+2].x = pegpoint.x+10;
        points[j*3+8*iColcount+2].y = pegpoint.y+6;
        polycounts[j + 2*iColcount] = 3;
        j++;
      }
    }
    else
    {
      pegpoint.x = m_pGradImpl->PointFromPos(m_gradient.GetPeg(i).m_fPosition);

      if (m_pGradImpl->m_bLeftDownSide)
      {
        pegpoint.y = clientrect.bottom - 23 + m_pGradImpl->GetPegIndent(i)*11;

        points[j*3+8*iColcount].x = pegpoint.x+1;
        points[j*3+8*iColcount].y = pegpoint.y-1;
        points[j*3+8*iColcount+1].x = pegpoint.x-4;
        points[j*3+8*iColcount+1].y = pegpoint.y+10;
        points[j*3+8*iColcount+2].x = pegpoint.x+7;
        points[j*3+8*iColcount+2].y = pegpoint.y+10;
        polycounts[j + 2*iColcount] = 3;
        j++;
      }

      if (m_pGradImpl->m_bRightUpSide)
      {
        pegpoint.y = clientrect.bottom - m_pGradImpl->GetPegIndent(i)*11 - iDrawWidth + 22;

        points[j*3+8*iColcount].x = pegpoint.x+1;
        points[j*3+8*iColcount].y = pegpoint.y+1;
        points[j*3+8*iColcount+1].x = pegpoint.x-4;
        points[j*3+8*iColcount+1].y = pegpoint.y-9;
        points[j*3+8*iColcount+2].x = pegpoint.x+6;
        points[j*3+8*iColcount+2].y = pegpoint.y-9;
        polycounts[j + 2*iColcount] = 3;
        j++;
      }
    }
  }

  rgn->CreatePolyPolygonRgn(points, polycounts,
    (iPegCount+2)*iColcount, ALTERNATE);

  delete[] points;
  delete[] polycounts;
}

BOOL GradientCtrl::PreTranslateMessage(MSG* pMsg) 
{
  if (m_pGradImpl->m_ctrlToolTip.GetSafeHwnd() != NULL)
    m_pGradImpl->m_ctrlToolTip.RelayEvent(pMsg);

  return CWnd::PreTranslateMessage(pMsg);
}

void GradientCtrl::ShowTooltips(bool bShow)
{
  m_bShowToolTip = bShow;
  if (m_pGradImpl->m_ctrlToolTip.GetSafeHwnd() != NULL)
    m_pGradImpl->m_ctrlToolTip.Activate(bShow);
  if (m_bShowToolTip)
    m_pGradImpl->SynchronizeTooltips();
}

float GradientCtrl::GetPosAtPoint(const CPoint& pt)
{
  if (m_pGradImpl->IsVertical())
    return m_pGradImpl->PosFromPoint(pt.y);
  else
    return m_pGradImpl->PosFromPoint(pt.x);
}


void GradientCtrl::SetPegSide(bool bSetRightOrUp, bool bEnable)
{
  if (bSetRightOrUp)
    m_pGradImpl->m_bRightUpSide = bEnable;
  else
    m_pGradImpl->m_bLeftDownSide = bEnable;
}

bool GradientCtrl::GetPegSide(bool bRightOrUp) const 
{
  return bRightOrUp ? m_pGradImpl->m_bRightUpSide : m_pGradImpl->m_bLeftDownSide;
}

void GradientCtrl::SetTooltipFormat(CString format)
{
  m_sToolTipFormat = format;
}

CString GradientCtrl::GetTooltipFormat() const
{
  return m_sToolTipFormat;
}

void GradientCtrl::EnableKeyboardEdit(const bool bEnable)
{
  m_bKeyboardEditEnabled = bEnable;
}

bool GradientCtrl::IsEnabledKeyboardEdit() const
{
  return m_bKeyboardEditEnabled;
}


void GradientCtrl::SendBasicNotification(UINT code, Peg peg, int index)
{
  //----- Send parent messages -----//
  CWnd *pParent = GetParent();
  if (pParent)
  {
    PegNMHDR nmhdr;
    nmhdr.nmhdr.code = code;
    nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
    nmhdr.nmhdr.idFrom = GetDlgCtrlID();
    nmhdr.peg = peg;
    nmhdr.iIndex = index;
    pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&nmhdr));

    nmhdr.nmhdr.code = GC_CHANGE;
    pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD)(&(nmhdr.nmhdr)));
  }
}

const Peg GradientCtrl::GetSelPeg() const
{
  if (m_iSelected == GradientList::NONE)
    return m_pGradImpl->m_pegNull;
  else if(m_iSelected >= -3 && m_iSelected < m_gradient.GetPegCount())
    return m_gradient.GetPeg(m_iSelected);
  else
    ASSERT(NULL); //Some kind of stupid selection error?
  return m_pGradImpl->m_pegNull;
}

UINT GradientCtrl::OnGetDlgCode() 
{
  return DLGC_WANTALLKEYS;
}

void GradientCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  CWnd::OnKillFocus(pNewWnd);
}

/**
 * Called to set mouse cursor icon
 * 
 * @param CWnd* pWnd      Pointer to window containing cursor
 * @param UINT uHitTest   Area within dialog where cursor is location (e.g. HT_BORDER)
 * @param UINT uMessage   Mouse message number
 * @return BOOL           TRUE if cursor was set
 */
BOOL GradientCtrl::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage) 
{
  CPoint pt;
  GetCursorPos(&pt);
  ScreenToClient(&pt);
  const int iOverPeg(GetPegAtPoint(pt));
  LPCTSTR cursorName(IDC_ARROW);
  switch (iOverPeg)
  {
    case GradientList::STARTPEG:
    case GradientList::ENDPEG:
    case GradientList::NONE:
      cursorName = IDC_ARROW;
      break;
    default:
      cursorName = IDC_SIZEWE;
      break;
  }

  SetCursor(AfxGetApp()->LoadStandardCursor(cursorName));
  return TRUE;
}

// $Log: GradientCtrl.C,v $
// Revision 1.4.2.1  2007/11/02 03:07:16  jmeade
// Issue 5583: allow for disabled/greyed gradient control.
//
// Revision 1.4  2007/03/14 23:36:14  jmeade
// code standards.
//
// Revision 1.3  2007/03/13 19:52:55  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.4  2004/07/02 00:45:35  jmeade
// Allow parent to handle mouse move msgs; set cursor to indicate hover over pegs.
//
// Revision 1.1.2.3  2004/06/23 18:19:38  jmeade
// It is unnecessary to deconstruct and reconstruct CPeg instances in order to pass as method arguments.
//
// Revision 1.1.2.2  2004/05/05 23:23:28  jmeade
// Incremental update.
//
// Revision 1.1.2.1  2004/04/21 03:44:20  jmeade
// Gradient color edit control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GradientCtrl.C,v 1.4.2.1 2007/11/02 03:07:16 jmeade Exp $
// $Id: GradientCtrl.C,v 1.4.2.1 2007/11/02 03:07:16 jmeade Exp $
