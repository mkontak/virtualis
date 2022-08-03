// $Id: CadResultsDlg.h,v 1.25.2.4 2009/11/13 17:37:06 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


/**
 * Dialog for displaying CAD results
 */


// pragmas
#pragma once


// includes
#include "resource.h"
#include "ViatronDialog.h"
#include "ViatronChildView.h"
#include "Dataset.h"
#include "ObservableObject.h"

class vxCAD::vxCadFinding;


namespace ColonViews
{
  namespace Dialogs
  {
    /// Listcontrol column indexes
    namespace CadListColumns
    {
      enum eLISTCOLS { eVISIBILITY = 0, eCOUNT = 1, eLOCATION = 2, eSIZE = 3, eID = 4 };
    } // namespace CadList

    /**
     * CadResultsDlg dialog
     */
    class CadResultsDlg : public ViatronDialog, public IObserver< vxCAD::vxCadFindingsArgs >
    {
      DECLARE_DYNAMIC(CadResultsDlg)

    public:

      /// constructor
      CadResultsDlg(CWnd* pParent = NULL, const uint4 uBkgdID = IDB_CADRES_DLG, const ReaderLib::DatasetOrientations::TypeEnum eDataset = ReaderLib::DatasetOrientations::ePRIMARY );

      /// destructor
      virtual ~CadResultsDlg();

    public:

      void OnNotified( const vxCAD::vxCadFindingsArgs & args );

    protected:
      /// Dialog data variable initialization
      virtual void DoDataExchange(CDataExchange* pDX);
      /// Dialog initialization
      virtual BOOL OnInitDialog();
      /// scroll messages
      afx_msg void OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar);
      /// Clicking an item in the list control
      afx_msg void OnClickCadList( NMHDR * pNMHDR, LRESULT * pResult );
      /// Clicking an item in the list control
      afx_msg void OnKeyDownCadList( NMHDR * pNMHDR, LRESULT * pResult );
      /// Double click
      afx_msg void OnLButtonDblClk(UINT uFlags, CPoint origPt);
      /// Jump to next finding
      afx_msg void OnNextFindingClicked();
      /// Toggles display of findings
      afx_msg void OnShowFindingsClicked();
      /// toggle slider visibility
      afx_msg void OnToggleShowSlider();
      /// User clicked sort column header
      afx_msg void OnColumnHeaderItemClick(NMHDR* pNMHDR, LRESULT* pResult);
      /// Message to update display
      afx_msg LRESULT OnMessageUpdateDisplay(WPARAM wParam, LPARAM lParam);
      /// WM_TIMER message
      afx_msg void OnTimer(UINT_PTR uIDEvent);
      /// WM_PAINT message
      afx_msg void OnPaint();
      /// Enter/OnOK
      virtual void OnOK();
      /// Esc/OnCancel
      virtual void OnCancel();
      /// Item state changes
      afx_msg void OnItemChangedResultsList(NMHDR *pNMHDR, LRESULT *pResult);
      /// Called when the dialog size changes
      afx_msg void OnSize(UINT nType, int cx, int cy);
      /// Mouse move handler
      afx_msg void OnMouseMove(UINT uFlags, CPoint point);
      /// When mouse leaves dialog
      virtual void OnMouseLeave();
      /// set cursor message
      afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);

      DECLARE_MESSAGE_MAP()

    private:
      /// initialize cad list control
      void InitCadList();
      /// Jump the viewpoint to a finding
      void JumpToFinding( const int4 iFinding );
      /// set the current finding selection
      void SetCurrentFindingSel(const int4 iSel);
      /// sets the text in the stats display
      void SetStatsText();
      /// Returns the current list control index of the finding
      int4 LocateFinding( const std::string & sID );
      /// Selects the finding in the list control
      void SelectFinding( const std::string & sID );
      /// Loads the cad vendor image
      void LoadCadVendorImage();
      /// Sets the cad slider value
      void SetSliderValue();
      /// Searches for an instance of the cad dialog and sends given message
      static BOOL SendMessageToDlgInstance(const ColonViews::ChildViews::ViatronChildView & childCaller, const UINT_PTR uMsg, const WPARAM wp = 0, const LPARAM lp = 0);
      /// Compare procedure for sorting list items
      static int CALLBACK FindingsListCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
      /// ensure the current displayed cad list is for the currently viewed dataset
      void SyncToCurrentDataset();

