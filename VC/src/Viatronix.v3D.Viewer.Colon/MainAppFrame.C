// $Id: MainAppFrame.C,v 1.31.2.27 2010/04/17 08:12:55 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MainAppFrame.C  
// Description: Main frame window for v3D Colon application
// Owner: Jeffrey Meade  jmeade@viatronix.net


#include "stdafx.h"
#include "Viatron.h"
#include "MainAppFrame.h"
#include "ServiceLocator.h"

#include "ViatronStudy.h"
#include "V1KMessages.h"

#include "PatientInfoDlg.h"

#include "resource.h"
#include "SkinButton.h"

#include "PreferencesDlg.h"
#include "ReportViewWnd.h"
#include "FunctionProgress.h"

#include "File.h"
#include "Crypt.h"

#include "ProgressDlg.h"
#include "ApplicationSettings.h"
#include "ProgressDlg.h"
#include "ThreadTask.h"

#include "SliceChildView.h"
#include "User.h"

#include "Theme.h"
#include "ViatronSplash.h"
#include "MonitorDisplay.h"
#include "vcUtils.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonViews::Layouts;
using namespace ColonViews::FramesManager;
using namespace ColonViews::Dialogs;
using namespace WinUtils;
using namespace ReaderLib;

#define FILE_REVISION "$Revision: 1.31.2.27 $"

/////////////////////////////////////////////////////////////////////////////
// MainFrame

std::shared_ptr< Loader > MainAppFrame::m_loader = nullptr;

IMPLEMENT_DYNAMIC(MainAppFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(MainAppFrame, CMDIFrameWnd)
  
  // windows messages
  ON_WM_CREATE()
  ON_WM_SETCURSOR()
  ON_WM_TIMER()
  ON_WM_SIZING()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_WM_TIMECHANGE()
  ON_WM_MOVE()
  
  // menu commands
  ON_COMMAND(IDM_STUDY_CLOSE, OnMenuStudyClose)
  ON_COMMAND(IDM_USERS, OnUsersMenu)
  ON_COMMAND(IDM_USR_LOGOUT, OnLogoutUser)
  ON_COMMAND(IDM_USR_PASSCHANGE, OnChangeUserPassword)
  ON_COMMAND(IDM_PROGRAM_PREFERENCES, OnProgramPreferences)
  ON_UPDATE_COMMAND_UI(IDM_USR_LOGOUT, OnUpdateUsrLogoutMenu)
  ON_UPDATE_COMMAND_UI(IDM_USERS, OnUpdateUserAdminMenu)
  ON_UPDATE_COMMAND_UI(IDM_USR_PASSCHANGE, OnUpdateUsrPasschangeMenu)
  ON_UPDATE_COMMAND_UI(IDM_PROGRAM_PREFERENCES, OnUpdateProgramPreferences)
 
  // global help commands
  ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
  ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
  ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
  ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)

  // registered Windows messages
  ON_REGISTERED_MESSAGE(FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS, OnSetFunctionProgress)
  ON_REGISTERED_MESSAGE(FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT, OnDisplayFunctionAndStepText)
  ON_REGISTERED_MESSAGE(BackgroundProcess::WM_REG_BACKGROUNDPROC_STARTFINISH, OnBackgroundProcessStartFinish)

  // private window msgs (copies of registered windows msgs, to prevent infinite loops when posting from children)
  ON_MESSAGE(WM_USER_SET_FUNCTION_PROGRESS, OnSetFunctionProgress)  // FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS
  ON_MESSAGE(WM_USER_DISPLAY_FUNCTION_TEXT, OnDisplayFunctionAndStepText) // FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT

  // application messages
  ON_MESSAGE(WM_APP_LOAD_VIATRON_STUDY_DONE, OnLoadViatronStudyProcDone)
  ON_MESSAGE(WM_APP_VIEW_SELECTION_CHANGING, OnViewSelectionChanging)
  ON_MESSAGE(WM_APP_VIEW_SELECTION_CHANGED, OnViewSelectionChanged)
  ON_MESSAGE(WM_APP_STUDY_LOAD_STEP_NOTIFY, OnStudyLoadStepNotify)
  ON_MESSAGE(WM_APP_DISPLAY_HELP_TEXT, OnDisplayHelpText)
  ON_MESSAGE(WM_APP_SET_MAIN_DATASET, OnSetMainDataset)
  ON_MESSAGE(WM_APP_VIEWMODE_CHANGED, OnAppViewModeChanged)

  // accelerator key messages
  ON_COMMAND(IDM_CANCEL_LOAD, OnCancelLoad)
  ON_COMMAND(IDM_SHOW_NEXT_VIEW, OnShowNextView)

END_MESSAGE_MAP()

static UINT vStatusBarIndicators[] =
{
  ID_SEPARATOR,
  IDS_INDICATOR_FUNCTION,
  IDS_INDICATOR_STEP,
  IDS_INDICATOR_DATETIME,
};

/**
 * Constructor.
 */
MainAppFrame::MainAppFrame() : 
  m_iCurrMainView(-1), 
  m_iCurrSubView(-1)
{

  m_bAutoMenuEnable  = FALSE;
  
  m_hMDIMenu  = NULL;
  m_hMDIAccel = NULL;

  // TODO: add member initialization code here
  m_pCurrentViewFrame = (ChildLayoutFrame *) NULL;

  m_hMainVtxFont = m_hLargeVtxFont = HFONT(NULL);

  m_pCurrFrame = VFrameManagerPtr(NULL);
  m_pCurrClass = RUNTIME_CLASS(CObject);

  m_previousView = std::make_pair( -1, -1 );

	m_bShowLinkState = false;

} // constructor


/**
 * Destructor.
 */
MainAppFrame::~MainAppFrame()
{
 
  SliceChildView::StoreUserPreferredDisplayOptions();
	//ApplicationSettings::UserSettings().SetBool( APP_SETTING_TOOLBAR_POSITION, rdrGlobal.m_bToolbarOnBottomForWideScreen );

  if (m_hMainVtxFont != HFONT(NULL))
    DeleteObject(m_hMainVtxFont);

  if (m_hLargeVtxFont != HFONT(NULL))
    DeleteObject(m_hLargeVtxFont);

  if (m_hMDIMenu != NULL)
    DestroyMenu(m_hMDIMenu);

  if (m_hMDIAccel != NULL)
    FreeResource(m_hMDIAccel);

} // destructor


/**
 * Window creation handler.
 *
 * @param lpCreateStruct   Create parameters.
 * @return                 Zero indicates success, -1 failure.
 */
int MainAppFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  // create the views
  HINSTANCE hInst = AfxGetResourceHandle();
  m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE( IDR_MAINFRAME ));
  m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));


  // remove the client edge from the mdi client window
  SetWindowLong( m_hWndMDIClient, GWL_EXSTYLE, GetWindowLong( m_hWndMDIClient, GWL_EXSTYLE ) & ~WS_EX_CLIENTEDGE );

  const HWND hWndThis = GetSafeHwnd();
  m_studyFrame.Create(hWndThis);
  m_verifyFrame.Create(hWndThis);
  m_navigationFrame.Create(hWndThis);
  m_reportFrame.Create(hWndThis);

  // Create status bar
  if (!m_statusBar.Create(this) || !m_statusBar.SetIndicators(vStatusBarIndicators,
                                      sizeof(vStatusBarIndicators)/sizeof(UINT)))
  {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_statusBar.ShowWindow( SW_HIDE );

  // Displays (*and Updates*) time in status bar
  OnTimer(m_timerUpdateTime);

  // view selection bar
  if( !m_viewSelBar.Create( NULL, "", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 1, 1 ), this, AFX_IDW_DIALOGBAR + 1 ) )
    return -1;

  // navigation bar
  if( !m_navigationBar.Create( NULL, "", WS_CHILD|WS_VISIBLE, CRect( 0, 0, 1, 1 ), this, AFX_IDW_DIALOGBAR + 2 ) )
    return -1;

  if( !m_rightBorder.Create( NULL, "", WS_CHILD | WS_VISIBLE, CRect( 0, 0, 1, 1 ), this,  AFX_IDW_DIALOGBAR + 3 ) )
    return -1;

  CMenu * pMainMenu = GetMenu();
  if (User::GetCurrentUser().GetType() != User::UserType_Service)
  {
    pMainMenu->DeleteMenu(ID_OPTIONS_DISPLAYRENDERSPEED, MF_BYCOMMAND);
  }


#if defined(_CAPTIONLESS_MENULESS_MAINWINDOW_) || 0
  CRect rect;
  GetClientRect(rect);
  ShowWindow(SW_SHOW);
  MoveWindow(0, -40, rect.right, rect.bottom, TRUE);
#endif

  // Create fonts for use in view windows
  m_hMainVtxFont = CreateFont(-16, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE,
    ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
    DEFAULT_PITCH, "tahoma");

  m_hLargeVtxFont = CreateFont(-20, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE,
    ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
    DEFAULT_PITCH, "tahoma");

  CClientDC dc(this);
  SkinButton::SetHelpTextMessageID(WM_APP_DISPLAY_HELP_TEXT);
  SkinButton::SetDefaultButtonResourceID(IDB_BUTTON_MEDIUM);

  //======================================================================================================================
  // Enable the tab for the study browser view if the launched from console flag os set (-console was on command line)
  // The last digit in he string identifies the study browser (console)  tab.
  //======================================================================================================================
  std::stringstream enableBits;
  enableBits << "0001" << ( ViatronApp::GetViatronApp()->GetCommandLine().GetLaunchedFromConsole() ) ? "1" : "0";
  m_viewSelBar.EnableItems(ViewSelectDlgBar::EnableViewsBitSet( enableBits.str() ));
  
//  m_menubar.Attach( this );

  // start the license renewal timer
  m_timerUpdateLocationLock.SetTimer( this, ColonLicense::RenewalInterval );

  return 0;
} // OnCreate()


/**
 * Must override this function and call CreateClient() ourself.
 *  Otherwise, the framework will do all kinds of strange things to our menus,
 *  believing that one of the items is the standard "Window" menu.
 *  (See MFC code, WinMDI.cpp line 130)  "Windows user interface guidelines" my ass!
 *  How about some flexibility?
 *
 * @param lpcs       Create parameters.
 * @param pContext   Create context.
 * @return           True/false indicating success/failure.
 */
BOOL MainAppFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  return CreateClient(lpcs, &CMenu());
} // OnCreateClient


/**
 * Sets parameters for window creation
 *
 * @param cs   Create parameters (in/out)
 * @return     Whether to continue with window create
 */
BOOL MainAppFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  if ( !CMDIFrameWnd::PreCreateWindow(cs) )
    return FALSE;
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

  CRect rect;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

  cs.cx = rect.Width(); cs.cy = rect.Height();

  if( User::GetCurrentUser().GetType() == User::UserType_Service )
  {
    cs.style |= WS_THICKFRAME;
  }

  return TRUE;
} // PreCreateWindow()


/**
 * Handles selection of a new view window.
 *
 * @param iMainView        Main tab selection.
 * @param iSubViewChange   Low word is new subview, high word is previous subview (from/in navigation tab).
 * @param bInitStudy       Whether or not to send initialize study message to new view.
 */
void MainAppFrame::ShowView(const int4 iMainView, const int4 iSubViewChange, const bool bInitStudy)
{
#ifdef FINAL_RELEASE
  try
#endif
  {
    if (ViatronStudy::m_modeEndoView.GetMutexModes() != ViewModesValue(0))
    {
      ViatronStudy::m_modeEndoView -= ViewModes::VIEWMODES(ViatronStudy::m_modeEndoView.GetMutexModes());
    }
    if (ViatronStudy::m_modeOrthogView.GetMutexModes() == ViewModes::AUTOFLY2D)
    {
      ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY2D;
    }
    if (ViatronStudy::m_modeObliqueView.GetMutexModes() == ViewModes::AUTOFLY2D)
    {
      ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY2D;
    }

    VFrameManagerPtr pFrameManager = VFrameManagerPtr(NULL);

    switch (iMainView)
    {
      case -1:  // No change in view frame manager (changing subview)
        pFrameManager = m_pCurrFrame;
        break;

      case 0:   // Study browser view
        pFrameManager = &m_studyFrame;
				m_bShowLinkState = false;
        break;

      case 1:   // Study verification view
        pFrameManager = &m_verifyFrame;
				m_bShowLinkState = false;
				EnableMenuBar( false );
        break;

      case 2:   // Navigation views
        pFrameManager = &m_navigationFrame;
				m_bShowLinkState = true;
				EnableMenuBar( true );
        break;

      case 3:   // Report view
        pFrameManager = &m_reportFrame;
				m_bShowLinkState = false;
				EnableMenuBar( false );
        break;
      case 4:   // Mark study complete
        OnStudyComplete();
        m_viewSelBar.SetCurrentViewSelection(m_iCurrMainView);
        break;

      default:
        return;
    } // end-switch

    if (pFrameManager == VFrameManagerPtr(NULL))
    {
      return;
    }

    int4 iPreviousSubView = (iSubViewChange >> 16); // high word is previous selection
    
    // sub view changing notification
    Notify( MainAppFrameStates_SubViewChanging );

    stdStringVector vTabStrings;
    bool bNewFrame = false;
    if (m_pCurrFrame != pFrameManager)
    {
      // Notify the previous frame manager that it's about to "lose focus"
      if (m_pCurrFrame)
      {
        int4 iNoNewView(-1);
        m_pCurrFrame->OnChangeViewSel(iPreviousSubView, iNoNewView, ViatronFrameManager::eFocusLose);
      }

      // Notify the new frame manager, get strings for navigation bar
      pFrameManager->OnFrameInit(vTabStrings);

      // Set strings in navigation bar
      m_navigationBar.SetNavigationTabs(vTabStrings);

      // No logical previous view when frame manager is changing
      iPreviousSubView = -1;

      bNewFrame = true;
    }

    // Clear any residual function/step text
    OnDisplayFunctionAndStepText(0,0);

    // Notify frame manager of view selection change, receive new view class
    int4 iNewSubView((iSubViewChange & 0x0000ffff));
    CRuntimeClass * pViewClass = pFrameManager->OnChangeViewSel(iPreviousSubView, iNewSubView,
      (iMainView != -1) ? ViatronFrameManager::eFocusGain : ViatronFrameManager::eFocusViewChange);
		
		LPCTSTR sLayoutName = pViewClass->m_lpszClassName;

    // only switch to a new view if necessary
    if (pViewClass && m_pCurrClass && (bNewFrame || (pViewClass != m_pCurrClass)))
    {
      ChildLayoutFrame * pOldFrame = m_pCurrentViewFrame;
      
      // Destroy the current view
      if( pOldFrame != NULL )
      {
        pOldFrame->EnableWindow( false );
        pOldFrame->GetActiveLayout()->OnLayoutChanging();
      }

      // Create the new view
       m_pCurrentViewFrame = (ChildLayoutFrame *)
        CreateNewChild(pViewClass, IDR_MAINFRAME, m_hMDIMenu, m_hMDIAccel);
      m_pCurrClass = pViewClass;

      // Destroy the current view
      if( pOldFrame != NULL )
      {
        pOldFrame->GetActiveLayout()->OnLayoutChanged();
        pOldFrame->ShowWindow( SW_HIDE );
        pOldFrame->DestroyWindow();
      }
    } // if switching views

    // Set the patient name and ID in the title
    SetPatientInfoInTitle();

    // Set the current view tab
    m_navigationBar.SetCurrentViewSelection(iNewSubView);

    // Set the new view ("frame") selection in the tab dialog bar
    if (pFrameManager != m_pCurrFrame)
    {
      m_viewSelBar.SetCurrentViewSelection(iMainView);
    }

    // Store the current selections
    m_pCurrFrame = pFrameManager;
    if ( iMainView != -1 )
      m_iCurrMainView = iMainView;
    m_iCurrSubView = iNewSubView;

    MainAppFrame::GetMainFrame()->GetCurrentLayout().BeginInitialUpdate();
    GetCurrentLayout().SendMessage(WM_APP_STUDY_LOAD_STEP_NOTIFY, StudyLoad::eINITIALIZE, StudyLoad::eLOAD_COMPLETE);

    // sub view changed notification
    Notify( MainAppFrameStates_SubViewChanged );

		/// The dual dataset layout does not need the link icon at the bottom status bar
		if( (m_navigationBar.GetItemCount() == 5 && iNewSubView >= 3) || (sLayoutName == "MovieLayoutFrame") ||
			  sLayoutName == "FullObliqueLayoutFrame" )
		{
			m_bShowLinkState = false;

			if( sLayoutName == "MovieLayoutFrame" )
				DisableMenuBar();
		}
		else if( m_navigationBar.GetItemCount() == 5 && m_navigationBar.GetSelectedIndex() < 3 )
			m_bShowLinkState = true;

		m_viewSelBar.SetLinkStateDisplay( m_bShowLinkState );

  } // end-try
#ifdef FINAL_RELEASE
  catch (CMemoryException *)
  {
    CString sTmp;
    sTmp.Format("ShowView(%d,%d): A MemoryException has occurred: error switch views", iMainView, iSubViewChange);
    AfxMessageBox(IDS_ERROR_SWITCHVIEW_EXCEPTION);
    ex::OutOfMemoryException(LogRec(sTmp.GetBuffer(sTmp.GetLength()), "MainAppFrame", "ShowView"));
  }
  catch (CException *)
  {
    CString sTmp;
    sTmp.Format("ShowView(%d,%d): A runtime exception (in MFC) has occurred: error switch views", iMainView, iSubViewChange);
    // Error::WarnLog(LPCTSTR(sTmp));
    AfxMessageBox(IDS_ERROR_SWITCHVIEW_EXCEPTION);
    ex::RuntimeException(LogRec(sTmp.GetBuffer(sTmp.GetLength()), "MainAppFrame", "ShowView"));
  }
  catch (...)
  {
    CString sTmp;
    sTmp.Format("ShowView(%d,%d): An unknown runtime exception has occurred: errro switch views", iMainView, iSubViewChange);
    // Error::WarnLog(LPCTSTR(sTmp));
    AfxMessageBox(IDS_ERROR_SWITCHVIEW_EXCEPTION);
    ex::RuntimeException(LogRec(sTmp.GetBuffer(sTmp.GetLength()), "MainAppFrame", "ShowView"));
  }
#endif
} // ShowView()


