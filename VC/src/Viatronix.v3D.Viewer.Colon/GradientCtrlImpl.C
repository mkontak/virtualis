// $Id: GradientCtrlImpl.C,v 1.4.2.1 2007/11/02 03:07:16 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GradientCtrlImpl.C
// Description: 
// Owner: Jeff Meade [jmeade@viatronix.com]

#include "stdafx.h"
#include "GradientCtrlImpl.h"
#include "GradientCtrl.h"
#include "Gradient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace WinUtils;

const TCHAR ampersandtag[] = _T("&&");
const TCHAR postag[] = _T("&SELPOS");
const TCHAR rtag[] = _T("&R");
const TCHAR gtag[] = _T("&G");
const TCHAR btag[] = _T("&B");
const TCHAR hexrtag[] = _T("&HEXR");
const TCHAR hexgtag[] = _T("&HEXG");
const TCHAR hexbtag[] = _T("&HEXB");
const TCHAR floatrtag[] = _T("&FLOATR");
const TCHAR floatgtag[] = _T("&FLOATG");
const TCHAR floatbtag[] = _T("&FLOATB");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * constructor
 *
 * @param rOwner   owner control
 **/
GradientCtrlImpl::GradientCtrlImpl(GradientCtrl &rOwner) : m_rGradOwner(rOwner), m_hToolTip(NULL), m_iRectCount(0),
m_bRightUpSide(true), m_bLeftDownSide(false)
{
} // constructor


/**
 * destructor
 **/
GradientCtrlImpl::~GradientCtrlImpl()
{
} // destructor


/**
 * draw the gradient control
 *
 * @param pDC   device context
 **/
void GradientCtrlImpl::Draw(CDC *pDC, const bool bEnabled)
{
  CRect clientrect;

  m_rGradOwner.GetClientRect(&clientrect);

  //----- Draw the Palette -----//
  DrawGradient(pDC);

  //----- Draw the marker arrows -----//
  DrawEndPegs(pDC);
  if (bEnabled)
  {
    DrawPegs(pDC); //The order is important - Explanation: The function DrawSelPeg is called in the function DrawPegs so if we then draw the end peg we will be drawing over the inverse rectangle!
    DrawSelPeg(pDC, m_rGradOwner.m_iSelected);
  }

  //----- Draw a box around the palette -----//
  CBrush blackbrush(GetSysColor(COLOR_WINDOWTEXT));
  if (IsVertical())
  {
    pDC->FrameRect(CRect(m_bLeftDownSide ? 23 : 4, 4,
      GetDrawWidth() - (m_bRightUpSide ? 23 : 4),
      clientrect.bottom-4), &blackbrush);
  }
  else
  {
    pDC->FrameRect(CRect(4, clientrect.bottom - GetDrawWidth() + (m_bRightUpSide ? 23 : 4),
      clientrect.right-4, clientrect.bottom-(m_bLeftDownSide?23:4)),
      &blackbrush);
  }

  //clientrect.DeflateRect(1,1,1,1);
  //pDC->DrawFocusRect(clientrect);
} // Draw()


/**
 * draw the gradient colours
 *
 * @param pDC   device context
 **/
void GradientCtrlImpl::DrawGradient(CDC *pDC)
{
  CRect clientrect;
  const bool bVertical(IsVertical());
  const int4 iDrawWidth(GetDrawWidth());
  const int4 iWidth(iDrawWidth - (m_bRightUpSide ? 24 : 5) - (m_bLeftDownSide ? 24 : 5));

  m_rGradOwner.GetClientRect(&clientrect);
  if (clientrect.bottom < 11) return;

  //----- Draw the Gradient -----//
  CBitmap membmp;
  CDC memdc;
  COLORREF colour;
  POINT point;
  const int4 iLength((bVertical ? clientrect.bottom : clientrect.right) - 10);

  RGBTRIPLE *pal = new RGBTRIPLE[iLength], *entry;
  ASSERT(pal);
  if (!pal) return;

  if (bVertical)
    membmp.CreateCompatibleBitmap(pDC, 1, clientrect.bottom-10);
  else
    membmp.CreateCompatibleBitmap(pDC, clientrect.right-10, 1);

  ASSERT(membmp.GetSafeHandle());

  memdc.CreateCompatibleDC(pDC);
  ASSERT(memdc.GetSafeHdc());

  memdc.SelectObject(membmp);
  point = CPoint(0, 0);

  m_rGradOwner.m_gradient.MakeEntries(pal, iLength);

  if (bVertical)
  {
    for (int4 i = 0; i < iLength; i++)
    {
      point.y = i;
      entry = &pal[i];
      colour = RGB(entry->rgbtRed, entry->rgbtGreen, entry->rgbtBlue);
      memdc.SetPixelV(point, colour);
    }
  }
  else
  {
    for(int i = 0; i < iLength; i++)
    {
      point.x = i;
      entry = &pal[i];
      colour = RGB(entry->rgbtRed, entry->rgbtGreen, entry->rgbtBlue);
      memdc.SetPixelV(point, colour);
    }
  }

  if (bVertical)
    pDC->StretchBlt(m_bLeftDownSide ? 24 : 5, 5, iWidth, iLength, &memdc, 0, 0, 1, iLength, SRCCOPY);
  else
    pDC->StretchBlt(5, clientrect.bottom - iDrawWidth + (m_bRightUpSide ? 24 : 5), iLength, iWidth, &memdc, 0, 0, iLength, 1, SRCCOPY);

  //memdc.SelectObject(oldbmp);

  delete[] pal;

} // DrawGradient()


