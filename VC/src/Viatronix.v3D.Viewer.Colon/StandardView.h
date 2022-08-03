// $Id: StandardView.h,v 1.8.2.5 2010/04/17 05:16:43 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey R. Meade (jeff@viatronix.net)


// pragmas
#pragma once


// includes
#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"
#include "OverViewDlgBar.h"


// namespaces
namespace ColonViews
{
  namespace Layouts
  {

    /**
     * Frame for the main colon view
     */
    class StandardLayoutFrame : public ChildLayoutFrame
    {
    public:

      /// Constructor
      StandardLayoutFrame();

    private:
      /// dynamic class creation
      DECLARE_DYNCREATE(StandardLayoutFrame)

    }; // class StandardLayoutFrame


    /**
     * The main colon view
     */
    class StandardLayout : public ColonViatronixWnd
    {
    public:

      /// Constructor
      StandardLayout();

      /// Resets the stored/last displayed main child view window
      static void ResetGlobalMainWindow();

      /// Is datasets locking tracked by this view
      virtual bool IsDatasetsLockingEnabled() const { return true; }

    protected:

      /// Perform view-specific initialization
      virtual void CreateChildViews(HWND hWndThis);

      /// Renders the child views
      virtual void GLRenderViews();

      /// Overridden to prevent normal setting of current ViatronStudy view window
      virtual void SetCurrentViatronStudyView();

      /// Validate the popup menu to be shown.
      virtual uint4 OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const;

      /// Notifies view window of the current progress of dataset load
      virtual void OnStudyLoadStepNotify(const ReaderLib::StudyLoad::StepsEnum eCompletedStep,
        const ReaderLib::StudyLoad::StepsEnum eCurrentStep, const ReaderLib::DatasetOrientations::TypeEnum eOrientation);

      /// Redirects a generic child view message to the main view window
      virtual void RedirectBroadcastMessage(int4 & wndTypes, const UINT uMsg) const;

      DECLARE_MESSAGE_MAP()
      /// resize message handler
      afx_msg virtual void OnSize(UINT uType, int iCX, int iCY);
      /// mouse move
      afx_msg void OnMouseMove(UINT uFlags, CPoint point);
      /// msg: sets the main view in the layout
      afx_msg LRESULT OnSetMainViewWnd(WPARAM newMainWndID, LPARAM);
      /// msg: gets the main view in the layout
      afx_msg LRESULT OnGetMainViewWndID(WPARAM, LPARAM);
      /// msg: whether to allow toggling between datasets
      afx_msg LRESULT OnQueryEnableSupineProneSwitch(WPARAM, LPARAM);
      /// msg: on 2d flight
      afx_msg LRESULT OnUpdate2DAutoFlight(WPARAM, LPARAM);
      /// msg: add snapshot to report
      afx_msg void OnReportAddSnapshot();
      /// msg: get ID of view that is being recorded
      afx_msg LRESULT OnMovieRecordGetViewID(WPARAM, LPARAM);
      /// msg: toggle autofly mode
      afx_msg void OnToggleAutofly();
      /// msg: for various childview messages
      virtual afx_msg LRESULT OnAppChildViewMessage(WPARAM wParam, LPARAM lParam);

      afx_msg void OnTabChanged( NMHDR * pNMHDR, LRESULT * pResult );

    protected:
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_STANDARDVIEW_LANDSCAPE; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return ( m_bBarTop ? IDT_STANDARDVIEW_WIDESCREEN : IDT_STANDARDVIEW_WIDESCREEN_BARTOP); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_STANDARDVIEW; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDT_STANDARDVIEW_PORTRAIT_LARGE; }

      /// Vector positions for this window's child views
      enum ePos { ePOS_OVERVIEW = 0, ePOS_CROSSSECTION, ePOS_NAVIGATION, ePOS_AXIAL, ePOS_SAGITTAL, ePOS_CORONAL };

    private:

      /// Current child view (displayed as center view)
      int4 m_iCurrMainWnd;

      /// Stores the current (or last displayed) main child view
      static int4 m_iGlobalMainWnd;

      /// store the last time the navigation view was updated (for optimizing interactivity during 2D slicing)
      Timer m_lastNavigationUpdateTimer;

      /// store the last time the overview was updated (for optimizing interactivity during 2D slicing)
      Timer m_lastOverviewUpdateTimer;

			bool m_bBarTop;
    }; // class StandardLayout

  } // Layouts

} // ColonViews