/**
 * Returns a reference to the current MDI child view.
 *
 * @return   Reference to the current MDI child view.
 */
GLWndRef MainAppFrame::GetCurrentLayout() const
{
  static ViatronixChildWnd nullLayout(0);
  if ( (m_pCurrentViewFrame == NULL) || (m_pCurrentViewFrame->GetActiveLayout() == NULL ) )
  {
    return (nullLayout);
  }

  return (*m_pCurrentViewFrame->GetActiveLayout());
} // GetCurrentLayout()


/**
 * Reloads the current layout
 */
void MainAppFrame::ReloadCurrentLayout()
{
  SendMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE, MAKELPARAM(VFrameManager::NavFrameManager::ePOS_BLANKVIEW, 0));
  SendMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE, MAKELPARAM(0, VFrameManager::NavFrameManager::ePOS_BLANKVIEW));
} // ReloadCurrentLayout()


/////////////////////////////////////////////////////////////////////////////
// 


/**
 * Gets the window's client rectangle, overridden because default handler
 *  doesn't account for dialog bars in calculation.
 *
 * @param pRect   Gets the client area of the main window.
 */
void MainAppFrame::GetClientRect(LPRECT pRect)
{
  CMDIFrameWnd::GetClientRect(pRect);

  // Default client rect handler doesn't account for dialog bars

  //// view select dialog bar (on bottom)
  //if (IsWindow(m_viewSelBar))
  //{
  //  CRect vsbRect;  m_viewSelBar.GetClientRect(vsbRect);
  //  pRect->bottom -= vsbRect.bottom+1;   // bottom in client coordinates = Height()
  //}

  //// status bar (on bottom)
  //if (IsWindow(m_statusBar.GetSafeHwnd()))
  //{
  //  CRect sbRect; m_statusBar.GetClientRect(sbRect);
  //  pRect->bottom -= sbRect.bottom+1;    // bottom in client coordinates = Height()
  //}

  //// navigation bar (on LHS)
  //if (IsWindow(m_navigationBar.GetSafeHwnd()))
  //{
  //  CRect nbRect; m_navigationBar.GetClientRect(nbRect);
  //  pRect->right -= nbRect.right+2;    // right in client coordinates = Width()
  //}
} // GetClientRect()

/////////////////////////////////////////////////////////////////////////////
// 


/**
 * Called when closing window.
 */
void MainAppFrame::OnClose() 
{
  // TODO: REST we should no longer need the link back to console
  //if( ViatronApp::GetViatronApp()->GetCommandLine().GetLaunchedFromConsole() )
  //{
  //  GetCoClass< ComServer >( ViatronApp::GetViatronApp()->GetServer() )->SetViewerClosing();
  //  GetCoClass< ComServer >( ViatronApp::GetViatronApp()->GetServer() )->UnloadServer();
  //}

  // Abort if study is currently loading
  if (rdrGlobal.IsStudyLoading())
  {
    OnCancelLoad();
    return;
  }


  // Allow another instance to be instantiated asap
  ViatronApp::CloseAppMutexHandle();

  // Hide window immediately to remove from user's view
  ShowWindow(SW_HIDE);

  // Wait for any running threads to exit
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

#if 1
  if (!m_bkgdProcIdMap.empty())
  {
    LogWrn("Waiting for background processes to close...", "MainAppFrame", "OnClose");
    Sleep(500);
  }
#else
  if (!m_bkgdProcIdMap.empty())
  {
    LogWrn("Waiting for background process objects to close...", "MainAppFrame", "OnClose");

    // The code below can only be used with the Windows2000+ SDK
    const int4 iThreadCt(m_bkgdProcIdMap.size());
    HANDLE * pHandles = new HANDLE[iThreadCt];
    try
    {
      // Initialize list of thread handles
      BackgroundProcIdMap::iterator idMapIter = m_bkgdProcIdMap.begin();
      BackgroundProcIdMap::iterator idMapEnd  = m_bkgdProcIdMap.end();
      for (int i(0); (i < iThreadCt) && (idMapIter != idMapEnd); ++idMapIter)
      {
        OpenThread(SYNCHRONIZE, FALSE, idMapIter->first);
        pHandles[i] = idMapIter->first;
      }

      // Wait for all of the threads to finish
      WaitForMultipleObjects(iThreadCt, pHandles, TRUE, INFINITE);

      // Jump to cleanup below
      throw true;
    }
    catch (...)
    {
      // Catch all, on failure or success, just cleanup and continue on
      if (pHandles != NULL)
      {
        delete []pHandles;
      }
    }
  }
#endif

  if (!m_bkgdProcIdMap.empty())
  {
    LogErr(ToAscii(m_bkgdProcIdMap.size()) + " background process(es) still running; aborting elegant "
      "close and ending application.", "MainAppFrame", "OnClose");
  }

  // Close this window
  try
  {
    CMDIFrameWnd::OnClose();
  }
  catch (...)
  {
    LogErr("An exception occurred in CMDIFrameWnd::OnClose()","MainAppFrame", "OnClose");
#ifndef FINAL_RELEASE
    // Note to Programmers: Please DO NOT report this to the user
    AfxMessageBox("DebugMsg: Exception in CMDIFrameWnd::OnClose()");
#endif
  }

  try
  {
    // Close the current study (if any) and perform memory cleanup
    OnMenuStudyClose();
  }
  catch (...)
  {
    LogErr("An exception occurred while closing the current study.","MainAppFrame", "OnClose");
#ifndef FINAL_RELEASE
    // Note to Programmers: Please DO NOT report this to the user
    AfxMessageBox("DebugMsg: Exception in OnMenuStudyClose()");
#endif
  }

} // OnClose()


