// $Id: ViatronViewWnd.C,v 1.36.2.14 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ColonViatronixWnd.C  
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "ViatronViewWnd.h"
#include "NavigationView.h"
#include "Overview.h"
#include "SliceChildView.h"
#include "StandardView.h"
#include "SlidingPaneView.h"
#include "VxVCReport.h"

#include "V1KMessages.h"
#include "MainAppFrame.h"
#include "ReportViewWnd.h"
#include "VCSettings.h"
#include "Viatron.h"
#include "PickScreenPoint.h"

#include "WriteAVI.h"
#include "File.h"
#include "FunctionProgress.h"
#include "User.h"
#include "MissedRegionsDlg.h"

#include <typeinfo.h>
#include <mmsystem.h>

#include "ApplicationSettings.h"
#include "ApplicationController.h"
#include "vcUtils.h"
#include "CadUtils.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_REVISION "$Revision: 1.36.2.14 $"

using namespace ColonViews::Dialogs;
using namespace ColonViews::Layouts;
using namespace ColonViews::FramesManager;
using namespace ReaderLib;
using namespace WinUtils;
using namespace Movies;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals

/// 2D measurement profile display option (for quick reference in rendering functions)
bool ColonViatronixWnd::m_bDisplaying2DMeasurementProfile(false);
/// Set of handles to any launched external processes
std::set<HANDLE> ColonViatronixWnd::m_runningProcSet;
/// Frames of a recorded movie
Movies::MovieCapture ColonViatronixWnd::m_movie;
Timer ColonViatronixWnd::m_movieTimer;

//////////////////////////////////////////////////////////////////////////////////////////////////
// ColonViatronixWnd::v3DViewerExternalProcess
// 

/**
 * Constructor
 *
 * @param hWndViewer            Window to receive a message once the process has completed.
 * @param hProcess              Process to monitor.
 * @param iPostProcessCmdShow   ShowWindow() command, executed once process has completed.
 */
ColonViatronixWnd::v3DViewerExternalProcess::v3DViewerExternalProcess(HWND hWndViewer, HANDLE hProcess, int4 iPostProcessCmdShow)
  : m_hWndViewer(hWndViewer), m_hProcess(hProcess), m_iPostProcessCmdShow(iPostProcessCmdShow)
{
} // constructor


/**
 * Starts a thread that will monitor an external process
 *
 * @param hWndViewer            Window to receive a message once the process has completed.
 * @param hProcess              Process to monitor.
 * @param iPostProcessCmdShow   ShowWindow() command, executed once process has completed.
 */
bool ColonViatronixWnd::v3DViewerExternalProcess::BkgdMonitorProcess(HWND hWndViewer, HANDLE hProcess, int4 iPostProcessCmdShow)
{
  try
  {
    AfxBeginThread(MonitorProc, LPVOID(new v3DViewerExternalProcess(hWndViewer,hProcess,iPostProcessCmdShow)),
      THREAD_PRIORITY_BELOW_NORMAL);
    return true;
  }
  catch (...)
  {
    LogErr("An exception occurred while starting thread", "v3dViewerExternalProcess", "BkgdMonitorProcess");
  }
  return false;
} // BkgdMonitorProcess()

/**
 * Waits for an external process to complete execution.
 *
 * @param lpData   Pointer to v3DViewerExternalProcess, information on the process
 * @return UINT    Always 0
 */
UINT ColonViatronixWnd::v3DViewerExternalProcess::MonitorProc(LPVOID lpData)
{
  try
  {
    v3DViewerExternalProcess * pExtData = (v3DViewerExternalProcess*)lpData;
    WaitForSingleObject(pExtData->m_hProcess, INFINITE);
    ::ShowWindow(pExtData->m_hWndViewer, pExtData->m_iPostProcessCmdShow);
    ColonViatronixWnd::m_runningProcSet.erase(pExtData->m_hProcess);
    delete lpData;
  }
  catch (...)
  {
    LogErr("An exception occurred", "v3dViewerExternalProcess", "MonitorProc");
  }
  return 0;
} // MonitorProc()


//////////////////////////////////////////////////////////////////////////////////////////////////
// ColonViatronixWnd
// 

/**
 * Constructor 
 *
 * @param uBkgdBmpID        Resource ID of the background image.
 */
ColonViatronixWnd::ColonViatronixWnd( bool bSupportsCAD, const uint4 uBkgdBmpID ): ViatronixWnd( uBkgdBmpID ),
  m_pViewMouseLastIn(GLChildViewPtr(NULL)),
  m_childRecordingMovieID(GLChildView::GLVID(0)),
  m_idMainView(0), 
  m_hSaveFrame(NULL),
  m_cadMonitor( this, bSupportsCAD )
{
  MainAppFrame::GetMainFrame()->m_viewSelBar.SetButtonFunctionText(std::string(),std::string(),std::string());
  ApplicationSettings::UserSettings().TryGet( APP_SETTING_SHOW_2D_MEASUREMENT_PROFILE, m_bDisplaying2DMeasurementProfile );
  ApplicationSettings::UserSettings().TryGet( APP_SETTING_COLOR_SCHEME, rdrGlobal.m_iColorScheme );
} // constructor()


/**
 * Destructor
 */
ColonViatronixWnd::~ColonViatronixWnd()
{
  if( m_hSaveFrame != NULL )
  {
    CloseHandle( m_hSaveFrame );
    m_hSaveFrame = NULL;
  }
} // ~ColonViatronixWnd()


/**
 * Create the child views for this window
 *
 * @param hWndThis   Handle to this window
 */
void ColonViatronixWnd::CreateChildViews(HWND hWndThis)
{
  ViatronixWnd::CreateChildViews( hWndThis );

  // common layout creation code

  // for layouts where Cad and Missed regions are to be toggled in same area
  const bool bCad( rdrGlobal.m_supine.IsCadFindingsLoaded() || rdrGlobal.m_prone.IsCadFindingsLoaded() );
  CWnd * pCadWnd = GetDlgItem(IDC_TEMPLATE_CADRESULTSDLG);
  CWnd * pMsdWnd = GetDlgItem(IDC_TEMPLATE_MISSED_REGIONS_DLG);

  if ( pCadWnd && pMsdWnd )
  {
    pCadWnd->ShowWindow(bCad ? SW_SHOW : SW_HIDE);
    pMsdWnd->ShowWindow(bCad ? SW_HIDE : SW_SHOW);
  }

  const BOOL bTipsEnabled(EnableToolTips(TRUE));

  // set the dataset link status
  Utilities::Datasets::Lock( IsDatasetsLockingEnabled() ? 
                                ApplicationSettings::UserSettings().Get<bool>( APP_SETTING_LINK_DATASETS ) :
                                VCSettings::GetInstance().GetBoolValue( VCSettings::ViewerXML(), VCSettings::DefaultLinkedDatasets() ));                                                      
} // CreateChildViews()


/**
 * WM_CREATE message handler
 *
 * @param lpCreateStruct   Create parameters
 * @return int             TRUE if creation successful
 */
int ColonViatronixWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if( IsDatasetsLockingEnabled() )
    Colon::ApplicationController::Connect( this );

  int iResult = ViatronixWnd::OnCreate(lpCreateStruct);
  m_pViewMouseLastIn = &GetNullEventView();
  m_idMainView = GetChildViewByPos(0).GetClassInstanceID();

  Colon::FullScreenController::GetInstance().Connect( this );
  
  return iResult;
} // OnCreate()


