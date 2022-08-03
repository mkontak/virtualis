// $Id: VoxelPosition.h,v 1.3 2007/03/15 13:11:43 geconomos Exp $
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
  class vxCadVoxelPosition;
}

// class definition
class ATL_NO_VTABLE VoxelPosition : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< VoxelPosition >,
  public IDispatchImpl< IVoxelPosition, &__uuidof( IVoxelPosition ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( VoxelPosition )

// interface map
BEGIN_COM_MAP( VoxelPosition )
  COM_INTERFACE_ENTRY( IVoxelPosition )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IVoxelPosition implementation
public:

  /// gets the x component 
  STDMETHOD( get_X )( int * pfx );

  /// sets the x component
  STDMETHOD( put_X )( int fx );

  /// gets the y component
  STDMETHOD( get_Y )( int * pfy );

  /// sets the y component
  STDMETHOD( put_Y )( int fy );

  /// gets the z component
  STDMETHOD( get_Z )( int * pfz );

  /// sets the z component
  STDMETHOD( put_Z )( int fz );

  /// sets x, y, and z componets
  STDMETHOD( Set )( int x, int y, int z );

// member functions
public:
  
  /// default constructor
  VoxelPosition();

  /// gets the internal vector
  const vxCAD::vxCadVoxelPosition * GetInternalPtr() const { return m_pVoxelPosition; }

  /// sets the internal vector
  void SetInternalPtr( vxCAD::vxCadVoxelPosition * pVoxelPosition ) { m_pVoxelPosition = pVoxelPosition; }

// member variables
private:

  /// cad vector
  vxCAD::vxCadVoxelPosition * m_pVoxelPosition;

}; // coclass Vector

// $Log: VoxelPosition.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VoxelPosition.h,v 1.3 2007/03/15 13:11:43 geconomos Exp $
// $Id: VoxelPosition.h,v 1.3 2007/03/15 13:11:43 geconomos Exp $
