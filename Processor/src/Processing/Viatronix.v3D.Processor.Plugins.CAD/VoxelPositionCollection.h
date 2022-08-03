// $Id: VoxelPositionCollection.h,v 1.3 2007/03/15 13:11:43 geconomos Exp $
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
class ATL_NO_VTABLE VoxelPositionCollection : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< VoxelPositionCollection >,
  public IDispatchImpl< IVoxelPositionCollection, &__uuidof( IVoxelPositionCollection ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( VoxelPositionCollection )

// interface map
BEGIN_COM_MAP( VoxelPositionCollection )
  COM_INTERFACE_ENTRY( IVoxelPositionCollection )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IVoxelPosition implementation
public:

  /// gets the number of voxels in the collection
  STDMETHOD( get_Count )( int * piCount );

  /// adds a voxel position instance to the collection
  STDMETHOD( Add )( int x, int y, int z );

  /// clears out the voxel collection
  STDMETHOD( Clear )( );

  /// gets the voxel position at the specified index
  STDMETHOD( Get )( int iIndex, IVoxelPosition ** ppiVoxelPosition );

  /// removes the voxel position at the specified index
  STDMETHOD( Remove )( int iIndex );

// member functions
public:
  
  /// default constructor
  VoxelPositionCollection();

  /// gets the internal vector
  const vxCAD::vxCadROI * GetInternalPtr() const { return m_pROI; }

  /// sets the internal vector
  void SetInternalPtr( vxCAD::vxCadROI * pROI ) { m_pROI = pROI; }

// member variables
private:

  /// cad roi
  vxCAD::vxCadROI * m_pROI;

}; // coclass Vector

// $Log: VoxelPositionCollection.h,v $
// Revision 1.3  2007/03/15 13:11:43  geconomos
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VoxelPositionCollection.h,v 1.3 2007/03/15 13:11:43 geconomos Exp $
// $Id: VoxelPositionCollection.h,v 1.3 2007/03/15 13:11:43 geconomos Exp $