      /// returns the primary and 'other' dataset indicated by m_eIntendedDataset
      std::pair<ReaderLib::Dataset *,ReaderLib::Dataset *> GetIntendedDatasets();
      /// returns the primary and 'other' dataset indicated by m_eIntendedDataset
      static std::pair<ReaderLib::Dataset *,ReaderLib::Dataset *> GetIntendedDatasets(const ReaderLib::DatasetOrientations::TypeEnum eOrient);

    private:

      /// a common finding comparison function
      struct SortAscendingSkeletonIndex : public std::binary_function< vxCAD::vxCadFinding, vxCAD::vxCadFinding, bool >
      {
        bool operator()( vxCAD::vxCadFinding & x, vxCAD::vxCadFinding & y );
      }; // compare fn

    public:
      /// Dialog Data
      enum { IDD = IDD_CAD_RESULTS };

    private:
      /// WM_USER private window messages
      enum WM_USER_MSGS { WM_USER_UPDATE_DISPLAY = WM_USER + 100, WM_USER_NEXT_FINDING };
      /// update display message parameters
      enum WM_USER_UPDATE_DISPLAY_PARAMS { CADLIST_UPDATE_STATS = 0, CADLIST_UPDATE_SHOWALL = 1, CADLIST_REINIT = 2,
                                            CADLIST_SELECT_ITEM = 3, };

    private:
      /// List control for results
      CListCtrl m_listResults;
      /// Display findings toggle
      WinUtils::SkinAutoCheckButton m_btnShowFindings;
      /// Jump to next finding button
      WinUtils::SkinButton m_btnNextFind;
      /// image list for checkboxes
      CImageList m_imglistChecks;
      /// font for the warning static text
      CFont m_fontWarning;
      /// Updates any current modes
      WinUtils::WmTimer m_timerModeCheck;

      /// dataset currently being displayed in findings
      ReaderLib::Dataset * m_pCurrDatasetList;
      /// dataset whose findings are to be displayed
      const ReaderLib::DatasetOrientations::TypeEnum m_eIntendedDataset;

      /// vendor image static
      CStatic m_staticVendorImage;
      ///  vendor image
      Gdiplus::Bitmap * m_pVendorImage;
      /// bound to draw vendor image
      Gdiplus::Rect m_vendorImageBounds;
      /// website for the current vendor
      std::string m_sVendorWebsite;
      /// Whether the Logo was last drawn as opaque or part transparent
      bool m_bLogoDrawnOpaque;
      /// slider button
      CButton m_btnSlider;
      /// adjustment slider
      CSliderCtrl m_slider;

    }; // class CadResultsDlg

  } // namespace Dialogs

} // namespace ColonViews


