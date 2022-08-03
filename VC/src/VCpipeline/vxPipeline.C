// $Id: vxPipeline.C,v 1.0 2011/06/21 dongqing Exp $
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

#include "vxPipeline.h"
#include "File.h"
#include "v2kmonitorclient.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"
#include "SeriesVersion.h"
#include "ReaderGlobal.h"
#include "vxViewerPipeline.h"

using namespace std;
using namespace vx;


#define FILE_REVISION "$Revision: 1.0 $"

/**
 * Constructor.
 * 
 * @param cmdLine Command line argument count
 */
vxPipeline::vxPipeline(CmdLineSwitches cmdLine) : m_cmdLine(cmdLine), m_servCmdLine(cmdLine)
{
  m_sVoxelType                 = "uint2";

  m_bCRESOn                    = true;

  m_sModality                  = "UNKNOWN";

	m_iErrorCode                 = 0;

  m_fResiduePercent            = 0.02F;

  try
  {
    m_iMessagePort = m_cmdLine.ToInt( "-messagePort" );
  }
  catch ( ex::VException )
  {
    m_iMessagePort = 8021;
  }

  try
  {
    m_sMessageHost = m_cmdLine.ToString( "-messageHost" );
  }
  catch ( ex::VException )
  {
    m_sMessageHost = "localhost";
  }

} // constructor


/**
 * Execute the pipeline from beginning to end.
 */
void vxPipeline::Run()
{ 
  std::string sourceFilePath (m_cmdLine.ToString( "-sourceFilePath" ));
  std::string destFilePath   (m_cmdLine.ToString( "-destFilePath"   ));

	if( sourceFilePath.size() == 0 )
	{
		LogErr("Miss source file path", "vxPipeline.C", "Run");
		m_iErrorCode = 1;
		return;
	}
  else if( destFilePath.size() == 0 )
	{
		destFilePath = sourceFilePath;
		LogErr("Miss destination file path", "vxPipeline.C", "Run");
	} // else if

  // write out processed series version
	// version 5.1: Adaptive to the Console 3.0, drop .slv and _1.csi files from built results
	//              The general patient natural body coordinate system starts to be used
	// version 6.0: The new .uld (uncleased SLD volume) file is generated. 
	// version 7.0: Upgrade to Visual Studio 2010 and built as 64 bits application
  vx::File file( destFilePath );
  const std::string sProcessedSeriesVersion = "7.0";
  SeriesVersion sv( sProcessedSeriesVersion );
  sv.ToFile( file.GetParent() + "\\version.xml" );

  stringstream tt;
  tt << "running with switches == \n"<< m_cmdLine;
  LogFyi(tt.str(),"vxPipeline","Run" );  tt.str("");
  
  tt << "V3D-Colon Processor " << sProcessedSeriesVersion;
  LogFyi( tt.str(), "vxPipeline ", "Run" );  tt.str("");  
  
  Timer timer, ttt;
  std::string stage(m_cmdLine.ToString("-stage"));

  // check if volume files is available or not
  // acquire info on volHeader version, body position, modality and voxel type
  { 
    File * pVolFileCheck = new File(sourceFilePath+".vol");
    if( pVolFileCheck->Exists() )
    {
      delete pVolFileCheck;
    
      // get header
	    vxVolumeHeader *ph = new vxVolumeHeader;
		  ph->Read( sourceFilePath+".vol" );

			/// check voxel type, the UINT2 is expected as default
	    if( ph->GetVoxelTypeEnum() == vxVolumeHeader::INT2 )
		  {
				LogErr("It is not UINT2 volume.", "vxPipeline.C", "Run");
				m_iErrorCode = 1;
				return;
			}

			/// get modality
	    ph->GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum::DataModality, m_sModality);

			/// get body position
			ph->GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum::PatientPosition, m_sBodyPosition);
			
      delete ph;
    }
    else
    {
      delete pVolFileCheck;
      LogErr("The volume file cannot be found", "vxPipeline.C", "Run");
			m_iErrorCode = 1;
      return;
    } // if-else

    if( !(m_sModality == "CT") )
    {
      LogErr("Image modality is not supported", "vxPipeline.C", "Run");
			m_iErrorCode = 1;
      return;
    }
  } // end of checking if volume files is available or not

  // get the system level settings, which is independent to vol
  // get the system level settings  
  { // read in parameter set from app.config file
		// singleton pattern
		static std::shared_ptr< VCSettings > settings;
		if ( settings == nullptr )
		{
			settings = VCSettings::Create(); 
		}

    m_bCRESOn         = settings->GetBoolValue  (VCSettings::ProcessorXML(), VCSettings::ElectronicCleanse());
	  m_fResiduePercent = settings->GetFloatValue (VCSettings::ProcessorXML(), VCSettings::ResiduePercent());

  } // end reading in parameter set from app.config file

  if( stage == "all" )
  { // apply cleansing 
    float8 eTime = m_servCmdLine.ToDouble("-eTime");
    timer.SetElapsedTime(eTime);
    
		m_iErrorCode = RunAllStages( timer, sourceFilePath, destFilePath );
  } 
  else if( stage == "allnoec" )
  { // cleansing is not applied
		return;
    //float8 eTime = m_servCmdLine.ToDouble("-eTime");
    //timer.SetElapsedTime(eTime);
    //if( m_sModality == "CT" )
    //{
	   // m_bCRESOn = false;
    //  m_iErrorCode = RunAllStages( timer, sourceFilePath, destFilePath );
    //}
    //else if( (m_sModality == "MR")||(m_sModality == "MRI") ) 
    //{
    //  m_iErrorCode = RunAllStagesMR( timer, sourceFilePath, destFilePath );
    //}
  } 
  else
  {
    LogErr("The processing option is not supported", "vxPipeline.C", "Run");
		m_iErrorCode = 1;
  }
		
  if( m_iErrorCode == 0 )
  {
	  std::cout << std::endl << "Colon Processor Successfuly End within total " << ttt << std::endl;
  }
  else
  {
	  std::cout << std::endl << "Colon Processor terminated abnormally within total " << ttt << std::endl;
  }
} // Run


