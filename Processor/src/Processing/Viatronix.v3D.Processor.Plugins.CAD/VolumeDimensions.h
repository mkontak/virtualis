// $Id: VolumeDimensions.h,v 1.4 2007/03/09 16:29:59 dongqing Exp $
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
class ATL_NO_VTABLE VolumeDimensions : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< VolumeDimensions >,
  public IDispatchImpl< IVolumeDimensions, &__uuidof( IVolumeDimensions ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( VolumeDimensions )

// interface map
BEGIN_COM_MAP( VolumeDimensions )
  COM_INTERFACE_ENTRY( IVolumeDimensions )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IDimensions dimensions
public:

  /// gets the x dimensions
  STDMETHOD( get_X )( long * pix );

  /// gets the y dimensions
  STDMETHOD( get_Y )( long * piy );

  /// gets the z dimensions
  STDMETHOD( get_Z )( long * piz );

// member functions
public:

  /// gets the internal dimensions 
  Triple< int4 > & GetDimensions() { return m_dimensions; } 

// member variables
private:

  /// dimensions
  Triple< int4 > m_dimensions;

}; // coclass Dimensions


// $Log: VolumeDimensions.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VolumeDimensions.h,v 1.4 2007/03/09 16:29:59 dongqing Exp $
// $Id: VolumeDimensions.h,v 1.4 2007/03/09 16:29:59 dongqing Exp $
