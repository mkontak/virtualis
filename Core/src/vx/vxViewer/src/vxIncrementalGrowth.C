// $Id: vxIncrementalGrowth.C,v 1.2 2007/03/07 18:06:10 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxIncrementalGrowth.h"
#include "vxIncrementalHeap.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
vxIncrementalGrowth::vxIncrementalGrowth()
: m_pDataVolume( NULL ),
m_pDataIter( NULL ),
m_pLockMaskIter( NULL ),
m_pComponentVolume( NULL ),
m_pComponentIter( NULL ),
m_pProcessedMaskVolume( NULL ),
m_pProcessedMaskIter( NULL )
{
} // vxIncrementalGrowth()


/**
 * Initialize all the data
 *
 * @param seedPosition  the 3D voxel position of the seedpoint
 * @param pDataVolume   the data volume
 * @param pDataIter     iterator for the data volume
 * @param pLockMaskIter iterator for the lock volume
 */
void vxIncrementalGrowth::Initialize( const Point3Di & seedPosition, vxBlockVolume<uint2> * pDataVolume,
                                      vxBlockVolumeIterator<uint2> * pDataIter,
                                      vxBlockVolumeIterator<bool> * pLockMaskIter )
{
  // set some fields
  m_pDataVolume = pDataVolume;
  m_pDataIter = pDataIter;
  m_pLockMaskIter = pLockMaskIter;
  m_seedPosition = seedPosition;

  // create the volume that selects the region voxels
  m_pComponentVolume = new vxBlockVolume< bool >( pDataVolume->GetHeader() );
  m_pComponentIter = new vxBlockVolumeIterator< bool >( * m_pComponentVolume );

  // mask indicates if the voxel has been added to the list already
  m_pProcessedMaskVolume = new vxBlockVolume< bool >( pDataVolume->GetHeader() );
  m_pProcessedMaskIter = new vxBlockVolumeIterator< bool >( * m_pProcessedMaskVolume );

  InitiateProcessing( seedPosition );

} // vxIncrementalGrowth()


/**
 * Start processing the first voxel
 *
 * @param seedPosition the 3D voxel position of the seedpoint
 */
void vxIncrementalGrowth::InitiateProcessing( const Point3Di & seedPosition )
{

  Clear();

  m_pProcessedMaskIter->SetPos( Vector3Di( seedPosition ) );
  m_pProcessedMaskIter->SetVoxel( true );

  ProcessVoxel( seedPosition );

} // InitiateProcessing()


/**
 * Process a single voxel from the top of the heap
 *
 * @param position the 3D voxel position
 */
void vxIncrementalGrowth::ProcessVoxel( const Point3Di & position )
{
  // check if it is locked by another component
  m_pLockMaskIter->SetPos( Vector3Di( position ) );
  bool bLocked( m_pLockMaskIter->GetVoxel() );

  // set the voxel in the component volume
  m_pComponentIter->SetPos( Vector3Di( position ) );

  // update the number of voxels processed
  ++m_uNumVoxelsProcessed;

  // check if it is locked by another component
  if ( bLocked == false )
  {
    // set the voxel in the volume
    m_pComponentIter->SetVoxel( true );

    // save the voxels in a list
    m_componentVoxels.push_back( position );
    m_voxelsProcessed.push_back( m_uNumVoxelsProcessed );

    // add neighbors
    PushUnprocessedBoundaryVoxels( position );

  } // if voxel is unlocked by another component

} // ProcessVoxel()


/**
 * Clear out the processed mask volume
 */
void vxIncrementalGrowth::Clear()
{
  m_newHeap.Clear();
  m_pComponentVolume->Clear();
  m_pProcessedMaskVolume->Clear();
  m_voxelsProcessed.clear();
  m_uNumVoxelsProcessed = 0;
  m_componentVoxels.clear();
  m_incrementalSlider.Clear();
} // ClearMask()


