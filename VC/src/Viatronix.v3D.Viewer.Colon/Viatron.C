// $Id: Viatron.C,v 1.41.2.5 2009/04/03 14:21:57 mkontak Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: Viatron.C
// Owner: Jeffrey R. Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "Viatron.h"
#include "ReaderGlobal.h"
#include "ViatronStudy.h"
#include "AboutDlg.h"
#include "MainAppFrame.h"
#include "VCSettings.h"
#include "Theme.h"
#include "BackgroundProcess.h"
#include "ViatronSplash.h"
#include "WinGLUtils.h"
#include "Crypt.h"
#include "vxConsole.h"
#include "vxDomDocument.h"
#include "SelectSeriesDialog.h"
#include "User.h"
#include "vxReportCreator.h"
#include "vxSystemSettings.h"
#include "CpuInfo.h"
#include "ThinClientLoadParams.h"
#include "MonitorDisplay.h"
#include "ServiceLocator.h"

#include "CpuInfo.h"
#include "LanguageSupport.h"
#include "LicenseFile.h"
#include "ApplicationSettings.h"
#include "vcUtils.h"
#include "Exception.h"
#include "vxTransportLayer.h"

#include "SystemInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class MyAtlModule : public CAtlModuleT< MyAtlModule > {};

MyAtlModule _AtlModule;

#define FILE_REVISION "$Revision: 1.41.2.5 $"

VxConsole::SingleMutex * ViatronApp::m_pAppSingleMutex = ((VxConsole::SingleMutex *) NULL);

using namespace ReaderLib;
using namespace WinUtils;
using namespace ConsoleLib;
using namespace ColonViews::Dialogs;

/**
 * ViatronApp pApp   The one and only ViatronApp object
 */
ViatronApp theApp;

/// global splash dialog instance
AboutDlg colonSplashDlg(IDB_V3D_COLON_ABOUT);

/**
 * message map
 */
BEGIN_MESSAGE_MAP(ViatronApp, CWinApp)
  ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
  ON_COMMAND(ID_HELP,OnHelpContents)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, OnUpdateAppAbout)
END_MESSAGE_MAP()


/**
 * Constructor
 *
 * Sets memory exception handlers
 */
ViatronApp::ViatronApp()
{
  // send all "new" memory exceptions thru VtxNewHandler function
  AfxSetNewHandler( VtxNewHandler );
  _set_new_handler( VtxNewHandler );
  _set_new_mode(1);

  try
  {
    // initialize the logger 
    ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_LOCAL ));

    vxLogFactory::CreateLogger( "vxcolonviewer", state.Get( APP_SETTING_LOG_PATH ),
                                                 state.Get< vxLogRecord::SeverityEnum >( APP_SETTING_LOG_LEVEL ),
                                                 state.Get< vxLogMessageFormats >( APP_SETTING_LOG_FORMAT ), true );
  }
  catch ( ex::VException & e )
  {
    
    LogErr("Failed initializing the application state : " + e.GetLogRecord().GetTextMessage(), "ViatroniApp", "ViatroniApp");
  }
} // ViatronApp() constructor


/**
 * Application class initialization
 *
 * @return   Success of application initialization
 */
