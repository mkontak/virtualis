// $Id: Fields.C,v 1.23.2.11 2010/12/21 15:40:48 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille( frank@viatronix.com )


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
#include "vxBlockVolumeFileReader.h"
#include "vxBlockVolumeUtilities.h"
#include "VolumeUtilities.h"
#include "vxHistogramUpdaterLabel.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "FilePtrFactory.h"

// defines
#define FILE_REVISION "$Revision: 1.23.2.11 $"
#ifdef max
#undef max
#endif


// namespaces
using namespace ReaderLib;


/**
 * Constructor
 */
Fields::Fields() : m_bIsElecCleansed( true )
{
  Reset();
} // Fields()


/**
 * Copy constructor
 * @param src   Copy source
 **/
Fields::Fields(const Fields & src)
{
  m_volumeRender              = src.m_volumeRender;
  m_paintVol                  = src.m_paintVol;
  m_dfs                       = src.m_dfs;
  m_dfe                       = src.m_dfe;
  m_csi                       = src.m_csi;
  m_sldInterior               = src.m_sldInterior;
	if( rdrGlobal.m_bUseULDvolume )
	{
    m_sldExterior               = src.m_sldExterior;  //:: Use this for un-cleansed SLD, i.e. the ULD volume
	}
  m_opacityVol                = src.m_opacityVol;
  m_ins                       = src.m_ins;
  m_diffVolume                = src.m_diffVolume;
  m_testVolumeUC              = src.m_testVolumeUC;
  m_bTestVolValid             = src.m_bTestVolValid;
  m_bOpacityVolCreated        = src.m_bOpacityVolCreated;
  m_bDiffVolumeExists         = src.m_bDiffVolumeExists;
  m_bDiffVolumeLoaded         = src.m_bDiffVolumeLoaded;
  m_bHQDiffVolumeLoaded       = src.m_bHQDiffVolumeLoaded;
  m_fSldScale                 = src.m_fSldScale;

  m_sliceRendererData         = src.m_sliceRendererData;
  m_sliceDimensions           = src.m_sliceDimensions;
  m_units2D                   = src.m_units2D;

  m_defaultWL                 = src.m_defaultWL;
  m_secondaryWL               = src.m_secondaryWL;
  m_bOpacityVolumePossible    = src.m_bOpacityVolumePossible;

  m_bIsElecCleansed           = src.m_bIsElecCleansed;
} // copy constructor


/**
 * Initializes all data members
 */
void Fields::Reset()
{
  const Triple<uint4> tripleDim(2,2,2);
  m_volumeRender.Reset();
  m_volumeRender.Resize(tripleDim);
  
  m_paintVol.Reset();
  m_paintVol.Resize(tripleDim);

  m_sldInterior.Resize(tripleDim);
	if( rdrGlobal.m_bUseULDvolume )
	{
    m_sldExterior.Resize(tripleDim);  //:: Use this for un-cleansed SLD, i.e. the ULD volume
	}
  m_dfs.GetHeader().SetVolDim( tripleDim );
  m_dfe.GetHeader().SetVolDim( tripleDim );
  m_csi.GetHeader().SetVolDim( tripleDim );
  m_ins.GetHeader().SetVolDim( tripleDim );
  m_diffVolume.GetHeader().SetVolDim( tripleDim );
  m_opacityVol.Resize(tripleDim);
  m_testVolumeUC.Resize(tripleDim);
  
  m_sliceRendererData.Reset();
  m_sliceDimensions = tripleDim;
  m_units2D = Triple<float4>(1.0f,1.0f,1.0f);
  m_defaultWL = std::make_pair<int4,int4>(2000,0);
  m_secondaryWL = std::make_pair<int4,int4>(2000,0);

  m_bTestVolValid            = false;
  m_bOpacityVolCreated       = false;
  m_bEcvVolLoaded            = false;
  m_bDiffVolumeExists        = false;
  m_bDiffVolumeLoaded        = false;
  m_bHQDiffVolumeLoaded      = false;

  m_fSldScale = 0.25F;
  m_bOpacityVolumePossible = true;

  m_bIsElecCleansed = true;
} // Reset()


/**
 * Assigns one Fields to the other
 * @param src   Source
 * @return this/destination
 */
Fields & Fields::operator = (const Fields & src)
{
  if (*this != src)
  {
    m_volumeRender              = src.m_volumeRender;
    m_paintVol                  = src.m_paintVol;
    m_dfs                       = src.m_dfs;
    m_dfe                       = src.m_dfe;
    m_csi                       = src.m_csi;
    m_sldInterior               = src.m_sldInterior;
    m_sldExterior               = src.m_sldExterior;
    m_opacityVol                = src.m_opacityVol;
    m_ins                       = src.m_ins;
    m_diffVolume                = src.m_diffVolume;
    m_testVolumeUC              = src.m_testVolumeUC;
    m_bTestVolValid             = src.m_bTestVolValid;
    m_bOpacityVolCreated        = src.m_bOpacityVolCreated;
    m_bDiffVolumeExists         = src.m_bDiffVolumeExists;
    m_bDiffVolumeLoaded         = src.m_bDiffVolumeLoaded;
    m_bHQDiffVolumeLoaded       = src.m_bHQDiffVolumeLoaded;
    m_fSldScale                 = src.m_fSldScale;
    m_sliceRendererData         = src.m_sliceRendererData;
    m_sliceDimensions           = src.m_sliceDimensions;
    m_units2D                   = src.m_units2D;

    m_defaultWL                 = src.m_defaultWL;
    m_secondaryWL               = src.m_secondaryWL;
    m_bOpacityVolumePossible    = src.m_bOpacityVolumePossible;

    m_bIsElecCleansed           = src. m_bIsElecCleansed;
  } // if (check for self assignment)
  return *this;
} // assignment operator


/**
 * Load all information from disk.
 *
 * @param sFilePrefix    the file prefix of the study to load (without the .ecv extension).
 * @param pDummy         the ending percent completed for feedback to the user on the gui.
 */
void Fields::LoadSpaceLeapFields(const std::string & sFilePrefix)
{
  const float4 fdivisionByZeroThreshold(0.00001F);
  
  // Read the sld file from disk
  // TODO: Store the ULD and the SLD separately to accelerate space leaping
  //       when showing the original (uncleansed) volume. Implement for 64-bit OS!
  if ( io::FilePtrFactory::Exists( sFilePrefix + vx::SpaceLeapDistExt ) )
    m_sldInterior.Read( sFilePrefix + vx::SpaceLeapDistExt );
  else if ( io::FilePtrFactory::Exists( sFilePrefix + vx::UncleansedSpaceLeapDistExt ) )
    m_sldInterior.Read( sFilePrefix + vx::UncleansedSpaceLeapDistExt );
  else
    throw ex::DataCorruptionException( LogRec( "Data corruption: SLD or ULD file is missing", "Fields", "LoadRemainder" ) );

  // Calculate the sld scale factor from the sld header map
  float4 fTempSldScale(0.25F);
  if(m_sldInterior.GetHdrVar("distanceScale", fTempSldScale))
  {
    if (fTempSldScale < fdivisionByZeroThreshold) throw ex::DataCorruptionException(LogRec("Data corruption: Division by zero or by a very small number","Fields","LoadRemainder"));
    m_fSldScale = 1.0F / fTempSldScale;
  }
  
} // LoadSpaceLeapFields


