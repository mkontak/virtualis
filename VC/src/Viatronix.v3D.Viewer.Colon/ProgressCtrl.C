// $Id: ProgressCtrl.C,v 1.1.2.6 2010/11/01 18:51:32 dongqing Exp $
//
// Copyright © 2000-2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak [mkontak@viatronix.com]



#include "StdAfx.h"
#include "ProgressCtrl.h"


using namespace WinUtils;

// defines
#define FILE_REVISION "$Revision: 1.1.2.6 $"


BEGIN_MESSAGE_MAP(ProgressCtrl, CProgressCtrl)
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
END_MESSAGE_MAP()


/**
 *  default constructor
 */
ProgressCtrl::ProgressCtrl() : m_bPaintBackground( true )
{
}


/**
 *  override: The flag has to be updated
 *
 *  @param bErase default parameter value. Always do.
 */
void ProgressCtrl::Invalidate(BOOL bErase = 1)
{
  m_bPaintBackground = true;

  CProgressCtrl::Invalidate();
} // Invalidate



/**
 *  override:
 *
 *  @param nPos the new position
 *  @return message id
 */
int ProgressCtrl::SetPos(int nPos)
{
  int iCurrentPos = GetPos();

  if ( nPos < iCurrentPos ) 
    m_bPaintBackground = true;

  return CProgressCtrl::SetPos(nPos);
} // SetPos()


/**
 *  override
 *
 *  @param pDC the pointer of CDC
 *  @return BOOL always false
 */
BOOL ProgressCtrl::OnEraseBkgnd( CDC * pDC )
{
	return FALSE;
} // OnEraseBkgnd



/**
 *  override paint to avoid Win 7 Aero conflict
 */
void ProgressCtrl::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  CRect clientRect;
  CRect windowRect;
  static CRect prevWindowRect;

  GetWindowRect(windowRect);
  windowRect.SetRect(0, 0, windowRect.Width(), windowRect.Height());

  GetClientRect(clientRect);
  clientRect.SetRect(1, 1, windowRect.Width() + 1, windowRect.Height() + 1);

  //if( m_bPaintBackground || prevWindowRect != windowRect ) // comments this out fix the resize problem
  {
    prevWindowRect.top = windowRect.top;
    prevWindowRect.bottom = windowRect.bottom;
    prevWindowRect.left = windowRect.left;
    prevWindowRect.right = windowRect.right;

    CBrush backgroundBrush;
    backgroundBrush.CreateSolidBrush( RGB( 145, 145, 145 ));
    dc.FillRect(windowRect, &backgroundBrush);
    m_bPaintBackground = false;
  }

  CRect barRect = clientRect;
  //Is Control Horizontal or Vertical.
  if(( GetStyle() & PBS_VERTICAL ) == 0 )
  {
      int rangeLow, rangeHigh;
      GetRange(rangeLow, rangeHigh);

      int x = GetPos(), y;

      if(x < rangeLow || x > rangeHigh)
      return;

      // Calculate the width

      x -= rangeLow;
      x *= 100;
      x /= (rangeHigh - rangeLow);

      y = clientRect.Width();
      y *= x;
      y /= 100;
      y += clientRect.left;

      // Using the y variable as the new width, define a new rectangle

      barRect.right = barRect.left + y;
  }
  else
  {
      int rangeLow, rangeHigh;
      GetRange(rangeLow, rangeHigh);

      int y = GetPos(), x;

      if(y < rangeLow || y > rangeHigh)
      return;

      //Calculate Height
      y -= rangeLow;
      y *= 100;
      y /= (rangeHigh - rangeLow);

      x = clientRect.Height();
      x *= y;
      x /= 100;
      x += clientRect.top;

      
      barRect.top = barRect.bottom - x;

  }
  // Use barRect as the rectangle for drawing the progress bar.
  CBrush brush;
  brush.CreateSolidBrush( RGB( 5, 190, 15 ));
  dc.FillRect(barRect, &brush);

  // Draw the border of the progress control. Use a standard 3D border
  dc.Draw3dRect(windowRect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DLIGHT));
} //OnPaint()

// undefines
#undef FILE_REVISION


// $Log: ProgressCtrl.C,v $
// Revision 1.1.2.6  2010/11/01 18:51:32  dongqing
// Fix the resize artifacts on background
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.5  2010/11/01 17:40:03  mkontak
// Fix for resize problem with artifacts appearing in theprogress bar.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.4  2010/11/01 12:40:55  dongqing
// add comments on Mark's fix for the Win 7 progress bar of 3D fly thru
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.0  2010/10/29 13:07:27  mkontak
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ProgressCtrl.C,v 1.1.2.6 2010/11/01 18:51:32 dongqing Exp $
// $Id: ProgressCtrl.C,v 1.1.2.6 2010/11/01 18:51:32 dongqing Exp $

