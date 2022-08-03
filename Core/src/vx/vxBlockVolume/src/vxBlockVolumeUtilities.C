// $Id: vxBlockVolumeUtilities.C,v 1.1 2006/09/13 15:11:41 frank Exp $
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
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeUtilities.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Local test function. This forces the compilation of the reformat
 * function so we have an implementation of it for later.
 */
void TestCompilationVxBlockVolumeUtilities()
{
  vxBlockVolume<uint1> volUint1;
  vxBlockVolume<uint2> volUint2;
  vxBlockVolume<float4> volFloat4;
  vxBlockVolumeUtilities::ReformatVolumeToSupine( volUint1, "FFP" );
  vxBlockVolumeUtilities::ReformatVolumeToSupine( volUint2, "FFP" );
  vxBlockVolumeUtilities::ReformatVolumeToSupine( volFloat4, "FFP" );
} // TestCompilation()


/**
 * reformat a block volume from a random body position to supine orientation
 *
 * @param sBodyPosition looking for "P", "DR", or "DL" (leaves "S" and everything else alone)
 * @param vol           the volume to rotate
 */
template <class T>
void vxBlockVolumeUtilities::ReformatVolumeToSupine( vxBlockVolume<T> & vol, const std::string & sBodyPosition )
{

  LogDbg( "reformatting volume to supine: " + vol.GetHeader().GetFileName(), "VolumeUtilities", "ReformatVolumeProneToSupine" );

  // dim.x may not equal dim.y
  int NeedToDifferentiateBetweenXandYdimensions;

  const Triple<int4> & dim( vol.GetHeader().GetVolDim() );
  const int iX( dim.m_x );
  const int iY( dim.m_y );
  const int iZ( dim.m_z );
  T * pS = new T[ iX*iY ];

  int4 x, y, z;

  vxBlockVolumeIterator<T> itv( vol );

  // prone position
  if( sBodyPosition[2] == 'P' )
  {
   for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
   {
	   int4 i = 0;
	   /// copy slice to buffer
	   for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	   {
		   for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		   {
			   pS[i] = itv.GetVoxel();
			   i++;
		   } // for x
	   } // for y

	   int4 j = ( iY - 1 ) * iX;
	   /// reformat slice
	   for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	   {
		   i = ( iX - 1 );

		   for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		   {
			   itv.SetVoxel( pS[i+j] );
			   i--;
		   } // for x

		   j -= iX;
	   } // for y
   } // for z
  } // if

  // decubitus right position
  else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'R' )
  {
   for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
   {
	   int4 i = 0;
	   /// copy slice to buffer
	   for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	   {
		   for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		   {
			   pS[i] = itv.GetVoxel();
			   i++;
		   } // for x
	   } // for y

	   /// reformat slice
	   for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	   {
       // TODO: see note above
		   i = ( iX - 1 ) * iX;

		   for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		   {
			   itv.SetVoxel( pS[y+i] );

			   i -= iX;
		   } // for x
	   } // for y
   } // for z
  } // else if

  // decubitus left position
  else if( sBodyPosition[2] == 'D' && sBodyPosition[3] == 'L' )
  {
   for( z=0, itv.SetPosZ(0); z<iZ; z++, itv.IncZ() )
   {
	   int4 i = 0;
	   /// copy slice to buffer
	   for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	   {
		   for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		   {
			   pS[i] = itv.GetVoxel();
			   i++;
		   } // for x
	   } // for y

	   int4 j = ( iY - 1 );
	   /// reformat slice
	   for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	   {
		   i = 0;

		   for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		   {
			   itv.SetVoxel( pS[j+i] );

			   i = i+iX;
		   } // for x

		   j--;
	   } // for y
   } // for z
  } // else if

  delete pS;

} // ReformatVolumeToSupine()


// undefines
#undef FILE_REVISION


// $Log: vxBlockVolumeUtilities.C,v $
// Revision 1.1  2006/09/13 15:11:41  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
// Added utilities to flip prone volumes to supine
//
// $Id: vxBlockVolumeUtilities.C,v 1.1 2006/09/13 15:11:41 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeUtilities.C,v 1.1 2006/09/13 15:11:41 frank Exp $
