// $Id: ProcessorEnvironment.C,v 1.16.2.1 2009/07/16 19:05:34 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "ProcessorEnvironment.h"
#include "Volume.h"
#include "FileExtensions.h"
#include "File.h"
#include "vxBlockVolumeFileReader.h"


#define FILE_REVISION "$Revision: 1.16.2.1 $"

/**
 * Default Constructor
 */
ProcessorEnvironment::ProcessorEnvironment()
{
} // Processor()


/**
 * Called after the object is fully instantiated.
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT ProcessorEnvironment::FinalConstruct()
{
  CComCreator< CComObject< Volume< uint2  > > >::CreateInstance( NULL, __uuidof( IVolume ), reinterpret_cast< void ** >( &m_spIntensityVolume ) );
  CComCreator< CComObject< Volume< uint1 > > >::CreateInstance( NULL, __uuidof( IVolume ), reinterpret_cast< void ** >( &m_spSegmentVolume ) );

  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void ProcessorEnvironment::FinalRelease()
{
  m_spIntensityVolume = NULL;
  m_spSegmentVolume = NULL;
} // FinalRelease()


/**
 * Gets the intensity volume
 *
 * @param   ppiIntensityVolume    upon return contains an IVolume interface pointer
 * @return S_OK for success
 */
STDMETHODIMP ProcessorEnvironment::get_IntensityVolume( IVolume ** ppiIntensityVolume )
{
	if( !ppiIntensityVolume )
    return E_INVALIDARG;
  return m_spIntensityVolume.QueryInterface( ppiIntensityVolume );
} // get_IntensityVolume( IVolume ** ppiIntensityVolume )


/**
 * Gets the segment volume
 *
 * @param   ppiSegmentVolume    upon return contains an IVolume interface pointer
 * @return S_OK for success
 */
STDMETHODIMP ProcessorEnvironment::get_SegmentVolume( IVolume ** ppiSegmentVolume )
{
	if( !ppiSegmentVolume )
    return E_INVALIDARG;
  return m_spSegmentVolume.QueryInterface( ppiSegmentVolume );
} // get_SegmentVolume( IVolume ** ppiSegmentVolume )


/**
 * Gets the dicom directory
 *
 * @param   psDicomDirectory    upon return contains the dicom directory
 * @return S_OK for success
 */
STDMETHODIMP ProcessorEnvironment::get_DicomDirectory( BSTR * psDicomDirectory )
{
	if( !psDicomDirectory )
    return E_INVALIDARG;
  return m_sDicomDirectory.CopyTo( psDicomDirectory );
} // get_DicomDirectory( BSTR * psDicomDirectory )


/**
 * Gets the dicom directory
 *
 * @param   sDicomDirectory    dicom directory
 * @return S_OK for success
 */
STDMETHODIMP ProcessorEnvironment::put_DicomDirectory( BSTR  sDicomDirectory )
{
  m_sDicomDirectory = sDicomDirectory;
  return S_OK;
} // put_DicomDirectory( BSTR  sDicomDirectory )


/**
 * Gets the cad destination directory
 *
 * @param   psDestDirectory    upon return contains the cad destination directory
 * @return S_OK for success
 */
STDMETHODIMP ProcessorEnvironment::get_DestDirectory( BSTR * psDestDirectory )
{
	if( !psDestDirectory )
    return E_INVALIDARG;
  return m_sDestFilePath.CopyTo( psDestDirectory );
} // get_DestDirectory( BSTR * psDestDirectory )


/**
 * Gets the cad destination directory
 *
 * @param   sDestDirectory    cad destination directory
 * @return S_OK for success
 */
STDMETHODIMP ProcessorEnvironment::put_DestDirectory( BSTR  sDestDirectory )
{
  m_sDestFilePath = sDestDirectory;
  return S_OK;
} // put_DestDirectory( BSTR  sDestDirectory )


/**
 * Initializes the environment object
 */
