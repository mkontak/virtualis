/// $Id: vxCRES.C,v 1.12 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen, dongqing@viatronix.net
//
// Complete history at bottom of file.


#include "stdafx.h"
#include "vxCRES.h"
#include "vxBoneRemoval.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "Logger.h"

using namespace std;
using namespace hdr;

/**
 *  This class implements the Colonic Residue Electronic Subtraction (CRES) algorithms.
 *  It will subtract the contrast enhanced colonic residue from CT images.
 */

#define FILE_REVISION "$Revision: 1.12 $"

/** 
*  Default constructor
*
*  @param pVol the pointer to the CT volume (may or may not be cleansed)
*  @param pMask the pointer to the EC mask
*  @param sourceFilePath
*  @param destFilePath
*/
vxCRES::vxCRES( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask, 
			          const std::string & sourceFilePath, const std::string destFilePath )
:m_pVol( pVol ), m_pMask( pMask ), m_sourceFilePath(sourceFilePath), m_destFilePath(destFilePath)
{
  m_uBGlabel   = 0xFF;
	m_uATBlabel  = 0xFE;

  // setting the thresholds
  m_uAirTh                  =  350;   // -650 HU Note, this value must coordinate to that of used in INS
	m_uTissueHigh             = 1100;   //  150 HU
  m_uTagThLow               = 1210;   //  210 HU
  m_uTagThHigh              = 1900;   //  450 HU
  m_iIntensityGapAtBoundary =  180;   //      HU

  m_uMaximumNumOfPuddle     = 100; 
	m_uAirLabel               = 2*m_uMaximumNumOfPuddle+1;

	const Triple<float4> vfUnits( m_pVol->GetHeader().GetVoxelUnits() );
	m_uSmallestAirLumenVolume = static_cast<int4>(2.8F/vfUnits.m_x) *
      static_cast<int4>(2.8F/vfUnits.m_y) * static_cast<int4>(2.6F/vfUnits.m_z);

  m_uSmallestPuddleVolume = static_cast<int4>(5.1F/vfUnits.m_x) *
      static_cast<int4>(5.1F/vfUnits.m_y) * static_cast<int4>(4.1F/vfUnits.m_z);

	m_fSignificantPortion = 0.45F;   // 45% of entire puddle region
} // default constructor


/** 
*  Default deconstructor
*/
vxCRES::~vxCRES()
{
}


/** 
*  Apply colonic residue electronic subtraction procedure
*
*  @return true if it is successful
*/
bool vxCRES::Run()
{
  LogFyi("Start CRES procedure", "Run", "vxCRES.C");

  try
  { // remove bone for efficiency
    vxBoneRemoval removeBone( m_pVol, m_pMask );
    removeBone.Run();
  }
  catch( ... )
  {
    LogErr("Bone removal fail", "Run", "vxCRES.C");
	  return(false);
  }

	try
	{
		DelineatAirRegion();
	}
	catch( ... )
	{
    LogErr("Delineat air region fail", "Run", "vxCRES.C");
	  return(false);
	}

	// container for unique feature intensity of all puddles
	std::vector<uint2> vUBIntensityOfPuddle;

	try
	{ // label contrast enhanced residual regions 
		LabelTaggedRegions( vUBIntensityOfPuddle );
	}
	catch( ... )
	{
    LogErr("Label tagged region fail", "Run", "vxCRES.C");
	  return(false);
	}

	try
	{ // Determine air/contrast-enhanced boundary
		DetermineATB();
	}
	catch( ... )
	{
    LogErr("Determine ATB voxel fail", "Run", "vxCRES.C");
	  return(false);
	}

	try
	{ // reverse the intensity of the contrast-enhanced residue
		IntensityReversion( vUBIntensityOfPuddle );
	}
	catch( ... )
	{
    LogErr("Intensity reversion fail", "Run", "vxCRES.C");
	  return(false);
	}

	return(true);
} // Run


/**
 *  Determine air lumen
 */
void vxCRES::DelineatAirRegion()
{
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());
	const int4 iX(viDim.m_x-1);
	const int4 iY(viDim.m_y-1);
	const int4 iZ(viDim.m_z-1);	

	vxBlockVolumeIterator<uint1> itm(*m_pMask);
	vxBlockVolumeIterator<uint2> itv(*m_pVol );

	register int4           n( 0 );  
	register Vector3D<int2> viSeed;
	const    uint1          uTmpLabel( m_uATBlabel-1 );

	/// label air region
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
	  {
		  if( itm.GetVoxel() == 0x00 )
			{
				viSeed = itm.GetPos();
				itv.SetPos( viSeed );

				if( itv.GetVoxel() < m_uAirTh && viSeed.m_z > 0 && viSeed.m_z < iZ &&
					  viSeed.m_y > 0 && viSeed.m_y < iY && viSeed.m_x > 0 && viSeed.m_x < iX )
				{
					n = 0;
				  n = GrowAirRegion( viSeed, 0x00, m_uAirLabel, m_uAirTh );

				  // iterator synchronization
				  itm.SetPos(itm.GetPos());

				  if( n < m_uSmallestAirLumenVolume )
				  {
            if( n != Grow3D6NB(m_pMask, viSeed, m_uAirLabel, uTmpLabel) )
							LogErr("Grow air region incorrectly", "DelineatAirRegion", "vxCRES");
					} // if
				} // if
			} // if
		} // for
	} // for

	/// reset the temporary label index
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		  if( itm.GetVoxel() == uTmpLabel )
			  itm.SetVoxel(0x00);
  }
} // DelineatAirRegion()


/**
 *  grwo out the air region 
 *
 *  @param viSeed
 *  @param uROIlabel
 *  @param uNewLabel
 *  @param uThreshold
 *  @return number of gorwn voxel
 */
