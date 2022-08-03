// $Id: vxIncrementalHeap.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxIncrementalHeap.h"
#include "vxIntensityStatistics.h"


// Defines
#define FILE_REVISION "$Revision: 1.1 $"


// static members
vxIncrementalHeap::vxIncrementalSimilarity vxIncrementalHeap::m_incrementalSimilarity;


/**
 * Constructor
 */
vxIncrementalHeap::vxIncrementalSimilarity::vxIncrementalSimilarity()
: m_pIntensityIter( NULL ),
m_pVesselnessIter( NULL ),
m_iInitialIntensity( 0 ),
m_fInitialStandardDeviation( 0.0F )
{
} // vxIncrementalSimilarity()


/// Constructor
vxIncrementalHeap::vxIncrementalHeap()
{
  m_heapList.reserve( 1024 );
  m_uTopList = 0;
} // vxIncrementalHeap()


/**
 * Clear the heap
 */
void vxIncrementalHeap::Clear()
{
  m_heapList.resize( 0 );
  m_heapList.reserve( 1024 );
  m_uTopList = 0;
} // Clear()


/**
 * Push an element onto the heap
 *
 * @param element the element to push on the heap
 */
void vxIncrementalHeap::Push( vxHeapListElement & element )
{
  uint4 uSimilarity( GetSimilarityMeasure( element ) );
  if ( m_heapList.size() < uSimilarity + 1 )
  {
    m_heapList.resize( uSimilarity + 1 );
  }

  vxHeapList & list = m_heapList[ uSimilarity ];
  m_heapList[ uSimilarity ].push_back( element );

  if ( uSimilarity < m_uTopList )
  {
    m_uTopList = uSimilarity;
  }
} // Push()


/**
 * If the heap is empty
 *
 * @return if the heap is empty
 */
bool vxIncrementalHeap::IsEmpty() const
{
  return m_uTopList == m_heapList.size();
} // IsEmpty()


/**
 * Pop an element off the top of the heap - only call if you are sure the heap is not empty.
 */
void vxIncrementalHeap::Pop()
{
  m_heapList[ m_uTopList ].pop_front();
  FindNext();
} // Pop()


/**
 * Find the next most likely voxel in the heap
 */
void vxIncrementalHeap::FindNext()
{
  while ( m_heapList[ m_uTopList ].empty() && m_uTopList < m_heapList.size() )
    ++m_uTopList;
} // FindNext()


/**
 * Get a reference to the top heap element - only call if you are sure the heap is not empty.
 *
 * @return the top heap element
 */
const vxIncrementalHeap::vxHeapListElement & vxIncrementalHeap::Top() const
{
  return m_heapList[ m_uTopList ].front();
} // Top()


/**
 * Determine the similarity of a voxel to the desired voxel
 *
 * @param element the element currently under comparison
 *
 * @return zero is very similar, higher numbers are less similar
 */
