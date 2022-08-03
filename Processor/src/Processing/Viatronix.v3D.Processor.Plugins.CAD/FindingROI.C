// $Id: FindingROI.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "FindingROI.h"
#include "VoxelPositionCollection.h"
#include "vxCadROI.h"

using namespace vxCAD;

/**
 * Default Constructor
 */
FindingROI::FindingROI() : m_pROI ( NULL )
{
} // FindingROI()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT FindingROI::FinalConstruct()
{
  CComCreator< CComObject< VoxelPositionCollection > >::CreateInstance( NULL, __uuidof( IVoxelPositionCollection ), reinterpret_cast< void ** >( &m_spVoxels ) );
  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void FindingROI::FinalRelease()
{ 
  m_spVoxels = NULL;
} // FinalRelease()

/**
 * Sets the internal vxCadROI pointer ( class does NOT assume ownership )
 *
 * @param   pROI    vxCadROI pointer
 */
void FindingROI::SetInternalPtr( vxCadROI * pROI )
{
  m_pROI = pROI;
  GetCoClassPtr< VoxelPositionCollection >( m_spVoxels )->SetInternalPtr( const_cast< vxCadROI * >( m_pROI ) );
} // SetROI( vxCadROI * pROI )


/**
 * Gets the voxel position
 *
 * @param   ppiVoxels   upon return contains an IVoxelPositionCollection interface pointer
 * @return              S_OK for success
 */
STDMETHODIMP FindingROI::get_Voxels( IVoxelPositionCollection ** ppiVoxels  )
{
	if( !ppiVoxels )
    return E_INVALIDARG;
  *ppiVoxels = NULL;
  return m_spVoxels.QueryInterface( ppiVoxels );
} // get_Voxels()


/**
 * Gets the average
 *
 * @param     pfAverage   upon return contains the average
 * @return    S_OK for success
 */
STDMETHODIMP FindingROI::get_AverageHU( float * pfAverage )
{
	if( !pfAverage )
    return E_INVALIDARG;
  *pfAverage = m_pROI->GetAverageHU();
  return S_OK;
} // get_AverageHU()


/**
 * Sets the average
 *
 * @param     fAverageHU    average HU
 * @return    S_OK for success
 */
STDMETHODIMP FindingROI::put_AverageHU( float  fAverage )
{
  m_pROI->SetAverageHU( fAverage );
  return S_OK;
} // put_AverageHU()


/**
 * Gets the standard deviation
 *
 * @param     pfStandardDeviation       upon return contains the standard deviation
 * @return    S_OK for success
 */
STDMETHODIMP FindingROI::get_StandardDeviationHU( float * pfStandardDeviation )
{
	if( !pfStandardDeviation )
    return E_INVALIDARG;
  *pfStandardDeviation = m_pROI->GetStandardDeviationHU();
  return S_OK;
} // get_AverageHU()


/**
 * Sets the standard deviation
 *
 * @param     fStandardDeviation      standard deviation
 * @return    S_OK for success
 */
STDMETHODIMP FindingROI::put_StandardDeviationHU( float  fStandardDeviation )
{
  m_pROI->SetStandardDeviationHU( fStandardDeviation );
  return S_OK;
} // put_StandardDeviationHU()


// $Log: FindingROI.C,v $
// Revision 1.2  2007/03/15 13:11:42  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/FindingROI.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
// $Id: FindingROI.C,v 1.2 2007/03/15 13:11:42 geconomos Exp $
