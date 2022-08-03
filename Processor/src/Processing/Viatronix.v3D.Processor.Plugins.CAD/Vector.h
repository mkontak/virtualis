// $Id: Vector.h,v 1.5 2007/03/15 13:11:43 geconomos Exp $
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
  class vxCadVector;
}

// class definition
class ATL_NO_VTABLE Vector : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< Vector >,
  public IDispatchImpl< IVector, &__uuidof( IVector ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( Vector )

// interface map
BEGIN_COM_MAP( Vector )
  COM_INTERFACE_ENTRY( IVector )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IVector implementation
public:

  /// gets the x component 
  STDMETHOD( get_X )( float * pfx );

  /// sets the x component
  STDMETHOD( put_X )( float fx );

  /// gets the y component
  STDMETHOD( get_Y )( float * pfy );

  /// sets the y component
  STDMETHOD( put_Y )( float fy );

  /// gets the z component
  STDMETHOD( get_Z )( float * pfz );

  /// sets the z component
  STDMETHOD( put_Z )( float fz );

 /// gets the units
  STDMETHOD( get_Units )( VectorUnits * pUnits );

  /// sets the units
  STDMETHOD( put_Units)( VectorUnits  eUnits );

  /// sets x, y, and z components
  STDMETHOD( Set )( float fx, float fy, float fz );

// member functions
public:
  
  /// default constructor
  Vector();

  /// gets the internal vector
  const vxCAD::vxCadVector * GetInternalPtr() const { return m_pVector; }

  /// sets the internal vector
  void SetInternalPtr( vxCAD::vxCadVector * pVector ) { m_pVector = pVector; }

// member variables
private:

  /// cad vector
  vxCAD::vxCadVector * m_pVector;

}; // coclass Vector

// $Log: Vector.h,v $
// Revision 1.5  2007/03/15 13:11:43  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.4  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.3  2005/09/30 19:54:02  geconomos
// added VectorUnits
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Vector.h,v 1.5 2007/03/15 13:11:43 geconomos Exp $
// $Id: Vector.h,v 1.5 2007/03/15 13:11:43 geconomos Exp $
