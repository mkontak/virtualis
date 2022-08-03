/// $Id: vxSegmentFinder.C,v 1.15.2.4 2010/02/12 20:40:05 dongqing Exp $
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
#include "vxSegmentFinder.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "Logger.h"
#include "Hazards.h"

using namespace std;
using namespace hdr;

#define FILE_REVISION "$Revision: 1.15.2.4 $"

/** 
*  Default constructor
*
*  @param pVol the pointer to the CT volume (may or may not be cleansed
*  @param pMask the pointer to the EC mask
*  @param pIns the pointer to the INS volume
*  @param sourceFilePath
*  @param destFilePath
*/
vxSegmentFinder::vxSegmentFinder( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask, vxBlockVolume<uint1> *pIns,
		                               const std::string & sourceFilePath, const std::string destFilePath )
:m_pVol(pVol), m_pMask(pMask), m_pIns(pIns), m_sourceFilePath(sourceFilePath), m_destFilePath(destFilePath)
{
  m_uBGlabel  = 0xFF;
  m_uTagLabel = 0xFE;

  // setting the thresholds
  m_uAirTh = 240;  // -760 HU

	Triple<float4> vfUnits( m_pVol->GetHeader().GetVoxelUnits() );
  m_iSmallestAirLumenVolume = static_cast<int4>(8.0F/vfUnits.m_x) *
                              static_cast<int4>(8.0F/vfUnits.m_y) * static_cast<int4>(6.3F/vfUnits.m_z);
} // constructor


/** 
*  Default deconstructor
*/
vxSegmentFinder::~vxSegmentFinder()
{
} // deconstructor


/** 
*  extract colonic segments
*
*  @return true if it is successful
*/
bool vxSegmentFinder::Run()
{
  LogFyi("Start INS procedure", "Run", "vxSegmentFinder.C");
  
  try
  { 
	  vxBlockVolumeIterator<uint2> itv ( *m_pVol  );
	  vxBlockVolumeIterator<uint2> itvv( *m_pVol  );
	  vxBlockVolumeIterator<uint1> itm ( *m_pMask );
	  vxBlockVolumeIterator<uint1> iti ( *m_pIns  );

		/// label background for correctly region growing
    for( iti.SetPos(0,0,0), itm.SetPos(0,0,0); itm.IsNotAtEnd(); iti.NextBlockZYX(), itm.NextBlockZYX() )
    {
      for( ; itm.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
			{
				if( itm.GetVoxel() == m_uBGlabel )
					iti.SetVoxel( m_uBGlabel );
			}
	  } // for

	  const    Triple<int2> viDim( m_pVol->GetHeader().GetVolDim() );
    register int4         iX( viDim.m_x );
    register int4         iY( viDim.m_y );
    register int4         iZ( viDim.m_z );

	  const uint2    uAirThSeed           ( 141 );              //  140 = -860 HU
		const uint1    ur1                  ( m_uTagLabel-1 );    //  label index for temporary usage
	  const uint1    uMaxNumOfInitialSeg  ( 100  );             /// hard coded threshold, it should not be sensitive
	  register uint1 uCurrentSegLabel     ( 0x01 );
	
	  register int4 x, y, z, i, j, k;

		std::vector<int4> viNumVoxelInSeg;
	  register Vector3D<int2> viSeed ( 0,0,0 );
	  register bool bIsSeed;
	  iZ = iZ-static_cast<int4>(2.5F*m_pVol->GetHeader().GetVoxelUnits().m_z);        /// 2.5 cm to avoid lung region
	  iY = iY-30;                                 /// 2.5 cm to avoid table region for small imaging FOV
	  iX = iX-4;

    // find air seed for colonic segments from rectum to splanic flexture
    for( z=3, itv.SetPosZ(3), itm.SetPosZ(3), iti.SetPosZ(3); z<iZ; z++, itv.IncZ(), itm.IncZ(), iti.IncZ() )
	  { 
      for( y=4, itv.SetPosY(4), itm.SetPosY(4), iti.SetPosY(4); y<iY; y++, itv.IncY(), itm.IncY(), iti.IncY() )
	    {
        for( x=4, itv.SetPosX(4), itm.SetPosX(4), iti.SetPosX(4); x<iX; x++, itv.IncX(), itm.IncX(), iti.IncX() )
	      { 
	        if( itm.GetVoxel() == 0x00 && itv.GetVoxel() < uAirThSeed && iti.GetVoxel() == 0x00 )
		      { // it is an air voxel
		        bIsSeed = true;
			      // check the air packet around it. The 3D air packet seems not necessary
			      for( j=-3; j<4; j++ )
			      {
			        for( k=-3; k<4; k++ )
				        if( itv.PeekRel(k,j,0) > uAirThSeed )
				        {
                  bIsSeed = false;
			            j = 20;
				          break;  // quit the air packet loop
				        } // if
			      } // for(j)

			      if( bIsSeed )
			      {
			        viSeed.m_x = x;
			        viSeed.m_y = y;
			        viSeed.m_z = z;
							i = 0;
			        i = GrowOutAirSegment( viSeed, uCurrentSegLabel );   /// air threshold is m_uAirTh = -760 HU

							// sychronization of iterator location after another iterator split the block
							iti.SetPos( iti.GetPos() );

			        if( i >= m_iSmallestAirLumenVolume ) 
			        {
			          uCurrentSegLabel++;             /// increase label index for next segment
								viNumVoxelInSeg.push_back( i );

				        if( uCurrentSegLabel >= uMaxNumOfInitialSeg )
				        {
				          LogFyi("Too many initial air segments", "Run", "vxSegmentFinder.C");
				          x = 2000;
				          y = 2000;
				          z = 2000;  // quit all loop
				        }
			        } // if(it is a real seed)
			        else
			        { // too small to keep it
								int4 nnn = Grow3D6NB( m_pIns, viSeed, uCurrentSegLabel, ur1 );
								if( i != nnn )
								  LogErr("Removal of small segment incorrect", "Run", "vxSegmentFinder.C");
			        }
			      } // if( it is a true air seed )
		      } // if
	      } // for(x)
	    } // for(y)
    } // for(z)
 
		uCurrentSegLabel--;

		// remove temporary label, still keep the background label
    for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
    {
      for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
			{
				if( iti.GetVoxel() == ur1 )
					iti.SetVoxel(0x00);
			} // for inside block
	  } // for all blocks

	  if( uCurrentSegLabel == 0x00 )
	  {
	    LogErr("No colonic lumen was found", "Run", "vxSegmentFinder.C");
	    return(false);
	  }

	  // if the segments is more than 15, get rid of smaller segments
	  if( uCurrentSegLabel > 15 )
	  { 
	    LogFyi("Initial segments are more than 15", "Run", "vxSegmentFinder.C");

	    std::vector<int4>      viLabelMatchTable;
	    y = static_cast<int4>( uCurrentSegLabel );
	    for( x=1; x<=y; x++ )
	      viLabelMatchTable.push_back(x);

	    z = y;
	    y = y-1;
	    // sorting segments in descending order of the number of voxel
      for( i=0; i<y; i++ )
	    {
	      for( j=i+1; j<z; j++ )
	      {
	        if( viNumVoxelInSeg[i] < viNumVoxelInSeg[j] )
	  	    { // exchange the number of voxels
		        x = viNumVoxelInSeg[i];
		        viNumVoxelInSeg[i] = viNumVoxelInSeg[j];
		        viNumVoxelInSeg[j] = x;

		        // exchange the label index
		        x = viLabelMatchTable[i];
		        viLabelMatchTable[i] = viLabelMatchTable[j];
		        viLabelMatchTable[j] = x;
		      } // if
	      } // for(j)
	    } // for(i)

			bool bbb;
			register uint1 u;
	    // relabeling segments to get rid of smaller segments
			for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
			{
				for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
				{
					if( iti.GetVoxel() > 0x00 )
					{
						u = iti.GetVoxel();
						for( x=0; x<15; x++ )
						{
				      bbb = true;
							if( u == viLabelMatchTable[x] )
							{
								iti.SetVoxel( static_cast<uint1>(x+1) );
								bbb = false;
								break;
							}
						} // for x

						if( bbb )
							iti.SetVoxel( 0x00 );
					}
				} // for inside the block
			} // for all blocks

	    uCurrentSegLabel = static_cast<uint1>(15);
		} // if more than 15 segment

    /// *********************************************************************
    /// Need to repopulate the m_pMask at here!
    vxBlockVolume<uint1> * pTmpMask = new vxBlockVolume<uint1>( m_pIns->GetHeader() );
    if( pTmpMask == NULL )
    {
	    LogErr("No memory space for temporary mask volume", "Run", "vxSegmentFinder.C");
	    return(false);
    } //
    vxBlockVolumeIterator<uint1> itt( *pTmpMask );

		for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
		{
			for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
			{
        if( iti.GetVoxel() > 0 && iti.GetVoxel() < 16 )
        {
          itt.SetPos( iti );
          itt.SetVoxel( m_uBGlabel );
        } // if
      } // for inside the block
    } // for all blocks
    /// *********************************************************************

    /// fill cavity in the segment, the background label will be cleaned up
		FillCavity( uCurrentSegLabel );

 		/// smoothing the contour of the initial lumen segments
		/// in order to cut off segments touching
		SmoothingLumenContour( uCurrentSegLabel );

		uCurrentSegLabel = 0x01;
		register Vector3D<int2> vPos;
		std::vector< int4 > vNumSegNew;
		// regrowing all lumens
    for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
    {
      for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
			{
				if( iti.GetVoxel() == m_uBGlabel )
				{
					vPos = iti.GetPos();

					i = Grow3D6NB( m_pIns, vPos, m_uBGlabel, uCurrentSegLabel );
					  
					iti.SetPos( iti.GetPos() );

					if( i > m_iSmallestAirLumenVolume )
					{
						uCurrentSegLabel++;
						vNumSegNew.push_back( i );
					}
					else
					{
						Grow3D6NB( m_pIns, vPos, uCurrentSegLabel, 0x00 );
					  iti.SetPos( iti.GetPos() );
					}
				}
			}
	  } // for

		uCurrentSegLabel--;
		if( uCurrentSegLabel == 0x00 )
	  {
	    LogErr("No colonic lumen was found", "Run", "vxSegmentFinder.C");
	    return(false);
	  }

	  if( uCurrentSegLabel > 15 )
	  { 
	    LogFyi("Initial segments are more than 15", "Run", "vxSegmentFinder.C");

	    std::vector<int4>      viLabelMatchTable;
	    y = static_cast<int4>( uCurrentSegLabel );
	    for( x=1; x<=y; x++ )
	      viLabelMatchTable.push_back(x);

	    z = y;
	    y = y-1;
	    // sorting segments in descending order of the number of voxel
      for( i=0; i<y; i++ )
	    {
	      for( j=i+1; j<z; j++ )
	      {
	        if( vNumSegNew[i] < vNumSegNew[j] )
	  	    { // exchange the number of voxels
		        x = vNumSegNew[i];
		        vNumSegNew[i] = vNumSegNew[j];
		        vNumSegNew[j] = x;

		        // exchange the label index
		        x = viLabelMatchTable[i];
		        viLabelMatchTable[i] = viLabelMatchTable[j];
		        viLabelMatchTable[j] = x;
		      } // if
	      } // for(j)
	    } // for(i)

			bool bbb;
			register uint1 u;
	    // relabeling segments to get rid of smaller segments
			for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
			{
				for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
				{
					if( iti.GetVoxel() > 0x00 )
					{
						u = iti.GetVoxel();
						for( x=0; x<15; x++ )
						{
				      bbb = true;
							if( u == viLabelMatchTable[x] )
							{
								iti.SetVoxel( static_cast<uint1>(x+1) );
								bbb = false;
								break;
							}
						} // for x

						if( bbb )
							iti.SetVoxel( 0x00 );
					}
				} // for inside the block
			} // for all blocks

	    uCurrentSegLabel = static_cast<uint1>(15);
		} // if more than 15 segment

    /// ***********************************************************
    /// Need to recover colon segment contour here!!!
    RecoverSegmentContourLayer( iti, pTmpMask );
    /// ***********************************************************

	  // keep record of the number of segments in the header
    VarMap mapi(m_pIns->GetHeader().GetVariableVolumeHeader());
    int4 iNumberOfSegments(static_cast<int4>(uCurrentSegLabel));
    mapi.SetHdrVar("numSegments", iNumberOfSegments, "Number of colonic segments");
    m_pIns->GetHeader().SetVariableVolumeHeader(mapi);

    // get cleansing indicator in the header of the mask
    VarMap map(m_pMask->GetHeader().GetVariableVolumeHeader());
    int4 indicator(0);
    map.GetHdrVar("IsEcleansed", indicator);

    if( indicator == 1 )
    { // cleansed volume. create the CDV file

			// remove all floating debries
			for( itv.SetPos(0,0,0), iti.SetPos(0,0,0), itm.SetPos(0,0,0); iti.IsNotAtEnd();  
					itv.NextBlockZYX(), iti.NextBlockZYX(), itm.NextBlockZYX() )
			{
				for( ; iti.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock(), iti.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
					if( iti.GetVoxel() > 0x00 && itv.GetVoxel() > m_uAirTh )
					{ 
						itm.SetVoxel( m_uTagLabel );

						itv.SetVoxel( 80 );    /// hard coded threshold
					}
			} // for

      vxVolumeHeader *pbh = new vxVolumeHeader;
      pbh->Read( m_sourceFilePath+".vol" );
      int4 iBias(1000);
      pbh->GetVariableVolumeHeader().GetHdrVar("bias", iBias);

			if( iBias != 1000 )
			{
				register int2 iShift(iBias-1000);
				
				int2 iii;
				for( itv.SetPos(0,0,0), itm.SetPos(0,0,0); itm.IsNotAtEnd();  
						itv.NextBlockZYX(), itm.NextBlockZYX() )
				{
					for( ; itm.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
					{
						if( itm.GetVoxel() == m_uTagLabel )
						{
							iii = itv.GetVoxel()+iShift;
							if( iii <= 0 )
								itv.SetVoxel( 1 );
							else
								itv.SetVoxel( iii );
						} // if
						else
							itv.SetVoxel( 0 );
					} // for inside the block
				} // for all blocks
			} // if
			else
			{
				for( itv.SetPos(0,0,0), itm.SetPos(0,0,0); itm.IsNotAtEnd();  
						itv.NextBlockZYX(), itm.NextBlockZYX() )
				{
					for( ; itm.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
					{
						if( itm.GetVoxel() != m_uTagLabel )
						  itv.SetVoxel( 0 );
					} // for inside the block
				} // for all blocks
			}
    
      // add EC indicator
      VarMap map(pbh->GetVariableVolumeHeader());
      map.SetHdrVar("IsEcleansed", "1", "Indicator to EC");
      pbh->SetVariableVolumeHeader(map);
      m_pVol->SetHeader(*pbh);
      delete pbh;

		  // record EC indicator in the header of the INS volume
      map = m_pIns->GetHeader().GetVariableVolumeHeader();
      map.SetHdrVar("IsEcleansed", "1", "Indicator to EC");
      m_pIns->GetHeader().SetVariableVolumeHeader(map);
    } // if( cleansed ) end of creation of CDV file
		else
		{
      vxVolumeHeader *pbh = new vxVolumeHeader;
      pbh->Read( m_sourceFilePath+".vol" );
      // add EC indicator
      VarMap map(pbh->GetVariableVolumeHeader());
      map.SetHdrVar("IsEcleansed", "0", "Indicator to EC");
      pbh->SetVariableVolumeHeader(map);
      m_pVol->SetHeader(*pbh);

			for( itv.SetPos(0,0,0); itv.IsNotAtEnd(); itv.NextBlockZYX() )
			{
			  for( ; itv.IsNotAtEndOfBlock(); itv.NextZYXinsideBlock() )
				  itv.SetVoxel( 0 );
			} // for all blocks
		}

    /// *************************************************************************************************
    for( itm.SetPos(0,0,0), itt.SetPos(0,0,0); itt.IsNotAtEnd(); itt.NextBlockZYX(), itm.NextBlockZYX() )
		{
			for( ; itt.IsNotAtEndOfBlock(); itt.NextZYXinsideBlock(), itm.NextZYXinsideBlock() )
			{
        if( itt.GetVoxel() == 0 || itt.GetVoxel() == m_uBGlabel )
          itm.SetVoxel(0);
        else
          itm.SetVoxel( itt.GetVoxel() );
      } // for inside the block
    } // for all blocks
    
    delete pTmpMask;

    m_pMask->GetHeader().SetVariableVolumeHeader( m_pIns->GetHeader().GetVariableVolumeHeader() );
    /// **************************************************************************************************
  }
  catch( ... )
  {
    LogErr("Extraction segment fail", "Run", "vxSegmentFinder.C");
	  return(false);
  }
  
  return(true);
}  // Run



/**
 *  Grow out 3D region with 6-connected neighbor 
 *
 *  @param pMask the pointer to the mask
 *  @param viSeed 
 *  @param uROIlabel the original label for the ROI
 *  @param uNewLabel for labeling grown region
 *  @return the number of voxels that are grown
 */
int4 vxSegmentFinder::Grow3D6NB( vxBlockVolume<uint1> * pMask, const Vector3D<int2> & viSeed, 
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
 *  grow out air segment
 *
 *  @param viSeed 
 *  @param uNewLabel the label index for the grown region
 *  @return number of the voxel in the grown segment
 */
int4 vxSegmentFinder::GrowOutAirSegment( const Vector3D<int2> & viSeed, const uint1 & uNewLabel )
{
  const Triple<int4> viDim(m_pVol->GetHeader().GetVolDim());
  const int4 iX(viDim.m_x-1);
  const int4 iY(viDim.m_y-1);
  const int4 iZ(viDim.m_z-1);

  vxBlockVolumeIterator<uint1> iti(*m_pIns);
  vxBlockVolumeIterator<uint2> itv(*m_pVol);

  register uint1 un(uNewLabel);
  register uint2 uav(m_uAirTh);

  int4 iCount(0);
  // queue for region growing
  register    Vector3D<int2>   vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  iti.SetPos(vi);
	if( iti.GetVoxel() != 0x00 )
		return(iCount);
  iti.SetVoxel(un);
	iCount++;

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
		iti.SetPos(vi);
		itv.SetPos(vi);

	  vi.m_x = vi.m_x+1;
	  if( vi.m_x < iX && iti.Peek1px0py0pz() == 0x00 &&
			   itv.Peek1px0py0pz() < uav )
	  {
	    viRegQueue.push( vi );
			iti.IncX();
			iti.SetVoxel(un);
			iti.DecX();
      iCount++;
    } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
	  if( (vi.m_y < iY)&&(iti.Peek0px1py0pz() == 0x00)&&
			  (itv.Peek0px1py0pz() < uav) )
	  {
	    viRegQueue.push( vi );
			iti.IncY();
			iti.SetVoxel(un);
			iti.DecY();
      iCount++;
	  } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
	  if( (vi.m_x > 0)&&(iti.Peek1nx0py0pz() == 0x00)&&
			  (itv.Peek1nx0py0pz() < uav) )
	  {
	    viRegQueue.push( vi );
			iti.DecX();
			iti.SetVoxel(un);
			iti.IncX();
      iCount++;
	  } // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
	  if( (vi.m_y > 0)&&(iti.Peek0px1ny0pz() == 0x00)&&
			  (itv.Peek0px1ny0pz() < uav) )
	  {
	    viRegQueue.push( vi );
			iti.DecY();
			iti.SetVoxel(un);
			iti.IncY();
      iCount++;
	  } // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
	  if( (vi.m_z > 0)&&(iti.Peek0px0py1nz() == 0x00)&&
			  (itv.Peek0px0py1nz() < uav) )
	  {
	    viRegQueue.push( vi );
			iti.DecZ();
			iti.SetVoxel(un);
			iti.IncZ();
      iCount++;
	  } // if

	  vi.m_z = vi.m_z+2;
	  if( (vi.m_z < iZ)&&(iti.Peek0px0py1pz() == 0x00)&&
			  (itv.Peek0px0py1pz() < uav) )
	  {
	    viRegQueue.push( vi );
			iti.IncZ();
			iti.SetVoxel(un);
			iti.DecZ();
      iCount++;
	  } // if

	  viRegQueue.pop();
  }  // end of while() 

  return(iCount);
} // GrowOutAirSegment


/**
 *  smoothing the contour of the lumen segments
 *
 *  @param uHighestLabelIndex
 */
void vxSegmentFinder::SmoothingLumenContour( const uint1 & uHighestLabelIndex )
{
	const uint1 ut(uHighestLabelIndex+1);
		
	// 18 neighbor position 
	const Vector3D<int2> viNeighbourhood[18] =
	{
		Vector3D<int2>( 0, 1, 0), Vector3D<int2>( 0,-1, 0),
		Vector3D<int2>( 1, 0, 0), Vector3D<int2>(-1, 0, 0), 
		Vector3D<int2>( 0, 0,-1), Vector3D<int2>( 0, 0, 1),	 
		Vector3D<int2>(-1, 1, 0), Vector3D<int2>( 1, 1, 0),
		Vector3D<int2>( 1,-1, 0), Vector3D<int2>(-1,-1, 0),      
		Vector3D<int2>( 1, 0, 1), Vector3D<int2>(-1, 0, 1),
		Vector3D<int2>( 0, 1, 1), Vector3D<int2>( 0,-1, 1),      
		Vector3D<int2>( 1, 0,-1), Vector3D<int2>(-1, 0,-1),
		Vector3D<int2>( 0, 1,-1), Vector3D<int2>( 0,-1,-1)
	};

	vxBlockVolumeIterator<uint1> iti(*m_pIns);

	register int4 i;
	/// erosion 1 layer of 6-connected
  for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
		{
			if( iti.GetVoxel() > 0x00 )
			{
				for( i=0; i<18; i++ )
				{
					if( iti.PeekRel(viNeighbourhood[i]) == 0x00 )
					{
						iti.SetVoxel(ut);
						break;
					} // if
				} // for(i)
			} // if
		} // for inside block
	} // for all blocks

	const uint1 uUB( ut );
	/// clean up label
  for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
		{
			if( iti.GetVoxel() == ut )
				iti.SetVoxel( 0x00 );
			else if( iti.GetVoxel() > 0x00 )
				iti.SetVoxel( m_uBGlabel );
		} // for inside block
	} // for all blocks

} // SmoothingLumenContour


/**
 *  fill cavity within the segment
 *
 *  @param uHighestLabelIndex is the highest label index for air lumen
 */
void vxSegmentFinder::FillCavity( const uint1 & uHighestLabelIndex )
{
	const int2 iX( m_pVol->GetHeader().GetVolDim().m_x-1 );
	const int2 iY( m_pVol->GetHeader().GetVolDim().m_y-1 );
	const int2 iZ( m_pVol->GetHeader().GetVolDim().m_z-1 );

	vxBlockVolumeIterator<uint1>          iti ( *m_pIns );
  vxBlockVolumeIteratorNeighbors<uint1> it26( *m_pIns, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26 );

	const uint1 uShellLabel( uHighestLabelIndex+1 );

	register uint1 u;
	register Vector3D<int2> vPos;
	/// fill cavity segment by segment
	for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
	  {
			u = iti.GetVoxel();
		  if( u > 0x00 && u < uShellLabel )
			{
				vPos = iti.GetPos();

				for( it26.CenterAt( vPos ); !it26.IsAtEnd(); ++it26 )
				{ // check if the neighbor has a lumen voxel	
          if( it26.GetVoxel() == 0x00 && it26.IsInsideVolume() )
					{
					  int4 iCount = DetermineSegmentShell( it26.GetPos(), u, uShellLabel );
									
						// re-synchronize location after another iterator split the block
					  iti.SetPos( iti.GetPos() );

            if( iCount > 26 )
            { // fill cavity for this segment
					    GrowWithinShell( vPos, u, u+uShellLabel );
            } // if

					  // re-synchronize location after another iterator split the block
						iti.SetPos(iti.GetPos());

						break;
					} // for all seed neighbors
				} // for 26-neighbors
			} // if it is a segment voxel

		} // for inside block
  } // for all blocks

	const uint1 uTmp( uShellLabel+uHighestLabelIndex+1 );
	// clean up labels
  for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
		{
		  if( iti.GetVoxel() > uShellLabel && iti.GetVoxel() < uTmp )
				iti.SetVoxel( iti.GetVoxel()-uShellLabel );
			else
				iti.SetVoxel(0x00);
		} // for
	} // for
} // FillCavity


/**
 *  Determine segment shell
 *
 *  @param viSeed 
 *  @param uRoiLabel the label index for the grown region
 *  @param uShellLabel the label for the found shell
 *  @return number of voxels
 */
int4 vxSegmentFinder::DetermineSegmentShell( const Vector3D<int2> & viSeed, const uint1 & uRoiLabel, 
																						 const uint1 & uShellLabel )
{
  vxBlockVolumeIterator<uint1>          iti  ( *m_pIns );
  vxBlockVolumeIteratorNeighbors<uint1> it26 ( *m_pIns, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26 );
  vxBlockVolumeIteratorNeighbors<uint1> itt26( *m_pIns, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26 );
  
  // queue for region growing
  register int4 iCount(0);
  register    Vector3D<int2>   vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push( vi );
  iti.SetPos( vi );
	if( iti.GetVoxel() != 0x00 )
	  return(iCount);

	iti.SetVoxel(uShellLabel);
	iCount++;

  register Vector3D<int2> vnb;
  // 26-connected 3D-shell searching
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();

	  for( it26.CenterAt(vi); !it26.IsAtEnd(); ++it26 )
	  { // check its neighbor
      if( (it26.GetVoxel() == 0x00 || it26.GetVoxel() == m_uBGlabel) && it26.IsInsideVolume() )
	    {
	      vnb = it26.GetPos();
		    for( itt26.CenterAt(vnb); !itt26.IsAtEnd(); ++itt26 )
		    { // check neighbor's neighbor
		      if( itt26.GetVoxel() == uRoiLabel && itt26.GetVoxel() != uShellLabel && itt26.GetVoxel() != m_uBGlabel )
		      {
		        it26.SetVoxel( uShellLabel );
			      viRegQueue.push( vnb );
						iCount++;
			      break;
		      } // if( on a shell )
		    } // for( inner neighborhood )	    
	    } // if
	  } // for( 26 NB )

	  viRegQueue.pop();
  } // end of while()

	return(iCount);
} // DetermineSegmentShell


