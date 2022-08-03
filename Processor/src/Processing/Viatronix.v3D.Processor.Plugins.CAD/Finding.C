// $Id: Finding.C,v 1.5 2007/03/15 13:11:42 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "Finding.h"
#include "FindingROI.h"
#include "FindingDimensions.h"
#include "Vector.h"
#include "Dictionary.h"
#include "vxCadFinding.h"

using namespace vxCAD;


/**
 * Default Constructor
 */
Finding::Finding()
{
} // Finding()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT Finding::FinalConstruct()
{
  // dimensions
  CComCreator< CComObject< FindingDimensions > >::CreateInstance( NULL, __uuidof( IFindingDimensions ), reinterpret_cast< void ** >( &m_spDimensions ) );
  GetCoClassPtr< FindingDimensions >( m_spDimensions )->SetInternalPtr( const_cast< vxCadDimensions * >( &m_finding.GetDimensions()) );

  // centroid
  CComCreator< CComObject< Vector > >::CreateInstance( NULL, __uuidof( IVector ), reinterpret_cast< void ** >( &m_spCentroid ) );
  GetCoClassPtr< Vector >( m_spCentroid )->SetInternalPtr( const_cast< vxCadVector * >( &m_finding.GetCentroidMM() ) );
  
  // normal
  CComCreator< CComObject< Vector > >::CreateInstance( NULL, __uuidof( IVector ), reinterpret_cast< void ** >( &m_spNormal ) );
  GetCoClassPtr< Vector >( m_spNormal )->SetInternalPtr( const_cast< vxCadVector * >( &m_finding.GetNormalMM() ) );

  // surface roi
  CComCreator< CComObject< FindingROI > >::CreateInstance( NULL, __uuidof( IFindingROI ), reinterpret_cast< void ** >( &m_spSurfaceROI ) );
  GetCoClassPtr< FindingROI >( m_spSurfaceROI )->SetInternalPtr( const_cast< vxCadROI * >( &m_finding.GetSurfaceROI() ) );
  
  // volume roi
  CComCreator< CComObject< FindingROI > >::CreateInstance( NULL, __uuidof( IFindingROI ), reinterpret_cast< void ** >( &m_spVolumeROI ) );
  GetCoClassPtr< FindingROI >( m_spVolumeROI )->SetInternalPtr( const_cast< vxCadROI * >( &m_finding.GetVolumeROI() ) );

  // vendor specific data
  CComCreator< CComObject< Dictionary > >::CreateInstance( NULL, __uuidof( IDictionary ), reinterpret_cast< void ** >( &m_spVendorSpecificData ) );
  GetCoClassPtr< Dictionary >( m_spVendorSpecificData )->SetInternalPtr( const_cast< Dictionary::DictionaryMap * >( &m_finding.GetVendorSpecificVariablesMap() ) );

  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void Finding::FinalRelease()
{
  m_spDimensions = NULL;
  m_spCentroid = NULL;
  m_spNormal = NULL;
  m_spSurfaceROI = NULL;
  m_spVolumeROI = NULL;
} // FinalRelease()


/**
 * Gets the id
 *
 * @param   psId  upon return contains the id string
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_Id ( BSTR * psId )
{
  if( !psId )
    return E_INVALIDARG;
  *psId = NULL;
  return CComBSTR().CopyTo( psId );
} // get_Id ( BSTR * psId )


/**
 * Gets the segment number
 *
 * @param   piSegment  upon return contains the segment number
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_Segment ( unsigned char * piSegment )
{
  if( !piSegment )
    return E_INVALIDARG;
  *piSegment = m_finding.GetSegment();
  return S_OK;
} // get_Segment ( unsigned char * piSegment )


/**
 * Sets the segment number
 *
 * @param   iSegment  segment number
 * @return  S_OK for success
 */
STDMETHODIMP Finding::put_Segment ( unsigned char iSegment )
{
  m_finding.SetSegment( iSegment );
  return S_OK;
} // put_Segment ( unsigned char iSegment )


/**
 * Gets the dimensions
 *
 * @param   ppiDimensions   upon return contains an IFindindDimensions interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_Dimensions( IFindingDimensions ** ppiDimensions )
{
  if( !ppiDimensions )
    return E_INVALIDARG;
  *ppiDimensions = NULL;
  return m_spDimensions.QueryInterface( ppiDimensions );
} // get_Dimensions( IFindingDimensions ** ppiDimensions )


/**
 * Gets the centroid
 *
 * @param   ppiCentroid   upon return contains an IVector interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_Centroid( IVector ** ppiCentroid )
{
  if( !ppiCentroid )
    return E_INVALIDARG;
  *ppiCentroid = NULL;
  return m_spCentroid.QueryInterface( ppiCentroid );
} // get_Centroid( IVector ** ppiCentroid )


/**
 * Gets the normal
 *
 * @param   ppiNormal   upon return contains an IVector interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_Normal( IVector ** ppiNormal )
{
  if( !ppiNormal )
    return E_INVALIDARG;
  *ppiNormal = NULL;
  return m_spNormal.QueryInterface( ppiNormal );
} // get_Normal( IVector ** ppiNormal )


/**
 * Gets the surface ROI
 *
 * @param   ppiSurfaceROI   upon return contains an IFindingROI interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_SurfaceROI( IFindingROI ** ppiSurfaceROI )
{
  if( !ppiSurfaceROI )
    return E_INVALIDARG;
  *ppiSurfaceROI = NULL;
  return m_spSurfaceROI.QueryInterface( ppiSurfaceROI );
} // get_SurfaceROI( IFindingROI ** ppiSurfaceROI )


/**
 * Gets the volume ROI
 *
 * @param   ppiVolumneROI   upon return contains an IFindingROI interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_VolumeROI( IFindingROI ** ppiVolumeROI )
{
  if( !ppiVolumeROI )
    return E_INVALIDARG;
  *ppiVolumeROI = NULL;
  return m_spVolumeROI.QueryInterface( ppiVolumeROI );
} // get_VolumeROI( IFindingROI ** ppiVolumeROI )


/**
 * Gets the vendor specific data
 *
 * @param   ppiVendorSpecificData   upon return contains an IDictionary interface pointer
 * @return  S_OK for success
 */
STDMETHODIMP Finding::get_VendorSpecificData( IDictionary ** ppiVendorSpecificData )
{
  if( !ppiVendorSpecificData )
    return E_INVALIDARG;
  *ppiVendorSpecificData= NULL;
  return m_spVendorSpecificData.QueryInterface( ppiVendorSpecificData );
} // get_VendorSpecificData( IDictionary ** ppiVendorSpecificData )


// $Log: Finding.C,v $
// Revision 1.5  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.4  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.3  2007/02/07 20:00:27  dongqing
// fix the bug on volumeROI add voxel
//
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.2  2006/01/09 20:01:05  geconomos
// corrected comments
//
// Revision 1.1.2.1  2005/12/08 17:00:45  geconomos
// added VendorSpecificData property
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Finding.C,v 1.5 2007/03/15 13:11:42 geconomos Exp $
// $Id: Finding.C,v 1.5 2007/03/15 13:11:42 geconomos Exp $
