// $Id: vxIncrementalGrowth.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <list>
#include <vector>
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeRegionGrowIteratorHeap.h"
#include "vxDecider.h"
#include "Image.h"
#include "vxElement.h"
#include "vxIncrementalSlider.h"
#include "vxIncrementalHeap.h"


/**
 * A class used to support the incremental growth tool.
 */
class VX_VIEWER_DLL vxIncrementalGrowth : public vxElement
{
// Member Functions
public:

  /// Constructor
  vxIncrementalGrowth();

  /// Initialize all the data
  void Initialize( const Point3Di & seedPosition, vxBlockVolume<uint2> * pDataVolume,
                   vxBlockVolumeIterator<uint2> * pDataIter, vxBlockVolumeIterator<bool> * pLockMaskIter );

  /// Choose one voxel from the top of the heap and process it
  void Cycle();

  /// If we are done
  bool IsDone() const;

  /// Get the region tag volume
  vxBlockVolume< bool > * GetRegionTagVolumePtr() { return m_pComponentVolume; }

  /// Get the seed position
  const Point3Di & GetSeedPosition() const { return m_seedPosition; }

  /// Clear out the processed data
  void Clear();

  /// Start processing the first voxel
  void InitiateProcessing( const Point3Di & seedPosition );

  /// Gets the volume of voxels that are part of the component
  vxBlockVolume< bool > * GetComponentVolumePtr() { return m_pComponentVolume; }

  /// Gets a list of voxels that are part of the component in the order discovered
  std::vector< Point3Di > & GetComponentVoxels() { return m_componentVoxels; }

  /// Get the number of voxels processed through the heap so far
  const std::vector< uint4 > & GetVoxelsProcessed() const { return m_voxelsProcessed; }

  /// Get the incremental slider object
  vxIncrementalSlider & GetIncrementalSlider() { return m_incrementalSlider; }

private:

  /// Process a single voxel from the top of the heap
  void ProcessVoxel( const Point3Di & position );

  /// Add all the neighbor voxels to the heap
  void PushUnprocessedBoundaryVoxels( const Point3Di & position );

  /// Decide if we should even look at this voxel
  bool IsPartOfRegion( const Point3Di & position );

// Data Members
private:

  /// The heap of ready-to-process voxels
  vxIncrementalHeap m_newHeap;

  /// The iterator for the data volume
  vxBlockVolumeIterator< uint2 > * m_pDataIter;

  /// The iterator for the component volume
  vxBlockVolumeIterator< bool > * m_pComponentIter;

  /// The iterator for the lock mask volume
  vxBlockVolumeIterator< bool > * m_pLockMaskIter;

  /// The iterator for the processed mask volume
  vxBlockVolumeIterator< bool > * m_pProcessedMaskIter;

  /// The data volume
  vxBlockVolume< uint2 > * m_pDataVolume;

  /// The component volume (true voxels will be added to the component)
  vxBlockVolume< bool > * m_pComponentVolume;

  /// True voxels have already been processed
  vxBlockVolume< bool > * m_pProcessedMaskVolume;

  /// A history of the number of voxels that were processed in order to find the component voxels
  std::vector< uint4 > m_voxelsProcessed;

  /// The number of voxels that were processed in order to find the component voxels
  uint4 m_uNumVoxelsProcessed;

  /// The cached seed position
  Point3Di m_seedPosition;

  /// The component voxels in the order they were found
  std::vector< Point3Di > m_componentVoxels;

  /// The incremental slider object
  vxIncrementalSlider m_incrementalSlider;

}; // class vxIncrementalGrowth


// $Log: vxIncrementalGrowth.h,v $
// Revision 1.2  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:33  geconomos
// moved from vascular
//
// Revision 1.4  2006/09/20 16:57:18  frank
// removed unnecessary includes
//
// Revision 1.3  2006/07/05 17:25:00  romy
// Added derivation from the vxElement
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
// Revision 1.4  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.3  2003/10/28 15:44:15  frank
// Improved the Easy Select speed by 35%
//
// Revision 1.2  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.7  2003/04/28 18:20:17  frank
// code review
//
// Revision 1.6  2003/04/18 17:50:02  frank
// Split out vxIncrementalGrowthImage to its own file
//
// Revision 1.5  2003/04/18 16:00:20  frank
// Moved vxIncrementalSlider to its own file.
//
// Revision 1.4  2003/04/07 18:09:13  frank
// Refactored code to improve reusability and maintainability.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIncrementalGrowth.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxIncrementalGrowth.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
