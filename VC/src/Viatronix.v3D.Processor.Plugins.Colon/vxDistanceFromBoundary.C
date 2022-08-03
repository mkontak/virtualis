/// $Id: vxDistanceFromBoundary.C,v 1.9 2007/03/06 16:34:01 dongqing Exp $
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
#include "vxDistanceFromBoundary.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "Logger.h"

using namespace std;
using namespace hdr;

#define FILE_REVISION "$Revision: 1.9 $"

/** 
*  Default constructor
*
*  @param pIns the pointer to the INS volume
*  @param pSld the pointer to the SLD volume
*  @param sourceFilePath
*  @param destFilePath
*/
vxDistanceFromBoundary::vxDistanceFromBoundary( vxBlockVolume<uint1> *pIns, vxBlockVolume<uint1> *pSld,
		                                 const std::string & sourceFilePath, const std::string destFilePath)
:m_pIns(pIns), m_pSld(pSld), m_sourceFilePath(sourceFilePath), m_destFilePath(destFilePath)
{
  m_fMaxDfb = 30.0F; // mm
}


/** 
*  Default deconstructor
*/
vxDistanceFromBoundary::~vxDistanceFromBoundary()
{
}


/** 
*  compute DFB
*
*  @return true if it is successful
*/
bool vxDistanceFromBoundary::ComputeDFB()
{
  LogFyi("Start DFB procedure", "Run", "vxDistanceFromBoundary.C");

  try
  { 
    const Triple<int4> dim(m_pIns->GetHeader().GetVolDim());
    const int4 iX(dim.m_x);
    const int4 iY(dim.m_y);
    const int4 iZ(dim.m_z);
	  const Triple<float4> units(m_pIns->GetHeader().GetVoxelUnits());
    const float4 fXUnits(units.m_x);
    const float4 fYUnits(units.m_y);
    const float4 fZUnits(units.m_z);

    float4 fDistForward, fMaxBackDist, fDist, fDistTemp;
    uint2  uRMax, uRStart, uREnd;
	  std::vector<float4> vBuff(2+iX+iY+iZ, 0.0);
    const int4 iBuffOffset( 1 );
  
	  //// step1
    //// forward scan
    // put increasing distances for left to right xbeams
    // but only for voxel positions in the mask  
    
	  vxVolumeHeader ph(dim, units, 32, 0);
	  vxBlockVolume<float4> *pOutVolDfb = new vxBlockVolume<float4>(ph);
  
	  register int4 x, y, z, i;
	  register Vector3D<int2> backPos;
	  vxBlockVolumeIterator<uint1>  iti   (*m_pIns);
	  vxBlockVolumeIterator<float4> ito   (*pOutVolDfb);
    vxBlockVolumeIterator<float4> backIt(*pOutVolDfb );

    for( z=0, ito.SetPosZ(0), iti.SetPosZ(0); z<iZ; ++z, ito.IncZ(), iti.IncZ() )
    {
      for( y=0, ito.SetPosY(0), iti.SetPosY(0); y<iY; ++y, ito.IncY(), iti.IncY() )
      {
        for( fDistForward = 0.0, x=0, ito.SetPosX(0), iti.SetPosX(0); x<iX; ++x, ito.IncX(), iti.IncX() )
		    {
		      if( iti.GetVoxel() )
		      {
            fDistForward += fXUnits;
            ito.SetVoxel( fDistForward * fDistForward );
		      } // if( iti.GetVoxel() > 0x00 )
		      else
		      {
		        if( fDistForward > 0.0 )
		        {
              i = x;
              backIt.SetPos( ito.GetPos() );
              fDist = 0.0;
              fMaxBackDist = fDistForward/2.0;

              while( (i-->0)&&(fDist <= fMaxBackDist) )
              {
                backIt.SetVoxel(fDist * fDist);
                fDist += fXUnits;
                backIt.DecX();
              } // while
		        } // if( fDistForward > 0.0 )
			
						fDistForward = 0.0;
		      } // if-else
		    } // for(x)

        // check the margin
        x = iX-1;
        ito.SetPosX( x );
        if( ito.GetVoxel() )
        {
          i = x;
          backIt.SetPos( ito.GetPos() );
          fDist = fXUnits;
          fMaxBackDist = ito.GetVoxel()/2.0;
          while( (i--> 0)&&(fDist <= fMaxBackDist) )
          {
            backIt.SetVoxel(fDist * fDist);
            fDist += fXUnits;
            backIt.DecX();
          }
        } // if( ito.GetVoxel() )
	    } // for(y)
	  } // for(z)

    //// step 2
    register int4 n;
    float4 fSy(fYUnits * fYUnits);
    for( z=0, ito.SetPosZ(0); z<iZ; ++z, ito.IncZ() )
    { 
      for( x=0, ito.SetPosX(0); x<iX; ++x, ito.IncX() )
      {
        for( y=0, ito.SetPosY(0); y<iY; ++y, ito.IncY() )
        {
          vBuff[y + iBuffOffset] = ito.GetVoxel();
        }
        for( y=0, ito.SetPosY(0); y<iY; ++y, ito.IncY() )
        {
          fDist = vBuff[y+iBuffOffset];
          if( fDist>0.0 )
          {
            uRMax = static_cast<uint2>(sqrt(fDist) / fYUnits) + 1;
            uRStart = Min(uRMax, static_cast<uint2>(y+1));
            uREnd = Min(uRMax, static_cast<uint2>(iY-y+1));

            for( n=-static_cast<int4>(uRStart); n<uREnd; n++ )
            {
              fDistTemp = vBuff[y+n+iBuffOffset] + fSy * static_cast<float4>(n * n);
              if( fDistTemp < fDist )  
				      fDist = fDistTemp;
            }
            ito.SetVoxel(fDist);
          } // if( fDist>0.0 )        
        } // for y
      } // for x
    } // for z

    for( y=0; y<iY; y++ )
    {
      vBuff[y+iBuffOffset] = 0.0; 
    } // clear buffer
  
    //// step 3
    float4 fSz(fZUnits * fZUnits);
    for( y=0, ito.SetPosY(0); y<iY; ++y, ito.IncY() )
    {
      for( x=0, ito.SetPosX(0); x<iX; ++x, ito.IncX() )
      {
        for( z=0, ito.SetPosZ(0); z<iZ; ++z, ito.IncZ() )
        {
          vBuff[z+iBuffOffset] = ito.GetVoxel();
        }
        for( z=0, ito.SetPosZ(0); z<iZ; ++z, ito.IncZ() )
        {
          fDist = vBuff[z+iBuffOffset];
          if( fDist>0.0 )
          {
            uRMax = static_cast<int4>(sqrt(fDist) / fZUnits) + 1;
            uRStart = Min(uRMax, static_cast<uint2>(z+1));
            uREnd = Min(uRMax, static_cast<uint2>(iZ-z+1));

            for( n=-static_cast<int4>(uRStart); n<uREnd; n++ )
            {
              fDistTemp = vBuff[z+n+iBuffOffset] + fSz * static_cast<float4>(n * n);
              if ( fDistTemp < fDist ) 
				        fDist = fDistTemp;
            }

            ito.SetVoxel(fDist);
          } // if ( fDist>0.0 )      
        } // for z
      } // for x
    } // for y

    vxBlockVolumeIterator<uint1> its(*m_pSld);
    /// calculate the sqrt and find max distance
    float4 fMax(0.0);
    const float4 fDfbScale = 4.0F;
    for( ito.SetPos(0,0,0), its.SetPos(0,0,0); ito.IsNotAtEnd(); ito.NextBlockZYX(), its.NextBlockZYX() )
    {
      for ( ; ito.IsNotAtEndOfBlock(); ito.NextZYXinsideBlock(), its.NextZYXinsideBlock() )
      {
        fDistTemp = ito.GetVoxel();
        if ( fDistTemp > 0.0 ) 
        {
          fDist = static_cast<float4>(sqrt(fDistTemp));
          if ( fDist > fMax )       
          {
            fMax = fDist;
          }
          fDist = fDist * fDfbScale + 0.5;
          if( fDist > 255.0 )
          {
            fDist = 255.0;
          }
          if( fDist > 0.51 )
          {
            its.SetVoxel(static_cast<uint1>(fDist));
          }
        }
      } // inside block
    } // thourgh blocks

	  delete pOutVolDfb;

	  VarMap map(m_pSld->GetHeader().GetVariableVolumeHeader());
    map.SetHdrVar("maxDfb", fMax, "Maximum distance from boundary");
    map.SetHdrVar("DfbScale", fDfbScale, "Scale factor for distance values");
    m_pSld->GetHeader().SetVariableVolumeHeader(map);

  }
  catch( ... )
  {
    LogErr("Compute DFB fail", "Run", "vxDistanceFromBoundary.C");
	  return(false);
  }
  
  return(true);
}  // ComputeDFB