/**
 *  pass the SLD and SLV to linear volume
 *
 * @param pSld
 */
void Fields::LoadSpaceLeapFields2( vxBlockVolume<uint1> * pSld )
{
	register int4 iX( pSld->GetHeader().GetVolDim().m_x );
	register int4 iY( pSld->GetHeader().GetVolDim().m_y );
	register int4 iZ( pSld->GetHeader().GetVolDim().m_z );
	register int4 iS( iX*iY);
	register int4 x, y, z, s, t, n;

	vxBlockVolumeIterator<uint1> itSld( *pSld );
	uint1 *pLinSld = m_sldInterior.GetDataPtr();   /// This has been resized by Dataset_ext.C before
	m_sldInterior.SetHdrMap(pSld->GetHeader().GetVariableVolumeHeader());

	s = 0;
  for( z=0; z<iZ; z++ )
	{
		t = s;
    for( y=0; y<iY; y++ )
		{
			itSld.SetPos(0,y,z);
			n = t;
			for( x=0; x<iX; n++, x++, itSld.IncX() )
			{
				pLinSld[n] = itSld.GetVoxel();
			} // for x

			t = t+iX;
		} // for y

		s = s+iS;
	} // for z

  const float4 fdivisionByZeroThreshold(0.00001F);
  
  // Calculate the sld scale factor from the sld header map
  float4 fTempSldScale(0.25F);
  if( pSld->GetHeader().GetVariableVolumeHeader().GetHdrVar("distanceScale", fTempSldScale) )
  {
    if (fTempSldScale < fdivisionByZeroThreshold) throw ex::DataCorruptionException(LogRec("Data corruption: Division by zero or by a very small number","Fields","LoadRemainder"));
    m_fSldScale = 1.0F / fTempSldScale;
  }
  
  return;
} // LoadSpaceLeapFields2


/**
 * Read and convert the SLD and ULD to linear volume
 *
 * @param sPreprocessedPrefix the filename prefix
 */
void Fields::LoadSpaceLeapFields3( const std::string & sPreprocessedPrefix )
{
  bool bULDloaded = false;

  vxBlockVolume<uint1> * pSld = new vxBlockVolume<uint1>;

  if ( io::FilePtrFactory::Exists( sPreprocessedPrefix + vx::SpaceLeapDistExt ) )
  {
    vxBlockVolumeFileReader< uint1 >::Read( sPreprocessedPrefix + vx::SpaceLeapDistExt, * pSld );
  }
  else if ( io::FilePtrFactory::Exists( sPreprocessedPrefix + vx::UncleansedSpaceLeapDistExt ) )
  {
    vxBlockVolumeFileReader< uint1 >::Read( sPreprocessedPrefix + vx::UncleansedSpaceLeapDistExt, * pSld );
    bULDloaded = true;
  }
  else
  {
    throw ex::DataCorruptionException( LogRec( "Data corruption: Both SLD and ULD file are missing", "Fields", "LoadSpaceLeapFields3" ) );
  }
  pSld->Compact();

	vxBlockVolumeIterator<uint1> itSld( * pSld );
	uint1 * pLinSld = m_sldInterior.GetDataPtr();   /// This has been resized by Dataset_ext.C before
	m_sldInterior.SetHdrMap( pSld->GetHeader().GetVariableVolumeHeader() );

  // define the increments
	const int4 iX( pSld->GetHeader().GetVolDim().m_x );
	const int4 iY( pSld->GetHeader().GetVolDim().m_y );
	const int4 iZ( pSld->GetHeader().GetVolDim().m_z );
	const int4 iS( iX*iY);

	int4 s = 0;
  for( int4 z=0; z<iZ; z++ )
	{
		int4 t = s;
    for( int4 y=0; y<iY; y++ )
		{
			itSld.SetPos(0,y,z);
			int4 n = t;
			for( int4 x=0; x<iX; n++, x++, itSld.IncX() )
			{
				pLinSld[n] = itSld.GetVoxel();
			} // for x

			t = t+iX;
		} // for y

		s = s+iS;
	} // for z

  const float4 fdivisionByZeroThreshold(0.00001F);
  
  // Calculate the sld scale factor from the sld header map
  //:: We assume that the SLD and ULD always have the identical sld scale! Dongqing 10 November 2009
  float4 fTempSldScale(0.25F);
  if( pSld->GetHeader().GetVariableVolumeHeader().GetHdrVar("distanceScale", fTempSldScale) )
  {
    if (fTempSldScale < fdivisionByZeroThreshold) throw ex::DataCorruptionException(LogRec("Data corruption: Division by zero or by a very small number","Fields","LoadRemainder"));
    m_fSldScale = 1.0F / fTempSldScale;
  } //
  
  delete pSld;

	if( !bULDloaded && IsDiffVolLoaded() && rdrGlobal.m_bUseULDvolume )
  {
    pSld = new vxBlockVolume<uint1>;

    if ( io::FilePtrFactory::Exists( sPreprocessedPrefix + vx::UncleansedSpaceLeapDistExt ) )
    {
      vxBlockVolumeFileReader< uint1 >::Read( sPreprocessedPrefix + vx::UncleansedSpaceLeapDistExt, * pSld );
      pSld->Compact();

	    vxBlockVolumeIterator<uint1> itUld( * pSld );
	    uint1 * pLinUld = m_sldExterior.GetDataPtr();   /// This has been resized by Dataset_ext.C before
	    m_sldExterior.SetHdrMap( pSld->GetHeader().GetVariableVolumeHeader() );

	    s = 0;
      for( int4 z=0; z<iZ; z++ )
	    {
		    int4 t = s;
        for( int4 y=0; y<iY; y++ )
		    {
			    itUld.SetPos(0,y,z);
			    int4 n = t;
			    for( int4 x=0; x<iX; n++, x++, itUld.IncX() )
			    {
				    pLinUld[n] = itUld.GetVoxel();
			    } // for x

			    t = t+iX;
		    } // for y

		    s = s+iS;
	    } // for z

    } // if ULD file existed

    delete pSld;
  } // else bULDloaded == true

} // LoadSpaceLeapFields3()



/**
 * Load all information from disk.
 *
 * @param sFilePrefix    the file prefix of the study to load (without the .ecv extension).
 */