uint4 vxIncrementalHeap::GetSimilarityMeasure( vxHeapListElement & element )
{

  // check if growing intensity volume
  if ( m_incrementalSimilarity.IsGrowingInIntensity() )
  {

    const bool bUseStandardDeviation( false );

    if ( bUseStandardDeviation )
    {

      static int4 iCounter(0);

      // compare the initial intensity to this intensity
      m_incrementalSimilarity.GetIntensityIterator()->SetPos( Vector3Di( element.GetPosition() ) );
      int4 iIntensityDistanceThis( m_incrementalSimilarity.GetInitialIntensity() -
                                   m_incrementalSimilarity.GetIntensityIterator()->GetVoxel() );
      if ( iIntensityDistanceThis < 0 ) iIntensityDistanceThis = -iIntensityDistanceThis;

      // compare the initial standard deviation to this standard deviation
      float4 fSDDistanceThis( m_incrementalSimilarity.GetInitialStandardDeviation() -
                              GetStandardDeviation( * m_incrementalSimilarity.GetIntensityIterator() ) );
      if ( fSDDistanceThis < 0 ) fSDDistanceThis = -fSDDistanceThis;

      // combine the two measures
      int4 iCombinedDistanceMeasure( iIntensityDistanceThis + 2 * fSDDistanceThis );

      if ( iCounter++ % 1000 == 0 )
      {
        LogDbg( "inten: " + ToAscii( iIntensityDistanceThis ) + "  sd: " + ToAscii( fSDDistanceThis ) + "  combined: " + ToAscii( iCombinedDistanceMeasure ), "", "" );
      }
      
      return iCombinedDistanceMeasure;

    }
    else
    {

      // compare the initial intensity to this intensity
      m_incrementalSimilarity.GetIntensityIterator()->SetPos( Vector3Di( element.GetPosition() ) );
      int4 iDistanceThis( m_incrementalSimilarity.GetInitialIntensity() -
        m_incrementalSimilarity.GetIntensityIterator()->GetVoxel() );
      if ( iDistanceThis < 0 ) iDistanceThis = -iDistanceThis;

      return iDistanceThis;

    }

  }

  // check if growing in vesselness volume
  else if ( m_incrementalSimilarity.IsGrowingInVesselness() )
  {

    // get vesselness of this
    m_incrementalSimilarity.GetVesselnessIterator()->SetPos( Vector3Di( element.GetPosition() ) );
    int4 iValueThis( ( 2000 - m_incrementalSimilarity.GetVesselnessIterator()->GetVoxel() ) / 2 );
    if ( iValueThis < 0 ) iValueThis = -iValueThis;

    // choose the voxel with the greatest vesselness
    return iValueThis;

  }
  else
  {

    throw ex::UnsupportedDataTypeException( LogRec( "Bad incremental growth data", "vxIncrementalHeap", "GetSimilarityMeasure" ) );

  }

  return false;

} // GetSimilarityMeasure()


/**
 * Set up for growing in an intensity volume
 *
 * @param pIntensityIter    the iterator for the intensity volume
 * @param pInitialIntensity the initial intensity at the seedpoint
 */
void vxIncrementalHeap::vxIncrementalSimilarity::SetGrowingInIntensity( vxBlockVolumeIterator< uint2 > * pIntensityIter,
                                                                        const int4 iInitialIntensity,
                                                                        const float4 fInitialStandardDeviation )
{

  m_pIntensityIter = pIntensityIter;
  m_pVesselnessIter = NULL;
  m_iInitialIntensity = iInitialIntensity;
  m_fInitialStandardDeviation = fInitialStandardDeviation;

} // SetGrowingInIntensity()


/**
 * Set up for growing in a vesselness volume
 *
 * @param pVesselnessIter the iterator for the vesselness volume
 */
void vxIncrementalHeap::vxIncrementalSimilarity::SetGrowingInVesselness( vxBlockVolumeIterator< uint2 > * pVesselnessIter )
{

  m_pIntensityIter = NULL;
  m_pVesselnessIter = pVesselnessIter;

} // SetGrowingInVesselness()


/**
 * helper function to get the standard deviation in a standard way for computing similarity
 *
 * @param volIter the volume iterator
 *
 * @return standard deviation
 */
float4 vxIncrementalHeap::GetStandardDeviation( vxBlockVolumeIterator<uint2> & volIter )
{

  vxIntensityStatistics stats;

  const int4 iRadius( 4 );
  for ( int z(-iRadius); z<=iRadius; ++z )
    for ( int y(-iRadius); y<=iRadius; ++y )
      for ( int x(-iRadius); x<=iRadius; ++x )
        stats.Add( volIter.PeekRel( Vector3Di( x, y, z ) ) );

  stats.Compute();
  return stats.GetStandardDeviation();

} // GetStandardDeviation()


// undefines
#undef FILE_REVISION


// $Log: vxIncrementalHeap.C,v $
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 14:59:02  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2004/03/02 01:10:33  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.2.2.1  2004/02/25 14:58:06  frank
// formatting
//
// Revision 1.2  2004/01/09 15:41:03  frank
// code review
//
// Revision 1.1  2003/10/28 15:43:44  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxIncrementalHeap.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
// $Id: vxIncrementalHeap.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