/**
 *  compute the external DFB
 *  
 *  @return true if necessary
 */
bool vxDistanceFromBoundary::ComputeDFBExt()
{
  const Triple<int4>   dim   (m_pIns->GetHeader().GetVolDim());
  const Triple<float4> units (m_pIns->GetHeader().GetVoxelUnits());
  const Triple<float4> fScale(256.0F/static_cast<float4>(dim.m_x),
                              256.0F/static_cast<float4>(dim.m_y),
                              256.0F/static_cast<float4>(dim.m_z));
  const Triple<int4>   newDim(256, 256, 256);
  const Triple<float4> newUnits(units.m_x/fScale.m_x, 
		                            units.m_y/fScale.m_y, 
																units.m_z/fScale.m_z);
  
  try
  { // down sampling INS volume for both memory and speed concern
    vxBlockVolume<uint1> *pSmallIns = new vxBlockVolume<uint1>(vxVolumeHeader(newDim, newUnits, 8, 0));
    vxBlockVolumeIterator<uint1> itsi(*pSmallIns);
    vxBlockVolumeIterator<uint1> iti(*m_pIns);
    vxBlockVolumeIteratorNeighbors<uint1> it6(*m_pIns, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06);
    
	  register Vector3D<uint4> vPos;
    register uint1 un(0x88);
    register int4 bIsLumen;
    for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
    {
      for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
	    {
	      if( iti.GetVoxel() > 0x00 )
	      {
	        vPos = iti.GetPos();
		      bIsLumen = true;
	        for( it6.CenterAt(vPos); !it6.IsAtEnd(); ++it6 )
					{
						if( it6.GetVoxel() == 0x00 )
		        {
		          bIsLumen = false;
			        break;
		        } // if
					} // for

	        if(bIsLumen)
		      {
	          vPos.m_x = static_cast<uint4>(fScale.m_x*(static_cast<float4>(vPos.m_x)+0.5F));
	          vPos.m_y = static_cast<uint4>(fScale.m_y*(static_cast<float4>(vPos.m_y)+0.5F));
	          vPos.m_z = static_cast<uint4>(fScale.m_z*(static_cast<float4>(vPos.m_z)+0.5F));

		        itsi.SetPos(vPos);
		        itsi.SetVoxel(un);
		      } // if(bIsLumen)
	      } // if( iti.GetVoxel() > 0x00 )
	    } // for in the block
    } // for all block

    LogFyi("Finish downsampling INS", "ComputeDFBExt", "vcDistanceFromBoundary.C");
    
    /// label colon wall
    uint1 uColonWall(un+1);
    IdentifyColonWall(pSmallIns, uColonWall);
    LogFyi("Finish wall labelling", "ComputeDFBExt", "vcDistanceFromBoundary.C");

    float4 fDistForward, fDist, fDistTemp;
    uint2  uRMax, uRStart, uREnd;
    
    const int4 iX(newDim.m_x);
    const int4 iY(newDim.m_y);
    const int4 iZ(newDim.m_z);
    
    vector<float4> vBuff(2+iX+iY+iZ, 0.0);
    
    const float4 fXUnits(newUnits.m_x);
    const float4 fYUnits(newUnits.m_y);
    const float4 fZUnits(newUnits.m_z);
    
    const int4 iBuffOffset(1);
    m_fMaxDfb = fXUnits * (static_cast<int4>(m_fMaxDfb)/fXUnits);
    float4 fSquareMaxDfb = m_fMaxDfb*m_fMaxDfb;
    /// step 1
    //// forward scan
    // put increasing distances for left to right xbeams
    // but only for voxel positions in the mask
    vxBlockVolume<float4> *pOutVolDfb = new vxBlockVolume<float4>(vxVolumeHeader(newDim, newUnits, 32, 0.0));
    vxBlockVolumeIterator<float4> ito(*pOutVolDfb);

    register int4 x, y, z;
    for( z=0, ito.SetPosZ(0), itsi.SetPosZ(0); z<iZ; ++z, ito.IncZ(), itsi.IncZ() )
    {
	    for( y=0, ito.SetPosY(0), itsi.SetPosY(0); y<iY; ++y, ito.IncY(), itsi.IncY() )
	    {
        for( fDistForward = m_fMaxDfb, x=0, ito.SetPosX(0), itsi.SetPosX(0); x<iX; ++x, ito.IncX(), itsi.IncX() )
	      {
	        if( itsi.GetVoxel() != uColonWall )
		      {
		        if( fDistForward < m_fMaxDfb )
		          fDistForward += fXUnits;
		      }
		      else
		      {
		        fDistForward = 0.0F;
		      }
		  
					ito.SetVoxel(fDistForward * fDistForward);
	      } // for(x)

	      fDistForward = m_fMaxDfb;
	      for( x=iX-1, itsi.SetPosX(iX-1); x>=0; --x, itsi.DecX() )
	      {
	        if( itsi.GetVoxel() != uColonWall )
		      {
		        if( fDistForward < m_fMaxDfb )
		          fDistForward += fXUnits;
		      }
		      else
		      {
		        fDistForward = 0.0F;
		      }
	      } // for(x)
	    } // for(y)
    } // for(z)

    delete pSmallIns;

    /// step 2
    int4 n;
    float4 fSy(fYUnits * fYUnits);
    for( z=0, ito.SetPosZ(0); z<iZ; z++, ito.IncZ() )
    {
      for(x=0, ito.SetPosX(0); x<iX; x++, ito.IncX() )
	    {
	      for( y=0, ito.SetPosY(0); y<iY; y++, ito.IncY() )
	      {
	        vBuff[y + iBuffOffset] = ito.GetVoxel();
	      }
	      for( y=0, ito.SetPosY(0); y<iY; y++, ito.IncY() )
	      {
	        fDist = vBuff[y + iBuffOffset];
		      if( fDist > 0.0F )
		      {
		        uRMax = static_cast<uint2>(sqrt(fDist) / fYUnits) + 1;
		        uRStart = Min(uRMax, static_cast<uint2>(y));
		        uREnd = Min(uRMax, static_cast<uint2>(iY-y));    /// removed -1 for the margin
		    
						for( n=-static_cast<int4>(uRStart); n<uREnd; n++ )
		        {
		          fDistTemp = vBuff[y+n+iBuffOffset] + fSy + static_cast<float4>(n*n);
			        if( fDistTemp < fDist )
                fDist = fDistTemp;
		        } // for(n)
		    
						ito.SetVoxel(fDist);
		      } // if
	      } // for(y)
	    } // for(x)
    } // for(z)

    for( y=0; y<iY; y++ )
      vBuff[y+iBuffOffset] = 0.0F;

    /// step 3
    float4 fSz(fZUnits * fZUnits);
    for( y=0, ito.SetPosY(0); y<iY; y++, ito.IncY() )
    {
      for(x=0, ito.SetPosX(0); x<iX; x++, ito.IncX() )
	    {
        for( z=0, ito.SetPosZ(0); z<iZ; z++, ito.IncZ() )
	      {
	        vBuff[z + iBuffOffset] = ito.GetVoxel();
	      }
	      for( z=0, ito.SetPosZ(0); z<iZ; z++, ito.IncZ() )
	      {
	        fDist = vBuff[z + iBuffOffset];
		      if( fDist > 0.0F )
		      {
		        uRStart = Min(uRMax, static_cast<uint2>(z));
		        uREnd = Min(uRMax, static_cast<uint2>(iZ-z));

		        for( n=-static_cast<int4>(uRStart); n<uREnd; n++ )
		        {
		          fDistTemp = vBuff[z+n+iBuffOffset] + fSz*static_cast<float4>(n*n);
		          if( fDistTemp < fDist )
		            fDist = fDistTemp;
		        } // for(n)
		   
						ito.SetVoxel(fDist);
		      } // if( fDist > 0.0F )
	      } // for(z)
	    } // for(x)
    } // for(y)
  
    vxBlockVolume<uint1> *pSlv = new vxBlockVolume<uint1>(vxVolumeHeader(newDim, newUnits, 8, 0));
    vxBlockVolumeIterator<uint1> its(*pSlv);

    /// calculate the sqrt and find max distance
    float4 fMax(0.0);
    const float4 fDfbScale = 4.0F;
    for( ito.SetPos(0,0,0), its.SetPos(0,0,0); ito.IsNotAtEnd(); ito.NextBlockZYX(), its.NextBlockZYX() )
    {
      for( ; ito.IsNotAtEndOfBlock(); ito.NextZYXinsideBlock(), its.NextZYXinsideBlock() )
      {
        fDistTemp = ito.GetVoxel();
        if ( fDistTemp > 0.0 ) 
        {
          fDist = static_cast<float4>(sqrt(fDistTemp));
          if ( fDist > fMax )       
          {
            fMax = fDist;
          }
          fDist = fDist * fDfbScale + 0.5;
          if( fDist > 255.0 )
          {
            fDist = 255.0;
          }
          if( fDist > 0.51 )
          {
            its.SetVoxel(static_cast<uint1>(fDist));
          }
        } // if
      } // inside block
    } // thourgh blocks

    delete pOutVolDfb;
		    
	  VarMap map(pSlv->GetHeader().GetVariableVolumeHeader());
    map.SetHdrVar("maxDfb", fMax, "Maximum distance from boundary");
    map.SetHdrVar("DfbScale", fDfbScale, "Scale factor for distance values");
    pSlv->GetHeader().SetVariableVolumeHeader(map);

    pSlv->Write(m_destFilePath+".slv");

    delete pSlv;
  }
  catch( ... )
  {
    LogErr("Fail computing DFB Ext", "ComputeDFBExt", "vxDistanceFromBoundary.C");
	  return(false);
  }

  return(true);
} // ComputeDFBExt()


