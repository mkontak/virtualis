// $Id: vxBlockVolumeFilterMedian.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Rekha {ingmar|rekha}@viatronix.com


/**
 * Select the median value
 *
 * @param vVoxels the list of values
 * @return the median value
 */
template <class VoxelType >
VoxelType vxBlockVolumeFilterMedian<VoxelType> :: SelectMedianValue(std::vector<VoxelType> & vVoxels)
{
  //
  // Why not use the std::nth_element function?  It's probably faster and more reliable.
  //

  int4 i,j,l,m ;
  int4 x ;
  
  l=0 ; m=vVoxels.size()-1 ;
  while (l<m) {
    x=vVoxels[vVoxels.size()/2] ;
    i=l ;
    j=m ;
    do {
      while (vVoxels[i]<x) i++ ;
      while (x<vVoxels[j]) j-- ;
      if (i<=j) {
        Swap(vVoxels[i],vVoxels[j]) ;
        i++ ; j-- ;
      }
    } while (i<=j) ;
    if (j<vVoxels.size()/2) l=i ;
    if (vVoxels.size()/2<i) m=j ;
  }

  return vVoxels[vVoxels.size()/2];

} // SelectMedianValue()


// $Log: vxBlockVolumeFilterMedian.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/22 14:37:34  frank
// code review
//
// Revision 1.8  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.7  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.6  2002/09/25 21:22:17  ingmar
// register -> int4
//
// Revision 1.5  2002/05/20 15:48:52  soeren
// removed redundant arguments from SelectMedianValue
//
// Revision 1.4  2002/05/14 23:02:29  ingmar
// added & to function parameter
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
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterMedian.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterMedian.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