int4 vxCRES::GrowAirRegion( const Vector3D<int2> & viSeed, const uint1 & uROIlabel, 
													  const uint1 & uNewLabel, const uint2 & uThreshold )
{
  const Triple<int4> viDim(m_pMask->GetHeader().GetVolDim());
  const int4 iX(viDim.m_x-1);
  const int4 iY(viDim.m_y-1);
  const int4 iZ(viDim.m_z-1);

  vxBlockVolumeIterator<uint1> itm( *m_pMask );
  vxBlockVolumeIterator<uint2> itv( *m_pVol  );

  // queue for region growing
  register int4   iCount(0);
  register    Vector3D<int2>   vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	itv.SetPos(vi);
	if( itm.GetVoxel() != uROIlabel || itv.GetVoxel() >= uThreshold )
		return(iCount);
  itm.SetVoxel(uNewLabel);
	iCount++;

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
	  itm.SetPos(vi);
		itv.SetPos(vi);

	  vi.m_x = vi.m_x+1;
		if( (vi.m_x < iX)&&(itm.Peek1px0py0pz() == uROIlabel)&&(itv.Peek1px0py0pz() < uThreshold) )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncX();
      itm.SetVoxel(uNewLabel);
			itm.DecX();
	    iCount++;
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( (vi.m_y < iY)&&(itm.Peek0px1py0pz() == uROIlabel)&&(itv.Peek0px1py0pz() < uThreshold) )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncY();
      itm.SetVoxel(uNewLabel);
			itm.DecY();
	    iCount++;
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( (vi.m_x > 0)&&(itm.Peek1nx0py0pz() == uROIlabel)&&(itv.Peek1nx0py0pz() < uThreshold) )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.DecX();
      itm.SetVoxel(uNewLabel);
			itm.IncX();
	    iCount++;
		} // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( (vi.m_y > 0)&&(itm.Peek0px1ny0pz() == uROIlabel)&&(itv.Peek0px1ny0pz() < uThreshold) )
		{
      viRegQueue.push( vi );
			itm.DecY();
      itm.SetVoxel(uNewLabel);
			itm.IncY();
	    iCount++;
		} // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( (vi.m_z > 0 )&&(itm.Peek0px0py1nz() == uROIlabel)&&(itv.Peek0px0py1nz() < uThreshold) )
		{
      viRegQueue.push( vi );
			itm.DecZ();
      itm.SetVoxel(uNewLabel);
			itm.IncZ();
	    iCount++;
		} // if

	  vi.m_z = vi.m_z+2;
		if( (vi.m_z < iZ )&&(itm.Peek0px0py1pz() == uROIlabel)&&(itv.Peek0px0py1pz() < uThreshold) )
		{
      viRegQueue.push( vi );
			itm.IncZ();
      itm.SetVoxel(uNewLabel);
			itm.DecZ();
	    iCount++;
		} // if

	  viRegQueue.pop();
  }  // end of while() 

	return(iCount);
} // GrowAirRegion


/**
 *  Grow out 3D region with 6-connected neighbor 
 *
 *  @param pMask the pointer to the mask
 *  @param viSeed 
 *  @param uROIlabel the original label for the ROI
 *  @param uNewLabel for labeling grown region
 *  @return the number of voxels that are grown
 */
int4 vxCRES::Grow3D6NB( vxBlockVolume<uint1> * pMask, const Vector3D<int2> & viSeed, 
											  const uint1 & uROIlabel, const uint1 & uNewLabel )
{
  const Triple<int4> viDim(pMask->GetHeader().GetVolDim());
  const int4 iX(viDim.m_x-1);
  const int4 iY(viDim.m_y-1);
  const int4 iZ(viDim.m_z-1);

  vxBlockVolumeIterator<uint1> itm(*pMask);

  // queue for region growing
  int4 iCount(0);
  register    Vector3D<int2>   vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	if( itm.GetVoxel() != uROIlabel )
		return(iCount);
  itm.SetVoxel(uNewLabel);
	iCount++;

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
	  itm.SetPos(vi);

	  vi.m_x = vi.m_x+1;
		if( (vi.m_x < iX)&&(itm.Peek1px0py0pz() == uROIlabel) )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncX();
      itm.SetVoxel(uNewLabel);
			itm.DecX();
	    iCount++;
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( (vi.m_y < iY)&&(itm.Peek0px1py0pz() == uROIlabel) )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.IncY();
      itm.SetVoxel(uNewLabel);
			itm.DecY();
	    iCount++;
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( (vi.m_x > 0)&&(itm.Peek1nx0py0pz() == uROIlabel) )
    { // add the neighbor to the queue
      viRegQueue.push( vi );
			itm.DecX();
      itm.SetVoxel(uNewLabel);
			itm.IncX();
	    iCount++;
		} // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( (vi.m_y > 0)&&(itm.Peek0px1ny0pz() == uROIlabel) )
		{
      viRegQueue.push( vi );
			itm.DecY();
      itm.SetVoxel(uNewLabel);
			itm.IncY();
	    iCount++;
		} // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( (vi.m_z > 0)&&(itm.Peek0px0py1nz() == uROIlabel) )
		{
      viRegQueue.push( vi );
			itm.DecZ();
      itm.SetVoxel(uNewLabel);
			itm.IncZ();
	    iCount++;
		} // if

	  vi.m_z = vi.m_z+2;
		if( (vi.m_z < iZ)&&(itm.Peek0px0py1pz() == uROIlabel) )
		{
      viRegQueue.push( vi );
			itm.IncZ();
      itm.SetVoxel(uNewLabel);
			itm.DecZ();
	    iCount++;
		} // if

	  viRegQueue.pop();
  }  // end of while() 

  return(iCount);
} //Grow3D6NB


/** 
 *  label the contrast enhanced residual regions
 *
 *  @param vUBIntensityOfPuddle is the container for all intensities upper bounds
 */
