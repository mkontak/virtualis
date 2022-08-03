// $Id: vxBlockVolumeFilterMajority.inl,v 1.2 2006/01/20 18:09:00 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Rekha {ingmar|rekha}@viatronix.com


// includes
#include "Global.h"
#include <algorithm>


/**
 * Select the majority value
 *
 * @param vVoxels the input array of values
 * @return the majority value
 */
template <class VoxelType >
VoxelType vxBlockVolumeFilterMajority<VoxelType> :: SelectMajorityValue(std::vector<VoxelType> & vVoxels)
{
  /// the common case should be that the voxel vector is roughly uniform and the first element is also the majority element

  int4 iOccurrances( 1 );
  for ( std::vector<VoxelType>::iterator it( vVoxels.begin() + 1 ); it<vVoxels.end() ; ++it )
  {
    if ( vVoxels[0] == *it )
    {
      ++iOccurrances;
    }
  }

  if ( iOccurrances >= vVoxels.size() / 2 )
    return vVoxels[0];

  /// first element is more than 50%
  std::sort(vVoxels.begin(), vVoxels.end());
  VoxelType currentValue  = vVoxels[0];
  VoxelType majorityValue = vVoxels[0];
  iOccurrances = 1;
  int4 iMaxOccurrances = 0;
  for ( std::vector<VoxelType>::iterator it = vVoxels.begin(); it < vVoxels.end() ; ++it )
  {
    if ( *it == currentValue )
    {
      ++iOccurrances;
    }
    else
    {
      // just discovered new value, check if last value was the up-to-now majority
      if ( iOccurrances > iMaxOccurrances )
      {
        iMaxOccurrances = iOccurrances;
        majorityValue = currentValue;
      }
      iOccurrances = 1;
      currentValue = *it;
    } // if (value was repeated)
  } // for all vector elements
  
  // check if the last value was the maximum occurring one
  if ( iOccurrances > iMaxOccurrances )
  {
    iMaxOccurrances = iOccurrances;
	  majorityValue = currentValue;
  }  

  return majorityValue;

} // SelectMajorityValue()


// $Log: vxBlockVolumeFilterMajority.inl,v $
// Revision 1.2  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.4  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.3  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.2  2002/09/27 16:36:51  ingmar
// fixed boundary condition
//
// Revision 1.1  2002/05/14 23:18:08  ingmar
// initial version
//
// Revision 1.3  2002/05/13 20:22:32  ingmar
// renamed FindMedianValue() to SelectMedianValue()
//
// Revision 1.2  2002/05/07 18:05:41  rekha
// added FindMedianValue() method
//
// Revision 1.1  2002/04/25 17:22:14  ingmar
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterMajority.inl,v 1.2 2006/01/20 18:09:00 geconomos Exp $
// $Id: vxBlockVolumeFilterMajority.inl,v 1.2 2006/01/20 18:09:00 geconomos Exp $