// $Id: PropertySheetDlg.h,v 1.4 2007/03/10 06:09:14 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PropertySheetDlg.h
// Author(s): Jeffrey Meade  jmeade@viatronix.net
//
// PropSheet.h : header file
// Adapted from MSDN article #Q142170 CMySheet
//

#if !defined(AFX_PROPERTYSHEETDLG_H__1033E6B7_6FB5_11D4_B6DE_00D0B7BF4092__INCLUDED_)
#define AFX_PROPERTYSHEETDLG_H__1033E6B7_6FB5_11D4_B6DE_00D0B7BF4092__INCLUDED_

#include "SkinButton.h"

/////////////////////////////////////////////////////////////////////////////
// PropertySheetDlg

namespace WinUtils
{
  /// tabbed properties dialog
  class PropertySheetDlg : public CPropertySheet
  {
  public:
    /// Constructor
    PropertySheetDlg(UINT uIDCaption, CWnd * pParentWnd = NULL, UINT uSelectPage = 0);

    /// Constructor
    PropertySheetDlg(LPCTSTR sCaption, CWnd * pParentWnd = NULL, UINT uSelectPage = 0);

    /// Destructor
    virtual ~PropertySheetDlg();

  /// Overrides
    /// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(PropertySheetDlg)
  public:
    /// initialization
    virtual BOOL OnInitDialog();

  protected:
    /// wm_notify command
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult);
    /// wm_command command
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

    /// Generated message map functions
  protected:
    DECLARE_DYNAMIC(PropertySheetDlg)

    /// Override called when building the list of property pages (i.e. used for initializations)
    virtual void BuildPropPageArray();

    //{{AFX_MSG(PropertySheetDlg)
    //}}AFX_MSG
    /// Called when the active page needs resizing
    afx_msg LONG_PTR OnResizePage (WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()

  protected:
    /// Display rectangle for property pages
    RECT m_pageRect;

    /// Font for all pages
    CFont m_pageFont;

    /// Skin buttons for the property sheet
    SkinButton m_okBtn, m_cancelBtn, m_applyBtn, m_helpBtn;
  }; // class PropertySheetDlg

} // namespace WinUtils

/////////////////////////////////////////////////////////////////////////////
#endif // if !defined(AFX_PROPERTYSHEETDLG_H__1033E6B7_6FB5_11D4_B6DE_00D0B7BF4092__INCLUDED_)

// Revision History:
// $Log: PropertySheetDlg.h,v $
// Revision 1.4  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.3  2007/03/08 22:22:20  romy
// code review - header files adjusted
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1  2002/09/25 18:55:48  jmeade
// Code standards.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:44   ingmar
// Initial revision.
// Revision 1.3  2001/05/11 21:13:52  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PropertySheetDlg.h,v 1.4 2007/03/10 06:09:14 jmeade Exp $
// $Id: PropertySheetDlg.h,v 1.4 2007/03/10 06:09:14 jmeade Exp $
