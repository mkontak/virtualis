// $Id: Volume.C,v 1.10 2006/12/22 14:40:27 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "Volume.h"
#include "VolumeDimensions.h"
#include "VolumeUnits.h"
#include "vxIntensityConverter.h"
#include "Timer.h"

template class Volume< uint1 >;
template class Volume< uint2 >;
template class Volume< int2 >;


#define FILE_REVISION "$Revision: 1.10 $"

/**
 * Default Constructor
 */
template <class VoxelType> 
Volume<VoxelType>::Volume()
{
} // Volume()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
template <class VoxelType> 
HRESULT Volume<VoxelType>::FinalConstruct()
{
  CComCreator< CComObject< VolumeDimensions > >::CreateInstance( NULL, __uuidof( IVolumeDimensions ), reinterpret_cast< void ** >( &m_spDimensions ) );
  CComCreator< CComObject< VolumeUnits > >::CreateInstance( NULL, __uuidof( IVolumeUnits ), reinterpret_cast< void ** >( &m_spUnits ) );
  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
template <class VoxelType> 
void Volume<VoxelType>::FinalRelease()
{
  m_spDimensions = NULL;
  m_spUnits = NULL;
} // FinalRelease()


/**
 * Gets the dimensions
 *
 * @param   ppiDimensions   contains upon return an IVolumeDimensions interface pointer
 * @return  S_OK for success
 */
template <class VoxelType> 
STDMETHODIMP Volume<VoxelType>::get_Dimensions( IVolumeDimensions ** ppiDimensions )
{
	if( !ppiDimensions )
    return E_INVALIDARG;
  *ppiDimensions = NULL;
  return m_spDimensions.QueryInterface( ppiDimensions );
} // get_Dimensions()


/**
 * Gets the units
 *
 * @param   ppiUnits  contains upon return an IVolumeUnits interface pointer
 * @return  S_OK for success
 */
template <class VoxelType> 
STDMETHODIMP Volume<VoxelType>::get_Units( IVolumeUnits ** ppiUnits )
{
	if( !ppiUnits )
    return E_INVALIDARG;
  *ppiUnits = NULL;
  return m_spUnits.QueryInterface( ppiUnits );
} // get_Units()


/**
 * Gets the voxels
 *
 * @param   pVoxels 
 * @return  S_OK for success
 */
template <class VoxelType> 
STDMETHODIMP Volume<VoxelType>::get_Voxels( VARIANT * pVoxels )
{
  try
  {
    // validate and initialize
    if( !pVoxels )
      return E_NOTIMPL;
    VariantClear( pVoxels );

    // calculate size
    const Triple< int4 > & dim = m_volume.GetHeader().GetVolDim();
    uint4 uSize( dim.m_x * dim.m_y * dim.m_z );

    // create a safe array of unsigned shorts
    bool bConvertToModality( false );
    if( sizeof( VoxelType ) == 1 )
    {
      pVoxels->vt = VT_UI1|VT_ARRAY;
      pVoxels->parray = SafeArrayCreateVector( VT_UI1, 0, uSize );
    }
    else
    {
      pVoxels->vt = VT_I2|VT_ARRAY;
      pVoxels->parray = SafeArrayCreateVector( VT_I2, 0, uSize );
      bConvertToModality = true;
    }  
    
    // access the array
    VoxelType * pData( NULL );
    SafeArrayAccessData( pVoxels->parray, reinterpret_cast< void ** >( &pData ) );

    //int4 iMaxVoxel( vxIntensityConverter::GetMaxValue( m_volume.GetHeader(), false ) );
    //int4 iVoxelBias( vxIntensityConverter::GetBias( m_volume.GetHeader(), false ) );
    int4 iVoxelBias1( vxIntensityConverter::GetBias( m_volume.GetHeader(), true ) );

    vxBlockVolumeIterator< VoxelType > volumeIter( m_volume );
    for( ; volumeIter.IsNotAtEnd(); volumeIter.NextZYX(), ++pData )
    {
      if( volumeIter.IsInsideVolume() )
      {
        if( bConvertToModality )
        {
          float4 fVoxel = volumeIter.GetVoxel() - iVoxelBias1;
        
          *pData = static_cast< VoxelType >( fVoxel );
        }
        else
        {
          *pData = volumeIter.GetVoxel();
        }
      }
    } // for( ; volumeIter.IsNotAtEnd(); volumeIter.NextZYX(), ++pData )

    // unaccess
    SafeArrayUnaccessData( pVoxels->parray );
  }
  catch( ex::VException &  )
  {
    return E_FAIL;
  }   
  return S_OK;
} // get_Units()


/**
 * Gets the specified dicom element from the header
 *
 * @param   bsGroup     dicom group
 * @param   bsField     dicom field
 * @param   pValue      upon return contain the value
 * @return  S_OK for success
 */
template <class VoxelType> 
STDMETHODIMP Volume<VoxelType>::GetHdrVar( BSTR bsGroup, BSTR bsField, BSTR * pValue )
{
  try
  {
    if( !pValue )
      return E_INVALIDARG;
    *pValue = NULL;

    std::string sGroup( static_cast< const char * >( _bstr_t( bsGroup ) ) );
    std::string sField( static_cast< const char * >( _bstr_t( bsField ) ) );
    std::string sValue;

    sValue = m_volume.GetHeader().GetDicomFieldValue( sGroup, sField, sValue );

    CComBSTR bsValue( sValue.c_str() );
    bsValue.CopyTo( pValue );
  }
  catch( ex::VException &  )
  {
    return E_FAIL;
  }
  return S_OK;
} // GetHdrVar


/**
 * Sets the block volume instance
 *
 * @param   volume    blovk volume
 */
template <class VoxelType> 
void Volume<VoxelType>::SetVolume( vxBlockVolume< VoxelType > & volume )
{
  m_volume = volume;
  GetCoClassPtr< VolumeDimensions >( m_spDimensions )->GetDimensions() = m_volume.GetHeader().GetVolDim(); 
  GetCoClassPtr< VolumeUnits >( m_spUnits )->GetVolumeUnits() = m_volume.GetHeader().GetVoxelUnits(); 
} // SetVolume( vxBlockVolume< uint2 > & volume )


// undefines
#undef FILE_REVISION


// $Log: Volume.C,v $
// Revision 1.10  2006/12/22 14:40:27  dongqing
// fix the RawToModality HU problem. It still provides HU for CT images
//
// Revision 1.9  2006/06/26 21:11:58  dongqing
// change the input volume voxel type from SHORT to UINT2
//
// Revision 1.8  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.7  2005/11/08 21:19:13  dongqing
// fix the GetUnits() bug
//
// Revision 1.6.2.1  2005/11/08 21:23:20  dongqing
// fix the GetUnits() bug
//
// Revision 1.6  2005/10/12 13:26:06  geconomos
// reworked getting volume voxels
//
// Revision 1.5  2005/10/11 17:17:15  geconomos
// corrected converting from raw voxel units (frank)
//
// Revision 1.4  2005/09/23 12:09:26  geconomos
// corrected get_Voxels
//
// Revision 1.3  2005/09/20 19:49:17  geconomos
// changed voxel type of intensity value to int2
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Volume.C,v 1.10 2006/12/22 14:40:27 dongqing Exp $
// $Id: Volume.C,v 1.10 2006/12/22 14:40:27 dongqing Exp $