BOOL ViatronApp::InitInstance()
{


#if !defined( FINAL_RELEASE ) && !defined( SKIP_EXT_DEBUGGER )
  if (__argc > 2)
  {
    const int1 sMsg[] = "Since you're launching a new copy of the V3D Viewer (to open a study),\n"
                        "here's your chance to attach a debugger to the newly launched process.";
    MessageWnd::Show(sMsg, 3000, MessageWnd::MW_WAIT);
  }
#endif

  // This is to guarantee that the timer calls reference the same processor timer core.
  // Otherwise, you get jumpy fly-thru's on some AMD systems (usually single processor, dual core systems).
  CpuInfo ci;
  if ( /*ci.GetVendor() == "AuthenticAMD" &&*/
       ci.GetCoresPerProcessor() == 2 &&
       ci.GetTotalCores() == 2 )
  {
    SetThreadAffinityMask( GetCurrentThread(), 1 );
  }


  // initialize gdi+
  Gdiplus::GdiplusStartupInput gsi;
  Gdiplus::GdiplusStartup(&m_pToken, &gsi, NULL);


  io::TransportLayer::GetCurrentTransportLayer().Initialize(VCSettings::GetInstance().GetSection("transportLayer") );

  CLanguageSupport::GetInstance().LoadBestLanguage();
  
  // parse command line
  ParseCommandLine( m_commandline );

  // do we need to register the server?
  if( m_commandline.GetRegServer() )
  {
    RegisterServer();
    return FALSE;
  }
  // do we need to unregister the server?
  else if( m_commandline.GetUnregServer() )
  {
    UnregisterServer();
    return FALSE;
  }
  else if( m_commandline.GetLicenseApplication() )
  {
    LicenseApplication();
    return FALSE;
  }

  // initialize common control library
  InitCommonControls();

  // COM initialization
  CoInitialize( NULL );
  AfxEnableControlContainer();


  // TODO: REST Use the following code if multiple instances of the viewer are allowed
  //try
  //{
  //  TCHAR szFileName[MAX_PATH + 1];
  //  RtlZeroMemory(szFileName, MAX_PATH + 1);
  //  GetModuleFileName( NULL, szFileName, MAX_PATH );

  //  uint4 uProcessCount(0);
  //  if ( (uProcessCount = SystemInfo::GetProcessCount(util::IO::GetFilename(szFileName))) > 1 )
  //  {
  //    AfxMessageBox("Viatronix Virtual Colonoscopy Application is already running, only 1 instance is allowed", MB_OK );
  //    return FALSE;
  //  }
  //}
  //catch ( ex::VException ex )
  //{
  //  std::string sMessage("Failed to determine f the application is running : " + ex.GetLogRecord().GetTextMessage());
  //  AfxMessageBox(sMessage.c_str(), MB_OK );
  //  return FALSE;
  //}
  


  // TODO: REST review an possible use the processes count version above taht allows for multple instances, but can have a limit
  // ensure there is only one running instance of the viewer application
  m_pAppSingleMutex = new VxConsole::SingleMutex( FALSE, VxConsole::VxMutex::sV3D_VC_VIEWER_MUTEX_NAME, NULL );
  if( m_pAppSingleMutex->Lock( 0 ) == FALSE )
    return FALSE;

  WinGLUtils::Text::sOK = LPCTSTR(LoadResourceString(IDS_OK));
  WinGLUtils::Text::sCancel = LPCTSTR(LoadResourceString(IDS_CANCEL));

  // initialize the ui theme
  Theme::Initialize();

  // initialize the xml library
  vxDOMDocument::Initialize();


  // create main window
  MainAppFrame * pMainFrame =  static_cast< MainAppFrame * >( m_pMainWnd = new MainAppFrame() );
  if( !pMainFrame->LoadFrame( IDR_MAINFRAME ) )
    return FALSE;

#ifndef DEBUG
  if( !LicenseFile::Check( GetLicenseFilePath() ))
  {
    AfxMessageBox( "Please contact customer support.", MB_OK );
    return FALSE;
  }
#endif

	CleanseTempReportFile();

  if( !m_commandline.GetUIDs().empty() )
  {
    colonSplashDlg.Create(AboutDlg::IDD);
    colonSplashDlg.CenterWindow();
    colonSplashDlg.ShowWindow(SW_SHOW);
    colonSplashDlg.RedrawWindow();

    StudyLoadParamsEx params;
    params.SetUIDs( m_commandline.GetUIDs() );

    // launch from console
    if( m_commandline.GetLaunchedFromConsole() )
    {
      // authenticate username and context id specified on the commandline
      if( ! LoginFromCommandLine() )
      {
        AfxMessageBox( IDS_INVALID_LOGIN );
        return FALSE;
      }

      params.SetLoaderStyle( StudyLoadParamsEx::LOADER_STYLE_CONSOLE );

    }
    else // load from commandline (stand-alone and PACS)
    {
      // login username and password passed on commandline
      if( !LoginFromCommandLine() )
      {
        AfxMessageBox( IDS_INVALID_LOGIN );
        return FALSE;
      }

      params.SetLoaderStyle( StudyLoadParamsEx::LOADER_STYLE_DATABASE );
    }

		SetDisplayPosition(params);

    pMainFrame->Open( params );

    Sleep( 1000 );

    HideSplashScreen();

    MonitorDisplay::SetCurrentMonitor();
    AfxGetMainWnd()->PostMessage( WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_VERIFICATION, 0 );     

    pMainFrame->ShowWindow( SW_SHOW );
    pMainFrame->UpdateWindow(); 
  }
  else
  {
    if( !m_licenseClient.HasLicense() && !LoginFromCommandLine() && !m_licenseClient.Login() )
    {
      LogErr( "User failed to acquire license.", "ViatronApp", "InitInstance" );
      return FALSE;
    }

    AfxGetMainWnd()->PostMessage( WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_VERIFICATION, 0 );

    MonitorDisplay::SetCurrentMonitor();

    pMainFrame->ShowWindow( SW_SHOW );
    pMainFrame->UpdateWindow();
    
    SelectSeriesDialog seriesDlg;
    if( seriesDlg.DoModal() == IDOK )
    {
			StudyLoadParamsEx params(seriesDlg.GetStudyLoadParams());
			SetDisplayPosition(params);

      pMainFrame->Open( params );
    }  
  }

  return TRUE;
} // InitInstance()


