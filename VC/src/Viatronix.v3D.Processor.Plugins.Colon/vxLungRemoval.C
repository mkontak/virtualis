/// $Id: vxLungRemoval.C,v 1.8 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxLungRemoval.C,v 1.8 2007/03/06 16:34:01 dongqing Exp $
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

/**  Removing bone region
*/

#include "stdafx.h"
#include "vxLungRemoval.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "Logger.h"

using namespace std;
using namespace hdr;

#define FILE_REVISION "$Revision: 1.8 $"


/** 
 *  default constructor
 *
 *  @param pVol the pointer to the input volume
 *  @param pMask the pointer to the segmentation mask
*/
vxLungRemoval::vxLungRemoval( vxBlockVolume<uint2> * pVol, vxBlockVolume<uint1> *pMask )
:m_pVol( pVol ), m_pMask( pMask )
{
  m_uBGLabel   = 0xFF;
  m_uAirThHigh = 240;          /// -760 HU
  m_uAirThLow  = 100;          /// -900 HU
  m_fZBratio   = (1.0F/3.0F);
} // default constructor


/** 
* destructor
*/
vxLungRemoval::~vxLungRemoval()
{
}


/** 
*  apply the lung removal procedure
*
*  @return bool If bone region is found, it is true, otherwise false.
*/
bool vxLungRemoval::Run()
{ 
  LogFyi("Start removing lung region", "Run", "vxLungRemoval.C");

  vxBlockVolumeIterator<uint2> itv(*m_pVol);
  vxBlockVolumeIterator<uint1> itm(*m_pMask);

  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());
  register int4 iX (viDim.m_x-20);
  register int4 iY (viDim.m_y-20);
  register int4 iZ (viDim.m_z   );
  register int4 x, y;

	try
	{
	  uint1 uAir(0x01);
	  int4  iNumOfAirLow, iNumOfAirHigh;
	  Vector3D<int2>    viPos;
		std::vector<int4> iLow;
		std::vector<int4> iHigh;

    // get air seed in the top slice and grow out the air region
    itv.SetPosZ(iZ-2), itm.SetPosZ(iZ-2);
    for( y=20, itv.SetPosY(20), itm.SetPosY(20); y<iY; y++, itv.IncY(), itm.IncY() )
	  {
      for( x=20, itv.SetPosX(20), itm.SetPosX(20); x<iX; x++, itv.IncX(), itm.IncX() )
	    { 
	      if( itv.GetVoxel() < m_uAirThHigh && itm.GetVoxel() == 0x00 )
		    { // an air seed was found
		      iNumOfAirLow  = 0;
				  iNumOfAirHigh = 0;
				  viPos = itv.GetPos();

				  GrowAirRegion( viPos, iNumOfAirLow, iNumOfAirHigh, uAir );

					// iterator synchronization
					itm.SetPos(itm.GetPos());
          itm.SetPosZ(iZ-2);
		    
					uAir++;
					iLow.push_back(iNumOfAirLow);
					iHigh.push_back(iNumOfAirHigh);

          if( uAir > m_uBGLabel-10 )
          { // this is too fragment, quit the procedure
            x = iX;
            y = iY;
          } // if
	      } // if(found air seed)
	    } // for(x)
    } // for(y)

		uAir = 0x01;
		const int4 n(iLow.size());
    itm.SetPosZ(iZ-2);
		// check whether it is a true lung region, still working in the top slice
		for( int4 i=0; i<n; i++ )
		{
      for( y=20, itm.SetPosY(20); y<iY; y++, itm.IncY() )
	    {
        for( x=20, itm.SetPosX(20); x<iX; x++, itm.IncX() )
	      { 
	        if( itm.GetVoxel() == uAir )
		      { // an air seed was found
		        if( static_cast<float4>(iLow[i])/static_cast<float4>(iHigh[i]) > 0.7F )  /// hard coded threshold for 60%
				    { // This should be a colon region. Don't label it as lung region
					    Grow3D6NB(viPos, uAir, 0x00);

              // synchronizing iterator
              itm.SetPos(itm.GetPos());
              itm.SetPosZ(iZ-2);
		        } // if

						uAir++;

            x = iX;
            y = iY;
	        } //if()
	      } // for(x)
      } // for(y)
		} // for(i)

    LogFyi("Finish air region grow", "Run", "vxLungRemoval.C");

	  // check if colon/lung touches
	  itm.SetPosZ(viDim.m_z-static_cast<int4>(static_cast<float4>(viDim.m_z)*(m_fZBratio)));
	  iX = viDim.m_x-3;
	  iY = viDim.m_y-3;
    for( y=3, itm.SetPosY(3); y<iY; y++, itm.IncY() )
	  {
      for( x=3, itm.SetPosX(3); x<iX; x++, itm.IncX() )
	    { 
				uAir = itm.GetVoxel();

	      if( uAir > 0x00 && uAir < m_uBGLabel )
		    { // the lung go too deep in the abdomen to be a lung
		      viPos = itm.GetPos();
          Grow3D6NB(viPos, uAir, 0x00);

          itm.SetPos( itm.GetPos() );
          itm.SetPosZ(viDim.m_z-static_cast<int4>(static_cast<float4>(viDim.m_z)*(m_fZBratio)));
	      } // if(found air)
	    } // for(x)
    } // for(y)

    LogFyi("Finish checking colon/lung touching", "Run", "vxLungRemoval.C");

    // clean up label index to label lung as background
    for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
    {
      for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
	    {
	      if( itm.GetVoxel() > 0x00 && itm.GetVoxel() < m_uBGLabel )
	        itm.SetVoxel(m_uBGLabel);
	    } // for(in the block)
    } // for(through block)  

		Dilation();
  }
	catch( ... )
	{
		LogErr("Fail labeling lung region", "Run", "vxLungRemoval.C");
		return(false);
	}
  return (true);
} //Run() 
 

