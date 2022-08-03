// $Id: Fields_ext.C,v 1.4.2.7 2008/02/28 23:10:25 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Dongqing Chen (dongqing@viatronix.com )


// includes
#include "StdAfx.h"
#include <limits>
#include "Fields.h"
#include "ReaderGlobal.h"
#include "Volume.h"
#include "Timer.h"
#include "File.h"
#include "logger.h"
#include "VCSettings.h"
#include "vxVolumeHeader.h"
#include "FileExtensions.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"


// defines
#define FILE_REVISION "$Revision: 1.4.2.7 $"
#ifdef max
#undef max
#endif

using namespace ReaderLib;


/**
 * Load all information from disk using vxBlockVolumeFileReader
 *
 * @param sVolumePreFix the path with volume filename prefix
 * @param sPreprocessedPrePix the path with volume filename prefix
 */
void Fields::LoadVolumeFields3( const std::string & sVolumePrefix, const std::string & sPreprocessedPrefix )
{
  // prepare all the file names
  const std::string sCdvFile( sPreprocessedPrefix + vx::CleansedDifferenceVolExt );
  const std::string sVolFile( sVolumePrefix + vx::VolumeExt );

	// read original intensity volumes
  vxBlockVolume<uint2> *pv = new vxBlockVolume<uint2>;
	if( pv == NULL )
	  throw ex::AbortLoadException(LogRec( "No memory for intensity volume", "Fields_ext", "LoadVolumeFields3" ));

	try
	{
		vxBlockVolumeFileReader< uint2,  vxHistogramUpdaterIntensity  >::Read( sVolumePrefix + vx::VolumeExt, *pv );
	  pv->Compact(); // This is necessary for compact memory usage. If the vxBlockVolumeFileReader change, this may not be necessary.
	}
	catch( ... )
	{
		throw ex::AbortLoadException( LogRec("Fail to load intensity volume", "Fields_ext", "LoadVolumeFields3") );
	}
	vxBlockVolumeIterator<uint2> itv(*pv);

  /// set the offset for converting VX world to DICOM world coordinate
  /// this is for quality assurance in general setting, where finding needs to be compared against to 
  /// a 3rd party application based on DICOM info
  try
  { 

    /// get the image orientation cosine
    std::string sImageOrientation("");
    sImageOrientation = pv->GetHeader().GetDicomFieldValue("0020", "0037", sImageOrientation);

    /// determine the body position based on the cosine
    {
      std::string sBreaker = "/\\,:";
      int4 i, n = sImageOrientation.size();
      int4 j[5], k = 0;
      for( i=0; i<5; i++ )
        j[i] = -1;

      for( i=0; i<n; i++ )
      {
        if( sImageOrientation[i] == sBreaker[0] || sImageOrientation[i] == sBreaker[1] || 
            sImageOrientation[i] == sBreaker[2] || sImageOrientation[i] == sBreaker[3] )
        {
          j[k] = i;
          k++;
        } // if
      } // for i

      if( j[0] < 0 || j[1] < 0 || j[2] < 0 || j[3] < 0 || j[4] < 0 )
      {
        m_sPatientOrientation = "unsupport body position";
        LogWrn("Could not retrieve image orientation patient info correctly",  "Fields_ext", "LoadVolumeFields3");
      } // if
      else
      { /// get the orientation cosine
        std::string srx, sry, srz, scx, scy, scz;
        float4      frx, fry, frz, fcx, fcy, fcz;
        for( i=0; i<j[0]; i++ )
          srx.push_back(sImageOrientation[i]);

        ToType(srx, frx);

        for( i=j[0]+1; i<j[1]; i++ )
          sry.push_back(sImageOrientation[i]);

        ToType(sry, fry);

        for( i=j[1]+1; i<j[2]; i++ )
          srz.push_back(sImageOrientation[i]);

        ToType(srz, frz);

        for( i=j[2]+1; i<j[3]; i++ )
          scx.push_back(sImageOrientation[i]);

        ToType(scx, fcx);

        for( i=j[3]+1; i<j[4]; i++ )
          scy.push_back(sImageOrientation[i]);

        ToType(scy, fcy);

        for( i=j[4]+1; i<n; i++ )
          scz.push_back(sImageOrientation[i]);

        ToType(scz, fcz);

        if( fcy == 1.0F && frx == 1.0F )
        {
          m_sPatientOrientation = "HFS";
        }
        else if( fcy == 1.0F && frx == -1.0F )
        {
          m_sPatientOrientation = "FFS";
        }
        else if( fcy == -1.0F && frx == -1.0F )
        {
          m_sPatientOrientation = "HFP";
        }
        else if( fcy == -1.0F && frx == 1.0F )
        {
          m_sPatientOrientation = "FFP";
        }
        else if( fry == 1.0F && fcx == -1.0F )
        {
          m_sPatientOrientation = "HFDR";
        }
        else if( fry == -1.0F && fcx == -1.0F )
        {
          m_sPatientOrientation = "FFDR";
        }
        else if( fry == -1.0F && fcx == 1.0F )
        {
          m_sPatientOrientation = "HFDL";
        } // else
        else if( fry == 1.0F && fcx == 1.0F )
        {
          m_sPatientOrientation = "FFDL";
        } // else
        else
        {
          m_sPatientOrientation = "unsupport body position";
          LogWrn("Body position in imaging is not axial",  "Fields_ext", "LoadVolumeFields3");
        } // if
      } // else

    } // end block for determining the body position based on the cosine

    /// get the image position patient
    std::string sImagePosition("");
		sImagePosition = pv->GetHeader().GetDicomFieldValue("0020", "0032", sImagePosition);

    if( sImagePosition.size() > 3 )
    {
      std::string sBreaker = "/\\,:";
      int4 i, n = sImagePosition.size();
      int4 j[3], k = 0;
      j[0] = -1; 
      j[1] = -1;

      for( i=0; i<n; i++ )
      {
        if( sImagePosition[i] == sBreaker[0] || sImagePosition[i] == sBreaker[1] || sImagePosition[i] == sBreaker[2] || sImagePosition[i] == sBreaker[3] )
        {
          j[k] = i;
          k++;
        }
      } // for i

      std::string sX;
      for( i=0; i<j[0]; i++ )
        sX.push_back(sImagePosition[i]);
      
      ToType(sX, m_vfShiftOffsetForDICOMWorldCoordinate.m_x);

      std::string sY;
      for( i=j[0]+1; i<j[1]; i++ )
        sY.push_back(sImagePosition[i]);
      
      ToType(sY, m_vfShiftOffsetForDICOMWorldCoordinate.m_y);

      std::string sZ;
      for( i=j[1]+1; i<n; i++ )
        sZ.push_back(sImagePosition[i]);
      
      ToType(sZ, m_vfShiftOffsetForDICOMWorldCoordinate.m_z);
    } // if
    else
    {
      m_vfShiftOffsetForDICOMWorldCoordinate = Vector3D<float4>(0.0F,0.0F,0.0F);
    } // else
  } // end set the offset
  catch( ... )
  {
    LogWrn("Error occurred in extraction of DICOM field -- image position patient", "Fields_ext", "LoadVolumeFields3");
  }

	/// read CDV volume
  vxBlockVolume<uint2> *pcdv = new vxBlockVolume<uint2>;
	if( pcdv == NULL )
	  throw ex::AbortLoadException( LogRec( "No memory for CDV", "Fields_ext", "LoadVolumeFields3") );

	try
	{
    vxBlockVolumeFileReader< uint2 >::Read( sPreprocessedPrefix + vx::CleansedDifferenceVolExt, *pcdv );
		pcdv->Compact();

    // set cleansed flag
    vxVolumeHeader hCDV;
    int4 iBoolIsCleansed;
    hCDV.Read( sPreprocessedPrefix + vx::CleansedDifferenceVolExt );
    VarMap map( hCDV.GetVariableVolumeHeader() );
    if (map.GetHdrVar("IsEcleansed", iBoolIsCleansed))
    {
      m_bIsElecCleansed = iBoolIsCleansed != 0;
    }
	}
	catch( ... )
	{
		throw ex::AbortLoadException(LogRec( "Fail to load CDV", "Fields_ext", "LoadVolumeFields3" ));
	}
	vxBlockVolumeIterator<uint2> itcd(*pcdv);

  vxVolumeHeader *ph = new vxVolumeHeader;
  *ph = pv->GetHeader();

	/// both intensity volume and CDV should have the identical Bias value. That was determined by the processor of V3D-Colon
 	int4 iBias(1000);
  int4 iMax(0);
  // get iMax
  std::string sBias;
  ph->GetHeaderEntry( vxVolumeHeader::Bias, sBias );
  iBias = atoi( sBias.c_str() );

  iMax = vxIntensityConverter::GetMaxValue( *ph );      
  if( iMax <= 0 )
    throw ex::AbortLoadException( LogRec("Incorrect maximum value", "Fields_ext", "LoadVolumeFields3") );  

	const float4 fMax (static_cast<float4>(iMax));

	/// 8 bits ECV volume. It has been resize in previous load stage. Make sure of this
  uint1 *pUC = m_volumeRender.GetDataPtr();    
  
	VarMap map = pcdv->GetHeader().GetVariableVolumeHeader();
	m_volumeRender.SetHdrMap(map);

  /// 8 bits DIF volume
	vxBlockVolume<uint1> *pd8 = new vxBlockVolume<uint1>(vxVolumeHeader(ph->GetVolDim(), ph->GetVoxelUnits(), 8, 0));
	if( pd8 == NULL )
	  throw ex::AbortLoadException( LogRec("No memory for DIF volume", "Fields_ext", "LoadVolumeFields3") );  

	pd8->GetHeader().SetVariableVolumeHeader(map);
	vxBlockVolumeIterator<uint1> it8d( *pd8 );

  // get EC indicator in the header of CDV
  map = pcdv->GetHeader().GetVariableVolumeHeader();
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
						it8d.SetPos(0,y,z);
						n = t;
            for( x=0; x<iX; n++, x++, itv.IncX(), itcd.IncX(), it8d.IncX() )
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
									it8d.SetVoxel(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									it8d.SetVoxel(255);

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
						it8d.SetPos(0,y,z);
						n = t;
						for( x=0; x<iX; n++, x++, itv.IncX(), itcd.IncX(), it8d.IncX() )
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
									it8d.SetVoxel(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									it8d.SetVoxel(255);

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
	} // else CDV is not available

	m_volumeRender.SetHdrMap(pcdv->GetHeader().GetVariableVolumeHeader());

  // pass the electronically cleansed volume to render
  m_bEcvVolLoaded = true;

  // Check the ecv-vol difference file (.dif extension)
	pd8->Compact();
	m_volumeRender.LinkToDiffVolume( pd8 );
  m_bDiffVolumeExists = true; 
  m_bDiffVolumeLoaded = true; 

	// the 2D units are always equal to that of 3D for View ver. 3.0
	// initialize 2d units as equal to 3D; change in block below if 2d vol exists
	m_units2D = m_volumeRender.GetUnits();
	m_sliceDimensions = m_volumeRender.GetDim();

  // load high quality volumes
  if ( rdrGlobal.GetSettings()->GetIntValue(VCSettings::ProcessorXML(), VCSettings::DisplayQuality2D()) > 8)
  {
    m_bHQDiffVolumeLoaded = m_sliceRendererData.Load3( pv, pcdv );
 
		delete pcdv;
		// the 2D units are always equal to that of 3D for View ver. 3.0
		// initialize 2d units as equal to 3D; change in block below if 2d vol exists
		m_units2D = ph->GetVoxelUnits();
		m_sliceDimensions = ph->GetVolDim();
  } // if UseHighQuality2D
	else
	{
		delete pv;
		delete pcdv;
		// the 2D units are always equal to that of 3D for View ver. 3.0
		// initialize 2d units as equal to 3D; change in block below if 2d vol exists
		m_units2D = m_volumeRender.GetUnits();
		m_sliceDimensions = m_volumeRender.GetDim();
	}
	delete ph;
} // LoadVolumeFields3


/**
 * load in the intensity block volume only
 *
 * @param pv the pointer to the input intensity block volume
 * @param sImageOrientation is the string contains the image orientation information
 */
void Fields::CreateVolumeOnly( vxBlockVolume<uint2> * pv, const std::string & sImageOrientation )
{
	vxBlockVolumeIterator<uint2> itv(*pv); 

  /// set the offset for converting VX world to DICOM world coordinate
  /// this is for quality assurance in general setting, where finding needs to be compared against to 
  /// a 3rd party application based on DICOM info
  try
  {   
		m_sPatientOrientation = sImageOrientation;

    /// get the image position patient
    std::string sImagePosition("");
    sImagePosition = pv->GetHeader().GetDicomFieldValue("0020", "0032", sImagePosition);
    if( sImagePosition.size() > 3 )
    {
      std::string sBreaker = "/\\,:";
      int4 i, n = sImagePosition.size();
      int4 j[3], k = 0;
      j[0] = -1; 
      j[1] = -1;

      for( i=0; i<n; i++ )
      {
        if( sImagePosition[i] == sBreaker[0] || sImagePosition[i] == sBreaker[1] || sImagePosition[i] == sBreaker[2] || sImagePosition[i] == sBreaker[3] )
        {
          j[k] = i;
          k++;
        }
      } // for i

      std::string sX;
      for( i=0; i<j[0]; i++ )
        sX.push_back(sImagePosition[i]);
      
      ToType(sX, m_vfShiftOffsetForDICOMWorldCoordinate.m_x);

      std::string sY;
      for( i=j[0]+1; i<j[1]; i++ )
        sY.push_back(sImagePosition[i]);
      
      ToType(sY, m_vfShiftOffsetForDICOMWorldCoordinate.m_y);

      std::string sZ;
      for( i=j[1]+1; i<n; i++ )
        sZ.push_back(sImagePosition[i]);
      
      ToType(sZ, m_vfShiftOffsetForDICOMWorldCoordinate.m_z);
    } // if
    else
    {
      m_vfShiftOffsetForDICOMWorldCoordinate = Vector3D<float4>(0.0F,0.0F,0.0F);
    } // else
  } // end set the offset
  catch( ... )
  {
    LogWrn("Error occoured in extraction of DICOM field -- image position patient", "Fields_ext", "CreateVolumeFields");
  }

  // set cleansed flag
  m_bIsElecCleansed = true;

  // pass the electronically cleansed volume to render
  m_bEcvVolLoaded     = true;
  m_bDiffVolumeExists = true; 
  m_bDiffVolumeLoaded = true; 

	// the 2D units are always equal to that of 3D for View ver. 3.0
	// initialize 2d units as equal to 3D; change in block below if 2d vol exists
	m_units2D         = pv->GetHeader().GetVoxelUnits(); 
	m_sliceDimensions = pv->GetHeader().GetVolDim();

  // load high quality volumes
	 m_bHQDiffVolumeLoaded = m_sliceRendererData.LoadVolOnly( pv );
 } // CreateVolumeOnly


/**
 * create CDV, 8bits intensity, and INS volumes
 *
 * @param pv the pointer to the input intensity block volume
 * @param plv the pointer to the linear CDV volume
 * @param pIns the pointer to the linear INS volume
 * @param vHeader the updated header with preprocessed paramter
 * @param sImageOrientation is the string contains the image orientation information
 */
void Fields::CreateCDV( vxBlockVolume<uint2> * pv, uint2 * plv, uint1 * pIns, 
	                      const vxVolumeHeader & vHeader, const std::string & sImageOrientation )
{
	vxBlockVolumeIterator<uint2> itv(*pv); 

  // set cleansed flag
  int4 iBoolIsCleansed;
  VarMap map( vHeader.GetVariableVolumeHeader() );
  if (map.GetHdrVar("IsEcleansed", iBoolIsCleansed))
  {
    m_bIsElecCleansed = iBoolIsCleansed != 0;
  }

	/// both intensity volume and CDV should have the identical Bias value. That was determined by the processor of V3D-Colon
 	int4 iBias(1000);
  int4 iMax(0);
  // get iMax
  std::string sBias;
  pv->GetHeader().GetHeaderEntry( vxVolumeHeader::Bias, sBias );
  iBias = atoi( sBias.c_str() );

  iMax = vxIntensityConverter::GetMaxValue( pv->GetHeader() );      
  if( iMax <= 0 )
    throw ex::AbortLoadException( LogRec("Incorrect maximum value", "Fields_ext", "CreateVolumeFields") );  

	const float4 fMax (static_cast<float4>(iMax));

	/// 8 bits ECV volume. It has been resize in previous load stage. Make sure of this
  uint1 *pUC = m_volumeRender.GetDataPtr();    
  
	m_volumeRender.SetHdrMap(map);

  /// 8 bits DIF volume
	vxBlockVolume<uint1> *pd8 = new vxBlockVolume<uint1>(vxVolumeHeader(vHeader.GetVolDim(), vHeader.GetVoxelUnits(), 8, 0));
	if( pd8 == NULL )
	  throw ex::AbortLoadException( LogRec("No memory for DIF volume", "Fields_ext", "CreateVolumeFields") );  

	pd8->GetHeader().SetVariableVolumeHeader(map);
	vxBlockVolumeIterator<uint1> it8d( *pd8 );

  // get EC indicator in the header of CDV
  int4 indicator(0);
  map.GetHdrVar("IsEcleansed", indicator);

	const int4 iX(vHeader.GetVolDim().m_x);
	const int4 iY(vHeader.GetVolDim().m_y);
	const int4 iZ(vHeader.GetVolDim().m_z);
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
						it8d.SetPos(0,y,z);
						n = t;
            for( x=0; x<iX; n++, x++, itv.IncX(), it8d.IncX() )
						{
							ff = static_cast<float4>(itv.GetVoxel())/fMax;
							iv = ff*iMax+iii;
							if( iv < 0 )
								iv = 0;

							ic = plv[n];
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
									it8d.SetVoxel(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									it8d.SetVoxel(255);

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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF for bias = 1000", "Fields_ext", "CreateVolumeFields" ));  
	    }
		} // if( iBias != 1000 )
		else
		{
      try
	    { // clamp image based on the bias
	      register     int2 iv; // ic;			
			  const float4 f( 255.0F/2000.0F );

				s = 0;
				for( z=0; z<iZ; z++ )
				{
					t = s;
				  for( y=0; y<iY; y++ )
					{
            itv.SetPos(0,y,z);
						it8d.SetPos(0,y,z);
						n = t;
						for( x=0; x<iX; n++, x++, itv.IncX(), it8d.IncX() )
						{
							iv = itv.GetVoxel();

							if( plv[n] == 0 )
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
									it8d.SetVoxel(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									it8d.SetVoxel(255);

								if( plv[n] < 2000 )
									pUC[n] = static_cast<uint1>( f*static_cast<float4>(plv[n]) );
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF for bias != 1000", "Fields_ext", "CreateVolumeFields" ));  
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF", "Fields_ext", "CreateVolumeFields" ));  
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF", "Fields_ext", "CreateVolumeFields" ));  
	    }
		} // else iBias == 1000
	} // else CDV is not available

	m_volumeRender.SetHdrMap( map );

  // Check the ecv-vol difference file (.dif extension)
	pd8->Compact();
	m_volumeRender.LinkToDiffVolume( pd8 );

  // load high quality volumes
  m_bHQDiffVolumeLoaded = m_sliceRendererData.LoadCDV( plv, vHeader );
 
	/// copy INS volume
	m_ins.SetHeader( vHeader );
	vxBlockVolumeIterator< uint1 > iti( m_ins );

	s = 0;
	for( z=0; z<iZ; z++ )
	{
		t = s;
		for( y=0; y<iY; y++ )
		{
			n = t;
			for( x=0; x<iX; x++ )
			{
				if( pIns[n] > 0x00 )
				{
					iti.SetPos(x,y,z);
					iti.SetVoxel( pIns[n] );
				} // if

				n++;
			} // for x

			t = t + iX;
		} // for y

		s = s + iS;
	} // for z
	    
	m_ins.Compact();
} // CreateCDV


/**
 *  set the scale
 *
 *  @param vHeader the updated block volume header
 */
void Fields::SetScaleFacter4SLD( const vxVolumeHeader & vHeader )
{
	const float4 fdivisionByZeroThreshold(0.00001F);
  
	// Calculate the sld scale factor from the sld header map
	//:: We assume that the SLD and ULD always have the identical sld scale! Dongqing 10 November 2009
	float4 fTempSldScale(0.25F);
	if( vHeader.GetVariableVolumeHeader().GetHdrVar("distanceScale", fTempSldScale) )
	{
		if( fTempSldScale < fdivisionByZeroThreshold )
    {
	    throw ex::DataCorruptionException(LogRec("Data corruption: Division by zero or by a very small number", "Fields_ext.C", "SetScaleFacter4SLD"));
		}
		m_fSldScale = 1.0F / fTempSldScale;
	} // if

} //SetScaleFacter4SLD


// undefines
#undef FILE_REVISION


// $Log: Fields_ext.C,v $
// Revision 1.4.2.7  2008/02/28 23:10:25  dongqing
// fix the computation bug in DICOM RCS display
//
// Revision 1.4.2.6  2008/02/28 19:19:09  dongqing
// the DICOM RCS convertion using Image Orientation Patient Cosine
// only now. The HFS stuff is no longer used for that purpose.
//
// Revision 1.4.2.5  2008/01/17 23:16:28  jmeade
// Issue 5944: added code for Issue 4042 that was not moved from previous LoadFields() versions.
//
// Revision 1.4.2.4  2008/01/11 21:59:57  dongqing
// finalize the DICOM world coordiante offset determination
//
// Revision 1.4.2.3  2008/01/10 22:37:14  dongqing
// enhance the dicom world coordinate display support all body position
//
// Revision 1.4.2.2  2008/01/09 21:58:49  dongqing
// add error handling for reading DICOM header information
//
// Revision 1.4.2.1  2008/01/02 22:09:39  dongqing
// convert world coordinate display from Viatronix to Standardized DICOM
//
// Revision 1.4  2006/10/04 20:54:38  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.3  2006/09/13 15:21:14  frank
// properly used a static method in the intensity converter
//
// Revision 1.2  2006/06/22 18:35:30  dongqing
// take away it.SetPosX(0) since it has already set
//
// Revision 1.1  2006/06/22 14:00:37  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.15  2006/06/16 dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Fields_ext.C,v 1.4.2.7 2008/02/28 23:10:25 dongqing Exp $
// $Id: Fields_ext.C,v 1.4.2.7 2008/02/28 23:10:25 dongqing Exp $