/**
 * Called upon app exit
 *
 * @return   Value of CWinApp::ExitInstance
 */
int ViatronApp::ExitInstance()
{

  CloseAppMutexHandle();

  if( m_pToken )
    Gdiplus::GdiplusShutdown(m_pToken);

  return CWinApp::ExitInstance();
} // ExitInstance()


/**
 * Registers the com server
 */
void ViatronApp::RegisterServer()
{
  HRESULT hr( _AtlModule.RegisterServer( TRUE ) );
  if( FAILED( hr ) )
  {
    std::strstream str;
    str << "Server registration failed. hr=" << hr;
    std::cout << str.str() << std::endl;
  }
  else
    std::cout << "Server registration succeeded" << std::endl;
    
} // RegisterServer()


/**
 * Unregisters the com server
 */
void ViatronApp::UnregisterServer()
{
  HRESULT hr( _AtlModule.UnregisterServer( TRUE ) );
  if( FAILED( hr ) )
  {
    std::strstream str;
    str << "Server unregister failed. hr=" << hr;
  }
  else
    std::cout << "Server unregister succeeded" << std::endl;
    
} // UnregisterServer()


/**
 * Closes the application mutex handle
 *
 * @return bool  true if the mutex handle is valid and is closed successfully
 */
bool ViatronApp::CloseAppMutexHandle()
{
  bool bWasOpenAndHasClosedSuccessfully(false);

  if (m_pAppSingleMutex != (VxConsole::SingleMutex*) NULL)
  {
    if (m_pAppSingleMutex->IsLocked())
    {
      bWasOpenAndHasClosedSuccessfully = m_pAppSingleMutex->Unlock() != FALSE;
    }

    delete m_pAppSingleMutex;
    m_pAppSingleMutex = (VxConsole::SingleMutex *) NULL;
  }

  return bWasOpenAndHasClosedSuccessfully;
} // CloseAppMutexHandle()


/**
 * Closes the current session
 */
void ViatronApp::CloseSession()
{

  // ============================================================
  // Only close the session if we have a context and session UID
  // =============================================================
  if (!ViatronApp::GetViatronApp()->GetContextId().empty() && !rdrGlobal.m_supine.GetSessionUID().empty())
  {
    LogFyi("Closing the session", "ViatroniApp", "CloseSession");
    ServiceLocator::GetRetrievalService()->CloseSession(ViatronApp::GetViatronApp()->GetContextId(), rdrGlobal.m_supine.GetSessionUID());
  }
}

/**
 * Releases the license and unlocks the transaction only if the context is not empty. If the context is empty either we have no license or
 * we have already released.
 *
 */
