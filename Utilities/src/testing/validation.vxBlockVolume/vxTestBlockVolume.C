// $Id: vxTestBlockVolume.C,v 1.0 2011/07/19 dongqing Exp $
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
#include "vxTestBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"
#include "vxVolumeHeader.h"
#include "vxBlockVolumeRegionGrowIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeRegionGrowIteratorVoxelValue.h"
#include "Vector3D.h"



/**
 * Constructor.
 * 
 * @param cmdLine Command line argument count
 */
vxTestBlockVolume::vxTestBlockVolume( const std::string & sSourceFilePath, const std::string & sDestFilePath )
	: m_sSourceFilePath( sSourceFilePath ), m_sDestFilePath( sDestFilePath )
{
} // constructor


/** 
 *  destructor
 */
vxTestBlockVolume::~vxTestBlockVolume()
{
} //


/**
 * Execute the pipeline from beginning to end.
 *
 *  @return true if all test items are passed, otherwise false
 */
bool vxTestBlockVolume::Run()
{ 
	std::cout << "  Start the validation ......\n" << std::endl;

	try
	{
	  std::cout << "  1. Create raw phantom data." << std::endl;

		if( !CreateRawPhantom() ) return false;

	  std::cout << "  1. Finish creation of raw phantom data.\n" << std::endl;
		//////////////////////////////////////////////////////////////////

	  std::cout << "  2. Read the new volume." << std::endl;

		const std::string sInputVolFilename = m_sSourceFilePath + "/a.vol";
		//const std::string sInputVolFilename = "c:/temp/vol/new.vol";

		vxBlockVolume<uint2> srcVol;
    srcVol.Read( sInputVolFilename );

	  std::cout << "  2. Finish reading the new volume.\n" << std::endl;
		//////////////////////////////////////////////////////////////////

		const std::string sReportFilename = m_sDestFilePath + "/ValidationResults.txt";
		//const std::string sReportFilename = "c:/temp/vol/ValidationResults.txt";

	  std::cout << "  3. Volume header validation." << std::endl;
		if( !ValidateHeader( srcVol, sReportFilename ) ) return false;
		
	  std::cout << "  3. Finish volume header validation.\n" << std::endl;
		//////////////////////////////////////////////////////////////////

	  std::cout << "  4. Volume value validation." << std::endl;

		const std::string sBenchmarkFilename = m_sSourceFilePath + "/phantom.raw";
		if( !ValidateHUvalue( srcVol, sBenchmarkFilename, sReportFilename ) ) return false;

	  std::cout << "  4. Finish volume value validation.\n" << std::endl;
		//////////////////////////////////////////////////////////////////

	  std::cout << "  5. Volume iterater validation." << std::endl;

		if( !ValidateVolIterator( srcVol, sReportFilename ) ) return false;

	  std::cout << "  5. Finish volume iterater validation.\n" << std::endl;
		//////////////////////////////////////////////////////////////////

		FILE * fp = fopen( sReportFilename.c_str(), "a+" );
		if( fp == NULL )
		{
			std::cout << "      Error: could not write to the report file." << std::endl;
			return false;
		}
		fseek( fp, 0, SEEK_END );

		fprintf( fp, "\n\n\n    Tester name: __________________________________\n", "");
		fprintf( fp, "\n\n\n    Signature:   __________________________________\n", "");
		fprintf( fp, "\n\n\n    Test date:   __________________________________\n", "");
		fclose(fp);
	}
	catch( ... )
	{
		std::cout << "      Error: exception thrown." << std::endl;
		return false;
	}

	std::cout << "\n  Validation finished." << std::endl;
	return true;

} // Run


/**
 *  create raw phantom data
 * 
 *  @return true if successful
 */
