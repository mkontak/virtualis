// $Id: vxBlockVolumeFilterAverage.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar (ingmar@viatronix.com)

#ifndef vxBlockVolumeFilterAverage_h
#define vxBlockVolumeFilterAverage_h


// include declarations
#include "vxBlockVolumeFilter.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


/**
 * Average filter
 */
template < class VoxelType >
class vxBlockVolumeFilterAverage : public vxBlockVolumeFilter<VoxelType>
{
// member functions
public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor & assignment

  // standard constructor
  vxBlockVolumeFilterAverage( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors07 );

  // Copy constructor
  vxBlockVolumeFilterAverage( vxBlockVolumeFilterAverage<VoxelType> & src );


  /////////////////////////////////////////////////////////////////////////////
  // Run the actual filter

  /// filters the source volume and writes the result into the desination volume
  void ApplyFilter();

  /// selects the average of the vector
  static inline VoxelType SelectAverageValue( std::vector<VoxelType> & vVoxels );


// private data members
private:

  /// the neighborhood of voxels to compute the median value from 
  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum m_eNeighborhood;

}; // class vxBlockVolumeFilterAverage


#include "vxBlockVolumeFilterAverage.inl"


#endif // vxBlockVolumeFilterAverage_h


// $Log: vxBlockVolumeFilterAverage.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.4  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.3  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.2  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.1  2002/05/20 20:38:33  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterAverage.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterAverage.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