void ViatronApp::ReleaseLicense()
{

  try
  {

    // ===============================================
    // Only release and unlock if there is a context.
    // ================================================
    if (HasLicense())
    {

      /// Closes the session
      CloseSession();

      LogFyi("Releasing the application license", "ViatronixApp", "ReleaseLicense");

      m_licenseClient.Release();

    } // END ... If the context exists

  }
  catch (ex::VException & ex)
  {
    LogErr(ex.GetLogRecord().GetTextMessage(), "ViatronApp", "ReleaseLicense");
  }
  catch (...)
  {
    LogErr("ReleaseLicense failed : unspecified error", "ViatronApp", "ReleaseLicense");
  }

} // ReleaseLicense()

/**
* Renews the license only if the context is not empty. If the context is emptry either we have no license or
* we have already released.
*
*/
bool ViatronApp::RenewLicense()
{
  bool renewed(false);

  try
  {

    // ===============================================
    // Only release and unlock if there is a context.
    // ================================================
    if (HasLicense())
    {

      LogFyi("Renewing the application license", "ViatronixApp", "RenewLicense");


      renewed = m_licenseClient.Renew();
    } // END ... If the context exists

  }
  catch (ex::VException & ex)
  {
    LogErr(ex.GetLogRecord().GetTextMessage(), "ViatronApp", "RenewLicense");
  }
  catch (...)
  {
    LogErr("RenewLicense failed : unspecified error", "ViatronApp", "RenewLicense");
  }

  return renewed;

} // RenewLicense()


/**
* Renews the license only if the context is not empty. If the context is emptry either we have no license or
* we have already released.
*
*/
void ViatronApp::ReAuthenticate(const std::string & sLogin, const std::string & sPassword)
{

  LogFyi(util::Strings::Format("Re-authenticating user %s", sLogin.c_str()), "ViatronixApp", "ReAuthenticate");

  m_licenseClient.ReAuthenticate(sLogin, sPassword);

} // RenewLicense()



/**
 * On idle application event handler (redraws views)
 *
 * @param lCount   Idle count
 * @return         TRUE - framework will provide more idle events, FALSE otherwise
 */
BOOL ViatronApp::OnIdle(LONG lCount)
{
  if( m_pMainWnd->IsWindowVisible() )
    m_idleManager.Monitor();
  else
    m_idleManager.Reset();

  // No need to process idle messages if app is minimized or if this is the Console
  if (m_pMainWnd->IsIconic() )
  {
    return FALSE;
  }

  static Timer idleTimer;
  const float4 fMaxIdleFrameRate( 100.0F );
  if ( idleTimer.ElapsedTime() > 1.0F / fMaxIdleFrameRate )
  {

    idleTimer.Reset();
    MainAppFrame *pAppFrame = STATIC_DOWNCAST(MainAppFrame, m_pMainWnd);
    if (pAppFrame != NULL)
    {
      CFrameWnd *pFrameWnd = pAppFrame->GetActiveFrame();
      if( pFrameWnd && pFrameWnd->IsWindowVisible() )
      {
        pFrameWnd->PostMessage( WM_APP_GL_IDLE, 0, 0 );
      }
    }
  }

  // TODO: Kill app that is idle for a long period of time (particularly if no study is open)

  return TRUE;
} // OnIdle(LONG lCount)


/**
 * Hides the splash screen
 */
void ViatronApp::HideSplashScreen()
{
  if (IsWindow(colonSplashDlg))
  {
    colonSplashDlg.ShowWindow(SW_HIDE);
    colonSplashDlg.DestroyWindow();
  }
} // HideSplashScreen()


/**
 * Runs the about modal dialog
 */
void ViatronApp::OnAppAbout()
{
  AboutDlg aboutDlg( IDB_V3D_COLON_ABOUT );
  aboutDlg.DoModal();
} // OnAppAbout()


/**
 * Updates the on app about menu.
 *
 * @param pCmdUI   Command update UI handler.
 */
void ViatronApp::OnUpdateAppAbout(CCmdUI* pCmdUI) 
{
  CString sText;
  sText.Format(IDS_ABOUT_APP_FORMAT,
    LoadResourceString(IDS_V3DVIEWER_COLON ));
  pCmdUI->SetText(sText);
} // OnUpdateAppAbout(CCmdUI* pCmdUI) 