/////////////////////////////////////////////////////////////////////////////
// MainAppFrame diagnostics

#ifdef _DEBUG

/**
 * Debug assertions.
 */
void MainAppFrame::AssertValid() const
{
  CMDIFrameWnd::AssertValid();
} // AssertValid()


/**
 * Crash dump.
 *
 * @param dc   Dump context.
 */
void MainAppFrame::Dump(CDumpContext& dc) const
{
  CMDIFrameWnd::Dump(dc);
} // Dump()

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MainAppFrame windows message handlers

/**
 * WM_MOVE handler
 *
 * @param iCX      New width.
 * @param iCY      New height.
 */
void MainAppFrame::OnMove(int iCX, int iCY) 
{
  CMDIFrameWnd::OnMove(iCX, iCY);
} // OnMove()


/**
 * Pre-resize handler
 *
 * @param uSide    Which side of the window is being 
 * @param iCX      New width.
 * @param iCY      New height.
 */
void MainAppFrame::OnSizing(UINT uSide, LPRECT lpRect)
{
  CRect r(*lpRect);

  const CSize szMinResStandard(1280,1024);
  const CSize szMinResWidescreen(1680,1050);
  const CRect rctDisplay(MonitorDisplay::GetCurrentDisplayRect());
  const CSize szScreen(rctDisplay.Width(), rctDisplay.Height());

  // display resolution
  float4 fRatio(1.0F); CSize szMinimumRes(1,1), szMaximumRes(1,1);
  switch ( MonitorDisplay::GetWindowLayoutDetect() )
  {
    case MonitorDisplay::WL_STANDARD:
      fRatio = float4(szMinResStandard.cx)/float4(szMinResStandard.cy);     szMinimumRes = szMinResStandard;      szMaximumRes = CSize(1600,1200);
      break;
    case MonitorDisplay::WL_WIDESCREEN:
      fRatio = float4(szMinResWidescreen.cx)/float4(szMinResWidescreen.cy); szMinimumRes = szMinResWidescreen;    szMaximumRes = CSize(1920,1200);
      break;
    case MonitorDisplay::WL_PORTRAIT:
      fRatio = float4(szMinResStandard.cy)/float4(szMinResStandard.cx); szMinimumRes = CSize(1200, 1600);
      szMaximumRes = CSize(1536,2048);
      break;
    case MonitorDisplay::WL_AUTODETECT:
    default:
      fRatio = float4(szScreen.cx) / float4(szScreen.cy);
      szMinimumRes = WinUtils::MonitorDisplay::IsPortrait() ? CSize(1200,1600) : (WinUtils::MonitorDisplay::IsWideScreen() ? szMinResWidescreen : szMinResStandard);
      szMaximumRes = WinUtils::MonitorDisplay::IsPortrait() ? CSize(min(szScreen.cx, 1536), min(szScreen.cy,2048)): CSize(min(szScreen.cx,2048), min(szScreen.cy, 1536));
      break;
  }

  switch ( uSide )
  {
    case WMSZ_TOPRIGHT: // seem to work ok
    case WMSZ_TOP: // seem to work ok
      lpRect->top = r.top = Bound(r.bottom-szMaximumRes.cy, r.top, r.bottom-szMinimumRes.cy);
      lpRect->right = r.left + (float4(r.Height()) * fRatio);
      break;

    case WMSZ_BOTTOMLEFT: // seems to work ok
      lpRect->bottom = r.bottom = Bound(r.top+szMinimumRes.cy, r.bottom, r.top+szMaximumRes.cy);
      lpRect->left = r.right - (float4(r.Height()) * fRatio);
      break;

    case WMSZ_TOPLEFT: // seems to work ok
    case WMSZ_LEFT: // seems to work ok
      lpRect->left = r.left = Bound(r.right-szMaximumRes.cx, r.left, r.right-szMinimumRes.cx);
      lpRect->top = r.bottom - (float4(r.Width()) * (1.0F/fRatio));
      break;

    case WMSZ_RIGHT: // works nicely
      lpRect->right = r.right = Bound(r.left+szMinimumRes.cx, r.right, r.left+szMaximumRes.cx);
      lpRect->bottom = r.top + (float4(r.Width()) * (1.0F/fRatio));
      break;

    case WMSZ_BOTTOMRIGHT: // works nicely
    case WMSZ_BOTTOM: // works nicely
      lpRect->bottom = r.bottom = Bound(r.top+szMinimumRes.cy, r.bottom, r.top+szMaximumRes.cy);
      lpRect->right = r.left + (float4(r.Height()) * fRatio);
      break;

    default:
      return;
  }

  CMDIFrameWnd::OnSizing(uSide, lpRect);
} // OnSizing()


/**
 * Resize handler
 *
 * @param uType   Type of resize event.
 * @param iCX      New width.
 * @param iCY      New height.
 */
void MainAppFrame::OnSize(UINT uType, int iCX, int iCY) 
{
  CMDIFrameWnd::OnSize( uType, iCX, iCY );

  CSize szPrevAppSize(ViatronixWnd::m_szAppWindow);

  // step (b) may need to be inserted here

  // set new application window size
  ViatronixWnd::m_szAppWindow = CSize(iCX, iCY);

  // TODO: a) Check if the new window size has crossed the small threshold; if so, trigger a layout-reload
  if ( MonitorDisplay::IsPortrait() )
  {
    if ( 1 ) // TODO: Check a special min-size setting for portrait??
      ReloadCurrentLayout();
  }
  else
  {
    if ( (szPrevAppSize.cx < ViatronixWnd::m_szSmallThreshold.cx && iCX >= ViatronixWnd::m_szSmallThreshold.cx)
    || (szPrevAppSize.cx >= ViatronixWnd::m_szSmallThreshold.cx && iCX < ViatronixWnd::m_szSmallThreshold.cx) )
      ReloadCurrentLayout();
  }

  // TODO: b) Check to ensure the window size matches the current display resolution; if not, SetWindowPos() to match the resolution
  if ( iCY != 0)
  {
    float4 fRatio(float4(iCX)/float4(iCY));
    CRect rctDisp(MonitorDisplay::GetCurrentDisplayRect());

    float4 fMonitorRatio(float4(rctDisp.Width())/float4(rctDisp.Height()));
  }


  m_rightBorder.MoveWindow( iCX - Theme::BkgndSideRight->GetWidth(), 0, Theme::BkgndSideRight->GetWidth(), iCY - Theme::BkgndSideBottom->GetHeight() );

  m_viewSelBar.MoveWindow( 0, 
                           iCY - Theme::BkgndSideBottom->GetHeight(), 
                           iCX, 
                           Theme::BkgndSideBottom->GetHeight() );

  m_navigationBar.MoveWindow( 0, 
                              0,
                              Theme::BkgndSideLeft->GetWidth(),
                              iCY );

  m_viewSelBar.BringWindowToTop();

#if defined(FINAL_RELEASE)

  int4 TODO_WORK_OUT_THIS_NONSENSE;
  //if (
  //  (VxUserAdmin::GetCurrentUserType() != UserType_Service) &&
  //  (RegistryKey(RegistryKey::eUSER).GetIntValue(VxUserAdmin::GetCurrentUserSettingsRegistrySection(),
  //    TEXT("AllowMinimize"), 0) == 0)
  //  )
  //{
  //  ShowWindow( SW_SHOWMAXIMIZED );
  //}
#endif
  Invalidate();
} // OnSize()


/**  
 * response to show window command
 *
 * @param uCmdShow   identifies the show event
 * @return           result of show window operation
 */