BEGIN_MESSAGE_MAP(ColonViatronixWnd, ViatronixWnd)
  //{{AFX_MSG_MAP(ColonViatronixWnd)
  ON_COMMAND(IDM_OPTIONS_SHOWMEASUREMENTPROFILES, OnOptionsToggleMeasurementProfileDisplay)
  ON_UPDATE_COMMAND_UI(IDM_OPTIONS_SHOWMEASUREMENTPROFILES, OnUpdateOptionsToggleMeasurementProfileDisplay)
  ON_UPDATE_COMMAND_UI(IDM_OPTIONS_COLORSCHEME_1, OnUpdateOptionsColorSchemes)
  ON_COMMAND(IDM_OPTIONS_COLORSCHEME_1, OnOptionsColorscheme1)
  ON_COMMAND(IDM_OPTIONS_COLORSCHEME_2, OnOptionsColorscheme2)
  ON_UPDATE_COMMAND_UI(IDM_BOOKMARKS_BOOKMARK_LIST, OnUpdateBookmarksList)
  ON_UPDATE_COMMAND_UI(IDM_BOOKMARKS_ADDBOOKMARK, OnUpdateBookmarksAddBookmark)
  ON_UPDATE_COMMAND_UI(IDM_BOOKMARKS_DELETEALLBOOKMARKS, OnUpdateBookmarksDeleteAllBookmarks)
  ON_COMMAND(IDM_NOTATIONS_ANNOTATIONS_DELETEALLANNOTATIONS, OnAnnotationsDeleteAllAnnotations)
  ON_UPDATE_COMMAND_UI(IDM_NOTATIONS_ANNOTATIONS_DELETEALLANNOTATIONS, OnUpdateAnnotationsDeleteAllAnnotations)
  ON_COMMAND(IDM_2DANNOTATIONS_EDITQUICKDROPNAME, On2DAnnotationsEditQuickDropName)
  ON_COMMAND(ID_REPORT_MARKCOMPLETE, OnToggleReportCompleteMark)
  ON_UPDATE_COMMAND_UI(ID_REPORT_MARKCOMPLETE, OnUpdateToggleReportCompleteMark)
  ON_COMMAND(ID_OPTIONS_DISPLAYRENDERSPEED, OnOptionsDisplayRenderSpeed)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_DISPLAYRENDERSPEED, OnUpdateOptionsDisplayRenderSpeed)
  ON_UPDATE_COMMAND_UI(IDM_NOTATIONS_ANNOTATION_LIST_FIRST, OnUpdateNotationsAnnotationList)
  ON_COMMAND(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY, OnViewWindowPseudoColorDisplay)
  ON_UPDATE_COMMAND_UI(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY, OnUpdateViewWindowPseudoColorDisplay)
  ON_COMMAND(ID_VIEWWINDOW_SHOWCLEANSED, OnViewWindowShowCleansed)
  ON_UPDATE_COMMAND_UI(ID_VIEWWINDOW_SHOWCLEANSED, OnUpdateViewWindowShowCleansed)
  ON_COMMAND(ID_2DDISPLAYOPTIONS_VERTICALSCROLLBARS, OnToggleScrollbarOrientation)
  ON_UPDATE_COMMAND_UI(ID_2DDISPLAYOPTIONS_VERTICALSCROLLBARS, OnUpdateToggleScrollbarOrientation)
  ON_COMMAND(ID_VIEWWINDOW_ARROWINSLICECENTER, OnViewWindowArrowInSliceCenter)
  ON_UPDATE_COMMAND_UI(ID_VIEWWINDOW_ARROWINSLICECENTER, OnUpdateViewWindowArrowInSliceCenter)
  ON_COMMAND(ID_VIEWWINDOW_SWITCHFLIGHTDIRECTION, OnViewWindowSwitchFlightDirection)
  ON_UPDATE_COMMAND_UI(ID_VIEWWINDOW_SWITCHFLIGHTDIRECTION, OnUpdateViewWindowSwitchFlightDirection)
  ON_COMMAND(ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW, OnViewWindowUpdateEndoluminalView)
  ON_UPDATE_COMMAND_UI(ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW, OnUpdateViewWindowUpdateEndoluminalView)
  ON_UPDATE_COMMAND_UI(IDM_OPTIONS_COLORSCHEME_2, OnUpdateOptionsColorSchemes)
  ON_UPDATE_COMMAND_UI(IDM_GOTO_BOOKMARK_FIRST, OnUpdateBookmarksList)
  ON_UPDATE_COMMAND_UI(IDM_GOTO_ANNOTATION_FIRST, OnUpdateNotationsAnnotationList)
  ON_UPDATE_COMMAND_UI(IDM_GOTO_3DMEASUREMENTS_FIRST, OnUpdateNotations3DMeasurements)
  ON_UPDATE_COMMAND_UI(IDM_NOTATIONS_3DMEASUREMENTS_LIST_FIRST, OnUpdateNotations3DMeasurements)
  ON_COMMAND(ID_OPTIONS_ALWAYSSHOWCADINDICATORS, OnOptionsAlwaysShowCadIndicators)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_ALWAYSSHOWCADINDICATORS, OnUpdateOptionsAlwaysShowCadIndicators)
  ON_COMMAND(ID_CADOPTIONS_SHOWEXTRACOLONIC, OnOptionsShowExtraColonicFindings)
  ON_UPDATE_COMMAND_UI(ID_CADOPTIONS_SHOWEXTRACOLONIC, OnUpdateOptionsShowExtraColonicFindings)
  ON_COMMAND(IDM_SRTOPTIONS_DISPLAYREGIONSOFINTEREST, OnSrtOptionsDisplayRegionsOfInterest)
  ON_UPDATE_COMMAND_UI(IDM_SRTOPTIONS_DISPLAYREGIONSOFINTEREST, OnUpdateSrtOptionsDisplayRegionsOfInterest)
  //}}AFX_MSG_MAP
  //ON_COMMAND(ID_VIEWWINDOW_2DSLICE, OnViewWindow2DSlice)
  //ON_COMMAND(ID_VIEWWINDOW_THINSLAB, OnViewWindowThinSlab)
  //ON_COMMAND(ID_VIEWWINDOW_MIP, OnViewWindowMIP)

  // Menu item and accelerator key messages
  ON_COMMAND(IDM_BOOKMARKS_ADDBOOKMARK, OnBookmarksAddBookmark)
  ON_COMMAND(IDM_BOOKMARKS_DELETEALLBOOKMARKS, OnBookmarksDeleteAllBookmarks)
  ON_COMMAND_RANGE(IDM_GOTO_BOOKMARK_FIRST, IDM_GOTO_BOOKMARK_FIRST + IDM_GOTO_BOOKMARK_LIST_MAX, OnBookmarksGotoBookmark)
  ON_COMMAND_RANGE(IDM_GOTO_ANNOTATION_FIRST, IDM_GOTO_ANNOTATION_FIRST + IDM_GOTO_BOOKMARK_LIST_MAX, OnNotationsAnnotationGotoAnnotation)
  ON_COMMAND_RANGE(IDM_GOTO_3DMEASUREMENTS_FIRST, IDM_GOTO_3DMEASUREMENTS_FIRST + IDM_GOTO_BOOKMARK_LIST_MAX, OnNotations3DMeasurementsGotoMeasurement)
  ON_COMMAND(IDM_JUMP_PREVSEGMENT, OnJumpPrevSegment)
  ON_COMMAND(IDM_JUMP_NEXTSEGMENT, OnJumpNextSegment)
  ON_COMMAND(IDM_TOGGLE_SUPINE_PRONE, OnToggleSupineProne)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE, OnToggleLineMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE, OnToggleRoiMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING, OnToggleLighting)
  ON_COMMAND(ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY, OnToggleTranslucent)
  ON_COMMAND(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION, OnToggleAnnotationMode)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, OnToggleSegmentPolypMode)
  ON_COMMAND(IDM_TOGGLE_VIEW_DIRECTION, OnToggleViewDirection)
  ON_COMMAND(IDM_NEXT_MISSED_REGION, OnNextMissedRegion)

  ON_MESSAGE(WM_APP_STUDY_LOAD_STEP_NOTIFY, OnAppStudyLoadStepNotify)
  ON_MESSAGE(WM_APP_VIEWMODE_CHANGED, OnAppViewModeChanged)
  ON_MESSAGE(WM_APP_MISSED_REGION_SELECTED, OnAppSelectMissingPatch)
  ON_MESSAGE(WM_APP_JUMP_NEXT_MISSED_REGION, OnAppJumpNextMissedRegion)
  ON_MESSAGE(WM_APP_DETECT_MISSED_REGIONS, OnAppDetectMissedRegions)
  ON_MESSAGE(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, OnAppUpdateLumenCoverageMarking)
  ON_MESSAGE(WM_USER_MOVIE_RECORD_MODE_QUERY, OnMovieRecordModeQuery)
  ON_MESSAGE(WM_USER_MOVIE_RECORD_MODE_TOGGLE, OnMovieRecordModeToggle)
  ON_MESSAGE(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID, OnGetMainViewWndID)
  ON_MESSAGE(WM_APP_2DAUTOFLIGHT_UPDATE, OnUpdate2DAutoFlight)

  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)

  ON_COMMAND(ID_OPTIONS_CONTROLPAD_MISSEDREGIONS, OnOptionsControlPadShowMissedRegions)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_CADRESULTS, OnOptionsControlPadShowCADResults)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_MISSEDREGIONS, OnUpdateOptionsControlPadMissedCAD)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_CADRESULTS, OnUpdateOptionsControlPadMissedCAD)
END_MESSAGE_MAP()


/**
 * Copies a locally-converted movie to study location and deletes local temp files (avi and wmv)
 *
 * @param pMovieConvertProcess   Pointer to MovieConvertProcess instance
 * @return                       1
 */