/**
 * Launches help.
 */
void ViatronApp::OnHelpContents()
{

  // first try the standard extension
  std::string sHelpFile(vx::File(VxConsole::GetAppPathname()).GetFullTitle() + vx::HelpFileExt);
 
  if ( vx::File( sHelpFile ).Exists() )
  {
    if (ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", sHelpFile.c_str(), "",
      NULL, SW_SHOWNORMAL) <= HINSTANCE(32))
    {
      CString sMsg; sMsg.Format(IDS_ERROR_GENERIC_FORMAT,
        LoadResourceString(IDS_OPEN), LoadResourceString(IDS_HELP_FACILITY));
      AfxMessageBox(sMsg + ": " + sHelpFile.c_str());
    }
  }

  // else try the new .chm format extension
  else
  {
    sHelpFile = vx::File(VxConsole::GetAppPathname()).GetFullTitle() + ".chm";
    if (ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", sHelpFile.c_str(), "",
      NULL, SW_SHOWNORMAL) <= HINSTANCE(32))
    {
      CString sMsg; sMsg.Format(IDS_ERROR_GENERIC_FORMAT,
        LoadResourceString(IDS_OPEN), LoadResourceString(IDS_HELP_FACILITY));
      AfxMessageBox(sMsg + ": " + sHelpFile.c_str());
    }
  }
} // OnHelpContents()


/**
 * Returns the registry section for user settings for this application
 *
 * @return   Registry section for application-specific settings for the current user
 */
std::string ViatronApp::GetUserRegistrySection()
{
  //std::string sReg( std::string( "Settings\\" ) + User::GetCurrentUser().GetLogin() += "\\Colon" );
  std::string sReg( "userSettings" );
  return sReg;
} // GetUserRegistrySection()


/**
 * New allocation memory exception handler
 *
 * @param uSize   Size of allocation
 * @return        Non-zero to retry allocation, 0 otherwise
 */
int AFX_CDECL ViatronApp::VtxNewHandler(size_t nSize)
{
  char sMemoryException[101];
  sprintf(sMemoryException, "Memory allocation failed for %d bytes", nSize);
#ifndef FINAL_RELEASE
  AfxMessageBox(sMemoryException, MB_ICONEXCLAMATION);
#endif
  TRACE(sMemoryException);
  throw ex::OutOfMemoryException(LogRec(sMemoryException,"CViatronApp","VtxNewHandler"));
  return 0;
} // VtxNewHandler(size_t nSize)


/**
 *  Returns the path to the license file
 */
std::string ViatronApp::GetLicenseFilePath() const
{
  char sExePath[ _MAX_PATH ];
  GetModuleFileName( NULL, sExePath, _MAX_PATH );
  char sDirectory[_MAX_DIR ];
  _splitpath_s( sExePath, NULL, 0, sDirectory, _MAX_DIR, NULL, 0, NULL, 0 );

  return std::string( sDirectory ) + "license";
} // GetLicenseFilePath()


/**
 * Generates a pdf file from the specified vrx file
 *
 * @param   sVrxFileName    vrx filename
 * @param   sPdfFileName    pdf filename 
 */
bool ViatronApp::GeneratePdf( const std::string & sVrxFileName, const std::string & sPdfFileName )
{
  bool bSuccess( false );
  try
  {

    // convenience hr
    HRESULT hr( S_OK );

    // verify that the vrx file exists
    if( !vx::File( sVrxFileName ).Exists() )
    {
      LogErr( "Report vrx file does not exist", "CViatronApp", "GeneratePdf" );
      return false;
    }
    
    // create an instance of the report
    ReportLib::IReportPtr spReport;
    if (FAILED( hr = spReport.CreateInstance( __uuidof( ReportLib::Report ) ) ) ) 
    {      
      LogErr( "Unable to create report instance. hr=" + ToAscii( hr ), "CViatronApp", "GeneratePdf" );
      return false;
    }

    // open the vrx file
    spReport->Open( sVrxFileName.c_str()  );
      
    // grab the first revision
    ReportLib::IReportRevisionPtr spRevision = spReport->Revisions->GetItem( 0L );

    // create the PDF document
    ConsoleLib::VxReportCreator pdfCreator( spRevision );
    bSuccess = pdfCreator.Create( sPdfFileName.c_str() ) ;
  }
  catch( _com_error & e )
  {
    LogErr( e.ErrorMessage(), "CViatronApp", "GeneratePdf" );
  }
  catch( ... )
  {
    LogErr( "Unhandled exception occurred while generated pdf.", "CViatronApp", "GeneratePdf" );
  }
  return bSuccess;
} // GeneratePdf( const std::string & sVrxFileName, const std::string, & sPdfFileName )


