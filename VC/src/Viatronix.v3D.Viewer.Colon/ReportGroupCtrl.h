// $Id: ReportGroupCtrl.h,v 1.3.2.2 2009/07/30 17:58:20 kchalupa Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Owner draw list control simulates radio groups and checkboxes
// Owner: George Economos

#ifndef ReportGroupCtrl_h__
#define ReportGroupCtrl_h__

#if _MSC_VER > 1000
#pragma once
#endif 

namespace ColonReport
{
  /// shows items in a radio group or checkbox group
  class ReportGroupCtrl : public CListCtrl
  {
  // enums and constants
  public:

    /// no group? :)
    enum { NO_GROUP = -100 };

  protected:

    /// check box state
    enum { UNCHECKED = 3, CHECKED = 4 };

  // construction
  public:

    /// Constructor
    ReportGroupCtrl();

  // member functions
  public:

    /// insert a new group into the list control
    int4 InsertGroup(const int4 iGroupID, LPCTSTR pszName);

    /// insert a group item into the list control
    bool InsertGroupItem(const int4 iGroupID, LPCTSTR pszText, const uint4 uData);

    /// sets the background image
    void SetBackgroundImage(HBITMAP hbm) { m_bmBackground.Attach(hbm); }

    /// set the ShowSelectedBold flag
    void SetSelectedBold(const bool newVal){m_bBoldSelected = newVal;}

    /// sets the UsingCheckboxes flag
    void SetCheckboxes(const bool newVal) {m_bCheckBoxes = newVal;}

    /// sets the UsingGroups flag
    void SetUseGroups(const bool newVal) {m_bUseGroups = newVal;}

    /// selects the item whose data matches uData
    void SelectItem(const uint4 uData);

    /// return the data of the selected item
    uint4 GetSelectedItem() const;

    /// selects or deselects all items
    void SetSelectionAll(const bool bSelectAll);

    /// return the check state of the item whose data matches uData
    bool GetCheck(const uint4 uData) const;

    /// sets the check state of the item whose data matches uData
    void SetCheck(const uint4 uData, const bool bCheck);

    /// finds an item whose data matches uData
    int4 FindItem(const uint4 uData) const;

    /// returns if a given item is a group item
    bool IsGroup(const int4 iItem) const;

    //{{AFX_VIRTUAL(ReportGroupCtrl)
  protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

    /// owner draw override
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  protected:
    /// sets an item's image
    void SetItemImage(const int4 iItem, const int4 iImage);

    /// return an item's image
    int4 GetItemImage(const int4 iItem) const;

    /// returns the next item within a group
    int4 GetNextGroupItem(const int4 iGroupID) const;

    //{{AFX_MSG(ReportGroupCtrl)
    
    /// handle erase of background
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    /// handle item changing
    afx_msg void OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle button down
    afx_msg void OnLButtonDown(UINT uFlags, CPoint point);

    /// handle button double click
    afx_msg void OnLButtonDblClk(UINT uFlags, CPoint point);

    /// handle r button down
    afx_msg void OnRButtonDown(UINT uFlags, CPoint point);

    /// handle r button double click
    afx_msg void OnRButtonDblClk(UINT uFlags, CPoint point);

    /// handle vertical scroll
    afx_msg void OnVScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar);

    /// handle item changed
    afx_msg BOOL OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  // member variables
  protected:

    /// background bitmap
    CBitmap m_bmBackground;
    
    /// image list control
    CImageList m_imlNormal;
    
    /// font for selected items
    CFont m_fontBold;
    
    /// ShowSelectedBold flag
    bool m_bBoldSelected;
    
    /// UseCheckboxes flag
    bool m_bCheckBoxes;
    
    /// UseGroups flag
    bool m_bUseGroups;

  }; // class ReportGroupCtrl

} // namespace ColonReport

#endif

// Revision History:
// $Log: ReportGroupCtrl.h,v $
// Revision 1.3.2.2  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.3.2.1  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.3  2007/03/10 04:10:43  jmeade
// code standards.
//
// Revision 1.2  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0.20.3  2004/03/30 23:45:46  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.0.20.2  2003/04/09 22:08:21  jmeade
// Coding standards.
//
// Revision 3.0.20.1  2003/04/09 21:41:25  jmeade
// Framework to process rt-click msg when not on an item; Coding standards.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:34   ingmar
// Initial revision.
// Revision 1.8  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.7  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.6  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportGroupCtrl.h,v 1.3.2.2 2009/07/30 17:58:20 kchalupa Exp $
// $Id: ReportGroupCtrl.h,v 1.3.2.2 2009/07/30 17:58:20 kchalupa Exp $