UINT ColonViatronixWnd::MoveConvertedMovie(LPVOID pMovieConvertProcess)
{
  MovieConvertProcess& rMCP = *((MovieConvertProcess*) pMovieConvertProcess);

  // Wait for conversion utility to exit
  if (rMCP.m_format.m_sFileTypeExt == vx::WindowsMediaVideoExt)
  {
    WaitForSingleObject(rMCP.m_hConvertProcess, INFINITE);

    // Wait till source is writable (finished converting and writing file)
    while (!rMCP.m_movieSrc.CanWrite())
    {
      Sleep(2000);
    }

    // Attempt to copy converted file (to the study directory)
    while (!rMCP.m_movieSrc.CopyTo(rMCP.m_movieDest))
    {
      Sleep(2000);
    }

    // Copy the preview image
    vx::File(rMCP.m_movieSrc.GetFullTitle() + vx::JPEGExt).CopyTo(rMCP.m_movieDest.GetFullTitle() + vx::JPEGExt);

    // Delete the source converted movie
    rMCP.m_movieSrc.Delete();
    // Delete the original (avi) movie
    vx::File(rMCP.m_movieSrc.GetFullTitle() + vx::AVIExt).Delete();
  }
  else
  {
    // If no conversion was requested, then just move the avi
    vx::File aviFile(rMCP.m_movieSrc.GetFullTitle() + vx::AVIExt);
    if (aviFile.CopyTo(vx::File(rMCP.m_movieDest.GetFullTitle() + vx::AVIExt)))
    {
      aviFile.Delete();
    }
    else
    {
      LogErr("An error occurred while moving avi movie: " + aviFile.ToString(), "ViatronViewWnd", "MoveConvertedMovie");
    }

    // Copy the preview image
    vx::File prevImg(rMCP.m_movieSrc.GetFullTitle() + vx::JPEGExt);
    if (prevImg.CopyTo(rMCP.m_movieDest.GetFullTitle() + vx::JPEGExt))
    {
      prevImg.Delete();
    }
  }

  delete pMovieConvertProcess;

  return 1;
} // MoveConvertedMovie()


/**
 * Returns a string of indicating the patient orientation
 *
 * @param rDataset   dataset
 * @param bAbbrev    true to retrieve an abbreviated string
 * @return           string representation of orientation
 */
std::string ColonViatronixWnd::PatientOrientationToString(const Dataset & rDataset, const bool bAbbrev)
{
  const SerializedDicom::PatientOrientation ePatOrient = rDataset.GetPatientOrientation();
  return PatientOrientationToString(ePatOrient, rDataset.GetModality(), bAbbrev);
} // PatientOrientationToString()


/**
 * Returns a string of indicating the patient orientation
 *
 * @param ePatOrient   dataset
 * @param eMod         modality
 * @param bAbbrev      true to retrieve an abbreviated string
 * @return             string representation of orientation/modality combination
 */
std::string ColonViatronixWnd::PatientOrientationToString(const SerializedDicom::PatientOrientation ePatOrient,
                                                          const ReaderLib::Dataset::DataModality eMod, const bool bAbbrev)
{
  int4 iId(0);
  if ( eMod == Dataset::CT )
  {
    switch ( ePatOrient )
    {
    case SerializedDicom::SUPINE:
      iId = IDS_SUPINE;
      break;
    case SerializedDicom::PRONE:
      iId = IDS_PRONE;
      break;
    case SerializedDicom::DECUBITUS:
      iId = !bAbbrev ? IDS_DECUBITUS : IDS_DECUB;
      break;
    case SerializedDicom::DECUBITUS_LEFT:
      iId = !bAbbrev ? IDS_DECUBITUS_L : IDS_DECUB_L;
      break;
    case SerializedDicom::DECUBITUS_RIGHT:
      iId = !bAbbrev ? IDS_DECUBITUS_R : IDS_DECUB_R;
      break;
    default:
      iId = 0;
      break;
    }
  }
  else if ( rdrGlobal.m_supine.GetModality() == Dataset::MR )
  {
    switch ( ePatOrient )
    {
    case SerializedDicom::SUPINE:
      iId = !bAbbrev ? IDS_POSTCONTRAST : IDS_POSTCONT;
      break;
    case SerializedDicom::PRONE:
      iId = !bAbbrev ? IDS_PRECONTRAST : IDS_PRECONT;
      break;
    default:
      iId = 0;
      break;
    }
  }

  return LPCTSTR(LoadResourceString(iId));
} // PatientOrientationToString


/**
 * WM_TIMER handler
 *
 * @param uIDEvent   ID of the timer event
 */
void ColonViatronixWnd::OnTimer(UINT uIDEvent) 
{
  if (uIDEvent == m_timerPauseInMovie)
  {
    if (m_childRecordingMovieID > 0)
    {
      ViatronChildView& childView = dynamic_cast<ViatronChildView&>(GetChildView(m_childRecordingMovieID));
      Dataset & dataset = childView.GetDatasetRef();
      Submarine & sub = dataset.m_submarine;

      switch (ViatronStudy::m_modeEndoView.GetMutexModes())
      {
        case ViewModes::BIOPSY:
        {
          MovieFrame& rPrev = m_movie.m_frames.back();
          Point<int4>& rMagicWinPt = dataset.m_fields.m_volumeRender.m_magicWindowStartPt;
          if (rPrev.m_eFrameType == MovieFrame::TRANSLUCENCY && rPrev.m_ptTransLoc.m_x == rMagicWinPt.m_x && rPrev.m_ptTransLoc.m_y == rMagicWinPt.m_y )
          {
            break;
          }

          MovieFrame frame;
          frame.m_eDatasetOrientation = dataset.GetOrientation();
          frame.m_eFrameType = MovieFrame::TRANSLUCENCY;
          frame.m_ptTransLoc = Point2D<int4>(rMagicWinPt.m_x, rMagicWinPt.m_y);
          frame.m_camera = Camera(sub.GetPositionWorld(), sub.GetOrientation(), sub.GetUpDirection());
          frame.m_bSpaceLeaping = rdrGlobal.m_bSpaceLeaping;
          frame.m_uSampleTime = ColonViatronixWnd::m_movieTimer.ElapsedTime() * 1000;

          frame.m_bShow2DCamera = SliceChildView::GetIsCameraShowing();

          // add n movie frames
          for (uint4 i(0); i < ePauseFrameElapseTimeMS / 1000 * eMovie_FrameRate; m_movie.m_frames.push_back(frame), i++);

          break;
        }

        default:
        {
          Submarine & sub = dataset.m_submarine;

          MovieFrame frame;
          frame.m_camera = Camera(sub.GetPositionWorld(), sub.GetOrientation(), sub.GetUpDirection());
          frame.m_bSpaceLeaping = rdrGlobal.m_bSpaceLeaping;
          frame.m_uSampleTime = ColonViatronixWnd::m_movieTimer.ElapsedTime() * 1000;

          frame.m_bShow2DCamera = SliceChildView::GetIsCameraShowing();

          // add n movie frames
          for (uint4 i(0); i < ePauseFrameElapseTimeMS / 1000 * eMovie_FrameRate; m_movie.m_frames.push_back(frame), i++);

          break;
        }
      }
    }

    m_timerPauseInMovie.KillTimer();
  }

  ViatronixWnd::OnTimer(uIDEvent);
} // OnTimer


/**
 * Sets the current autofly mode.
 *
 * @param bSetModeOn   Whether to turn autofly on or off.
 */
void ColonViatronixWnd::SetAutoflyMode(const bool bSetModeOn)
{
  VtxViewModes & currModes = ViatronStudy::GetCurrentViewModes();
#if defined(FINAL_RELEASE)
  // This prevents the "Turbo Mode" easter-egg in Release builds
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && bSetModeOn)    // replace with IsManualFlying() or like
  {
    return;
  }
#endif

  ViewModes::VIEWMODES iMode = ViewModes::VIEWMODES(0);
  if (currModes.GetValid() & ViewModes::AUTOFLY_VALID)
  {
    iMode = ViewModes::AUTOFLY;

    if (!bSetModeOn)
    {
      AfxGetMainWnd()->SendMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);    
    }
  }
  else if (rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()) && (currModes.GetValid() & ViewModes::AUTOFLY2D_VALID))
  {
    iMode = ViewModes::AUTOFLY2D;
  }
  else
  {
    return;
  }

  if (bSetModeOn)   currModes += iMode;
  else              currModes -= iMode;

  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
} // SetAutoflyMode()


/**
 * Toggles the current autofly mode.
 */
void ColonViatronixWnd::ToggleAutoflyMode()
{
  VtxViewModes & currModes = ViatronStudy::GetCurrentViewModes();
#if defined(FINAL_RELEASE)
  // This prevents the "Turbo Mode" easter-egg in Release builds
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }
#endif

  const bool bBiopsy(BitCheck(ViatronStudy::GetCurrentViewModes(), ViewModes::BIOPSY));

  if (currModes.GetValid() & ViewModes::AUTOFLY_VALID)
  {
    currModes.Toggle(ViewModes::AUTOFLY);
  }
  else if (rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()) && (currModes.GetValid() & ViewModes::AUTOFLY2D_VALID))
  {
    currModes.Toggle(ViewModes::AUTOFLY2D);
  }

  AfxGetMainWnd()->SendMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
  
  if (bBiopsy)
  {
    rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
    rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  }
} // ToggleAutoflyMode()


