// $Id: VoxelPositionCollection.C,v 1.2 2007/03/15 13:11:43 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "VoxelPositionCollection.h"
#include "VoxelPosition.h"
#include "vxCadROI.h"

using namespace vxCAD;

/**
 * Default Constructor
 */
VoxelPositionCollection::VoxelPositionCollection() : m_pROI ( NULL )
{
} // VoxelPosition()


/**
 * Gets the number of voxel positions in the collection component
 *
 * @param     piCount   contains the count upon return
 * @return    S_OK for success
 */
STDMETHODIMP VoxelPositionCollection::get_Count( int * piCount )
{
	if( !piCount )
    return E_INVALIDARG;
  *piCount = m_pROI->GetCount();
  return S_OK;
} // get_Count( int * piCount )


/**
 * Adds the voxel position to the collection
 *
 * @param     x   x position
 * @param     y   y position
 * @param     z   z position
 * @return    S_OK for success
 */
STDMETHODIMP VoxelPositionCollection::Add(  int x, int y, int z   )
{
  vxCAD::vxCadVoxelPosition position( x, y, z );
  m_pROI->AddVoxel( position ); 
  return S_OK;
} // Add(  IVoxelPosition * piPosition  )


/**
 * Adds the voxel position to the collection
 *
 * @param     iIndex         index of voxel position to return
 * @param     ppiPosition    upon return contains the voxel position
 * @return    S_OK for success
 */
STDMETHODIMP VoxelPositionCollection::Get( int iIndex, IVoxelPosition ** ppiVoxelPosition )
{
	if( iIndex < 0 || iIndex >= m_pROI->GetCount() )
    return E_INVALIDARG;

  if( !ppiVoxelPosition )
    return E_INVALIDARG;
  *ppiVoxelPosition = NULL;

  CComPtr< IVoxelPosition > spPosition;
  CComObject< VoxelPosition >().QueryInterface( __uuidof( IVoxelPosition ), reinterpret_cast< void ** >( &spPosition ) );
  GetCoClassPtr< VoxelPosition >( spPosition )->SetInternalPtr( const_cast< vxCAD::vxCadVoxelPosition * >( & m_pROI->GetVoxel( iIndex ) ) );
  
  return spPosition.QueryInterface( ppiVoxelPosition );
} // Get( int iIndex, IVoxelPosition ** ppiVoxelPosition );


/**
 * Removes the voxel position at the specified index
 *
 * @param     iIndex         index of voxel position to remove
 * @return    S_OK for success
 */
STDMETHODIMP VoxelPositionCollection::Remove( int iIndex  )
{
  return E_NOTIMPL;
} // Remove( int iIndex  );


/**
 * Clears out the collection
 *
 * @return    S_OK for success
 */
STDMETHODIMP VoxelPositionCollection::Clear()
{
  return E_NOTIMPL;
} // Clear()

// $Log: VoxelPositionCollection.C,v $
// Revision 1.2  2007/03/15 13:11:43  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/VoxelPositionCollection.C,v 1.2 2007/03/15 13:11:43 geconomos Exp $
// $Id: VoxelPositionCollection.C,v 1.2 2007/03/15 13:11:43 geconomos Exp $