/**
 *  Grow out air region 
 *
 *  @param viSeed 
 *  @param iNumOfAirLow the number of voxel having intensity lower than the m_uAirThLow
 *  @param iNumOfAirHigh the number of voxel having intensity lower than the m_uAirThHigh
 *  @param uNewLabel
 */
void vxLungRemoval::GrowAirRegion( Vector3D<int2> & viSeed, int4 & iNumOfAirLow, int4 & iNumOfAirHigh, uint1 uNewLabel )
{
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());

  vxBlockVolumeIterator<uint1> itm(*m_pMask);
  vxBlockVolumeIterator<uint2> itv(*m_pVol);

  const int4  iX(viDim.m_x);
  const int4  iY(viDim.m_y);
  const int4  iZ(viDim.m_z);
  const uint1 un(uNewLabel);
  register int4  iCount(0);
	register int4  iLow(0);

  // queue for region growing
  register Vector3D<int2>      vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	if( itm.GetVoxel() != 0x00 )
		return;
  itm.SetVoxel(un);

  iCount++;
	itv.SetPos(vi);
	if( itv.GetVoxel() < m_uAirThLow )
		iLow++;

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
		itm.SetPos(vi);
		itv.SetPos(vi);

	  vi.m_x = vi.m_x+1;
		if( vi.m_x < iX && itm.PeekRel(1,0,0) == 0x00 && itv.PeekRel(1,0,0) < m_uAirThHigh ) 
    { // add the neighbor to the queue
	    itm.IncX();
      viRegQueue.push( vi );
      itm.SetVoxel(un);
	    itm.DecX();

		  iCount++;

			if( itv.PeekRel(1,0,0) < m_uAirThLow )
				iLow++;
    } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( vi.m_y < iY && itm.PeekRel(0,1,0) == 0x00 && itv.PeekRel(0,1,0) < m_uAirThHigh ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.IncY();
      itm.SetVoxel(un);
      itm.DecY();
			iCount++;

			if( itv.PeekRel(0,1,0) < m_uAirThLow )
			  iLow++;
    } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_x > 0 && itm.PeekRel(-1,0,0) == 0x00 && itv.PeekRel(-1,0,0) < m_uAirThHigh ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.DecX();
      itm.SetVoxel(un);
      itm.IncX();
		  iCount++;

		  if( itv.PeekRel(-1,0,0) < m_uAirThLow )
					iLow++;
    } // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_y > 0 && itm.PeekRel(0,-1,0) == 0x00 && itv.PeekRel(0,-1,0) < m_uAirThHigh ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.DecY();
      itm.SetVoxel(un);
      itm.IncY();
		  iCount++;

		  if( itv.PeekRel(0,-1,0) < m_uAirThLow )
			  iLow++;
    } // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( vi.m_z > 0 && itm.PeekRel(0,0,-1) == 0x00 && itv.PeekRel(0,0,-1) < m_uAirThHigh ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.DecZ();
      itm.SetVoxel(un);
      itm.IncZ();
		  iCount++;

		  if( itv.PeekRel(0,0,-1) < m_uAirThLow )
			  iLow++;
    } // if

	  vi.m_z = vi.m_z+2;
		if( vi.m_z < iZ && itm.PeekRel(0,0,1) == 0x00 && itv.PeekRel(0,0,1) < m_uAirThHigh ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.IncZ();
      itm.SetVoxel(un);
      itm.DecZ();
		  iCount++;

		  if( itv.PeekRel(0,0,1) < m_uAirThLow )
			  iLow++;
    } // if

	  viRegQueue.pop();
  }  // end of while() 

  // return the statistics
	iNumOfAirLow  = iLow;
	iNumOfAirHigh = iCount;

} // GrowAirRegion

 
/**
 *  3D region grow in 6-neighbors 
 *
 *  @param viSeed 
 *  @param uROILabel
 *  @param uNewLabel
 */
