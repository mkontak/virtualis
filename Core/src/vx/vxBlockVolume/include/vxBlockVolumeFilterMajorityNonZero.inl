// $Id: vxBlockVolumeFilterMajorityNonZero.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com
//
// Complete history at bottom of file.

#include "Global.h"
#include <algorithm>

#define FILE_REVISION "$Revision: 1.1.1.1 $"

template <class VoxelType >
VoxelType vxBlockVolumeFilterMajorityNonZero<VoxelType> :: SelectMajorityValueNonZero(std::vector<VoxelType> & vVoxels)
{
  /// the common case should be that the voxel vector is roughly uniform and the first element is also the majority element

  int4 iOccurrances = 1;
  std::vector<VoxelType>::iterator it(vVoxels.begin()+1);
  std::vector<VoxelType>::iterator itEnd(vVoxels.end());
  for ( ; it<itEnd ; ++it )
  {
    if ( vVoxels[0] == *it )
    {
      ++iOccurrances;
    }
  }
  if ( iOccurrances == vVoxels.size() )
  {
    return (vVoxels[0]);
  }
  else if ( iOccurrances >= vVoxels.size()/2   &&   vVoxels[0] != 0)
  {
    return (vVoxels[0]);
  }

  /// pick from sorted list that is known to contain non-zero values
  std::sort(vVoxels.begin(), vVoxels.end());
  iOccurrances = 0;
  int4 iMaxOccurrances = 0;
  // advance past initial zeros
  for ( it = vVoxels.begin(); *it==0; ++it )
  {
	  ; // nop
  }
  VoxelType currentValue  = *it;
  VoxelType majorityValue = *it;

  for ( ; it < itEnd ; ++it )
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
  
  return (majorityValue);
}// SelectMajorityValueNonZero()

#undef FILE_REVISION


// $Log: vxBlockVolumeFilterMajorityNonZero.inl,v $
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
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterMajorityNonZero.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterMajorityNonZero.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $