// $Id: vxBlockVolumeFilterAverage.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com


// Includes
#include "Global.h"
#include <algorithm>


/**
 * selects the average of the vector
 *
 * @param vVoxels the list of voxels
 * @return the average
 */
template <class VoxelType >
VoxelType vxBlockVolumeFilterAverage<VoxelType> :: SelectAverageValue(std::vector<VoxelType> & vVoxels)
{

  VoxelType sum(0);

  for ( std::vector<VoxelType>::iterator it( vVoxels.begin() ); it<vVoxels.end() ; ++it )
    sum += *it;

  return sum / vVoxels.size();

} // SelectAverageValue()


// $Log: vxBlockVolumeFilterAverage.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.5  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.4  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.3  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.2  2002/05/23 15:41:54  ingmar
// the average is now taken of all elements in the vector, it nolonger skipps the first
//
// Revision 1.1  2002/05/20 20:38:33  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterAverage.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterAverage.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $