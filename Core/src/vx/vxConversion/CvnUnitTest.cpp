// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing Chen dongqing@viatronix.net

#include "stdafx.h"
#include "CvnUnitTest.h"
#include "timer.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"
#include "CvnExceptions.h"


using namespace cvn;

/**
 * Default constructor
 *
 * @param sVolumeFilePath: the volume file path
 */
CvnUnitTest::CvnUnitTest( const std::string & sVolumeFilePath )
	: m_sVolumeFilePath( sVolumeFilePath )
{
} // UnitTest()



/**
 * Destructor
 */
CvnUnitTest::~CvnUnitTest()
{
} // ~UnitTest()


/**
 * Run all steps of the unit test and output results into a .csv file.
 * The test uses the colon digital phantom dicom study.
 */
void CvnUnitTest::Run()
{
	if( m_sVolumeFilePath.size() < 3 )
    throw CvnInvalidDataException(LogRec("Volume file path was corrupted", "CvnUnitTest", "Run"));

	std::string sPath = m_sVolumeFilePath;
	sPath = sPath.substr( 0, sPath.find( ".vol" ));
	std::string sOutputFileName = sPath + "_UnitTest_Result.csv";
	sPath = sPath + "_0.vol";

	vxBlockVolume<uint2> vol;
	try
	{
		vol.Read( sPath );

	  FILE * fp = fopen( sOutputFileName.c_str(), "w");
	  if( fp == nullptr )
		  throw CvnInvalidDataException(LogRec("Unit test file could not be opened.", "CvnUnitTest", "Run"));

	  fprintf( fp, ", Dicom to Volume, Conversion, Unit Test Results\n");

	  time_t t;
	  time(&t);

	  struct tm * timeinfo;
    timeinfo = localtime( &t );

	  std::string sDate( asctime(timeinfo) ); 
	  sDate = ", Date, " + sDate;

	  fprintf( fp, sDate.c_str() );
	  fprintf( fp, "\n");
	  fprintf( fp, ", , List of Test Items\n\n" );
	  fprintf( fp, "Index, Description, Tested Value, Benchmark Value, Test Result\n" );

		vxBlockVolumeIterator<uint2> it(vol);
		std::string s("");

		Triple<int4> viDim = vol.GetHeader().GetVolDim();
		if( viDim == Triple<int4>(512, 512, 350) )
		  s = "1, Volume dim, " + ToAscii(viDim.m_x) + "/" + ToAscii(viDim.m_y) + "/" + ToAscii(viDim.m_z) + ", 512/512/350, pass\n";
		else
		  s = "1, Volume dim, " + ToAscii(viDim.m_x) + "/" + ToAscii(viDim.m_y) + "/" + ToAscii(viDim.m_z) + ", 512/512/350, fail\n";
		fprintf( fp, s.c_str() );

		Triple<float4> vfUnits = vol.GetHeader().GetVoxelUnits();
		if( vfUnits == Triple<float4>(0.693359f, 0.693359f, 1.0f) )
		  s = "2, voxel unit, " + ToAscii(vfUnits.m_x) + "/" + ToAscii(vfUnits.m_y) + "/" + ToAscii(vfUnits.m_z) + ", 0.693359/0.693359/1.0, pass\n";
		else
		  s = "2, voxel unit, " + ToAscii(vfUnits.m_x) + "/" + ToAscii(vfUnits.m_y) + "/" + ToAscii(vfUnits.m_z) + ", 0.693359/0.693359/1.0, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		vol.GetHeader().GetHeaderEntry( vxVolumeHeader::VarVolHeaderEntryEnum::PatientName, s );
		if( std::string::npos != s.find(",") )
		  s.replace( s.find(","), 1, " " );
		if( s == "VCVOL  PHANTOM" )
		  s = "3, patient name, " + s + ", VCVOL PHANTOM, pass\n";
		else
		  s = "3, patient name, " + s + ", VCVOL PHANTOM, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		vol.GetHeader().GetHeaderEntry( vxVolumeHeader::VarVolHeaderEntryEnum::PatientID, s );
		if( std::string::npos != s.find(",") )
		  s.replace( s.find(","), 1, " " ); 
		if( s == "0249" )
		  s = "4, patient ID, " + s + ", 0249, pass\n";
		else
		  s = "4, patient ID, " + s + ", 0249, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		vol.GetHeader().GetHeaderEntry( vxVolumeHeader::VarVolHeaderEntryEnum::StudyDate, s );
		if( std::string::npos != s.find(",") )
		  s.replace( s.find(","), 1, " " );
		if( s == "2004-01-01" )
		  s = "5, study date, " + s + ", 2004-01-01, pass\n";
		else
		  s = "5, study date, " + s + ", 2004-01-01, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos(348, 259, 340);
		if( -760 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "6, lung HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -760, pass\n";
		else
		  s = "6, lung HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -760, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 255, 387, 315);
		if( 400 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "7, spine HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", 400, pass\n";
		else
		  s = "7, spine HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", 400, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 259, 173, 296);
		if( -49 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "8, tissue HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -49, pass\n";
		else
		  s = "8, tissue HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -49, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 257, 225, 287);
		if( -927 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "9, trans colon air HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -927, pass\n";
		else
		  s = "9, trans colon air HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -927, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 240, 256, 287);
		if( 250 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "10, trans colon tagged HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", 250, pass\n";
		else
		  s = "10, trans colon tagged HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", 250, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 424, 240, 241);
		if( -951 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "11, descending colon HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -951, pass\n";
		else
		  s = "11, descending colon HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -951, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 301, 248, 68);
		if( -975 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "12, sigmoid colon HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -975, pass\n";
		else
		  s = "12, sigmoid colon HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -975, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 253, 255, 43);
		if( -999 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "13, rectum HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -999, pass\n";
		else
		  s = "13, rectum HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", -999, fail\n";
		fprintf( fp, s.c_str() );

		s = "";
		it.SetPos( 343, 355, 68);
		if( 50 == vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader()) )
		  s = "14, muscle HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", 50, pass\n";
		else
		  s = "14, muscle HU, " + ToAscii(vxIntensityConverter::RawToModality(it.GetVoxel(), vol.GetHeader())) + ", 50, fail\n";
		fprintf( fp, s.c_str() );

		float4 fDiameter = 400.0f*vfUnits.m_x; 
		if( fDiameter == 277.3436f )
			s = "15, FOV diameter, " + ToAscii(fDiameter) + ", 277.3436, pass\n";
		else
			s = "15, FOV diameter, " + ToAscii(fDiameter) + ", 277.3436, fail\n";
		fprintf( fp, s.c_str() );

		fprintf( fp, "\n" );
		fprintf( fp, ", Tester name:, , , \n" );
		fprintf( fp, ", Signature:, , , \n" );
		fprintf( fp, sDate.c_str() );

		fclose(fp);
	}
	catch( ... )
	{
    throw CvnInvalidDataException( LogRec("Error occurred in unit test.", "CvnUnitTest", "Run") );
	} 
} // Run()



