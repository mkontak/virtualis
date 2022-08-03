// $Id: VolumeUtilities.C,v 1.3 2007/07/20 19:56:44 cqian Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "VolumeUtilities.h"
#include "Volume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolume.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Local test function. This forces the compilation of the reformat
 * function so we have an implementation of it for later.
 */
void TestCompilationVolumeUtilities()
{
  LinVolUC volume;
  VolumeUtilities::ReformatVolumeProneToSupine( volume );
} // TestCompilation()


/**
 * Rotate the volume 180 degrees around the z-m_longAxis.
 * This could be another case in PermuteCoordinates(), but
 * I couldn't bear to perpetuate that mess any further.
 *
 * @param vol           the volume to rotate
 */
void VolumeUtilities::ReformatVolumeProneToSupine( LinVolUC & volume )
{

  LogDbg( "reformatting volume to supine: " + volume.GetFileName(), "VolumeUtilities", "ReformatVolumeProneToSupine" );

  // We're just going to run through each slice and
  // swap the voxels at the start and end of the slice.
  // This will achieve a 180 degree rotation around Z.
  const int4 iSliceSize = volume.GetDim().m_x * volume.GetDim().m_y;
  for ( int4 z=0; z<volume.GetDim().m_z; ++z )
  {
    uint1 * pSliceData = & volume.GetDataPtr()[ z * iSliceSize ];

    for ( int i=0; i<iSliceSize/2; ++i )
    {
      Swap( pSliceData[i], pSliceData[ iSliceSize - 1 - i ] );
    }
  }

} // ReformatVolumeToSupine()


/**
 * Rotate the volume 180 degrees around the z-axis.
 * This could be another case in PermuteCoordinates(), but
 * I couldn't bear to perpetuate that mess any further.
 *
 * @param vol           the volume to rotate
 */
void VolumeUtilities::ReformatVolumeProneToSupineB( vxBlockVolume<uint1> & volume )
{
  // this operation takes 3 ~ 4 seconds!!!  Dongqing 20080930

  //LogDbg( "reformatting volume to supine: " + volume.GetFileName(), "VolumeUtilities", "ReformatVolumeProneToSupine" );

  vxBlockVolumeIterator<uint1> it1(volume);
  vxBlockVolumeIterator<uint1> it2(volume);
  const int4 iX = volume.GetHeader().GetVolDim().m_x;
  const int4 iY = volume.GetHeader().GetVolDim().m_y;
  const int4 iZ = volume.GetHeader().GetVolDim().m_z;
  const int4 ix = iX-1;
  const int4 iy = iY-1;

  register uint1 uv;
  // We're just going to run through each slice and
  // swap the voxels at the start and end of the slice.
  // This will achieve a 180 degree rotation around Z.
  register int4 x, y, z;
  for( z=0, it1.SetPosZ(0), it2.SetPosZ(0); z<iZ; z++, it1.IncZ(), it2.IncZ() )
  {
    for( y=0, it1.SetPosY(0), it2.SetPosY(iy); y<iY; y++, it1.IncY(), it2.DecY() )
    {
      for( x=0, it1.SetPosX(0), it2.SetPosX(ix); x<y; x++, it1.IncX(), it2.DecX() )
      {
        uv = it1.GetVoxel();
        it1.SetVoxel( it2.GetVoxel() );
        it2.SetVoxel( uv );
      } // for x
    } // for y
  } // for z

} // ReformatVolumeToSupine()



// undefines
#undef FILE_REVISION


// $Log: VolumeUtilities.C,v $
// Revision 1.3  2007/07/20 19:56:44  cqian
// Check in for testing
//
// Revision 1.2  2007/03/06 18:57:36  dongqing
// code review
//
// Revision 1.1  2006/09/13 15:11:41  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
// Added utilities to flip prone volumes to supine
//
// $Id: VolumeUtilities.C,v 1.3 2007/07/20 19:56:44 cqian Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/VolumeUtilities.C,v 1.3 2007/07/20 19:56:44 cqian Exp $
