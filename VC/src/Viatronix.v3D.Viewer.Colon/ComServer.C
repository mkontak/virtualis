// $Id: ComServer.C,v 1.15.2.4 2009/08/24 17:50:24 dongqing Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#include "stdafx.h"
#include "ComServer.h"
#include "MainAppFrame.h"
#include "Viatron.h"
#include "User.h"
#include "AppSettings.h"

#define FILE_REVISION "$Revision: 1.15.2.4 $"

// namespaces
using namespace ReaderLib;


/**
 * Constructor
 */
ComServer::ComServer() : m_dwRegisterCookie( 0 )
{
} // ComServer()


/**
 * Loads the server
 *
 * @param     ppiUnknown    upon return contains IUnknown for server
 * @return    S_OK for success
 */
HRESULT ComServer::LoadServer( IUnknown ** ppiUnknown )
{
  HRESULT hr( S_OK );
  
  // check input parameters
  if( ppiUnknown == NULL )
  {
    return LogHRESULT( E_FAIL, "IUnknown is NULL!", "ComServer", "LoadServer" );
  }
  *ppiUnknown = NULL;
  
  // convert the server string into classid 
  CLSID clsid;
  if( FAILED( hr = CLSIDFromString(L"{CE999526-8935-40d1-ABCE-3B5DA8B75427}", &clsid ) ) )
  {
    return LogHRESULT( hr, "CLSIDFromString failed", "ComServer", "LoadServer" );
  }

  /// create instance of server and query for IUnknown
  CComCreator< CComObject< ComServer > >::CreateInstance( NULL, __uuidof( IUnknown ), reinterpret_cast< void ** >( ppiUnknown ) );

  /// register the server in the running object table
  if( FAILED( hr = RegisterActiveObject( *ppiUnknown, clsid, ACTIVEOBJECT_STRONG, &( GetCoClass< ComServer >( *ppiUnknown )->m_dwRegisterCookie ) ) ) )
  {
    return LogHRESULT( hr, "RegisterActiveObject failed", "ComServer", "LoadServer" );
  }

  return hr;
} // LoadServer( ppiUnknown )


/**
 * Unloads the server
 */
HRESULT ComServer::UnloadServer()
{
  HRESULT hr( S_OK );

  /// register the server in the running object table
  if( FAILED( hr = RevokeActiveObject( m_dwRegisterCookie, NULL ) ) )
  {
    return LogHRESULT( hr, "RevokeActiveObject failed", "ComServer", "UnloadServer" );
  }

  return hr;
} // UnloadServer()


/**
 * Shows the viewer
 *
 * @param rc
 * @param eState
 * @return    S_OK on success
 */
STDMETHODIMP ComServer::Show(RECT rc, WindowStateEnum eState )
{
  std::pair< int4,int4 > previous = static_cast< MainAppFrame * >( AfxGetMainWnd() )->GetPreviousView();
  if( previous.first != -1 )
  {
    AfxGetMainWnd()->SendMessage( WM_APP_VIEW_SELECTION_CHANGED, previous.first, previous.second );
  }
  
  HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

  WINDOWPLACEMENT wp;
  ZeroMemory( &wp, sizeof( WINDOWPLACEMENT ) );
  wp.length = sizeof( WINDOWPLACEMENT );
  
  // get the current WIDNOWPLACEMENT struct
  GetWindowPlacement( hWnd, &wp );

  wp.flags = WPF_ASYNCWINDOWPLACEMENT;
  wp.rcNormalPosition = rc;
  wp.showCmd = eState == NormalState? SW_RESTORE : SW_SHOWMAXIMIZED;

  m_rctConsole = rc;

  // update the WINDOWPLACEMENT
  //SetWindowPlacement( hWnd, &wp );    
  SetForegroundWindow( hWnd );
  PostMessage( hWnd, WM_NULL, 0, 0 );
  ShowWindow( hWnd, SW_SHOW );
  
  return S_OK;
} // Show( rc, eWindowState )


/**
 * Closes the viewer
 *
 * @return    S_OK on success
 */
STDMETHODIMP ComServer::Close()
{
  AfxGetMainWnd()->PostMessage( WM_CLOSE );
  return S_OK;
} // Close()


/**
 * Initializes the viewer
 *
 * @param   piConsole   console interface
 * @param iIdleInterval
 * @return              S_OK on success
 */
STDMETHODIMP ComServer::Initialize( IConsole * piConsole, int iIdleInterval )
{
  USES_CONVERSION;

  m_spConsole = piConsole;

	ViatronApp::GetViatronApp()->GetIdleManager().SetIdleInterval( iIdleInterval / 1000.0f );

  return S_OK;
} // Initialize( piConsole, iIdleInterval )


/**
 * Loads a session using the specified primary and secondary series
 *
 * @param     piSession     series for the session
 * @param     piPrimary     series for the primary dataset
 * @param     piSecondary   series for the secondary dataset
 * @return    S_OK on success
 */