// $Log: CadResultsDlg.h,v $
// Revision 1.25.2.4  2009/11/13 17:37:06  dongqing
// Issue Cad09-03: second cad pane to registration-cad layout
//
// Revision 1.25.2.3  2009/04/07 21:26:01  dongqing
// forget to check if the CAD finding list is empty or not. It was fixed now.
//
// Revision 1.25.2.2  2009/04/02 18:24:47  dongqing
// check if Medicsight CAD version is 4.0 or later. If it is, don't display slider
//
// Revision 1.25.2.1  2009/01/30 19:14:22  jmeade
// Issue 6238: ensure the cad of the current dataset is listed when jumping to findings
//
// Revision 1.25  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.24  2007/03/10 04:10:43  jmeade
// code standards.
//
// Revision 1.23  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.22  2006/09/14 17:41:23  jmeade
// Issue 4959: select finding on jump-to-finding.
//
// Revision 1.21  2006/09/01 22:56:22  jmeade
// Issue 4955: refresh cad findings list when toggling extra-colonic findings view.
//
// Revision 1.20  2006/08/02 23:05:55  jmeade
// Issue 4609: 'c' key toggles visibility of cad findings.
//
// Revision 1.19  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.18  2006/01/31 18:19:16  romy
// code review-rerranged include files
//
// Revision 1.17  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.16  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.15.2.15  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.15.2.13.2.1  2007/01/22 20:36:55  jmeade
// Issue 5325: implemented updating of CAD display.
//
// Revision 1.15.2.14  2006/09/18 16:21:29  jmeade
// Issue 4959: select finding on jump-to-finding.
//
// Revision 1.15.2.13  2006/04/14 18:06:40  jmeade
// Issue 4698: implemented list-based sort (instead of data-based sort); disabled sorting until bugs are worked out.
//
// Revision 1.15.2.12  2006/03/28 20:08:37  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.15.2.11  2006/03/22 20:56:44  frank
// moved the finding visibility and other information into a class
//
// Revision 1.15.2.10  2006/01/25 23:57:40  jmeade
// Issue 4607: Set the slider value when toggling between datasets.
//
// Revision 1.15.2.9  2006/01/07 04:35:20  frank
// fixed slider functionality and adjusted finding number display
//
// Revision 1.15.2.8  2006/01/06 16:49:25  jmeade
// horizontal sphericity slider.
//
// Revision 1.15.2.7  2006/01/05 19:14:36  jmeade
// scrollbar for adjustment.
//
// Revision 1.15.2.6  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.15.2.5  2005/12/20 19:47:30  jmeade
// highlight logo to indicate "clickability" when mouse hovers over.
//
// Revision 1.15.2.4  2005/11/15 19:41:55  frank
// allowed reversing through the cad findings
//
// Revision 1.15.2.3  2005/11/15 14:49:58  frank
// Issue #4514: Fixed crash jumping to a finding not in the current dataset
//
// Revision 1.15.2.2  2005/11/11 02:39:44  jmeade
// Shift-Space in EndoView jumps to previous cad finding.
//
// Revision 1.15.2.1  2005/11/09 02:15:00  jmeade
// cad pane on supine/prone view.
//
// Revision 1.15  2005/10/27 21:32:58  jmeade
// double-click on logo invokes website.
//
// Revision 1.14  2005/10/14 19:31:05  jmeade
// jump to cad finding on selection changes.
//
// Revision 1.13  2005/10/07 16:17:23  geconomos
// reworked cad vendor image
//
// Revision 1.12  2005/10/06 02:14:03  jmeade
// cad vendor image.
//
// Revision 1.11  2005/10/05 22:49:27  jmeade
// update w/ plenty changes.
//
// Revision 1.10  2005/10/04 18:05:55  jmeade
// update cad list after changes.
//
// Revision 1.9  2005/09/30 22:20:52  jmeade
// background bitmap for cad dialog; sorting.
//
// Revision 1.8  2005/09/28 00:35:49  jmeade
// context menu functionality in overview.
//
// Revision 1.7  2005/09/27 21:41:47  jmeade
// show/hide all findings; hide on 1st open.
//
// Revision 1.6  2005/09/23 21:01:01  jmeade
// jump to next finding; summary text.
//
// Revision 1.5  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.4  2005/09/22 16:55:53  jmeade
// comments.
//
// Revision 1.3  2005/09/21 17:16:55  jmeade
// read, display cad findings list info.
//
// Revision 1.2  2005/09/20 20:00:03  jmeade
// updated
//
// Revision 1.1  2005/09/13 21:45:14  jmeade
// cad results dialog, initial checkin.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/CadResultsDlg.h,v 1.25.2.4 2009/11/13 17:37:06 dongqing Exp $
// $Id: CadResultsDlg.h,v 1.25.2.4 2009/11/13 17:37:06 dongqing Exp $
