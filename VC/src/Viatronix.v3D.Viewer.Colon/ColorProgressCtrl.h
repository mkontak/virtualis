// $Id: ColorProgressCtrl.h,v 1.4 2007/03/08 22:21:49 romy Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include "WinGDIUtils.h"
//#include "typedef.h"


namespace WinUtils
{

  class ColorProgressCtrl : public CProgressCtrl
  {
  public:

    /// Constructor
    ColorProgressCtrl();

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ColorProgressCtrl)
    //}}AFX_VIRTUAL

    // Generated message map functions

    int SetPosAndColor(int nPos, COLORREF uColor);

  protected:

    //{{AFX_MSG(ColorProgressCtrl)
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT uType, int iCX, int iCY);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

  private:

    /// Progress display update bitmap.
    BitmapDC m_bmpdc;

    /// Minimum of range for progress currently displayed in bitmap.
    int4 m_iCurrMin;

    /// Maximum of range for progress currently displayed in bitmap.
    int4 m_iCurrMax;

    /// Position for progress currently displayed in bitmap.
    int4 m_iCurrPos;

    /// Color to display the bar in.
    COLORREF m_uColor;

  };

} // namespace WinUtils


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: ColorProgressCtrl.h,v $
// Revision 1.4  2007/03/08 22:21:49  romy
// code review - header files adjusted
//
// Revision 1.3  2006/01/31 14:02:53  frank
// code review
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.4  2002/11/14 19:34:12  kevin
// Set the color along with the pos of the thermometer
//
// Revision 1.3  2002/08/15 17:05:45  jmeade
// Only update paint bitmap when necessary.
//
// Revision 1.2  2002/08/15 15:47:11  jmeade
// Black background (for contrast); Use background bitmap to prevent flicker.
//
// Revision 1.1  2002/08/14 20:41:41  jmeade
// Color progress control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ColorProgressCtrl.h,v 1.4 2007/03/08 22:21:49 romy Exp $
// $Id: ColorProgressCtrl.h,v 1.4 2007/03/08 22:21:49 romy Exp $
