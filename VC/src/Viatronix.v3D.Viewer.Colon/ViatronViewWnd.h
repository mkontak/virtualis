// $Id: ViatronViewWnd.h,v 1.21.2.9 2010/04/15 14:28:01 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Interface for the ColonViatronixWnd class, for Colon application-specific workflow
// Owner: Jeff Meade  jmeade@viatronix.net


#if !defined(AFX_COLONVIATRONIXWND_H__92D26796_9636_11D4_B6EA_00D0B7BF4092__INCLUDED_)
#define AFX_COLONVIATRONIXWND_H__92D26796_9636_11D4_B6EA_00D0B7BF4092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <utility>
#include <list>

#include "ViatronixWnd.h"
#include "StudyLoad.h"
#include "ControlsDialog.h"
#include "PatientInfoDlg.h"
#include "WmTimer.h"
#include "File.h"
#include "MovieCreateDialog.h"
#include "MovieCapture.h"
#include "ObservableObject.h"
#include "ApplicationController.h"
#include "CadInputMonitor.h"
#include "FullScreenController.h"

namespace ColonViews
{

  namespace Layouts
  {

    class ColonViatronixWnd : public ViatronixWnd, public IObserver< Colon::DatasetsLockedArgs >, public IObserver< Colon::FullScreenArgs >
    {
      /**
       * Class to assist in monitoring the execution of an external process
       */
      class v3DViewerExternalProcess
      {
      public:
        /// Constructor
        v3DViewerExternalProcess(HWND hWndViewer, HANDLE hProcess, int4 iPostProcessCmdShow = SW_RESTORE);
        /// Starts a thread that will monitor an external process
        static bool BkgdMonitorProcess(HWND hWndViewer, HANDLE hProcess, int4 iPostProcessCmdShow);

      private:
        /// Waits for an external process to complete execution.
        static UINT MonitorProc(LPVOID lpData);

      public:
        /// Window to show after process completes.
        HWND m_hWndViewer;
        /// Process to monitor
        HANDLE m_hProcess;
        /// ShowWindow() command (e.g. SW_SHOW, SW_RESTORE) executed on viewer after process completes
        int4 m_iPostProcessCmdShow;
      }; // v3DViewerExternalProcess

    public:
      enum PrivateMessagesEnum_ColonViatronixWnd
      {
        // Toggle mode on/off
        WM_USER_MOVIE_RECORD_MODE_TOGGLE = WM_USER + 100,
        // Query whether or not a movie is recording
        WM_USER_MOVIE_RECORD_MODE_QUERY,
        // Query for the view to record (at start of recording)
        WM_USER_MOVIE_RECORD_GET_VIEW_ID
      };

    protected:

      // some layout constants
      enum
      {
        /// the gap between images on the layout
        LAYOUT_GAP = 5,

        /// the width of the floating panes
        LAYOUT_WIDTH_PANES = 327,

        /// the thickness of a scrollbar
        LAYOUT_SCROLLBAR_THICKNESS = 12,

        /// the height of the buddy window
        LAYOUT_BUDDY_HEIGHT = 19,

      }; // enum

    protected:
      /// Used to monitor process currently converting a movie file, and move file to study directory file upon completion
      /// Note: Helper class, contains no methods, only stores values; hence doesn't warrant a separate module.
      class MovieConvertProcess
      {
      public:
        /// Constructor
        MovieConvertProcess(const vx::File& src, const vx::File& dst, const HANDLE hProc, const Movies::MovieFormat& fmt)
          : m_movieSrc(src), m_movieDest(dst), m_hConvertProcess(hProc), m_format(fmt) { }
      public:
        /// File path of movie upon completion of conversion.
        vx::File m_movieSrc;
        /// File path to where completed movie should be copied.
        vx::File m_movieDest;
        /// Handle to external process converting movie file (i.e. writing m_convertSrc).
        HANDLE m_hConvertProcess;
        /// Format options for the movie
        Movies::MovieFormat m_format;
      };

      enum
      {
        // Frame rate of converted movie
        eMovie_FrameRate = 20,
        // The number of movie frames that make up the opening credits
        eOpeningCreditsLength = 30,
        // The amount of time that must elapse when flight halts before an equivalent number
        //  still frames will be added to a movie
        ePauseFrameElapseTimeMS = 2000,
      };