/**
 * draw the gradient pegs
 *
 * @param pDC   device context
 **/
void GradientCtrlImpl::DrawPegs(CDC *pDC)
{
  Peg peg;
  CRect clientrect;

  m_rGradOwner.GetClientRect(&clientrect);

  // No stupid selection
  if (m_rGradOwner.m_iSelected > m_rGradOwner.m_gradient.GetPegCount())
    m_rGradOwner.m_iSelected = -1;

  int4 iPegIndent(0);

  for (int4 i = 0; i < m_rGradOwner.m_gradient.GetPegCount(); i++)
  {
    peg = m_rGradOwner.m_gradient.GetPeg(i);

    if (m_bRightUpSide)
    {
      //Indent if close
      iPegIndent = GetPegIndent(i)*11 + GetDrawWidth() - 23;

      //Obvious really
      if (IsVertical())
        DrawPeg(pDC, CPoint(iPegIndent, PointFromPos(peg.GetPosition())), peg.GetColor(), 0);
      else
        DrawPeg(pDC, CPoint(PointFromPos(peg.GetPosition()), clientrect.bottom - iPegIndent - 1), peg.GetColor(), 1);
    }

    if (m_bLeftDownSide)
    {
      //Indent if close
      iPegIndent = 23 - GetPegIndent(i)*11;

      //Obvious really
      if (IsVertical())
        DrawPeg(pDC, CPoint(iPegIndent, PointFromPos(peg.GetPosition())), peg.GetColor(), 2);
      else
        DrawPeg(pDC, CPoint(PointFromPos(peg.GetPosition()), clientrect.bottom - iPegIndent - 1), peg.GetColor(), 3);
    }
  }
} // DrawPegs()


/**
 * draw a peg as selected
 *
 * @param pDC           device context
 * @param point         location for peg
 * @param iDirection    direction of peg
 **/
void GradientCtrlImpl::DrawSelPeg(CDC *pDC, const CPoint& point, const int4 iDirection)
{
  POINT vPoints[3];

  //Select The Colour
  CPen *pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
  CBrush *pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
  const int4 iOldROP(pDC->SetROP2(R2_NOT));

  //Prepare the coordinates
  switch(iDirection)
  {
  case 0:
    vPoints[0].x = 8+point.x;
    vPoints[0].y = point.y-2;
    vPoints[1].x = 2+point.x;
    vPoints[1].y = point.y+1;
    vPoints[2].x = 8+point.x;
    vPoints[2].y = point.y+4;
    break;
  case 1:
    vPoints[0].x = point.x-2;
    vPoints[0].y = point.y-8;
    vPoints[1].x = point.x+1;
    vPoints[1].y = point.y-2;
    vPoints[2].x = point.x+4;
    vPoints[2].y = point.y-8;
    break;
  case 2:
    vPoints[0].x = point.x-9,  vPoints[0].y = point.y-2;
    vPoints[1].x = point.x-3,  vPoints[1].y = point.y+1;
    vPoints[2].x = point.x-9,  vPoints[2].y = point.y+4;
    break;
  default:
    vPoints[0].x = point.x-2;
    vPoints[0].y = point.y+8;
    vPoints[1].x = point.x+1;
    vPoints[1].y = point.y+2;
    vPoints[2].x = point.x+4;
    vPoints[2].y = point.y+8;
    break;
  }
  pDC->Polygon(vPoints, 3);

  // Restore the old brush and pen
  pDC->SetROP2(iOldROP);
  pDC->SelectObject(pOldBrush);
  pDC->SelectObject(pOldPen);

} // DrawSelPeg()


