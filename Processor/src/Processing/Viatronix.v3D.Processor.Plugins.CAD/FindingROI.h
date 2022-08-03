// $Id: FindingROI.h,v 1.3 2007/03/15 13:11:42 geconomos Exp $
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
  class vxCadROI;
}

// class definition
class ATL_NO_VTABLE FindingROI : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< FindingROI >,
  public IDispatchImpl< IFindingROI, &__uuidof( IFindingROI ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE(FindingROI)

// interface map
BEGIN_COM_MAP( FindingROI )
  COM_INTERFACE_ENTRY( IFindingROI )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()


// IFindingROI implementation
public:

  /// gets the voxel positions collection
  STDMETHOD( get_Voxels )( IVoxelPositionCollection ** ppiVoxels  );

  /// gets the average HU
  STDMETHOD( get_AverageHU )( float * pfAverageHU  );

  /// sets the average HU
  STDMETHOD( put_AverageHU )( float  fAverageHU  );

  /// gets the standard deviation HU
  STDMETHOD( get_StandardDeviationHU )( float * pfStdDevHU  );

  /// sets the average HU
  STDMETHOD( put_StandardDeviationHU )( float  fStdDevHU  );

// member functions
public:
  
  /// default constructor
  FindingROI();

  /// called after the object is fully instantiated
  HRESULT FinalConstruct();
	
  /// called after the final reference to the instance has been released
	void FinalRelease();

  /// gets the vxCadROI pointer
  vxCAD::vxCadROI * GetInternalPtr() { return m_pROI; }

  /// sets the internal vxCadROI pointer ( class does NOT assume ownership )
  void SetInternalPtr( vxCAD::vxCadROI * pROI );

// member variables
private:

  // voxel position collection
  CComPtr< IVoxelPositionCollection > m_spVoxels;
  
  /// cad FindingROI
  vxCAD::vxCadROI * m_pROI;

}; // coclass FindingROI

// $Log: FindingROI.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingROI.h,v 1.3 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingROI.h,v 1.3 2007/03/15 13:11:42 geconomos Exp $
