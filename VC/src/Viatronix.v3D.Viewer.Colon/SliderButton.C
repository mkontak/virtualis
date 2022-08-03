// $Id: SliderButton.C,v 1.4 2007/03/09 01:11:49 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SliderButton.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

// includes
#include "stdafx.h"
#include "SliderButton.h"
#include "WinUtils.h"

// namespaces
using namespace WinUtils;


/////////////////////////////////////////////////////////////////////////////
// SliderButton


/**
 * Constructor 
 */
SliderButton::SliderButton() : SkinButton()
{
} // SliderButton()


/**
 * Destructor 
 */
SliderButton::~SliderButton()
{
} // ~SliderButton()


BEGIN_MESSAGE_MAP(SliderButton, SkinButton)
  //{{AFX_MSG_MAP(SliderButton)
  ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SliderButton windows message handlers


/**
 * Creation routine
 *
 * @param lpCreateStruct   create parameters
 * @return                 success/failure
 */
int SliderButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (SkinButton::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  /// initialize the slider
  InitSlider();

  return 0;

} // OnCreate(LPCREATESTRUCT lpCreateStruct) 


/**
 * Initializes the menu list box.
 */
void SliderButton::InitSlider()
{
  CRect rect;
  if (!IsWindow(m_slider.GetSafeHwnd()))
  {
    CWnd * pParent = GetParentOwner();

    GetWindowRect(rect);  pParent->ScreenToClient(rect);

    // Place just below, and off left side, of the button
    CPoint topLeftPt(rect.left+int4(rect.Width()*0.25), rect.bottom);
    m_slider.Create(0, CRect(topLeftPt, CSize(100,100)), this, GetDlgCtrlID()+1000);
    m_slider.ModifyStyleEx(0, WS_EX_TRANSPARENT);

    m_slider.SetRange(0, 100);
  }

//  m_listBox.UpdateSizeAndPosition();

  m_slider.SetWindowPos(NULL, rect.left, rect.bottom+5, 50, 20, SWP_NOZORDER);
}  // InitSlider()


/**
 * WM_MOUSELEAVE handler.
 */
void SliderButton::OnMouseLeave()
{
} // OnMouseLeave()


/**
 * Handler for button press.
 */
void SliderButton::OnClicked()
{
  InitSlider();
  m_slider.ShowWindow(!m_slider.IsWindowVisible() ? SW_SHOW : SW_HIDE);
  m_slider.SetFocus();
} // OnClicked()


/**
 * Left button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Point of mouse click.
 */
void SliderButton::OnLButtonDown(UINT uFlags, CPoint point)
{
  SkinButton::OnLButtonDown(uFlags, point);
} // OnLButtonDown(UINT uFlags, CPoint point)


/**
 * Right button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Location of mouse click.
 */
void SliderButton::OnRButtonDown(UINT uFlags, CPoint point) 
{
  OnClicked();  
  SkinButton::OnRButtonDown(uFlags, point);
} // OnRButtonDown(UINT uFlags, CPoint point) 


// Revision History:
// $Log: SliderButton.C,v $
// Revision 1.4  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.3  2006/02/01 14:33:08  mkontak
// Coding standards
//
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.1  2006/01/06 18:43:39  jmeade
// initial checkin.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliderButton.C,v 1.4 2007/03/09 01:11:49 jmeade Exp $
// $Id: SliderButton.C,v 1.4 2007/03/09 01:11:49 jmeade Exp $