/**
 * draw the gradient colours
 *
 * @param pDC    device context
 * @param iPeg   the peg
 **/
void GradientCtrlImpl::DrawSelPeg(CDC *pDC, const int4 iPeg)
{
  CBrush *pOldBrush;
  CPen *pOldPen;
  CRect clientrect;
  const int4 iDrawWidth(GetDrawWidth()-23);
  const bool bVertical(IsVertical());

  m_rGradOwner.GetClientRect(&clientrect);

  //"Select objects"//
  pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
  pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
  const int4 iOldROP(pDC->SetROP2(R2_NOT));

  if (iPeg == GradientList::STARTPEG)
  {
    if (m_bRightUpSide)
      if (bVertical) pDC->Rectangle(iDrawWidth+9, 5, iDrawWidth+14, 10);
      else pDC->Rectangle(5, clientrect.bottom-iDrawWidth-9,
        10, clientrect.bottom-iDrawWidth-14);

    if (m_bLeftDownSide)
      if (bVertical) pDC->Rectangle(9, 5, 14, 10);
      else pDC->Rectangle(5, clientrect.bottom-9, 10, clientrect.bottom-14);

      return;
  }

  if (iPeg == GradientList::ENDPEG)
  {
    if (m_bRightUpSide)
      if (bVertical) pDC->Rectangle(iDrawWidth+9, clientrect.bottom-10, iDrawWidth+14, clientrect.bottom-5);
      else pDC->Rectangle(clientrect.right-10, clientrect.bottom-iDrawWidth-9,
        clientrect.right-5, clientrect.bottom-iDrawWidth-14);

    if (m_bLeftDownSide)
      if (bVertical) pDC->Rectangle(9, clientrect.bottom-5, 14, clientrect.bottom-10);
      else pDC->Rectangle(clientrect.right-5, clientrect.bottom-9,
        clientrect.right-10, clientrect.bottom-14);
    return;
  }

  pDC->SelectObject(pOldBrush);
  pDC->SetROP2(iOldROP);
  pDC->SelectObject(pOldPen);

  if (iPeg >= 0 && m_rGradOwner.m_gradient.GetPegCount())
  {
    Peg mypeg = m_rGradOwner.m_gradient.GetPeg(iPeg);
    int4 iPegIndent = GetPegIndent(iPeg)*11;

    if (IsVertical())
    {
      if (m_bRightUpSide)
        DrawSelPeg(pDC, CPoint(iPegIndent + iDrawWidth, PointFromPos(mypeg.GetPosition())), 0);
      if (m_bLeftDownSide)
        DrawSelPeg(pDC, CPoint(23-iPegIndent, PointFromPos(mypeg.GetPosition())), 2);
    }
    else
    {
      if (m_bRightUpSide)
        DrawSelPeg(pDC, CPoint(PointFromPos(mypeg.GetPosition()), clientrect.bottom-iPegIndent-iDrawWidth-1), 1);
      if (m_bLeftDownSide)
        DrawSelPeg(pDC, CPoint(PointFromPos(mypeg.GetPosition()), clientrect.bottom-23+iPegIndent), 3);
    }
  }

  //"Restore old objects"//

} // DrawSelPeg()


/**
 * draw the end pegs
 *
 * @param pDC   device context
 **/