//void Fields::LoadVolumeFields(const std::string & sFilePrefix)
//{
//
//  throw "don't call me";
//
//  // prepare all the file names
//  const std::string sEcvFile( sFilePrefix + vx::ElecCleansedVolExt );
//  const std::string sDifFile( sFilePrefix + vx::DifferenceVolExt );
//  const std::string sCdvFile( sFilePrefix + vx::CleansedDifferenceVolExt );
//  const std::string sVolFile( sFilePrefix + vx::VolumeExt );
//  const std::string sHQVol( sFilePrefix + "_hq" + vx::ElecCleansedVolExt );
//  const std::string sHQDiffVol( sFilePrefix + "_hq" + vx::DifferenceVolExt );
//
//  // Read the electronically cleansed volume
//  bool bResult( m_volumeRender.ReadFile( sEcvFile ) ); 
//  m_bEcvVolLoaded = true;
//
//  // Check the ecv-vol difference file (.dif extension)
//  m_sDiffVolFilename = sDifFile;
//  if(vx::File(m_sDiffVolFilename).Exists())
//  {
//    m_bDiffVolumeExists = true; 
//  }
//
//  // initialize 2d units as equal to 3D; change in block below if 2d vol exists
//  m_units2D = m_volumeRender.GetUnits();
//
//  // load high quality volumes
//  if ( rdrGlobal.GetSettings()->GetIntValue(VCSettings::ProcessorXML(), VCSettings::DisplayQuality2D()) > 8 )
//  {
//
//    // try to load the .vol and .cdv if available
//    if ( vx::File(sVolFile).Exists() && vx::File(sCdvFile).Exists() )
//    {
//      LogFyi( "Loading the .vol and .cdv files", "Fields", "LoadVolumeFields" );
//      m_bHQDiffVolumeLoaded = m_sliceRendererData.Load2( sVolFile, sCdvFile, false );
//
//      // set cleansed flag
//      vxVolumeHeader hCDV;
//      int4 iBoolIsCleansed;
//      hCDV.Read( sCdvFile );
//      VarMap map( hCDV.GetVariableVolumeHeader() );
//      if (map.GetHdrVar("IsEcleansed", iBoolIsCleansed))
//      {
//        m_bIsElecCleansed = iBoolIsCleansed != 0;
//      }
//
//      // set dimensions and units
//      vxVolumeHeader hHQVol;
//      hHQVol.Read( sVolFile );
//      m_sliceDimensions = hHQVol.GetVolDim();
//      m_units2D = hHQVol.GetVoxelUnits();
//    }
//
//    // try to load the .ecv and .dif
//    else if ( vx::File( sHQVol ).Exists() && vx::File( sHQDiffVol ).Exists() )
//    {
//      LogFyi( "Loading the _hq.ecv and _hq.dif files", "Fields", "LoadVolumeFields" );
//      m_bHQDiffVolumeLoaded = m_sliceRendererData.Load( sHQVol, sHQDiffVol, false );
//
//      // set window/level for MR (only works if _hq files are there right now)
//      vxVolumeHeader hHQVol;
//      hHQVol.Read(sHQVol);
//      vxVolumeHeader hHQDiffVol;
//      hHQDiffVol.Read(sHQDiffVol);
//
//      m_sliceDimensions = hHQVol.GetVolDim();
//
//      m_units2D = hHQVol.GetVoxelUnits();
//
//      std::string sVarHeader;
//      VarMap map;
//
//      // read hq ecv header and set window/level if available (should only be available if MR data)
//      map.CreateMap(hHQVol.GetVarMapsAsString());
//      if ( map.GetHdrVar("varHeader", sVarHeader) )
//      {
//        VarMap varHdr(sVarHeader);
//        int4 iValue(0);
//        if (varHdr.GetHdrVar("Window", iValue))
//          m_defaultWL.first = iValue;
//        if (varHdr.GetHdrVar("Level", iValue))
//          m_defaultWL.second = iValue;
//      }
//
//      // read hq diff header and set window/level if available (should only be available if MR data)
//      map.CreateMap(hHQDiffVol.GetVarMapsAsString());
//      if ( map.GetHdrVar("varHeader", sVarHeader) )
//      {
//        VarMap varHdr(sVarHeader);
//        int4 iValue(0);
//        if (varHdr.GetHdrVar("Window", iValue))
//          m_secondaryWL.first = iValue;
//        if (varHdr.GetHdrVar("Level", iValue))
//          m_secondaryWL.second = iValue;
//      }
//    }
//
//  } // if UseHighQuality2D
//
//} // LoadVolumeFields



/**
 * Load all information from disk.
 *
 * @param sFilePrefix    the file prefix of the study to load (without the .ecv extension).
 */
void Fields::LoadVolumeFields2( const std::string & sVolumePrefix,
                                const std::string & sPreprocessedPrefix,
                                const bool & bRotateProneToSupine )
{

  // prepare all the file names
  const std::string sEcvFile( (io::FilePtrFactory::Exists(sVolumePrefix + vx::ElecCleansedVolExt) ? sVolumePrefix : sPreprocessedPrefix) + vx::ElecCleansedVolExt );
  const std::string sDifFile( sPreprocessedPrefix + vx::DifferenceVolExt );
  const std::string sCdvFile( sPreprocessedPrefix + vx::CleansedDifferenceVolExt );
  const std::string sVolFile( sVolumePrefix + vx::VolumeExt );
  const std::string sHQVol( sPreprocessedPrefix + "_hq" + vx::ElecCleansedVolExt );
  const std::string sHQDiffVol( sPreprocessedPrefix + "_hq" + vx::DifferenceVolExt );

  // Read the electronically cleansed volume 
  bool bResult( m_volumeRender.ReadFile( sEcvFile ) );
  if ( bRotateProneToSupine )
    VolumeUtilities::ReformatVolumeProneToSupine( * static_cast< LinVolUC * >( & m_volumeRender ) );
  m_bEcvVolLoaded = true;

  // Check the ecv-vol difference file (.dif extension)
  m_sDiffVolFilename = sDifFile;
  if(io::FilePtrFactory::Exists(m_sDiffVolFilename))
  {
    m_bDiffVolumeExists = true; 

    if( bRotateProneToSupine )
      LazyLoadDiffVolume();  
    /// if it is not loaded here, it will be loaded after user click 'Finish' button. 
    /// That causes blackout due to slow volume reformat for prone. Dongqing 20080930
  }

  // initialize 2d units as equal to 3D; change in block below if 2d vol exists
  m_units2D = m_volumeRender.GetUnits();

  // load high quality volumes
  if ( rdrGlobal.GetSettings()->GetIntValue(VCSettings::ProcessorXML(), VCSettings::DisplayQuality2D()) > 8)
  {

    // try to load the .vol and .cdv if available
    if ( io::FilePtrFactory::Exists(sVolFile) && io::FilePtrFactory::Exists(sCdvFile) )
    { 
      LogFyi( "Loading the .vol and .cdv files", "Fields", "LoadVolumeFields" );
      m_bHQDiffVolumeLoaded = m_sliceRendererData.Load2( sVolFile, sCdvFile, bRotateProneToSupine );

      // set cleansed flag
      vxVolumeHeader hCDV;
      int4 iBoolIsCleansed;
      hCDV.Read( sCdvFile );
      VarMap map( hCDV.GetVariableVolumeHeader() );
      if (map.GetHdrVar("IsEcleansed", iBoolIsCleansed))
      {
        m_bIsElecCleansed = iBoolIsCleansed != 0;
      }

      // set dimensions and units
      vxVolumeHeader hHQVol;
      hHQVol.Read( sVolFile );
      m_sliceDimensions = hHQVol.GetVolDim();
      m_units2D = hHQVol.GetVoxelUnits();
    }

    // try to load the .ecv and .dif
    else if ( io::FilePtrFactory::Exists( sHQVol ) && io::FilePtrFactory::Exists( sHQDiffVol ) )
    { 
      LogFyi( "Loading the _hq.ecv and _hq.dif files", "Fields", "LoadVolumeFields" );
      m_bHQDiffVolumeLoaded = m_sliceRendererData.Load( sHQVol, sHQDiffVol, bRotateProneToSupine );

      // set window/level for MR (only works if _hq files are there right now)
      vxVolumeHeader hHQVol;
      hHQVol.Read(sHQVol);
      vxVolumeHeader hHQDiffVol;
      hHQDiffVol.Read(sHQDiffVol);

      m_sliceDimensions = hHQVol.GetVolDim();

      m_units2D = hHQVol.GetVoxelUnits();

      std::string sVarHeader;
      VarMap map;

      // read hq ecv header and set window/level if available (should only be available if MR data)
      map.CreateMap(hHQVol.GetVarMapsAsString());
      if ( map.GetHdrVar("varHeader", sVarHeader) )
      {
        VarMap varHdr(sVarHeader);
        int4 iValue(0);
        if (varHdr.GetHdrVar("Window", iValue))
          m_defaultWL.first = iValue;
        if (varHdr.GetHdrVar("Level", iValue))
          m_defaultWL.second = iValue;
      }

      // read hq diff header and set window/level if available (should only be available if MR data)
      map.CreateMap(hHQDiffVol.GetVarMapsAsString());
      if ( map.GetHdrVar("varHeader", sVarHeader) )
      {
        VarMap varHdr(sVarHeader);
        int4 iValue(0);
        if (varHdr.GetHdrVar("Window", iValue))
          m_secondaryWL.first = iValue;
        if (varHdr.GetHdrVar("Level", iValue))
          m_secondaryWL.second = iValue;
      }
    }

  } // if UseHighQuality2D  

} // LoadVolumeFields2