/**
 *  Grow out 3D region with 6-connected neighbor within a shell
 *
 *  @param viSeed 
 *  @param uRoilabel the original label for the ROI
 *  @param uNewLabel for labeling grown region
 */
void vxSegmentFinder::GrowWithinShell( const Vector3D<int2> & viSeed, const uint1 & uRoiLabel, 
																			 const uint1 & uNewLabel)
{
  const Triple<int4> viDim(m_pVol->GetHeader().GetVolDim());
  const int4 iX(viDim.m_x-1);
  const int4 iY(viDim.m_y-1);
  const int4 iZ(viDim.m_z-1);

  vxBlockVolumeIterator<uint1> iti(*m_pIns);

  // queue for region growing
  register    Vector3D<int2>   vi(viSeed);
  std::queue< Vector3D<int2> > viRegQueue;
  viRegQueue.push(vi);
  iti.SetPos(vi);
	if( iti.GetVoxel() != uRoiLabel )
		return;
  iti.SetVoxel(uNewLabel);

  // start 6-connected 3D-region grow
  while( !viRegQueue.empty() ) 
  {
    vi = viRegQueue.front();
		iti.SetPos(vi);

	  vi.m_x = vi.m_x+1;
	  if( vi.m_x < iX && (iti.Peek1px0py0pz() == uRoiLabel || iti.Peek1px0py0pz() == 0x00) )
	  {
	    viRegQueue.push( vi );
			iti.IncX();
			iti.SetVoxel(uNewLabel);
			iti.DecX();
    } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y+1;
	  if( vi.m_y < iY && (iti.Peek0px1py0pz() == uRoiLabel || iti.Peek0px1py0pz() == 0x00) )
	  {
	    viRegQueue.push( vi );
			iti.IncY();
			iti.SetVoxel(uNewLabel);
			iti.DecY();
	  } // if

	  vi.m_x = vi.m_x-1;
	  vi.m_y = vi.m_y-1;
	  if( vi.m_x > 0 && (iti.Peek1nx0py0pz() == uRoiLabel || iti.Peek1nx0py0pz() == 0x00) )
	  {
	    viRegQueue.push( vi );
			iti.DecX();
			iti.SetVoxel(uNewLabel);
			iti.IncX();
	  } // if

	  vi.m_x = vi.m_x+1;
	  vi.m_y = vi.m_y-1;
	  if( vi.m_y > 0 && (iti.Peek0px1ny0pz() == uRoiLabel || iti.Peek0px1ny0pz() == 0x00) )
	  {
	    viRegQueue.push( vi );
			iti.DecY();
			iti.SetVoxel(uNewLabel);
			iti.IncY();
	  } // if

	  vi.m_y = vi.m_y+1;
	  vi.m_z = vi.m_z-1;
	  if( vi.m_z > 0 && (iti.Peek0px0py1nz() == uRoiLabel || iti.Peek0px0py1nz() == 0x00) )
	  {
	    viRegQueue.push( vi );
			iti.DecZ();
			iti.SetVoxel(uNewLabel);
			iti.IncZ();
	  } // if

	  vi.m_z = vi.m_z+2;
	  if( vi.m_z < iZ && (iti.Peek0px0py1pz() == uRoiLabel || iti.Peek0px0py1pz() == 0x00) )
	  {
	    viRegQueue.push( vi );
			iti.IncZ();
			iti.SetVoxel(uNewLabel);
			iti.DecZ();
	  } // if

	  viRegQueue.pop();
  }  // end of while() 
} //GrowWithinShell


