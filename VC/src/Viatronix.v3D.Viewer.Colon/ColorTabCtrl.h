// $Id: ColorTabCtrl.h,v 1.2 2005/08/16 21:53:47 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ColorTabCtrl.h
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_COLORTABCTRL_H__BBFDEF95_0A37_4E13_9424_94D058AFDCD8__INCLUDED_)
#define AFX_COLORTABCTRL_H__BBFDEF95_0A37_4E13_9424_94D058AFDCD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ColorTabCtrl window

namespace WinUtils
{

  class ColorTabCtrl : public CTabCtrl
  {
  public:
    ColorTabCtrl();
    virtual ~ColorTabCtrl();

    virtual bool IsItemEnabled(const int iTab);
    bool EnableItem(const int iTab, const bool bEnable);

    int SetCurSelWithSelChangingMsg(int iNewSel);
    BOOL SubclassDlgItem(UINT uID, CWnd * pParent);

    int InsertItem(int iItem, LPCTSTR sItem, const bool bEnabled = true);
    BOOL SetItem(int iItem, LPCTSTR sItem, const bool bEnabled = true);

    int GetItemAtCursorPos();

  protected:
    BOOL TranslatePropSheetMsg(MSG * pMsg);
    int NextEnabledTab(const int iCurrentTab, const bool bWrap);
    int PrevEnabledTab(const int iCurrentTab, const bool bWrap);

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ColorTabCtrl)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    // Generated message map functions
  protected:
    //{{AFX_MSG(ColorTabCtrl)
    afx_msg BOOL OnSelChangingEx(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    // override to draw text only; eg, colored text or different font
    virtual void OnDrawText(CDC & dc, CRect rc, const CString & sText, const bool bEnabled);
  };

} // namespace WinUtils

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORTABCTRL_H__BBFDEF95_0A37_4E13_9424_94D058AFDCD8__INCLUDED_)

// Revision History:
// $Log: ColorTabCtrl.h,v $
// Revision 1.2  2005/08/16 21:53:47  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.2  2002/01/14 20:40:07  jmeade
// "Overridden" SetCurSel() function should not send selchange message, which
//  would be in direct contrast to base CTabCtrl class.
//
// Revision 3.1  2001/11/09 01:09:02  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/06 23:30:03  jmeade
// Allow parent to also handle TCN_SELCHANGING; Method to get item at current cursor position.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:38   ingmar
// Initial revision.
// Revision 1.1  2001/05/23 16:53:01  jmeade
// Color tab control, replete with enabled OR DISABLED tabs!!!!!
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ColorTabCtrl.h,v 1.2 2005/08/16 21:53:47 jmeade Exp $
// $Id: ColorTabCtrl.h,v 1.2 2005/08/16 21:53:47 jmeade Exp $