    public:

      /// Default constructor
      ColonViatronixWnd( bool bSupportsCAD = true, const uint4 uBackgroundImageBitmapID = 0);
      // NOTE (JRM): Not convinced this is better than: ColonViatronixWnd(const uint4 uBackgroundImageBitmapID = IDB_V3DCOLON_BACKGROUND);

      /// Destructor
      virtual ~ColonViatronixWnd();

      /// Perform view-specific initialization
      virtual void CreateChildViews(HWND hWndThis);

      /// Is datasets locking tracked by this view
      virtual bool IsDatasetsLockingEnabled() const { return false; }

      /// gets camera coordinates and orientation from which you can view a given point
      static void GetCameraAngleFromWorldPt(ReaderLib::Dataset & rDataset, const Point<float4> & ptWorld, 
        Point<float4> & outCamLoc, Normal<float4> & outCamOrient, Vector<float4> & outCamUp);

      /// Returns true if displaying 2D measurement profile data, false otherwise
      static bool IsDisplaying2DMeasurementProfile() { return m_bDisplaying2DMeasurementProfile;  }
      /// Prompts user and deletes list of bookmarks from the specified dataset(s)
      static bool DeleteAllBookmarks(const ReaderLib::DatasetOrientations::TypeEnum eDatasetPos);

      /// Sets autofly mode for the current dataset
      static void SetAutoflyMode(const bool bNewMode);
      /// Toggles the autofly mode for the current dataset on/off
      static void ToggleAutoflyMode();

      /// returns text equivalent of the orientation of the given dataset
      static std::string PatientOrientationToString(const ReaderLib::Dataset & rDataset, const bool bAbbrev = false);
      /// returns text equivalent of the orientation of the given dataset
      static std::string PatientOrientationToString(const SerializedDicom::PatientOrientation ePatOrient,
        const ReaderLib::Dataset::DataModality eMod = ReaderLib::Dataset::CT, const bool bAbbrev = false);
      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ColonViatronixWnd)
    protected:
      //}}AFX_VIRTUAL
      virtual void OnDestroy();

      // Generated message map functions
    protected:
      //{{AFX_MSG(ColonViatronixWnd)
      virtual afx_msg void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);
      virtual afx_msg void OnKeyUp(UINT uChar, UINT uRepCnt, UINT uFlags);
      virtual afx_msg void OnSize(UINT uType, int iSizeX, int iSizeY);
      virtual afx_msg void OnTimer(UINT uIDEvent);

