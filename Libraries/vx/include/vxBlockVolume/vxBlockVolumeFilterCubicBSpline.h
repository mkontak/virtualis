// $Id: vxBlockVolumeFilterCubicBSpline.h,v 1.2 2005/05/12 13:55:03 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Wenli {ingmar|wenli}@viatronix.com

#ifndef vxBlockVolumeFilterCubicBSpline_h
#define vxBlockVolumeFilterCubicBSpline_h


// include declarations
#include "vxBlockVolumeFilter.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


/**
 * Cubic b-spline filter
 */
template < class VoxelType >
class VX_BLOCK_VOLUME_DLL vxBlockVolumeFilterCubicBSpline : public vxBlockVolumeFilter<VoxelType>
{
// member functions
public:

  // standard constructor
  vxBlockVolumeFilterCubicBSpline( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors07 );

  // Copy constructor
  vxBlockVolumeFilterCubicBSpline( vxBlockVolumeFilterCubicBSpline<VoxelType> & other );

  /// filters the source volume and writes the result into the desination volume
  void ApplyFilter( );
  
// data members
private:

  /// the neighborhood of voxels to compute the median value from 
  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum m_eNeighborhood;

}; // class vxBlockVolumeFilterCubicBSpline


#include "vxBlockVolumeFilterCubicBSpline.inl"


#endif // vxBlockVolumeFilterCubicBSpline_h


// $Log: vxBlockVolumeFilterCubicBSpline.h,v $
// Revision 1.2  2005/05/12 13:55:03  frank
// added more unit and performance tests
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.3  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.2  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.1  2002/05/22 23:39:15  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterCubicBSpline.h,v 1.2 2005/05/12 13:55:03 frank Exp $
// $Id: vxBlockVolumeFilterCubicBSpline.h,v 1.2 2005/05/12 13:55:03 frank Exp $