STDMETHODIMP ComServer::Load( ISeries * piSession, ISeriesCollection * piPrimary, ISeriesCollection * piSecondary )
{
  USES_CONVERSION;

  HRESULT hr( S_OK );
  
  return S_OK;
} // Load( piSession, piPrimary, piSecondary )


/**
 * Sets the viewer as closing
 */
void ComServer::SetViewerClosing()
{
  HRESULT hr( S_OK );
  
  if( !m_spConsole )
  {
    LogHRESULT( E_FAIL, "Console pointer is NULL", "ComServer", "ShowConsole" );
  }

  ShowConsole();

  if( FAILED( hr = m_spConsole->SetViewerClosing() ) )
  {
    LogHRESULT( E_FAIL, "Show console failed", "ComServer", "ShowConsole" );
  }
} // SetViewerClosing()


/*
 * Hides the viewer and shows the console
 */
void ComServer::ShowConsole()
{
  HRESULT hr( S_OK );
  
  if( !m_spConsole )
  {
    LogHRESULT( E_FAIL, "Console pointer is NULL", "ComServer", "ShowConsole" );
  }

  int4 iProcessId( 0 );
  m_spConsole->get_ProcessId( &iProcessId );

  CWnd * pMainWnd( AfxGetMainWnd() );
  
  WINDOWPLACEMENT wp;
  ZeroMemory( &wp, sizeof( WINDOWPLACEMENT ) );
  wp.length = sizeof( WINDOWPLACEMENT );

  wp.rcNormalPosition = m_rctConsole;
  
  // get the current WIDNOWPLACEMENT struct
  GetWindowPlacement( pMainWnd->GetSafeHwnd(), &wp );

  WindowStateEnum eWindowState( NormalState );
  if( pMainWnd->IsIconic() )
  {
    eWindowState = MinimizedState;
  }
  else if( pMainWnd->IsZoomed() )
  {
    eWindowState = MaximizedState;
  }

  AllowSetForegroundWindow( iProcessId );

  if( FAILED( hr = m_spConsole->Show( wp.rcNormalPosition, eWindowState ) ) )
  {
    LogHRESULT( E_FAIL, "Show console failed", "ComServer", "ShowConsole" );
  }

  pMainWnd->ShowWindow( SW_HIDE );

} // ShowConsole()


/**
 * Logins into the console.
 *
 * @param     sUserName         user's login name
 * @param     sPassword         user's password
 *
 * @return    true if login successful; otherwise false
 */
bool ComServer::Login( const std::string & sUserName, const std::string & sPassword )
{
  HRESULT hr( S_OK );

  VARIANT_BOOL bResult( VARIANT_TRUE );
  if( FAILED( hr = m_spConsole->Login( _bstr_t( sUserName.c_str() ), _bstr_t( sPassword.c_str() ), &bResult )))
  {
    LogHRESULT( E_FAIL, "Login console failed", "ComServer", "Login" );
  }

  return ( bResult == VARIANT_TRUE ) ? true : false;
} // Login( sUserName, sPassword )


/**
 * Notifies the console that the application.
 */
void ComServer::LockApplication()
{
  HRESULT hr( S_OK );

  if( FAILED( hr = m_spConsole->LockApplication() ))
  {
    LogHRESULT( E_FAIL, "Locking Application Failed", "ComServer", "LockApplication" );
  }
} // LockApplication()


/**
 * signals the event that is freezing console
 *
 * @return    S_OK for success
 */
HRESULT ComServer::SignalEvent()
{
  HRESULT hr( S_OK );

  // create a named event (the console is blocking on this guy)
  HANDLE hNamedEvent = OpenEvent( EVENT_ALL_ACCESS , FALSE, "C5D9F4E1-6B67-4f8b-A904-BBE1FC64A1C6" );
  if( hNamedEvent == NULL  )
  {
    return LogHRESULT( E_FAIL, "Unable to create named event", "ComServer", "LoadServer" );
  }
  
  // let the console know we are ready for business
  SetEvent( hNamedEvent );
  
  // free named event
  CloseHandle( hNamedEvent ); 

  return hr;
} // SignalEvent()


/**
 * Builds the dataset load parameters from the specified ISeriesCollection
 *
 * @param     piSeriesCollection    ISeriesCollection instance
 * @param     params                upon return contains the required load parameters
 * @param     sHint                 indicates which volume to use as the base prefix
 * @return    S_OK on success
 */