/**
 * Called at each step during study load
 *
 * @param iPrevAndNextStep   LOWORD(Previous study load step), HIWORD(Current/next study load step),
 *                            cast each to StudyLoad::StepsEnum
 * @param iOrientation       Cast to DatasetOrientations::TypeEnum, identifies study orientation
 * @return                   0
 */
LRESULT ColonViatronixWnd::OnAppStudyLoadStepNotify(WPARAM iPrevAndNextStep, LPARAM iOrientation)
{
  OnStudyLoadStepNotify(StudyLoad::StepsEnum(LOWORD(iPrevAndNextStep)),
    StudyLoad::StepsEnum(HIWORD(iPrevAndNextStep)), DatasetOrientations::TypeEnum(iOrientation));
  return 0;
} // OnAppStudyLoadStepNotify()


/**
 * Notifies view window of the current progress of study load
 *
 * @param eCompletedStep   The previously completed step
 * @param eCurrentStep     The next (current) study loading step
 * @param eOrientation     Orientation on which the next step is performing
 */
void ColonViatronixWnd::OnStudyLoadStepNotify(const StudyLoad::StepsEnum eCompletedStep,
  const StudyLoad::StepsEnum eCurrentStep, const DatasetOrientations::TypeEnum eOrientation)
{

} // OnStudyLoadStepNotify()


/**
 * WM_APP_ENDO_MODE_CHANGED:  Called when a view mode changes
 *
 * @param uOldMode   Old view mode
 * @param uNevMode   New view mode
 * @return           TRUE
 */
LRESULT ColonViatronixWnd::OnAppViewModeChanged(WPARAM uOldMode, LPARAM uNewMode)
{
  for (int4 iView = 0; iView < NumGLViews(); iView++)
  {
    ((ViatronChildView&)GetChildViewByPos(iView)).OnViewModeChange(uOldMode, uNewMode);
  }
  
  // Sets mouse use key strings, etc.
  SetChildViewMisc(*m_pViewMouseLastIn, true);

  return TRUE;
} // OnAppViewModeChanged()


/**
 * WM_APP_DETECT_MISSED_REGIONS: OBSOLETE -- functionality localized with missed region listbox
 *
 * @param WPARAM    Unused
 * @param LPARAM    Unused
 * @return          TRUE
 */
LRESULT ColonViatronixWnd::OnAppDetectMissedRegions(WPARAM, LPARAM)
{
  return TRUE;
} // OnAppDetectMissedRegions


/**
 * Message: WM_APP_MISSED_REGION_SELECTED: OBSOLETE -- functionality localized with missed region listbox
 *
 * @param iSelection   The i-th patch to jump to
 * @param LPARAM       Unused
 * @return             TRUE
 */
afx_msg LRESULT ColonViatronixWnd::OnAppSelectMissingPatch(WPARAM iSelection, LPARAM)
{
  return TRUE;
} // OnAppSelectMissingPatch()


/**
 * Message: WM_APP_JUMP_NEXT_MISSED_REGION: functionality localized with missed region listbox, send message to be handled there
 *
 * @param prevORnext   Zero to move to the previous patch, otherwise move to next patch
 * @param LPARAM       Unused
 * @return             TRUE
 */
LRESULT ColonViatronixWnd::OnAppJumpNextMissedRegion(WPARAM prevORnext, LPARAM)
{
  MissedRegionsDlg * pDlg = static_cast<MissedRegionsDlg*>(GetDlgItem(IDC_TEMPLATE_MISSED_REGIONS_DLG));
  if (pDlg && pDlg->IsWindowVisible())
  {
    pDlg->OnNextMissedRgnsBtn();
  }
  return TRUE;
} // OnAppJumpNextMissedRegion()


/**
 * Message WM_APP_UPDATE_LUMEN_COVERAGE_MARKING: Update lumen coverage (marking) display in endoscopic view
 *
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         TRUE
 */
LRESULT ColonViatronixWnd::OnAppUpdateLumenCoverageMarking(WPARAM, LPARAM)
{
  rdrGlobal.m_pCurrDataset->m_fields.m_paintVol.Transfer3DMarking();
  if(rdrGlobal.m_pSecondaryDataset)
  {
    rdrGlobal.m_pSecondaryDataset->m_fields.m_paintVol.Transfer3DMarking();
  }

  InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  if(rdrGlobal.m_pSecondaryDataset)
  {
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  }
  return TRUE;
} // OnAppUpdateLumenCoverageMarking


/**
 * Message: WM_SIZE: Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void ColonViatronixWnd::OnSize(UINT uType, int iCX, int iCY) 
{
  // Turn off autofly
  ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
  rdrGlobal.m_pCurrDataset->m_submarine.FullStop();
  ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;

  ViatronixWnd::OnSize(uType, iCX, iCY);
} // OnSize


/**
 * WM_MOUSEMOVE handler
 *
 * @param uFlags   Current key flags
 * @param point    Current point of mouse move
 */
void ColonViatronixWnd::OnMouseMove(UINT uFlags, CPoint point)
{
  GLChildViewRef prevView = GetCurrentView();

  if (!IsAnExternalProcRunning())
  {
    SetFocus();
  }

  ViatronixWnd::OnMouseMove(uFlags, point);

  GLChildViewRef currView = GetCurrentView();
  if (prevView.GetClassInstanceID() != currView.GetClassInstanceID())
  {
    // Current view, as a result of ViatronixWnd::OnMouseMove() call, has changed
    // (i.e. mouse cursor moved between view windows)

    SetChildViewMisc(prevView, false);

    m_pViewMouseLastIn = &currView;

    SetChildViewMisc(currView, true);

    InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
  }
  else
  {
    SetChildViewMisc(prevView, true);
  }

  if (typeid(currView) != typeid(GLChildView))
  {
    m_idMainView = currView.GetClassInstanceID();
  }

  // Gives the current view the ViatronStudy "focus"
  SetCurrentViatronStudyView();
} // OnMouseMove


/**
 * Message: WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID: Gets the class instance ID of the current main view window
 *
 * @param WPARAM    Unused
 * @param LPARAM    Unused
 * @return          Main child view's ID
 */
LRESULT ColonViatronixWnd::OnGetMainViewWndID(WPARAM, LPARAM)
{
  return LRESULT(m_idMainView);
} // OnGetMainViewWndID


/** 
 * Message: WM_APP_2DAUTOFLIGHT_UPDATE: 2D auto-flight: signal to set up new skeleton position.
 *
 * @param WPARAM   position on current skeleton.
 * @param LPARAM   Unused
 * @return         TRUE to set up new position on current skeleton.
 */
LRESULT ColonViatronixWnd::OnUpdate2DAutoFlight(WPARAM wParam, LPARAM lParam)
{
  switch (lParam)
  {
  case MAKELPARAM(0,1):   // 0+1: render overview
  case MAKELPARAM(1,1):   // 1+1: rerender overview at once.
    {
      for (int4 iView=0; iView < NumGLViews(); iView++)
      {
        if (typeid(GetChildViewByPos(iView)) == typeid(OverView))
        {
          OverView & overView = (OverView&) GetChildViewByPos(iView);
          overView.RefreshImage(/*time click=*/ 3);

          if (lParam == MAKELPARAM(1,1) /* rerender overview at once*/)
          {
            GLRenderView( iView );
          }
          break;
        }
      }
    }
    break;
  
  default: break;
  }
  return 1;
} // OnUpdate2DAutoFlight()


/**
 * Message: WM_APP_SYNCRONIZE_DATASET_LOCATIONS: Synchronize the 3D world location of two (registered) datasets
 *
 * @param wpSrcOrientation    Dataset (or ID) of dataset for fixed location
 * @param lpDestOrientation   Dataset (or ID) of dataset for adjusting location
 * @return                    Always true (unused)
 */
LRESULT ColonViatronixWnd::OnSyncronizeDatasetLocations(WPARAM wpSrcOrientation, LPARAM lpDestOrientation )
{
  if (rdrGlobal.IsDualDataLoaded() && Utilities::Datasets::IsLocked()) // For Linked actions
  {
    Dataset& rDest = ViatronChildView::GetDatasetRef(DatasetOrientations::TypeEnum(lpDestOrientation));
    rdrGlobal.UpdateWorldPosition(ViatronChildView::GetDatasetRef(DatasetOrientations::TypeEnum(wpSrcOrientation)), rDest);

    rDest.m_fields.m_volumeRender.FlushAccumulationBuffer();
    OnAppGLChildViewTypeInvalidate( GLChildView::eGVT_ALL, 0 );
  }
  return 1;
} // OnSyncronizeDatasetLocations()