void vxLungRemoval::Grow3D6NB( Vector3D<int2> & viSeed, uint1 uROILabel, uint1 uNewLabel )
{
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());

  vxBlockVolumeIterator<uint1> itm(*m_pMask);

  const int4  iX(viDim.m_x);
  const int4  iY(viDim.m_y);
  const int4  iZ(viDim.m_z);
  const uint1 un(uNewLabel);
  const uint1 uo(uROILabel);

  // queue for region growing
  register Vector3D<int2> vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  itm.SetPos(vi);
	if( itm.GetVoxel() != uo )
		return;
  itm.SetVoxel(un);

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
		itm.SetPos(vi);

	  vi.m_x = vi.m_x+1;
		if( vi.m_x < iX && itm.PeekRel(1,0,0) == uo ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.IncX();
      itm.SetVoxel(un);
			itm.DecX();
    } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
		if( vi.m_y < iY && itm.PeekRel(0,1,0) == uo ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.IncY();
      itm.SetVoxel(un);
			itm.DecY();
    } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_x > 0 && itm.PeekRel(-1,0,0) == uo ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.DecX();
      itm.SetVoxel(un);
			itm.IncX();
    } // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
		if( vi.m_y > 0 && itm.PeekRel(0,-1,0) == uo ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.DecY();
      itm.SetVoxel(un);
			itm.IncY();
    } // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
		if( vi.m_z > 0 && itm.PeekRel(0,0,-1) == uo ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.DecZ();
      itm.SetVoxel(un);
			itm.IncZ();
    } // if

	  vi.m_z = vi.m_z+2;
		if( vi.m_z < iZ && itm.PeekRel(0,0,1) == uo ) 
    { // add the neighbor to the queue
      viRegQueue.push( vi );
	    itm.IncZ();
      itm.SetVoxel(un);
			itm.DecZ();
    } // if

	  viRegQueue.pop();
  }  // end of while() 

} // Grow3D6NB


/**
 *  Dilation one 18-connected layer
 */
void vxLungRemoval::Dilation()
{
	const uint1 u  (m_uBGLabel-1);
	const uint1 ubg(m_uBGLabel  );

	vxBlockVolumeIterator<uint1>          itm (*m_pMask);
	vxBlockVolumeIteratorNeighbors<uint1> it18(*m_pMask, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18);

  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
      if( itm.GetVoxel() == ubg )
		  { // check its neighbor
        for( it18.CenterAt(itm.GetPos()); !it18.IsAtEnd(); ++it18 )
		    {
		      if( it18.GetVoxel() == 0x00 )
						it18.SetVoxel(u);
				} // for 18 neighbors
			} // if
		} // for inside block
	} // for all blocks

  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
		{
      if( itm.GetVoxel() == u )
		  { // check its neighbor
        itm.SetVoxel(ubg);
			} // if
		} // for inside block
	} // for all blocks

} // Dilation()


#undef FILE_REVISION 

// complete revision history:
// $Log: vxLungRemoval.C,v $
// Revision 1.8  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.7  2006/10/12 15:25:52  dongqing
// Fix nonPosSetting bug in the Region Grow
//
// Revision 1.6  2006/10/05 15:22:11  dongqing
// Fix the bug for memory overflow due to wrong setting of slice location
//
// Revision 1.5  2006/04/14 16:08:52  dongqing
// fix the missing check of AirLabel which was updated for each segment
//
// Revision 1.4  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.3  2006/03/15 16:15:18  dongqing
// best algorith so far
//
// Revision 1.2  2006/02/07 20:51:11  dongqing
// add in intensity flipping
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
// Revision 1.0  2006/01/31  dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxLungRemoval.C,v 1.8 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxLungRemoval.C,v 1.8 2007/03/06 16:34:01 dongqing Exp $