void ProcessorEnvironment::Initialize( const std::string & spreProcessDirectory, const std::string & sDicomDirectory, 
                                       const std::string & sVolumeDirectory, const std::string & sDestDirectory, const bool & bDicomOnly )
{
  m_sDicomDirectory = sDicomDirectory.c_str();
  m_sDestFilePath = sDestDirectory.c_str();
  if( bDicomOnly )
    return;
 
  //Find the Hq Dif file
  std::string sEcvHqFile;
  std::string sEcvHqFileExtension = sVolumeDirectory + "_hq." + vx::ElecCleansedVolExt;
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind = FindFirstFile( sEcvHqFileExtension.c_str(), &FindFileData );
  if (hFind != INVALID_HANDLE_VALUE) 
  {
    sEcvHqFile = sEcvHqFileExtension;
    FindClose(hFind);
  }

  //Find the Volume file
  std::string sVolFile = sVolumeDirectory + "." + vx::VolumeExt; 

  std::string sDifHqFile = spreProcessDirectory + vx::DifferenceVolExt;

  // load the cleansed and diff volumes
  vxBlockVolume< uint2 > volume;

  if( vx::File( sEcvHqFile ).Exists() && vx::File( sDifHqFile ).Exists() )
  {
    volume.Read( sEcvHqFile );
    
    vxBlockVolume< uint2 > diffVolume;
    diffVolume.Read( sDifHqFile );

    // create an iterator for the cleansed and diff volumes
    vxBlockVolumeIterator< uint2 > volumeIter( volume );
    vxBlockVolumeIterator< uint2 > diffIter( diffVolume );

    // create the "uncleansed" volume by combining the cleansed and difference volumes
    for( ; diffIter.IsNotAtEnd(); diffIter.NextNonBackgroundBlockZYX() )
    {      
      volumeIter.SetPos( diffIter.GetPos() );
      for( ; diffIter.IsNotAtEndOfBlock(); diffIter.NextZYXinsideBlock(), volumeIter.NextZYXinsideBlock() )
      {
        if( diffIter.GetVoxel() != 0 )
          volumeIter.SetVoxel( diffIter.GetVoxel() );
      }
    } // for( ; diffIter.IsNotAtEnd(); diffIter.NextNonBackgroundBlockZYX() )
  }
  else if( vx::File( sVolFile ).Exists() )
  {
		vxBlockVolumeFileReader< uint2 >::Read( sVolFile, volume );
		volume.Compact();
  }
  else
  {
    throw ex::VException( LogRec( "Intensity volume file is missing.", "ProcessorEnvironment", "Initialize" ) );
  }
  GetCoClassPtr< Volume< uint2 > >( m_spIntensityVolume )->SetVolume( volume );
 

  vx::File segmentFile( spreProcessDirectory +  vx::InsideLabelVolExt );
  if( segmentFile.Exists() )
  {
    // load the segment volume
    vxBlockVolume< uint1 > segmentVolume;
		vxBlockVolumeFileReader<uint1>::Read( segmentFile.GetAbsolutePath(), segmentVolume );
		segmentVolume.Compact();
    GetCoClassPtr< Volume< uint1 > >( m_spSegmentVolume )->SetVolume( segmentVolume );
  }
  else
  {
    vxBlockVolume< uint1 > segmentVolume( volume.GetHeader() );
    GetCoClassPtr< Volume< uint1 > >( m_spSegmentVolume )->SetVolume( segmentVolume );
    LogWrn( "Segment volume does not exist. Using empty volume.", "ProcessorEnvironment", "Initialize" );
  }
} // Initialize


// $Log: ProcessorEnvironment.C,v $
// Revision 1.16.2.1  2009/07/16 19:05:34  dongqing
// add cad destination path into the processorEnvironment
//
// Revision 1.16  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.15  2006/06/26 21:11:58  dongqing
// change the input volume voxel type from SHORT to UINT2
//
// Revision 1.14  2006/02/09 21:19:29  romy
// moved commandline processing to ColonEnvironmentArgs class
//
// Revision 1.13  2006/02/09 19:28:47  romy
// updated to take the vol file path
//
// Revision 1.12  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.11  2005/11/08 14:42:17  geconomos
// corrected error related to "-dicomonly" flag
//
// Revision 1.10.2.1  2005/12/19 15:50:36  geconomos
// add flag to Initialize() to indicate if only the dicom directory is valid
//
// Revision 1.10  2005/10/12 13:25:34  geconomos
// code cleanup
//
// Revision 1.9  2005/10/11 19:27:49  geconomos
// renabled loading of segmentation volume
//
// Revision 1.8  2005/10/11 13:23:29  geconomos
// renabled loading of .vol file
//
// Revision 1.7  2005/10/06 17:59:08  geconomos
// temporarily commented out reading of .vol file condition
//
// Revision 1.6  2005/09/30 19:54:45  geconomos
// not loading ins volume - temporary
//
// Revision 1.5  2005/09/29 13:59:14  geconomos
// updated to load .vol instead of low quality ecv and dif if high quality is not present
//
// Revision 1.4  2005/09/23 14:33:40  geconomos
// improved error handing
//
// Revision 1.3  2005/09/20 19:49:17  geconomos
// changed voxel type of intensity value to int2
//
// Revision 1.2  2005/09/20 18:58:00  geconomos
// changed all interfaces to be dual instead of cutom only
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ProcessorEnvironment.C,v 1.16.2.1 2009/07/16 19:05:34 dongqing Exp $
// $Id: ProcessorEnvironment.C,v 1.16.2.1 2009/07/16 19:05:34 dongqing Exp $
