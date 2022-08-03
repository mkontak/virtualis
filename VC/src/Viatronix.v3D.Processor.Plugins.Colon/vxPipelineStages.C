// $Id: vxPipelineStages.C,v 1.23.2.3 2010/02/12 20:38:34 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this
// file may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen dongqing@viatronix.net
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "vxPipeline.h"
#include "VCSettings.h"
#include "Thresholds.h"
#include "vxBackgroundRemoval.h"
#include "vxLungRemoval.h"
#include "vxCRES.h"
#include "vxSegmentFinder.h"
#include "vxDistanceFromBoundary.h"
#include "vxTriangulator.h"
#include "vxSkeletonator.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxVolumeHeader.h"
#include "vxIntensityConverter.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"

#include "Timer.h"
#include "Date.h"
#include "File.h"
#include "V2kVersion.h"
// includes only needed when the test code in -stage test is useed
#include "Logger.h"
#include "SystemInfo.h"
#include "Mutex.h"

using namespace std;
using namespace hdr;
using namespace vx;

/** 
 *  This file includes all functions for each stage of the pipeline
 */

#define FILE_REVISION "$Revision: 1.23.2.3 $"

/**
*  setting thresholds for rendering.
* 
*  @param destFilePath is the path for the output file
*  @return True iff the stage was completed successfull.
*/
bool vxPipeline::ComputeThresholds( const std::string & destFilePath )
{
  Thresholds thresholds;
	thresholds.m_vThresholds.push_back(    0 );
	thresholds.m_vThresholds.push_back(  200 );
	thresholds.m_vThresholds.push_back(  280 );
	thresholds.m_vThresholds.push_back(  760 );
	thresholds.m_vThresholds.push_back( 1200 );
	thresholds.m_vThresholds.push_back( 1350 );
	thresholds.m_vThresholds.push_back( 1500 );
	thresholds.m_vThresholds.push_back( 1900 );
	thresholds.m_vThresholds.push_back( 4095 );
	try
	{
    thresholds.Write(destFilePath+".thr");
	}
	catch( ... )
	{
	  LogErr("Fail to write THR", "vxPipelineStages.C", "ComputeThresholds");
	}
  return(true);
} // ComputeThresholds


/**
* Performs segmentation of body region
* 
* @param pVol the pointer to the CT input volume
* @param pMask the pointer to the mask volume to be set by the function.
* @param destFilePath is the path for the output files
* @return True iff the stage was completed successfull.
*/
bool vxPipeline::ComputeBodyRegion( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask, 
								                    const std::string & destFilePath )
{
  try
  {
    vxBackgroundRemoval backgroundRemoval( pVol, pMask );
    if( !backgroundRemoval.Run() )
    {
      return(false);
    }
    SendState( 35,  "CLA" );

    vxLungRemoval lungRemoval( pVol, pMask );
    if( !lungRemoval.Run() )
    {
	    return(false);
    }
		pMask->Compact();
    SendState( 38, "CLA" );
  }
  catch( ... )
  {
    LogErr("Compute body region fail", "ComputeBodyRegion", "vxPipelineStages.C");
	  return(false);
  }

  return(true);
} // ComputeClassification


/**
* Performs colonic residue electronic subtraction
* 
* @param pVol               the pointer to the CT volume
* @param pMask              the pointer to the mask volume to be set by the function.
* @param sourceFilePath     the path for the input volume file
* @param destFilePath       the path for the output files 
* @return True iff the stage was completed successfull.
*/
bool vxPipeline::ComputeCRES( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask,
                              const std::string & sourceFilePath, const std::string & destFilePath )
{ 
  // background label index in the mask
  register uint1 uBGlabel(0xFF);
  register uint2 uHigh(1220);  // 220 HU
  const Triple<int2> viDim(pVol->GetHeader().GetVolDim());
  register int4 iX(viDim.m_x);
  register int4 iY(viDim.m_y);
  register int4 iZ(viDim.m_z);

  vxBlockVolumeIterator<uint2>  itv(*pVol);
  vxBlockVolumeIterator<uint1>  itm(*pMask);

  int4 i(0);
  // count the number of the high intensity voxel
  for( itm.SetPos(0,0,0), itv.SetPos(0,0,0); itm.IsNotAtEnd(); itm.NextBlockZYX(), itv.NextBlockZYX() )
  {
    for( ; itm.IsNotAtEndOfBlock(); itm.NextZYXinsideBlock(), itv.NextZYXinsideBlock() )
    {
      if( (itm.GetVoxel() == 0x00)&&(itv.GetVoxel() > uHigh)&& itm.IsInsideVolume() )
			{
				i++;
				if( itv.GetVoxel() > 1900 )
					itv.SetVoxel(1900);
			}
    } // for inside the block
  } // for all blocks

  int4 iNumOfBodyVoxel(0);
  pMask->GetHeader().GetVariableVolumeHeader().GetHdrVar("numOfBodyVoxel", iNumOfBodyVoxel);
  float4 f(static_cast<float4>(i)/static_cast<float4>(iNumOfBodyVoxel));
  
  //cout << "Threshold = " << m_fResiduePercent << ", Actual ratio = " << f << std::endl;

  if( f < m_fResiduePercent )  // This threshold must be robust and accurate enough !! However, skip EC does not hurt clinic diagnosis
  { 
    m_bCRESOn = false;
	  LogFyi("High intensity voxels are few. Skip CRES!!!", "ComputeCRES", "vxPipelineStages.C");
	  return(true);
  }
  SendState(  42, "CRES" );

  try
  { // apply colonic residue electronic subtraction procedure
	  /** =====================================================================
	   *  Note: 1. The CDV volume will be output in INS stage rather than here.
	   *        That is for mitigating flying debris in the lumen.
	   *        2. The subtracted region is labeled in the mask for generating
	   *        CDV volume later.
	   *  ====================================================================*/
    vxCRES cres(pVol, pMask, sourceFilePath, destFilePath);
    if( !cres.Run() )
	  {
	    return(false);
	  }
		pVol->Compact();
		pMask->Compact();
    SendState( 48,  "CRES" );
  }
  catch( ... )
  {
    LogErr("Colonic residue electronic subtraction fail", "ComputeCRES", "vxPipelineStages.C");
	  return(false);
  }
  
  return(true);
} // ComputeCRES


/**
*   determine colon lumen mask.
* 
*  @param pVol               the pointer to the CT volume that may or may not be cleansed.
*  @param pMask              the pointer to the mask volume.
*  @param pIns               the pointer to the INS volume
*  @param sourceFilePath     the path for the input volume file
*  @param destFilePath       the path for the output files 
*  @return True iff the stage was completed successfull.
*/
bool vxPipeline::ComputeInsideSegments( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask,
	           vxBlockVolume<uint1> *pIns, const std::string & sourceFilePath, const std::string & destFilePath )
{
  // use thresholds[2], because that is what rendering uses 
  SendState(  51, "INS" );
  vxSegmentFinder segFinder(pVol, pMask, pIns, sourceFilePath, destFilePath);

  if( !segFinder.Run() )
  {
	  return(false);
  }
	pIns->Compact();
  SendState(  54, "INS" );

  return(true);
} // ComputeInsideSegments


/**
* Compute distance from boundary.
* 
* @param pInsNoCRES
* @param pIns
* @param pUld
* @param pSld
* @param sourceFilePath
* @param destFilePath
* @return true if success.
*/
bool vxPipeline::ComputeDFB( vxBlockVolume<uint1> *pInsNoCRES, vxBlockVolume<uint1> *pIns, 
														 vxBlockVolume<uint1> *pUld, vxBlockVolume<uint1> *pSld, 
							 const std::string & sourceFilePath, const std::string & destFilePath, const bool & bTurnOffDebugger )
{
  SendState(   56, "DFB" );
	{
	  vxDistanceFromBoundary dfb(pIns, pSld, sourceFilePath, destFilePath);
	  if( !dfb.ComputeDFB() )
	  { // This will compute a SLD volume 
	    return(false);
	  }
		pSld->Compact();
	}
	if( m_bCRESOn )
	{
	  vxDistanceFromBoundary dfbu(pInsNoCRES, pUld, sourceFilePath, destFilePath);
	  if( !dfbu.ComputeDFB() )
	  { // This will compute a ULD volume 
	    return(false);
	  }
		pUld->Compact();
	}
  SendState(   74, "DFB" );

	//if( !bTurnOffDebugger )
	//{ //The SLV is no longer necessary. The Viewer has new render now. Dongqing 3-6-07
	//	if( !dfb.ComputeDFBExt() )
	//	{ // This will compute a 256^3 dimension SLV volume and write out to destFilePath
	//		return(false);
	//	}
	//	SendState( "DFB", 74 );
	//}
  return(true);
} // ComputeDFB


/**
* Computes triangles for the colon overview.
* 
* @param pIns
* @param sourceFilePath
* @param destFilePath
* @return true if success.
*/
bool vxPipeline::ComputeTriangles( vxBlockVolume<uint1> * pIns, const std::string & sourceFilePath, 
                                   const std::string & destFilePath )
{
  vxTriangulator tri(pIns, destFilePath);
  SendState(  76, "TRI" );
  if( !tri.Run() )
  {
    return(false);
  }
  SendState( 84, "TRI" );
  return(true);
} // ComputeTriangles

/**
* Compute the centerline and navigation fields of each colon segment.
* 
* @param pIns
* @param pSld
* @param sourceFilePath
* @param destFilePath
* @return true if success.
*/
bool vxPipeline::ComputeSkeleton( vxBlockVolume<uint1> * pIns, vxBlockVolume<uint1> * pSld, 
	                                const std::string & sourceFilePath, const std::string & destFilePath )
{
  vxSkeletonator ske(pIns, pSld, destFilePath);
  SendState(  86, "SKE" );
  if( !ske.Run() )
  {
    return(false);
  }
  SendState(  98, "SKE" );
  return(true);
} // ComputeSkeleton

/// All the MRI related codes needs to be rewriten due to the volume
/// coordinate system change !


/**
* Compute distance from boundary for MRI.
* 
* @param insBVol
* @param sldVol
* @param fileNameRoot
* @return true if success.
*/
bool vxPipeline::ComputeDFBMRI(BlockVolUC & insBVol, BlockVolUC & sldVol,
					const std::string & destFilePath )
{
	//DistanceFromBoundary dfb;
	//dfb.ComputeDistanceFromBoundary(insBVol, sldVol);
	//// convert to linear to write
	//LinVolUC sldVolLin(sldVol.GetDim(), sldVol.GetBitsPerVoxel(), sldVol.GetUnits(), sldVol.GetUnitType());
	//sldVolLin.SetHdrMap(sldVol.GetHdrMap());
	//for ( BlockVolIterUC sldIter(sldVol.GetVolIterator()); !sldIter.End(); sldIter.NextNonZero() )
	//  sldVolLin.SetVoxel(sldIter.GetPos(), sldIter.GetVoxel());
	//sldVolLin.Write(destFilePath+".sld", false);
	return(true);
} // ComputeDFBMRI


/**
* Compute distance from boundary for a downsized volume.
* 
* @param insBVol
* @param fileNameRoot
* @return 
*/
bool vxPipeline::ComputeDFBSmallMRI(BlockVolUC & insBVol, const std::string & destFilePath )
{    
	//Triple<int4> vDim( insBVol.GetDim() );
	//BlockVolUC smallInsVol;    
	//const Triple<uint4> halfDim(vDim.m_x/2, vDim.m_y/2, vDim.m_z/2);
	//smallInsVol = BlockVolUC (halfDim,  insBVol.GetBitsPerVoxel(), insBVol.GetUnits(), insBVol.GetUnitType(), 
	//	insBVol.GetBackground(), insBVol.GetExtension(), insBVol.GetFileName(), insBVol.GetTitle(), insBVol.GetAuthor());
	//insBVol.Fit(smallInsVol, hdr::TRILIN);
	//Triple<uint4>          dim = smallInsVol.GetDim();
	//hdr::UnitTypeEnum uintType = smallInsVol.GetUnitType();

	//BlockVolUC sldExtVol(dim, 8*sizeof(uint1), smallInsVol.GetUnits(), uintType,
	//	0, SLD, destFilePath+".slv", "Space Leap Distance volume (small)", "Made by Viatronix");

	//DistanceFromBoundary dfb;
	//dfb.IdentifyColonWall(smallInsVol);
	//dfb.ComputeDistanceFromBoundaryExt(smallInsVol, sldExtVol);
	//sldExtVol.Write(destFilePath+".slv", false);
	//vol::LinVolUInt1 tmp;
	//tmp.Read(destFilePath+".slv");
	//tmp.Write(destFilePath+".slv", false);
	return(true);
} // ComputeDFBSmallMRI


/**
 *  This is for beta version debugging
 */
bool vxPipeline::CreateOldFormatOutput( const std::string & sourceFilePath, const std::string & destFilePath )
{ 
	///============================================================
	/// create 8 bits ECV and DIF

	// read volumes in
  vxBlockVolume<uint2> *pv = new vxBlockVolume<uint2>;
	if( pv == NULL )
	{
		throw ex::AbortDataProcessingException(LogRec("Fail initialization of volume: CreateOldGFormatOutput", 
			             "vxPipelineStages", "CreateOldFormatOutput"));
		return(false);
	}
	try
	{
	  vxBlockVolumeFileReader< uint2 >::Read( sourceFilePath+".vol", *pv );
		pv->Compact();
	}
	catch( ... )
	{
		throw ex::AbortDataProcessingException(LogRec("Fail to read the original volume", 
			             "vxPipelineStages", "CreateOldFormatOutput"));
		return(false);
	}
	vxBlockVolumeIterator<uint2> itv(*pv);

  File * pCdvFileCheck = new File(destFilePath+".cdv");
  if( pCdvFileCheck->Exists() )
  { // CDV volume exist. This is a cleansed case.
    delete pCdvFileCheck;
	}
	else
	{
		throw ex::AbortDataProcessingException(LogRec("CDV file was missed", 
			             "vxPipelineStages", "CreateOldFormatOutput"));
		return(false);
	}

  vxBlockVolume<uint2> *pcdv = new vxBlockVolume<uint2>;
	if( pcdv == NULL )
	{
		std::cout << "Fail initialization of volume" << std::endl;
		return(false);
	}
  vxBlockVolumeFileReader< uint2 >::Read( destFilePath+".cdv", *pcdv );
	pcdv->Compact();

	vxBlockVolumeIterator<uint2> itcd(*pcdv);

	vxVolumeHeader *ph = new vxVolumeHeader;
	*ph = pcdv->GetHeader();

	/// both intensity volume and CDV should have the identical Bias value.
 	int4 iBias(1000);
  int4 iMax(0);
  // get iMax
  std::string sBias;
  ph->GetHeaderEntry( vxVolumeHeader::Bias, sBias );
  iBias = atoi( sBias.c_str() );

  vxIntensityConverter intenConv;
  iMax = intenConv.GetMaxValue( *ph );      
  if( iMax <= 0 )
    throw ex::AbortLoadException( LogRec("Incorrect maximum value", "vxPipelineStages", "CreateOldFormatOutput") );  

	const float4 fMax (static_cast<float4>(iMax));

	/// 8 bits ECV volume. It has been resize in previous load stage. Make sure of this
	LinVolUC *pVolUC = new LinVolUC(ph->GetVolDim(), 8*sizeof(uint1), ph->GetVoxelUnits(),
		        MILLIMETER, 0, UNKNOWN_EXT, destFilePath+".ecv", "Electronically cleansed volume", "Made by Viatronix");
  if( pVolUC == NULL )
	{
		throw ex::AbortDataProcessingException(LogRec("Outof memory", 
			             "vxPipelineStages", "CreateOldFormatOutput"));
		return(false);
	}
	uint1 *pUC = pVolUC->GetDataPtr();    
  
	BlockVolUC *pDiffUC = new BlockVolUC(ph->GetVolDim(), 8*sizeof(uint1), ph->GetVoxelUnits(), 
		         MILLIMETER, 0, UNKNOWN_EXT, destFilePath+".dif", "Electronically cleansed Difference-volume", "Made by Viatronix");
  if( pDiffUC == NULL )
	{
		throw ex::AbortDataProcessingException(LogRec("Outof memory", 
			             "vxPipelineStages", "CreateOldFormatOutput"));
		return(false);
	}  
	it::IterBlkVolUInt1 itDIF(pDiffUC->GetVolIterator());

  // get EC indicator in the header of CDV
  VarMap map( ph->GetVariableVolumeHeader() );
  int4 indicator(0);
  map.GetHdrVar("IsEcleansed", indicator);

	const int4 iX(ph->GetVolDim().m_x);
	const int4 iY(ph->GetVolDim().m_y);
	const int4 iZ(ph->GetVolDim().m_z);
	const int4 iS(iX*iY);
	register int4 x, y, z, s, t, n;

  if( indicator == 1 )
	{
	  if( iBias != 1000 )
	  { // shift value based on the Bias value
      try
	    { // clamp image based on the bias
	      register     int2 iv, ic;			
	      register     float4 ff;
				const int4   iii( 1000-iBias );
			  const float4 f  ( 255.0F/2000.0F );

				s = 0;
				for( z=0; z<iZ; z++ )
				{
					t = s;
				  for( y=0; y<iY; y++ )
					{
            itv.SetPos(0,y,z);
						itcd.SetPos(0,y,z);

						n = t;
            for( x=0; x<iX; n++, x++, itv.IncX(), itcd.IncX(), itDIF.NextZYX() )
						{
							ff = static_cast<float4>(itv.GetVoxel())/fMax;
							iv = ff*iMax+iii;
							if( iv < 0 )
								iv = 0;

							ic = itcd.GetVoxel();
							if( ic > 0 )
							{
								ff = static_cast<float4>(ic)/fMax;
								ic = ff*iMax+iii;
								if( ic < 0 )
									ic = 0;
							} // if( itc.GetVoxel() > 0 )

							if( ic == 0 )
							{ // this is a non-cleansed voxel. keep the original value
								if( iv < 2000 )
									pUC[n] = static_cast<uint1>(f*static_cast<float4>(iv));
								else
									pUC[n] = 255;
							}
							else
							{ // this is a cleansed voxel
								if( iv < 2000 )
									itDIF.SetVoxelRaw(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									itDIF.SetVoxelRaw(255);

								if( ic < 2000 )
									pUC[n] = static_cast<uint1>(f*static_cast<float4>(ic));
								else
									pUC[n] = 255;
							}	// if-else	
						} // for x

						t = t+iX;
					} // for y

					s = s+iS;
				} // for(z)
	    } // try
	    catch( ... )
	    {		
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF for bias = 1000", "Fields_ext", "LoadVolumeFields3" ));  
	    }
		} // if( iBias != 1000 )
		else
		{
      try
	    { // clamp image based on the bias
	      register     int2 iv, ic;			
			  const float4 f( 255.0F/2000.0F );

				s = 0;
				for( z=0; z<iZ; z++ )
				{
					t = s;
				  for( y=0; y<iY; y++ )
					{
            itv.SetPos(0,y,z);
						itcd.SetPos(0,y,z);

						n = t;
						for( x=0; x<iX; n++, x++, itv.IncX(), itcd.IncX(), itDIF.NextZYX() )
						{
							iv = itv.GetVoxel();
							ic = itcd.GetVoxel();

							if( ic == 0 )
							{ // this is a non-puddle voxel. no cleansing happens here
								if( iv < 2000 )
									pUC[n] = static_cast<uint1>(f*static_cast<float4>(iv));
								else
									pUC[n] = 255;
							}
							else
							{
								// this is a voxel in the cleansed area
								if( iv < 2000 )
									itDIF.SetVoxelRaw(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									itDIF.SetVoxelRaw(255);

								if( ic < 2000 )
									pUC[n] = static_cast<uint1>(f*static_cast<float4>(ic));
								else
									pUC[n] = 255;
							}	// if-else	
						} // for x

						t = t+iX;
					} // for y

					s = s+iS;
				} // for(z)
	    } // try
	    catch( ... )
	    {		
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF for bias != 1000", "Fields_ext", "LoadVolumeFields3" ));  
	    }
		} // else iBias == 1000
	} // if( CRES happened)
	else 
	{ // The CRES did not apply			
		if( iBias != 1000 )
	  { // shift value based on the Bias value
      try
	    { // clamp image based on the bias
	      register int2   iv;			
	      register float4 ff;
			  const float4    f  ( 255.0F/2000.0F );
				const int4      iii( 1000-iBias );

				s = 0;
				for( z=0; z<iZ; z++ )
				{
					t = s;
				  for( y=0; y<iY; y++ )
					{
						itv.SetPos(0,y,z);
						n = t;
						for( x=0; x<iX; n++, x++, itv.IncX() )
						{
							ff = static_cast<float4>(itv.GetVoxel())/fMax;
							iv = ff*iMax+iii;
							if( iv < 0 )
								iv = 0;

							if( iv < 2000 )
								pUC[n] = static_cast<uint1>(f*static_cast<float4>(iv));
							else
								pUC[n] = 255;
						} // for x

						t = t+iX;
					} // for y

					s = s+iS;
				} // for(z)
	    } // try
	    catch( ... )
	    {		
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF", "Fields_ext", "LoadVolumeFields3" ));  
	    }
		} // if( iBias != 1000 )
		else
		{
      try
	    { // clamp image based on the bias
	      register     int2 iv;			
			  const float4 f( 255.0F/2000.0F );

				s = 0;
				for( z=0; z<iZ; z++ )
				{
					t = s;
				  for( y=0; y<iY; y++ )
					{
						itv.SetPos(0,y,z);
						n = t;
						for( x=0; x<iX; n++, x++, itv.IncX() )
						{
							iv = itv.GetVoxel();
							if( iv < 2000 )
								pUC[n] = static_cast<uint1>(f*static_cast<float4>(iv));
							else
								pUC[n] = 255;
						} // for x

						t = t+iX;
					} // for y

					s = s+iS;
				} // for(z)
	    } // try
	    catch( ... )
	    {		
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF", "Fields_ext", "LoadVolumeFields3" ));  
	    }
		} // else iBias == 1000
	} // else if CRES not apply
  
	delete pv;
	delete pcdv;

  pVolUC->SetHdrMap(map);
  pVolUC->Write(destFilePath+".ecv", false);
  delete pVolUC;

  pDiffUC->SetHdrMap(map);
  pDiffUC->Write(destFilePath+".dif", false);
  delete pDiffUC;

	///============================================================
	/// create INS
  vxBlockVolume<uint1> *pIns = new vxBlockVolume<uint1>;
	vxBlockVolumeFileReader< uint1 >::Read( destFilePath+".ins", *pIns );
	pIns->Compact();

	BlockVolUC *pOIns = new BlockVolUC(pIns->GetHeader().GetVolDim(), 8*sizeof(uint1), pIns->GetHeader().GetVoxelUnits(), 
		       MILLIMETER, 0, INS, destFilePath+".ins", "Inside volume", "Made by Viatronix");

	it::IterBlkVolUInt1 itOIns(pOIns->GetVolIterator());
	vxBlockVolumeIterator<uint1> itIns(*pIns);
	for( itIns.SetPos(0,0,0); itIns.IsNotAtEnd(); itOIns.NextZYX(), itIns.NextZYX() )
	{
		itOIns.SetVoxelRaw( itIns.GetVoxel() );
	}

	map = pIns->GetHeader().GetVariableVolumeHeader();
	pOIns->SetHdrMap(map);
	delete pIns;
	pOIns->Write(destFilePath+".ins", false);
	delete pOIns;

	///============================================================
	/// create SLD and SLV
  vxBlockVolume<uint1> *pSld = new vxBlockVolume<uint1>;
	vxBlockVolumeFileReader< uint1 >::Read( destFilePath+".sld", *pSld );
	pSld->Compact();

	BlockVolUC *pOSld = new BlockVolUC(pSld->GetHeader().GetVolDim(), 8*sizeof(uint1), pSld->GetHeader().GetVoxelUnits(), 
		       MILLIMETER, 0, SLD, destFilePath+".sld", "Space Leap Distance volume", "Made by Viatronix");

	it::IterBlkVolUInt1 itOSld(pOSld->GetVolIterator());
	vxBlockVolumeIterator<uint1> itSld(*pSld);
	for( itSld.SetPos(0,0,0); itSld.IsNotAtEnd(); itOSld.NextZYX(), itSld.NextZYX() )
	{
		itOSld.SetVoxelRaw( itSld.GetVoxel() );
	}

	map = pSld->GetHeader().GetVariableVolumeHeader();
	pOSld->SetHdrMap(map);
	delete pSld;
	pOSld->Write(destFilePath+".sld", false);
	delete pOSld;

  vxBlockVolume<uint1> *pSlv = new vxBlockVolume<uint1>;
	vxBlockVolumeFileReader< uint1 >::Read( destFilePath+".slv", *pSlv );
	pSlv->Compact();
	
	BlockVolUC *pOSlv = new BlockVolUC(pSlv->GetHeader().GetVolDim(), 8*sizeof(uint1), pSlv->GetHeader().GetVoxelUnits(), 
		       MILLIMETER, 0, SLD, destFilePath+".sld", "Space Leap Distance volume (small)", "Made by Viatronix");

	it::IterBlkVolUInt1 itOSlv(pOSlv->GetVolIterator());
	vxBlockVolumeIterator<uint1> itSlv(*pSlv);
	for( itSlv.SetPos(0,0,0); itSlv.IsNotAtEnd(); itOSlv.NextZYX(), itSlv.NextZYX() )
	{
		itOSlv.SetVoxelRaw( itSlv.GetVoxel() );
	}

	map = pSlv->GetHeader().GetVariableVolumeHeader();
	pOSlv->SetHdrMap(map);
	delete pSlv;
	pOSlv->Write(destFilePath+".slv", false);
	delete pOSlv;

	///==================================================================
	/// create PDR
	{
		vxBlockVolume<float4> PDR0;
	  vxBlockVolumeFileReader< float4 >::Read( destFilePath+"_0.pdr", PDR0 );
	  PDR0.Compact();

	  BlockVolF Opdr0( PDR0.GetHeader().GetVolDim(), 8*sizeof(float4), PDR0.GetHeader().GetVoxelUnits(), 
		          MILLIMETER, FLT_MAX, DFS, destFilePath+"_0.pdr","potential distance from root0 volume", "Made by Viatronix");
	  it::IterBlkVolFloat4 itOpdr0(Opdr0.GetVolIterator());
	  vxBlockVolumeIterator<float4> itPDR0(PDR0);
	  for( itPDR0.SetPos(0,0,0); itPDR0.IsNotAtEnd(); itOpdr0.NextZYX(), itPDR0.NextZYX() )
	  {
		  itOpdr0.SetVoxelRaw( itPDR0.GetVoxel() );
	  }

	  map = PDR0.GetHeader().GetVariableVolumeHeader();
	  Opdr0.SetHdrMap(map);
	  Opdr0.Write(destFilePath+"_0.pdr", false);
	}

	{
		vxBlockVolume<float4> PDR1;
		vxBlockVolumeFileReader< float4 >::Read( destFilePath+"_1.pdr", PDR1 );
		PDR1.Compact();

		BlockVolF Opdr1(PDR1.GetHeader().GetVolDim(), 8*sizeof(float4), PDR1.GetHeader().GetVoxelUnits(), 
								MILLIMETER, FLT_MAX, DFS, destFilePath+"_1.pdr","potential distance from root1 volume", "Made by Viatronix");
		it::IterBlkVolFloat4 itOpdr1(Opdr1.GetVolIterator());
		vxBlockVolumeIterator<float4> itPDR1(PDR1);
		for( itPDR1.SetPos(0,0,0); itPDR1.IsNotAtEnd(); itOpdr1.NextZYX(), itPDR1.NextZYX() )
		{
			itOpdr1.SetVoxelRaw( itPDR1.GetVoxel() );
		}

		map = PDR1.GetHeader().GetVariableVolumeHeader();
		Opdr1.SetHdrMap(map);
		Opdr1.Write(destFilePath+"_1.pdr", false);
	}

	///===================================================================
	/// create CSI
	{
		vxBlockVolume<uint2> CSI0;
	  vxBlockVolumeFileReader< uint2 >::Read( destFilePath+"_0.csi", CSI0 );
	  CSI0.Compact();

	  BlockVolUS Ocsi0(CSI0.GetHeader().GetVolDim(), 8*sizeof(uint2), CSI0.GetHeader().GetVoxelUnits(), 
		          MILLIMETER, 0,UNKNOWN_EXT,destFilePath+"_0.csi","closest centerline0 index volume",  "Made by Viatronix");
	  it::IterBlkVolUInt2 itOcsi0(Ocsi0.GetVolIterator());
	  vxBlockVolumeIterator<uint2> itCSI0(CSI0);
	  for( itCSI0.SetPos(0,0,0); itCSI0.IsNotAtEnd(); itOcsi0.NextZYX(), itCSI0.NextZYX() )
	  {
		  itOcsi0.SetVoxelRaw( itCSI0.GetVoxel() );
	  }

	  map = CSI0.GetHeader().GetVariableVolumeHeader();
	  Ocsi0.SetHdrMap(map);
	  Ocsi0.Write(destFilePath+"_0.csi", false);
	}

  return(true);
} // CreateOldFormatOutput()



#undef FILE_REVISION
  
// $Log: vxPipelineStages.C,v $
// Revision 1.23.2.3  2010/02/12 20:38:34  dongqing
// comments out debug output
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.23.2.2  2008/09/23 15:13:10  dongqing
// add reset EC indicator before the tagged threhsold checking skipping EC
//
// Revision 1.23.2.1  2008/09/04 16:18:21  dongqing
// fix the bug of checking percentage of tagged voxel in body ROI
// Use the value from Setting file rather than fixed value.
//
// Revision 1.23  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.22  2006/09/05 15:29:54  dongqing
// write out ULD for fast flying when "s" key on
//
// Revision 1.21  2006/08/31 20:57:57  dongqing
// made change to use new vxBlockVolume coordinate system
// That is the patient natural coordinate system
//
// Revision 1.20  2006/08/29 18:38:29  gchin
// Changes from Dongqing for Dr. Cadi
//
// Revision 1.19  2006/06/22 19:28:26  dongqing
// add vxBlockVolume::Compact to save memory space
//
// Revision 1.18  2006/06/06 20:14:02  dongqing
// fix fill cavity bug for the vxSegmentFinder
//
// Revision 1.17  2006/05/30 13:13:45  dongqing
// switch to vxBlockVolumeReader
//
// Revision 1.16  2006/05/19 20:31:11  dongqing
// fix bug of reading CDV from sourceFilePath
//
// Revision 1.15  2006/05/17 12:41:16  dongqing
// fix the clamping bug in the CreateOldFormatOutput
//
// Revision 1.14  2006/05/05 16:37:32  dongqing
// change auto CRES detector threshold from 3.6% to 2.8%
//
// Revision 1.13  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.12  2006/04/19 19:53:45  dongqing
// bug free beta version
//
// Revision 1.11  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.10  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.9  2006/03/14 21:22:45  dongqing
// 1st step tune up
//
// Revision 1.8  2006/03/13 18:39:24  dongqing
// finialize SKE codes
//
// Revision 1.7  2006/03/10 16:39:05  dongqing
// replace the iterator for better speed
//
// Revision 1.6  2006/03/09 18:40:13  dongqing
// ad vxSkeletonator class initial version
//
// Revision 1.5  2006/03/07 13:30:18  dongqing
// initial check in the TRI stage codes
//
// Revision 1.4  2006/03/03 20:47:00  dongqing
// finalize DFB codes
//
// Revision 1.3  2006/03/02 21:22:43  dongqing
// check in initial DFB module
//
// Revision 1.2  2006/02/24 20:24:17  dongqing
// add INS module
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxPipelineStages.C,v 1.23.2.3 2010/02/12 20:38:34 dongqing Exp $
// $Id: vxPipelineStages.C,v 1.23.2.3 2010/02/12 20:38:34 dongqing Exp $

