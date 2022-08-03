// $Id: Volume.h,v 1.4 2007/03/07 16:03:16 gdavidson Exp $
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
template< typename VoxelType >
class ATL_NO_VTABLE Volume : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< Volume< VoxelType >  >,
  public IDispatchImpl< IVolume, &__uuidof( IVolume ) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( Volume )

// interface map
BEGIN_COM_MAP( Volume )
  COM_INTERFACE_ENTRY( IVolume )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IVolume implementation
public:

  /// gets the dimensions of the volume
  STDMETHOD( get_Dimensions )( IVolumeDimensions ** piDimensions );

  /// gets the units for the volume
  STDMETHOD( get_Units )( IVolumeUnits ** piUnits );

  /// gets the voxels for the volume
  STDMETHOD( get_Voxels )( VARIANT * pVoxels );

  /// gets the value for the specified dicom header variable
  STDMETHOD( GetHdrVar )( BSTR sGroup, BSTR sField, BSTR * pValue );

// member functions
public:
  
  /// default constructor
  Volume();

  /// called after the object is fully instantiated
  HRESULT FinalConstruct();
	
  /// called after the final reference to the instance has been released
	void FinalRelease();

  /// sets the block volume instance
  void SetVolume( vxBlockVolume< VoxelType > & volume );

// member variables
private:

 /// dimensions
 CComPtr< IVolumeDimensions > m_spDimensions;

 /// units
 CComPtr< IVolumeUnits > m_spUnits;

 /// internal block volume
 vxBlockVolume< VoxelType > m_volume;
  
}; // coclass Volume 

// $Log: Volume.h,v $
// Revision 1.4  2007/03/07 16:03:16  gdavidson
// Code review
//
// Revision 1.3  2006/06/02 13:00:53  geconomos
// corrected compilation error porting to 2005
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Volume.h,v 1.4 2007/03/07 16:03:16 gdavidson Exp $
// $Id: Volume.h,v 1.4 2007/03/07 16:03:16 gdavidson Exp $
