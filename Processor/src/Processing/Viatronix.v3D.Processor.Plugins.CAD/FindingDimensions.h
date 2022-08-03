// $Id: FindingDimensions.h,v 1.3 2007/03/15 13:11:42 geconomos Exp $
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

// forward declarations
namespace vxCAD
{
  class vxCadDimensions;
}

// class definition
class ATL_NO_VTABLE FindingDimensions : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< FindingDimensions >,
  public IDispatchImpl< IFindingDimensions, &__uuidof( IFindingDimensions ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE(FindingDimensions)

// interface map
BEGIN_COM_MAP( FindingDimensions )
  COM_INTERFACE_ENTRY( IFindingDimensions )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IFindingDimensions implementation
public:

  /// gets the largest dimension
  STDMETHOD( get_Largest )( IFindingDimension ** ppiLargest );

  /// gets the perpendicular dimension
  STDMETHOD( get_Perpendicular )( IFindingDimension ** ppiLargest );

  /// gets the depth dimension
  STDMETHOD( get_Depth )( IFindingDimension ** ppiDepth );

// member functions
public:
  
  /// default constructor
  FindingDimensions();

  /// called after the object is fully instantiated
  HRESULT FinalConstruct();
	
  /// called after the final reference to the instance has been released
	void FinalRelease();

  /// gets the vxCadDimension pointer
  vxCAD::vxCadDimensions * GetInternalPtr() { return m_pDimensions; }

  /// sets the internal vxCadDimensions pointer ( class does NOT assume ownership )
  void SetInternalPtr( vxCAD::vxCadDimensions * pDimensions );

// member variables
private:

 // largest dimension
 CComPtr< IFindingDimension > m_spLargestDimension;

 // perpendicular dimension
 CComPtr< IFindingDimension > m_spPerpendicularDimension;

 // depth dimension
 CComPtr< IFindingDimension > m_spDepthDimension;

 // internal dimensions
 vxCAD::vxCadDimensions * m_pDimensions;
  
}; // coclass FindingDimensions 

// $Log: FindingDimensions.h,v $
// Revision 1.3  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingDimensions.h,v 1.3 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingDimensions.h,v 1.3 2007/03/15 13:11:42 geconomos Exp $
