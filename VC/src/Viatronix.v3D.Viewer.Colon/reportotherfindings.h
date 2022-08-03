// $Id: reportotherfindings.h,v 1.3.2.1 2008/02/12 20:32:25 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for other findings
// Owner: George Economos

#if !defined(AFX_REPORTOTHERFINDINGS_H__FE92E55F_0750_49D1_9746_918889F93F91__INCLUDED_)
#define AFX_REPORTOTHERFINDINGS_H__FE92E55F_0750_49D1_9746_918889F93F91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "ReportWndBase.h"
#include "ReportControls.h"
#include "ReportSubFinding.h"
#include "ReportGroupCtrl.h"

#include <map>
#include <utility>

#include "resource.h"

namespace ColonReport
{
  /// Group ID
  enum OrganGroupIDEnum
  {
    GID_Lung = 1,
    GID_Liver = 2,
    GID_Renal = 3,
    GID_Adrenal = 4,
    GID_Spleen = 5,
    GID_GI = 6,
    GID_Gynecology = 7,
    GID_Vascular = 8,
    GID_Miscellaneous = 9,
  };

  /// Other finding data structure
  struct OtherFindingData
  {
    OrganGroupIDEnum eGroupID;
    CString sName;
    UINT uDlgID;
    UINT uCommandID;
  };

  /// mapping a group to its string name
  typedef std::map<OrganGroupIDEnum, CString> TOrganGroupMap;

  /// mapping type of sub-findings to possible data
  typedef std::map<ReportLib::OtherFindingsEnum, OtherFindingData* > TSubFindingMap;

  // extra-colonic findings
  class ReportOtherFindings : public ReportWndBase
  {

  public:
    /// Constructor
    ReportOtherFindings(CWnd* pParent = NULL);
    /// Destructor
    virtual ~ReportOtherFindings();

    /// fill in map information
    static void FillGroupMaps(TOrganGroupMap& mapGroupNames, TSubFindingMap& mapSubFindings);

    //{{AFX_DATA(ReportOtherFindings)
    enum { IDD = IDD_RPT_OTHER_FINDINGS };
    //}}AFX_DATA

    /// Sets the current report
    virtual void SetReport(ReportLib::IReportRevisionPtr& spRevision);
    /// Called when the report view is deactivated
    virtual void OnReportViewUnactivate();
    
    //{{AFX_VIRTUAL(ReportOtherFindings)
    protected:
    /// DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);
    /// dialog init
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

  protected:
    /// Creates a new entry for the other findings data map
    static OtherFindingData* CreateMapEntry(const OrganGroupIDEnum eGroupID, LPCTSTR pszName, const UINT uDlgID, const UINT uCommandID);

    //{{AFX_MSG(ReportOtherFindings)
    afx_msg void OnDestroy();
    afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnShowWindow(BOOL bShow, UINT uStatus);
    afx_msg void OnMouseMove(UINT uFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
    afx_msg void OnLButtonDown(UINT uFlags, CPoint point);
    afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRClickItem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRptDeleteItem();
    afx_msg void OnRptDeleteAllItems();
    //}}AFX_MSG
    afx_msg void OnAddFinding(UINT uID);
    DECLARE_MESSAGE_MAP()

    /// Deletes the indicated item
    void DeleteItem(const int4 iItem);
   
    /// show a sub-finding
    BOOL ShowSubFinding(LPCTSTR pszID);
    /// hide currently visible sub-finding
    void HideSubFinding();
    /// true if item is within specified group
    bool IsItemInGroup(const int4 iGroupID);

  private:
    /// Report group control
    ReportGroupCtrl m_wndGroup;
    /// ID of item that has the current focus
    int4 m_iFocusID;

    /// sub-finding pointer
    ReportSubFinding* m_pSubFinding;
    /// win rectangle
    CRect m_rcFrame;
    /// true when successfully initialized
    bool m_bInitComplete;

    struct ItemData
    {
      CString sID;
    };

    /// map of all groups
    TOrganGroupMap m_mapGroupNames;
    /// map of findings
    TSubFindingMap m_mapSubFindings;

  }; // class ReportOtherFindings

} // using namespace ColonReport

#endif

// Revision History:
// $Log: reportotherfindings.h,v $
// Revision 1.3.2.1  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.3  2007/03/10 03:19:30  jmeade
// code standards.
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1.2.1  2003/04/09 21:44:01  jmeade
// Delete all other findings option; Coding standards.
//
// Revision 3.1  2003/01/21 19:39:49  jmeade
// Duplicate: Revision : 3.0.4.1: Moving hard-coded strings to string table.
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
//    Rev 2.0   Sep 16 2001 23:41:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:38   ingmar
// Initial revision.
//
// Revision 1.10  2001/05/30 14:23:21  jmeade
// coding standards to fix name clashes
//
// Revision 1.9  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.8  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.7  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.6  2001/02/01 22:29:22  geconomos
// Fixed bug with not saving current view in other findings
//
// Revision 1.5  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/reportotherfindings.h,v 1.3.2.1 2008/02/12 20:32:25 jmeade Exp $
// $Id: reportotherfindings.h,v 1.3.2.1 2008/02/12 20:32:25 jmeade Exp $