/** 
 *  recover the colon segment contour layer by dilation
 *
 *  @param iti the iterator of the INS volume
 *  @param pTmpMask the pointer to the temporary mask volume
 */
void vxSegmentFinder::RecoverSegmentContourLayer( vxBlockVolumeIterator<uint1> & iti, vxBlockVolume<uint1> * pTmpMask )
{
  vxBlockVolumeIterator<uint1> itt( *pTmpMask );
  vxBlockVolumeIteratorNeighbors<uint1> it6i( *m_pIns, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );
  vxBlockVolumeIteratorNeighbors<uint1> it6t( *pTmpMask, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );

  register Vector3D<int2> vPos;
	for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
	{
		for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
		{
      if( iti.GetVoxel() > 0 && iti.GetVoxel() < 16 )
      {
        vPos = iti.GetPos();
        itt.SetPos( vPos );
        itt.SetVoxel( iti.GetVoxel() );

	      for( it6i.CenterAt(vPos), it6t.CenterAt(vPos); !it6i.IsAtEnd(); ++it6i, ++it6t )
        {
          if( (it6i.GetVoxel() == 0 || it6i.GetVoxel() > 15)&& it6t.GetVoxel() == m_uBGlabel )
            it6t.SetVoxel( iti.GetVoxel() );
        } // for all 6 neighbors
      } // if
    } // for inside the block
  } // for all blocks
} // RecoverSegmentContourLayer