void vxCRES::LabelTaggedRegions( std::vector<uint2> & vUBIntensityOfPuddle )
{
	if( vUBIntensityOfPuddle.size() > 0 )
	{
		LogErr("The average intensity container is not initialized correctly", "LabelTaggedRegions", "vxCRES.C");
		return;
	}

	const int4 iX( m_pVol->GetHeader().GetVolDim().m_x-1 );
	const int4 iY( m_pVol->GetHeader().GetVolDim().m_y-1 );
	const int4 iZ( m_pVol->GetHeader().GetVolDim().m_z-1 );

	vxBlockVolumeIterator<uint1> ittm( *m_pMask );
	vxBlockVolumeIterator<uint2> itv ( *m_pVol  );

	register uint1 v1, v2, v3, uTmpLabel( m_uATBlabel-1 );
	register int4  n;
	register Vector3D<int2> vPos;
	uint2    uUpBoundIntensity;
	uint1    uCurrentPuddleLabel( 0x01 );

	// find out all tagged puddles
	for( ittm.SetPos(0,0,0), itv.SetPos(0,0,0); ittm.IsNotAtEnd(); ittm.NextBlockZYX(), itv.NextBlockZYX() )
	{
		bool bBreakLoop(false);

		for( ; ittm.IsNotAtEndOfBlock(); ittm.NextZYXinsideBlock(), itv.NextZYXinsideBlock() )
		{
			if( ittm.GetVoxel() == 0x00 && itv.GetVoxel() >= m_uTagThLow )
			{ // this is a contrast enhanced voxel
        ittm.DecY();
				v1 = ittm.GetVoxel();
        ittm.DecY();
				v2 = ittm.GetVoxel();
				v3 = ittm.PeekRel(0,-1,0);
				ittm.IncY();
				ittm.IncY();

				vPos = ittm.GetPos();
        // check if there is air near by
				if( v1 == m_uAirLabel || v2 == m_uAirLabel || v3 == m_uAirLabel )
				{ // The voxel has air voxel near by
					if( vPos.m_z > 0 && vPos.m_z < iZ &&
						  vPos.m_y > 0 && vPos.m_y < iY &&
						  vPos.m_x > 0 && vPos.m_x < iX )
					{
						uUpBoundIntensity = 0;
						n = 0;
						n = GrowTaggedRegion( vPos, 0x00, uCurrentPuddleLabel, uUpBoundIntensity );

						// synchronize the iterator in case the block were split.
						ittm.SetPos(ittm.GetPos());

						if( n < m_uSmallestPuddleVolume )
						{ // ROI too small to keep
							if( n != Grow3D6NB( m_pMask, vPos, uCurrentPuddleLabel, uTmpLabel ) )
								LogErr("Growing small puddle incorrectly", "LabelTaggedRegions", "vxCRES.C");

							ittm.SetPos( ittm.GetPos() );
						}
						else
						{
							uCurrentPuddleLabel++;
							vUBIntensityOfPuddle.push_back( uUpBoundIntensity );
						} // if-else
					} // if seed is not at the boundary of volume
				} // if has air voxel near by
			} // if ittm.GetVoxel() == 0x00 && itv.GetVoxel() > m_uTagThLow 

			if( uCurrentPuddleLabel > m_uMaximumNumOfPuddle )
			{ 
				bBreakLoop = true;
				break;
			} // if
		} // for inside the block

		if( bBreakLoop )
			break;
	} // for all the blocks

	// reset temporary label
	for( ittm.SetPos(0,0,0); ittm.IsNotAtEnd(); ittm.NextBlockZYX() )
	{
	  for( ; ittm.IsNotAtEndOfBlock(); ittm.NextZYXinsideBlock() )
		{
			if( ittm.GetVoxel() == uTmpLabel )
				ittm.SetVoxel(0x00);
		}
	} // for

	register uint1 uu;
	// dilate 18-connected layer of tagged puddle
	for( ittm.SetPos(0,0,0), itv.SetPos(0,0,0); ittm.IsNotAtEnd(); ittm.NextBlockZYX(), itv.NextBlockZYX() )
	{
		for( ; ittm.IsNotAtEndOfBlock(); ittm.NextZYXinsideBlock(), itv.NextZYXinsideBlock() )
		{
      uu   = ittm.GetVoxel();
			vPos = ittm.GetPos();
			if( uu < uCurrentPuddleLabel && uu > 0x00 )
			{ // puddle voxel, now set neighbor label indexes
		    if( ittm.PeekRel(1,0,0) == 0x00 && itv.PeekRel(1,0,0) > m_uTissueHigh )
				{
					ittm.IncX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecX();
				} // if
		    if( ittm.PeekRel(-1,0,0) == 0x00 && itv.PeekRel(-1,0,0) > m_uTissueHigh )
				{
					ittm.DecX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncX();
				} // if
		    if( ittm.PeekRel(0,-1,0) == 0x00 && itv.PeekRel(0,-1,0) > m_uTissueHigh )
				{
					ittm.DecY();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncY();
				} // if
		    if( ittm.PeekRel(0,1,0) == 0x00 && itv.PeekRel(0,1,0) > m_uTissueHigh )
				{
					ittm.IncY();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecY();
				} // if
		    if( ittm.PeekRel(1,1,0) == 0x00 && itv.PeekRel(1,1,0) > m_uTissueHigh )
				{
					ittm.IncY(); ittm.IncX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecY(); ittm.DecX();
				} // if
		    if( ittm.PeekRel(-1,1,0) == 0x00 && itv.PeekRel(-1,1,0) > m_uTissueHigh )
				{
					ittm.IncY(); ittm.DecX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecY(); ittm.IncX();
				} // if
		    if( ittm.PeekRel(-1,-1,0) == 0x00 && itv.PeekRel(-1,-1,0) > m_uTissueHigh )
				{
					ittm.DecY(); ittm.DecX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncY(); ittm.IncX();
				} // if
		    if( ittm.PeekRel(1,-1,0) == 0x00 && itv.PeekRel(1,-1,0) > m_uTissueHigh )
				{
					ittm.DecY(); ittm.IncX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncY(); ittm.DecX();
				} // if
		    if( ittm.PeekRel(0,0,1) == 0x00 && itv.PeekRel(0,0,1) > m_uTissueHigh )
				{
					ittm.IncZ();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecZ();
				} // if
		    if( ittm.PeekRel(0,0,-1) == 0x00 && itv.PeekRel(0,0,-1) > m_uTissueHigh )
				{
					ittm.DecZ();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncZ();
				} // if
		    if( ittm.PeekRel(1,0,-1) == 0x00 && itv.PeekRel(1,0,-1) > m_uTissueHigh )
				{
					ittm.DecZ(); ittm.IncX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncZ(); ittm.DecX();
				} // if
		    if( ittm.PeekRel(-1,0,-1) == 0x00 && itv.PeekRel(-1,0,-1) > m_uTissueHigh )
				{
					ittm.DecZ(); ittm.DecX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncZ(); ittm.IncX();
				} // if
		    if( ittm.PeekRel(0,-1,-1) == 0x00 && itv.PeekRel(0,-1,-1) > m_uTissueHigh )
				{
					ittm.DecZ(); ittm.DecY();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncZ(); ittm.IncY();
				} // if
		    if( ittm.PeekRel(0,1,-1) == 0x00 && itv.PeekRel(0,1,-1) > m_uTissueHigh )
				{
					ittm.DecZ(); ittm.IncY();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.IncZ(); ittm.DecY();
				} // if
		    if( ittm.PeekRel(1,0,1) == 0x00 && itv.PeekRel(1,0,1) > m_uTissueHigh )
				{
					ittm.IncZ(); ittm.IncX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecZ(); ittm.DecX();
				} // if
		    if( ittm.PeekRel(-1,0,1) == 0x00 && itv.PeekRel(-1,0,1) > m_uTissueHigh )
				{
					ittm.IncZ(); ittm.DecX();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecZ(); ittm.IncX();
				} // if
		    if( ittm.PeekRel(0,-1,1) == 0x00 && itv.PeekRel(0,-1,1) > m_uTissueHigh )
				{
					ittm.IncZ(); ittm.DecY();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecZ(); ittm.IncY();
				} // if
		    if( ittm.PeekRel(0,1,1) == 0x00 && itv.PeekRel(0,1,1) > m_uTissueHigh )
				{
					ittm.IncZ(); ittm.IncY();
					ittm.SetVoxel(uu+m_uMaximumNumOfPuddle);
					ittm.DecZ(); ittm.DecY();
				} // if
			} // if 
		} // for inside the block
	} // for all blocks

	// clean up
	for( ittm.SetPos(0,0,0); ittm.IsNotAtEnd(); ittm.NextBlockZYX() )
	{
		for( ; ittm.IsNotAtEndOfBlock(); ittm.NextZYXinsideBlock() )
		{
      uu = ittm.GetVoxel();
			if( uu < m_uAirLabel && uu > m_uMaximumNumOfPuddle )
			  ittm.SetVoxel(uu-m_uMaximumNumOfPuddle);
		} // for inside the block
	} // for all blocks

} // LabelTaggedRegions()


