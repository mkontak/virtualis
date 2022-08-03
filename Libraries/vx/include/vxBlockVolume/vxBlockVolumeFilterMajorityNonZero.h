// $Id: vxBlockVolumeFilterMajorityNonZero.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Rekha {ingmar|rekha}@viatronix.com
//
// Complete history at bottom of file.

#ifndef vxBlockVolumeFilterMajorityNonZero_h
#define vxBlockVolumeFilterMajorityNonZero_h



// include declarations
#include "vxBlockVolumeFilter.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"

/**
 * Block volume server. Can make and return volumes of any resolution.
 */
template < class VoxelType >
class vxBlockVolumeFilterMajorityNonZero : public vxBlockVolumeFilter<VoxelType>
{
  // public member functions
public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor & assignment

  // standard constructor
  vxBlockVolumeFilterMajorityNonZero(vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors07);

  // Copy constructor
  vxBlockVolumeFilterMajorityNonZero(vxBlockVolumeFilterMajorityNonZero<VoxelType> & src);


  /////////////////////////////////////////////////////////////////////////////
  // Set src and dest Volumes

  // derived from vxBlockVolumeFilter, no local change


  /////////////////////////////////////////////////////////////////////////////
  // Run the actual filter

  /// filters the source volume and writes the result into the desination volume
  void ApplyFilter( );
  
  /// selects the majority element of the vector
  static inline VoxelType SelectMajorityValueNonZero(std::vector<VoxelType> & vVoxels);
  
// private (locked) member functions
private:

// private data members
private:

  /// the neighborhood of voxels to compute the median value from 
  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum m_eNeighborhood;

}; // class vxBlockVolumeFilterMajorityNonZero


#include "vxBlockVolumeFilterMajorityNonZero.inl"


#endif // vxBlockVolumeFilterMajorityNonZero_h


// $Log: vxBlockVolumeFilterMajorityNonZero.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.1  2002/09/27 16:41:45  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterMajorityNonZero.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterMajorityNonZero.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $