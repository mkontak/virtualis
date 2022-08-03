/// $Id: vxBackgroundRemoval.C,v 1.4.2.1 2009/09/17 14:19:44 dongqing Exp $
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
#include "vxBackgroundRemoval.h"
#include "vxBlockVolumeIterator.h"
#include "Logger.h"
#include "Hazards.h"

using namespace std;
using namespace hdr;

#define FILE_REVISION "$Revision: 1.4.2.1 $"

/** 
*  Default constructor
*
*  @param pVol is the input volume
*  @param pMask is the mask for the segmention
*/
vxBackgroundRemoval::vxBackgroundRemoval( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask )
:m_pVol( pVol ), m_pMask( pMask )
{
  // background threshold in raw value = -340 HU
  m_uBackgroundTh = 660;  
  
  // label index for background
  m_uBGlabel = 0xFF;

  const Triple<float4> vfUnits( pVol->GetHeader().GetVoxelUnits() );
  const Triple<int2>   viDim  ( pVol->GetHeader().GetVolDim() );

  /// bound for scanner table removal
  m_iTableUpBound = (viDim.m_y)/2 + static_cast<int4>(50.0F/vfUnits.m_y);
} // default constructor


/** 
*  Default deconstructor
*/
vxBackgroundRemoval::~vxBackgroundRemoval()
{
} // destructor


/** 
*  Apply background removal
*
*  @return true if it is successful
*/
bool vxBackgroundRemoval::Run()
{
  LogFyi("Start background removal", "Run", "vxBackgroundRemoval.C");

  const Triple<int4> viDim(m_pVol->GetHeader().GetVolDim());

  // check the table upper bound
  if( m_iTableUpBound > viDim.m_y-5 )
  {
    LogErr("Inapproprite imaging FOV", "Run", "vxBackgroundRemoval.C");
    return(false);
  } // if

  if( !ExtractBodyRegion() )
  {
	  LogErr("Cannot extract body region", "Run", "vxBackgroundRemoval.C");
    return(false);
  }
  
  // count the number of body voxels
  int4 iNumOfBodyVoxel(0);
  vxBlockVolumeIterator<uint1> itm(*m_pMask);

  for( itm.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock() )
	  {
	    if( itm.GetVoxel() == 0x00 )
	      iNumOfBodyVoxel++;
	  } // for(in the block)
  } // for(through block)

  /// check whether the body voxels are too little.
  if( static_cast<float4>(iNumOfBodyVoxel)/static_cast<float4>(viDim.m_x*viDim.m_y) < 0.12F )
  {
    Hazards::AddHazard(Hazards::WRONG_SCAN_PROTOCOL);
	  LogFyi("Body region is smaller than usual", "Run", "vxBackgroundRemoval.C");
  }

  // store the number of body voxel for later use in the volume header
  VarMap map( m_pMask->GetHeader().GetVariableVolumeHeader() );
  map.SetHdrVar("numOfBodyVoxel", iNumOfBodyVoxel, "Number of body voxels");
  m_pMask->GetHeader().SetVariableVolumeHeader(map);

  return(true);
}  // Run


/**
 *  Extract body region slice by slice
 *
 *  @return true if success
 */
bool vxBackgroundRemoval::ExtractBodyRegion()
{
  const Triple<int2>   viDim(m_pVol->GetHeader().GetVolDim());
  const Triple<float4> vfUnits(m_pMask->GetHeader().GetVoxelUnits());
  const int4 iX(viDim.m_x);
  const int4 iY(viDim.m_y);
  const int4 iZ(viDim.m_z);
  const int4 iS(viDim.m_x*viDim.m_y);
  const int4 iZincrement = max( 1, static_cast<int4>(4.0F/vfUnits.m_z) );
  register int4 x, y, z(0), i, j, k;

  vxBlockVolumeIterator<uint2> itv(*m_pVol);
  vxBlockVolumeIterator<uint1> itm(*m_pMask);

  uint1 * puSliceMask = new uint1[iS];
  if( puSliceMask == NULL )
  {
    LogErr("Run out of memory", "ExtractBodyRegion", "vxBlockVolumeRemoval.C");
	  return(false);
  } // if

  // corner offsets. There are four corners for each slice
  const int4  iCorner1(3*iX + 3);
  const int4  iCorner2(4*iX - 4);
  const int4  iCorner3(iS - 3*iX - 4);
  const int4  iCorner4(iS - 3*iX + 3);
  const uint1 uBodyLabel(0x01);
	const uint1 u1(m_uBGlabel-1);
  const int4  iTableLowBound(iY-4);

  // processing every iZincrement slices
  try
  {
    // process slice by slice, can be speed up by process every 3 slice or so
	  while(z < iZ)
	  { 
	    itv.SetPosZ(z);
      
	    i = 0;
	    // thresholding for creating temporary body mask
	    for( y=0, itv.SetPosY(0); y<iY; y++, itv.IncY() )
	    {
	      for( x=0, itv.SetPosX(0); x<iX; x++, itv.IncX() )
		    {
		      if(itv.GetVoxel() > m_uBackgroundTh)
		        puSliceMask[i] = uBodyLabel;
		      else
		        puSliceMask[i] = 0x00;
		      i++;
		    } // for(x)
	    } // for(y)

			// label slice boundary as background
			j = iX-1;
			for( y=0; y<iY; y++ )
			{
				i = y*iX;
				puSliceMask[i]   = m_uBGlabel;
				puSliceMask[i+j] = m_uBGlabel;
			} // for(y)
			i = (y-1)*iX;
			for( x=0; x<iX; x++ )
			{
				puSliceMask[x]   = m_uBGlabel;
				puSliceMask[i+x] = m_uBGlabel;
			} // for(x)

      // grow out background from corner 1.
	    if(puSliceMask[iCorner1] == 0x00)
      { 
        GrowBGInSlice( puSliceMask, iCorner1, m_uBGlabel );
      }

      // grow out background from corner 2.
	    if(puSliceMask[iCorner2] == 0x00)
      { 
        GrowBGInSlice( puSliceMask, iCorner2, m_uBGlabel );
      }

      // grow out background from corner 3.
	    if(puSliceMask[iCorner3] == 0x00)
      {
        GrowBGInSlice( puSliceMask, iCorner3, m_uBGlabel );
      }

      // grow out background from corner 4.
	    if(puSliceMask[iCorner4] == 0x00)
      {
        GrowBGInSlice( puSliceMask, iCorner4, m_uBGlabel );
      }
 
      // double check table area
      for( i=iTableLowBound; i>m_iTableUpBound; i-- ) 
      {
        j = i*iX + 3;        // left side
        if( puSliceMask[j] == 0x00 )
        {
          GrowBGInSlice( puSliceMask, j, m_uBGlabel );
        }

        j = (i+1)*iX - 4;    // right side
        if( puSliceMask[j] == 0x00 )
        {
          GrowBGInSlice( puSliceMask, j, m_uBGlabel );
        }
      }  // for(i)

      // dilation 2 layers for background
  	  DilateBackground( puSliceMask );

      y = (iY/2+1)*iX;
			i = iX-3;
      // grow out true body region
      for( x=3; x<i; x++ )
			{
				if( puSliceMask[x+y] == uBodyLabel )
				{
					Grow2D4NB(puSliceMask, x+y, uBodyLabel, u1);
				} // if
			} // for(x)

			j = iS-iX-1;
			// remove table air region since some table has hollow space in it
			for( i=iX+1; i<j; i++ )
			{
				if( puSliceMask[i] == 0x00 )
				{ // check 4 neighbors if touch background
					if( (puSliceMask[i+1]  == m_uBGlabel)||
						  (puSliceMask[i-1]  == m_uBGlabel)||
							(puSliceMask[i+iX] == m_uBGlabel)||
							(puSliceMask[i-iX] == m_uBGlabel) )
					{
						Grow2D4NB( puSliceMask, i, 0x00, uBodyLabel, m_uBGlabel );
					} // if
				}
			} // for(i)

			j = z+iZincrement;
	    // copy back to the m_pMask
	    for( i=z, itm.SetPosZ(i); i<j; i++, itm.IncZ() )
	    { 
				if( i < iZ )
				{
					k = 0;
					for( y=0, itm.SetPosY(0); y<iY; y++, itm.IncY() )
					{
						for( x=0, itm.SetPosX(0); x<iX; x++, itm.IncX() )
						{
							if(puSliceMask[k] == m_uBGlabel)
								itm.SetVoxel( m_uBGlabel );
							else
								itm.SetVoxel( 0x00 );
							k++;
						} // for(x)
					} // for(y)
				} // if(i < iZ)
	    } // for(i)

	    z = z+iZincrement;
	  } // end of while

	  delete puSliceMask;
  }
  catch( ... )
  {
    LogErr("Slice processing fail", "ExtractBodyRegion", "vxBackgroundRemoval.C");
	  return(false);
  }

  return(true);
} // ExtractBodyRegion


