/// $Id: vxBoneRemoval.C,v 1.6 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBoneRemoval.C,v 1.6 2007/03/06 16:34:01 dongqing Exp $
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

/**  
*  Removing bone region
*/

#include "stdafx.h"
#include "vxBoneRemoval.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "Logger.h"

using namespace std;
using namespace hdr;

#define FILE_REVISION "$Revision: 1.6 $"


/** 
 *  default constructor
 *
 *  @param pVol the input volume pointer
 *  @param pMask the segmentation mask pointer
*/
vxBoneRemoval::vxBoneRemoval( vxBlockVolume<uint2> * pVol, vxBlockVolume<uint1> *pMask )
:m_pVol( pVol ), m_pMask( pMask )
{
  m_uBGlabel = 0xFF;
} // default constructor


/** 
* destructor
*/
vxBoneRemoval::~vxBoneRemoval()
{
}


/** 
*  apply the bone removal procedure
*
*  @return true if bone region is found
*/
bool vxBoneRemoval::Run()
{ 
  LogFyi("Start removing bone region", "Run", "vxBoneRemoval.C");

  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());

  vxBlockVolumeIterator<uint2> itv(*m_pVol);
  vxBlockVolumeIterator<uint1> itm(*m_pMask);

  try
  {
	  const int4 iX(viDim.m_x);
	  const int4 iY(viDim.m_y);
	  const int4 iZ(viDim.m_z);
	  const uint2 uThHigh  (1350);    // 300 HU hard coded threshold
	  const uint2 uThBone  (1250);    // 250 HU hard coded threshold
	  const uint2 uThPuddle(1210);    // 210 HU hard coded threshold

	  const Triple<float4> vfUnits(m_pVol->GetHeader().GetVoxelUnits());
	  vxBlockVolumeIterator<uint2> itvv(*m_pVol);

	  // bounds for tagged puddle seed finding
	  int4 iPB(static_cast<int4>(4.8F/vfUnits.m_x));
	  register int4 iyu(iY-iPB-1);
	  register int4 ixu(iX-iPB-1);

	  register int4 x, y, i, j;
	  itm.SetPosZ(iZ-1);
		itv.SetPosZ(iZ-1);
	  itvv.SetPosZ(iZ-1);
	  Vector3D<int2> viSeed(0,0,iZ-1);
	  register int4 ib(2*iPB+1);
    uint1 u1(0x01);
	  bool bIsSeed(true);

	  // search for all possible seed of puddle at top slice and re-label it
	  for( y=iPB+1, itm.SetPosY(iPB+1), itv.SetPosY(iPB+1); y<iyu; y++, itm.IncY(), itv.IncY() )
	  {
	    for( x=iPB+1, itm.SetPosX(iPB+1), itv.SetPosX(iPB+1); x<ixu; x++, itm.IncX(), itv.IncX() )
	    { // if it is high intensity contrast enhanced voxel
		    if( (itv.GetVoxel() > uThHigh)&&(itm.GetVoxel() == 0x00) )
		    {
		      bIsSeed = true;
		      for( j=0, itvv.SetPosY(y-iPB); j<ib; j++, itvv.IncY() )
		      {
			      for( i=0, itvv.SetPosX(x-iPB); i<ib; i++, itvv.IncX() )
			      {
			        if( itvv.GetVoxel() <= uThHigh )
			        {
				        bIsSeed = false;
				        j = 10000;
				        i = 10000;
			        } // if
		        } // for(i)
		      } // for(k)

		      if( bIsSeed )
		      { 
			      viSeed.m_x = x;
			      viSeed.m_y = y;
			      Grow3D6NBth(viSeed, 0x00, u1, uThPuddle);
		      } // if(is a seed)
		    } // if
	    } // for(x)
	  } // for(y)

		iyu = iY-4;
		ixu = iX-4;
		uint1 u2(u1+1);

	  // search all bone seed and grow out the connected region
	  for( y=4, itv.SetPosY(4), itm.SetPosY(4); y<iyu; y++, itv.IncY(), itm.IncY() )
	  {
	    for( x=4, itv.SetPosX(4), itm.SetPosX(4); x<ixu; x++, itv.IncX(), itm.IncX() )
	    { // if it is high intensity voxel
	      if( (itv.GetVoxel() > uThHigh)&&(itm.GetVoxel() == 0x00) )
	      {
		      viSeed.m_x = x;
		      viSeed.m_y = y;
		      Grow3D6NBth(viSeed, 0x00, u2, uThBone);
	      } // if
	    } // for(x)
	  } // for(y)

	  uint1 u3(u2+1);
	  // one layer dilation for the found bone region
	  DilateBone(u2, u3);

	  // clean up label index
	  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
	  {
	    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
	    {
		    if( (itm.GetVoxel() >= u2)&&(itm.GetVoxel() <= u3) )
			    itm.SetVoxel(m_uBGlabel);
		    else if( itm.GetVoxel() < m_uBGlabel )
			    itm.SetVoxel(0x00);
	    } // for(inside the block)
	  } // for(all blocks)  
  }
  catch( ... )
  {
    LogErr("Exception throw while removing bone", "Run", "vxBoneRemoval.C");
	  return(false);
  }

  return (true);
} //Run() 
 