/**
 *  grwo out the tagged region 
 *
 *  @param viSeed
 *  @param uROIlabel
 *  @param uNewLabel
 *  @param uUpperBoundIntensity
 *  @return number of gorwn voxel
 */
int4 vxCRES::GrowTaggedRegion( const Vector3D<int2> & viSeed, const uint1 & uROIlabel, const uint1 & uNewLabel, 
		                           uint2 & uUpperBoundIntensity )
{
  const Triple<int4> viDim(m_pMask->GetHeader().GetVolDim());
  const int4 iX(viDim.m_x-1);
  const int4 iY(viDim.m_y-1);
  const int4 iZ(viDim.m_z-1);

  vxBlockVolumeIterator<uint1> itm( *m_pMask );
  vxBlockVolumeIterator<uint2> itv( *m_pVol  );

	// histogram bin intensity gap
	const int4 iHistBinGap( 6 );         /// hard-coded threshold
	const int4 iHistBinNum( (m_uTagThHigh-m_uTagThLow-1)/iHistBinGap + 1 );
	// histogram bin container
	int4 * pviHist = new int4[ iHistBinNum ];
	if( pviHist == NULL )
	{
		LogErr("No memory", "GrowTaggedRegion", "vxCRES.C");
		return(0);
	}
	for( int4 i=0; i<iHistBinNum; i++ )
		pviHist[i] = 0;

  register int4  iCount(0);
  // queue for region growing
  register    Vector3D<int2>   vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	itv.SetPos(vi);
	if( itm.GetVoxel() != uROIlabel || itv.GetVoxel() <= m_uTagThLow )
		return(iCount);
  itm.SetVoxel(uNewLabel);
	iCount++;

	register int4 j;
	register int4 icv;
  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
	  itm.SetPos(vi);
		itv.SetPos(vi);

		icv = itv.GetVoxel();
		// update the histogram
	  j = (icv-m_uTagThLow-1) / iHistBinGap;
	  pviHist[j] = pviHist[j] + 1;

	  vi.m_x = vi.m_x+1;
		if( vi.m_x < iX && itm.Peek1px0py0pz() == uROIlabel && itv.Peek1px0py0pz() > m_uTagThLow )
    { 
				viRegQueue.push( vi );
				itm.IncX();
				itm.SetVoxel(uNewLabel);
				itm.DecX();
				iCount++;
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( vi.m_y < iY && itm.Peek0px1py0pz() == uROIlabel && itv.Peek0px1py0pz() > m_uTagThLow )
    { 
				viRegQueue.push( vi );
				itm.IncY();
				itm.SetVoxel(uNewLabel);
				itm.DecY();
				iCount++;
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_x > 0 && itm.Peek1nx0py0pz() == uROIlabel && itv.Peek1nx0py0pz() > m_uTagThLow )
    { 
				viRegQueue.push( vi );
				itm.DecX();
				itm.SetVoxel(uNewLabel);
				itm.IncX();
				iCount++;
		} // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_y > 0 && itm.Peek0px1ny0pz() == uROIlabel && itv.Peek0px1ny0pz() > m_uTagThLow )
		{
				viRegQueue.push( vi );
				itm.DecY();
				itm.SetVoxel(uNewLabel);
				itm.IncY();
				iCount++;
		} // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( vi.m_z > 0 && itm.Peek0px0py1nz() == uROIlabel && itv.Peek0px0py1nz() > m_uTagThLow )
		{
				viRegQueue.push( vi );
				itm.DecZ();
				itm.SetVoxel(uNewLabel);
				itm.IncZ();
				iCount++;
		} // if

	  vi.m_z = vi.m_z+2;
		if( vi.m_z < iZ && itm.Peek0px0py1pz() == uROIlabel && itv.Peek0px0py1pz() > m_uTagThLow )
		{
				viRegQueue.push( vi );
				itm.IncZ();
				itm.SetVoxel(uNewLabel);
				itm.DecZ();
				iCount++;
		} // if

	  viRegQueue.pop();
  }  // end of while() 

  uUpperBoundIntensity = m_uTagThLow;

	/// compute the feature intensity for the puddle
	const int4 iB( static_cast<float4>(iCount)*m_fSignificantPortion );
	register int4 iVoxelNum(0);
  for( j=iHistBinNum-1; j>=0; j-- )
	{
    iVoxelNum = iVoxelNum + pviHist[j];
		
		if( iVoxelNum > iB )
		{
			uUpperBoundIntensity = m_uTagThLow + static_cast<uint2>(j*iHistBinGap) + iHistBinGap/2;
			break;
		}
	} // for( j )

	delete [] pviHist;
	return(iCount); 
} // GrowTaggedRegion



