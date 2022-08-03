// include declarations
#include "stdafx.h"
#include "ProcessorPlugin.h"
#include <iostream>

#include <windows.h>
#include <direct.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <math.h>
#include <vector>


// namespace declaration
using namespace vxComCAD;


/**
 * 
 */
HRESULT ProcessorPlugin::Run( vxComCAD::IProcessorPtr & processor )
{
  std::string  sSeriesDCMpath( static_cast< const char * >( _bstr_t( processor->Environment->DicomDirectory ) ) );
  std::string  sLogInfo = "~~~~~~~~~~~~~~~~Dicom files path: "+sSeriesDCMpath;
  processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );

  // DICOM filename list container
  std::vector<std::string> vsDCMfileList;

  // The following block of codes is for getting all dicom file names
  // in the directory. We assume that there is only one series in the 
  // directory and there is no non-DICOM file.
	try
  { // go to the DICOM directory
    if( _chdir(sSeriesDCMpath.c_str()) < 0 )
    {
      sLogInfo = "Fail to locate the DICOM directory";
      processor->Logger->LogError( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
      return(E_FAIL);
    };

    WIN32_FIND_DATA  FileData; 
    HANDLE           hSearch; 
    
    // Start searching for *.dcm files in the current directory.
    hSearch = FindFirstFile("*.dcm", &FileData); 

    if( hSearch == INVALID_HANDLE_VALUE ) 
    { 
      sLogInfo = "No DICOM file in the directory";
      processor->Logger->LogError( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
      return(E_FAIL);
    } // if
    else
    {
			vsDCMfileList.push_back( FileData.cFileName );
    } // if-else find a DICOM file
    
    bool bFinished(false); 
    // Get each DICOM filename    
    while( !bFinished ) 
    { 
      if( FindNextFile(hSearch, &FileData) ) 
      {
        vsDCMfileList.push_back( FileData.cFileName );
      } // if
      else
      {
        bFinished = true;
      } // if( find more DICOM files)
    } // while
    
    // Close the search handle.      
    FindClose(hSearch);

    if( vsDCMfileList.size() < 1 )
    {
      sLogInfo = "DICOM file is not found";
      processor->Logger->LogError( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
      return(E_FAIL);
    }
    else
    {
      std::stringstream sst;
      sst << "Find " << vsDCMfileList.size() << " DICOM files";
      sLogInfo = sst.str();
      processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
    } // if-else
  } // end of getting DICOM file names 
	catch( ... )
	{
    sLogInfo = "Fail to get DICOM filename list";
    processor->Logger->LogError( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
    return(E_FAIL);
	}

	try
	{ /// initial 

    /// Execute the Dummy CAD API ////////////////////////////////////////////////////
    sLogInfo = "=========== Start Dummy CAD preprocessing ===========";
    processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );

		const int iN = 512*512*256;
		int * piData = new int[iN];
		if( piData == NULL )
		{
      sLogInfo = "Out of memory";
      processor->Logger->LogError( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
      return(E_FAIL);
		}
		for( int ii=0; ii<iN; ii++ )
			piData[ii] = (iN-ii)/9;

		Sleep(20000);

		delete piData;

    sLogInfo = "============== End Dummy CAD Processing ===============";
    processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );

		int iNumOfFindings = 2; 
    // add finding ///////////////////////////////////////////////////////////
    if( iNumOfFindings < 1 )
    {
      sLogInfo = "The CAD finding list is empty";
      processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
      return(S_OK);
    }
    else
    {
      std::stringstream sst;
      sst << "Number of findings in the list is "<< iNumOfFindings;
      sLogInfo = sst.str();
      processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
    }
 
    /*/ get the body position
    IVolumePtr   intensityVolume  = processor->Environment->IntensityVolume;
    std::string  sBodyPosition = static_cast< const char * >( intensityVolume->GetHdrVar("0018", "5100") );
  
    // check the body position
    if( sBodyPosition[2] != 'S' && sBodyPosition[2] != 'P' && sBodyPosition[2] != 'D' )
    {
      sLogInfo = "Un-supported body position";
      processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );
    } 
  
    // get volume dimensions
		const int XX(intensityVolume->Dimensions->X);
		const int YY(intensityVolume->Dimensions->Y);
    const int ZZ(intensityVolume->Dimensions->Z);
		const float fx( intensityVolume->Units->X );
		const float fy( intensityVolume->Units->Y ); 
		const float fz( intensityVolume->Units->Z );

		// determine the colonic segment ID associated to each finding
		try
		{ // grab the INS volume 
      IVolumePtr volumeSegment = processor->Environment->SegmentVolume;

			if( ZZ != volumeSegment->Dimensions->Z )
			{ // The compressed3D switch on
				processor->Logger->LogError( "3D display switch is in wrong setting", "Run", "ProcessorPlugin" );
        return E_FAIL;
			} // if

      // pointer to contiguous memory for volume 
      unsigned char * pDataSegment( NULL );

      // access the voxel data
      _variant_t voxelsSegment( volumeSegment->Voxels );
      SafeArrayAccessData( voxelsSegment.parray, reinterpret_cast< void ** >( &pDataSegment ) );

			const int iS ( XX*YY );
			const int iN ( iS*ZZ-1   );
			int iCurrentOffset(0);
			bool bFound(false);
			for( int i=0; i<iNumOfFindings; i++ )
			{
				iCurrentOffset = 12    +
				                 12*XX +
				                 12*iS;

				bFound = false;

				if( pDataSegment[iCurrentOffset] > 0x00 )
				{
					bFound = true;
				} // if
        
			} // for(i)

      // unlock voxel memory
      SafeArrayUnaccessData( voxelsSegment.parray );

      sLogInfo = "Finish determining segment ID";
      processor->Logger->LogInformation( sLogInfo.c_str(), "Run", "ProcessorPlugin" );

		} // determine the colonic segment ID associated to each finding
		catch( ... )
		{
      processor->Logger->LogError( "Fail to identify segment ID", "Run", "ProcessorPlugin" );
      return E_FAIL;
		}
		*/

    processor->Logger->LogInformation( "Adding findings", "Run", "ProcessorPlugin" );
    processor->Findings->Vendor = "Dummy Vendor";
    processor->Findings->Version = "1.1";

    register int ix, iy, iz;
    char buffer[4];
    for( int i=0; i < iNumOfFindings; i++ )
    {
      IFindingPtr finding = processor->Findings->CreateFinding();

      // segment number
      finding->Segment = i+1;

      // largest dimension
      finding->Dimensions->Largest->StartPoint->Set( 0.0F, 0.0F, 0.0F );
      finding->Dimensions->Largest->EndPoint->Set  ( 1.0F, 1.0F, 1.0F );
    
      // perpendicular dimension
      finding->Dimensions->Perpendicular->StartPoint->Set( 0.0F, 0.0F, 0.0F );
      finding->Dimensions->Perpendicular->EndPoint->Set  ( 1.0F, 1.0F, 1.0F );

      // depth dimension
      finding->Dimensions->Depth->StartPoint->Set( 0.0F, 0.0F, 0.0F );
      finding->Dimensions->Depth->EndPoint->Set  ( 1.0F, 1.0F, 1.0F );
    
      // normal
      finding->Normal->Units = Voxels;
      finding->Normal->Set( 1, 0, 0 );
      
      ix = 12;
      iy = 256;
      iz = 12;

			if( i == 1 )
			{
				ix = 128;
				iy = 2;
				iz = 28;
			}

      // centroid
      finding->Centroid->Units = Voxels;
      finding->Centroid->Set( ix, iy, iz );
    
      // volume roi
      finding->VolumeROI->AverageHU = 1.0F;
      finding->VolumeROI->StandardDeviationHU = 28.0F;

      for( int z=iz-3; z<iz+3; ++z )
        for( int y=iy-3; y<iy+3; ++y )
			    for( int x=ix-3; x<ix+3; ++x )
            finding->VolumeROI->Voxels->Add( x, y, z );

      processor->Findings->Add( finding );  
    } // for
  } // try
  catch( _com_error & e )
  {
    processor->Logger->LogError(  _bstr_t(e.ErrorMessage()), "Run", "ProcessorPlugin" );
    return E_FAIL;
  }
	catch( ... )
	{
    processor->Logger->LogError("Exception throw for some reason", "Run", "ProcessorPlugin" );
    return E_FAIL;
	}

  return S_OK;

} // Run( vxComCAD::IProcessorPtr & processor )

