// $Id: VolumeUnits.h,v 1.4 2007/03/09 16:29:59 dongqing Exp $
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
class ATL_NO_VTABLE VolumeUnits : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< VolumeUnits >,
  public IDispatchImpl< IVolumeUnits, &__uuidof( IVolumeUnits ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( VolumeUnits )

// interface map
BEGIN_COM_MAP( VolumeUnits )
  COM_INTERFACE_ENTRY( IVolumeUnits )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IVolumeUnits dimensions
public:

  /// returns the units in x
  STDMETHOD( get_X )( float * pfx );

  /// returns the units in y
  STDMETHOD( get_Y )( float * pfy );

  /// returns the units in z
  STDMETHOD( get_Z )( float * pfz );

// member functions
public:
  
  /// volume units
  Triple< float4 > & GetVolumeUnits() { return m_units; } 

// member variables
private:

  /// volume units
  Triple< float4 > m_units;

}; // coclass VolumeUnits


// $Log: VolumeUnits.h,v $
// Revision 1.4  2007/03/09 16:29:59  dongqing
// fix the code review error
//
// Revision 1.3  2007/03/07 16:25:06  gdavidson
// Code review
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VolumeUnits.h,v 1.4 2007/03/09 16:29:59 dongqing Exp $
// $Id: VolumeUnits.h,v 1.4 2007/03/09 16:29:59 dongqing Exp $
