// $Id: ComServer.h,v 1.9.2.2 2009/07/30 17:58:19 kchalupa Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#pragma once

// include declarations
#include <atlbase.h>
#include <atlcom.h>
#include "ComUtilities.h"
#include "ConsoleInterop.h"
#include "StudyLoadParams.h"
#include <functional>

// class declaration
class ATL_NO_VTABLE ComServer :  
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< ComServer >,
  public IDispatchImpl< IViewer, &__uuidof( IViewer ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( ComServer )

// interface map
BEGIN_COM_MAP( ComServer )
  COM_INTERFACE_ENTRY( IViewer )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IViewer
public:

  /// initializes the viewer
  STDMETHOD( Initialize )( IConsole * piConsole, int iIdleInterval );

  /// shows the viewer
  STDMETHOD( Show )( RECT rc, WindowStateEnum eState );

  /// closes the viewer
  STDMETHOD( Close )();

  /// loads a session using the specified primary and secondary series
  STDMETHOD( Load )( ISeries * piSession, ISeriesCollection * piPrimary, ISeriesCollection * piSecondary );
 
// construction
public:

  /// constructor
  ComServer();
  
// member functions
public:

  /// sets the viewer as closing
  void SetViewerClosing();

  /// shows the console
  void ShowConsole();

  /// logins into console
  bool Login( const std::string & sUserName, const std::string & sPassword );

  /// notifies the console that the application.
  void LockApplication();

  /// signals the event that is freezing console
  static HRESULT SignalEvent();

  /// loads the server
  static HRESULT LoadServer( IUnknown ** ppiUknown );

  /// unloads the server
  HRESULT UnloadServer();

private:

  /// builds the dataset load parameters from the specified ISeriesCollection
  HRESULT BuildLoadParameters( ISeriesCollection * piSeriesCollection, ReaderLib::DatasetLoadParams & params, const std::string & sHint );

  /// logs the specified error to the logfile
  static HRESULT LogHRESULT( HRESULT hr, const std::string & sMsg, const std::string & sClass, const std::string & sFunction );

// member variables
private:

  /// console interface
  CComPtr< IConsole > m_spConsole;

  /// handle to instance in running object table
  DWORD m_dwRegisterCookie;

  /// window position of console
  CRect m_rctConsole;

}; // class ComServer

// $Log: ComServer.h,v $
// Revision 1.9.2.2  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.9.2.1  2009/02/02 23:35:57  jmeade
// Issue 6218.
//
// Revision 1.9  2007/02/05 16:36:44  gdavidson
// Issue #5366: Signal the event that is blocking console on a license failure
//
// Revision 1.8  2006/12/21 16:44:34  gdavidson
// Added LockApplication
//
// Revision 1.7  2006/12/19 15:29:29  gdavidson
// Issue #5019: Added Login method
//
// Revision 1.6  2006/04/07 20:15:37  gdavidson
// Issue #4558: Changed the in IViewer interface
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ComServer.h,v 1.9.2.2 2009/07/30 17:58:19 kchalupa Exp $
// $Id: ComServer.h,v 1.9.2.2 2009/07/30 17:58:19 kchalupa Exp $