      afx_msg void OnOptionsToggleMeasurementProfileDisplay();
      afx_msg void OnUpdateOptionsToggleMeasurementProfileDisplay(CCmdUI* pCmdUI);
      afx_msg void OnUpdateOptionsColorSchemes(CCmdUI* pCmdUI);
      afx_msg void OnOptionsColorscheme1();
      afx_msg void OnOptionsColorscheme2();
      afx_msg void OnUpdateBookmarksList(CCmdUI* pCmdUI);
      afx_msg void OnUpdateBookmarksAddBookmark(CCmdUI* pCmdUI);
      afx_msg void OnUpdateBookmarksDeleteAllBookmarks(CCmdUI* pCmdUI);
      afx_msg void OnUpdateMeasurementsDeleteAll(CCmdUI* pCmdUI);
      afx_msg void OnAnnotationsDeleteAllAnnotations();
      afx_msg void OnUpdateAnnotationsDeleteAllAnnotations(CCmdUI * pCmdUI);
      afx_msg void On2DAnnotationsEditQuickDropName();
      afx_msg void OnToggleReportCompleteMark();
      afx_msg void OnUpdateToggleReportCompleteMark(CCmdUI* pCmdUI);
      afx_msg void OnOptionsDisplayRenderSpeed();
      afx_msg void OnUpdateOptionsDisplayRenderSpeed(CCmdUI* pCmdUI);
      afx_msg void OnUpdateNotationsAnnotationList(CCmdUI* pCmdUI);
      afx_msg void OnUpdateNotations3DMeasurements(CCmdUI* pCmdUI);
      afx_msg void OnViewWindowPseudoColorDisplay();
      afx_msg void OnUpdateViewWindowPseudoColorDisplay(CCmdUI* pCmdUI);
      afx_msg void OnViewWindowShowCleansed();
      afx_msg void OnUpdateViewWindowShowCleansed(CCmdUI* pCmdUI);
      afx_msg void OnToggleScrollbarOrientation();
      afx_msg virtual void OnUpdateToggleScrollbarOrientation(CCmdUI* pCmdUI);
      afx_msg void OnViewWindowArrowInSliceCenter();
      afx_msg virtual void OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI);
      afx_msg void OnViewWindowSwitchFlightDirection();
      afx_msg virtual void OnUpdateViewWindowSwitchFlightDirection(CCmdUI* pCmdUI);
      afx_msg void OnViewWindowUpdateEndoluminalView();
      afx_msg virtual void OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI);
      afx_msg void OnOptionsAlwaysShowCadIndicators();
      afx_msg virtual void OnUpdateOptionsAlwaysShowCadIndicators(CCmdUI* pCmdUI);
      afx_msg void OnOptionsShowExtraColonicFindings();
      afx_msg virtual void OnUpdateOptionsShowExtraColonicFindings(CCmdUI* pCmdUI);
      afx_msg void OnSrtOptionsDisplayRegionsOfInterest();
      afx_msg virtual void OnUpdateSrtOptionsDisplayRegionsOfInterest(CCmdUI* pCmdUI);
      //}}AFX_MSG
      /// Adds a new bookmark
      afx_msg void OnBookmarksAddBookmark();
      /// Delete all bookmarks
      afx_msg void OnBookmarksDeleteAllBookmarks();
      /// Delete all 3D measurements
      afx_msg void OnMeasurementsDeleteAll();
      /// Jumps to a selected bookmark
      afx_msg void OnBookmarksGotoBookmark(UINT uCmdID);
      /// Jumps to a selected annotation
      afx_msg void OnNotationsAnnotationGotoAnnotation(UINT uCmdID);
      /// Jumps to a selected measurement
      afx_msg void OnNotations3DMeasurementsGotoMeasurement(UINT uCmdID);
      /// Window create handler
      afx_msg virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
      /// Handles notification that a new step in study loading has begun
      afx_msg LRESULT OnAppStudyLoadStepNotify(WPARAM iPrevAndNextStep, LPARAM iOrientation);
      /// Refreshes any overviews
      /// Handles notification that the current view mode has changed
      afx_msg LRESULT OnAppViewModeChanged(WPARAM, LPARAM);
      /// Jumps to next missed region
      afx_msg LRESULT OnAppJumpNextMissedRegion(WPARAM iPrevORnext, LPARAM);
      /// Finds missed regions
      afx_msg LRESULT OnAppDetectMissedRegions(WPARAM, LPARAM);
      /// Handles msg for selecting a missed region
      afx_msg LRESULT OnAppSelectMissingPatch(WPARAM iSelection, LPARAM);
      /// Handles msg to refresh/update display of lumen coverage
      afx_msg LRESULT OnAppUpdateLumenCoverageMarking(WPARAM, LPARAM);
      /// WM_MOUSEMOVE handler
      afx_msg virtual void OnMouseMove(UINT uFlags, CPoint point);
      /// Handles message to add a report snapshot (i.e. of current view)
      afx_msg void OnReportAddSnapshot();
      /// Updates the add snapshot menu item
      afx_msg void OnUpdateReportAddSnapshot(CCmdUI* pCmdUI);
      /// Toggles movie recording mode on or off
      afx_msg LRESULT OnMovieRecordModeToggle(WPARAM,LPARAM);
      /// Returns status of movie recording mode.
      afx_msg LRESULT OnMovieRecordModeQuery(WPARAM, LPARAM);
      /// Gets the view wnd ID of the main view
      afx_msg LRESULT OnGetMainViewWndID(WPARAM, LPARAM);
      /// Update 2D flight for overview/slices
      afx_msg LRESULT OnUpdate2DAutoFlight(WPARAM, LPARAM);
      /// synchronize 3d location between datasets
      afx_msg LRESULT ColonViatronixWnd::OnSyncronizeDatasetLocations(WPARAM wpSrcOrientation, LPARAM lpDestOrientation );

      /// Accelerator message, jumps to previous segment
      afx_msg void OnJumpPrevSegment();
      /// Accelerator message, jumps to next segment
      afx_msg void OnJumpNextSegment();
      /// Accelerator message, toggles between supine and prone datasets
      afx_msg void OnToggleSupineProne();
      /// Accelerator message, toggles autofly on/off
      afx_msg void OnToggleAutofly();
      /// Accelerator message, toggles line measure mode on/off
      afx_msg void OnToggleLineMeasure();
      /// Accelerator message, toggles ROI measure mode on/off
      afx_msg void OnToggleRoiMeasure();
      /// Accelerator message, toggles lighting mode on/off
      afx_msg void OnToggleLighting();
      /// Accelerator message, toggles translucent on/off
      afx_msg void OnToggleTranslucent();
      /// Accelerator message, toggles annotation mode on/off
      afx_msg void OnToggleAnnotationMode();
      /// Accelerator message, toggles segment polyp mode
      afx_msg void OnToggleSegmentPolypMode();
      /// Accelerator message, toggles view direction
      afx_msg void OnToggleViewDirection();
      /// Accelerator message, jumps to next missed region
      afx_msg void OnNextMissedRegion();
      /// Display the flight control pad
      afx_msg void OnControlPadFlightControls();
      /// Update routine for flight control pad menu item
      afx_msg void OnUpdateControlPadFlightControls(CCmdUI* pCmdUI);
      /// Display the adjust translucency controls
      afx_msg void OnControlPadAdjustTranslucency();
      /// Update routine for adjust translucency pad menu item
      afx_msg void OnUpdateControlPadAdjustTranslucency(CCmdUI* pCmdUI);
      /// display the missed region dialog
      afx_msg void OnOptionsControlPadShowMissedRegions();
      /// show the cad results dialog
      afx_msg void OnOptionsControlPadShowCADResults();
      /// Update routine for the menu options
      afx_msg void OnUpdateOptionsControlPadMissedCAD(CCmdUI* pCmdUI);
      /// tooltips for controls
      virtual BOOL OnToolTipNeedText(UINT uID, NMHDR *pTTTStruct, LRESULT *pResult);
      DECLARE_MESSAGE_MAP()

      /// Select between the dialogs
      void OnMissedRegions_And_CadResults_Select(const uint4 uID);

      /// handles datasets locked changes
      virtual void OnDatasetsLocked( const Colon::DatasetsLockedArgs & args ) {}

    protected:
      /// Render all OpenGL Views
      virtual void GLRenderViews();
      /// Overridden to send paint messages to Viatron parent controls
      virtual void RenderControls();
      /// Override to perform any necessary idle time processing
      virtual void GLIdle();

      /// Notifies view window of the current progress of study load
      virtual void OnStudyLoadStepNotify(const ReaderLib::StudyLoad::StepsEnum eCompletedStep,
        const ReaderLib::StudyLoad::StepsEnum eCurrentStep, const ReaderLib::DatasetOrientations::TypeEnum eOrientation);

      /// Sets the current view as the current ViatronStudy view (i.e. calls ViatronStudy::SetCurrentView())
      /// NOTE:  Override if current view is not to be set normally, e.g. by mouse movement.
      virtual void SetCurrentViatronStudyView();

      /// Add a report entry with the given list of snapshots to the current report (displays only the noted views in reverse video during capture)
      void AddReportSnapshot(const std::list<GLChildViewPtr>& snapshotList, const std::list<GLChildViewPtr>& inverseImgList, const MeasureLineList& currMeasureList = MeasureLineList());
      /// Add a report entry with the given list of snapshots to the current report (displays each view in reverse video during capture)
      void AddReportSnapshot(const std::list<GLChildViewPtr>& childViewList, const MeasureLineList& currMeasureList = MeasureLineList());

      /// Ends saving movie frames and creates movie file for the specified view.
      void RecordMovie(const GLChildView::GLVID & childRecordingMovieID, const Movies::MovieFormat& fmt = Movies::MovieFormat());
      /// Save the current viewpoint, mode information, etc. in a buffer.
      static void StoreMovieFrameInfo(ColonViatronixWnd* pWnd);
      /// Save movie to an avi file and convert to a more appropriate media file
      bool CreateMovieFile(CBitmap& titleFrameBmp, bool (*pProgressCallbackFn)(const int4), Movies::MovieFormat fmt = Movies::MovieFormat());

      /// Returns eBOTH if both datasets are visible (on the current view), or indicates which sole dataset is visible
      static ReaderLib::DatasetOrientations::TypeEnum GetVisibleDatasets(ViatronixWnd& parentWnd);

      /// Copies a locally-converted movie to study location and deletes local temp files (avi and wmv)
      static UINT MoveConvertedMovie(LPVOID pParam);

      /// Toggles a given view mode
      virtual void ToggleViewMode(const ViewModes::VIEWMODES & eMode);

      ///Handles the SRT toggling
      void OnToggleSegmentRegion();

    private:

      /// Updates a given menu with items from the specified list
      template<class T>
      int4 UpdateMenuList(const std::list<T>& itemList, CCmdUI& cmdUI, const uint4 uFirstMenuID, const uint4 uMaxNumItems);

      /// Method to invoke the StoreMovieFrameInfo() method of the given ColonViatronixWnd instance
      static uint4 Run_StoreMovieFrameInfo(void * pColonViatronixWnd);

      /// Do the final job to mark the report complete: session will be marked as read only too
      bool CompleteReport();

      /// Remark the completed report as incomplete
      bool FlipCompleteReport();

    public:
      /// Returns true if set of running processes is non-empty
      static bool IsAnExternalProcRunning() { return !m_runningProcSet.empty(); }
      /// Set of handles to any launched external processes
      static std::set<HANDLE> m_runningProcSet;

      /// notification handler for datasets locked change 
      void OnNotified( const Colon::DatasetsLockedArgs & args );

      /// notification handler for fullscreen change 
      void OnNotified( const Colon::FullScreenArgs & args );

    protected:
      /// Main view will be the last view in which the user has interacted
      GLChildView::GLVID m_idMainView;

      /// movie capture parameters
      static Movies::MovieCapture m_movie;
      /// timer used in movie frame capture
      static Timer m_movieTimer;

    private:
      /// current/last view containing mouse
      GLChildViewPtr m_pViewMouseLastIn;

      /// Whether or not to display 2D measurement profile for the current user
      static bool m_bDisplaying2DMeasurementProfile;

      /// ID of current child recording movie, or zero if none
      GLChildView::GLVID m_childRecordingMovieID;

      /// Pause in movie flight frame timer
      WinUtils::WmTimer m_timerPauseInMovie;

      /// Handle to end save movie frame background thread
      HANDLE m_hSaveFrame;

      /// Whether 2D will update during 3D flight
      bool m_bSynchronous3D2DFlight;

      /// cad input monitor
      CadInputMonitor m_cadMonitor;

    }; // ColonViatronixWnd
    /// helper typedef
    typedef ColonViatronixWnd *ColonViatronixWndPtr;

  } // Layouts

} // ColonViews

