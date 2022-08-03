// $Id: SupineProneViewWnd.h,v 1.9.2.5 2010/04/17 05:16:41 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Interface for the SupineProneColonWnd class.
// Owner: Jeff Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include "ChildViewFrame.h"
#include "ViatronViewWnd.h"
#include "SkinButton.h"

// forward declarations
namespace ColonViews { namespace ChildViews { class SliceChildView; } }

// classes
namespace ColonViews
{

  namespace Layouts
  {

    /**
     * Dual-overview frame window
     */
    class DualOverviewLayoutFrame : public ChildLayoutFrame
    {
    public:

      /// default constructor
      DualOverviewLayoutFrame();

      DECLARE_DYNCREATE(DualOverviewLayoutFrame)
    }; // class DualOverviewLayoutFrame


    /**
     * Dual-orthogonal frame window
     */
    class DualOrthogonalLayoutFrame : public ChildLayoutFrame
    {
    public:

      /// default constructor
      DualOrthogonalLayoutFrame();

      DECLARE_DYNCREATE(DualOrthogonalLayoutFrame)
    }; // class DualOrthogonalLayoutFrame


    /**
     * Dual-view window, common code for dual-orthogonal and dual-supine views
     */
    class SupineProneLayout : public ColonViatronixWnd
    {
      // enums
    public:

      // operating modes for this layout
      enum SPLayoutTypeEnum { eSPL_ORTHOGONAL = 1, eSPL_OVERVIEW = 2 };

      // member functions
    public:

      /// constructor
      SupineProneLayout( const SPLayoutTypeEnum & eLayoutType = eSPL_OVERVIEW, bool bSupportsCAD = true );

      /// destructor
      ~SupineProneLayout();

    public:

      /// Is datasets locking tracked by this view
      virtual bool IsDatasetsLockingEnabled() const;

      /// this view displays multiple datasets
      virtual bool IsDisplayingMultipleDatasets() const { return true; }

    protected:

      /// create event handler
      afx_msg virtual int OnCreate( LPCREATESTRUCT lpCreateStruct );

      /// destroy event handler
      afx_msg virtual void OnDestroy();

      /// creates the child views
      void CreateChildViews( HWND hWndThis );

      /// called on idle
      virtual void GLIdle();

      /// called when the child views need to be rendered
      virtual void GLRenderViews();

      /// called at view initialization
      virtual void OnStudyLoadStepNotify( const ReaderLib::StudyLoad::StepsEnum eCompletedStep,
        const ReaderLib::StudyLoad::StepsEnum eCurrentStep, 
        const ReaderLib::DatasetOrientations::TypeEnum eOrientation );

      /// called when the mouse is moved over the view
      afx_msg void OnMouseMove( UINT uFlags, CPoint point );

      /// called when the view is being resized
      afx_msg void OnSize( UINT uType, int iSizeX, int iSizeY );

      /// called when a snapshot is being requested
      afx_msg void OnReportAddSnapshot();

      /// called when 2d autofly is toggled
      afx_msg void OnToggleAutofly();

      /// called when the "linked" button is clicked
      afx_msg void OnLinkViewsClicked();

      /// gets the id of the view that will be recorded
      LRESULT OnMovieRecordGetViewID(WPARAM, LPARAM);

      /// indicates that the supine/prone button should be enabled in the PatientInfo dialog
      LRESULT OnQueryEnableSupineProneSwitch(WPARAM, LPARAM);

      /// mfc message map
      DECLARE_MESSAGE_MAP()

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID() { return (m_eSupineProneLayoutType== eSPL_OVERVIEW ? IDT_DUALOVERVIEW_LANDSCAPE : (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUALORTHOGONAL_LANDSCAPE : IDT_SINGLEORTHOGONAL_LANDSCAPE) ); }
      /// returns the resource ID for indicated window orientation
			virtual uint4 GetWidescreenLayoutID() { return (m_eSupineProneLayoutType == eSPL_OVERVIEW ? IDT_DUALOVERVIEW_WIDESCREEN : (ReaderLib::rdrGlobal.m_bDualDataDetected ? (m_bToolbarAtBottom ? IDT_DUALORTHOGONAL_WIDESCREEN : IDT_DUALORTHOGONAL_WIDESCREEN_BARTOP) : IDT_SINGLEORTHOGONAL_WIDESCREEN) ); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return (m_eSupineProneLayoutType == eSPL_OVERVIEW ? IDT_DUALOVERVIEW : (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUALORTHOGONAL : IDT_SINGLEORTHOGONAL) ); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID()   { return (m_eSupineProneLayoutType == eSPL_OVERVIEW ? IDT_DUALOVERVIEW : (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUALORTHOGONAL_PORTRAIT_LARGE : IDT_SINGLEORTHOGONAL_PORTRAIT_LARGE) ); }

      /// handles datasets locked changes
      virtual void OnDatasetsLocked( const Colon::DatasetsLockedArgs & args );

      // member variables
    private:

      // child view identifiers
      enum ViewIDEnum { eVIEWNONE = 0, eVIEWLEFT = 1, eVIEWRIGHT = 2, eVIEWBOTH = 3, eVIEWOVERVIEW = 4, eVIEWALL = 7 };

      // indicates which child views to render
      ViewIDEnum m_eDrawViews;

      // indicates the last active child view
      ViewIDEnum m_eLastActive;

      // left slice child view 
      ColonViews::ChildViews::SliceChildView * m_pSliceChildViewLeft;

      // right slice child view
      ColonViews::ChildViews::SliceChildView * m_pSliceChildViewRight;

      /// was the 2D slice camera position indicator on when the layout was invoked
      bool m_bWas2DCameraShowingWhenEnteringLayout;

      /// identifies type of child views contained in the layout
      const SPLayoutTypeEnum m_eSupineProneLayoutType;

      /// bypass the lookup
      bool m_bBypassDatasetLockingCheck;

			/// flag for toobar position of the layout
			bool m_bToolbarAtBottom;

    }; // class SupineProneLayout.

  } // namespace Layouts

} // namespace ColonViews
