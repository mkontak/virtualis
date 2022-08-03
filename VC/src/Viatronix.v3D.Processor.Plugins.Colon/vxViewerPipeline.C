// $Id: vxViewerPipeline.C,v 1.0 2011/04/21  dongqing Exp $
//
// Copyright(c) 2000-2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this
// file may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen dongqing@viatronix.net
//
// Complete History at bottom of file.


#include "stdafx.h"

#include "vxViewerPipeline.h"
#include "Timer.h"
#include "Date.h"
#include "File.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxVolumeHeader.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"
#include "vxIntensityConverter.h"
#include "Logger.h"

#include "vxBackgroundRemoval.h"
#include "vxLungRemoval.h"
#include "vxBoneRemoval.h"
#include "vxCRES.h"
#include "vxSegmentFinder.h"
#include "vxDistanceFromBoundary.h"
#include "vxTriangulator.h"
#include "vxSkeletonator.h"


using namespace std;
using namespace hdr;
using namespace vx;

#define FILE_REVISION "$Revision: 1.0 $"

/**
 * Constructor.
 * 
 * @param cmdLine Command line argument count
 */
vxViewerPipeline::vxViewerPipeline()
{
  m_fResiduePercent = 0.02F;
} // constructor


/**
  * destructor
	*/
vxViewerPipeline::~vxViewerPipeline()
{
} // destructor


/**
  * read volume
	*
	* @param pVol the pointer to the input volume
	* @param sFilename the input volume file name
	*/
void vxViewerPipeline::ReadVolume( vxBlockVolume<uint2> * pVol, const std::string & sFilename )
{
	try
	{
		vxBlockVolumeFileReader< uint2,  vxHistogramUpdaterIntensity  >::Read( sFilename, *pVol );
	}
	catch( ... )
	{
		throw ex::AbortLoadException( LogRec("    Fail to load intensity volume", "vxViewerPipeline.C", "ReadVolume") );
	}

	m_vHeader = pVol->GetHeader();
	m_iX = m_vHeader.GetVolDim().m_x;
	m_iY = m_vHeader.GetVolDim().m_y;
	m_iZ = m_vHeader.GetVolDim().m_z;
	m_iS = m_iX*m_iY;
	m_iN = m_iZ*m_iS;

	if( m_iN == 0 )
		throw ex::AbortLoadException( LogRec("    Intensity volume corrupted", "vxViewerPipeline.C", "ReadVolume") );
} // ReadVolume


/**
  * get the image orientation fromt the DICOM header (orientation cosine)
	*/
void vxViewerPipeline::ComputeImageOrientation()
{
  /// get the image orientation cosin
  std::string sOrientationCosine;
	sOrientationCosine = m_vHeader.GetDicomFieldValue("0020", "0037", sOrientationCosine);

  /// determine the body position based on the cosine
  std::string sBreaker = "/\\,:";
  int4 i, n = sOrientationCosine.size();
  int4 j[5], k = 0;
  for( i=0; i<5; i++ )
    j[i] = -1;

  for( i=0; i<n; i++ )
  {
    if( sOrientationCosine[i] == sBreaker[0] || sOrientationCosine[i] == sBreaker[1] || 
        sOrientationCosine[i] == sBreaker[2] || sOrientationCosine[i] == sBreaker[3] )
    {
      j[k] = i;
      k++;
    } // if
  } // for i

  if( j[0] < 0 || j[1] < 0 || j[2] < 0 || j[3] < 0 || j[4] < 0 )
  {
    m_sImageOrientation = "unsupport body position";
    LogWrn("Could not retrieve image orientation patient info correctly",  "vxViewerPipeline.C", "GetImageOrientation");
  } // if
  else
  { /// get the orientation cosine
    std::string srx, sry, srz, scx, scy, scz;
    float4      frx, fry, frz, fcx, fcy, fcz;
    for( i=0; i<j[0]; i++ )
      srx.push_back(sOrientationCosine[i]);

    ToType(srx, frx);

    for( i=j[0]+1; i<j[1]; i++ )
      sry.push_back(sOrientationCosine[i]);

    ToType(sry, fry);

    for( i=j[1]+1; i<j[2]; i++ )
      srz.push_back(sOrientationCosine[i]);

    ToType(srz, frz);

    for( i=j[2]+1; i<j[3]; i++ )
      scx.push_back(sOrientationCosine[i]);

    ToType(scx, fcx);

    for( i=j[3]+1; i<j[4]; i++ )
      scy.push_back(sOrientationCosine[i]);

    ToType(scy, fcy);

    for( i=j[4]+1; i<n; i++ )
      scz.push_back(sOrientationCosine[i]);

    ToType(scz, fcz);

    if( fcy == 1.0F && frx == 1.0F )
    {
      m_sImageOrientation = "HFS";
    }
    else if( fcy == 1.0F && frx == -1.0F )
    {
      m_sImageOrientation = "FFS";
    }
    else if( fcy == -1.0F && frx == -1.0F )
    {
      m_sImageOrientation = "HFP";
    }
    else if( fcy == -1.0F && frx == 1.0F )
    {
      m_sImageOrientation = "FFP";
    }
    else if( fry == 1.0F && fcx == -1.0F )
    {
      m_sImageOrientation = "HFDR";
    }
    else if( fry == -1.0F && fcx == -1.0F )
    {
      m_sImageOrientation = "FFDR";
    }
    else if( fry == -1.0F && fcx == 1.0F )
    {
      m_sImageOrientation = "HFDL";
    } // else
    else if( fry == 1.0F && fcx == 1.0F )
    {
      m_sImageOrientation = "FFDL";
    } // else
    else
    {
      m_sImageOrientation = "unsupport body position";
      LogWrn("Body position in imaging is not axial", "vxViewerPipeline.C", "GetImageOrientation");
    } // if
  } // else
} // GetImageOrientation


/**
 *  format the linear volume from new to old volume layout
 *
 *  @param pVol  the pointer to the input block volume
 *  @param pLVol the pointer of the input linear volume
 */
void vxViewerPipeline::PrepareLinearVolume( vxBlockVolume<uint2> * pVol, uint2 * pLVol )
{
	try
	{
		const int4 iz1 = m_iZ/3;
		const int4 iz2 = 2*iz1;

		PrepareLinearVolThread  cv0( pVol, pLVol,   0, iz1, m_sImageOrientation );
		cv0.Start();

		PrepareLinearVolThread  cv1( pVol, pLVol, iz1, iz2, m_sImageOrientation );
		cv1.Start();

		PrepareLinearVolThread  cv2( pVol, pLVol, iz2,m_iZ, m_sImageOrientation );
		cv2.Start();

		cv0.End();
		cv1.End();
		cv2.End();
	}
	catch( ... )
	{
	  throw ex::AbortLoadException(LogRec("    Error occured in preparation of linear vol", "vxViewerPipeline.C", "PrepareLinearVolThread::Run"));
	}

} //ReformatFromNew2OldLayout


 /**
  *  thread implementation for volume copy, reformat, and clamping from block to linear
	*
	*  @param pVol the pointer of orignal intensity block volume
	*  @param pLVol the pointer of the linear volume
	*  @param iZMin the low bound for slice index
	*  @param iZMax the upper bound for slice index
	*  @param sImageOrientation is the string for image orientation information
	*/
vxViewerPipeline::PrepareLinearVolThread::PrepareLinearVolThread( vxBlockVolume<uint2> * pVol, uint2 * pLVol, 
	  const int4 & iZMin, const int4 & iZMax, const std::string & sImageOrientaiton )
	 : m_pVol( pVol ), m_pLVol( pLVol ), m_iZMin( iZMin ), m_iZMax( iZMax ), m_sImageOrientation( sImageOrientaiton )
 {
 } //PrepareLinearVolThread::PrepareLinearVolThread


 /** 
  *  HandleVolumeThread::Run copy block volume to linear volume and 
	*  reformat the new layout to old layout
	*/
void vxViewerPipeline::PrepareLinearVolThread::Run()
{
	 const int4 iX( m_pVol->GetHeader().GetVolDim().m_x );
	 const int4 iY( m_pVol->GetHeader().GetVolDim().m_y );
	 const int4 iZ( m_pVol->GetHeader().GetVolDim().m_z );
	 const int4 iS( iX*iY );
	 const int4 iStart( m_iZMin*iS );

	 if( m_iZMax > iZ || m_iZMin < 0 ) 
   {
		 throw ex::AbortDataProcessingException(LogRec("    Incorrect z-slice index", "vxViewerPipeline.C", "PrepareLinearVolThread::Run"));
		 return;
	 } // if

	 vxBlockVolumeIterator<uint2> it( *m_pVol );
	 register int4 x, y, z, i = iStart;

	 /// copy the voxel value to the linear volume
	 for( z=m_iZMin, it.SetPosZ(m_iZMin); z<m_iZMax; z++, it.IncZ() )
	 {
		 for( y=0, it.SetPosY(0); y<iY; y++, it.IncY() )
		 {				
			 for( x=0, it.SetPosX(0); x<iX; x++, it.IncX() )
			 {
			   m_pLVol[i] = it.GetVoxel();
			   i++;
			 } // for x
		 } // for y
	 } // for z
	  
	 /// temporary slice mask
	 uint2 * pS = new uint2[ iS ];
	 if( pS == NULL )
	 {
		 throw ex::OutOfMemoryException(LogRec("    Out of memory", "vxViewerPipeline.C", "Run"));
		 return;
	 } // if

	 register int4 j, k, m;
	 const int4 ix(iX-1);
	 const int4 iy(iY-1);

	 /// reformat the volume to align image along gravitational direction
	 if( m_sImageOrientation[2] == 'P' )
	 { // prone position
		 k = iStart;
		 for( z=m_iZMin; z<m_iZMax; z++ )
		 {
			 i = 0;
			 m = k;
			 /// copy slice to buffer
			 for( y=0; y<iS; y++ )
			 {
					pS[y] = m_pLVol[m];
					m++;
			 } // for y

			 j = iy*iX;
			 m = k;
			 /// reformat slice
			 for( y=0; y<iY; y++ )
			 {
				 i = ix;
				 for( x=0; x<iX; x++ )
				 {
					 m_pLVol[m] = pS[i+j];
					 i--;
					 m++;
				 } // for x

				 j = j-iX;
			 } // for y

			 k = k + iS;
		 } // for z
	 } // if 
	 
	 else if( m_sImageOrientation[2] == 'D' && m_sImageOrientation[3] == 'R' )
	 { // decubitus right position
		 k = iStart;
		 for( z=m_iZMin; z<m_iZMax; z++ )
		 {
			 i = 0;
			 m = k;
			 /// copy slice to buffer
			 for( y=0; y<iS; y++ )
			 {
					pS[y] = m_pLVol[m];
					m++;
			 } // for y

			 j = iy;
			 m = k;
			 /// reformat slice
			 for( y=0; y<iY; y++ )
			 {
				 i = 0;

				 for( x=0; x<iX; x++ )
				 {
					 m_pLVol[m] = pS[j+i];

					 i = i+iX;
					 m++;
				 } // for x

				 j--;
			 } // for y

			 k = k + iS;
		 } // for z
	 } // else if
	 
	 else if( m_sImageOrientation[2] == 'D' && m_sImageOrientation[3] == 'L' )
	 { // decubitus left position
		 k = iStart;
		 for( z=m_iZMin; z<m_iZMax; z++ )
		 {
			 i = 0;
			 m = k;
			 /// copy slice to buffer
			 for( y=0; y<iS; y++ )
			 {
				 pS[y] = m_pLVol[m];
				 m++;
			 } // for y

			 m = k;
			 /// reformat slice
			 for( y=0; y<iY; y++ )
			 {
				 i = ix*iX;

				 for( x=0; x<iX; x++ )
				 {
					 m_pLVol[m] = pS[y+i];
					 i = i-iX;
					 m++;
				 } // for x
			 } // for y

			 k = k + iS;
		 } // for z
	 } // else if 

	 delete pS;   /// release memory

} // PrepareLinearVolThread::Run


/** 
  *  check if the intensity range is abnormal
	*
	*  @param pVol the pointer to the input linear volume
	*/
bool vxViewerPipeline::CheckIntensityRange( uint2 * pVol )
{
	if( m_iS == 0 )
	{
		LogErr("    The volume dimension has not been initialized", "vxViewerPipeline.C", "CheckIntensityRange");
		return( false );
	}

	const uint2 uValueHigh = vxIntensityConverter::ModalityToRaw( 3095, m_vHeader );
	const uint2 uValueLow  = vxIntensityConverter::ModalityToRaw( -300, m_vHeader );

	const int4 iStart( m_iS*(m_iZ/2) );
	const int4 iEnd  ( iStart + m_iS );
	register int4 iHigh  (0);
	register int4 iTissue(0);
	register int4 i;

	try
	{
		// check if intensity range is normal or not
		for( i=iStart; i<iEnd; i++)
		{
			if( pVol[i] > uValueHigh )
				iHigh++;
			else if( pVol[i] > uValueLow )
				iTissue++;
		} // for i
    
		if( (static_cast<float4>(iHigh)  /static_cast<float4>(m_iS) > 0.3F) ||
				(static_cast<float4>(iTissue)/static_cast<float4>(m_iS) < 0.1F) )
		{
			return(false);  
		} // if 
	}
	catch( ... )
	{
		throw ex::AbortDataProcessingException( LogRec("    Error occured in check intensity range", "vxViewerPipeline.C", "CheckIntensityRange") );
	}

  return(true);
} //CheckIntensityRrange


/**
  *  extract body region
	*
	*  @param pVol the pointer to the linear volume
	*  @param pMask the pointer to the mask volume
	*/
void vxViewerPipeline::ComputeBodyRegion( uint2 * pVol, uint1 * pMask )
{
  try
  { 
    vxBackgroundRemoval BGRemoval( m_vHeader, pVol, pMask );
    BGRemoval.Run();

		vxLungRemoval lungRemoval( m_vHeader, pVol, pMask );
		lungRemoval.Run();
  }
  catch( ... )
  {
    throw ex::AbortOperationException( LogRec("    Error occured in ComputeBodyRegion", "vxViewerPipeline.C", "ComputeBodyRegion") );
  }
} // ComputeBodyRegion


/**
 *  count tagged voxel and clamp the linear volume
 *
 * @param pVol the pointer to the input linear volume
 * @param pMask the pointer to the mask volume
 */
void vxViewerPipeline::CountTaggedVoxel( uint2 * pVol, uint1 * pMask )
{
  try
  { 
		const int4 i1( m_iN/3 );
		const int4 i2( i1*2 );
		int4 x,y,z;
		x = 0;
		y = 0;
		z = 0;

		CountTaggedVoxelThread ct1( m_vHeader, pVol, pMask, 0,    i1, x );
		ct1.Start();
		CountTaggedVoxelThread ct2( m_vHeader, pVol, pMask, i1,   i2, y );
		ct2.Start();
		CountTaggedVoxelThread ct3( m_vHeader, pVol, pMask, i2, m_iN, z );
		ct3.Start();

		ct1.End();
		ct2.End();
		ct3.End();

		x = x+y+z;

		int4 iNumOfBodyVoxel( 0 );
		m_vHeader.GetVariableVolumeHeader().GetHdrVar("numOfBodyVoxel", iNumOfBodyVoxel);
		float4 f(static_cast<float4>(x)/static_cast<float4>(iNumOfBodyVoxel));
  
		if( f < m_fResiduePercent )  // This threshold must be robust and accurate enough !! However, skip EC does not hurt clinic diagnosis
		{ 
			LogWrn("    High intensity voxels are very few", "vxViewerPipeline.C", "CountTaggedVoxel");
		}
  }
  catch( ... )
  {
    throw ex::AbortOperationException( LogRec("    Error occured in ComputeCRES", "vxViewerPipeline.C", "ComputeBodyRegion") );
  } 
} // CountTaggedVoxel


/**
 *  constructor: thread implementation for counting the number of tagged voxels
 *
 *  @param vHeader the block volume header
 *  @param pVol the pointer to the input liinear CT volume
 *  @param pMask the pointer to the mask volume
 *  @param iStart the start volume offset
 *  @param iEnd the end volume offset
 *  @param iNumOfTaggedVoxel the voxel counter
 */
vxViewerPipeline::CountTaggedVoxelThread::CountTaggedVoxelThread( vxVolumeHeader & vHeader, uint2 *pVol, uint1 * pMask, 
			  const int4 & iStart, const int4 & iEnd, int4 & iNumOfTaggedVoxel )
 : m_vHeader( vHeader ), m_pVol( pVol ), m_pMask( pMask ), m_iStart( iStart ), 
   m_iEnd( iEnd ), m_iCounter( iNumOfTaggedVoxel )
{
} // CountTaggedVoxelThread


/**
 *  Thread run function for CountTaggedVoxelThread
 */
void vxViewerPipeline::CountTaggedVoxelThread::Run()
{
  // background label index in the mask
  const uint1 uBGlabel(0xFF);
  const uint2 uHigh       = vxIntensityConverter::ModalityToRaw( 220, m_vHeader ); 
	const uint2 uClampBound = vxIntensityConverter::ModalityToRaw( 900, m_vHeader ); 

	register int4 i, j=0;

	for( i=m_iStart; i<m_iEnd; i++ )
	{
		if( m_pMask[i] == 0x00 && m_pVol[i] > uHigh )
		{
			j++;
			if( m_pVol[i] > uClampBound ) /// clamp high HU value, the metalic object will look like bone now
			{
				m_pVol[i] = uClampBound;
			} // if
		} // if
	} // for i

	m_iCounter = j;

} // CountTaggedVoxelThread::Run()


/**
 *  remove bone region from body region
 *
 * @param pVol the pointer to the input linear volume
 * @param pMask the pointer to the mask volume
 */
void vxViewerPipeline::RemoveBone( uint2 * pVol, uint1 * pMask )
{
  try
  { 
		vxBoneRemoval removeBone( m_vHeader, pVol, pMask );
		removeBone.Run();
  }
  catch( ... )
  {
    throw ex::AbortOperationException( LogRec("    Error occured in RemoveBone", "vxViewerPipeline.C", "ComputeBodyRegion") );
  } 
} // RemoveBone


/**
 *  apply electronic cleansing
 *
 * @param pVol the pointer to the input linear volume
 * @param pMask the pointer to the mask volume
 */
void vxViewerPipeline::ComputeCRES( uint2 * pVol, uint1 * pMask )
{
  try
  { 
		vxCRES cres( m_vHeader, pVol, pMask );
		cres.Run();
  }
  catch( ... )
  {
    throw ex::AbortOperationException( LogRec("    Error occured in ComputeCRES", "vxViewerPipeline.C", "ComputeBodyRegion") );
  } 
} // ComputeCRES


/**
 *  reformate the linear and mask volume back into new block volume layout
 *
 * @param pVol the pointer to the input linear volume
 * @param pMask the pointer to the mask volume
 */
void vxViewerPipeline::ReformatBack2NewLayout( uint2 * pVol, uint1 * pMask )
{
	try
	{
		const int4 iz1 = m_iZ/3;
		const int4 iz2 = 2*iz1;

		ReformatBack2NewLayoutThread cv0( m_vHeader, pVol, pMask,   0, iz1, m_sImageOrientation );
		cv0.Start();

		ReformatBack2NewLayoutThread cv1( m_vHeader, pVol, pMask, iz1, iz2, m_sImageOrientation );
		cv1.Start();

		ReformatBack2NewLayoutThread cv2( m_vHeader, pVol, pMask, iz2,m_iZ, m_sImageOrientation );
		cv2.Start();

		cv0.End();
		cv1.End();
		cv2.End();
	}
	catch( ... )
	{
    throw ex::AbortOperationException( LogRec("    Error occured in reformat back", "vxViewerPipeline.C", "ReformatBack2NewLayout") );
	}
} //ReformatBack2NewLayout


/**
 *  constructor: thread implementation for counting the number of tagged voxels
 *
 *  @param vHeader the block volume header
 *  @param pVol the pointer to the input liinear CT volume
 *  @param pMask the pointer to the mask volume
 *  @param iStart the start volume offset
 *  @param iEnd the end volume offset
 *  @param iNumOfTaggedVoxel the voxel counter
 */
vxViewerPipeline::ReformatBack2NewLayoutThread::ReformatBack2NewLayoutThread( vxVolumeHeader & vHeader, uint2 * pVol, 
	  uint1 * pMask, const int4 & iZMin, const int4 & iZMax, const std::string & sImageOrientation )
 : m_vHeader( vHeader ), m_pVol( pVol ), m_pMask( pMask ), m_iZMin( iZMin ), 
   m_iZMax( iZMax ), m_sImageOrientation( sImageOrientation )
{
} // ReformatBack2NewLayoutThread


/**
 *  Thread run function for CountTaggedVoxelThread
 */
void vxViewerPipeline::ReformatBack2NewLayoutThread::Run()
{
	 const int4 iX( m_vHeader.GetVolDim().m_x );
	 const int4 iY( m_vHeader.GetVolDim().m_y );
	 const int4 iZ( m_vHeader.GetVolDim().m_z );
	 const int4 iS( iX*iY );
	 const int4 iStart( m_iZMin*iS );

	 if( m_iZMax > iZ || m_iZMin < 0 ) 
   {
		 throw ex::AbortDataProcessingException(LogRec("    Incorrect z-slice index", "vxViewerPipeline.C", "ReformatBack2NewLayoutThread::Run"));
		 return;
	 } // if

	 /// temporary slice mask
	 uint2 *pS = new uint2[ iS ];
	 uint1 *pC = new uint1[ iS ];
	 if( pS == NULL || pC == NULL )
	 {
		 throw ex::OutOfMemoryException(LogRec("    Incorrect z-slice index", "vxViewerPipeline.C", "ReformatBack2NewLayoutThread::Run"));
	 } // if

	 register int x, y, z, i, j, k, m;
	 const int ix(iX-1);
	 const int iy(iY-1);

	 if( m_sImageOrientation[2] == 'P' )
	 { // prone position
		 k = iStart;
		 for( z=m_iZMin; z<m_iZMax; z++ )
		 {
			 i = 0;
			 m = k;
			 /// copy slice to buffer
			 for( y=0; y<iS; y++ )
			 {
				 pS[y] = m_pVol[m];
				 pC[y] = m_pMask[m];
				 m++;
			 } // for y

			 j = iy*iX;
			 m = k;
			 /// reformat slice
			 for( y=0; y<iY; y++)
			 {
				 i = ix;

				 for( x=0; x<iX; x++)
				 {
					 m_pVol[m]  = pS[i+j];
					 m_pMask[m] = pC[i+j];
					 i--;
					 m++;
				 } // for x

				 j = j-iX;
			 } // for y

			 k = k + iS;
		 } // for z
	 } // if

	 else if( m_sImageOrientation[2] == 'D' && m_sImageOrientation[3] == 'R' )
	 { // decubitus right position
		 k = iStart;
		 for( z=m_iZMin; z<m_iZMax; z++ )
		 {
			 i = 0;
			 m = k;
			 /// copy slice to buffer
			 for( y=0; y<iS; y++ )
			 {
				 pS[y] = m_pVol[m];
				 pC[y] = m_pMask[m];
				 m++;
			 } // for y

			 m = k;
			 /// reformat slice
			 for( y=0; y<iY; y++ )
			 {
				 i = ix*iX;

				 for( x=0; x<iX; x++ )
				 {
					 m_pVol[m]  = pS[y+i];
					 m_pMask[m] = pC[y+i];

					 i = i-iX;
					 m++;
				 } // for x
			 } // for y

			 k = k + iS;
		 } // for z
	 } // else if

	 else if( m_sImageOrientation[2] == 'D' && m_sImageOrientation[3] == 'L' )
	 {// decubitus left position
		 k = iStart;
		 for( z=m_iZMin; z<m_iZMax; z++ )
		 {
			 i = 0;
			 m = k;

			 /// copy slice to buffer
			 for( y=0; y<iS; y++ )
			 {
				 pS[y] = m_pVol[m];
				 pC[y] = m_pMask[m];
				 m++;
			 } // for y

			 j = iy;
			 m = k;
			 /// reformat slice
			 for( y=0; y<iY; y++)
			 {
				 i = 0;

				 for( x=0; x<iX; x++ )
				 {
					 m_pVol[m]  = pS[j+i];
					 m_pMask[m] = pC[j+i];

					 i = i+iX;
					 m++;
				 } // for x

				 j--;
			 } // for y

			 k = k + iS;
		 } // for z
	 } // else if

	 delete pS;
} // ReformatBack2NewLayoutThread::Run()


/**
 *  compute inside colon segments
 *
 * @param pVol  the pointer to the input linear volume
 * @param pMask the pointer to the mask linear volume
 * @param pIns  the pointer to the INS linear volume
 */
void vxViewerPipeline::ComputeInsideSegments( uint2 * pVol, uint1 * pMask, uint1 * pIns )
{
	try
	{
	  vxSegmentFinder computeINS( m_vHeader, pVol, pMask, pIns );
	  computeINS.Run();
	}
	catch( ... )
	{
    throw ex::AbortOperationException( LogRec("    Error occured in ComputeINS", "vxViewerPipeline.C", "ComputeInsideSegments") );
	} // catch
} //ComputeInsideSegments


/**
 *  compute the un-cleansed INS volume
 *
 *  @param pv the pointer to the original intensity block volume
 *  @param pIns the pointer to the linear INS volume
 *  @param pInsNoec the pointer to the linear noncleansed INS volume
 */
void vxViewerPipeline::ComputeUncleansedINS( vxBlockVolume<uint2> * pv, uint1 * pIns, uint1 * pInsNoec )
{
	vxBlockVolumeIterator<uint2> itv( * pv );

	const uint2 uAirTh( vxIntensityConverter::ModalityToRaw(-760, pv->GetHeader()) );   // hard-coded threshold

	register int4 x, y, z, i;
	i = 0;
	for( z=0; z<m_iZ; z++ )
	{
		for( y=0; y<m_iY; y++ )
		{
			for( x=0; x<m_iX; x++ )
			{
				if( pIns[i] > 0x00 )
				{
					itv.SetPos(x,y,z);
					if( itv.GetVoxel() < uAirTh )
					{
						pInsNoec[i] = pIns[i];
					} // if
				} // if

				i++;
			} // for x
		} // for y
	} // for z	
} // ComputeUncleansedINS


/**
 *  compute SLD volume and TRI
 *
 *  @param pMask the pointer to the linear mask volume
 *  @param pIns the pointer to the linear INS volume
 *  @param pInsNoec the pointer to the linear noncleansed INS volume
 *  @param pSld the pointer to the cleansed space leap distance volume
 *  @param pUld the pointer to the uncleansed space leap distance volume
 */
void vxViewerPipeline::ComputeSLDandTRI( uint1 * pMask, uint1 * pIns, uint1 * pInsNoec, 
	      uint1 * pSld, uint1 * pUld, std::string & sTemporaryDirectory )
{
	try
	{
		vxDistanceFromBoundary computeSLD( m_vHeader,     pIns, pSld );
		computeSLD.Start();

		vxDistanceFromBoundary computeULD( m_vHeader, pInsNoec, pUld );
		computeULD.Start();
	
		vxTriangulator cTRI( pMask, sTemporaryDirectory, m_vHeader );
	  cTRI.Start();

		computeSLD.End();
		computeULD.End();
		cTRI.End();
	}
	catch( ... )
	{
    throw ex::AbortOperationException( LogRec("    Error occured in ComputeSLDandTRI", "vxViewerPipeline.C", "ComputeSLDandTRI") );
	}
} // ComputeSLDandTRI


/**
 *  compute PDF and CSI
 * 
 *  @param vHeader
 *  @param pIns
 *  @param pSld
 *  @param pPdr0
 *  @param pPdr1
 *  @param pCsi
 */
void vxViewerPipeline::ComputePDFandCSI( vxBlockVolume<uint1> * pIns, uint1 * pSld,
		vxBlockVolume<float4> * pPdr0, vxBlockVolume<float4> * pPdr1, vxBlockVolume<uint2> * pCsi,
		std::string & sTemporaryDirectory )
{
	vxBlockVolume<uint1> * pBlockSld = new vxBlockVolume<uint1>( m_vHeader );

	const int4 iX( m_vHeader.GetVolDim().m_x );
	const int4 iY( m_vHeader.GetVolDim().m_y );
	const int4 iZ( m_vHeader.GetVolDim().m_z );
	const int4 iS( iX*iY );
	const int4 iN( iZ*iS );

	try
	{
		vxSkeletonator ske( pIns, pSld, pPdr0, pPdr1, pCsi, sTemporaryDirectory );
		ske.Run();
	}
	catch( ... )
	{
    throw ex::AbortOperationException( LogRec("    Error occured in ComputePDFandCSI", "vxViewerPipeline.C", "ComputePDFandCSIs") );
	}

	delete pBlockSld;
} ///ComputePDFandCSI



#undef FILE_REVISION
  
// $Log: vxViewerPipeline.C,v $
// Revision 1.0 2011/04/21   dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxViewerPipeline.C,v 1.0 2011/04/21  dongqing Exp $
// $Id: vxViewerPipeline.C,v 1.0 2011/04/21  dongqing Exp $