bool vxTestBlockVolume::CreateRawPhantom()
{
	int2 * pData = new int2[512*512*350];
	if( pData == NULL )
	{
		std::cout << "      Error: out of memory." << std::endl;
		return false;
	}

	register int4 x, y, z, s, n;

	const int4 iS = 512*512;
	const int4 iX = 512;
	const int4 iR2 = 200*200;

	const int4 iBG         = -1024;
	const int4 iSofttissue =   -50;
	const int4 iAir        =  -761;
	const int4 iAir1       = -1000;
	const int4 iAir2       =  -976;
	const int4 iAir3       =  -952;
	const int4 iAir4       =  -928;
	const int4 iTagged     =   250;
	const int4 iBone       =   400;
	const int4 iMuscle     =    50;

	s = 0;
	/// body region
	for( z=0; z<350; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			for( x=0; x<512; x++ )
			{
				if( (x-255)*(x-255)+(y-255)*(y-255) <= iR2 )
				{
					pData[x+n] = iSofttissue;
				}
				else
				{
					pData[x+n] = iBG;
				}
			} // for x

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set body" << std::endl;

	s = 320*iS;
	/// lung region
	for( z=320; z<350; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 155) && (y <350) )
			{
				for( x=100; x<200; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iAir;
					} // if
				} // for x

				for( x=300; x<400; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iAir;
					} // if
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set lung" << std::endl;

	s = 0;
	/// spine region
	for( z=0; z<350; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 375) && (y < 415) )
			{
				for( x=220; x<275; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iBone;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set bone" << std::endl;

	s = 0;
	/// rectum
	for( z=0; z<60; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 205) && (y < 295) )
			{
				for( x=205; x<295; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iAir1;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set rectum" << std::endl;

	s = 60*iS;
	/// sigmoid
	for( z=60; z<90; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 225) && (y < 275) )
			{
				for( x=220; x<440; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iAir2;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set sigmoid" << std::endl;

	s = 90*iS;
	/// descending
	for( z=90; z<280; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 210) && (y < 275) )
			{
				for( x=410; x<440; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iAir3;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set descending" << std::endl;

	s = 280*iS;
	/// transverse
	for( z=280; z<310; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 240) && (y < 275) )
			{
				for( x=70; x<440; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iTagged;
					}
				} // for x
			} // if

			if( (y >= 210) && (y < 240) )
			{
				for( x=70; x<440; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iAir4;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set transverse" << std::endl;

	s = 160*iS;
	/// ascending
	for( z=160; z<280; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 240) && (y < 275) )
			{
				for( x=70; x<120; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iTagged;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	std::cout << "    Set ascending" << std::endl;

	/// set muscle block
	s = 60*iS;
	/// body region
	for( z=60; z<79; z++ )
	{
		n = s;
		for( y=0; y<512; y++ )
		{
			if( (y >= 300) && (y < 400) )
			{
				for( x=300; x<400; x++ )
				{
					if( pData[x+n] == iSofttissue )
					{
						pData[x+n] = iMuscle;
					}
				} // for x
			} // if

			n = n + iX;
		} // for y

		s = s + iS;
	} // for z

	int4 xx = 512;
	int4 zz = 350;

	std::string sRawPath = m_sSourceFilePath + "/phantom.raw";

	FILE *fp = fopen( sRawPath.c_str(), "wb" );
	if( fp == NULL )
	{
		std::cout << "      Error: could not write raw to disc." << std::endl;
		return false;
	}
	fwrite( &xx, sizeof(int4), 1, fp );
	fwrite( &xx, sizeof(int4), 1, fp );
	fwrite( &zz, sizeof(int4), 1, fp );
	fwrite( pData, sizeof(int2), iS*zz, fp );
	fclose(fp);

	delete pData;

	return true;
} // CreateRawPhantom()


/**
 *  validate the header information
 *
 *  @param vol the input block volume
 *  @param sReportFilename the output test report file name
 *  @return true if finishing successful
 */
bool vxTestBlockVolume::ValidateHeader( vxBlockVolume<uint2> & vol, const std::string & sReportFilename )
{
	const vxVolumeHeader header = vol.GetHeader();
	const Triple<int4>   viDim  = header.GetVolDim();

	std::cout << "   dimension = " << viDim << std::endl;

	if( viDim.m_x <= 0 || viDim.m_x > 2048 || viDim.m_y <= 0 ||
		  viDim.m_y > 2048 || viDim.m_z <= 0 || viDim.m_z > 2048 )
	{
		std::cout << "      Error: volume header corrupted." << std::endl;
		return false;
	} // if
	
	/// parameters from v3D volume header
	std::string shStudyDate;
	std::string shPhysicianName;
	std::string shPatientName;
	std::string shPatientID;
	std::string shPatientDB;
	std::string shPatientSex;
	std::string shPatientPosition;
	std::string shModality;
	std::string shBias;
	
	header.GetHeaderEntry( vxVolumeHeader::StudyDate,             shStudyDate );
	header.GetHeaderEntry( vxVolumeHeader::PhysicianName,     shPhysicianName );
	header.GetHeaderEntry( vxVolumeHeader::PatientName,         shPatientName );
	header.GetHeaderEntry( vxVolumeHeader::PatientID,             shPatientID );
	header.GetHeaderEntry( vxVolumeHeader::PatientBirthdate,      shPatientDB );
	header.GetHeaderEntry( vxVolumeHeader::PatientSex,           shPatientSex );
	header.GetHeaderEntry( vxVolumeHeader::PatientPosition, shPatientPosition );
	header.GetHeaderEntry( vxVolumeHeader::DataModality,           shModality );
	header.GetHeaderEntry( vxVolumeHeader::Bias,                       shBias );

	/// parameters from DICOM fields
	std::string sSOPClassUID;
	std::string sdStudyDate;
	std::string sdModality;
	std::string sdInstitutionName;
	std::string sdPhysicianName;
	std::string sdStudyDescription;
	std::string sdSeriesDescription;
	std::string sdPatientName;
	std::string sdPatientID;
	std::string sdPatientDB;
	std::string sdPatientSex;
	std::string sdPatientPosition;
	std::string sdStudyUID;
	std::string sdSeriesUID;
	std::string sdAcquiNumber;
	std::string sdOrientation;
	std::string sdRows;
	std::string sdColumns;
	std::string sdPixelSpacing;
	std::string sdScaleIntercept;
	std::string sdRecaleSlope;

  sSOPClassUID        = header.GetDicomFieldValue("0008", "0016", "");
	sdStudyDate         = header.GetDicomFieldValue("0008", "0020", "");
  sdModality          = header.GetDicomFieldValue("0008", "0060", "");
	sdInstitutionName   = header.GetDicomFieldValue("0008", "0080", "");
	sdPhysicianName     = header.GetDicomFieldValue("0008", "0090", "");
	sdStudyDescription  = header.GetDicomFieldValue("0008", "1030", "");
	sdSeriesDescription = header.GetDicomFieldValue("0008", "103E", "");
	sdPatientName       = header.GetDicomFieldValue("0010", "0010", "");
  sdPatientID         = header.GetDicomFieldValue("0010", "0020", "");
	sdPatientDB         = header.GetDicomFieldValue("0010", "0030", "");
	sdPatientSex        = header.GetDicomFieldValue("0010", "0040", "");
	sdPatientPosition   = header.GetDicomFieldValue("0018", "5100", "");
	sdStudyUID          = header.GetDicomFieldValue("0020", "000D", "");
	sdSeriesUID         = header.GetDicomFieldValue("0020", "000E", "");
	sdAcquiNumber       = header.GetDicomFieldValue("0020", "0012", "");
	sdOrientation       = header.GetDicomFieldValue("0020", "0037", "");
	sdRows              = header.GetDicomFieldValue("0028", "0010", "");
	sdColumns           = header.GetDicomFieldValue("0028", "0011", "");
	sdPixelSpacing      = header.GetDicomFieldValue("0028", "0030", "");
  sdScaleIntercept    = header.GetDicomFieldValue("0028", "1052", "");
  sdRecaleSlope       = header.GetDicomFieldValue("0028", "1053", "");

	FILE * fp = fopen( sReportFilename.c_str(), "w" );
	if( fp == NULL )
	{
		std::cout << "      Error: could not write out the report." << std::endl;
		return false;
	}
	fprintf( fp, "Viatronix Virtual Colonoscopy Digital Phantom Validation Report%s\n\n", "");

	fprintf( fp, "Part I: Volume Header Information%s\n", "");
	fprintf( fp, "===================================\n", "");
	fprintf( fp, "vx header study date:        %s\n",        shStudyDate.c_str() );
	fprintf( fp, "vx header physician name:    %s\n",    shPhysicianName.c_str() );
	fprintf( fp, "vx header patient name:      %s\n",      shPatientName.c_str() );
	fprintf( fp, "vx header patient ID:        %s\n",        shPatientID.c_str() );
	fprintf( fp, "vx header patient DB:        %s\n",        shPatientDB.c_str() );
	fprintf( fp, "vx header patient sex:       %s\n",       shPatientSex.c_str() );
	fprintf( fp, "vx header patient position:  %s\n",  shPatientPosition.c_str() );
	fprintf( fp, "vx header modality:          %s\n",         shModality.c_str() );
	fprintf( fp, "vx header bias:              %s\n",             shBias.c_str() );

	fprintf( fp, "\n\n        DICOM Field Values From V3D Volume\n\n",  sSOPClassUID.c_str() );
	fprintf( fp, "-----------------------------------------------------------------\n" );
	fprintf( fp, " Tag ID   |         Tag Name            |     Extracted Value\n",  sSOPClassUID.c_str() );
	fprintf( fp, "-----------------------------------------------------------------\n" );

	fprintf( fp, "0008-0016   SOP Class UID:               %s\n", sSOPClassUID.c_str() );
	fprintf( fp, "0008-0020   Study Date:                  %s\n",	sdStudyDate.c_str() );
	fprintf( fp, "0008-0060   Modality:                    %s\n", sdModality.c_str() );
	fprintf( fp, "0008-0080   Institutional Name:          %s\n",	sdInstitutionName.c_str() );
	fprintf( fp, "0008-0090   Referring Physician's Name:  %s\n",	sdPhysicianName.c_str() );
	fprintf( fp, "0008-1030   Study Description:           %s\n",	sdStudyDescription.c_str() );
	fprintf( fp, "0008-103E   Series Description:          %s\n",	sdSeriesDescription.c_str() );
	fprintf( fp, "0010-0010   Patient's Name:              %s\n",	sdPatientName.c_str() );
	fprintf( fp, "0010-0020   Patient ID:                  %s\n", sdPatientID.c_str() );
	fprintf( fp, "0010-0030   Patient's Birth Date:        %s\n",	sdPatientDB.c_str() );
	fprintf( fp, "0010-0040   Patient's Sex:               %s\n",	sdPatientSex.c_str() );
	fprintf( fp, "0018-5100   Patient Position:            %s\n",	sdPatientPosition.c_str() );
	fprintf( fp, "0020-000D   Study Instance UID:          %s\n",	sdStudyUID.c_str() );
	fprintf( fp, "0020-000E   Series Instance UID:         %s\n",	sdSeriesUID.c_str() );
	fprintf( fp, "0020-0012   Acquisition Number:          %s\n",	sdAcquiNumber.c_str() );
	fprintf( fp, "0020-0037   Image Orientation Patient:   %s\n",	sdOrientation.c_str() );
	fprintf( fp, "0028-0010   Rows:                        %s\n",	sdRows.c_str() );
	fprintf( fp, "0028-0011   Columns:                     %s\n",	sdColumns.c_str() );
	fprintf( fp, "0028-0030   Pixel Spacing:               %s\n",	sdPixelSpacing.c_str() );
	fprintf( fp, "0028-1052   Rescale Intercept:           %s\n", sdScaleIntercept.c_str() );
	fprintf( fp, "0028-1053   Rescale Slope:               %s\n", sdRecaleSlope.c_str() );
	fclose(fp);

	fclose(fp);

	return true;
} // ValidateHeader


/**
 *  validate the voxel HU value
 *
 *  @param vol the input block volume
 *  @param sBenchmarkFilename the input benchmark raw phantom file name
 *  @param sReportFilename the output test report file name
 *  @return true if finishing successful
 */
bool vxTestBlockVolume::ValidateHUvalue( vxBlockVolume<uint2> & vol, const std::string & sBenchmarkFilename, 
	             const std::string & sReportFilename )
{
	FILE * fp = fopen( sBenchmarkFilename.c_str(), "rb" );
	if( fp == NULL )
	{
		std::cout << "      Error: could not find benchmark phantom data." << std::endl;
		return false;
	} // if

	int4 iX, iY, iZ;
	fread(&iX, sizeof(int4), 1, fp);
	fread(&iY, sizeof(int4), 1, fp);
	fread(&iZ, sizeof(int4), 1, fp);

	if( iX<=0 || iX>2048 || iY<=0 || iY>2048 || iZ<=0 || iZ>2048 )
	{
		std::cout << "      Error: raw phantom was corrupted." << std::endl;
		return false;
	} // if
		
	vxVolumeHeader header = vol.GetHeader();
	if( iX != header.GetVolDim().m_x || iY != header.GetVolDim().m_y || 
			iZ != header.GetVolDim().m_z ) 
	{
		std::cout << "      Error: Volume dimension did not match." << std::endl;
		return false;
	}

	const int4 iS = iX*iY;
	const int4 iVoxNum = iS*iZ;
	int2 * pPhantom = new int2[iVoxNum];
	if( pPhantom == NULL )
	{
		std::cout << "      Error: Out of memory." << std::endl;
		return false;
	} // if
			
	fread( pPhantom, sizeof(int2), iVoxNum, fp);
	fclose(fp);

	vxBlockVolumeIterator<uint2> it(vol);
	register int4 x, y, z, s, n;

	s = 0;
	for( z=0, it.SetPosZ(0); z<iZ; z++, it.IncZ() )
	{
		n = s;
		for( y=0, it.SetPosY(0); y<iY; y++, it.IncY() )
		{
			for( x=0, it.SetPosX(0); x<iX; x++, it.IncX() )
			{
				if( !(pPhantom[n+x] == vxIntensityConverter::RawToModality(it.GetVoxel(), header)) )
				{
						std::cout << "      Error: HU value did not match raw data at (" << x << ", " << y << ", " << z << ")" << std::endl;
						return(false);
				} // if
			} // for x

			n = n + iX;
		} // for y

		s = s + iS;
		std::cout << "  " << z;
	} // for z
	std::cout << std::endl;

	delete pPhantom;

  fp = fopen( sReportFilename.c_str(), "a+" );
	if( fp == NULL )
	{
		std::cout << "      Error: could not write out the report." << std::endl;
		return false;
	}
	fseek( fp, 0, SEEK_END );

	fprintf( fp, "\nPart III: HU value validation%s\n", "");
	fprintf( fp, "===================================\n", "");
	fprintf( fp, "    PASS\n", "");

	fclose(fp);

	return true;
} // ValidateHUvalue


/**
 *  validate volume iterators
 *
 *  @param vol the input block volume
 *  @param sReportFilename the path to the output report text file
 *  @return true if successful
 */
bool vxTestBlockVolume::ValidateVolIterator( vxBlockVolume<uint2> & vol, const std::string & sReportFilename )
{
	bool bIteratorTestPass = false;
	register long iCount = 0;
	const uint2 uLung    = vxIntensityConverter::ModalityToRaw( -761, vol.GetHeader() );
	const uint2 uLungNew = uLung + 10;
	{
	  vxBlockVolumeIterator<uint2> it(vol);
		for( ; it.IsNotAtEnd(); it.NextBlockZYX() )
		{
			for( ; it.IsNotAtEndOfBlock(); it.NextZYXinsideBlock() )
			{
				if( it.GetVoxel() == uLung )
				{
					iCount++;
				  it.SetVoxel( uLungNew );
				}

			} // for inside block
		} // go over all blocks

		it.SetPos(128, 180, 330);

		if( (iCount == 1170000) && (it.GetVoxel() == uLungNew) )
			bIteratorTestPass = true;

		it.SetPos(513, 400, 40);
		if( it.IsInsideVolume() )
			bIteratorTestPass = false;
	} // iterator test 

	bool  bIteratorNeighborTestPass = false;
	const uint2 uMuscle             = vxIntensityConverter::ModalityToRaw( 50, vol.GetHeader() );
	{
		iCount = 0;
		vxBlockVolumeIteratorNeighbors<uint2> itb06(vol, (Vector3D<int4>(300, 300, 60)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06);
		for( ; !itb06.IsAtEnd(); ++itb06 )
		{
			if( itb06.GetVoxel() == uMuscle )
				iCount++;
		} // for 

		if( iCount == 3 )
    {
	    bIteratorNeighborTestPass = true;
		}
		
		iCount = 0;
		vxBlockVolumeIteratorNeighbors<uint2> itb18(vol, (Vector3D<int4>(300, 300, 60)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18);
		for( ; !itb18.IsAtEnd(); ++itb18 )
		{
			if( itb18.GetVoxel() == uMuscle )
				iCount++;
		} // for 

		if( !(iCount == 6) && bIteratorNeighborTestPass )
    {
	    bIteratorNeighborTestPass = false;
		}

		iCount = 0;
		vxBlockVolumeIteratorNeighbors<uint2> itb26(vol, (Vector3D<int4>(300, 300, 60)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26);
		for( ; !itb26.IsAtEnd(); ++itb26 )
		{
			if( itb26.GetVoxel() == uMuscle )
				iCount++;
		} // for 

		if( !(iCount == 7) && bIteratorNeighborTestPass )
    {
	    bIteratorNeighborTestPass = false;
		}		
	} // neighbors iterator test

	bool bRegionGrowIteratorTestPass = false;
	const uint2 uBG    = vxIntensityConverter::ModalityToRaw( -1024, vol.GetHeader() );
	const uint2 uAirUp = vxIntensityConverter::ModalityToRaw(  -600, vol.GetHeader() );
	{
		iCount = 0;
		vxBlockVolume<bool>   maskVol( vol.GetHeader() );
		vxDeciderRange<uint2> decider( uBG, uAirUp );
		Vector3D<int4>        viSeed( 257, 245, 31 );

    vxBlockVolumeRegionGrowIterator<uint2> regionGrowIter(viSeed, vol, decider, 
                     vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06, &maskVol);

		std::cout << "    please wait ......" << std::endl; /// the region grow is very slow!

		// use it to find all voxels in the intensity range
		for( ; regionGrowIter.IsNotAtEnd(); ++regionGrowIter )
		{
			if( !regionGrowIter.IsInsideVolume() )
			{
				std::cout << "      Error: grow outside of the volume." << std::endl;
				return false;
			}

			++iCount; 
		} // for

		if( iCount == 1519500 )
    {
	    bRegionGrowIteratorTestPass = true;
		}		
	} // region grow iterator test


  FILE * fp = fopen( sReportFilename.c_str(), "a+" );
	if( fp == NULL )
	{
		std::cout << "      Error: could not write to the report file." << std::endl;
		return false;
	}
	fseek( fp, 0, SEEK_END );

	fprintf( fp, "\nPart VI: Iterator validation%s\n", "");
	fprintf( fp, "===================================\n", "");
	if( bIteratorTestPass )
	{
		fprintf( fp, "    Iterator test:              PASS\n", "");
	}
	else
	{
		fprintf( fp, "    Iterator test:              FAIL\n", "");
	}
	if( bIteratorNeighborTestPass )
	{
		fprintf( fp, "    Neighbors iterator test:    PASS\n", "");
	}
	else
	{
		fprintf( fp, "    Neighbors iterator test:    FAIL\n", "");
	}
	if( bRegionGrowIteratorTestPass )
	{
		fprintf( fp, "    Region grow iterator test:  PASS\n", "");
	}
	else
	{
		fprintf( fp, "    Region grow iterator test:  FAIL\n", "");
	}
	fclose(fp);

	return true;
} //ValidateVolIterator



// $Log: vxTestBlockVolume.C,v $
// Revision 1.0 2011/06/21  dongqing
//
// $Header: Viatronix.v3D.Processor.Plugins.Colon/vxTestBlockVolume.C,v 1.0 2011/06/21 dongqing Exp $
// $Id: vxTestBlockVolume.C,v 1.0 2011/06/21 dongqing Exp $