/**
 *  label colon wall
 *
 *  @param pMask the label mask
 *  @param uWallLabel the label index for the colonic wall voxels
 */
void vxDistanceFromBoundary::IdentifyColonWall( vxBlockVolume<uint1> * pMask, const uint1 & uWallLabel )
{
  vxBlockVolumeIterator<uint1> it(*pMask);
  vxBlockVolumeIteratorNeighbors<uint1> it26(*pMask, (Vector3D<int2>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26);

  register uint1 ul(uWallLabel-1);
  register uint1 uw(uWallLabel);
  for( it.SetPos(0,0,0); it.IsNotAtEnd(); it.NextBlockZYX() )
  {
    for( ; it.IsNotAtEndOfBlock(); it.NextZYXinsideBlock() )
    {
	    if( it.GetVoxel() == ul )
	    {
		    for( it26.CenterAt(it.GetPos()); !it26.IsAtEnd(); ++it26 )
		    {
		      if( (it26.GetVoxel() == 0x00) && it26.IsInsideVolume() )
		        it26.SetVoxel(uw);
		    } // for(it26)
	    } // if
	  } // for inside block
  } // for all block

} // IdentifyColonWall


#undef FILE_REVISION 

// complete revision history:
// $Log: vxDistanceFromBoundary.C,v $
// Revision 1.9  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.8  2006/06/02 19:01:16  geconomos
// updated to vs 2005
//
// Revision 1.7  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.6  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.5  2006/04/06 15:08:57  dongqing
// rewrite the iterator
//
// Revision 1.4  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.3  2006/03/07 13:30:18  dongqing
// initial check in the TRI stage codes
//
// Revision 1.2  2006/03/03 20:47:00  dongqing
// finalize DFB codes
//
// Revision 1.1  2006/03/02 21:22:43  dongqing
// check in initial DFB module
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxDistanceFromBoundary.C,v 1.9 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxDistanceFromBoundary.C,v 1.9 2007/03/06 16:34:01 dongqing Exp $