/**
 *  Thresholdting region grow in 6-neighbors 
 *
 *  @param viSeed is the seed of ROI
 *  @param uROIlabel is the old label index 
 *  @param uNewLabel is the new label index for the to be grown ROI
 *  @param uThreshold is the threshold
 */
void vxBoneRemoval::Grow3D6NBth( Vector3D<int2> viSeed, uint1 uROIlabel, uint1 uNewLabel, uint2 uThreshold )
{
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());

  vxBlockVolumeIterator<uint1> itm(*m_pMask);
  vxBlockVolumeIterator<uint2> itv(*m_pVol);

  const int4  iX(viDim.m_x);
  const int4  iY(viDim.m_y);
  const int4  iZ(viDim.m_z);
  const uint1 un(uNewLabel);
  const uint1 uo(uROIlabel);
  const uint2 uv(uThreshold);

  // queue for region growing
  register Vector3D<int2> vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
  itm.SetVoxel(un);

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();

	  vi.m_x = vi.m_x+1;
		if( vi.m_x < iX )
		{
	    itm.SetPos(vi);
			itv.SetPos(vi);
      if( (itm.GetVoxel() == uo)&&(itv.GetVoxel() > uv) ) 
      { // add the neighbor to the queue
        viRegQueue.push( vi );
        itm.SetVoxel(un);
      } // if
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( vi.m_y < iY )
		{
	    itm.SetPos(vi);
			itv.SetPos(vi);
      if( (itm.GetVoxel() == uo)&&(itv.GetVoxel() > uv) ) 
      { // add the neighbor to the queue
        viRegQueue.push( vi );
        itm.SetVoxel(un);
      } // if
		} // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_x > 0 )
		{
	    itm.SetPos(vi);
			itv.SetPos(vi);
      if( (itm.GetVoxel() == uo)&&(itv.GetVoxel() > uv) ) 
      { // add the neighbor to the queue
        viRegQueue.push( vi );
        itm.SetVoxel(un);
      } // if
		} // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_y > 0 )
		{
	    itm.SetPos(vi);
			itv.SetPos(vi);
      if( (itm.GetVoxel() == uo)&&(itv.GetVoxel() > uv) ) 
      { // add the neighbor to the queue
        viRegQueue.push( vi );
        itm.SetVoxel(un);
      } // if
		} // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( vi.m_z > 0 )
		{
	    itm.SetPos(vi);
			itv.SetPos(vi);
      if( (itm.GetVoxel() == uo)&&(itv.GetVoxel() > uv) ) 
      { // add the neighbor to the queue
        viRegQueue.push( vi );
        itm.SetVoxel(un);
      } // if
		} // if

	  vi.m_z = vi.m_z+2;
		if( vi.m_z < iZ )
		{
	    itm.SetPos(vi);
			itv.SetPos(vi);
      if( (itm.GetVoxel() == uo)&&(itv.GetVoxel() > uv) ) 
      { // add the neighbor to the queue
        viRegQueue.push( vi );
        itm.SetVoxel(un);
      } // if
		} // if

	  viRegQueue.pop();
  }  // end of while()  
} // Grow3D6NBth


/** 
 *  26-neighbor dilation for one layer
 *
 *  @param uROIlabel is the label index of the ROI
 *  @param uNewLabel is the label index of the new layer
 */
void vxBoneRemoval::DilateBone( uint1 & uROIlabel, uint1 & uNewLabel )
{
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());

  vxBlockVolumeIterator<uint1> itm(*m_pMask);
  vxBlockVolumeIteratorNeighbors<uint1> itnb( *m_pMask, (Vector3D<int4>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26);

  register uint1 un(uNewLabel);
  register uint1 uo(uROIlabel);

  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
			if( itm.GetVoxel() == uo )
			{
				for( itnb.CenterAt(itm.GetPos()); !itnb.IsAtEnd(); ++itnb )
				{
					if( itnb.GetVoxel() < uo )
						itnb.SetVoxel(un);
				} // for neighborhood
			}
		} // for inside block
	} // for all blocks			

	register uint1 uu(un);
	un++;

  vxBlockVolumeIteratorNeighbors<uint1> it6( *m_pMask, (Vector3D<int4>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06);
  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
			if( itm.GetVoxel() == uu )
			{
				for( it6.CenterAt(itm.GetPos()); !it6.IsAtEnd(); ++it6 )
				{
					if( it6.GetVoxel() < uo )
						it6.SetVoxel(un);
				} // for neighborhood
			}
		} // for inside block
	} // for all blocks			

	uNewLabel = un;
} // DilateBone


#undef FILE_REVISION 


// complete revision history:
// $Log: vxBoneRemoval.C,v $
// Revision 1.6  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.5  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.4  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.3  2006/03/15 19:03:51  dongqing
// speed up by optimizing iterators
//
// Revision 1.2  2006/02/07 20:51:11  dongqing
// add in intensity flipping
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
// Revision 1.0  2006/01/31  dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBoneRemoval.C,v 1.6 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxBoneRemoval.C,v 1.6 2007/03/06 16:34:01 dongqing Exp $