/**
 * Generates the pdf and snapshots
 *
 * @param       sReportFile           report path
 * @param       sOutputPath           output directory
 * @return      true if successful; otherwise false
 */
bool ViatronApp::GenerateOutputFiles( const std::string & sReportFile, const std::string & sOutputPath )
{
  bool bResult( false );

  ReportLib::IReportPtr spReport;
  if( Utilities::Reporting::OpenReport(sReportFile, spReport ))
  {
    // close the report
    SCOPE_EXIT { spReport->Close(); };

    ReportLib::IReportRevisionPtr spRevision = spReport->Revisions->GetItem( spReport->Revisions->Count - 1 );

    // create the PDF document
    std::string pdfPath( sOutputPath + "\\report.pdf" );
    ConsoleLib::VxReportCreator pdfCreator( spRevision );
    if( bResult = pdfCreator.Create( pdfPath.c_str() ))
    {
      // create snapshots
      Utilities::Imaging::ImageList images;
      if( bResult = Utilities::Reporting::ExtractSnapshots( spRevision, images ))
        Utilities::Imaging::WriteImages( images, sOutputPath );
    }
  }

  return bResult;
} // GenerateOutputFiles( sReportFile, sOutputPath )


/** 
 * send to pacs
 */
bool ViatronApp::TransferToPacs( const std::string & sReportFile, const std::string & uid, uint4 iSendOptions )
{
  bool bResult( false );

  LogDbg( "Transfer to pacs started.", "ViatronApp", "TransferToPacs" );

  ReportLib::IReportPtr spReport;
  if( Utilities::Reporting::OpenReport( sReportFile, spReport ))
  { 
    // close the report
    SCOPE_EXIT { spReport->Close(); };

    ReportLib::IReportRevisionPtr spRevision = spReport->Revisions->GetItem( spReport->Revisions->Count - 1 );

    try
    {
      Utilities::Reporting::SendToPacs( spRevision, uid, static_cast< Utilities::Reporting::PacsSendOptions >( iSendOptions ));
      bResult = true;
    }
    catch( ex::VException & )
    {
      LogErr( "Transfer to pacs failed.", "ViatronApp", "TransferToPacs" );
 	  AfxMessageBox("An error occurred while transferring report to PACS.\nPlease make sure PACS conncetion setup correctly.");
    }

    LogDbg( "Transfer to pacs completed.", "ViatronApp", "TransferToPacs" );
  }
	else
	{
		AfxMessageBox("Report file failed to load.");
	}

  return bResult;
} // TransferToPacs( sReportFile, uid, iSendOptions )


/**
 * Licenses the application
 */
void ViatronApp::LicenseApplication()
{
  try
  {
    LicenseFile::Generate( GetLicenseFilePath() );
  }
  catch( ex::VException & )
  {
    std::cerr << "Unable to license application." << std::endl;
    AfxMessageBox( "Unable to license application.\nAn previous license might exist." );
  }
} // LicenseApplication()


/**
 * Logs the user in using commandline args
 *
 * @return        true if a successful login; otherwise false
 */
bool ViatronApp::LoginFromCommandLine()
{
  try
  {
    if( m_commandline.GetLogin().empty() )
      return false;

    // login for user
    std::vector< std::string > tokens;
    Utilities::RegularExpression::Split( m_commandline.GetLogin(), "\\|", tokens );
    return m_licenseClient.Login( tokens[0], tokens[1], ( tokens.size() > 2 ? tokens[2] : "" ) );
  }
  catch( ... )
  {
    LogErr( "Failed to login.", "ViatronApp", "LoginFromCommandLine" );
    return false;
  }
} // LoginFromCommandLine()