/**
 *  Determine air/contrast-enhanced boundary area
 *  this is the critical algorithm for CRES procedure.
 *  We call it balloon blowing algorithm.
 */
void vxCRES::DetermineATB()
{
	vxBlockVolumeIterator<uint1> itm(*m_pMask);
	vxBlockVolumeIterator<uint2> itv(*m_pVol );

	//// start balloon blowing algorithm ////////////////////////
  register Triple<float4> vfUnits(m_pVol->GetHeader().GetVoxelUnits());
  vfUnits.m_x = vfUnits.m_x*vfUnits.m_x;  // square them for speed up computing
  vfUnits.m_y = vfUnits.m_y*vfUnits.m_y;
  vfUnits.m_z = vfUnits.m_z*vfUnits.m_z;
	const int4 iX(m_pVol->GetHeader().GetVolDim().m_x-1);
	const int4 iY(m_pVol->GetHeader().GetVolDim().m_y-1);
	const int4 iZ(m_pVol->GetHeader().GetVolDim().m_z-1);

	// 18 neighbor position 
	std::vector< Vector3D<int2> > viNeighbourhood;
	{
		viNeighbourhood.push_back( Vector3D<int2>( 0, 1, 0));  /// direction of the gravity
		viNeighbourhood.push_back( Vector3D<int2>( 1, 0, 0));  /// horizontal direction in imaging plance
		viNeighbourhood.push_back( Vector3D<int2>(-1, 0, 0));  /// horizontal direction in imaging plance
		viNeighbourhood.push_back( Vector3D<int2>(-1, 1, 0));  /// direction of the tilt gravity XY
		viNeighbourhood.push_back( Vector3D<int2>( 1, 1, 0));  /// direction of the tilt gravity XY
		viNeighbourhood.push_back( Vector3D<int2>( 0, 1, 1));  /// direction of the tilt gravity ZY
		viNeighbourhood.push_back( Vector3D<int2>( 0, 1,-1));  /// direction of the tilt gravity XY

		//////////// only above directions are used in the probe analysis
		viNeighbourhood.push_back( Vector3D<int2>( 0, 1, 1));  /// direction of the tilt gravity ZY
		viNeighbourhood.push_back( Vector3D<int2>( 0, 1,-1));  /// direction of the tilt gravity XY
		viNeighbourhood.push_back( Vector3D<int2>( 0, 0,-1));  /// horizontal direction across imaging plane
		viNeighbourhood.push_back( Vector3D<int2>( 0, 0, 1));  /// horizontal direction across imaging plane	 
    viNeighbourhood.push_back( Vector3D<int2>( 0,-1, 0));
		viNeighbourhood.push_back( Vector3D<int2>( 1,-1, 0));
		viNeighbourhood.push_back( Vector3D<int2>(-1,-1, 0));      
		viNeighbourhood.push_back( Vector3D<int2>( 1, 0, 1));
		viNeighbourhood.push_back( Vector3D<int2>(-1, 0, 1));
		viNeighbourhood.push_back( Vector3D<int2>( 1, 0,-1));
		viNeighbourhood.push_back( Vector3D<int2>(-1, 0,-1));
		viNeighbourhood.push_back( Vector3D<int2>( 0,-1, 1));      
		viNeighbourhood.push_back( Vector3D<int2>( 0,-1,-1));
	}

	LogFyi("Start Balloon-blowing algorithm", "DetermineATB", "vxCRES.C");
  register Vector3D<int2> currentPos, neighborPos;
  register int4 i;
  bool          bIsATB( false );
  const   uint1 ua1( m_uAirLabel+1 );
	int4          viProbeLength[7];

	for( i=0; i<7; i++ )
	{
		if( i == 0 )
			viProbeLength[i] = 5;  // 5
		else if( i < 5 )
			viProbeLength[i] = 3;  // 3
		else if( i < 7 )
			viProbeLength[i] = 2;  // 2
	} // for(i)

  // 1st layer dilation with 5-NB
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
      if( itm.GetVoxel() == m_uAirLabel )
		  { 
				currentPos = itm.GetPos();
				
				if( currentPos.m_x > 0 && currentPos.m_x < iX &&
            currentPos.m_y > 0 && currentPos.m_y < iY &&
            currentPos.m_z > 0 && currentPos.m_z < iZ )
				{
					if( itm.PeekRel(viNeighbourhood[0]) < m_uMaximumNumOfPuddle )
					{
						bIsATB = false;
						bIsATB = ProbeAnalysis(currentPos, viNeighbourhood[0], 5, ua1, 150, 800); 

						// set the neighbor value
						if( bIsATB )
						{
							neighborPos = currentPos+viNeighbourhood[0];
							itm.SetPos(neighborPos);
							itm.SetVoxel( ua1 );
							itm.SetPos(currentPos);
						} // if
					} // if neighbor is not air, nor tissue, nor background

				  // check its neighbor
					for( i=1; i<7; i++ )
					{ // If neighbor is not air, nor background, nor volume boundary
						if( itm.PeekRel(viNeighbourhood[i]) < m_uMaximumNumOfPuddle )
						{
							bIsATB = false;
							bIsATB = ProbeAnalysis(currentPos, viNeighbourhood[i], viProbeLength[i], ua1, 340, 800); 

							// set the neighbor value
							if( bIsATB )
							{
								neighborPos = currentPos+viNeighbourhood[i];
								itm.SetPos(neighborPos);
								itm.SetVoxel( ua1 );
								itm.SetPos(currentPos);
							} // if
						} // if neighbor is not air, nor tissue, nor background
					} // for( i )
				} // if

		  } // if( itm.GetVoxel() == uAirLabel )
	  } // for inside block
  } // for all blocks

	for( i=0; i<5; i++ )
	{
		if( i == 0 )
			viProbeLength[i] = 4; 
		else if( i < 5 ) 
			viProbeLength[i] = 2;
	} // for(i)

	const uint1 ua2(ua1+1);
  // 2nd layer dilation with 5-NB
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
      if( itm.GetVoxel() == ua1 )
		  { 
				currentPos = itm.GetPos();
				
				if( currentPos.m_x > 0 && currentPos.m_x < iX &&
            currentPos.m_y > 0 && currentPos.m_y < iY &&
            currentPos.m_z > 0 && currentPos.m_z < iZ )
				{
					if( itm.PeekRel(viNeighbourhood[0]) < m_uMaximumNumOfPuddle )
					{
						bIsATB = false;
						bIsATB = ProbeAnalysis(currentPos, viNeighbourhood[0], 4, ua2, 140, 600 ); 

						// set the neighbor value
						if( bIsATB )
						{
							neighborPos = currentPos+viNeighbourhood[0];
							itm.SetPos(neighborPos);
							itm.SetVoxel( ua2 );
							itm.SetPos(currentPos);
						} // if
					} // if neighbor is not air, nor tissue, nor background

					// check its neighbor
					for( i=1; i<5; i++ )
					{ // If neighbor is not air, nor tissue, nor background
						if( itm.PeekRel(viNeighbourhood[i]) < m_uMaximumNumOfPuddle )
						{
							bIsATB = false;
							bIsATB = ProbeAnalysis(currentPos, viNeighbourhood[i], viProbeLength[i], ua2, 250, 610); 

							// set the neighbor value
							if( bIsATB )
							{
								neighborPos = currentPos+viNeighbourhood[i];
								itm.SetPos(neighborPos);
								itm.SetVoxel( ua2 );
								itm.SetPos(currentPos);
							} // if
						} // if neighbor is not air, nor tissue, nor background
					} // for( i )
				} // if

		  } // if( itm.GetVoxel() == uAirLabel )
	  } // for inside block
  } // for all blocks

	for( i=0; i<5; i++ )
	{
		if( i == 0 )
			viProbeLength[i] = 3;
		else if( i < 3 ) 
			viProbeLength[i] = 1;
	} // for(i)

	const uint1 ua3(ua2+1);
  // 3rd layer dilation with 1-NB
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
      if( itm.GetVoxel() == ua2 )
		  { 
				currentPos = itm.GetPos();
				
				if( currentPos.m_x > 0 && currentPos.m_x < iX &&
            currentPos.m_y > 0 && currentPos.m_y < iY &&
            currentPos.m_z > 0 && currentPos.m_z < iZ )
				{
					// If neighbor is not air, nor tissue, nor background
						if( itm.PeekRel(viNeighbourhood[0]) < m_uMaximumNumOfPuddle )
						{
							bIsATB = false;
							bIsATB = ProbeAnalysis(currentPos, viNeighbourhood[0], 3, ua3, 20, 80);

							// set the neighbor value
							if( bIsATB )
							{
								neighborPos = currentPos+viNeighbourhood[0];
								itm.SetPos(neighborPos);
								itm.SetVoxel( ua3 );
								itm.SetPos(currentPos);
							} // if
						} // if neighbor is not air, nor tissue, nor background
				} // if

		  } // if( itm.GetVoxel() == uAirLabel )
	  } // for inside block
  } // for all blocks

	const uint1 ua4(ua3+1);
  // 4th layer dilation with 1-NB
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
      if( itm.GetVoxel() == ua3 )
		  { 
				currentPos = itm.GetPos();
				
				if( currentPos.m_x > 0 && currentPos.m_x < iX &&
            currentPos.m_y > 0 && currentPos.m_y < iY &&
            currentPos.m_z > 0 && currentPos.m_z < iZ )
				{
					// If neighbor is not air, nor tissue, nor background
						if( itm.PeekRel(viNeighbourhood[0]) < m_uMaximumNumOfPuddle )
						{
							bIsATB = false;
							bIsATB = ProbeAnalysis(currentPos, viNeighbourhood[0], 2, ua4, 6, 20);

							// set the neighbor value
							if( bIsATB )
							{
								neighborPos = currentPos+viNeighbourhood[0];
								itm.SetPos(neighborPos);
								itm.SetVoxel( ua4 );
								itm.SetPos(currentPos);
							} // if
						} // if neighbor is not air, nor tissue, nor background
				} // if

		  } // if( itm.GetVoxel() == uAirLabel )
	  } // for inside block
  } // for all blocks

	const uint1 ua5(ua4+1);
	// up clean up partial-volume-effact with air region
  for( itm.SetPos(0,0,0), itv.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX(), itv.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock(), itv.NextZYXinsideBlock() )
		{
			if( itm.GetVoxel() >= ua1 && itm.GetVoxel() <= ua4 )
			{
				currentPos = itm.GetPos();

				if( itm.PeekRel(0,-1,0) == m_uAirLabel && itv.PeekRel(0,-1,0) > 140 )  /// hard-coded threshold -860 HU
				{
					itm.SetPos(currentPos+Vector3D<uint2>(0,-1,0));
					itm.SetVoxel(ua5);
					itm.SetPos(currentPos);

					if( itm.PeekRel(0,-2,0) == m_uAirLabel && itv.PeekRel(0,-2,0) > 140 )
					{
						itm.SetPos(currentPos+Vector3D<uint2>(0,-2,0));
						itm.SetVoxel(ua5);
						itm.SetPos(currentPos);
					}
				} // if
			} // if
		} // for
	} // for

	// clean up label
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
			if( itm.GetVoxel() == m_uAirLabel )
				itm.SetVoxel(0x00);
			else if( itm.GetVoxel() >= ua1 && itm.GetVoxel() <= ua5 )
				itm.SetVoxel( m_uATBlabel );
		}
	} // for

	std::vector< Vector3D<int2> > vp;
	// remove thin spikes
  for( itm.SetPos(0,0,0), itv.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX(), itv.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock(), itv.NextZYXinsideBlock() )
		{
			if( itm.GetVoxel() == m_uATBlabel &&
				  ((itm.PeekRel(1,0,0) != m_uATBlabel && itm.PeekRel(-1,0,0) != m_uATBlabel) ||
					 (itm.PeekRel(0,1,0) != m_uATBlabel && itm.PeekRel(0,-1,0) != m_uATBlabel) ||
					 (itm.PeekRel(0,0,1) != m_uATBlabel && itm.PeekRel(0,0,-1) != m_uATBlabel)) )
			{
				vp.push_back(itm.GetPos());
			} // if
		} // for
	} // for

	for( i=vp.size()-1; i>=0; i-- )
	{
		itm.SetPos( vp[i] );
		itv.SetPos( vp[i] );

		if( itv.GetVoxel() >= m_uTagThLow && itm.PeekRel(0,1,0) > 0x00 && itm.PeekRel(0,1,0) < m_uMaximumNumOfPuddle )
			itm.SetVoxel( itm.PeekRel(0,1,0) );
		else
		  itm.SetVoxel(0x00);
	} // for
} // DetermineATB


