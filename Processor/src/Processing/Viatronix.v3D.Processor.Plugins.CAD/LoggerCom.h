// $Id: LoggerCom.h,v 1.1 2005/09/23 12:07:29 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragma declarations
#pragma once

// include declarations
#include "vxComCAD.h"


// class definition
class ATL_NO_VTABLE Logger : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< Logger >,
  public IDispatchImpl< ILogger, &__uuidof( ILogger ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( Logger )

// interface map
BEGIN_COM_MAP( Logger )
  COM_INTERFACE_ENTRY( ILogger )
  COM_INTERFACE_ENTRY( IDispatch )
END_COM_MAP()

// ILogger implementation
public:

  /// logs the specified error message
  STDMETHOD( LogError )( BSTR bsMsg, BSTR bsFunction, BSTR bsClass );

  /// logs the specified warning message
  STDMETHOD( LogWarning )( BSTR bsMsg, BSTR bsFunction, BSTR bsClass );

  /// logs the specified informational message
  STDMETHOD( LogInformation )( BSTR bsMsg, BSTR bsFunction, BSTR bsClass );

  /// logs the specified debug message
  STDMETHOD( LogDebug )( BSTR bsMsg, BSTR bsFunction, BSTR bsClass );

  /// logs the specified temporary message
  STDMETHOD( LogTemporary )( BSTR bsMsg, BSTR bsFunction, BSTR bsClass );

// member funtions
private:

  // logs a message with the specified values
  void LogMessage( BSTR bsMsg, BSTR bsFunction, BSTR bsClass, const vxLogRecord::SeverityEnum & eSeverity );

}; // coclass Logger

// $Log: LoggerCom.h,v $
// Revision 1.1  2005/09/23 12:07:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/LoggerCom.h,v 1.1 2005/09/23 12:07:29 geconomos Exp $
// $Id: LoggerCom.h,v 1.1 2005/09/23 12:07:29 geconomos Exp $