BOOL MainAppFrame::ShowWindow(int uCmdShow)
{
  return CWnd::ShowWindow(uCmdShow);
} // ShowWindow


/**
 * Sets the mouse cursor.
 *
 * @param pWnd       Pointer to window containing cursor(?).
 * @param uHitTest   Code identifying where cursor is located.
 * @param uMessage   Mouse message generating the set cursor message.
 * @return           True if cursor was set, False to let Windows set the default.
 */
BOOL MainAppFrame::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage)
{
  // TODO: Speed this up with one flag (inside m_hideParamsRefCtMap) instead of the three checks
  if ( (m_hideParamsRefCtMap[eBKHIDE_VIEWWIN] > 0) || (m_hideParamsRefCtMap[eBKHIDE_MENU] > 0)
   || (m_hideParamsRefCtMap[eBKHIDE_TABS] > 0) )
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    return TRUE;
  }
  else
  {
    return CMDIFrameWnd::OnSetCursor(pWnd, uHitTest, uMessage);
  }
} // OnSetCursor()


/**
 * Program preferences menu item handler.
 */
void MainAppFrame::OnProgramPreferences()
{
  PreferencesDlg dlg(IDS_PREFS_DLGNAME);
  dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;
  dlg.m_psh.dwFlags &= ~(PSH_HASHELP);
  
  dlg.DoModal();
} // OnProgramPreferences()


/**
 * Program preferences menu item update.
 *
 * @param pCmdUI   Pointer to the update command object.
 */
void MainAppFrame::OnUpdateProgramPreferences(CCmdUI* pCmdUI) 
{
  //const User::UserTypeEnum eCurrUserType( User::GetCurrentUser().GetType()  );
  //pCmdUI->Enable((eCurrUserType == User::UserType_Administrator) || (eCurrUserType == User::UserType_Service));
} // OnUpdateProgramPreferences()


/////////////////////////////////////////////////////////////////////////////
// MainAppFrame application message handlers


/**
 * Message: WM_APP_SET_MAIN_DATASET. Switches main dataset based on orientation
 *
 * @param wNewDatasetOrientation   new dataset to change to
 */
LRESULT MainAppFrame::OnSetMainDataset(WPARAM wNewDatasetOrientation, LPARAM)
{
  rdrGlobal.SetDatasetOrientation(DatasetOrientations::TypeEnum(wNewDatasetOrientation));
  GLWnd & currLayout = GetCurrentLayout();
  currLayout.SendMessage(WM_APP_SET_MAIN_DATASET, wNewDatasetOrientation, 0);
  for( int4 i( 0 ); i < currLayout.NumGLViews(); ++i )
    currLayout.GetChildViewByPos( i ).OnDatasetChanged();
  SendMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);

  ColonViews::Dialogs::PatientInfoDlg * pDlg = dynamic_cast<ColonViews::Dialogs::PatientInfoDlg*> (currLayout.GetDlgItem(IDC_TEMPLATE_PATIENTINFO));
  if( pDlg != NULL )
    pDlg->SendMessage(ColonViews::Dialogs::PatientInfoDlg::WM_USER_INITIALIZE_STUDY, 0, 0);

  return TRUE;
} // OnSetMainDataset


/**
 * Message: WM_APP_VIEWMODE_CHANGED.  Called when one or mode view modes change.
 *
 * @param uOldMode   Previous set of view modes.
 * @param uNewMode   New set of view modes after update.
 */
LRESULT MainAppFrame::OnAppViewModeChanged(WPARAM wpOldMode, LPARAM lpNewMode)
{
  bool bShareLightPosition = false;

  switch (VtxViewModes(wpOldMode).GetMutexModes())
  {
    case ViewModes::LIGHTING:
      bShareLightPosition = true;
      break;

    case ViewModes::AUTOFLY:
      PostMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);    
      rdrGlobal.m_pCurrDataset->m_submarine.SwitchToManualMode();
      rdrGlobal.m_pSecondaryDataset->m_submarine.SwitchToManualMode();
      break;

    case ViewModes::BIOPSY:
      bShareLightPosition = true;
      rdrGlobal.m_bBiopsyMode = false;
      rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      break;

    case ViewModes::LINEMEASURE:
      bShareLightPosition = true;
      break;

    default:
      break;
  }

  bool bResetLightPosition = false;
  switch (VtxViewModes(lpNewMode).GetMutexModes())
  {
    case ViewModes::AUTOFLY:
      if ( bShareLightPosition ) bResetLightPosition = true;
      rdrGlobal.m_pCurrDataset->m_submarine.SwitchToAutomaticMode();
      rdrGlobal.m_pSecondaryDataset->m_submarine.ExitReplayMode();
      break;

    case ViewModes::BIOPSY:
      rdrGlobal.m_bBiopsyMode = true;
      rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      break;

    case 0:
      if ( bShareLightPosition ) bResetLightPosition = true;
      break;
  }

  // reset the light position if going from any of {lighting, measurement, biopsy} to either {autofly, nothing}
  if ( bResetLightPosition )
  {
    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.m_fMoveLightVertDirection = 0.0;
    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.m_fMoveLightHorizDirection = 0.0;
    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();

    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.m_fMoveLightVertDirection = 0.0;
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.m_fMoveLightHorizDirection = 0.0;
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  }

  BroadcastMessage(GLChildView::eGVT_NONE, WM_APP_VIEWMODE_CHANGED, wpOldMode, lpNewMode);

  return 1;
} // OnAppViewModeChanged()


/**
 * Sends a message to multiple child views.
 *
 * @param iWndTypes   Identifies childviews.
 * @param uMsg        Message.
 * @param wParam      Message parameter.
 * @param lParam      Message parameter.
 * @param bWait       Whether the message will be Sent (true) or Posted (false).
 */
void MainAppFrame::BroadcastMessage(const int4 iWndTypes, const UINT uMsg,
                                    const WPARAM wParam, const LPARAM lParam, const bool bWait)
{
#ifdef _DEBUG
  ChildLayoutFrame* pFrame = (ChildLayoutFrame *) GetActiveFrame();
#else
  ChildLayoutFrame* pFrame = STATIC_DOWNCAST(ChildLayoutFrame, GetActiveFrame());
#endif
  if (!pFrame || (pFrame == ((ChildLayoutFrame*) this)))
  {
    return;
  }

  GLWnd & currLayout = GetCurrentLayout();

  currLayout.BroadcastMessage(iWndTypes, uMsg, wParam, lParam, bWait);
} // BroadcastMessage()


/**
 * Send a message to a particular window or dialog
 *
 * @param iWinItemID  Child window dialog ID.
 * @param uMsg        Message.
 * @param wParam      Message parameter.
 * @param lParam      Message parameter.
 */
void MainAppFrame::DirectMessage(const int4 iWinItemID, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  GLWnd & currLayout = GetCurrentLayout();
  CWnd * pWnd = (iWinItemID == currLayout.GetDlgCtrlID()) ? &currLayout : currLayout.GetDlgItem(iWinItemID);
  if (pWnd) pWnd->PostMessage(uMsg, wParam, lParam);
} // DirectMessage()


/**
 * In essence, validates the new view selection.
 *
 * @param iNewView   New main view.
 * @param iSubView   New sub view.
 * @return           TRUE to allow view selection change, FALSE to disallow.
 */
LRESULT MainAppFrame::OnViewSelectionChanging(WPARAM iNewView, LPARAM iSubView)
{
  return TRUE;
} // OnViewSelectionChanging()


/**
 * Handles changing between views.
 *
 * @param iView      The new view.
 * @param iSubView   Sub view.
 * @return           TRUE.
 */
