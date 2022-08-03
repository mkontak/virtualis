// $Id: Finding.h,v 1.4 2007/03/15 13:11:42 geconomos Exp $
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
#include "vxCadFinding.h"

// class definition
class ATL_NO_VTABLE Finding : 
  public CComObjectRootEx< CComSingleThreadModel >, 
  public CComCoClass< Finding >,
  public IDispatchImpl< IFinding, &__uuidof( IFinding ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE(Finding)

// interface map
BEGIN_COM_MAP(Finding)
  COM_INTERFACE_ENTRY(IFinding)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IFinding implementation
public:

  /// gets the id
  STDMETHOD( get_Id )( BSTR * psId );

  /// gets the segment
  STDMETHOD( get_Segment )( unsigned char * piSegment );

  /// sets the segment
  STDMETHOD( put_Segment )( unsigned char iSegment );

  /// gets the dimensions
  STDMETHOD( get_Dimensions )( IFindingDimensions ** ppiDimensions );

  /// gets the centroid
  STDMETHOD( get_Centroid )( IVector ** ppiCentroid );

  /// gets the normal
  STDMETHOD( get_Normal )( IVector ** ppiNormal );

  /// gets the surface roi
  STDMETHOD( get_SurfaceROI )( IFindingROI ** ppiSurfaceROI );

  /// gets the volume roi
  STDMETHOD( get_VolumeROI )( IFindingROI ** ppiVolumeROI );

 /// gets the vendor specific data
 STDMETHOD( get_VendorSpecificData )( IDictionary ** ppiVolumeROI );

// member functions
public:
  
  /// default constructor
  Finding();

  /// called after the object is fully instantiated
  HRESULT FinalConstruct();
	
  /// called after the final reference to the instance has been released
	void FinalRelease();

  /// gets the internal findinds pointer
  vxCAD::vxCadFinding * GetInternalPtr() { return &m_finding;}
  
// member variables
private:

  // dimensions
  CComPtr< IFindingDimensions > m_spDimensions;

  // centroid
  CComPtr< IVector > m_spCentroid;

  // normal
  CComPtr< IVector > m_spNormal;

  // surface roi
  CComPtr< IFindingROI > m_spSurfaceROI;

  // volume roi
  CComPtr< IFindingROI > m_spVolumeROI;

  // vendor specific data
  CComPtr< IDictionary > m_spVendorSpecificData;

  /// cad finding
  vxCAD::vxCadFinding  m_finding;

}; // coclass Finding

// $Log: Finding.h,v $
// Revision 1.4  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2.2.1  2005/12/08 17:00:45  geconomos
// added VendorSpecificData property
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Finding.h,v 1.4 2007/03/15 13:11:42 geconomos Exp $
// $Id: Finding.h,v 1.4 2007/03/15 13:11:42 geconomos Exp $