/**
 * Sets the ViatronStudy current view to the view returned by GetCurrentView()
 *  (that based on the current mouse focus)
 *
 * NOTE: Override to change ViatronStudy current view modes behavior
 * Implementation Note: This functionality can really be combined with what's implemented in OnMouseMove
 */
void ColonViatronixWnd::SetCurrentViatronStudyView()
{
  ViatronChildView& currChild = (ViatronChildView&) GetCurrentView();
  if ((currChild.GetViewWndType() != GLChildView::eGVT_NONE) &&
    (!(ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY) &&
    !(ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)))
  {
    ViatronStudy::SetCurrentView(currChild.GetViewWndType(), currChild.GetDatasetRef().GetOrientation());
  }
} // SetCurrentViatronStudyView


/**
 * WM_DESTROY handler, resets miscellaneous info as appropriate for the
 *  last child view containing the cursor
 */
void ColonViatronixWnd::OnDestroy()
{
  if( IsDatasetsLockingEnabled() )
    Colon::ApplicationController::Disconnect( this );

  Colon::FullScreenController::GetInstance().Disconnect( this );

  if( m_hSaveFrame != NULL )
  {
    SetEvent( m_hSaveFrame );
  }

  SetChildViewMisc(*(m_pViewMouseLastIn = &GetNullEventView()), false);

  rdrGlobal.m_pCurrDataset->m_submarine.FullStop();

  ViatronixWnd::OnDestroy();
} // OnDestroy


/**
 * Background procedure to update the progress bar while movie is being saved
 *
 * @param iProgress   Progress of saving 1-100
 * @return            true                  
 */
bool UpdateMovieSaveProgress(const int4 iProgress)
{
  AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS, iProgress, 0);
  return true;
} // UpdateMovieSaveProgress()


/**
 * Converts a string to a UNICODE (multibyte) string
 *
 * @param s   String to convert
 * @return    Converted, multibyte, string
 */
inline std::wstring widestring(const std::string &s)
{
  int iWCLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), -1, NULL, 0);
  if (iWCLen <= 0)    return std::wstring();
  wchar_t * pWCStr = new wchar_t[iWCLen];
  if (pWCStr == NULL) return std::wstring();
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), -1, pWCStr, iWCLen);
  std::wstring wsRtn(pWCStr);
  delete pWCStr;
  return wsRtn;
} // widestring()


/**
 * WM_KEYDOWN handler, sets/resets miscellaneous info as appropriate for the
 *  last child view containing the cursor
 *
 * @param uChar     Key code identifier.
 * @param uRepCnt   Key repeat count.
 * @param uFlags    Specifies the scan code, key-transition code, previous key state, and context code.
 */
void ColonViatronixWnd::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  // Notify current child view for modifier keys
  switch (uChar)
  {
    case VK_F1: case VK_F2: case VK_F3: case VK_F4:  case VK_F5:  case VK_F6:
    case VK_F7: case VK_F8: case VK_F9: case VK_F10: case VK_F11:  case VK_F12:
    case VK_CONTROL:
    case VK_SHIFT:
      SetChildViewMisc(GetCurrentView(), true);
      break;

    case 'P':
    {
      GLChildView::GLVID idMainID(SendMessage(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID,0,0));
      if (idMainID <= 0) { idMainID = GetCurrentView().GetClassInstanceID(); }
      GLChildViewRef childView = GetChildView(idMainID);
      Dataset & myDataset = ((ViatronChildView&)childView).GetDatasetRef();
      const vx::File tempFile(vx::File().CreateTempFile(TEXT("v3D")).ToString());
      const vx::File pictureFile(tempFile.ToString() + vx::JPEGExt);
      tempFile.Delete();
      //extern std::wstring widestring(const std::string &);

      int4 iWidth(0), iHeight(0), iWidthAlignedDWORD( 0 );
      LPBYTE pBuffer = LPBYTE(childView.GetUpdateBuffer(iWidth, iHeight, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE));
      if (!pBuffer)
      {
        LogErr("Memory error occurred while getting update buffer for image view snapshot (case uChar == 'P')", "ColonViatronixWnd", "OnKeyDown");
        CString sMsg;
        sMsg.Format(IDS_ERROR_GENERIC_FORMAT, LoadResourceString(IDS_SAVE), LoadResourceString(IDS_IMAGE));
        AfxMessageBox(sMsg, MB_ICONEXCLAMATION | MB_OK);
        return;
      }
      ReaderGlobal::WriteJPEGImage(iWidth, iHeight, pBuffer, (wchar_t*)widestring(pictureFile.ToString()).c_str());

      SetCursor(AfxGetApp()->LoadCursor(IDC_SNAPSHOT));
      PlaySound(LPCSTR(IDW_SNAPSHOT), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);

      SHELLEXECUTEINFO sei;
      memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

      // Open the picture file
      sei.cbSize = sizeof(SHELLEXECUTEINFO);
      sei.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_DDEWAIT;
      sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
      std::string sFilename = pictureFile.ToString();
      sei.lpFile = sFilename.c_str();
      sei.lpVerb = "open";
      sei.lpDirectory = ".";
      sei.nShow = SW_SHOW;

      ShellExecuteEx(&sei);

      if (m_runningProcSet.insert(sei.hProcess).second == true)
      {
        const int4 iShowCmd(AfxGetMainWnd()->IsZoomed() ? SW_SHOWMAXIMIZED : SW_RESTORE);
        v3DViewerExternalProcess::BkgdMonitorProcess(sei.hwnd, sei.hProcess, iShowCmd);
      }

      return;
    }
    case 'D':
    {
      if( IsDatasetsLockingEnabled() && rdrGlobal.IsDualDataLoaded() )
      {
				Utilities::Datasets::ToggleLock();
			} // if
      break;
    }

    default:
      break;
  }

  // monitor cad commands
  m_cadMonitor.Monitor( uChar, uRepCnt, uFlags );

  ViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown


/**
 * WM_KEYUP handler
 *
 * @param uChar     Key code identifier
 * @param uRepCnt   Key repeat count
 * @param uFlags    Repeat code flags, other flags
 */
void ColonViatronixWnd::OnKeyUp(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  // TODO: Add your message handler code here and/or call default
  switch (uChar)
  {
    case VK_CONTROL:
    case VK_SHIFT:
      SetChildViewMisc(GetCurrentView(), true);
      break;

    default:
      break;
  }

  ViatronixWnd::OnKeyUp(uChar, uRepCnt, uFlags);
} // OnKeyUp


/**
 * Called to render child views, renders only navigation and over views
 *  if flying in current view, renders all views otherwise
 */
void ColonViatronixWnd::GLRenderViews()
{
  // If a view has capture set, only render the one view (to maximize user responsiveness)
  if (GetCaptureViewPtr() != NULL)
  {
    GLRenderView( *GetCaptureViewPtr() );
  }
  // Else, when flying, only render 3D and overviews
  else if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying())
  {
    for (uint4 uView = 0; uView < NumGLViews(); uView++)
    {
      if ( (typeid(GetChildViewByPos(uView)) == typeid(EndoluminalView))
        || (typeid(GetChildViewByPos(uView)) == typeid(OverView)) )
      {
        GLRenderView( uView );
      }
    }
  }
  else
  {
    ViatronixWnd::GLRenderViews();
  }
} // GLRenderViews


/**
 * Overridden to send paint messages to Viatron parent controls
 */
void ColonViatronixWnd::RenderControls()
{
  return;
  for (CWndPtrList::const_iterator it = m_controlPtrList.begin(); it != m_controlPtrList.end(); it++)
  {
    const type_info& controlType = typeid(*(*it));
    if (controlType == typeid(ColonViatronixWnd))
    {
      HWND hWnd = *(*it);
      ::SendMessage(hWnd, WM_PAINT, WPARAM(::GetDC(hWnd)), 0);
    }
    else if (controlType == typeid(ViatronixChildWnd))
    {
      ViatronixChildWnd * pVCWnd = (ViatronixChildWnd*) (*it);
      ViatronChildView& childView = (ViatronChildView&)(GLChildViewRef(*pVCWnd));

      if (0)//typeid(childView) == typeid(OverView))
      {
        if (childView.GetDatasetRef().GetOrientation() == DatasetOrientations::eSUPINE)
        {
          if (!rdrGlobal.m_prone.m_submarine.IsFlying())
          {
            HWND hWnd = *(*it);
            ::SendMessage(hWnd, WM_PAINT, WPARAM(::GetDC(hWnd)), 0);
          }
        }
        else if (childView.GetDatasetRef().GetOrientation() == DatasetOrientations::ePRONE)
        {
          if (!rdrGlobal.m_supine.m_submarine.IsFlying())
          {
            HWND hWnd = *(*it);
            ::SendMessage(hWnd, WM_PAINT, WPARAM(::GetDC(hWnd)), 0);
          }
        }
      }
      else
      {
        HWND hWnd = *(*it);
        ::SendMessage(hWnd, WM_PAINT, WPARAM(::GetDC(hWnd)), 0);
      }
    }
  }
} // RenderControls