/**
 * The Complete vxPipeline 
 * 
 * @param timer           The timer that was started at the beginning of the Dicom stage.
 * @param sourceFilePath  The path of the input volume file.
 * @param destFilePath    The path for the output files.
 * @return error message code number .
 */
int4 vxPipeline::RunAllStages( Timer & timer, const std::string & sourceFilePath, const std::string & destFilePath )
{  
  Timer stageTime;
  LogFyi( "    Starting Complete VC Pipeline.", "vxPipeline", "RunAllStages" );
  stringstream tt;
    
  tt << "    Starting - Reading input volume " + sourceFilePath + ".vol..."; 
  LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");
  SendState( "RV", 1, 1 );

	try
	{
		/// set a pipeline instance
		vxViewerPipeline pipeline;

		/// alocat memory for the input intensity volume
		vxBlockVolume<uint2> *pv = new vxBlockVolume<uint2>;

		//============= Step 1: Reading volulme =========================
		std::string sFilename( sourceFilePath + ".vol" );
		pipeline.ReadVolume( pv, sFilename );

		pipeline.ComputeImageOrientation();

		// Load the volume header information
		const vxVolumeHeader volumeHeader = pv->GetHeader();
		const Triple<float4> vUnits       = volumeHeader.GetVoxelUnits();
		const Triple<int4>   vDim         = volumeHeader.GetVolDim();

		tt << "    Finished - Reading input volume " + sourceFilePath + ".vol. Elapsed load " << stageTime; stageTime.Reset(); 
		LogFyi( tt.str(), "vxPipeline", "RunAllStages" ); tt.str("");
		SendState( "RV", 100, 5 );


		// ========= Step 2: make an linear volume =================
		tt << "    Starting - Prepare linear volume..."; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");
		SendState( "PLV", 1, 6 );

		const int4 iX( vDim.m_x );
		const int4 iY( vDim.m_y );
		const int4 iZ( vDim.m_z );
		const int4 iS( iX*iY );
		const int4 iN( iS*iZ );

    if( (vUnits.m_z > 1.5)||(vUnits.m_y > 1.0)||(vUnits.m_x > 1.0) )
		{ /// provide warning 
			LogWrn("    Slice thickness is greater than nomal.", "VCpipeline.C", "RunAllStages" );
		}

		/// alocate memory for the working linear volume
		uint2 * plv = new uint2[iN];
		if( plv == NULL )
		{
			throw ex::OutOfMemoryException( LogRec("    Out of memory", "VCpipeline.C", "RunAllStages") );
			return(-1);
		} // if

		pipeline.PrepareLinearVolume( pv, plv );

		if( !pipeline.CheckIntensityRange( plv ) )
		{ /// This may be caused by error background removal, imaging noise, or inproper FOV setup
		  LogErr( "    Abnormal intensity range.", "VCpipeline.C", "RunAllStages" );
			return(-1);
		} // if

		SendState( "PLV", 100, 10 );
		tt << "    Finished - Prepare linear volume " << stageTime; stageTime.Reset(); 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  


		// ========= Step 3: extracting ROI ===========
		tt << "    Starting - Extracting body region"; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");
		SendState( "EROI", 1, 11 );

		uint1 * pMask = new uint1[iN];
		if( pMask == NULL )
		{
			throw ex::AbortLoadException( LogRec("    Fail to allocate memory for mask volume", "VCpipeline.C", "RunAllStages") );
			return(-1);
		}

		pipeline.ComputeBodyRegion( plv, pMask );

		pipeline.CountTaggedVoxel( plv, pMask );

		pipeline.RemoveBone( plv, pMask );

		SendState( "EROI", 100, 20 );
		tt << "    Finished - Extracting body region" << stageTime; stageTime.Reset(); 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  


		// ========= Step 4: apply electronic cleansing ==========================
		tt << "    Starting - Electronic cleansing"; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");
		SendState( "CRES", 1, 21 );

		pipeline.ComputeCRES( plv, pMask );

		pipeline.ReformatBack2NewLayout( plv, pMask );

		SendState( "CRES", 100, 40 );
		tt << "    Finished - Electronic cleansing" << stageTime; stageTime.Reset(); 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  

		// ========= Step 5: compute inside colon segments ==========================
		tt << "    Starting - Electronic cleansing"; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");
		SendState( "EROI", 1, 41 );

		uint1 *pIns = new uint1[iN];
		if( pIns == NULL )
		{
			throw ex::OutOfMemoryException( LogRec("    Out of memory", "VCpipeline.C", "RunAllStages") );
		} // if

		pipeline.ComputeInsideSegments( plv, pMask, pIns );

		WriteOutCDVvols( pv, plv, pIns, pipeline.GetVolumeHeader(), pipeline.GetImageOrientation(), destFilePath );
		delete plv;

		SendState( "EROI", 100, 50 );
		tt << "   Finished - Electronic cleansing" << stageTime; stageTime.Reset(); 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  


		// ========= Step 6: Compute SLD volumes and TRI =============================
		tt << "    Starting - Compute SLD and TRI"; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");
		SendState( "SLD", 1, 51 );

		uint1 *pInsNoec = new uint1[iN];
		if( pInsNoec == NULL )
		{
			throw ex::OutOfMemoryException( LogRec("    Out of memory", "VCpipeline.C", "RunAllStages") );
		} // if

		pipeline.ComputeUncleansedINS( pv, pIns, pInsNoec );

    LinVolUC sldInterior;
		sldInterior.Resize  ( vDim   );
		sldInterior.SetUnits( vUnits );

    LinVolUC sldExterior;
		sldExterior.Resize  ( vDim   );
		sldExterior.SetUnits( vUnits );

		std::string ss = destFilePath;
		pipeline.ComputeSLDandTRI( pMask, pIns, pInsNoec, sldInterior.GetDataPtr(), sldExterior.GetDataPtr(), ss );

		vxBlockVolume<uint1> insVol( pipeline.GetVolumeHeader() );
		WriteOutINSvol( pIns, insVol, destFilePath );

		delete pInsNoec;
		delete pMask;
		delete pIns;

		sldInterior.Write( destFilePath + ".sld", false );
		sldExterior.Write( destFilePath + ".uld", false );

		SendState( "SLD", 100, 70 );
		tt << "    Finished - Compute SLD and TRI" << stageTime; stageTime.Reset(); 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  


		// ========= Step 7: Compute PDF and CSI =============================
		tt << "    Starting - Compute PDF and CSI"; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");
		SendState( "SLD", 1, 71 );

		vxBlockVolume<float4> dfs( vxVolumeHeader(vDim, vUnits, 32, 0.0) );
		vxBlockVolume<float4> dfe( vxVolumeHeader(vDim, vUnits, 32, 0.0) );
		vxBlockVolume<uint2>  csi( vxVolumeHeader(vDim, vUnits, 16,   0) );

		pipeline.ComputePDFandCSI( &insVol, sldInterior.GetDataPtr(), &dfs, &dfe, &csi, ss );

	  dfs.Write( destFilePath + "_0.pdr" );
		dfe.Write( destFilePath + "_1.pdr" );
		csi.Write( destFilePath + "_0.csi" );

		SendState( "SLD", 100, 100 );
		tt << "    Finished - Compute PDF and CSI" << stageTime; stageTime.Reset(); 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  

		tt << "    VCpipeline completed. Overall elapsed " << timer; 
		LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");           

	}
	catch( ... )
	{
		throw ex::AbortDataProcessingException( LogRec("Expection thrown.", "RunAllStages", "vxPipeline.C") );
		return(-1);
	}
	
	m_iErrorCode = 0;
  return(0);
} // RunAllStages