/**
 *  probe analysis. The probe is the line segments going out from the current voxel 
 *
 *  @param startPos
 *  @param directionStep
 *  @param numStep
 *  @param uNewLabel is the label index for the new grown voxel
 *  @param uStepTh is the average step intensity increase
 *  @param uTotalTh is the total intensity increase along the probe
 *  @param return true if the probe should go
 */
bool vxCRES::ProbeAnalysis( const Vector3D<int2> & startPos, const Vector3D<int2> & directionStep, 
													  const int4  & iProbeLength,      const uint1 & uNewLabel, 
														const uint2 & uStepTh,           const uint2 & uTotalTh )
{
	vxBlockVolumeIterator<uint1> itm( *m_pMask );
	vxBlockVolumeIterator<uint2> itv( *m_pVol  );

	itv.SetPos( startPos );
	register uint2 uPrevious( itv.GetVoxel() );

	std::vector<uint2> vu;   // intensity value container along the probe
	Vector3D<int2>     vi( startPos );
	uint2 uu, uTotalGain( 0 );
	register int4 i;
	bool bHitPuddle( false );

	// get intensity value along probing path, if the intensity is not increasing, STOP!
	for( i=1; i<=iProbeLength; i++ )
	{
		vi = vi+directionStep;

		itm.SetPos(vi);
		itv.SetPos(vi);
		if( itm.GetVoxel() > uNewLabel )
			break;                              // stop at the air, or tissue, or background

		uu = itv.GetVoxel();
		if( uu > uPrevious )
		{ // make sure intensity increase
			uTotalGain = uTotalGain + uu - uPrevious;
			uPrevious  = uu;
		  vu.push_back( uPrevious );

			if( !bHitPuddle && itm.GetVoxel() > 0x00 && itm.GetVoxel() < m_uMaximumNumOfPuddle )
			{
				bHitPuddle = true;
			}

			if( uPrevious > 1350 )     /// 350 HU is a hard coded threshold. It should not be sensitive.
				break;  // stop at tagged voxel
		}
		else
			break;
	} // for(i)
	
	int4 iActualLength( vu.size() );	
	if( iActualLength == 0 || uPrevious < m_uTagThLow || !bHitPuddle )
		return(false);            /// the probe never reach tagged region

	bool bToGo(false);
	// check if intensity increase enough
  if( uTotalGain/iActualLength > uStepTh && uTotalGain > uTotalTh )
	  bToGo = true;

	if( bToGo )
	{ // check if the tail of the probe inside air region
		vi = startPos-directionStep;
		itm.SetPos( vi );
		if( itm.GetVoxel() >= uNewLabel || itm.GetVoxel() < m_uMaximumNumOfPuddle ) 
			bToGo = false;
	} // if

	return(bToGo);
} // ProbAnalysis()