/**
 * Idle time processing
 */
void ColonViatronixWnd::GLIdle()
{
  ViatronixWnd::GLIdle();

  if (m_childRecordingMovieID != GLChildView::GLVID(0))
    ColonViatronixWnd::StoreMovieFrameInfo( this );
  
} // GLIdle


/**
 * Message: IDM_OPTIONS_SHOWMEASUREMENTPROFILES: Toggles display of measurement profiles for current user
 */
void ColonViatronixWnd::OnOptionsToggleMeasurementProfileDisplay()
{
  m_bDisplaying2DMeasurementProfile = !m_bDisplaying2DMeasurementProfile;
  ApplicationSettings::UserSettings().SetInt( APP_SETTING_SHOW_2D_MEASUREMENT_PROFILE, (m_bDisplaying2DMeasurementProfile ? 1 : 0));

  // redraw!
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnOptionsToggleMeasurementProfileDisplay


/**
 * Update Message: IDM_OPTIONS_SHOWMEASUREMENTPROFILES: Updates menu item for display of measurement profiles for current user
 *
 * @param pCmdUI    Command update instance
 */
void ColonViatronixWnd::OnUpdateOptionsToggleMeasurementProfileDisplay(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(m_bDisplaying2DMeasurementProfile ? 1 : 0);
} // OnUpdateOptionsToggleMeasurementProfileDisplay


/**
 * Message: IDM_OPTIONS_COLORSCHEME_1: Selects Color Scheme 1 (default)
 */
void ColonViatronixWnd::OnOptionsColorscheme1() 
{
  ApplicationSettings::UserSettings().SetInt( APP_SETTING_COLOR_SCHEME, int4(rdrGlobal.m_iColorScheme = 0));
  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnOptionsColorscheme1


/**
 * Message: IDM_OPTIONS_COLORSCHEME_2: Selects Color Scheme 2 (color blind scheme)
 */
void ColonViatronixWnd::OnOptionsColorscheme2() 
{
  ApplicationSettings::UserSettings().SetInt( APP_SETTING_COLOR_SCHEME, int4(rdrGlobal.m_iColorScheme = 1));
  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // OnOptionsColorscheme2


/**
 * Message: IDM_OPTIONS_COLORSCHEME_1: Updates check on color scheme menu items.
 *
 * @param pCmdUI   Command update instance.
 */
void ColonViatronixWnd::OnUpdateOptionsColorSchemes(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(rdrGlobal.m_iColorScheme == (pCmdUI->m_nID - IDM_OPTIONS_COLORSCHEME_1));
} // OnUpdateOptionsColorSchemes


/**
 * Menu command update handler for Add Snapshot menu item
 *
 * @param pCmdUI   Menu command update handler instance
 */
void ColonViatronixWnd::OnUpdateReportAddSnapshot(CCmdUI* pCmdUI)
{
  // Enable item if: There is at least one dataset view displayed, a study is loaded, and proper permissions on report exist
  pCmdUI->Enable(rdrGlobal.m_pCurrDataset->IsLoadingComplete()
    && ReportLayout::HasPermissions() && ReportLayout::CanModifyReport() && ReportLayout::CanOpenReport()
    && User::GetCurrentUser().IsAllowed("CompleteReport") );
} // OnUpdateReportAddSnapshot


/**
 * Message: IDM_REPORT_ADDSNAPSHOT - Default report add snapshot command handler (can be overridden in derived classes)
 */
void ColonViatronixWnd::OnReportAddSnapshot()
{
  // Add a snapshot of the current view and a blank view (there's no overview to display?)
  std::list<GLChildViewPtr> lViews;
  lViews.push_back(&GetCurrentView());
  AddReportSnapshot(lViews, rdrGlobal.m_pCurrDataset->m_session.m_measureLineList);
} // OnReportAddSnapshot


/**
 * Adds a report entry with snapshots from the specified list of views
 *
 * @param childViewList   Child views for snapshot
 */
void ColonViatronixWnd::AddReportSnapshot(const std::list<GLChildViewPtr>& childViewList, const MeasureLineList& currMeasureList /* = MeasureLineList()*/)
{
  AddReportSnapshot(childViewList, childViewList, currMeasureList);
} // AddReportSnapshot


/**
 * Adds a report entry with snapshots from the specified list of views
 *
 * @param snapshotList     Child views for snapshot
 * @param inverseImgList   Child views to display in reverse video during snapshot
 */
void ColonViatronixWnd::AddReportSnapshot(const std::list<GLChildViewPtr>& snapshotList, const std::list<GLChildViewPtr>& inverseImgList, const MeasureLineList& currMeasureList /* = MeasureLineList()*/)
{
  SetCursor(AfxGetApp()->LoadCursor(IDC_SNAPSHOT));

  int4 iRetVal = ReportLayout::AddReportEntry(snapshotList, currMeasureList);

  if (iRetVal != TRUE)
  {
    if (iRetVal == -1)
    {
      AfxMessageBox(LoadResourceString(IDS_ERROR_NO_REPORT_OPEN) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE));
    }
    else if (iRetVal == -2)
    {
      AfxMessageBox(LoadResourceString(IDS_ERROR_REPORT_READONLY) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE));
    }

    return;
  }
  else
  {
    // Play snapshot sound
    PlaySound(LPCSTR(IDW_SNAPSHOT), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);

    std::list<GLChildViewPtr>::const_iterator viewIt;

    // temporarily display captured views in reverse video
    for( viewIt = inverseImgList.begin(); viewIt != inverseImgList.end(); viewIt++ )
    {
      wglMakeCurrent(NULL,NULL);
      (*viewIt)->SetInverseImage( true );
      GLRenderView( **viewIt );
      wglMakeCurrent(NULL,NULL);
    }

    Sleep(100);

    for( viewIt = inverseImgList.begin(); viewIt != inverseImgList.end(); viewIt++ )
    {
      wglMakeCurrent(NULL,NULL);
      (*viewIt)->SetInverseImage( false );
      GLRenderView( **viewIt );
      wglMakeCurrent(NULL,NULL);
    }

    // Add temporary (quiet) notification that report snapshot has been added
    MainAppFrame::GetMainFrame()->PostMessage(WM_APP_DISPLAY_HELP_TEXT, IDS_REPORT_SNAPSHOT_ADDED, 1);
  }

  SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
} // AddReportSnapshot()


/**
 * Method to invoke the StoreMovieFrameInfo() method of the given ColonViatronixWnd instance
 */
uint4 ColonViatronixWnd::Run_StoreMovieFrameInfo(void * pvColonViatronixWnd)
{
  ColonViatronixWnd::m_movieTimer.Reset();

  ColonViatronixWnd* pColonViatronixWnd = ((ColonViatronixWnd*) pvColonViatronixWnd);
  if (pColonViatronixWnd)
  {
    while( WaitForSingleObject(pColonViatronixWnd->m_hSaveFrame, 0) == WAIT_TIMEOUT )
    {
      ColonViatronixWnd::StoreMovieFrameInfo(((ColonViatronixWnd*) pColonViatronixWnd));
      Sleep(25);
    }
  }

  return 1;
} // Run_StoreMovieFrameInfo


/**
 * Message: WM_USER_MOVIE_RECORD_MODE_QUERY: Query whether movie recording mode is on.
 *
 * @param wpQueryRecordStart   0 to simply query mode, non-zero indicates query is for starting a recording.
 * @param LPARAM               Unused.
 * @return                     Non-zero if movie recording mode is on, zero otherwise.
 */
LRESULT ColonViatronixWnd::OnMovieRecordModeQuery(WPARAM wpQueryRecordStart, LPARAM)
{
  return (m_childRecordingMovieID != GLChildView::GLVID(0));
} // OnMovieRecordModeQuery


/**
 * Message: WM_USER_MOVIE_RECORD_MODE_TOGGLE: Toggle on/off movie recording mode.
 *
 * @param WPARAM    Unused.
 * @param LPARAM    Unused.
 * @return          Unused, always 1.
 */
LRESULT ColonViatronixWnd::OnMovieRecordModeToggle(WPARAM,LPARAM)
{
  if (m_childRecordingMovieID == GLChildView::GLVID(0))
  {
    // Allow derivatives to indicate which view will be recorded; otherwise request user to specify
    m_childRecordingMovieID = SendMessage(WM_USER_MOVIE_RECORD_GET_VIEW_ID, 0, 0);
    if ((m_childRecordingMovieID == GLChildView::GLVID(0)) || (m_childRecordingMovieID == GetNullEventView().GetClassInstanceID()))
    {
      PickScreenPointDlg pickDlg;
      CPoint pt;

      if (pickDlg.DoModal() != IDOK)
      {
        return 1;
      }

      GetCursorPos(&pt);  ScreenToClient(&pt);
      if ( (m_childRecordingMovieID = GetChildViewFromPoint(Point2D<int4>(pt.x,pt.y)).GetClassInstanceID())
        == GetNullEventView().GetClassInstanceID())
      {
        m_childRecordingMovieID = GLChildView::GLVID(0);
        return 1;
      }
    }

    m_hSaveFrame = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_movie.m_frames.clear();
    m_movieTimer.Reset();
   // BackgroundProcess::Run(Run_StoreMovieFrameInfo, (void*) this, "Run_StoreMovieFrameInfo", eBKHIDE_NONE);
  }
  else
  {
    SetEvent(m_hSaveFrame);

    // Turn off autofly
    SetAutoflyMode(false);

    ViatronChildView& childView = ((ViatronChildView&)GetChildView(m_childRecordingMovieID));

    // Turn off movie recording mode
    m_childRecordingMovieID = GLChildView::GLVID(0);

    // Movie file setup
    vx::File tempMovieFile(vx::File::CreateTempFile("v3d"));
    tempMovieFile.Delete();

    std::string sTempMovieFile(tempMovieFile.ToString());
    m_movie.m_ssAVIFile << sTempMovieFile.c_str() << std::string(vx::AVIExt).c_str() << std::ends;
    m_movie.m_ssTempWMVFile << sTempMovieFile.c_str() << std::string(vx::WindowsMediaVideoExt).c_str() << std::ends;

    DeleteFile(m_movie.m_ssAVIFile.str());  DeleteFile(m_movie.m_ssTempWMVFile.str());

    // Get saved movie format settings for user
    std::string sInFormat;
    ApplicationSettings::UserSettings().TryGet( APP_SETTING_MOVIE_FORMAT, sInFormat );

    // Initialize movie format options
    MovieCreateDialog videoDlg;//(childView);

    if (!sInFormat.empty())
    {
      std::istrstream ssInFormat(sInFormat.c_str());
      ssInFormat >> videoDlg.m_format;
      ssInFormat.rdbuf()->freeze(false);
    }

    videoDlg.m_format.m_sFileTitle = LPCTSTR("Movie-" + COleDateTime::GetCurrentTime().Format("%Y-%m-%d-%H-%M-%S"));

    // Show movie format options dialog
    if (videoDlg.DoModal() != IDOK)
    {
      m_movie.m_frames.clear();
      AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT, 0, 0);
      return 1;
    }

    // Include overview if requested and the selected child is not an overview
    videoDlg.m_format.m_bIncludeOverview &= (typeid(childView) != typeid(OverView))? true : false;

    // Copy user-selected settings
    m_movie.m_format = videoDlg.m_format;
    m_movie.m_eType = childView.GetViewWndType();
    m_movie.m_bFullOblique = (typeid(childView) == typeid(SliceChildView)) ? ((SliceChildView&)childView).GetFullObliqueMode() : false;

    // Save movie format settings for user across sessions
    std::ostrstream ssOutFmt;
    ssOutFmt << m_movie.m_format << std::ends;
    ApplicationSettings::UserSettings().Set( APP_SETTING_MOVIE_FORMAT, ssOutFmt.str() );
    ssOutFmt.freeze(false);

    // Switch to movie creation pane here
    AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE, MAKELPARAM(VFrameManager::NavFrameManager::ePOS_MOVIE,0));

    AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT, 0, 0);
  }

  return 1;
} // OnMovieRecordModeToggle


