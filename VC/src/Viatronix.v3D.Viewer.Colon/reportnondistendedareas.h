// $Id: reportnondistendedareas.h,v 1.9.2.1 2009/12/09 20:19:44 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for Non Distended findings
// Owner: George Economos

#if !defined(AFX_REPORTNONDISTENDEDAREAS_H__426A9EE1_37A6_44AF_B4B4_80FC76C76E4A__INCLUDED_)
#define AFX_REPORTNONDISTENDEDAREAS_H__426A9EE1_37A6_44AF_B4B4_80FC76C76E4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ReportWndBase.h"
#include "ReportControls.h"
#include "ViatronChildView.h"
#include "trackball.h"
#include "reportGroupCtrl.h"
#include "ViatronStudy.h"


namespace ColonReport
{
  /** displays non-distended areas of scans **/
  class ReportNonDistendedAreas : public ReportWndBase
  {
  public:
    /// standard constructor
    ReportNonDistendedAreas(CWnd* pParent = NULL);

    //{{AFX_DATA(ReportNonDistendedAreas)
    enum { IDD = IDD_RPT_NON_DISTENDED };
    CStatic m_wndFrame;
    ReportListCtrl  m_wndEntries;
    //}}AFX_DATA

    /// set the report revision
    virtual void SetReport(ReportLib::IReportRevisionPtr& spRevision);
    /// called when this view is activated
    virtual void OnReportViewActivate();
    /// called when the view is deactivated
    virtual void OnReportViewUnactivate();

    //{{AFX_VIRTUAL(ReportNonDistendedAreas)
    protected:
    /// DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

  protected:
    /// entry location control
    ReportGroupCtrl m_wndGroupLocation;
    /// idle-time processing
    virtual void  GLIdle();

    struct ItemData
    {
      CString m_sID;
      ReportLib::DatasetPositionEnumProp m_eDatasetPos;
    }; // struct ItemData

    /// small layout with a single overview
    class OverviewChildWnd : public ColonViews::Layouts::ViatronixChildWnd
    {
    public:
      /// constructor
      OverviewChildWnd(const bool bDisplayAllSegments = true);
      /// destructor
      virtual ~OverviewChildWnd();
      /// gets pointer to the overview
      GLChildViewPtr GetOverview() { return &GetChildViewByPos(0); }

    protected:
      /// override to perform view creation
      void CreateChildViews(HWND hWndThis);
      /// override to calculate child view dimensions/location
      void CalculateViewRects(const int4 iWidth, const int4 iHeight);
      /// renders child views
      void GLRenderViews();
      /// mouse movement handler
      afx_msg virtual void OnMouseMove(UINT uFlags, CPoint point);
      /// mfc message map
      DECLARE_MESSAGE_MAP()
      /// WM_SIZE handler
      afx_msg void OnSize( UINT uType, int4 iCX, int4 iCY );

      /// returns the resource ID for indicated window orientation
      inline virtual uint4 LandscapeLayoutID()  { return 0; }
      /// returns the resource ID for indicated window orientation
      inline virtual uint4 WidescreenLayoutID() { return 0; }
      /// returns the resource ID for indicated window orientation
      inline virtual uint4 PortraitLayoutID()   { return 0; }

    private:
      /// Store current overview modes and reset in destructor
      VtxViewModes m_savedOverViewModes;
      /// Save the current supine segment ordering, set all segments for non-distended areas display
      SegmentList::SegmentOrderVector m_vSupineSegmentOrder;
      /// Save the current segment, which is altered during display
      int4 m_iSavedSupineSegment;

      /// Save the current prone segment ordering, set all segments for non-distended areas display
      SegmentList::SegmentOrderVector m_vProneSegmentOrder;
      /// Save the current segment, which is altered during display
      int4 m_iSavedProneSegment;

      /// saves constructor value of all-segments
      const bool m_bDisplayAllSegments;
      /// friend owner class
      friend ReportNonDistendedAreas;
    }; // class Overview

    /// overview child view
    OverviewChildWnd m_wndOverview;
     
    //{{AFX_MSG(ReportNonDistendedAreas)
    virtual BOOL OnInitDialog();
    afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRptDeleteNondistended();
    afx_msg void OnRptDeleteAllNondistended();
    afx_msg void OnRptReloadNondistended();
    afx_msg void OnRptNondistendedAreaUncertain();
    afx_msg void OnRclickEntries(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    /// after changing between entries
    BOOL OnEntryChanged(LPCTSTR pszEntryID);
    /// before changing between entries
    BOOL OnEntryChanging(LPCTSTR pszEntryID);
    /// loads the report
    void LoadReport();
    /// identifies current entry
    CString m_sCurrentEntry;
  };

} // namespace ColonReport


#endif

// Revision History:
// $Log: reportnondistendedareas.h,v $
// Revision 1.9.2.1  2009/12/09 20:19:44  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.9  2007/03/10 03:19:30  jmeade
// code standards.
//
// Revision 1.8  2006/10/03 18:14:08  jmeade
// Issue 4946: display of decubitus patient positions.
//
// Revision 1.7  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.6.2.2  2006/01/05 16:04:00  geconomos
// Issue# 4589: Overview not being sized correctly
//
// Revision 1.6.2.1  2005/12/14 20:16:57  jmeade
// Issue 4563: Only 'reset display' for non-distended overview image.
//
// Revision 1.6  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.5  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.8.2.3.2.1.4.1  2005/07/18 19:54:31  geconomos
// Issue #4288: fixed overview image
//
// Revision 3.8.2.3.2.1  2005/04/22 15:48:55  jmeade
// Issue 4134: Disable mouse help.
//
// Revision 3.8.2.3  2005/02/15 19:46:33  jmeade
// merge from vc1.4 branch.
//
// Revision 3.8.2.2.2.1  2005/01/20 21:32:49  jmeade
// allow drawing of overview text.
//
// Revision 3.8.2.2  2004/06/09 15:13:05  jmeade
// Issue 3709: Must save and reset the current segment before changing segment selections.
//
// Revision 3.8.2.1  2003/02/22 11:58:21  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.8  2002/11/15 00:37:50  jmeade
// No hard-coding of # of child views; Hide child view storage (use access methods).
//
// Revision 3.7  2002/09/10 20:50:13  jmeade
// Unused (and undefined) class variable.
//
// Revision 3.6  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.4  2002/07/03 03:23:21  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.3.2.1  2002/07/02 20:47:53  jmeade
// Issue 2062:  Reset segment ordering for displaying non-distended areas.
//
// Revision 3.3  2002/01/15 20:49:14  binli
// issue 1424: option to show/not show distance in report
//
// Revision 3.2  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.1  2001/10/20 10:24:55  jmeade
// Issue 1226:  Toggle off translucency option when viewing non-distended areas
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:38   ingmar
// Initial revision.
// Revision 1.13  2001/05/15 18:55:44  jmeade
// Code conventions
//
// Revision 1.12  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.11  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.10  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.9  2001/03/14 16:43:45  geconomos
// Added delete all to NonDistended areas
//
// Revision 1.8  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.7  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/reportnondistendedareas.h,v 1.9.2.1 2009/12/09 20:19:44 dongqing Exp $
// $Id: reportnondistendedareas.h,v 1.9.2.1 2009/12/09 20:19:44 dongqing Exp $
