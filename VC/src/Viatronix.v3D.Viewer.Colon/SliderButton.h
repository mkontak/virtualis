// $Id: SliderButton.h,v 1.3 2006/02/01 14:33:08 mkontak Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SliderButton.h
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_SLIDERBUTTON_H__192E50B2_8D70_11D4_B6E7_00D0B7BF4092__INCLUDED_)
#define AFX_SLIDERBUTTON_H__192E50B2_8D70_11D4_B6E7_00D0B7BF4092__INCLUDED_

// includes
#include "Global.h"
#include "SkinButton.h"
#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



namespace WinUtils
{

  /**
   * Slider button class
   */
  class SliderButton : public SkinButton
  {
  public:

    /// constructor
    SliderButton();

    /// destructor
    virtual ~SliderButton();

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(SliderButton)
    protected:
    //}}AFX_VIRTUAL

    // Generated message map functions
  protected:
    //{{AFX_MSG(SliderButton)
    afx_msg virtual void OnClicked();
    afx_msg virtual void OnLButtonDown(UINT uFlags, CPoint point);
    afx_msg virtual void OnRButtonDown(UINT uFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnMouseLeave();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

  private:

    /// initialize internal slider control
    void InitSlider();

  private:

    /// internal slider control
    CSliderCtrl m_slider;

  };  // class SliderButton

} // namespace WinUtils

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDERBUTTON_H__192E50B2_8D70_11D4_B6E7_00D0B7BF4092__INCLUDED_)

// Revision History:
// $Log: SliderButton.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliderButton.h,v 1.3 2006/02/01 14:33:08 mkontak Exp $
// $Id: SliderButton.h,v 1.3 2006/02/01 14:33:08 mkontak Exp $
