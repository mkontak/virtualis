// $Id: ColorProgressCtrl.C,v 1.2 2005/08/12 21:48:53 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ProgressCtrl.cpp
// Author(s): Jeffrey Meade  jmeade@viatronix.net

// ProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColorProgressCtrl.h"
#include "typedef.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

#define FILE_REVISION "$Revision: 1.2 $"

using namespace WinUtils;

/////////////////////////////////////////////////////////////////////////////
// ProgressCtrl


/**
 * Constructor 
 */
ColorProgressCtrl::ColorProgressCtrl() : m_iCurrMin(0), m_iCurrMax(0), m_iCurrPos(0), m_uColor(RGB(255,255,255))
{
}

BEGIN_MESSAGE_MAP(ColorProgressCtrl, CProgressCtrl)
  //{{AFX_MSG_MAP(ColorProgressCtrl)
  ON_WM_PAINT()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ColorProgressCtrl message handlers


/**
 * WM_SIZE handler, initializes update bitmap.
 *
 * @param UINT uType   Type of resize event.
 * @param int iCX      Width of control.
 * @param int iCY      Height of control.
 */
void ColorProgressCtrl::OnSize(UINT uType, int iCX, int iCY)
{
  CProgressCtrl::OnSize(uType, iCX, iCY);
  CDC * pDC = GetDC();
  if (pDC)
  {
    m_bmpdc.Reset();
    m_bmpdc.m_bmp.CreateCompatibleBitmap(pDC, iCX, iCY);
    m_bmpdc.CreateBmpDC(pDC);
    ReleaseDC(pDC);
    m_iCurrMin = 0;
    m_iCurrMax = 0;
    m_iCurrPos = 0;
  }
}


/**
 * Sets the level and also changes the color.
 */
int ColorProgressCtrl::SetPosAndColor(int nPos, COLORREF uColor)
{
  m_uColor = uColor;
  return CProgressCtrl::SetPos(nPos);
}


/**
 * WM_PAINT handler, draws progress.
 */
void ColorProgressCtrl::OnPaint() 
{
  int4 iMin, iMax;
  GetRange(iMin, iMax);
  if ((GetPos() != m_iCurrPos) || (iMin != m_iCurrMin) || (iMax != m_iCurrMax))
  {
    const int4 iRange(iMax - iMin);
    const float4 fPos(iRange != 0 ? (float4(GetPos()) / float4(iRange)) : 0);
    CRect clientRect;   GetClientRect(clientRect);
    // Draw black background
    m_bmpdc.m_dc.FillSolidRect(clientRect, 0x0);
    // Draw outline border in progress color
    m_bmpdc.m_dc.Draw3dRect(clientRect, m_uColor, m_uColor);

    // Leave border around progress meter
    clientRect.DeflateRect(3,3);
    // Calculate rectangle for progress
    if (GetStyle() & PBS_VERTICAL)
    {
      clientRect.top = clientRect.bottom - int4(float4(clientRect.Height()) * fPos);
    }
    else
    {
      clientRect.right = clientRect.left + int4(float4(clientRect.Width()) * fPos);
    }
    // Draw progress meter
    m_bmpdc.m_dc.FillSolidRect(clientRect, m_uColor);

    // Update draw statistics
    m_iCurrPos = GetPos();
    m_iCurrMin = iMin;
    m_iCurrMax = iMax;
  }

  CPaintDC dc(this); // device context for painting
  CRect clRect; GetClientRect(clRect);
  dc.BitBlt(0,0,clRect.Width(),clRect.Height(),&m_bmpdc.m_dc,0,0,SRCCOPY);
}

#undef FILE_REVISION
// $Log: ColorProgressCtrl.C,v $
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.6  2002/11/14 19:34:11  kevin
// Set the color along with the pos of the thermometer
//
// Revision 1.5  2002/11/11 21:18:25  kevin
// Debug to see why this is not being called
//
// Revision 1.4  2002/11/11 14:25:04  kevin
// First attempt at making the thermometer match the centerline
// color (why does the LogFyi not work at all??)
//
// Revision 1.3  2002/08/15 17:05:24  jmeade
// Only update paint bitmap when necessary.
//
// Revision 1.2  2002/08/15 15:47:11  jmeade
// Black background (for contrast); Use background bitmap to prevent flicker.
//
// Revision 1.1  2002/08/14 20:41:41  jmeade
// Color progress control.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ColorProgressCtrl.C,v 1.2 2005/08/12 21:48:53 jmeade Exp $
// $Id: ColorProgressCtrl.C,v 1.2 2005/08/12 21:48:53 jmeade Exp $