HRESULT ComServer::BuildLoadParameters( ISeriesCollection * piSeriesCollection, DatasetLoadParams & params, const std::string & sHint )
{
  USES_CONVERSION;

  HRESULT hr( S_OK );

  // get volume series
  CComPtr< ISeries > spVolumeSeries;
  if( FAILED( hr = piSeriesCollection->Get( _bstr_t( "volume" ), &spVolumeSeries ) ) )
  {
    return LogHRESULT( hr, "Unable to retrieve ISeries for volume", "ComServer", "ExtractLoadParameters" );
  }

  // get volume series directory
  CComBSTR bsVolumeDirectory;
  if( FAILED( hr = spVolumeSeries->get_Directory( &bsVolumeDirectory ) ) )
  {
    return LogHRESULT( hr, "Unable to retrieve directory name for volume", "ComServer", "ExtractLoadParameters" );
  }

  // get preprocessed series
  CComPtr< ISeries > spPreprocessedSeries;
  if( FAILED( hr = piSeriesCollection->Get( _bstr_t( "preprocessed" ), &spPreprocessedSeries ) ) )
  {
    return LogHRESULT( hr, "Unable to retrieve ISeries for preprocessed", "ComServer", "ExtractLoadParameters" );
  }

  // get preprocessed series directory
  CComBSTR bsPreprocessedDirectory;
  if( FAILED( hr = spPreprocessedSeries->get_Directory( &bsPreprocessedDirectory ) ) )
  {
    return LogHRESULT( hr, "Unable to retrieve directory name for preprocessed", "ComServer", "ExtractLoadParameters" );
  }

  // get cad series (it isn't required)
  CComPtr< ISeries > spCadSeries;
  if( SUCCEEDED( hr = piSeriesCollection->Get( _bstr_t( "cad" ), &spCadSeries ) ) && ( !spCadSeries == false ) )
  {      
    // get cad series directory
    CComBSTR bsCadDirectory;
    if( FAILED( hr = spCadSeries->get_Directory( &bsCadDirectory ) ) )
    {
      return LogHRESULT( hr, "Unable to retrieve directory name for cad", "ComServer", "ExtractLoadParameters" );
    }
    params.SetCadDirectory( OLE2A( bsCadDirectory ) );
  }
  else
  {
    LogFyi( "Loading without cad", "ComServer", "BuildLoadParameters" );
  }

  // set the directories in the load parameters
  params.SetVolumeDirectory( OLE2A( bsVolumeDirectory ) );
  params.SetPreprocessedDirectory( OLE2A( bsPreprocessedDirectory ) );
  
  // set the prefix
  params.SetPrefix( DatasetLoadParams::GeneratePrefix( params.GetVolumeDirectory(), sHint ) );

  return hr;
} // ExtractLoadParameters( piSeriesCollection, params )


/**
 * Logs the specified error to the logfile
 *
 * @param     hr          hresult code
 * @param     sMsg        error message
 * @param     sClass      class name
 * @param     sFuntion    function name
 * @return    hresult of supplied error code
 */
HRESULT ComServer::LogHRESULT( HRESULT hr, const std::string & sMsg, const std::string & sClass, const std::string & sFunction )
{
  LogErr( sMsg, sClass, sFunction );
  return hr;
} // LogHRESULT( hr, sMsg, csClass, sFunction )



// $Log: ComServer.C,v $
// Revision 1.15.2.4  2009/08/24 17:50:24  dongqing
// code review
//
// Revision 1.15.2.3  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.15.2.2  2009/02/03 22:26:23  jmeade
// comments.
//
// Revision 1.15.2.1  2009/02/02 23:35:57  jmeade
// Issue 6218.
//
// Revision 1.15  2007/03/07 20:47:43  jmeade
// code standards.
//
// Revision 1.14  2007/02/28 15:23:32  gdavidson
// Removed unnecessary log message
//
// Revision 1.13  2007/02/12 19:17:16  jmeade
// Issue 5388: removed debugging message being output as an exception.
//
// Revision 1.12  2007/02/05 16:36:44  gdavidson
// Issue #5366: Signal the event that is blocking console on a license failure
//
// Revision 1.11  2006/12/21 16:44:34  gdavidson
// Added LockApplication
//
// Revision 1.10  2006/12/19 15:29:29  gdavidson
// Issue #5019: Added Login method
//
// Revision 1.9  2006/12/18 16:09:33  gdavidson
// Changed wait event to use a guid
//
// Revision 1.8  2006/07/07 13:23:52  gdavidson
// Updates the user rights when loading from console
//
// Revision 1.7  2006/04/07 20:16:26  gdavidson
// Issue #4558: Changed the in IViewer interface
//
// Revision 1.6  2006/01/13 19:16:38  geconomos
// removed call to show the console from SetViewerClosing()
//
// Revision 1.5  2005/11/23 18:33:55  geconomos
// launching from console revisted
//
// Revision 1.4  2005/11/23 03:59:18  geconomos
// viewer launching revisted
//
// Revision 1.3  2005/11/16 14:28:07  geconomos
// added the ability to hint at which volume ( .ecv or .vol ) to base the dataset prefix on
//
// Revision 1.2  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.1  2005/11/07 20:36:40  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ComServer.C,v 1.15.2.4 2009/08/24 17:50:24 dongqing Exp $
// $Id: ComServer.C,v 1.15.2.4 2009/08/24 17:50:24 dongqing Exp $