void GradientCtrlImpl::DrawEndPegs(CDC *pDC)
{
  CRect clientrect;
  CPen blackpen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
  const bool bVertical(IsVertical());
  const int4 iDrawWidth(GetDrawWidth());

  m_rGradOwner.GetClientRect(&clientrect);
  CPen *pOldPen = pDC->SelectObject(&blackpen);

  //----- Draw the first marker -----//
  CBrush brush(m_rGradOwner.m_gradient.GetStartPegColour());
  CBrush *pOldBrush = pDC->SelectObject(&brush); //select the brush
  if (m_bRightUpSide)
  {
    if (bVertical)
      pDC->Rectangle(iDrawWidth-15, 4, iDrawWidth-8, 11); //draw the rectangle
    else
      pDC->Rectangle(4, clientrect.bottom-iDrawWidth+15,
      11, clientrect.bottom-iDrawWidth+8); //draw the rectangle
  }

  if (m_bLeftDownSide)
  {
    if (bVertical)
      pDC->Rectangle(8, 4, 15, 11); //draw the rectangle
    else
      pDC->Rectangle(4, clientrect.bottom-8,
      11, clientrect.bottom-15); //draw the rectangle
  }

  pDC->SelectObject(pOldBrush); // restore the old brush
  brush.DeleteObject();

  //----- Draw the second one -----//
  brush.CreateSolidBrush(m_rGradOwner.m_gradient.GetEndPegColour());
  pOldBrush = pDC->SelectObject(&brush); //select the brush

  if (m_bLeftDownSide)
  {
    if(bVertical)
      pDC->Rectangle(8, clientrect.bottom-4, 15,
      clientrect.bottom-11);
    else
      pDC->Rectangle(clientrect.right-4, clientrect.bottom-15,
      clientrect.right-11, clientrect.bottom-8);
  }

  if (m_bRightUpSide)
  {
    if (bVertical)
      pDC->Rectangle(iDrawWidth-15, clientrect.bottom-4, iDrawWidth-8, clientrect.bottom-11);
    else
      pDC->Rectangle(clientrect.right-4, clientrect.bottom-iDrawWidth+8, clientrect.right-11, clientrect.bottom-iDrawWidth+15);
  }

  pDC->SelectObject(pOldBrush); //restore the old brush

  pDC->SelectObject(pOldPen);
} // DrawEndPegs()


/**
 * draw a single peg
 *
 * @param pDC           device context
 * @param point         location of peg
 * @param colour        colour of peg
 * @param iDirection    direction of peg
 **/
void GradientCtrlImpl::DrawPeg(CDC *pDC, const CPoint& point, const COLORREF colour, const int4 iDirection)
{
  CBrush brush, *pOldBrush;
  POINT vPoints[3];

  //Select The Colour
  brush.CreateSolidBrush(colour);
  CPen *pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
  pOldBrush = pDC->SelectObject(&brush);

  //Prepare the coordinates
  switch (iDirection)
  {
  case 0:
    vPoints[0].x = point.x;
    vPoints[0].y = point.y+1;
    vPoints[1].x = point.x+9;
    vPoints[1].y = point.y-3;
    vPoints[2].x = point.x+9;
    vPoints[2].y = point.y+5;
    break;
  case 1:
    vPoints[0].x = point.x+1;
    vPoints[0].y = point.y;
    vPoints[1].x = point.x-3;
    vPoints[1].y = point.y-9;
    vPoints[2].x = point.x+5;
    vPoints[2].y = point.y-9;
    break;
  case 2:
    vPoints[0].x = point.x-1;
    vPoints[0].y = point.y+1;
    vPoints[1].x = point.x-10;
    vPoints[1].y = point.y-3;
    vPoints[2].x = point.x-10;
    vPoints[2].y = point.y+5;
    break;
  default:
    vPoints[0].x = point.x+1;
    vPoints[0].y = point.y+1;
    vPoints[1].x = point.x-3;
    vPoints[1].y = point.y+10;
    vPoints[2].x = point.x+5;
    vPoints[2].y = point.y+10;
    break;
  }
  pDC->Polygon(vPoints, 3);

  //Restore the old brush and pen
  pDC->SelectObject(pOldBrush);
  pDC->SelectObject(pOldPen);

  //----- Draw lines manually in the right directions ------//
  CPen outlinepen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
  pOldPen = pDC->SelectObject(&outlinepen);

  pDC->MoveTo(vPoints[0]);
  pDC->LineTo(vPoints[1]);
  pDC->LineTo(vPoints[2]);
  pDC->LineTo(vPoints[0]);

  pDC->SelectObject(pOldPen);

  brush.DeleteObject();

} // DrawPeg


/**
 * extract a number of lines from text
 *
 * @param sSource   source text
 * @param iLine     number of lines
 **/
CString GradientCtrlImpl::ExtractLine(const CString& sSource, int4 iLine) const
{
  int4 iStart(0), iEnd(0);
  CString sTextLine;

  if (sSource == _T(""))
    return _T("");

  while (iLine > 0)
  {
    iStart = sSource.Find(_T('\n'), iStart);
    if (iStart == -1)
      return _T("");
    iStart++;
    iLine--;
  }

  iEnd = sSource.Find(_T('\n'), iStart);
  if (iEnd == -1)
    iEnd = sSource.GetLength();

  sTextLine = sSource;
  CString sOp = sTextLine.Mid(iStart, iEnd-iStart);

  return sTextLine.Mid(iStart, iEnd-iStart);

} // ExtractLine()


