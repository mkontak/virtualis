// $Id: Fields_64b.C,v 1.0 2011/07/15 dongqing Exp $
//
// Copyright © 2000-2011, Viatronix Inc., All Rights Reserved.
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
#define FILE_REVISION "$Revision: 1.0 $"
#ifdef max
#undef max
#endif

using namespace ReaderLib;


/**
 * Load all volumes from builts of 64bit preprocessing
 *
 * @param sVolumePreFix the path with volume filename prefix
 * @param sPreprocessedPrePix the path with volume filename prefix
 */
void Fields::LoadVolumeFields64b( const std::string & sVolumePrefix, const std::string & sPreprocessedPrefix )
{
  // prepare all the file names
  const std::string sCdvFile( sPreprocessedPrefix + vx::CleansedDifferenceVolExt );
  const std::string sVolFile( sVolumePrefix + vx::VolumeExt );

	// read original intensity volumes
  vxBlockVolume<uint2> *pv = new vxBlockVolume<uint2>;
	if( pv == NULL )
	  throw ex::AbortLoadException(LogRec( "No memory for intensity volume", "Fields_64b", "LoadVolumeFields64b"  ));

	try
	{
		vxBlockVolumeFileReader< uint2,  vxHistogramUpdaterIntensity  >::Read( sVolumePrefix + vx::VolumeExt, *pv );
	  pv->Compact(); // This is necessary for compact memory usage. If the vxBlockVolumeFileReader change, this may not be necessary.
	}
	catch( ... )
	{
		throw ex::AbortLoadException( LogRec("Fail to load intensity volume", "Fields_64b", "LoadVolumeFields64b" ) );
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
        LogWrn("Could not retrieve image orientation patient info correctly",  "Fields_64b", "LoadVolumeFields64b" );
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
          LogWrn("Body position in imaging is not axial",  "Fields_64b", "LoadVolumeFields64b" );
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
    LogWrn("Error occurred in extraction of DICOM field -- image position patient", "Fields_64b", "LoadVolumeFields64b" );
  }

	/// read CDV volume
  vxBlockVolume<uint2> *pcdv = new vxBlockVolume<uint2>;
	if( pcdv == NULL )
	  throw ex::AbortLoadException( LogRec( "No memory for CDV", "Fields_64b", "LoadVolumeFields64b" ) );

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
		throw ex::AbortLoadException(LogRec( "Fail to load CDV", "Fields_64b", "LoadVolumeFields64b"  ));
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
    throw ex::AbortLoadException( LogRec("Incorrect maximum value", "Fields_64b", "LoadVolumeFields64b" ) );  

	const float4 fMax (static_cast<float4>(iMax));

	/// 8 bits ECV volume. It has been resize in previous load stage. Make sure of this
  uint1 *pUC = m_volumeRender.GetDataPtr();    
  
	VarMap map = pcdv->GetHeader().GetVariableVolumeHeader();
	m_volumeRender.SetHdrMap(map);
  
  /// 8 bits DIF volume
	vxBlockVolume<uint1> *pd8(new vxBlockVolume<uint1>());
	pd8->Read( sPreprocessedPrefix + "_8bit.cdv" );
	if( pd8 == NULL )
	  throw ex::AbortLoadException( LogRec("No memory for DIF volume", "Fields_64b", "LoadVolumeFields64b" ) );  

	pd8->GetHeader().SetVariableVolumeHeader(map);

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
						n = t;
            for( x=0; x<iX; n++, x++, itv.IncX(), itcd.IncX() )
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF for bias = 1000", "Fields_64b", "LoadVolumeFields64b" ));  
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
						for( x=0; x<iX; n++, x++, itv.IncX(), itcd.IncX() )
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF for bias != 1000", "Fields_64b", "LoadVolumeFields64b"  ));  
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF", "Fields_64b", "LoadVolumeFields64b"  ));  
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
        throw ex::AbortLoadException(LogRec( "Fail to generate 8bits ECV and DIF", "Fields_64b", "LoadVolumeFields64b"  ));  
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

} // LoadVolumeFields64b


// undefines
#undef FILE_REVISION


// $Log: Fields_64b.C,v $
// Revision 1.0 2011/07/15 dongqing
//
// $Header: /vxColon/src/Fields_64b.C,v 1.0 2011/07/15 dongqing Exp $
// $Id: Fields_64b.C,v 1.0 2011/07/15 dongqing Exp $