#endif // !defined(AFX_COLONFRAMEWND_H__92D26796_9636_11D4_B6EA_00D0B7BF4092__INCLUDED_)

// $Log: ViatronViewWnd.h,v $
// Revision 1.21.2.9  2010/04/15 14:28:01  dongqing
// toggle between missed regions and cad results on some layouts
//
// Revision 1.21.2.8  2010/01/31 02:13:56  dongqing
// support for vertical lumen coverage progress bar; toggle missed regions/cad dialogs
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.7  2009/12/09 19:40:08  dongqing
// layouts will specify various IDs for different orientations, and the baseclass will decide which orientation is needed.
// somewhat similar to the IDD= enum using in mfc dialogs
//
// Revision 1.21.2.6  2009/10/07 16:13:17  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.21.2.5  2009/01/15 04:06:26  jmeade
// jump to 3D measurements implemented
//
// Revision 1.21.2.4  2009/01/07 01:39:02  jmeade
// volume measure interface changes.
//
// Revision 1.21.2.3  2008/02/07 19:31:10  jmeade
// removed hard-coded dialog sizes.
//
// Revision 1.21.2.2  2008/01/16 01:20:32  jmeade
// Issue 5917: support for vertical scrollbars throughout application.
//
// Revision 1.21.2.1  2008/01/11 02:18:55  jmeade
// issue 5907.
//
// Revision 1.21  2007/03/07 22:35:30  jmeade
// code standards.
//
// Revision 1.20  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.19  2007/01/03 06:24:06  jmeade
// future ud segments layout.
//
// Revision 1.18  2006/10/04 21:13:09  jmeade
// allow parent to handle view mode update.
//
// Revision 1.17  2006/09/07 23:55:16  jmeade
// enable segment polyp mode.
//
// Revision 1.16  2006/08/31 20:03:08  jmeade
// Issue 4946: text in non-distended regions.
//
// Revision 1.15  2006/08/23 21:20:53  jmeade
// Issue 4946: new strings for decubitus studies.
//
// Revision 1.14  2006/07/06 15:59:55  jmeade
// Read patient position from volume header.
//
// Revision 1.13  2006/06/30 20:34:31  jmeade
// Read patient position from volume header.
//
// Revision 1.12  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.11  2006/02/17 18:32:11  jmeade
// add preview image along with movie file.
//
// Revision 1.10  2006/02/07 23:34:37  jmeade
// code review: comments.
//
// Revision 1.9  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.8  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.7.2.6  2006/05/22 12:31:07  geconomos
// made destructor virtual
//
// Revision 1.7.2.5  2006/03/24 23:45:36  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.7.2.4  2006/03/22 22:55:31  jmeade
// Issue 4667: restrict registration synchronization to views where both datasets are visible, and a link button is available.
//
// Revision 1.7.2.3  2006/03/08 00:17:33  jmeade
// Issue 4620: Shortcut key for annotation mode.
//
// Revision 1.7.2.2  2006/01/11 16:45:16  frank
// added more layout constants
//
// Revision 1.7.2.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.7  2005/10/28 13:29:38  geconomos
// modified to capture movie frames in idle loop
//
// Revision 1.6  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.5  2005/10/12 22:38:37  jmeade
// option to always show cad findings arrows in 3D
//
// Revision 1.4  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.35.2.17.2.9  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.35.2.17.2.8  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.35.2.17.2.7.2.1  2005/06/13 19:50:15  jmeade
// Set full oblique in movie view.
//
// Revision 3.35.2.17.2.7  2005/05/31 22:44:04  jmeade
// add snapshot method, default fn.
//
// Revision 3.35.2.17.2.6  2005/04/28 17:20:00  jmeade
// Issue 4147: Fix bugs to movie frame capture introduced by new framework changes.
//
// Revision 3.35.2.17.2.5  2005/04/08 18:23:40  frank
// Issue #4113: Disabled menu items in verification view
//
// Revision 3.35.2.17.2.4  2005/04/08 13:34:10  frank
// Issue #4049: Added mouse text for overview images in sliding panes
//
// Revision 3.35.2.17.2.3  2005/03/31 17:28:29  jmeade
// Issue 4038: Generalized idle time processing.
//
// Revision 3.35.2.17.2.2  2005/03/26 19:46:05  frank
// revamped main application idle/processing loop - checkin for Jeff
//
// Revision 3.35.2.17.2.1  2005/03/12 14:36:03  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.35.2.15.2.4  2005/02/23 03:17:26  jmeade
// Stoolball and air bubble colon finding classifications.
//
// Revision 3.35.2.15.2.3  2005/02/11 15:40:58  jmeade
// Issue 3881: Store current dataset with each frame.
//
// Revision 3.35.2.15.2.2  2005/01/24 16:36:32  jmeade
// Separate movie-creation view, incremental checkin.
//
// Revision 3.35.2.15.2.1  2005/01/20 21:28:14  jmeade
// Separate movie view, incremental checkin.
//
// Revision 3.35.2.15  2004/09/23 17:46:19  jmeade
// Toggle between adjust translucency and flight controls in menu.
//
// Revision 3.35.2.14  2004/08/19 18:19:36  jmeade
// Allow child windows to handle their own "Invalidate" calls.
//
// Revision 3.35.2.13  2004/05/05 23:18:33  jmeade
// Incremental update.
//
// Revision 3.35.2.12  2003/06/11 14:52:10  jmeade
// footer.
//
// Revision 3.35.2.11  2003/05/05 21:33:23  jmeade
// Comments.
//
// Revision 3.35.2.10  2003/04/24 16:33:16  jmeade
// Issue ???:  Space leaping toggle state identified in movies.
//
// Revision 3.35.2.9  2003/03/12 20:34:30  jmeade
// Issue 2827:  Allow user to select which view to capture for movie.
//
// Revision 3.35.2.8  2003/03/07 19:28:31  jmeade
// Issue 2877:  Allow caller to AddReportSnapshot() to specify which views to display in reverse video.
//
// Revision 3.35.2.7  2003/03/07 05:33:45  jmeade
// Issue 2878:  Delete all for bookmarks and annotations based on which datasets are visible.
//
// Revision 3.35.2.6  2003/02/27 03:32:16  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.35.2.5  2003/02/22 19:06:41  jmeade
// Issue 2837:  Install non-overriden routine to capture movie frames during recording.
//
// Revision 3.35.2.4  2003/02/22 12:23:51  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.35.2.3  2003/02/21 18:09:15  jmeade
// Removed an essential identical app message; Method to update bookmark menu generalized to work for any std::list.
//
// Revision 3.35.2.2  2003/02/19 22:38:01  jmeade
// Issue 2842:  Implemented variable number of snapshots per report entry.
//
// Revision 3.35.2.1  2003/02/19 07:30:31  jmeade
// Issue 2848:  Localizing view mode changing code to ViatronViewWnd.
//
// Revision 3.35  2003/02/10 17:22:30  jmeade
// Heading.
//
// Revision 3.34  2002/12/20 21:51:26  jmeade
// Provide multiple movie format options.
//
// Revision 3.33  2002/12/12 22:38:45  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.32  2002/11/15 00:45:28  jmeade
// No hard-coding of # of child views.
//
// Revision 3.31  2002/11/11 16:15:17  kevin
// Jeffs first attempt to get neighbroview in registration sliding panes control
//
// Revision 3.30  2002/09/11 20:32:08  jmeade
// Monitor movie conversion (external) process, and move to study dir when complete.
//
// Revision 3.29  2002/09/11 18:23:28  jmeade
// Transfer movie file in bkgd thread (so as to not pause current thread on long transfer); exception handling on a couple thread functions.
//
// Revision 3.28  2002/09/11 15:44:25  jmeade
// Create and convert movies in local temp folder, then transfer to study location.
//
// Revision 3.27  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.26  2002/09/05 17:03:04  jmeade
// Comments.
//
// Revision 3.25  2002/08/27 18:13:39  jmeade
// Store template parent and controls client rects instead of repeated reloading
//  of template wnd on size messages; Unused app message.
//
// Revision 3.24  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.22  2002/07/26 22:55:16  jmeade
// Override for specific rendering of controls (in m_vpControls list).
//
// Revision 3.21  2002/07/03 03:23:43  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.20  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.19  2002/05/09 20:54:16  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.18  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.17  2002/04/12 02:17:57  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.16  2002/03/21 02:11:41  jmeade
// Display render speed service menu option.
//
// Revision 3.15  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.14.2.18  2002/08/05 23:47:50  jmeade
// Issue 2591:  Added default handlers for main view window identification in ViatronViewWnd.
//
// Revision 3.14.2.17  2002/08/01 14:46:47  jmeade
// Issue 2587:  Made a number of 2D slice options global.
//
// Revision 3.14.2.16  2002/06/19 21:00:43  jmeade
// Issue 2453:  Identify in which study to delete all bookmarks.
// Issue 2454:  Stray semi-colon after if-statement.
//
// Revision 3.14.2.15  2002/06/18 22:06:44  jmeade
// Issue 2454:  Only SetFocus() when no known processes, spawned by Viewer, as running;
// monitor external process for snapshot-to-file functionality.
//
// Revision 3.14.2.14  2002/06/17 17:59:16  jmeade
// Issue 2453:  Add bookmark override in base, enabled for any view with an overview.
//
// Revision 3.14.2.13  2002/05/31 16:15:08  jmeade
// Issue 2331:  Moved snapshot of a view to parent class.
//
// Revision 3.14.2.12  2002/05/25 05:36:32  jmeade
// Issue 2270:  Select annotation to jump to frorm (menu) list of annotations.
//
// Revision 3.14.2.11  2002/05/23 17:09:09  jmeade
// Issue 2270:  2D annotations delete all menu items.
//
// Revision 3.14.2.10  2002/05/21 20:17:03  jmeade
// Issue 2271:  "Delete all" menu items for measurements and bookmarks.
//
// Revision 3.14.2.9  2002/04/27 00:20:57  jmeade
// Issue 2063:  Find and refresh all overviews on WM_APP_REFRESH_OVERVIEW.
//
// Revision 3.14.2.8  2002/04/24 01:01:03  jmeade
// Issue 2044:  Set current ViatronStudy modes view based on mouse focus (GetCurrentView()).
//
// Revision 3.14.2.7  2002/03/28 22:35:15  jmeade
// Display render speed service menu option.
//
// Revision 3.14.2.6  2002/03/01 20:10:42  jmeade
// Hack: check for completed wmv with a timer after starting conversion.
//
// Revision 3.14.2.5  2002/02/27 22:19:45  jmeade
// Issue 1629:  Toggle report complete mark; Comments.
//
// Revision 3.14.2.4  2002/02/26 16:54:16  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.14.2.3  2002/02/22 20:06:03  binli
// issue 1543: 1> duplicate playsound in case of failed first
//      2> visual cue: 'snapshot' cursor. To be improved.
//
// Revision 3.14.2.2  2002/02/21 18:12:06  jmeade
// Issue 1910:  Push handlers for bookmark menu to ViatronViewWnd; enable bookmarks for Endo-Ortho view.
//
// Revision 3.14.2.1  2002/02/06 23:52:41  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.14  2001/01/01 09:55:49  jmeade
// No more need for a null ViatronViewWnd class (no longer an abstract class).
//
// Revision 3.13  2002/01/23 21:46:19  binli
// issue 1589: Endo-SliceView: take snapshot from all 3 views now.
//
// Revision 3.12  2002/01/10 19:24:09  jmeade
// Removed the movie cursor (it was a bad idea).
//
// Revision 3.11  2002/01/09 16:17:06  jmeade
// Movie record button implemented; Coding standards, comments.
//
// Revision 3.10  2001/12/29 23:10:59  jmeade
// Set proper default values for null view.
//
// Revision 3.9  2001/12/22 00:21:25  jmeade
// Moved movie recording (for use in other views); Set SliceChildView dataset type in constructor.
//
// Revision 3.8  2001/11/21 15:26:36  jmeade
// Removed obsolete.
//
// Revision 3.7  2001/11/21 00:30:50  jmeade
// Allow base class to color controls.
//
// Revision 3.6  2001/11/20 09:52:45  jmeade
// Allow caller to set background image resource.
//
// Revision 3.5  2001/11/20 04:26:30  jmeade
// SliceScrollbar control.
//
// Revision 3.4  2001/11/17 03:50:01  jmeade
// Forms for view window creation.  Please don't mind the mess, we're refurbishing our store just for you!
//
// Revision 3.3  2001/11/09 01:17:50  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.2  2001/10/19 01:20:16  jmeade
// Issue 1198:  Allow user to toggle 2D measurement profile display on/off
//
// Revision 3.1  2001/10/19 00:13:40  jmeade
// Code standards; Use a null view class and keep ViatronViewWnd as an abstract class
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 12 2001 23:03:36   jmeade
// Add Snapshot handlers
// 
//    Rev 2.2   Oct 10 2001 17:31:14   jmeade
// Stop autofly on resize, fixing the disappearing orthogonal images on window resizeResolution for 309: Orthogonal images disappear when minimizing or maximizing screen
// 
//    Rev 2.1   Oct 03 2001 12:17:26   jmeade
// Snapshot function moved to ViatronViewWnd for all to use; coding standards
// 
//    Rev 2.0   Sep 16 2001 23:43:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:32   ingmar
// Initial revision.
// Revision 1.26  2001/06/05 22:57:58  binli
// bug 000357: fixed
// (different number of slices in S&P studies)
//
// Revision 1.25  2001/04/13 02:35:22  jmeade
// headers and footers
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronViewWnd.h,v 1.21.2.9 2010/04/15 14:28:01 dongqing Exp $
// $Id: ViatronViewWnd.h,v 1.21.2.9 2010/04/15 14:28:01 dongqing Exp $