/**
 * parse a tooltip text
 *
 * @param sTipText   text to parse
 * @param peg        the peg
 **/
void GradientCtrlImpl::ParseToolTipLine(CString& sTipText, const Peg& peg) const
{
  CString str;

  str = _T("&");
  sTipText.Replace(ampersandtag, str);

  str.Format(_T("%.2f"), peg.GetPosition());
  sTipText.Replace(postag, str);

  str.Format(_T("%2.2X"), GetRValue(peg.GetColor()));
  sTipText.Replace(hexrtag, str);

  str.Format(_T("%2.2X"), GetGValue(peg.GetColor()));
  sTipText.Replace(hexgtag, str);

  str.Format(_T("%2.2X"), GetBValue(peg.GetColor()));
  sTipText.Replace(hexbtag, str);

  str.Format(_T("%0.3f"), ((float)GetRValue(peg.GetColor()))/255.0);
  sTipText.Replace(floatrtag, str);

  str.Format(_T("%0.3f"), ((float)GetGValue(peg.GetColor()))/255.0);
  sTipText.Replace(floatgtag, str);

  str.Format(_T("%0.3f"), ((float)GetBValue(peg.GetColor()))/255.0);
  sTipText.Replace(floatbtag, str);

  str.Format(_T("%u"), GetRValue(peg.GetColor()));
  sTipText.Replace(rtag, str);

  str.Format(_T("%u"), GetGValue(peg.GetColor()));
  sTipText.Replace(gtag, str);

  str.Format(_T("%u"), GetBValue(peg.GetColor()));
  sTipText.Replace(btag, str);

} // ParseToolTipLine()


/**
 * synchs the tooltips
 **/
void GradientCtrlImpl::SynchronizeTooltips()
{
  CString sTipText;
  CRect pegrect, clientrect;
  const int4 iDrawWidth(GetDrawWidth());

  CClientDC dc(&m_rGradOwner);

  m_rGradOwner.GetClientRect(&clientrect);

  if (!m_ctrlToolTip.GetSafeHwnd())
    m_ctrlToolTip.Create(&m_rGradOwner);

  //----- Out with the old -----//
  int4 i(0);
  for(i = 0; i < m_iRectCount+1; i++)
    m_ctrlToolTip.DelTool(&m_rGradOwner, i);
  m_iRectCount = m_rGradOwner.m_gradient.GetPegCount();

  if (m_rGradOwner.m_sToolTipFormat == _T(""))
    return;

  //----- Add the main pegs -----//
  for (i = 0; i < m_iRectCount; i++)
  {
    if (m_bLeftDownSide)
    {
      GetPegRect(i, &pegrect, false);
      sTipText = ExtractLine(m_rGradOwner.m_sToolTipFormat, 1);
      ParseToolTipLine(sTipText, m_rGradOwner.m_gradient.GetPeg(i));
      m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, i+6);
    }

    if (m_bRightUpSide)
    {
      GetPegRect(i, &pegrect, true);
      sTipText = ExtractLine(m_rGradOwner.m_sToolTipFormat, 1);
      ParseToolTipLine(sTipText, m_rGradOwner.m_gradient.GetPeg(i));
      m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, i+6);
    }
  }

  //----- Add ones for the end pegs -----//
  sTipText = ExtractLine(m_rGradOwner.m_sToolTipFormat, 2);
  ParseToolTipLine(sTipText, m_rGradOwner.m_gradient.GetPeg(GradientList::STARTPEG));
  if (sTipText != _T(""))
  {
    if (IsVertical())
    { 
      if (m_bRightUpSide)
      {
        pegrect.SetRect(iDrawWidth-15, 4, iDrawWidth-8, 11);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 1);
      }

      if(m_bLeftDownSide)
      {
        pegrect.SetRect(8, 4, 15, 11);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 2);
      }
    }
    else
    { 
      if (m_bRightUpSide)
      {
        pegrect.SetRect(4, clientrect.bottom-iDrawWidth+8, 11,
          clientrect.bottom-iDrawWidth+15);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 1);
      }

      if(m_bLeftDownSide)
      {
        pegrect = CRect(4, clientrect.bottom-15, 11, clientrect.bottom-8);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 2);
      }
    }
  }

  sTipText = ExtractLine(m_rGradOwner.m_sToolTipFormat, 3);
  ParseToolTipLine(sTipText, m_rGradOwner.m_gradient.GetPeg(GradientList::ENDPEG));
  if (sTipText != _T(""))
  {
    if (IsVertical())
    {   
      if (m_bRightUpSide)
      { 
        pegrect.SetRect(iDrawWidth-15, clientrect.bottom-11, iDrawWidth-8,
          clientrect.bottom-4);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 3);
      }

      if (m_bLeftDownSide)
      {
        pegrect.SetRect(8, clientrect.bottom-11, 15,
          clientrect.bottom-4);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 4);
      }
    }
    else
    {   
      if(m_bRightUpSide)
      { 
        pegrect.SetRect(clientrect.right-11, clientrect.bottom-iDrawWidth+8,
          clientrect.right-4, clientrect.bottom-iDrawWidth+15);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 3);
      }

      if(m_bLeftDownSide)
      {
        pegrect.SetRect(clientrect.right-11, clientrect.bottom-15,
          clientrect.right-4, clientrect.bottom-8);
        m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 4);
      }
    }
  }

  //----- Add one for the gradient -----//
  if (IsVertical())
    pegrect.SetRect(m_bLeftDownSide?23:4, 4,
    iDrawWidth-(m_bRightUpSide?23:4),
    (clientrect.bottom>10)?clientrect.bottom-4:clientrect.bottom);
  else
    pegrect.SetRect(4, clientrect.bottom-iDrawWidth+(m_bRightUpSide?23:4),
    (clientrect.right>10)?clientrect.right-4:clientrect.right,
    clientrect.bottom-(m_bLeftDownSide?23:4));

  sTipText = ExtractLine(m_rGradOwner.m_sToolTipFormat, 4);
  if(sTipText != _T(""))
  {
    m_ctrlToolTip.AddTool(&m_rGradOwner, sTipText, pegrect, 5);
  }

} // SynchronizeTooltips()