/**
 * Decide if we should even look at this voxel.  Currently looks for non-zero voxels.
 * So, don't try to grow out air.
 *
 * @param position the 3D position
 *
 * @return true if part of region
 */
bool vxIncrementalGrowth::IsPartOfRegion( const Point3Di & position )
{
  m_pDataIter->SetPos( Vector3Di( position ) );
  const int2 iValue( m_pDataIter->GetVoxel() );
  return iValue > 0;
} // IsPartOfRegion()


/**
 * Add all the neighbor voxels to the heap
 *
 * @param position the 3D position
 */
void vxIncrementalGrowth::PushUnprocessedBoundaryVoxels( const Point3Di & position )
{

  // loop over all neighbors
  vxBlockVolumeIteratorNeighbors< uint2 > neighbors( * m_pDataVolume, Vector3Di( position ), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26 );
  while ( ! neighbors.IsAtEnd() )
  {

    // make sure we are inside the volume
    if ( neighbors.IsInsideVolume() )
    {

      // make sure we don't process a voxel twice!
      m_pProcessedMaskIter->SetPos( Vector3Di( neighbors.GetPos() ) );
      if ( m_pProcessedMaskIter->GetVoxel() == false )
      {
        m_pProcessedMaskIter->SetVoxel( true );

        // make sure it's part of the valid region
        if ( IsPartOfRegion( Point3Di( neighbors.GetPos() ) ) )
        {

          // push this neighbor onto the stack
          m_newHeap.Push( vxIncrementalHeap::vxHeapListElement( Point3Di( neighbors.GetPos() ) ) );

        } // if part of valid region

      } // if unprocessed

    } // if inside volume

    // go to the next neighbor
    ++neighbors;
  }
} // PushUnprocessedBoundaryVoxels()


/**
 * Choose one voxel from the top of the heap and process it
 */
void vxIncrementalGrowth::Cycle()
{
  if ( m_newHeap.IsEmpty() )
    return;

  ProcessVoxel( m_newHeap.Top() );
  m_newHeap.Pop();
} // Cycle()


// undefines
#undef FILE_REVISION


// $Log: vxIncrementalGrowth.C,v $
// Revision 1.2  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 14:58:37  vxconfig
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
// Revision 1.6  2004/03/02 01:10:33  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/25 14:57:55  frank
// formatting
//
// Revision 1.5  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.4  2003/10/28 15:44:15  frank
// Improved the Easy Select speed by 35%
//
// Revision 1.3  2003/10/27 20:09:53  frank
// Removed debug warning
//
// Revision 1.2  2003/06/18 15:19:44  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.11  2003/04/30 16:38:49  frank
// Fixed file revision define
//
// Revision 1.10  2003/04/28 18:20:17  frank
// code review
//
// Revision 1.9  2003/04/23 14:42:53  frank
// Stored the number of voxels processed so that we can be sure about how many times to try to find voxels in the second pass of incremental growth.
//
// Revision 1.8  2003/04/18 17:50:02  frank
// Split out vxIncrementalGrowthImage to its own file
//
// Revision 1.7  2003/04/18 16:00:20  frank
// Moved vxIncrementalSlider to its own file.
//
// Revision 1.6  2003/04/07 18:09:13  frank
// Refactored code to improve reusability and maintainability.
//
// Revision 1.5  2003/04/07 14:26:55  frank
// Modified the visualization
//
// Revision 1.4  2003/04/07 14:07:28  frank
// Fixed algorithm for choosing number of points to display
//
// Revision 1.3  2003/04/07 13:18:24  frank
// Made it only segment the number of voxels desired according to the slider.
//
// Revision 1.2  2003/04/04 21:27:45  frank
// Made the number of voxels in the region interactive
//
// Revision 1.1  2003/04/04 15:55:20  frank
// Cleaned up class and file names and respected the component lock flag.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxIncrementalGrowth.C,v 1.2 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxIncrementalGrowth.C,v 1.2 2007/03/07 18:06:10 geconomos Exp $