/**
 *  reverse intensity for the contrast-enhanced residual region
 *
 *  @param vUBIntensityOfPuddle is the container of feature intensity for all contrast-enhanced region
 */
void vxCRES::IntensityReversion( std::vector<uint2> & vUBIntensityOfPuddle )
{
  //===============================================================
  // TA .           The drawing shows intensity reversion transform
  //    |\        
  //    | \         TA: tissue average
  //    |  \        AA: air average
  //    |   \       TH: tissue highest
  //    |    \      RA: residue average
  //    |     \
  // AA -------*    Y = AA + (TA-AA)/(RA-TH)*(RA-X) 
  //    TH      RA
  //===============================================================

  /// critical thresholds for EC
  const uint2 uTissueAverage( 1000 );      ///     0 HU
  const uint2 uAirAverage   (  140 );      ///  -860 HU
  std::vector<float> vfScale;              /// ((uTissueAverage-uAirAverage)/(uResidueAverage-uTissueHighest));
	/// container for the pure stool intensity lower bound of puddles
	std::vector<uint2> vuLBintensity;

	const uint1   n( vUBIntensityOfPuddle.size() );
	/// parameter for puddle self-adaptive
	const float4 fs( static_cast<float4>(1250-m_uTissueHigh) / static_cast<float4>(1900-m_uTagThLow) );
	const float4 ft( 1250.0F-fs*1900.0F );

	register int4 i;
	/// computing the parameters of reversion transform
  for( i=0; i<(int4) n; i++ )
	{
		vuLBintensity.push_back( static_cast<uint2>(static_cast<float4>(vUBIntensityOfPuddle[i])*fs + ft) );

		vfScale.push_back( static_cast<float4>(uTissueAverage-uAirAverage) /
		                   static_cast<float4>(vUBIntensityOfPuddle[i]-vuLBintensity[i]) );
	} // for(i)

  vxBlockVolumeIterator<uint2> itv(*m_pVol);
  vxBlockVolumeIterator<uint1> itm(*m_pMask);

	register uint2 uv;
	// intensity reversion and mask label cleanning up
  for( itv.SetPos(0,0,0), itm.SetPos(0,0,0); itv.IsNotAtEnd();  
		   itv.NextBlockZYX(), itm.NextBlockZYX() )
  {
    for( ; itv.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
	  {
		  if( itm.GetVoxel() > 0x00 && itm.GetVoxel() <= n )
			{
		    i = static_cast<int4>(itm.GetVoxel())-1;
				uv = itv.GetVoxel();
		    if( uv >= vUBIntensityOfPuddle[i] )
		      itv.SetVoxel( uAirAverage );

		    else if( uv > vuLBintensity[i] )
		      itv.SetVoxel( uAirAverage + static_cast<uint2>(vfScale[i]*static_cast<float4>(vUBIntensityOfPuddle[i]-uv)) );

				else if( uv > m_uTissueHigh )
					itv.SetVoxel( uTissueAverage );

				itm.SetVoxel(m_uATBlabel);
			} // if
			else if( itm.GetVoxel() == m_uATBlabel )
			{
				itv.SetVoxel( uAirAverage );
			}
			else if( itm.GetVoxel() < m_uBGlabel )
				itm.SetVoxel(0x00);
		} // for inside block
	} // for all blocks

  // record cleansing indicator in the header of the mask
  VarMap map(m_pMask->GetHeader().GetVariableVolumeHeader());
  map.SetHdrVar("IsEcleansed", "1", "Indicator to EC");
  m_pMask->GetHeader().SetVariableVolumeHeader(map);

	const int4 iX(m_pVol->GetHeader().GetVolDim().m_x-3);
	const int4 iY(m_pVol->GetHeader().GetVolDim().m_y-3);
	const int4 iZ(m_pVol->GetHeader().GetVolDim().m_z-3);

	register Vector3D<int2> vPos;
	std::vector< Vector3D<int2> > vvpp;
	// try to remove floating debries in the lumen
  for( itv.SetPos(0,0,0), itm.SetPos(0,0,0); itv.IsNotAtEnd();  
		   itv.NextBlockZYX(), itm.NextBlockZYX() )
  {
    for( ; itv.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
	  {
		  if( itm.GetVoxel() == m_uATBlabel && itv.GetVoxel() >= 1200 )
			{
				vPos = itm.GetPos();
				vvpp.push_back(vPos);
			}
		  else if( itm.GetVoxel() == m_uATBlabel && itv.GetVoxel() < 700 )
			{
				vPos = itm.GetPos();
				if( vPos.m_x > 2 && vPos.m_x < iX && 
					  vPos.m_y > 2 && vPos.m_y < iY &&
						vPos.m_z > 2 && vPos.m_z < iZ &&
					  itv.PeekRel( 1, 0, 0) < 700 && itv.PeekRel( 2, 0, 0) < 700 &&
					  itv.PeekRel(-1, 0, 0) < 700 && itv.PeekRel(-2, 0, 0) < 700 &&
					  itv.PeekRel( 0, 1, 0) < 700 && itv.PeekRel( 0, 2, 0) < 700 &&
					  itv.PeekRel( 0,-1, 0) < 700 && itv.PeekRel( 0,-2, 0) < 700 &&
					  itv.PeekRel( 0, 0, 1) < 700 && itv.PeekRel( 0, 0,-1) < 700 )
				{
					vvpp.push_back(vPos);
				}
			} // if
		} // for
	} // for

	if( vvpp.size() > 0 )
	{
		for( i=vvpp.size()-1; i>=0; i-- )
		{
			itv.SetPos(vvpp[i]);
			itv.SetVoxel( uAirAverage );
		}
	}

  Mutex peakMemoryUsageMutex("555E2EE4-FFCD-43af-9A59-CA9B59704B05");
  try
  { // Mutex for potential memory overloaded
    peakMemoryUsageMutex.Lock();

		// initialize a temporary volume for median filtering
    vxBlockVolume<uint2> *pvTmp = new vxBlockVolume<uint2>(m_pVol->GetHeader());
		vxBlockVolumeIterator<uint2> itt(*pvTmp);
    vxBlockVolumeIteratorNeighbors<uint2> it18(*pvTmp, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18);

		// copy volume to temporary volume
    for( itt.SetPos(0,0,0), itv.SetPos(0,0,0); itv.IsNotAtEnd();  
		     itt.NextBlockZYX(), itv.NextBlockZYX() )
    {
      for( ; itv.IsNotAtEndOfBlock(); itt.NextZYXinsideBlock(), itv.NextZYXinsideBlock() )
				itt.SetVoxel( itv.GetVoxel() );
		} // for all blocks

    register std::vector<uint2> vnb;
    for( i=0; i<18; i++ )
      vnb.push_back(0);

	  // median filtering the cleansed volume
    for( itv.SetPos(0,0,0), itt.SetPos(0,0,0), itm.SetPos(0,0,0); itv.IsNotAtEnd();  
		     itv.NextBlockZYX(), itt.NextBlockZYX(),itm.NextBlockZYX() )
    {
      for( ; itv.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock(), itt.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
			{
		    if( itm.GetVoxel() == m_uATBlabel )
		    {					
					for( i=0, it18.CenterAt(itm.GetPos()); !it18.IsAtEnd(); i++, ++it18 )
							vnb[i] = it18.GetVoxel();
			        
					nth_element(vnb.begin(), vnb.begin()+9 , vnb.end());
					itv.SetVoxel(vnb[9]);
		    } // if
			} // for inside volume
		} // for all blocks

    delete pvTmp;
    peakMemoryUsageMutex.Unlock(); 
  }
  catch( ... )
  {
    if( peakMemoryUsageMutex.IsLocked() )
	    peakMemoryUsageMutex.Unlock();

	  LogErr("Median filtering fail", "IntensityReversion", "vxCRES.C"); 
  }
} // IntensityReversion


#undef FILE_REVISION 

// complete revision history:
// $Log: vxCRES.C,v $
// Revision 1.12  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.11  2006/06/14 15:03:25  dongqing
// 2nd fine tune per Diane's validation
//
// Revision 1.10  2006/06/06 20:14:02  dongqing
// fix fill cavity bug for the vxSegmentFinder
//
// Revision 1.9  2006/06/02 20:14:19  dongqing
// improve CRES quality per Diane's request.
//
// Revision 1.8  2006/04/24 13:26:53  dongqing
// fix the bug for generating CDV
//
// Revision 1.7  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.6  2006/04/20 20:41:34  dongqing
// add code for removal of floating debrises
//
// Revision 1.5  2006/04/19 19:53:45  dongqing
// bug free beta version
//
// Revision 1.4  2006/04/14 16:09:20  dongqing
// fix the air seed searching bug
//
// Revision 1.3  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.2  2006/04/06 15:08:57  dongqing
// rewrite the iterator
//
// Revision 1.1  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxCRES.C,v 1.12 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxCRES.C,v 1.12 2007/03/06 16:34:01 dongqing Exp $