LRESULT MainAppFrame::OnViewSelectionChanged(WPARAM iView, LPARAM iSubView)
{
#ifdef FINAL_RELEASE
  if (rdrGlobal.IsLoadingComplete())
#endif
  {

    // Set the Console tabs enable flag based on the presence of the -console flag on the command line
    std::string sConsoleTabEnabled(ViatronApp::GetViatronApp()->GetCommandLine().GetLaunchedFromConsole() ? "1" : "0" );

    // Default tab enable vector
    std::string sEnableVector(std::string("1111") + sConsoleTabEnabled);

    //=====================================================================================
    // If the user is allowed to open a report then make sure the report tab is enabled.
    //=====================================================================================
#ifdef FINAL_RELEASE
    if ( (iView == 1) || ((iView == -1) && (m_iCurrMainView == 1)) ) // special case for verification tab
      sEnableVector = std::string("0001") + sConsoleTabEnabled;
    else
#endif
      if ( ReportLayout::CanOpenReport() )
				sEnableVector = std::string( ReportLayout::HasPermissions() ? "1111" : "0111") + sConsoleTabEnabled;
      else
        sEnableVector = std::string("0011") + sConsoleTabEnabled;

    // Enable/Disable the tabs based on the vector 
    m_viewSelBar.EnableItems(ViewSelectDlgBar::EnableViewsBitSet( sEnableVector ));

  } // END ... If the loading is complete

  m_previousView = std::make_pair( m_iCurrMainView, m_iCurrSubView );

  ShowView(iView, iSubView, true);
  
  return 1;
} // OnViewSelectionChanged()


/**
 * Sets the progress of an ongoing process.
 *
 * @param progress   Percentage progress (0-100).
 * @param LPARAM     Unused.
 * @return           TRUE.
 */
LRESULT MainAppFrame::OnSetFunctionProgress(WPARAM wpProgress, LPARAM)
{
  m_viewSelBar.SetProgress(wpProgress);
  return 1;
} // OnSetFunctionProgress()


/**
 * Displays help text in status bar.
 *
 * @param wpStringID             Resource ID of the text.
 * @param lpIsRemoveAfterDelay   Non-zero to clear the message after a short delay, Zero otherwise.
 * @return                       TRUE.
 */
LRESULT MainAppFrame::OnDisplayHelpText(WPARAM wpStringID, LPARAM lpIsRemoveAfterDelay)
{
  CString str;
  if (!str.LoadString(wpStringID))
  {
    str.LoadString(AFX_IDS_IDLEMESSAGE);
  }

  if (IsWindow(m_statusBar))
    m_statusBar.SetPaneText(HELP_PANE, str);

  if (lpIsRemoveAfterDelay != 0)
  {
    m_timerResetHelpText.SetTimer(this, 10000);
  }

  return 1;
} // OnDisplayHelpText()


/**
 * Displays text indicating a lengthy or current function
 *
 * @param wpFunctionStringID   Resource ID for string indicating current function.
 * @param lpStepStringID       Resource ID for string indicating current step in function.
 * @return                     TRUE.
 */
LRESULT MainAppFrame::OnDisplayFunctionAndStepText(WPARAM wpFunctionStringID, LPARAM lpStepStringID)
{
  m_statusBar.SetPaneText(FUNCTION_PANE, LoadResourceString(wpFunctionStringID));
  m_statusBar.SetPaneText(STEP_PANE, LoadResourceString(lpStepStringID));
  return 1;
} // OnDisplayFunctionAndStepText()


/**
 * Displays text indicating a lengthy or current function
 *
 * @param sFunction   String indicating current function.
 * @param sStep       String indicating current step in function.
 * @return            TRUE.
 */
LRESULT MainAppFrame::DisplayFunctionAndStepText(const std::string& sFunction, const std::string& sStep)
{
  m_statusBar.SetPaneText(FUNCTION_PANE, sFunction.c_str());
  m_statusBar.SetPaneText(STEP_PANE, sStep.c_str());
  return 1;
} // DisplayFunctionAndStepText()


/**
 * Unused.  Was to mimic functionality of Windows ALT-TAB.
 */
void MainAppFrame::OnShowNextView()
{
  // ShowView((m_viewSelBar.GetCurrentViewSelection() + 1) % MainAppFrame::NUm_vpViews, true);
} // OnShowNextView()


/////////////////////////////////////////////////////////////////////////////
// MainAppFrame accelerator messages

/**
 * User requests to abort loading a study.
 */
void MainAppFrame::OnCancelLoad()
{
  if (rdrGlobal.IsStudyLoading())
  {   
    if( m_loader )
      m_loader->Abort();

    OnDisplayFunctionAndStepText(IDS_STUDYLOAD_ABORTING, 0);

    /// Release the license
    ViatronApp::GetViatronApp()->ReleaseLicense();


    // Hide the current viewer instance and allow another instance to be instantiated asap
    ViatronApp::CloseAppMutexHandle();
    ShowWindow(SW_HIDE);

    TerminateProcess( GetCurrentProcess(), 0 );
  }
} // OnCancelLoad()


/**
 * Sets patient and current user info in the titlebar.
 */
void MainAppFrame::SetPatientInfoInTitle()
{
  CString str;

  str.LoadString(IDS_V3DVIEWER_COLON);
  str += CString("       ") +
    GetVolumeHeaderVariable("patientID").c_str() + CString("   ") +
    // PersonName(GetVolumeHeaderVariable("patientName")).GetLastFirstMidSuffix().c_str();
    GetVolumeHeaderVariable("patientName").c_str();

  str += CString("       ");

#ifndef FINAL_RELEASE
  str += LoadResourceString(IDS_VVS_TEST_VERSION);
#else
	std::string ss = User::GetCurrentUser().GetFullName();
	str += CString( ss.c_str() );
#endif

  SetWindowText(str);
} // SetPatientInfoInTitle()


/**
 * Notifies the application of progress during study load.
 *
 * @param wpPrevNnextStep   Previous (LOWORD) and next (HIWORD) step.
 * @param lpOrientation     Orientation on which the step is being performed.
 * @return                  TRUE.
 */
