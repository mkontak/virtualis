// $Id: MainAppFrame.h,v 1.11.2.5 2010/02/01 16:36:16 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: interface of the MainAppFrame class 
// Owner: Jeffrey Meade  jmeade@viatronix.net

#pragma once

#include "ChildViewFrame.h"
#include "ViewSelectDlgBar.h" // Added by ClassView
#include "BackgroundProcess.h"
#include "ViatronViewWnd.h"
#include "NavigationBar.h"  // Added by ClassView
#include "VFrameManager.h"
#include "RightBorder.h"
#include "Menubar.h"
#include "V1KMessages.h"
#include "WmTimer.h"
#include "StudyLoadParamsEx.h"
#include "ObservableObject.h"
#include "MainAppFrameStates.h"

#include <memory>
#include "Loader.h"


/**
 * main frame window encapsulating the application
 */
class MainAppFrame : public CMDIFrameWnd
{
  /// dynamic class creation
  DECLARE_DYNAMIC(MainAppFrame)


// Methods
public:
  /// Constructor
  MainAppFrame();
  /// Destructor
  virtual ~MainAppFrame();


  bool Open( StudyLoadParamsEx & params );

  /// Send a message to child view window types
  void BroadcastMessage(const int4 iWndTypes, const UINT uMsg, const WPARAM wParam,
    const LPARAM lParam, const bool bWait = false);