/**
 * show a tooltip
 *
 * @param point   location for tip display
 * @param sText   tip text to display
 **/
void GradientCtrlImpl::ShowTooltip(const CPoint& point, const CString& sText)
{
  uint4 uID(0);       // for ti initialization

  if (sText == _T(""))
    return;

  // CREATE A TOOLTIP WINDOW
  if (m_hToolTip == NULL)
  {
    m_hToolTip = CreateWindowEx(WS_EX_TOPMOST,
      TOOLTIPS_CLASS, NULL, TTS_NOPREFIX | TTS_ALWAYSTIP,   
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      NULL, NULL, NULL, NULL);


    // INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE
    m_toolInfo.cbSize = sizeof(TOOLINFO);
    m_toolInfo.uFlags = TTF_TRACK;
    m_toolInfo.hwnd = NULL;
    m_toolInfo.hinst = NULL;
    m_toolInfo.uId = uID;
    m_toolInfo.lpszText = (LPTSTR)(LPCTSTR) sText;
    // ToolTip control will cover the whole window
    m_toolInfo.rect.left = 0;
    m_toolInfo.rect.top = 0;
    m_toolInfo.rect.right = 0;
    m_toolInfo.rect.bottom = 0;

    // SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW
    ::SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &m_toolInfo);
  }

  ::SendMessage(m_hToolTip, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD) MAKELONG(point.x, point.y));
  ::SendMessage(m_hToolTip, TTM_TRACKACTIVATE, true, (LPARAM)(LPTOOLINFO) &m_toolInfo);

} // ShowTooltip()

/*void GradientCtrlImpl::HideTooltip()
{
if(m_hToolTip != NULL)
::SendMessage (m_hToolTip, TTM_TRACKACTIVATE, false, (LPARAM) (LPTOOLINFO) &m_toolInfo);
}*/

/*void GradientCtrlImpl::MoveTooltip(CPoint point)
{
if(m_hToolTip != NULL)
{
::SendMessage(m_hToolTip, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD) MAKELONG(point.x, point.y));
}
}*/


/**
 * set tooltip
 *
 * @param sText   text for tip
 **/
void GradientCtrlImpl::SetTooltipText(const CString& sText)
{ 
  if (m_hToolTip != NULL)
  {
    m_toolInfo.lpszText = (LPTSTR)(LPCTSTR) sText; 
    ::SendMessage(m_hToolTip, TTM_SETTOOLINFO, 0, (LPARAM)(LPTOOLINFO) &m_toolInfo);    
  }
} // SetTooltipText()


/**
 * destroy tooltip window
 **/