/**
 * Subtract two normals.
 *
 * @param l   Left-hand side.
 * @param r   Right-hand side.
 * @return    l-r
 */
template <class T>
Vector<T> operator-(const Normal<T> & l, const Normal<T> & r)
{
  return Vector<T>(l.X()-r.X(), l.Y()-r.Y(), l.Z()-r.Z());
} // operator-


/**
 * Save information on a movie frame.
 */
void ColonViatronixWnd::StoreMovieFrameInfo(ColonViatronixWnd* pWnd)
{
  if (pWnd->m_childRecordingMovieID <= 0)
  {
    return;
  }

  ViatronChildView& childView = dynamic_cast<ViatronChildView&>(pWnd->GetChildView(pWnd->m_childRecordingMovieID));
  if (childView.GetDatasetPtr() == NULL)
  {
    throw ex::NullPointerException(LogRec("An invalid view for movie recording was specified",
      "ColonViatronixWnd", "StoreMovieFrameInfo"));
  }

  Dataset & dataset = childView.GetDatasetRef();
  Submarine & sub = dataset.m_submarine;

  // if there is anything in the FIFO already
  if (m_movie.m_frames.size() > 0) 
  {
    switch (ViatronStudy::m_modeEndoView.GetMutexModes())
    {
      case ViewModes::BIOPSY:
      {
        MovieFrame& rPrev = m_movie.m_frames.front();
        Point<int4>& rMagicWinPt = dataset.m_fields.m_volumeRender.m_magicWindowStartPt;
        if ((rPrev.m_eFrameType == MovieFrame::TRANSLUCENCY) && rPrev.m_ptTransLoc.m_x == rMagicWinPt.m_x  && rPrev.m_ptTransLoc.m_y == rMagicWinPt.m_y )
        {
          break;
        }

        MovieFrame frame;
        frame.m_eDatasetOrientation = dataset.GetOrientation();
        frame.m_eFrameType = MovieFrame::TRANSLUCENCY;
        frame.m_ptTransLoc = Point2D<int4>(rMagicWinPt.m_x, rMagicWinPt.m_y);
        frame.m_camera = Camera(sub.GetPositionWorld(), sub.GetOrientation(), sub.GetUpDirection());
        frame.m_bSpaceLeaping = rdrGlobal.m_bSpaceLeaping;
        frame.m_uSampleTime = ColonViatronixWnd::m_movieTimer.ElapsedTime() * 1000;
        frame.m_bShow2DCamera = SliceChildView::GetIsCameraShowing();

        m_movie.m_frames.push_back(frame);
        pWnd->m_timerPauseInMovie.ResetTimer(pWnd, ePauseFrameElapseTimeMS);
        break;
      }
      case ViewModes::LINEMEASURE:
      {
        break;
      }
      default:
      {
        //const Vector<float4> vPos(sub.GetPositionWorld() - m_movie.m_frames.front().m_camera.GetVrp());
        //const Vector<float4> vOrn(sub.GetOrientation() - m_movie.m_frames.front().m_camera.GetVpn());
        //const Vector<float4> vUp (sub.GetUpDirection() - m_movie.m_frames.front().m_camera.GetVup());

        //const float4 fMinDist(0.1F);  // world coordinates (in mm)
        //// check to see if we moved outside of a distance threshold
        //if ((vPos.GetLength() > fMinDist) || (vOrn.GetLength() > 0.01F) || (vUp.GetLength() > 0.01F))
        {
          // add it to the front of the FIFO
          MovieFrame frame;
          frame.m_eDatasetOrientation = dataset.GetOrientation();
          frame.m_camera = Camera(sub.GetPositionWorld(), sub.GetOrientation(), sub.GetUpDirection());
          frame.m_bSpaceLeaping = rdrGlobal.m_bSpaceLeaping;
          frame.m_uSampleTime = ColonViatronixWnd::m_movieTimer.ElapsedTime() * 1000;

          frame.m_bShow2DCamera = SliceChildView::GetIsCameraShowing();

          m_movie.m_frames.push_back(frame);
          pWnd->m_timerPauseInMovie.ResetTimer(pWnd, ePauseFrameElapseTimeMS);
        }

        break;
      }
    }
  }
  else 
  {
    // nothing in the FIFO yet, just add it
    MovieFrame frame;
    frame.m_eDatasetOrientation = dataset.GetOrientation();
    frame.m_camera = Camera(sub.GetPositionWorld(), sub.GetOrientation(), sub.GetUpDirection());

    m_movie.m_frames.push_back(frame);
    pWnd->m_timerPauseInMovie.ResetTimer(pWnd, ePauseFrameElapseTimeMS);
    ColonViatronixWnd::m_movieTimer.Reset();
  } // end if anything in the FIFO yet

#ifndef FINAL_RELEASE
  float4 fSeconds = float4(m_movie.m_frames.size()+eOpeningCreditsLength) / float4(10);
  CString sStats;
  sStats.Format(IDS_MOVIE_STATS_FORMAT, int4(fSeconds/60.0), int4(fmod(fSeconds,60)), int4(fSeconds*50.0));
//  MainAppFrame::GetMainFrame()->DisplayFunctionAndStepText(std::string(), LPCTSTR(sStats));
#endif
} // StoreMovieFrameInfo


