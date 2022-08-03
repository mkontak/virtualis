// $Id: vxBlockVolumeFilterMedian.h,v 1.2 2005/05/12 13:55:03 frank Exp $
//
// Copyright � 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Rekha {ingmar|rekha}@viatronix.com

#ifndef vxBlockVolumeFilterMedian_h
#define vxBlockVolumeFilterMedian_h


// include declarations
#include "vxBlockVolumeFilter.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


/**
 * Median filter
 */
template < class VoxelType >
class VX_BLOCK_VOLUME_DLL vxBlockVolumeFilterMedian : public vxBlockVolumeFilter<VoxelType>
{
// member functions
public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor & assignment

  // standard constructor
  vxBlockVolumeFilterMedian( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors07 );

  // Copy constructor
  vxBlockVolumeFilterMedian( vxBlockVolumeFilterMedian<VoxelType> & other );


  /////////////////////////////////////////////////////////////////////////////
  // Run the actual filter

  /// filters the source volume and writes the result into the desination volume
  void ApplyFilter();
  
  /// selects the median value of the elementas in the vector
  static inline VoxelType SelectMedianValue( std::vector<VoxelType> & vVoxels );


// private data members
private:

  /// the neighborhood of voxels to compute the median value from 
  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum m_eNeighborhood;

}; // class vxBlockVolumeFilterMedian


#include "vxBlockVolumeFilterMedian.inl"


#endif // vxBlockVolumeFilterMedian_h


// $Log: vxBlockVolumeFilterMedian.h,v $
// Revision 1.2  2005/05/12 13:55:03  frank
// added more unit and performance tests
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/22 14:37:34  frank
// code review
//
// Revision 1.9  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.8  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.7  2002/05/20 15:49:29  soeren
// removed redundant arguments from SelectMedianValue()
// made SelectMedianValue() static and public, so it can be called on its own
//
// Revision 1.6  2002/05/14 23:02:29  ingmar
// added & to function parameter
//
// Revision 1.5  2002/05/14 21:53:22  ingmar
// made e3Dneighbors07 the default neighborhood
//
// Revision 1.4  2002/05/13 20:13:20  ingmar
// renamed FindMedianValue() to SelectMedianValue()
//
// Revision 1.3  2002/05/07 18:05:17  rekha
// added FindMedianValue() method
//
// Revision 1.2  2002/04/25 17:59:07  ingmar
// skeleton for median filter using neighborhood iterators
//
// Revision 1.1  2002/04/25 17:22:11  ingmar
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterMedian.h,v 1.2 2005/05/12 13:55:03 frank Exp $
// $Id: vxBlockVolumeFilterMedian.h,v 1.2 2005/05/12 13:55:03 frank Exp $