void GradientCtrlImpl::DestroyTooltip()
{
  ::DestroyWindow(m_hToolTip);
  m_hToolTip = NULL;
} // DestroyTooltip()


/**
 * draw the gradient colours
 *
 * @return   true if control is displayed vertical, false for horizontal
 **/
bool GradientCtrlImpl::IsVertical() const
{
  if (m_rGradOwner.m_eOrientation == GradientCtrl::FORCEVERTICAL)
    return true;
  else if (m_rGradOwner.m_eOrientation == GradientCtrl::FORCEHORIZONTAL)
    return false;
  else
  {
    CRect clientrect;
    m_rGradOwner.GetClientRect(&clientrect);
    return (clientrect.right <= clientrect.bottom);
  }
} // IsVertical()


/**
 * get draw width from owner
 * @return   width
 **/
int4 GradientCtrlImpl::GetDrawWidth() const
{
  CRect clientrect;
  int4 iDW;

  m_rGradOwner.GetClientRect(&clientrect);

  iDW = (m_rGradOwner.m_iWidth == GCW_AUTO) ? (IsVertical() ? clientrect.right :
  clientrect.bottom) : m_rGradOwner.m_iWidth;
  return iDW;

} // GetDrawWidth()


/**
 * translates a position to a point
 *
 * @param fPos   position
 * @return       point
 **/
int4 GradientCtrlImpl::PointFromPos(const float4 fPos) const
{
  CRect clientrect;
  m_rGradOwner.GetClientRect(clientrect);
  const float4 fLength(IsVertical() ? ((float4)clientrect.Height() - 10.0f)
    : ((float)clientrect.Width() - 10.0f));

  return (int4)(fPos*fLength) + 4;
} // PointFromPos()


/**
 * translates a point to a position
 *
 * @param iPoint   point
 * @return         position
 **/
float4 GradientCtrlImpl::PosFromPoint(const int4 iPoint) const
{ 
  CRect clientrect;
  m_rGradOwner.GetClientRect(clientrect);
  const int4 iLength(IsVertical() ? (clientrect.bottom -  9) : (clientrect.right -  9));
  const int4 x(iPoint-5);

  float4 fRetVal((float4)x/(float4)iLength);

  if (fRetVal < 0) fRetVal = 0.0F;
  else if (fRetVal > 1) fRetVal = 1.0F;

  return fRetVal;
} // PosFromPoint()


/**
 * gets the amount of indent for a peg
 *
 * @param iIndex    peg index
 * @return          amount of indent
 **/
int4 GradientCtrlImpl::GetPegIndent(const int4 iIndex) const
{
  int4 iLastPegPos(-1), iPegIndent(0);

  for (int4 i(0); i <= iIndex; i++)
  {
    const Peg &peg = m_rGradOwner.m_gradient.GetPeg(i);
    if (iLastPegPos != -1 && iLastPegPos >= PointFromPos(peg.GetPosition())-10)
      iPegIndent += 1;
    else iPegIndent = 0;

    iLastPegPos = PointFromPos(peg.GetPosition());
  }

  return iPegIndent%2;
} // GetPegIndent()


/**
 * finds a peg from the given point
 *
 * @param point  point
 * @return       the peg in which the point is located
 **/
int4 GradientCtrlImpl::PtInPeg(const CPoint& point) const
{
  Peg peg;
  CRect pegrect, clientrect;
  const int4 iDrawWidth(GetDrawWidth());

  //----- Check if the point is on a marker peg -----//
  for (int4 iPeg(0); iPeg < m_rGradOwner.m_gradient.GetPegCount(); iPeg++)
  {
    peg = m_rGradOwner.m_gradient.GetPeg(iPeg);

    if (m_bLeftDownSide)
    {
      GetPegRect(iPeg, &pegrect, false);
      if (pegrect.PtInRect(point))
        return iPeg;
    }

    if (m_bRightUpSide)
    {
      GetPegRect(iPeg, &pegrect, true);
      if(pegrect.PtInRect(point))
        return iPeg;
    }
  }

  //----- Check if the point is on an  end peg -----//
  m_rGradOwner.GetClientRect(&clientrect);

  pegrect.left = iDrawWidth+8, pegrect.top = 4;
  pegrect.right = iDrawWidth+15, pegrect.bottom = 11;
  if (pegrect.PtInRect(point))
    return GradientList::STARTPEG;

  pegrect.left = iDrawWidth+8, pegrect.top = clientrect.bottom-11;
  pegrect.right = iDrawWidth+15, pegrect.bottom = clientrect.bottom-4;
  if (pegrect.PtInRect(point))
    return GradientList::ENDPEG;

  return -1;
} // PtInPeg()


/**
 * translates a position to a point
 *
 * @param iIndex    peg index
 * @param pRect     (out) display rect for the peg
 * @param bRight    left or right peg
 **/
void GradientCtrlImpl::GetPegRect(const int4 iIndex, CRect *pRect, const bool bRight) const
{
  CRect clientrect;
  const int4 iDrawWidth(GetDrawWidth());
  const bool bVertical(IsVertical());
  m_rGradOwner.GetClientRect(&clientrect);

  if (iIndex == GradientList::STARTPEG)
  {
    if (bRight)
    { 
      if (bVertical)
      {
        pRect->left = iDrawWidth-15, pRect->top = 4;
        pRect->right = iDrawWidth-8, pRect->bottom = 11;
      }
      else
      {
        pRect->left = 4, pRect->top = clientrect.bottom-iDrawWidth+8;
        pRect->right = 11, pRect->bottom = clientrect.bottom-iDrawWidth+15;
      }
    }
    else
    {
      if (bVertical)
      {
        pRect->left = 8, pRect->top = 4;
        pRect->right = 15, pRect->bottom = 11;
      }
      else
      {
        pRect->left = 4, pRect->top = clientrect.bottom-15;
        pRect->right = 11, pRect->bottom = clientrect.bottom-8;
      }
    }

    return;
  }

  if (iIndex == GradientList::ENDPEG)
  {
    if (bRight)
    { 
      if (bVertical)
      {
        pRect->left = iDrawWidth-15, pRect->top = clientrect.bottom-11;
        pRect->right = iDrawWidth-8, pRect->bottom = clientrect.bottom-4;
      }
      else
      {
        pRect->left = clientrect.right-11, pRect->top = clientrect.bottom-iDrawWidth+8;
        pRect->right = clientrect.right-4, pRect->bottom = clientrect.bottom-iDrawWidth+15;
      }
    }
    else
    {
      if (bVertical)
      {
        pRect->left = 8, pRect->top = clientrect.bottom-11;
        pRect->right = 15, pRect->bottom = clientrect.bottom-4;
      }
      else
      {
        pRect->left = clientrect.right-11, pRect->top = clientrect.bottom-15;
        pRect->right = clientrect.right-4, pRect->bottom = clientrect.bottom-8;
      }
    }

    return;
  }

  const Peg peg(m_rGradOwner.m_gradient.GetPeg(iIndex));
  int4 p(PointFromPos(peg.GetPosition()));
  int4 iIndent(GetPegIndent(iIndex)*11);

  if (bRight)
  {
    if (bVertical)
    {
      pRect->top = p - 3;
      pRect->bottom = p + 6;
      pRect->left = iDrawWidth+iIndent-23;
      pRect->right = iDrawWidth+iIndent-13;
    }
    else
    {
      pRect->top = clientrect.bottom-iDrawWidth-iIndent+13;
      pRect->bottom = clientrect.bottom-iDrawWidth-iIndent+23;
      pRect->left = p - 3;
      pRect->right = p + 6;
    }
  }
  else
  {
    if (bVertical)
    {
      pRect->top = p - 3;
      pRect->bottom = p + 6;
      pRect->left = 13-iIndent;
      pRect->right = 23-iIndent;
    }
    else
    {
      pRect->top = clientrect.bottom+iIndent-23;
      pRect->bottom = clientrect.bottom+iIndent-13;
      pRect->left = p - 3;
      pRect->right = p + 6;
    }
  }
} // GetPegRect()


// $Log: GradientCtrlImpl.C,v $
// Revision 1.4.2.1  2007/11/02 03:07:16  jmeade
// Issue 5583: allow for disabled/greyed gradient control.
//
// Revision 1.4  2007/03/13 18:36:38  jmeade
// code standards.
//
// Revision 1.3  2006/06/01 20:20:31  frank
// updated to visual studio 2005
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2004/05/05 23:23:28  jmeade
// Incremental update.
//
// Revision 1.1.2.1  2004/04/21 03:44:20  jmeade
// Gradient color edit control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GradientCtrlImpl.C,v 1.4.2.1 2007/11/02 03:07:16 jmeade Exp $
// $Id: GradientCtrlImpl.C,v 1.4.2.1 2007/11/02 03:07:16 jmeade Exp $
