// $Id: vxBlockVolumeFilterMedianSameSize.h,v 1.2 2005/05/12 13:55:03 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dongqing Chen dongqing@viatronix.com

#ifndef vxBlockVolumeFilterMedianSameSize_h
#define vxBlockVolumeFilterMedianSameSize_h


// include declarations
#include "vxBlockVolumeFilter.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


/**
 * Median filter for specific 2D neighbor
 */
template < class VoxelType >
class VX_BLOCK_VOLUME_DLL vxBlockVolumeFilterMedianSameSize : public vxBlockVolumeFilter<VoxelType>
{
// member functions
public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor & assignment

  // standard constructor
  vxBlockVolumeFilterMedianSameSize( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xy  );

  // Copy constructor
  vxBlockVolumeFilterMedianSameSize( vxBlockVolumeFilterMedianSameSize<VoxelType> & other );

  /// sets the label Volume
  void SetLabVolume( vxBlockVolume<uint1> & labelVolume ) { m_pLabelVolume = & labelVolume; }

  /// sets the label index of ROI
  void SetLabelIndexForROI( uint1 uIndex ) { m_uLabelIndexOfROI = uIndex; }

  /////////////////////////////////////////////////////////////////////////////
  // Run the actual filter

  /// filters the source volume and writes the result into the desination volume
  void ApplyFilter();

// private data members
private:

  /// the neighborhood of voxels to compute the median value from 
  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum m_eNeighborhood;

  /// the label volume
  vxBlockVolume<uint1> * m_pLabelVolume;

  /// the label index for the ROI
  uint1  m_uLabelIndexOfROI;

}; // class vxBlockVolumeFilterMedianSameSize

#endif // vxBlockVolumeFilterMedianSameSize_h


// $Log: vxBlockVolumeFilterMedianSameSize.h,v $
// Revision 1.2  2005/05/12 13:55:03  frank
// added more unit and performance tests
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/07/24 18:23:29  dongqing
// add label volume
//
// Revision 1.1  2003/07/23 19:54:39  dongqing
// new
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterMedianSameSize.h,v 1.2 2005/05/12 13:55:03 frank Exp $
// $Id: vxBlockVolumeFilterMedianSameSize.h,v 1.2 2005/05/12 13:55:03 frank Exp $