LRESULT MainAppFrame::OnStudyLoadStepNotify(WPARAM wpPrevNnextStep, LPARAM lpOrientation)
{ 
  const StudyLoad::StepsEnum ePrevStep(StudyLoad::StepsEnum(LOWORD(wpPrevNnextStep)));
  const StudyLoad::StepsEnum eNextStep(StudyLoad::StepsEnum(HIWORD(wpPrevNnextStep)));

  if (lpOrientation == 1)
  {
    m_statusBar.SetPaneText(FUNCTION_PANE, LoadResourceString(IDS_STUDYLOAD_SUPINE));
  }
  else if (lpOrientation == 2)
  {
    m_statusBar.SetPaneText(FUNCTION_PANE, LoadResourceString(IDS_STUDYLOAD_PRONE));
  }

  switch (ePrevStep)
  {
    case StudyLoad::eSTART:
    {
      // Enable (only) and show verification screen
      ViatronApp* pApp = dynamic_cast<ViatronApp*>(AfxGetApp());

      //======================================================================================================================
      // Enable the tab for the study browser view if the launched from console flag os set (-console was on command line)
      // The last digit in he string identifies the study browser (console)  tab.
      //======================================================================================================================
      std::stringstream enableBits;
      enableBits << "0001" << ( pApp->GetCommandLine().GetLaunchedFromConsole() ) ? "1" : "0";
      m_viewSelBar.EnableItems(ViewSelectDlgBar::EnableViewsBitSet( enableBits.str() ));

      ShowView(1, 0, false);

      pApp->HideSplashScreen();

      break;

    } // END ... StudyLoad::eSTART

    case StudyLoad::eHEADER:
    {
      SetPatientInfoInTitle();
			m_viewSelBar.SetLinkStateDisplay( m_bShowLinkState );
      break;
    }

    case StudyLoad::eAXIAL:
    case StudyLoad::eSAGITTAL:
    case StudyLoad::eCORONAL:
      break;

    case StudyLoad::eSEGMENTS:
      break;
  }

  GetCurrentLayout().SendMessage(WM_APP_STUDY_LOAD_STEP_NOTIFY, wpPrevNnextStep, lpOrientation);

  CString sStepString;
  switch (eNextStep)
  {
    case StudyLoad::eSTART:
      break;

    case StudyLoad::eLARGESIZE_QUERY:
      if (AfxMessageBox(IDS_PROMPT_LOAD_LARGE_STUDY, MB_ICONQUESTION | MB_YESNO) != IDYES)
      {
        if( m_loader )
          m_loader->Abort();
      }
      return 1;

    case StudyLoad::eHEADER:
      sStepString.LoadString(IDS_STUDYLOAD_HEADER);
      break;

    case StudyLoad::eSEGMENTS:
      sStepString.LoadString(IDS_STUDYLOAD_SEGMENTS);
      break;

    case StudyLoad::eVOLFIELDS:
      sStepString.LoadString(IDS_STUDYLOAD_VOLFIELDS);
      break;

    case StudyLoad::eAXIAL:
      sStepString.LoadString(IDS_STUDYLOAD_AXIAL);
      break;

    case StudyLoad::eSAGITTAL:
      sStepString.LoadString(IDS_STUDYLOAD_SAGITTAL);
      break;

    case StudyLoad::eCORONAL:
      sStepString.LoadString(IDS_STUDYLOAD_CORONAL);
      break;

    case StudyLoad::eTHRESHOLD:
      sStepString.LoadString(IDS_STUDYLOAD_THRESHOLD);
      break;

    case StudyLoad::eVOLUME:
      sStepString.LoadString(IDS_STUDYLOAD_VOLUME);
      break;

    case StudyLoad::eDFS:
      sStepString.LoadString(IDS_STUDYLOAD_DFS);
      break;
    case StudyLoad::eDFE:
      sStepString.LoadString(IDS_STUDYLOAD_DFE);
      break;
    case StudyLoad::eCSI:
      sStepString.LoadString(IDS_STUDYLOAD_CSI);
      break;
    case StudyLoad::eSLD:
      sStepString.LoadString(IDS_STUDYLOAD_SLD);
      break;
    case StudyLoad::eINS:
      sStepString.LoadString(IDS_STUDYLOAD_INS);
      break;

    case StudyLoad::eSUBMARINE:
      sStepString.LoadString(IDS_STUDYLOAD_SUBMARINE);
      break;

    case StudyLoad::eBOOKMARKS:
      sStepString.LoadString(IDS_STUDYLOAD_BOOKMARKS);
      break;

    case StudyLoad::eINITIALIZE:
      m_statusBar.SetPaneText(FUNCTION_PANE, LoadResourceString(IDS_STUDYLOAD_PAINTING) + "...");
      m_statusBar.SetPaneText(STEP_PANE, "");
      return 1;

    case StudyLoad::eINITIALIZE_COMPLETE:
    case StudyLoad::eLOAD_COMPLETE:
      GetCurrentLayout().SendMessage(WM_APP_STUDY_LOAD_STEP_NOTIFY, MAKEWORD(eNextStep,0), lpOrientation);
      m_statusBar.SetPaneText(FUNCTION_PANE, CString());
      m_statusBar.SetPaneText(STEP_PANE, CString());
      return 1;

    default:
      m_statusBar.SetPaneText(STEP_PANE, CString());
      return 1;
  }
	
  m_viewSelBar.SetProgress(50);
  m_statusBar.SetPaneText(STEP_PANE, sStepString + "...");
  return 1;
} // OnStudyLoadStepNotify()


/**
 * Enables/disables selected menu bar items. The Program and Help items are always enable.
 *
 * @param bEnable   True to enable, false to disable.
*/
void MainAppFrame::EnableMenuBar( const bool bEnable )
{
  CMenu * pMenuBar = GetMenu();
  if (!pMenuBar)
  {
    return;
  } // if

	UINT uEnableProgramAndHelp( MF_BYPOSITION | MF_ENABLED ); 
  UINT uEnable( MF_BYPOSITION | (bEnable ? MF_ENABLED : ( MF_DISABLED | MF_GRAYED ) ) );
 
	//==========================================================
	// Assume that pMenuBar->GetMenuItemCount() == 5. 
	//Otherwise, the following code needs to change accordingly.
	//==========================================================
	pMenuBar->EnableMenuItem(0, uEnableProgramAndHelp ); 
  pMenuBar->EnableMenuItem(1, uEnable );
  pMenuBar->EnableMenuItem(2, uEnable );
  pMenuBar->EnableMenuItem(3, uEnable );
	pMenuBar->EnableMenuItem(4, uEnableProgramAndHelp ); 
 
  DrawMenuBar();

	/// extract the user preference on toolbar position for view layout
	ApplicationSettings::UserSettings().TryGet< bool >( APP_SETTING_TOOLBAR_POSITION, rdrGlobal.m_bToolbarOnBottomForWideScreen );

} // EnableMenuBar()


/**
 * Disable entire menu bar
*/
void MainAppFrame::DisableMenuBar()
{
  CMenu * pMenuBar = GetMenu();
  if (!pMenuBar)
  {
    return;
  } // if

  UINT uEnable( MF_BYPOSITION | (MF_DISABLED | MF_GRAYED) );
 
	//==========================================================
	// Assume that pMenuBar->GetMenuItemCount() == 5. 
	//Otherwise, the following code needs to change accordingly.
	//==========================================================
	pMenuBar->EnableMenuItem(0, uEnable ); 
  pMenuBar->EnableMenuItem(1, uEnable );
  pMenuBar->EnableMenuItem(2, uEnable );
  pMenuBar->EnableMenuItem(3, uEnable );
	pMenuBar->EnableMenuItem(4, uEnable ); 
 
  DrawMenuBar();
} // DisnableMenuBar()



/**
 *  Translate window messages before they are dispatched 
 *
 * @param     pMsg    message to process
 * @return    Nonzero if the message was translated and should not be dispatched;0 otherwise.
 */
BOOL MainAppFrame::PreTranslateMessage( MSG * pMsg )
{
  ViatronApp::GetViatronApp()->GetIdleManager().ProcessMessage( pMsg );
  return CWnd::PreTranslateMessage( pMsg );
} // PreTranslateMessage()


/**
 * WM_COMMAND handler.
 *
 * @param wParam   Message-dependent parameter.
 * @param lParam   Message-dependent parameter.
 * @return         TRUE if message is handled, FALSE otherwise (or return from base class handler).
 */
BOOL MainAppFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  if (HIWORD(wParam) == 1)
  {
    // Keyboard accelerator message
    GLWnd & currLayout = GetCurrentLayout();
    currLayout.PostMessage(WM_SETCURSOR, WPARAM(currLayout.GetSafeHwnd()),
      MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
  }
  return CMDIFrameWnd::OnCommand(wParam, lParam);
} // OnCommand()


/**
 * Message loop.
 *
 * @param uMessage   Message.
 * @param wParam     Message-dependent parameter.
 * @param lParam     Message-dependent parameter.
 * @return           Always 1.
 */