/**
* Grow out air background outside the body region in 2D slice
*
* @param pSliceMask the pointer of the slice mask
* @param seedOffset is the offset of the seed for the background
* @param uNewLabel is the label index for the grown background
*/
void vxBackgroundRemoval::GrowBGInSlice( uint1 *pSliceMask, int4 seedOffset, uint1 uNewLabel )
{
  // get volume dimension.
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());
  const int4         iX(viDim.m_x);
  register int4      i;
  register uint1     u(uNewLabel);

  // queue for region growing
  std::queue<int4> viRegQueue;
  viRegQueue.push(seedOffset);
  pSliceMask[seedOffset] = u;

  // start 4-connected 2D-region grow
  while( !viRegQueue.empty() ) 
  {
    // get the offset of the right neighbor
    i = viRegQueue.front() + 1;
    // check the neighbor whether it is in background, if so, label it
    if( pSliceMask[i] == 0x00 ) 
    { // add the neighbor to the queue
      viRegQueue.push( i );
      // label the neighbor as background voxel
      pSliceMask[i] = u;
    } // if(neighbor 1)

    // get the offset of the right neighbor
    i = viRegQueue.front() - 1;
    // check the neighbor whether it is in background, if so, label it
    if( pSliceMask[i] == 0x00 ) 
    { // add the neighbor to the queue
      viRegQueue.push( i );
      // label the neighbor as background voxel
      pSliceMask[i] = u;
    } // if(neighbor 2)

    // get the offset of the right neighbor
    i = viRegQueue.front() + iX;
    // check the neighbor whether it is in background, if so, label it
    if( pSliceMask[i] == 0x00 ) 
    { // add the neighbor to the queue
      viRegQueue.push( i );
      // label the neighbor as background voxel
      pSliceMask[i] = u;
    } // if(neighbor 3)

    // get the offset of the right neighbor
    i = viRegQueue.front() - iX;
    // check the neighbor whether it is in background, if so, label it
    if( pSliceMask[i] == 0x00 ) 
    { // add the neighbor to the queue
      viRegQueue.push( i );
      // label the neighbor as background voxel
      pSliceMask[i] = u;
    } // if(neighbor 4)

    viRegQueue.pop();
  }  // end of while()  
}  // GrowBGInSlice()


/**
*  dilation 2 layers for background in 2D slice
* 
* @param puSliceMask is the pointer of the temporary slice mask
*/
void vxBackgroundRemoval::DilateBackground(uint1 * puSliceMask)
{
  // get volume dimension.
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());
  const int4 iX(viDim.m_x);
  const int4 iY(viDim.m_y);
  const int4 iS(iX*iY);

  // neighbors offsets
  const int4 viNeighbor[8] = 
  {
	  1, iX, -1, -iX, iX-1, iX+1, -iX-1, -iX+1 
  };    

  register int4 i, k;
  const uint1 u(m_uBGlabel);

  register uint1 u1(u-1);                   
  const int4  iL(iX+2);
  const int4  iU(iS-iX-2);

  // dilate the first layer
  for( i=iL; i<iU; i++) 
  { // If it is a body voxel
    if( puSliceMask[i] == u ) 
    {
      if(puSliceMask[k=i+viNeighbor[0]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[1]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[2]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[3]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[4]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[5]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[6]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
      else if(puSliceMask[k=i+viNeighbor[7]] < u) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u1;
      }
    } // if
  }  // end for(i)

  register uint1 u2(u1-1);
  // dilate the second layer
  for( i=iL; i<iU; i++ )  
  { // If it is a body voxel
    if( puSliceMask[i] == u1 ) 
    {
      if(puSliceMask[(k=i+viNeighbor[0])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[1])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[2])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[3])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[4])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[5])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[6])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
      else if(puSliceMask[(k=i+viNeighbor[7])] < u1) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u2;
      }
    } // if
  }  // end for()

  register uint1 u3(u2-1);
  // dilate the third layer
  for( i=iL; i<iU; i++ ) 
  { // Is it in the body?
    if( puSliceMask[i] == u2 ) 
    {
      if(puSliceMask[(k=i+viNeighbor[0])] < u2) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u3;
      }
      else if(puSliceMask[(k=i+viNeighbor[1])] < u2) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u3;
      }
      else if(puSliceMask[(k=i+viNeighbor[2])] < u2) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u3;
      }
      else if(puSliceMask[(k=i+viNeighbor[3])] < u2) 
      { // current voxel is a boundary voxel
		    puSliceMask[k] = u3;
      }
    } // if
  }  // end for()

  // replace the temporary label to background label
  for( i=iL; i<iU; i++ ) 
  {
    if( (puSliceMask[i] >= u3)&&(puSliceMask[i] <= u1) ) 
    {
      puSliceMask[i] = u;
    }
  }  // end for()
}  // DilateBackground()


/**  
*   4-neighbor region grow in 2D slice
*
*  @param puSliceMask is the pointer to the temporary slice mask
*  @param iSeed offset
*  @param uRoiLabel is the label index for the ROI
*  @param uNewLabel is the label index for the grown region
*/
void vxBackgroundRemoval::Grow2D4NB( uint1 *puSliceMask, int4 iSeed, uint1 uRoiLabel, uint1 uNewLabel )
{
  // get volume dimension.
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());
  register int4 iX(viDim.m_x);

  // neighbors offsets
	const uint1 u(uRoiLabel);
	const uint1 un(uNewLabel);

  // queue for region growing
  std::queue<int4> iRegQueue;
	iRegQueue.push(iSeed);
	puSliceMask[iSeed] = un;

  register int4 i, j;
  while( !(iRegQueue.empty()) ) 
  {
	  i = iRegQueue.front();
	  if(puSliceMask[j=i+1] == u) 
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 
	  if(puSliceMask[j=i-1] == u) 
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 
	  if(puSliceMask[j=i+iX] == u) 
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 
	  if(puSliceMask[j=i-iX] == u) 
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 

	  iRegQueue.pop();
  }  // while()
} // Grow2D4NB()


/**  
*   4-neighbor region grow in 2D slice 
*
*  @param puSliceMask is the pointer to the temporary slice mask
*  @param iSeed offset
*  @param uRoiLabel is the label index for the ROI
*  @param uRoiLabel1 is the label index for the ROI
*  @param uNewLabel is the label index for the grown region
*/
void vxBackgroundRemoval::Grow2D4NB( uint1 *puSliceMask, int4 iSeed, uint1 uRoiLabel, uint1 uRoiLabel1, uint1 uNewLabel )
{
  // get volume dimension.
  const Triple<int2> viDim(m_pVol->GetHeader().GetVolDim());
  register int4 iX(viDim.m_x);

  // neighbors offsets
	const uint1 u ( uRoiLabel  );
	const uint1 u1( uRoiLabel1 );
	const uint1 un( uNewLabel  );

  // queue for region growing
  std::queue<int4> iRegQueue;
	iRegQueue.push(iSeed);
	puSliceMask[iSeed] = un;

  register int4 i, j;
  while( !(iRegQueue.empty()) ) 
  {
	  i = iRegQueue.front();
	  if((puSliceMask[j=i+1] == u)||(puSliceMask[j] == u1)) 
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 
	  if((puSliceMask[j=i-1] == u)||(puSliceMask[j] == u1))  
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 
	  if((puSliceMask[j=i+iX] == u)||(puSliceMask[j] == u1))  
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 
	  if((puSliceMask[j=i-iX] == u)||(puSliceMask[j] == u1))  
    { 
		  iRegQueue.push(j);
		  puSliceMask[j] = un;
    } 

	  iRegQueue.pop();
  }  // while()
} // Grow2D4NB()


#undef FILE_REVISION 

// complete revision history:
// $Log: vxBackgroundRemoval.C,v $
// Revision 1.4.2.1  2009/09/17 14:19:44  dongqing
// add checking to make sure the Zincrement is greater than 0
//
// Revision 1.4  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.3  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.2  2006/03/14 21:22:45  dongqing
// 1st step tune up
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
// Revision 1.0  2006/01/31
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBackgroundRemoval.C,v 1.4.2.1 2009/09/17 14:19:44 dongqing Exp $
// $Id: vxBackgroundRemoval.C,v 1.4.2.1 2009/09/17 14:19:44 dongqing Exp $