/**
 * Load the Diff Volume only when it is needed
 *
 * @return               true upon success.
 */
void Fields::LazyLoadDiffVolume()
{ 
  try
  {
    // Check the ecv-vol difference file (.dif extension)
    if(io::FilePtrFactory::Exists(m_sDiffVolFilename))
    {
      m_diffVolume.Read(m_sDiffVolFilename);

      // set the correct dataset orientation
      std::string sOrientation;
      m_diffVolume.GetHeader().GetHeaderEntry(vxVolumeHeader::PatientPosition, sOrientation);
      if ( sOrientation[2] == 'P' )
      { // old version prone dif volume, need to reformat it. New version has not dif volume. That became cdv volume.
        VolumeUtilities::ReformatVolumeProneToSupineB( m_diffVolume );
      } // 

      m_volumeRender.LinkToDiffVolume(&m_diffVolume);
      m_bDiffVolumeLoaded = true; 
    }
    else
    {
      m_bDiffVolumeExists = false; 
    }
  }
  catch (ex::VException & ep)
  {
    throw ex::VException(LogRec(ep.GetLogRecord().GetTextMessage(),"Fields","LoadRemainder"));
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught in 'lazy loading diff'","Fields","LoadRemainder"));
  }

  return;
} // LazyLoadDiffVolume()


/**
 * Create the research/trial volume ONLY if a single dataset
 *
 * @return               true upon success.
 */
bool Fields::LazyCreateTestVolume()
{
  if(!rdrGlobal.IsDualDataLoaded())
  {
    m_testVolumeUC.Resize(m_volumeRender.GetDim());
    m_testVolumeUC.SetUnits(m_volumeRender.GetUnits());
    m_bTestVolValid = true;
    return true;
  }
  return false;
} // LazyCreateTestVolume()


/**
 * Gets the label volume (create it if necessary)
 */
vxBlockVolume<uint2> & Fields::GetLabelVolume()
{
  // lazily create label volume
  if ( m_labelVolume.GetHeader().GetVolDim() != m_ins.GetHeader().GetVolDim() )
  {
		// set the header
    m_labelVolume.SetHeader( m_ins.GetHeader() );
		vxBlockVolumeIterator<uint2> itl( m_labelVolume );

    vxBlockVolumeIterator<uint1> iti( m_ins );

		/// copy the lumen regions
    for ( ; iti.IsNotAtEnd(); iti.NextNonBackgroundBlockZYX() )
    { // quickly traverse the non-empty block
      itl.SetPos( iti );
      for ( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock(), itl.NextZYXinsideBlock() )
      { // just copy the non-zero voxels
        if ( iti.GetVoxel() > 0x00 )
          itl.SetVoxel( static_cast<uint2>( iti.GetVoxel() ) );
      } // for
    } // for

    //vxHistogramUpdaterLabel<uint2>::Update( m_labelVolume );

    vxBlockVolumeIteratorNeighbors<uint2> it26( m_labelVolume, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26 );
    vxBlockVolumeIteratorNeighbors<uint2> it6( m_labelVolume, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );
    const    uint2 u01 = 20;
		register uint2 uu;

		/// dialte 1st layer of the lumen
		for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
		{
			for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
			{
				if( itl.GetVoxel() > 0 && itl.GetVoxel() < u01 )
				{
				  for( it6.CenterAt(itl.GetPos()); !it6.IsAtEnd(); ++it6 )
				  {
            if( it6.GetVoxel() == 0x00 )
              it6.SetVoxel( u01+itl.GetVoxel() );
					} // for neighbor
				} // if
			} // for inside block
		} // for all blocks

    const uint2 u02 = 40;
		/// dialte 2nd layer of lumen
		for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
		{
			for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
			{
				if( itl.GetVoxel() > u01 && itl.GetVoxel() < u02 )
				{
					uu = itl.GetVoxel() - u01;
				  for( it26.CenterAt(itl.GetPos()); !it26.IsAtEnd(); ++it26 )
				  {
            if( it26.GetVoxel() == 0x00 )
              it26.SetVoxel( u02+uu );
					} // for neighbor
				} // if
			} // for inside block
		} // for all blocks

		/// the 3rd layer seems not necessary
    const uint2 u03 = 60;
		/// dialte 3rd layer of lumen
		for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
		{
			for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
			{
				if( itl.GetVoxel() > u02 && itl.GetVoxel() < u03 )
				{
					uu = itl.GetVoxel() - u02;
				  for( it6.CenterAt(itl.GetPos()); !it6.IsAtEnd(); ++it6 )
				  {
            if( it6.GetVoxel() == 0x00 )
              it6.SetVoxel( u03+uu );
					} // for neighbor
				} // if
			} // for inside block
		} // for all blocks

		/// clean up
		for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
		{
			for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
			{
				if( itl.GetVoxel() > u01 && itl.GetVoxel() < u02 )
				  itl.SetVoxel( itl.GetVoxel() - u01 );
				else if( itl.GetVoxel() > u02 && itl.GetVoxel() < u03 )
				  itl.SetVoxel( itl.GetVoxel() - u02 );
				else if( itl.GetVoxel() > u03 )
				  itl.SetVoxel( itl.GetVoxel() - u03 );
			} // for
		} // for

		///// the 4th layer seems not necessary
  //  const uint2 u04 = 80;
		///// dialte 4th layer of lumen
		//for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
		//{
		//	for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
		//	{
		//		if( itl.GetVoxel() > u03 && itl.GetVoxel() < u04 )
		//		{
		//			uu = itl.GetVoxel() - u03;
		//		  for( it26.CenterAt(itl.GetPos()); !it26.IsAtEnd(); ++it26 )
		//		  {
  //          if( it26.GetVoxel() == 0x00 )
  //            it26.SetVoxel( u04+uu );
		//			} // for neighbor
		//		} // if
		//	} // for inside block
		//} // for all blocks

		///// clean up
		//for( itl.SetPos(0,0,0); itl.IsNotAtEnd(); itl.NextBlockZYX() )
		//{
		//	for( ; itl.IsNotAtEndOfBlock(); itl.NextZYXinsideBlock() )
		//	{
		//		if( itl.GetVoxel() > u01 && itl.GetVoxel() < u02 )
		//		  itl.SetVoxel( itl.GetVoxel() - u01 );
		//		else if( itl.GetVoxel() > u02 && itl.GetVoxel() < u03 )
		//		  itl.SetVoxel( itl.GetVoxel() - u02 );
		//		else if( itl.GetVoxel() > u03 && itl.GetVoxel() < u04 )
		//		  itl.SetVoxel( itl.GetVoxel() - u03 );
		//		else if( itl.GetVoxel() > u04 )
		//		  itl.SetVoxel( itl.GetVoxel() - u04 );
		//	} // for
		//} // for

    vxHistogramUpdaterLabel<uint2>::Update( m_labelVolume );
		//m_labelVolume.Compact();
  } // if the label volume is not available

  return m_labelVolume;
} // GetLabelVolume()