LRESULT MainAppFrame::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam) 
{
  // TODO: Add your specialized code here and/or call the base class
  LRESULT iWinProcResult;

#ifdef FINAL_RELEASE
  try
#endif
  {
    // let the menu bar have first crack
    //if( m_menubar.CanProcessMessage( uMessage, wParam, lParam  ))
    //  return m_menubar.ProcessMessage( uMessage, wParam, lParam );

    switch (uMessage)
    {
      case WM_SIZING:
        OnSizing((UINT)wParam, (LPRECT)lParam);
        break;
      case WM_DEVICECHANGE:
        SendMessageToDescendants(uMessage, wParam, lParam, TRUE, FALSE);
        break;

      case WM_DISPLAYCHANGE: 
				/** This handle affects situation like screen saver, monitor change, monitor setting change, and graphics card change
				 *  Application behave may vary based on CPU chips, graphics card driver, the way OpenGL and GDI/GDI+ implemented.
				 *  At this point, all tests shows that the following codes are not necessary need. Future review might occures 
				 *  if new bug is found that is related to the message handle. Dongqing 09/26/2012.
				 */
        //MonitorDisplay::EnumAttachedDisplays();
        //ReloadCurrentLayout();
        //ShowWindow( SW_SHOWMAXIMIZED );
        break;

      case WM_INITMENU:
      case WM_INITMENUPOPUP:
        break;

      case WM_ENTERMENULOOP:
      {
        rdrGlobal.m_pCurrDataset->m_submarine.StopFlying();
        PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
        if (ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY)
        {
          ColonViatronixWnd::ToggleAutoflyMode();
        }
        else if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
        {
          ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
          PostMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
        }
        break;
      }

      default:
      {
        // Registered Windows messages
        if ((uMessage >= 0xC000) && (uMessage <= 0xFFFF))
        {
          SendMessageToDescendants(*this, uMessage, wParam, lParam, TRUE, TRUE);
        }
        else if ((uMessage >= WM_APP_FIRST_MESSAGE) && (uMessage <= WM_APP_MESSAGE_HIGH))
        {
          SendMessageToDescendants(*this, uMessage, wParam, lParam, TRUE, TRUE);
        }
        break;
      }
    }

    iWinProcResult = CMDIFrameWnd::WindowProc(uMessage, wParam, lParam);
  }
#ifdef FINAL_RELEASE
  catch(...)
  {
    CString str;
    str.Format("MainAppFrame::WindowProc error: message (%d), wParam (%d), lParam(%d)", uMessage, wParam, lParam);
    // Error::WarnLog(LPCTSTR(str));
    AfxMessageBox(IDS_ERROR_UNSPECIFIED);
    ex::RuntimeException(LogRec(str.GetBuffer(str.GetLength()), "MainAppFrame", "WindowProc"));
  }
#endif

  return iWinProcResult;
} // WindowProc()


/**
 * WM_TIMER event handler.
 *
 * @param uIDEvent   ID of the timer event.
 */
void MainAppFrame::OnTimer(UINT_PTR uIDEvent) 
{
  // TODO: Add your message handler code here and/or call default

  if (uIDEvent == m_timerUpdateTime)
  {
    // update time display
    const int4 iBufferSz(100); char timeStr[iBufferSz+1];
    GetTimeFormat(LANG_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, timeStr, iBufferSz);
    if (IsWindow(m_statusBar))
      m_statusBar.SetPaneText(DATETIME_PANE, timeStr);

    // Align timer to minute boundary
    const COleDateTime currTime(COleDateTime::GetCurrentTime());
    m_timerUpdateTime.SetTimer(this, (60 - currTime.GetSecond()) * 1000);    // Set up timer event for time updates every minute
  }
  else if (uIDEvent == m_timerResetHelpText)
  {
    // Reset the help text display
    OnDisplayHelpText(0, 0);
  }
  else if ( (uIDEvent == m_timerUpdateLocationLock) || (uIDEvent == -1) )
  {
    // Reserved a timer ID of UINT(-1) to update lock and timer from another thread
    // (using PostMessage(..., WM_TIMER, uint4(-1), 0).  A bit kludgy, I know, but....


    // Halt time until done
    m_timerUpdateLocationLock.KillTimer();

    LogFyi("Renewing software license", "MainAppFrame", "OnTimer");
    ViatronApp * pApp = dynamic_cast<ViatronApp*>(AfxGetApp());
    if( pApp->HasLicense() )
    {
      CString closeViewer;
      closeViewer.LoadString(IDS_CLOSE_VIEWER);

      OnDisplayFunctionAndStepText(IDS_REQUESTING_LICENSE, 0);
      try
      {
        if( !pApp->RenewLicense() )
        {
          // keep trying in quiet for a bit before giving up and bothering the user
          if (ColonLicense::m_iTimerRetries > ColonLicense::MaxNumberRetries)
            throw ex::RuntimeException( LogRec( "Unable to renew", "MainAppFrame", "OnTimer" ));

          m_timerUpdateLocationLock.SetTimer(this, ColonLicense::RenewalRetryInterval);
          ColonLicense::m_iTimerRetries++;          
        }
      }
      // ===============================================================================
      // Catch the Account Expiration Exception from the security resource provider
      // ===============================================================================
      catch ( ex::AccountExpiredException ex)
      {
        std::stringstream msg;
        msg <<  ex.GetLogRecord().GetTextMessage() << ", " << (LPCTSTR)closeViewer;

        this->ShowWindow(SW_HIDE);

        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);

        rdrGlobal.m_supine.m_submarine.FullStop();
        rdrGlobal.m_prone.m_submarine.FullStop();
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
        CWaitCursor crsr; 

        LogErr( "Closing application. Account has expired.", "MainAppFrame", "OnTimer" );
        OnClose();
        return;
 
      }
      // ===============================================================================
      // Catch the License Expiration Exception from the security resource provider
      // ===============================================================================
      catch ( ex::LicenseExpiredException ex)
      {
        std::stringstream msg;
        msg <<  ex.GetLogRecord().GetTextMessage() << ", " << (LPCTSTR)closeViewer;

        this->ShowWindow(SW_HIDE);

        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);

        rdrGlobal.m_supine.m_submarine.FullStop();
        rdrGlobal.m_prone.m_submarine.FullStop();
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
        CWaitCursor crsr; 

        LogErr( "Closing application. License has expired.", "MainAppFrame", "OnTimer" );
        OnClose();
        return;
 
      }
      // ===============================================================================
      // Catch the all other Exceptions from the security resource provider
      // ===============================================================================
      catch( ... )
      {
        CString renewalFailed;
        renewalFailed.LoadString(IDS_RENEWAL_FAILED);

        std::stringstream msg;
        msg <<  (LPCTSTR)renewalFailed << ", " << (LPCTSTR)closeViewer;

        this->ShowWindow(SW_HIDE);

        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);

        // turn off auto flight
        rdrGlobal.m_supine.m_submarine.FullStop();
        rdrGlobal.m_prone.m_submarine.FullStop();
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
        CWaitCursor crsr; 

        LogErr( "Closing application. License has not been renewed.", "MainAppFrame", "OnTimer" );
        OnClose();
        return;
      }

      // successful license 
      OnDisplayFunctionAndStepText(0, 0);
    }
    m_timerUpdateLocationLock.SetTimer( this, ColonLicense::RenewalInterval );
  }

  CMDIFrameWnd::OnTimer(uIDEvent);
} // OnTimer()


/**
 * WM_TIMECHANGE handler.
 */
void MainAppFrame::OnTimeChange() 
{
  CMDIFrameWnd::OnTimeChange();

  OnTimer(m_timerUpdateTime); 
} // OnTimeChange()


/**
 * Called when user marks the study as complete.
 */
void MainAppFrame::OnStudyComplete()
{
	/// check user right for marking report complete
  if( User::GetCurrentUser().IsAllowed( "CompleteReport" ) )
	{
		PostMessage(WM_COMMAND, MAKEWPARAM(ID_REPORT_MARKCOMPLETE, 0), 0);
	}
	else
		AfxMessageBox( IDS_NON_PHYSICIAN_USER );   // only physician user can mark report Complete.
} // OnStudyComplete()


/**
  * register an observer 
  *
  * @param       pObserver       IObserver
  */
void MainAppFrame::Connect( IObserver< MainAppFrameStates > * pObserver )
{
  m_observableHelper.Connect( pObserver );
} // Connect( pObserver )


/**
  * unregister an observer
  *
  * @param       pObserver       IObserver
  */
void MainAppFrame::Disconnect( IObserver< MainAppFrameStates > * pObserver )
{
  m_observableHelper.Disconnect( pObserver );
} // Disconnect( pObserver )


/**
  * notify any observers
  *
  * @param           args          MainAppFrameStates
  */
void MainAppFrame::Notify( const MainAppFrameStates & args )
{
  m_observableHelper.Notify( args );
} // Notify( args )


#undef FILE_REVISION