  /// Send a message to a particular window or dialog
  void DirectMessage(const int4 iWinItemID, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

  /// Gets main window client rectangle
  virtual void GetClientRect(LPRECT pRect);

  /// Returns a type-casted pointer to the main frame
  static inline MainAppFrame* GetMainFrame() { return dynamic_cast<MainAppFrame*> (AfxGetMainWnd()); }
  /// Returns the current view window
  ColonViews::Layouts::GLWndRef GetCurrentLayout() const;

  /// Reload the current layout
  void ReloadCurrentLayout();

  /// Gets the LHS navigation dialog bar
  ColonViews::Dialogs::NavigationBar& GetNavigationBar() { return m_navigationBar; }
  /// Gets the view selection dialog bar
  ColonViews::Dialogs::ViewSelectDlgBar& GetViewSelectDlgBar() { return m_viewSelBar; }

  /// Background thread study load function
  static int4 StudyLoadNotify(void* pData, const ReaderLib::StudyLoad::StepsEnum ePrevStep,
                    const ReaderLib::StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
                    const ReaderLib::DatasetOrientations::TypeEnum eNewOrientation);

  /// Returns true if no hazards file exists or if user answers 'Yes' to hazards prompt
  static bool CheckHazards( ReaderLib::StudyLoadParams & params );

  /// Displays current function/step text
  LRESULT DisplayFunctionAndStepText(const std::string& sFunction, const std::string& sStep);
  /// Shows the window
  BOOL ShowWindow(int nCmdShow);

  /// gets the report prefix
  const std::string & GetReportPrefix() const { return m_sReportPrefix; }
   
  /// gets the previous view and subview indicies
  std::pair<uint4,uint4> GetPreviousView() const { return m_previousView; }

public:

  /// register an observer 
  void Connect( IObserver< MainAppFrameStates > * pObserver );

  /// unregister an observer
  void Disconnect( IObserver< MainAppFrameStates > * pObserver );

private:

  /// notify any observers
  void Notify( const MainAppFrameStates & args );

public:
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(MainAppFrame)
  public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  protected:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

  virtual BOOL PreTranslateMessage( MSG * pMsg );
  //}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
  //{{AFX_MSG(MainAppFrame)
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg void OnMenuStudyClose();
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnUsersMenu();
  afx_msg void OnLogoutUser();
  afx_msg void OnChangeUserPassword();
  afx_msg void OnUpdateUsrLogoutMenu(CCmdUI* pCmdUI);
  afx_msg void OnUpdateUserAdminMenu(CCmdUI* pCmdUI);
  afx_msg void OnUpdateUsrPasschangeMenu(CCmdUI* pCmdUI);
  afx_msg void OnSizing(UINT uSide, LPRECT lpRect);
  afx_msg void OnSize(UINT uType, int iCX, int iCY);
  afx_msg void OnProgramPreferences();
  afx_msg void OnClose();
  afx_msg void OnTimeChange();
  afx_msg void OnUpdateProgramPreferences(CCmdUI* pCmdUI);
  afx_msg void OnMove(int x, int y);

  //}}AFX_MSG
  /// Handles end of load study message
  LRESULT OnLoadViatronStudyProcDone(WPARAM bSuccess, LPARAM);
  /// Handles verification of view selection change
  LRESULT OnViewSelectionChanging(WPARAM iView, LPARAM);
  /// Handles message to change view selection
  LRESULT OnViewSelectionChanged(WPARAM iView, LPARAM);
  /// Handles message indicating transition between steps during study load
  LRESULT OnStudyLoadStepNotify(WPARAM iPrevAndNextStep, LPARAM iOrientation);
  /// Handles message to set progress bar position
  LRESULT OnSetFunctionProgress(WPARAM iProgress, LPARAM);
  /// Handles display of help string
  LRESULT OnDisplayHelpText(WPARAM iStringID, LPARAM);
  /// Handles display of current function/step text
  LRESULT OnDisplayFunctionAndStepText(WPARAM iFunctionStringID, LPARAM iStepStringID);
  /// Handles start and completion of a background process
  LRESULT OnBackgroundProcessStartFinish(WPARAM iStartParams, LPARAM iProcessId);
  /// Switches main dataset based on orientation
  LRESULT OnSetMainDataset(WPARAM wNewDatasetOrientation, LPARAM);
  /// Method invoked when view mode is changed.
  LRESULT OnAppViewModeChanged(WPARAM wpOldMode, LPARAM lpNewMode);
  /// advances to the next layout
  afx_msg void OnShowNextView();
  /// Accelerator message, aborts the load
  afx_msg void OnCancelLoad();

  DECLARE_MESSAGE_MAP()

private:
  /// Switches to the indicated view window
  void ShowView(const int4 iMainView, const int4 iSubViewState = 0, const bool bInitStudy = false);
  /// Sets patient and current user info in titlebar
  void SetPatientInfoInTitle();
  /// Handles user request to mark current study as complete
  void OnStudyComplete();
  /// Background process to load study
  static UINT LoadViatronStudyProc2(LPVOID pParam);
  /// Enables/disables menubar items Option, Notation, and Report
  void EnableMenuBar(const bool bEnable);
	/// disable entire menubar
	void DisableMenuBar();

public:
  /// Normal font for use throughout application
  HFONT m_hMainVtxFont;
  /// Large size font for use throughout application
  HFONT m_hLargeVtxFont;
  /// Normal font for use throughout application
  GLFont m_mainVtxGLFont;
  /// Large size font for use throughout application
  GLFont m_largeVtxGLFont;
  /// Use these messages to post instead of the corresponding registered windows messages
  enum ePrivateMsgs { WM_USER_SET_FUNCTION_PROGRESS = WM_USER + 100, WM_USER_DISPLAY_FUNCTION_TEXT };

protected:
  /// Current view frame, frame manager
  ColonViews::Layouts::ChildLayoutFrame* m_pCurrentViewFrame;
  /// Current view frame manager
  ColonViews::FramesManager::VFrameManagerPtr m_pCurrFrame;
  /// Current view frame class
  CRuntimeClass* m_pCurrClass;
  /// Stores current main view window
  int4 m_iCurrMainView;
  /// Stores current sub view window
  int4 m_iCurrSubView;

  /// Application status bar
  CStatusBar  m_statusBar;
  /// View selection dialog bar
  ColonViews::Dialogs::ViewSelectDlgBar m_viewSelBar;
  /// Navigation selection dialog bar
  ColonViews::Dialogs::NavigationBar m_navigationBar;

  /// friend declaration for access
  friend ColonViews::Layouts::ColonViatronixWnd;
  /// friend declaration for access
  friend ColonViews::Layouts::ViatronixWnd;

private:

  /// Map of background process (i.e. thread) identifiers and specified hide parameters
  typedef std::map<DWORD, HideParamsEnum> BackgroundProcIdMap;
  BackgroundProcIdMap m_bkgdProcIdMap;

  /// Reference counter to number of threads specifying each hide parameter
  typedef std::map<HideParamsEnum, uint4> HideParamsRefCountMap; // hide params, number of threads (reference count)
  HideParamsRefCountMap m_hideParamsRefCtMap;

  /// Enums identifying status bar pane IDs
  enum StatusBarPaneEnum { HELP_PANE = 0, FUNCTION_PANE, STEP_PANE, DATETIME_PANE };

  /// Timer to update system time (once per minute)
  WinUtils::WmTimer m_timerUpdateTime;
  /// Timer to reset help text (i.e. periodic text displayed after a successful operation)
  WinUtils::WmTimer m_timerResetHelpText;
  /// Timer to update the location lock for the open study
  WinUtils::WmTimer m_timerUpdateLocationLock;

  /// right border image
  WinUtils::RightBorder m_rightBorder;

  /// menubar
  WinUtils::Menubar m_menubar;

  /// previous view and subview indices
  std::pair<int4,int4> m_previousView;

  /// Study browser frame
  ColonViews::FramesManager::VFrameManager::StudyBrowserFrameManager     m_studyFrame;
  /// Verification frame
  ColonViews::FramesManager::VFrameManager::VerifyFrameManager           m_verifyFrame;
  /// Navigation frame
  ColonViews::FramesManager::VFrameManager::NavFrameManager              m_navigationFrame;
  /// Report frame
  ColonViews::FramesManager::VFrameManager::ReportFrameManager           m_reportFrame;

  /// Handle to the main menu
  HMENU m_hMDIMenu;
  /// Handle to the apps accelerator map
  HACCEL m_hMDIAccel;

  /// prefix to report file
  std::string m_sReportPrefix;

  /// instance of an observable object
  ObservableObject< MainAppFrameStates > m_observableHelper;

  static std::shared_ptr< Loader > m_loader;

  /// indicator for Link State display
	bool m_bShowLinkState;

}; // class MainAppFrame