/**
 * Return the index of the skeleton node closest to the current view position.
 *
 * @param rDataset   The position in the volume to look up
 * @return           The index of the closest skeleton node.
 */
uint2 Fields::GetClosestSkeletonIndex( const Point<float4> & volumePosition ) const
{
  Point<uint2> positionVolumeUSpoint = volumePosition;
  try
  {
    vxBlockVolumeIterator<uint2> csiIter( m_csi );
    return csiIter.Peek( positionVolumeUSpoint.m_x, positionVolumeUSpoint.m_y, positionVolumeUSpoint.m_z );
  }
  catch ( ... )
  {
    LogWrn( std::string( "Error accessing the closest skeleton index at volume position: " ) + positionVolumeUSpoint.ToString(), "Fields", "GetClosestSkeletonIndex" );
  }
  return 0;
} // GetClosestSkeletonIndex()


/**
 * Get the gradient direction of the appropriate potential field.
 *
 * @param positionVolumePoint  the position inside the volume at which to resample.
 * @param bFromStart           whether to use the potential distance from start or from end field.
 * @return Vector<float4>
 */
//Vector<float4> Fields::GetResampledPotentialGradient(const Point<float4> & positionVolumePoint, const bool bFromStart)
//{
//
//  // get a pointer to the proper field depending on the desired direction.
//  vxBlockVolume<float4> * pField = bFromStart ? (& m_dfs) : (& m_dfe);
//
//  // the central difference requires division by two because
//  // the two sample points are two units apart.
//  float4 fGradType;
//  return ( pField->CentralDiffGradient(positionVolumePoint, fGradType) * (-0.5F) );
//
//} // GetResampledPotentialGradient()


/**
 * Search either of the potential fields and return a gradient vector which points in the direction of smaller values.
 *
 * @param positionVolumePoint  the position inside the volume at which to resample.
 * @param bFromStart           whether to use the potential distance from start or from end field.
 */
Vector<float4> Fields::GetMinimumPotentialDirection(const Point<float4> & positionVolumePoint, const bool bFromStart)
{
  // get a pointer to the proper field depending on the desired direction.
  vxBlockVolume<float4> & field = bFromStart ? (m_dfs) : (m_dfe);
  
  // search the neighbors for the minimum value and go toward that one
  Point<uint2> myPointLocation;
  myPointLocation = positionVolumePoint;
  bool bFoundLowerValue(false);
  float4 fMinimumValue( std::numeric_limits<float4>::max() );
  Point<float4> minimumLocationPoint(positionVolumePoint);
  
  // search a block of voxels around the current point
  const int4 radius(2);
  vxBlockVolumeIterator<uint1> insIter( m_ins );
  vxBlockVolumeIterator<float4> fieldIter( field );
  for (int4 z(-radius+myPointLocation.m_z); z<=radius+myPointLocation.m_z; z++)
  {
    for (int4 y(-radius+myPointLocation.m_y); y<=radius+myPointLocation.m_y; y++)
    {
      for (int4 x(-radius+myPointLocation.m_x); x<=radius+myPointLocation.m_x; x++)
      {
        const Point<uint2> pointPosition(x,y,z);
        if ( insIter.Peek( x, y, z ) > 0 )
        {
          const float4 value( fieldIter.Peek( pointPosition.m_x, pointPosition.m_y, pointPosition.m_z ) );
          if ( value < fMinimumValue )
          {
            fMinimumValue = value;
            minimumLocationPoint = pointPosition;
            bFoundLowerValue = true;
          } // (value < fMinimumValue) -- found lower value
        } // if (m_ins.GetVoxel(pointPosition) > 0) -- inside the volume
      } // for x loop
    } // for y loop
  } // for z loop over block of voxels
  
  // if we have found any voxels with lower value
  if (bFoundLowerValue)
  {
    return Vector<float4>(minimumLocationPoint - positionVolumePoint);
  }
  else
  {
    // no lower value voxels found
    return Vector<float4>(0.0F, 0.0F, 0.0F);
  }
    
} // GetMinimumPotentialDirection()


/**
 * Searches for the closest valid csi in ever increasing shells.
 *
 * @param startPositionPoint  the starting point in the search.
 * @param iThisSize           the radius of the currently searched shell.
 * @param iMax                the maximum search radius.
 * @param vpFoundPoint        the closest valid csi location, if found.
 * @return                    true upon finding a valid csi.
 */
const bool Fields::FindClosestValidCSI(const Point<uint2> & startPositionPoint, const int2 iThisSize, const int2 iMax, Point<uint2> & foundPoint) const
{
  if (iThisSize == iMax)
  {
    return false;
  } // if (iThisSize == iMax) - check if iThisSize is at the iMax limit
  else if (iThisSize > iMax)
  {
   throw ex::DataCorruptionException( LogRec(  "Exceeded maximum search radius", "Fields","FindClosesValidCSI" ));
  } // if (iThisSize > iMax) - check if iThisSize doesn't overpassed the iMax limit
  
  // figure out the bounds of our neighborhood
  const int2 iMinX(Bound(int2(0), int2(int2(startPositionPoint.m_x)-iThisSize), int2(m_csi.GetHeader().GetVolDim().m_x)));
  const int2 iMaxX(Bound(int2(0), int2(int2(startPositionPoint.m_x)+iThisSize), int2(m_csi.GetHeader().GetVolDim().m_x)));
  const int2 iMinY(Bound(int2(0), int2(int2(startPositionPoint.m_y)-iThisSize), int2(m_csi.GetHeader().GetVolDim().m_y)));
  const int2 iMaxY(Bound(int2(0), int2(int2(startPositionPoint.m_y)+iThisSize), int2(m_csi.GetHeader().GetVolDim().m_y)));
  const int2 iMinZ(Bound(int2(0), int2(int2(startPositionPoint.m_z)-iThisSize), int2(m_csi.GetHeader().GetVolDim().m_z)));
  const int2 iMaxZ(Bound(int2(0), int2(int2(startPositionPoint.m_z)+iThisSize), int2(m_csi.GetHeader().GetVolDim().m_z)));
  
  // loop over all voxels in our little neighborhood
  vxBlockVolumeIterator<uint2> csiIter( m_csi );
  for (int2 z(iMinZ); z<=iMaxZ; z++)
  {
    for (int2 y(iMinY); y<=iMaxY; y++)
    {
      for (int2 x(iMinX); x<=iMaxX; x++)
      {
        if ( csiIter.Peek( x, y, z ) != 0 )
        {
          foundPoint = Point<uint2>( x, y, z );
          return true;
        } // if the voxel was found
      } // for x
    } // for y
  } // for z

  return FindClosestValidCSI(startPositionPoint,iThisSize+1,iMax,foundPoint);
} // FindClosestValidCSI()