/** write out the CDV volume
 *  
 *  @param pv the pointer to the input block volume
 *  @param plv the pointer to the linear working volume
 *  @param pIns the pointer to the linear INS volume
 *  @param vHeader the working copy of the volume header
 *  @param sImageOrientation
 *  @param destFilePath
 */
void vxPipeline::WriteOutCDVvols( vxBlockVolume<uint2> * pv, uint2 * plv, uint1 * pIns, vxVolumeHeader & vHeader, 
		const std::string & sImageOrientation, const std::string & destFilePath )
{
	vxBlockVolumeIterator<uint2> itv(*pv); 

	/// both intensity volume and CDV should have the identical Bias value. That was determined by the processor of V3D-Colon
 	int4 iBias(1000);
  int4 iMax(0);
  // get iMax
  std::string sBias;
  pv->GetHeader().GetHeaderEntry( vxVolumeHeader::Bias, sBias );
  iBias = atoi( sBias.c_str() );

  iMax = vxIntensityConverter::GetMaxValue( pv->GetHeader() );      
  if( iMax <= 0 )
    throw ex::AbortLoadException( LogRec("    Incorrect maximum value", "Fields_ext", "CreateVolumeFields") );  

	const float4 fMax (static_cast<float4>(iMax));
  
  /// 8 bits DIF volume
	vxBlockVolume<uint1> vb8(vxVolumeHeader(vHeader.GetVolDim(), vHeader.GetVoxelUnits(), 8, 0));

	vb8.GetHeader().SetVariableVolumeHeader( vHeader.GetVariableVolumeHeader() );
	vxBlockVolumeIterator<uint1> it8d( vb8 );

  // get EC indicator in the header of CDV
  int4 indicator(0);
  vHeader.GetVariableVolumeHeader().GetHdrVar("IsEcleansed", indicator);

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

							if( ic != 0 )
							{ // this is a cleansed voxel
								if( iv < 2000 )
									it8d.SetVoxel(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									it8d.SetVoxel(255);
							}	// if-else	      							
						} // for x
						t = t+iX;
					} // for y
					s = s+iS;
				} // for(z)
	    } // try
	    catch( ... )
	    {		
        throw ex::AbortLoadException(LogRec( "    Fail to generate 8bits ECV and DIF for bias = 1000", "Fields_ext", "CreateVolumeFields" ));  
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

							if( plv[n] != 0 )
							{
								// this is a voxel in the cleansed area
								if( iv < 2000 )
									it8d.SetVoxel(static_cast<uint1>(f*static_cast<float4>(iv)));
								else
									it8d.SetVoxel(255);
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

  // Check the ecv-vol difference file (.dif extension)
	vb8.Write( destFilePath + "_8bit.cdv" );
 
  try
  {
		vxBlockVolume<uint2> cdv( vHeader );

		const uint2 u = vxIntensityConverter::ModalityToRaw( -1000, vHeader );

    vxBlockVolumeIterator< uint2 > cleansedIter( cdv );

		register int4 i, j, k;
		i = 0;
		for( z=0; z<iZ; z++ )
		{
			j = i;
			for( y=0; y<iY; y++ )
			{
				k = j;
				for( x=0; x<iX; x++ )
				{
					if( plv[k] > u )
					{
            cleansedIter.SetPos(x,y,z);
            cleansedIter.SetVoxel( plv[k] );
					} // if

					k++;
				} // x

				j = j + iX;
			} // for y

			i = i + iS;
		} // for z

    cdv.GetHeader().SetSignificantBitsPerVoxel(12);
		cdv.Write( destFilePath + ".cdv" );
  } // try
  catch( ... )
  {    
    throw ex::AbortDataProcessingException( LogRec("    Fail to create CSV", "vxPipeline.C", "WriteOutCDVvols") );
  } // catch( ... )  

} // WriteOutCDVvols


/** write out the INS volume
 *
 *  @param pIns the pointer to the linear INS volume
 *  @param insVol the block INS volume
 *  @param destFilePath
 */
void vxPipeline::WriteOutINSvol( uint1 * pIns, vxBlockVolume<uint1> & insVol, const std::string & destFilePath  )
{
	/// copy INS volume
	vxBlockVolumeIterator< uint1 > iti( insVol );

	const int4 iX(insVol.GetHeader().GetVolDim().m_x);
	const int4 iY(insVol.GetHeader().GetVolDim().m_y);
	const int4 iZ(insVol.GetHeader().GetVolDim().m_z);
	const int4 iS(iX*iY);

	register int4 x, y, z, n, t, s = 0;

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
	    
	insVol.Write( destFilePath + ".ins" );
} // WriteOutINSvol


/**
 * Sends State Information to message handler
 */
 void vxPipeline::SendState(const std::string & sStage, const int4 iStateProgress, const int4 iOverallProgress )
 {
   V2kMonitorClient::SendState( m_iMessagePort, 
                                m_sMessageHost, 
                                "Pipeline", 
                                sStage,
                                iStateProgress, 
                                iOverallProgress);

 }  // SendState()


/**
 * Sends Information to message handler
 */
 void vxPipeline::SendInformation(const std::string & sStage, const std::string & sMessage )
 {
   V2kMonitorClient::SendInformation( m_iMessagePort, 
                                      m_sMessageHost, 
                                      "Pipeline", 
                                      sStage, 
                                      sMessage);
 }  // SendInformation()


 /** 
  *  This is for executing another application within the pipeline
  *
  *  @param sCommandLine: Sample command line:
  */
 bool vxPipeline::LaunchProcessAndWait( const std::string & sCommandLine )
 {
	 STARTUPINFO si;

	 memset( &si, 0, sizeof( si ) );

	 si.cb = sizeof(si);

	 PROCESS_INFORMATION pi;

	 memset( &pi, 0, sizeof( pi ) );

	 char * s = const_cast< char * >( sCommandLine.c_str() );

	 if( !CreateProcess( NULL, s, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) ) 
		 return false;

	 // wait until child process exits

	 WaitForSingleObject( pi.hProcess, INFINITE );
	 DWORD dwExitCode( 0 );
	 GetExitCodeProcess( pi.hProcess, &dwExitCode );

	 // close process and thread handles 
	 CloseHandle( pi.hProcess );
	 CloseHandle( pi.hThread );

	 return dwExitCode == 0 ? true : false;
 } // LaunchProcessAndWait



#undef FILE_REVISION
  
// $Log: vxPipeline.C,v $
// Revision 1.0 2011/06/21  dongqing
//
// $Header: Viatronix.v3D.Processor.Plugins.Colon/vxPipeline.C,v 1.0 2011/06/21 dongqing Exp $
// $Id: vxPipeline.C,v 1.0 2011/06/21 dongqing Exp $