/**
 * set display position based on the input datasets' body position
 *
 *  @param params the load parameters object that will be reset
 */
void ViatronApp::SetDisplayPosition(StudyLoadParamsEx & params)
{
	const std::string sDataset_1 = params.GetPrimaryDataset().GetVolumeDirectory();
	const std::string sPosition_1 = GetBodyPosition(sDataset_1);

	const std::string sDataset_2 = params.GetSecondaryDataset().GetVolumeDirectory();
	const std::string sPosition_2 = GetBodyPosition(sDataset_2);

	/// swap the primary and secondary if necessary based on the radiologist's habit
	if ((sPosition_1 == "P" && sPosition_2 == "") || (sPosition_1 == "P" && sPosition_2 == "S") ||
		(sPosition_1 == "P" && sPosition_2 == "DR") || (sPosition_1 == "P" && sPosition_2 == "DL") ||
		(sPosition_1 == "DL" && sPosition_2 == "") || (sPosition_1 == "DL" && sPosition_2 == "S") ||
		(sPosition_1 == "DR" && sPosition_2 == "S") || (sPosition_1 == "DR" && sPosition_2 == "DL") ||
		(sPosition_1 == "DR" && sPosition_2 == ""))
	{
		DatasetLoadParams datapara01 = (params.GetPrimaryDataset());
		DatasetLoadParams datapara02 = (params.GetSecondaryDataset());

		/// reset the primary dataset
		params.GetPrimaryDataset().SetPreprocessedDirectory(datapara02.GetPreprocessedDirectory());
		params.GetPrimaryDataset().SetUID(datapara02.GetUID());
		params.GetPrimaryDataset().SetCadDirectory(datapara02.GetCadDirectory());
		params.GetPrimaryDataset().SetPrefix(datapara02.GetPrefix());
		params.GetPrimaryDataset().SetVolumeDirectory(datapara02.GetVolumeDirectory());
		params.GetPrimaryDataset().SetOrientation(datapara02.GetOrientation());

		/// reset the secondary dataset
		params.GetSecondaryDataset().SetPreprocessedDirectory(datapara01.GetPreprocessedDirectory());
		params.GetSecondaryDataset().SetUID(datapara01.GetUID());
		params.GetSecondaryDataset().SetCadDirectory(datapara01.GetCadDirectory());
		params.GetSecondaryDataset().SetPrefix(datapara01.GetPrefix());
		params.GetSecondaryDataset().SetVolumeDirectory(datapara01.GetVolumeDirectory());
		params.GetSecondaryDataset().SetOrientation(datapara01.GetOrientation());

		uint4 uFlags(0);
		if (params.GetPrimaryDataset().GetVolumeDirectory().length() && params.GetPrimaryDataset().GetPreprocessedDirectory().length())
			uFlags |= StudyLoadParams::PRIMARY_DATASET_VALID;

		if (params.GetSecondaryDataset().GetVolumeDirectory().length() && params.GetSecondaryDataset().GetPreprocessedDirectory().length())
			uFlags |= StudyLoadParams::SECONDARY_DATASET_VALID;

		params.SetFlags(uFlags);
	} // SetDisplayPosition()
} // SetDisplayPosition(StudyLoadParamsEx & params)