/**
 * Synchronizes toggle of view modes between views
 *
 * @param eMode   Mode being toggled
 */
void ColonViatronixWnd::ToggleViewMode(const ViewModes::VIEWMODES & eMode)
{

  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;

  if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !bAutoFly)    // replace with IsManualFlying() or like
  {
    return;
  }

  if (bAutoFly)
  {
    OnToggleAutofly();
  }

  if ( (ViatronStudy::m_modeOrthogView & eMode) // ViewModes::LINEMEASURE)
    && (ViatronStudy::m_modeObliqueView & eMode) // ViewModes::LINEMEASURE)
    && (ViatronStudy::m_modeEndoView & eMode) ) // ViewModes::LINEMEASURE))
  {
    ViatronStudy::m_modeEndoView  -= eMode; // ViewModes::LINEMEASURE;
    ViatronStudy::m_modeOrthogView -= eMode; // ViewModes::LINEMEASURE;
    ViatronStudy::m_modeObliqueView -= eMode; // ViewModes::LINEMEASURE;
  }
  else
  {
    ViatronStudy::m_modeEndoView  += eMode; // ViewModes::LINEMEASURE;
    ViatronStudy::m_modeOrthogView += eMode; // ViewModes::LINEMEASURE;
    ViatronStudy::m_modeObliqueView += eMode; // ViewModes::LINEMEASURE;
  }

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
} // ToggleViewMode()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION: Toggles Region Segment Mode.
 */
void ColonViatronixWnd::OnToggleSegmentRegion()
{
  ToggleViewMode(ViewModes::SEGMENTPOLYP);
} // OnToggleTranslucent()


/**
 * Show Missed regions dialog
 */
void ColonViatronixWnd::OnOptionsControlPadShowMissedRegions()
{
  OnMissedRegions_And_CadResults_Select(IDC_TEMPLATE_MISSED_REGIONS_DLG);
} // OnOptionsControlPadShowMissedRegions


/**
 * Show CAD dialog
 */
void ColonViatronixWnd::OnOptionsControlPadShowCADResults()
{
  OnMissedRegions_And_CadResults_Select(IDC_TEMPLATE_CADRESULTSDLG);
} // OnOptionsControlPadShowCADResults


/**
 * Toggle between dialogs
 */
void ColonViatronixWnd::OnMissedRegions_And_CadResults_Select(const uint4 uID)
{
  const bool bCad( rdrGlobal.m_supine.IsCadFindingsLoaded() || rdrGlobal.m_prone.IsCadFindingsLoaded() );
  CWnd * pCadWnd = GetDlgItem(IDC_TEMPLATE_CADRESULTSDLG);
  CWnd * pMsdWnd = GetDlgItem(IDC_TEMPLATE_MISSED_REGIONS_DLG);

  if ( bCad && pCadWnd && pMsdWnd )
  {
    // split up these two cases, instead of using an inline if, so as to show the hidden dialog first.
    //  this prevents the split-second of time where the two dialogs are hidden (leading to a flash or blank space)
    if ( uID == IDC_TEMPLATE_CADRESULTSDLG )
    {
      pCadWnd->ShowWindow(SW_SHOW);
      pMsdWnd->ShowWindow(SW_HIDE);
      m_cadMonitor.On();
    }
    else
    {
      pMsdWnd->ShowWindow(SW_SHOW);
      pCadWnd->ShowWindow(SW_HIDE);
      m_cadMonitor.Off();
    }
  }
	else if( bCad && pCadWnd && uID == IDC_TEMPLATE_CADRESULTSDLG )
	{
		pCadWnd->ShowWindow(SW_SHOW);
	}
	else if( pMsdWnd )
	{
		pMsdWnd->ShowWindow(SW_SHOW);
	}		

} // OnTemplateDialogToggle()


/**
 * Update routine for the menu options
 * @param pCmdUI    Command update instance
 */
void ColonViatronixWnd::OnUpdateOptionsControlPadMissedCAD(CCmdUI* pCmdUI)
{
  const bool bCad( rdrGlobal.m_supine.IsCadFindingsLoaded() || rdrGlobal.m_prone.IsCadFindingsLoaded() );
  CWnd * pCadWnd = GetDlgItem(IDC_TEMPLATE_CADRESULTSDLG);
  CWnd * pMsdWnd = GetDlgItem(IDC_TEMPLATE_MISSED_REGIONS_DLG);

  CRect rtCad(0,0,0,0);
	if( pCadWnd != nullptr )
	  rtCad = pCadWnd ? WinUtils::GetWindowRect(pCadWnd) : CRect();
  CRect rtMsd(1,1,1,1);
	if( pMsdWnd != nullptr )
	  rtMsd = pMsdWnd ? WinUtils::GetWindowRect(pMsdWnd) : CRect();
  bool bToggleNeeded = bCad && !rtCad.IsRectEmpty() && !rtMsd.IsRectEmpty() && (CRect().IntersectRect(rtCad,rtMsd) != FALSE);

	/// This must be done everytime. Otherwise, the submenu will disable after user switch from a tab without cad pane 
	/// to a tab that has both cad and missed region pane. 03/30/2012
	pCmdUI->Enable(TRUE);

  if ( bToggleNeeded )
  {
    const bool bMissedVisible(pMsdWnd->IsWindowVisible());
    pCmdUI->SetCheck( (pCmdUI->m_nID == ID_OPTIONS_CONTROLPAD_CADRESULTS) == (pCadWnd->IsWindowVisible()==TRUE) ? 1 : 0 );
  }
  else
  {
    pCmdUI->Enable(FALSE);
  }
} // OnUpdateOptionsControlPadMissedCAD(CCmdUI* pCmdUI)


/**
 * Gets tool tip text
 *
 * @param uID          Control ID
 * @param pTTTStruct   Pointer to tool tip structure
 * @param pResult      Result
 * @return             TRUE if text is set
 */
BOOL ColonViatronixWnd::OnToolTipNeedText(UINT uID, NMHDR *pTTTStruct, LRESULT *pResult)
{
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
  UINT uIDFrom = pTTTStruct->idFrom;
  if (pTTT->uFlags & TTF_IDISHWND)
  {
    // uIDFrom is actually the HWND of the tool
    uIDFrom = ::GetDlgCtrlID((HWND)uIDFrom);
    if (uIDFrom == 0)
    {
      return FALSE;
    }
    else
    {
      pTTT->lpszText = MAKEINTRESOURCE(uIDFrom);
    }

    pTTT->hinst = AfxGetResourceHandle();
    return TRUE;
  }

  return FALSE;
} // OnToolTipNeedText()


/**
 * Notification handler for dataset locked changes
 *
 * @param           args                  DatasetsLockedChangedArgs
 */
void ColonViatronixWnd::OnNotified( const Colon::DatasetsLockedArgs & args )
{
  if( IsDatasetsLockingEnabled() )
    OnDatasetsLocked( args );

	/// The bottom status bar does not need to display the link icon in dual display
	/// layouts since the big ling skin button is on the top already. The tricky part
	/// is how to determine at this point which layout tab is the current Frame. The
	/// following implementation may not be the optimal implementation. 20120125 
	if( MainAppFrame::GetMainFrame()->GetNavigationBar().GetItemCount() == 5 && 
		  MainAppFrame::GetMainFrame()->GetNavigationBar().GetSelectedIndex() >= 3 )
  { /// This is for Navigation workflow tab
    MainAppFrame::GetMainFrame()->GetViewSelectDlgBar().SetLinkStateDisplay( false );
  } // if

	if( MainAppFrame::GetMainFrame()->GetNavigationBar().GetItemCount() == 2 && 
		  MainAppFrame::GetMainFrame()->GetNavigationBar().GetSelectedIndex() == 1 )
  { /// This is for Verification workflow tab
    MainAppFrame::GetMainFrame()->GetViewSelectDlgBar().SetLinkStateDisplay( false );
  } // if
} // OnNotified( args )


/**
 * Notification handler for fullscreen changes
 *
 * @param           args                  FullScreenArgs
 */
void ColonViatronixWnd::OnNotified( const Colon::FullScreenArgs & args )
{
  if( args.IsFullScreen() )
  {
    // this notification event occurred immediately (view changing)
    Colon::FullScreenController::GetInstance().PersistLayout( *this );
  }
  else
  {
    // this notification event was delayed until the mainappframe changed the view (view changed)
    Colon::FullScreenController::GetInstance().SetupLayout( *this );
  }
} // OnNotified( args )