// undefines
#undef FILE_REVISION


// $Log: Fields.C,v $
// Revision 1.23.2.11  2010/12/21 15:40:48  dongqing
// comments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.23.2.10  2010/12/20 18:53:26  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.23.2.9  2009/11/10 19:03:48  dongqing
// Improving fly speed for non-cleansed images by using ULD volume
//
// Revision 1.23.2.8  2009/01/15 04:14:09  jmeade
// const modifier on method
//
// Revision 1.23.2.7  2009/01/14 17:42:36  jmeade
// comments.
//
// Revision 1.23.2.6  2008/09/30 15:36:52  dongqing
// Add dif volume load in the LoadVolumeFields2(). This applied the prone
// reformation to 8bit dif volume for backward compatible.
//
// Revision 1.23.2.5  2008/02/01 15:56:08  dongqing
// decomments
//
// Revision 1.23.2.4  2008/01/29 14:20:16  geconomos
// Aded call to histogram updater in GetLabelVolume()
//
// Revision 1.23.2.3  2008/01/21 22:17:18  dongqing
// Fix iterator error in the dilation of GetLabelVolume
//
// Revision 1.23.2.2  2008/01/21 13:46:22  dongqing
// change the algorithm for creating label volume
//
// Revision 1.23.2.1  2007/12/20 21:33:33  jmeade
// Issue 5944: replaced code for Issue 4042 that was erroneously removed.
//
// Revision 1.23  2006/10/04 20:54:14  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.22  2006/10/02 20:01:08  frank
// Issue #5020: Properly read in the uld file when there is no sld
//
// Revision 1.21  2006/09/13 15:19:44  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.20  2006/08/23 21:10:56  romy
// calling dialate volume for a better transparent view
//
// Revision 1.19  2006/06/26 12:58:47  frank
// creates a label volume out of the dilated ins volume
//
// Revision 1.18  2006/06/22 18:35:30  dongqing
// take away it.SetPosX(0) since it has already set
//
// Revision 1.17  2006/06/22 15:20:24  dongqing
// Get rid of reading SLV file
//
// Revision 1.16  2006/06/22 14:01:24  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.15  2006/04/25 17:16:36  jmeade
// new study directory structure.
//
// Revision 1.14  2006/04/24 15:17:57  jmeade
// no hard-coded extensions/text.
//
// Revision 1.13  2005/11/25 17:29:36  jmeade
// correct load directory for volume file.
//
// Revision 1.12  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.11  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.10  2005/11/04 20:04:34  dongqing
// change usage of VCSetting for HQ2Don as GetIntValue
//
// Revision 1.9.2.3  2005/11/10 00:37:37  jmeade
// Issue 4042: EC header var changed to an int, labeled: "IsEcleansed"
//
// Revision 1.9.2.2  2005/11/09 01:37:13  jmeade
// Issue 4042: Check cdv file for flag indicating if study processed with EC.
//
// Revision 1.9.2.1  2005/11/04 19:25:43  dongqing
// change usage of VCSetting for HQ2Don as GetIntValue
//
// Revision 1.9  2005/11/04 01:11:43  jmeade
// Issue 4513: Recoded vcsettings xml functionality.
//
// Revision 1.8  2005/11/02 14:56:22  frank
// Issue #4503: Further fixes
//
// Revision 1.7  2005/11/01 14:49:18  frank
// Issue #4503: Read new .cdv and .vol files instead of _hq files
//
// Revision 1.6  2005/10/13 18:11:32  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.5  2005/09/29 15:01:32  vxconfig
// switched dfs and dfe volumes to new vx block volume
//
// Revision 1.4  2005/09/29 14:11:46  vxconfig
// switched csi volume to new vx block volume
//
// Revision 1.3  2005/09/29 13:33:13  vxconfig
// made the difference volume a new vx block volume
//
// Revision 1.2  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.19.2.7.2.7  2005/06/14 22:13:58  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.19.2.7.2.6.2.1  2005/06/14 17:58:54  jmeade
// spaces.
//
// Revision 3.19.2.7.2.6  2005/04/22 16:48:16  frank
// cleaned up field class interface
//
// Revision 3.19.2.7.2.5  2005/04/08 22:37:58  jmeade
// Issue 4042: Store whether hq diff volume loads.
//
// Revision 3.19.2.7.2.4  2005/04/08 14:51:36  frank
// made it just a warning
//
// Revision 3.19.2.7.2.3  2005/04/08 14:49:41  frank
// Issue #4112: Masked problem accessing the CSI volume out of bounds
//
// Revision 3.19.2.7.2.2  2005/04/01 17:33:33  frank
// Issue #4094: Overview will now try to create the transparent volume, if that fails, it will simply use triangles from that point forward.
//
// Revision 3.19.2.7.2.1  2005/03/14 22:57:01  jmeade
// Loading and displaying slice data as early as possible during study load.
//
// Revision 3.19.2.7  2005/02/15 18:04:25  jmeade
// merge from vc1.4 branch.
//
// Revision 3.19.2.6.2.5  2005/02/09 00:45:45  jmeade
// Set volume units value for non-hq studies.
//
// Revision 3.19.2.6.2.4  2005/02/04 23:52:48  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.19.2.6.2.3  2004/11/17 21:55:46  jmeade
// Separate W/L for pre and post-contrast studies.
//
// Revision 3.19.2.6.2.2  2004/11/16 16:38:28  jmeade
// compile error.
//
// Revision 3.19.2.6.2.1  2004/11/16 16:30:20  jmeade
// _16bit ==> _hq; Default window/level read from hq volume header (if there).
//
// Revision 3.19.2.6  2004/09/29 19:52:09  jmeade
// Preference to disable 2d auto flight.
//
// Revision 3.19.2.5  2004/08/12 00:06:26  jmeade
// VCSettings determines whether to use high-quality data.
//
// Revision 3.19.2.4  2004/05/19 13:07:06  geconomos
// Added missing vxSliceRendererData assignment in copy constructor.
//
// Revision 3.19.2.3  2004/04/23 15:33:14  geconomos
// Initial addition of merging in newer 12-bit slice renderers.
//
// Revision 3.19.2.2  2003/05/22 19:59:43  kevin
// Tracker issue 2784: This is now fixed correctly. The  system will now use
// more memory and the external DFB volume is sotred in it's own volume
// so the internal and enternal will not "leak" together.
//
// This also indirectly fixes the black holes problem
//
// Revision 3.19.2.1  2003/03/26 23:08:37  jmeade
// Issue 2849:  Removed unnecessary non-const pointer parameter.
//
// Revision 3.19  2003/01/22 22:26:02  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.18  2002/11/26 00:41:05  kevin
// Put Translucent Overview SPace Leap fields back in
//
// Revision 3.17  2002/11/24 17:52:26  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.16  2002/10/29 19:36:30  kevin
// Why was this never there
//
// Revision 3.15  2002/09/13 00:45:09  jmeade
// Call paint volume instance Reset() function in Fields::Reset() function.
//
// Revision 3.14  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.13  2002/04/12 02:12:21  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.12  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.11.2.1  2002/02/25 16:04:32  kevin
// Dramatically changed the patient loading mechanism. This is required to fix
// memory erros both in physicial memory and in adress space fragmentation.
// Moved the control of the loading order to Study class. Fields now only
// handles mutiple volumes in one load. ReaderGlobal now only calls Study
// in a loop which allows Supine and Prone loads to be interleaved (the
// part in Readglobal was simplified alot and the logic placed in study so that
// all the logic is in one place in case we want to play with the order of
// loading in the future)
//
// Also added pre-allocation of all the linear volumes before address is fragmented.
//
// Also added timing info into the LogFyi log file so that we can track the time it
// takes to laod even in the field.
//
// Revision 3.11  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.10  2002/01/04 16:33:19  kevin
// Some cleanup after the hacking to get TTP rendering without ses,set,sec
//
// Revision 3.9  2002/01/03 02:29:08  kevin
// Removed smallINS volue and clean up
// Also fixed TTP rendering noew that set, ses and sec are gone
//
// Revision 3.8  2001/11/09 04:42:55  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.7  2001/11/06 15:26:32  kevin
// Removed ses, set, and sec volumes
//
// Revision 3.6  2001/11/02 16:13:06  binli
// 2D render from block vol (.diff)
//
// Revision 3.5  2001/01/13 09:48:00  kevin
// Fixed the bug in uncleased rendering where both supine and prone datasets
// pointed the curent study for the block diff volume
//
// Revision 3.4  2001/11/01 19:53:41  kevin
// Added 3D Uncleansed DIff-Block_volume rendering
//
// Revision 3.3  2001/11/01 17:10:50  binli
// 2D render from one vol. (.ecv]
//
// Revision 3.2  2001/10/31 00:44:53  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0.2.1  2001/10/31 01:21:29  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding the volumes
//
// Revision 3.2  2001/10/31 00:44:53  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 10 2001 11:55:40   binli
// issue 1027: transparent rendering of overview reflect selection
// 
//    Rev 2.1   Oct 03 2001 20:06:20   jmeade
// User load abort should throw AbortLoadException, not AbortOperationEx; spelling errors in exception comment
// 
//    Rev 2.0   Sep 16 2001 23:40:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:58   ingmar
// Initial revision.
// Revision 1.79  2001/08/31 18:57:23  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.78  2001/08/30 23:30:10  pagron
// named Resize method on volumes more appropriately as Resample and added a Resize func that does a dummy resize
//
// Revision 1.77  2001/08/30 15:33:15  soeren
// optimized creation of paint volume
//
// Revision 1.76  2001/08/24 20:03:20  frank
// Synchronized exceptions.
//
// Revision 1.75  2001/08/20 20:08:07  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.74  2001/07/26 20:29:49  binli
// ID 89: exception handling.
//
// Revision 1.73  2001/07/25 22:52:16  binli
// ID 89: new exception handling.
//
// Revision 1.72  2001/07/19 15:07:35  binli
// caught 'Abort' exception
//
// Revision 1.71  2001/07/16 23:46:31  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.70  2001/07/13 19:52:37  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.69  2001/07/12 21:12:39  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.68  2001/06/27 21:49:44  dave
// removed warning about bits per voxel on startup
//
// Revision 1.67  2001/06/26 16:21:38  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.66  2001/05/31 21:56:48  binli
// bug 000484 (step1: distance): use current study, not rdrGlobal.m_pStudy.
//
// Revision 1.65  2001/05/19 00:41:04  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.64  2001/05/07 00:51:26  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.63  2001/05/02 21:39:43  jmeade
// corrections after mergeLostApril26 merge
//
// Revision 1.62  2001/05/02 20:12:09  ingmar
// BookmarkList.C
//
// Revision 1.61  2001/04/30 11:37:54  frank
// Added exception handling.
//
// Revision 1.60.2.1  2001/05/01 23:38:47  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.60  2001/04/18 19:11:37  liwei
// m_pOpacityVol changed to m_opacityVol
//
// Revision 1.59  2001/04/13 13:29:02  frank
// Started using the streams interface.
//
// Revision 1.58  2001/04/13 13:01:39  frank
// Moved slice loading to Fields.
//
// Revision 1.57  2001/04/13 11:53:39  frank
// Protected some members of Fields.
//
// Revision 1.56  2001/04/13 00:36:30  jmeade
// file extensions
//
// Revision 1.55  2001/04/12 19:20:50  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.54  2001/04/12 15:58:20  dave
// added namespace vx for exceptions
//
// Revision 1.53  2001/04/11 18:17:09  frank
// Renamed a csi search function.
//
// Revision 1.52  2001/04/11 17:09:51  frank
// Cleaned up pdr searching/gradient functions.
//
// Revision 1.51  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.50  2001/04/11 11:23:10  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.49  2001/04/10 17:51:42  frank
// Code walkthrough changes.
//
// Revision 1.48  2001/04/07 04:45:29  jmeade
// Bug: Interface reporting error on user abort.  Fix:  Better throw and catch
// of abort exception in ReaderGlobal, Study, Fields
//
// Revision 1.47  2001/03/19 15:30:39  frank
// Updated to reflect new coding standards.
//
// Revision 1.46  2001/03/19 12:00:38  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.45  2001/03/16 17:11:09  frank
// Updated Fields class variables for new coding standards.
//
// Revision 1.44  2001/03/07 16:02:44  liwei
// Added some comments
//
// Revision 1.43  2001/02/14 19:54:46  liwei
// TTP transfer function tuned
//
// Revision 1.42  2001/02/12 23:28:28  kevin
// Removed slv
//
// Revision 1.41  2001/02/10 00:46:01  liwei
// The dissappearing problem of TTP after switching among views fixed
//
// Revision 1.40  2001/02/09 14:33:31  kevin
// Try removing the second sld volume to see if it fixes the"out of memory" problem
//
// Revision 1.39  2001/02/06 14:13:48  frank
// Revamped study load progress display.
//
// Revision 1.38  2001/02/06 04:52:53  dave
// changed order of volume reading
//
// Revision 1.37  2001/02/05 13:29:37  frank
// Fixed crashing due to uninitialized member.
//
// Revision 1.36  2001/02/02 16:29:36  liwei
// A try-catch pair added to wrap the rendering of TTP, hopefully solve the disapperaing problem
//
// Revision 1.35  2001/01/30 23:44:59  liwei
// pVolOpa moved to Study from RenderContext, so that when toggle between
// supine and prone, it is initialized properly
//
// Revision 1.34  2001/01/29 22:15:50  liwei
// Attempted to move the opacity volume to study or fields, not finished yet
//
// Revision 1.33  2001/01/29 20:43:10  liwei
// Minor changes
//
// Revision 1.32  2001/01/27 02:02:01  kevin
// 1) Fixed 90% missed Patch error
// 2) Moved GetPotenetil to fields from Submarine
// as part of this fix.
//
// Revision 1.31  2001/01/24 19:18:19  liwei
// Software volume rendering for overview
//
// Revision 1.30  2001/01/23 19:35:56  frank
// Removed excess warning boxes.
//
// Revision 1.29  2001/01/11 13:08:44  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.28  2001/01/03 18:35:28  frank
// Improved error handling during read.
//
// Revision 1.27  2000/12/19 02:31:28  kevin
// Removed unused var giving warning
//
// Revision 1.26  2000/12/12 16:09:23  dave
// removed SetBackground()
//
// Revision 1.25  2000/12/11 21:39:35  dave
// slight changes in Study.C,Fields.C,VolumeRender.C in try catch
//
// Revision 1.24  2000/11/25 03:04:57  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.23  2000/11/22 12:38:16  frank
// Added study load aborting
//
// Revision 1.22  2000/11/20 19:04:44  kevin
// Made INS block Again (finally not being used for painting)
//
// Revision 1.21  2000/11/17 18:25:52  frank
// Added Reset functions
//
// Revision 1.20  2000/11/16 15:26:28  jeff
// LoadCallback class to keep track of study load state information
//
// Revision 1.19  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.18  2000/11/09 15:43:08  frank
// Cleaned up load messages
//
// Revision 1.17  2000/11/09 02:55:03  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.16  2000/11/08 17:38:55  frank
// Added m_study to ReaderGlobal
//
// Revision 1.15  2000/11/03 20:32:12  dave
// converted back to unsigned dim.
//
  // Revision 1.14  2000/11/02 20:09:12  dave
  // Dimensions changed to Triple<short> from uint
  //
  // Revision 1.13  2000/11/02 17:35:47  kevin
  // Changed to "Search"CSI with a sprial iterator
  //
  // Revision 1.12  2000/10/31 14:37:40  frank
  // Merged in new segment structure
  //
  // Revision 1.11  2000/10/29 17:41:54  kevin
  // Added GetClosestSkeletonINdex to a given point
  // instead of the current viewpoint
  //
  // Revision 1.10.2.1  2000/10/26 13:28:32  frank
  // Added new segment structure
  //
  // Revision 1.10  2000/10/20 17:13:01  kevin
  // Added debug for load timing
  //
  // Revision 1.9  2000/10/11 15:40:22  kevin
  // Added Notify for all Fields
  //
  // Revision 1.8  2000/10/04 14:25:20  kevin
  // Put the ins read back in (now that we
  // can read headers)
  //
  // Revision 1.7  2000/10/02 21:56:59  jeff
  // Changes for staggered study load (had to check in because my cheapo computer is crashing)
  //
  // Revision 1.6  2000/09/28 20:46:10  frank
  // Replaced DFB with SLD
  //
  // Revision 1.5  2000/09/12 20:42:39  kevin
  // Switched back to Block volumes (including .ins)
  //
  // Revision 1.4  2000/09/12 14:54:51  frank
  // Changes for coding standard compliance
  //
  // Revision 1.3  2000/09/12 00:02:20  kevin
  // CHanged back to Hash due to "grey screen" bug
  // even though block uses significantly less memory
  //
  // Revision 1.2  2000/09/11 18:06:55  frank
  // Changed from Hash volumes to Block volumes
  //
  // Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
  // unit tests complete
  //
  // 
  // 36    9/05/00 1:33a Ingmar
  // now works with new naming
  // 
  // 35    9/01/00 4:55p Frank
  // Worked on unwrapping the colon wall
  // 
  // 34    8/29/00 1:25p Frank
  // Added corrupt dataset detection
  // 
  // 33    8/28/00 11:25a Antonio
  // updated header and footer
  //
  //*****************  Version 32  *****************
  //User: Kevin           Date:  8/23/00  Time: 12:26p
  //Checked in $/Viatron1000/src/ReaderLib
  //Comment:
  //  Reduced the Hash table to save some memory
  //
  //*****************  Version 31  *****************
  //User: Kevin           Date:  8/20/00  Time:  1:48p
  //Checked in $/Viatron1000/src/ReaderLib
  //Comment:
  //  Fixed centerline coloring
  //
  //*****************  Version 30  *****************
  //User: Kevin           Date:  8/16/00  Time: 12:39p
  //Checked in $/Viatron1000/src/ReaderLib
  //Comment:
  //  Chabged tu use ddb instead of dfb
  //
  //*****************  Version 29  *****************
  //User: Kevin           Date:  8/13/00  Time:  4:50p
  //Checked in $/Viatron1000/src/ReaderLib
  //Comment:
  //  Added memory usage debugs (commented out though)
  //
  //*****************  Version 28  *****************
  //User: Frank           Date:  8/11/00  Time:  4:25p
  //Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Debugged skeleton index