#undef FILE_REVISION 

// complete revision history:
// $Log: vxSegmentFinder.C,v $
// Revision 1.15.2.4  2010/02/12 20:40:05  dongqing
// Fix the bug in the FillCavity. When body ROI touch the segment or enclosed part
// of a segment, the old FillCavity function get all the body as a colon segment.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.15.2.3  2009/02/12 18:33:17  dongqing
// Reduce the size of air pucket for seed searching to 7 x 7 2D window
//
// Revision 1.15.2.2  2008/06/20 22:04:09  dongqing
// fix the bug in the FillCavity. One is typo on ShellLabel. Another is add
// extra check the seed of the segment for growing shell.
//
// Revision 1.15.2.1  2007/10/24 14:58:17  dongqing
// add post INS dilation for recovering the lumen lose
// that should fix the skinny overview only. It should not
// affect anything else
//
// Revision 1.15  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.14  2006/06/14 15:03:25  dongqing
// 2nd fine tune per Diane's validation
//
// Revision 1.13  2006/06/06 20:14:02  dongqing
// fix fill cavity bug for the vxSegmentFinder
//
// Revision 1.12  2006/06/02 20:14:19  dongqing
// improve CRES quality per Diane's request.
//
// Revision 1.11  2006/05/01 17:57:55  dongqing
// remove redundant codes
//
// Revision 1.10  2006/04/26 16:40:26  dongqing
// fix the CSI bug
//
// Revision 1.9  2006/04/24 13:26:53  dongqing
// fix the bug for generating CDV
//
// Revision 1.8  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.7  2006/04/19 19:53:45  dongqing
// bug free beta version
//
// Revision 1.6  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.5  2006/04/06 12:35:41  dongqing
// rewrite the region growing by using Peek function instead of GetVoxel
//
// Revision 1.4  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.3  2006/03/07 13:30:18  dongqing
// initial check in the TRI stage codes
//
// Revision 1.2  2006/03/02 21:22:43  dongqing
// check in initial DFB module
//
// Revision 1.1  2006/02/24 20:24:17  dongqing
// add INS module
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxSegmentFinder.C,v 1.15.2.4 2010/02/12 20:40:05 dongqing Exp $
// $Id: vxSegmentFinder.C,v 1.15.2.4 2010/02/12 20:40:05 dongqing Exp $