/**
*  determine the body position from the volume header
*
*  @param sVolumeFilePath
*  @return the string of the body position : S, P, DL, or DR
*/
std::string ViatronApp::GetBodyPosition(const std::string & sVolumeFilePath)
{
	if ((sVolumeFilePath.size() < 1) || (_chdir(sVolumeFilePath.c_str()) == -1))
	{
		return std::string("");
	}

	struct _finddata_t c_file;
	intptr_t hFile;

	std::string sTmp = "*.vol";
	if ((hFile = _findfirst(sTmp.c_str(), &c_file)) != -1)
	{
		sTmp = c_file.name;

		vxVolumeHeader vhd;
		vhd.Read(sTmp);

		/// get the image orientation cosine
		std::string sImageOrientation("");
		sImageOrientation = vhd.GetDicomFieldValue("0020", "0037", sImageOrientation);

		/// determine the body position based on the cosine
		std::string sBreaker = "/\\,:";
		int4 i, n = sImageOrientation.size();
		int4 j[5], k = 0;
		for (i = 0; i<5; i++)
			j[i] = -1;

		for (i = 0; i<n; i++)
		{
			if (sImageOrientation[i] == sBreaker[0] || sImageOrientation[i] == sBreaker[1] ||
				sImageOrientation[i] == sBreaker[2] || sImageOrientation[i] == sBreaker[3])
			{
				j[k] = i;
				k++;
			} // if
		} // for i

		sTmp = "";
		if (j[0] < 0 || j[1] < 0 || j[2] < 0 || j[3] < 0 || j[4] < 0)
		{
			LogWrn("Could not retrieve image orientation patient info correctly", "FileSystemLoader", "GetBodyPosition");
		} // if
		else
		{ /// get the orientation cosine
			std::string srx, sry, srz, scx, scy, scz;
			float4      frx, fry, frz, fcx, fcy, fcz;
			for (i = 0; i<j[0]; i++)
				srx.push_back(sImageOrientation[i]);

			ToType(srx, frx);

			for (i = j[0] + 1; i<j[1]; i++)
				sry.push_back(sImageOrientation[i]);

			ToType(sry, fry);

			for (i = j[1] + 1; i<j[2]; i++)
				srz.push_back(sImageOrientation[i]);

			ToType(srz, frz);

			for (i = j[2] + 1; i<j[3]; i++)
				scx.push_back(sImageOrientation[i]);

			ToType(scx, fcx);

			for (i = j[3] + 1; i<j[4]; i++)
				scy.push_back(sImageOrientation[i]);

			ToType(scy, fcy);

			for (i = j[4] + 1; i<n; i++)
				scz.push_back(sImageOrientation[i]);

			ToType(scz, fcz);

			if ((fcy == 1.0F && frx == 1.0F) || (fcy == 1.0F && frx == -1.0F))
			{
				sTmp = "S";
			}
			else if ((fcy == -1.0F && frx == -1.0F) || (fcy == -1.0F && frx == 1.0F))
			{
				sTmp = "P";
			}
			else if ((fry == 1.0F && fcx == -1.0F) || (fry == -1.0F && fcx == -1.0F))
			{
				sTmp = "DR";
			}
			else if ((fry == -1.0F && fcx == 1.0F) || (fry == 1.0F && fcx == 1.0F))
			{
				sTmp = "DL";
			} // else
			else
			{
				LogWrn("Could not retrieve image orientation patient info correctly", "FileSystemLoader", "GetBodyPosition");
			} // if
		} // else

		_findclose(hFile);

		return sTmp;
	} // if
	else
	{
		LogErr("Could not find volume file.", "FileSystemLoader", "GetBodyPosition");
		return std::string("");
	} // else

} // GetBodyPosition


/**
*  Remove all temporary report vrx files
*/
void ViatronApp::CleanseTempReportFile()
{
	int1 sTempDir[MAX_PATH + 1];
	const int4 iPathStringLen(GetTempPath(MAX_PATH, sTempDir));
	GetLongPathName(sTempDir, sTempDir, MAX_PATH);

	std::string sReportPath = std::string(sTempDir);

	// Include the parent path if non-empty
	_chdir(sReportPath.c_str());

	struct _finddata_t c_file;
	intptr_t hFile;

	std::string sss = "*" + vx::ReportExt;

	std::vector<std::string> sv;
	if ((hFile = _findfirst(sss.c_str(), &c_file)) != -1)
	{ /// remove all .vrx file in the current directory
		do
		{
			sv.push_back(c_file.name);
		} while (_findnext(hFile, &c_file) == 0);

		_findclose(hFile);

		for (int4 i = 0; i < sv.size(); i++)
			remove(sv[i].c_str());
	} // if

} // CleanseTempReportFile()