//
//*****************  Version 27  *****************
//User: Frank           Date:  8/11/00  Time:  2:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated to reflect new Volume classes
//
//*****************  Version 26  *****************
//User: Frank           Date:  8/10/00  Time:  4:11p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  No background is necessary in DFB
//
//*****************  Version 25  *****************
//User: Kevin           Date:  8/07/00  Time:  6:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed CSI problems with new segment data structures
//
//*****************  Version 24  *****************
//User: Kevin           Date:  8/06/00  Time:  9:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  moved m_colonWall Thershold ot readerGloabl (from fields) and moved
//the reading of the inf file
//
//*****************  Version 23  *****************
//User: Kevin           Date:  8/04/00  Time:  4:42p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added inside file
//Changed aborts to warns
//Added background voxels
//
//*****************  Version 22  *****************
//User: Kevin           Date:  8/02/00  Time:  2:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Begining of multi-segmnet handling
//
//*****************  Version 21  *****************
//User: Kevin           Date:  8/01/00  Time: 10:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added num segments to inf file
//
//*****************  Version 20  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 19  *****************
//User: Kevin           Date:  8/01/00  Time: 10:23a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Code Cleanup after changeing to new file format
//
//*****************  Version 18  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 17  *****************
//User: Frank           Date:  7/27/00  Time: 11:20a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added SetBackground to set the return value of empty voxels
//
//*****************  Version 16  *****************
//User: Frank           Date:  7/27/00  Time:  9:47a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed Aborts to Warns
//
//*****************  Version 15  *****************
//User: Kevin           Date:  7/26/00  Time:  8:51p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added the use of .sld (space lead distance" file. But it did not
//work, so commented it out (left it in though until v2000 gets it computed
//correctly)
//
//*****************  Version 14  *****************
//User: Frank           Date:  7/26/00  Time:  4:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added DFS and CSI files and support
//
//*****************  Version 13  *****************
//User: Frank           Date:  7/26/00  Time: 10:15a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Cleaned out extra functions
//
//*****************  Version 12  *****************
//User: Frank           Date:  7/26/00  Time:  8:17a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Cleaned out old functions and updated header
//
//*****************  Version 11  *****************
//User: Frank           Date:  7/25/00  Time:  1:19p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Disabled validity testing
//
//*****************  Version 10  *****************
//User: Frank           Date:  7/25/00  Time:  1:07p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed volume format problem
//
//*****************  Version 9  *****************
//User: Frank           Date:  7/25/00  Time: 10:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added validity testing for position
//
//*****************  Version 8  *****************
//User: Frank           Date:  7/25/00  Time: 10:06a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed debugging information
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 6  *****************
//User: Frank           Date:  7/20/00  Time:  1:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added PDS field using new Volume class
//
//*****************  Version 5  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 4  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 4
//Comment:
//  
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:35p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/17/00  Time:  4:19a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Improved the performance of ray casting by 16%
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Fields.C
//Comment:
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Fields.C,v 1.23.2.11 2010/12/21 15:40:48 dongqing Exp $
// $Id: Fields.C,v 1.23.2.11 2010/12/21 15:40:48 dongqing Exp $
