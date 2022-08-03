// $Id: FindingCollection.h,v 1.5 2007/03/15 13:11:42 geconomos Exp $
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
#include "vxCadFindings.h"

// class definition
class ATL_NO_VTABLE FindingCollection : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< FindingCollection >,
  public IDispatchImpl< IFindingCollection, &__uuidof( IFindingCollection ) >
{
/// ATL macros and defines
public:
	
// protects object from being deleted if the reference count decrements the count to 0 during construction
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( FindingCollection )

// interface map
BEGIN_COM_MAP( FindingCollection )
  COM_INTERFACE_ENTRY( IFindingCollection )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IVoxelPosition implementation
public:

  /// gets the number of findings in the collection
  STDMETHOD( get_Count )( int * piCount );

  /// gets the cad vendor
  STDMETHOD( get_Vendor )( BSTR * psVendor );

  /// sets the cad vendor
  STDMETHOD( put_Vendor )( BSTR sVendor );

  /// gets the cad vendor version
  STDMETHOD( get_Version )( BSTR * psVendor );

  /// sets the cad vendor version
  STDMETHOD( put_Version )( BSTR sVersion );

  /// gets the vendor specific data
  STDMETHOD( get_VendorSpecificData )( IDictionary ** ppiVolumeROI );

  /// adds a finding to the collection
  STDMETHOD( Add )( IFinding * piFinding );

  /// gets the finding at the specified index
  STDMETHOD( Get )( int iIndex, IFinding ** ppiFinding);

  /// clears out the finding
  STDMETHOD( Clear )();

  /// removes the finding at the specified index
  STDMETHOD( Remove )( int iIndex );

  /// creates a new Finding object and returns it's IFinding interface pointer
  STDMETHOD( CreateFinding )( IFinding ** ppiFinding  );

  /// writes out the findings in xml to the specified file
  STDMETHOD( ToFile )( BSTR sFilename );

  // member functions
public:
  
  /// default constructor
  FindingCollection();
  
  /// called after the object is fully instantiated
  HRESULT FinalConstruct();

  /// called after the final reference to the instance has been released
	void FinalRelease();

  /// gets the internal vector
  const vxCAD::vxCadFindings * GetInternalPtr() const { return &m_findings; }

// member variables
private:

  /// cad findings
  vxCAD::vxCadFindings  m_findings;

  // vendor specific data
  CComPtr< IDictionary > m_spVendorSpecificData;

}; // coclass Vector

// $Log: FindingCollection.h,v $
// Revision 1.5  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.2.1  2006/01/09 20:01:26  geconomos
// added vendor specific data to findings collection
//
// Revision 1.3  2005/10/11 13:23:11  geconomos
// added api version property
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingCollection.h,v 1.5 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingCollection.h,v 1.5 2007/03/15 13:11:42 geconomos Exp $
