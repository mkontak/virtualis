// $Id: Processor.h,v 1.3 2005/09/23 12:09:36 geconomos Exp $
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
class ATL_NO_VTABLE Processor : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< Processor >,
  public IDispatchImpl< IProcessor, &__uuidof( IProcessor ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE(Processor)

// interface map
BEGIN_COM_MAP( Processor )
  COM_INTERFACE_ENTRY( IProcessor )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IProcessor implementation
public:

  /// gets the environmet
  STDMETHOD( get_Environment )( IProcessorEnvironment ** ppiEnvironment );

  /// gets the findings
  STDMETHOD( get_Findings )( IFindingCollection ** ppiFindings );

  /// gets the logger
  STDMETHOD( get_Logger )( ILogger ** ppiLogger );

// member functions
public:
  
  /// default constructor
  Processor();

  /// called after the object is fully instantiated
  HRESULT FinalConstruct();
	
  /// called after the final reference to the instance has been released
	void FinalRelease();

// member variables
private:

  /// environment
  CComPtr< IProcessorEnvironment > m_spEnvironment;

  /// findings
  CComPtr< IFindingCollection > m_spFindings;

  /// logger
  CComPtr< ILogger > m_spLogger;

}; // coclass Processor 

// $Log: Processor.h,v $
// Revision 1.3  2005/09/23 12:09:36  geconomos
// added Logger property
//
// Revision 1.2  2005/09/20 18:58:00  geconomos
// changed all interfaces to be dual instead of cutom only
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Processor.h,v 1.3 2005/09/23 12:09:36 geconomos Exp $
// $Id: Processor.h,v 1.3 2005/09/23 12:09:36 geconomos Exp $
