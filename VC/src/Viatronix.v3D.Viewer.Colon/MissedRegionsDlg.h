// $Id: MissedRegionsDlg.h,v 1.1.2.5 2010/02/16 11:43:16 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  @jmeade@viatronix.net

#include "resource.h"
#include "SkinButton.h"
#include "ReaderGlobal.h"
#include "ViatronDialog.h"
#include "WmTimer.h"

#pragma once


namespace ColonViews
{
  namespace Controls
  {
    // Missed regions list box
    class MissedRegionsListBox : public CListCtrl
    /// Class to trigger all missed regions calculations and handle the associated list control
    {
    public:
      /// constructor
      MissedRegionsListBox();

      /// Set up columns and other listbox features
      void Initialize();

      /// Override to size columns wide enough to display <lpszColumnHeading> text
      int InsertColumn(int iCol, LPCTSTR lpszColumnHeading, int iFormat = LVCFMT_LEFT, int iWidth = -1, int iSubItem = -1);

      /// Gets index of selected missed region
      int4 GetCurMissedRgn();

      /// Sets index of current missed region
      void SetCurMissedRgn(const int4 iSel);

      /// Updates the missed region listbox
      void UpdateList();

      /// Calculate the missed regions
      afx_msg void OnDetectMissedRgnsBtn();

      /// Jump to the next missed region
      afx_msg void OnNextMissedRgnsBtn();

    protected:
      /// windows message mapping
      DECLARE_MESSAGE_MAP()
      /// Called when the current selection has been changed
      afx_msg BOOL OnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);

      /// Message loop
      virtual LRESULT WindowProc(UINT_PTR uMessage, WPARAM wParam, LPARAM lParam);

      /// Timer events
      afx_msg void OnTimer(UINT_PTR uIDEvent);

    private:
      /// Dataset for the current missed region list
      ReaderLib::Dataset * m_pCurrDataset;
      /// Timer to ensure list is up-to-date
      WinUtils::WmTimer m_timerUpdate;

    }; // class MissedRegionsListBox

  } // namespace Controls


  namespace Dialogs
  {
    class MissedRegionsDlg : public ViatronDialog
    {
      /// dynamic 
      DECLARE_DYNAMIC(MissedRegionsDlg)

    public:
      /// constructor
      MissedRegionsDlg(CWnd* pParent = NULL);
      /// destructor
      virtual ~MissedRegionsDlg();

      /// Jump to the next missed region
      afx_msg void OnNextMissedRgnsBtn();

    protected:
      /// dialog data exchange
      virtual void DoDataExchange(CDataExchange* pDX);

      /// Enter/OnOK
      virtual void OnOK();

      /// Esc/OnCancel
      virtual void OnCancel();

      /// windows message mapping
      DECLARE_MESSAGE_MAP()
      /// dialog initialization
      virtual BOOL OnInitDialog();

      /// Calculate the missed regions
      afx_msg void OnDetectMissedRgnsBtn();

      /// dialog resized
      afx_msg void OnSize(UINT uType, int iCX, int iCY);

    private:
      /// update the missed regions
      void UpdateState();

    public:
      // Dialog Data
      enum { IDD = IDD_MISSED_REGIONS };

    private:
      /// list of regions display
      Controls::MissedRegionsListBox m_missedRegionList;
      /// calculate list of regions
      WinUtils::SkinButton  m_detectMissedBtn;
      /// jump to next region
      WinUtils::SkinButton  m_nextMissedBtn;

      /// Stores current dataset orientation
      ReaderLib::DatasetOrientations::TypeEnum m_eCurrOrient;

    }; // class MissedRegionsDlg

  } // namespace Dialogs

} // namespace ColonViews


// Revision History:
// $Log: MissedRegionsDlg.h,v $
// Revision 1.1.2.5  2010/02/16 11:43:16  dongqing
// free up the next missed region jump method
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.4  2010/01/31 02:07:14  dongqing
// make sure dialog doesn't disappear
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.3  2010/01/17 18:29:16  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.2  2009/12/17 16:24:03  dongqing
// comments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MissedRegionsDlg.h,v 1.1.2.5 2010/02/16 11:43:16 dongqing Exp $
// $Id: MissedRegionsDlg.h,v 1.1.2.5 2010/02/16 11:43:16 dongqing Exp $
