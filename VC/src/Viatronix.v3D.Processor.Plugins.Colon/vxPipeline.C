// $Id: vxPipeline.C,v 1.37.2.4 2010/02/12 20:38:33 dongqing Exp $
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
#include "Timer.h"
#include "Date.h"
#include "VCSettings.h"
#include "File.h"
#include "V2kVersion.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxVolumeHeader.h"
#include "vxBlockVolumeFileReader.h"
#include "vxIntensityConverter.h"
#include "Logger.h"
#include "SystemInfo.h"
#include "SeriesVersion.h"
#include "ReaderGlobal.h"

using namespace std;
using namespace hdr;
using namespace vx;


#define FILE_REVISION "$Revision: 1.37.2.4 $"

/**
 * Constructor.
 * 
 * @param cmdLine Command line argument count
 */
vxPipeline::vxPipeline(CmdLineSwitches cmdLine) : m_cmdLine(cmdLine), m_servCmdLine(cmdLine)
{
  m_bComputeIcons              = false;

  m_sVoxelType                 = "uint2";
  m_iSliceBoundForCompressed3D = 481; // This is selected based on 48 cm general abdomanal length.

  m_bCRESOn                    = true;
  m_bHighQuality2DOn           = true;
  m_bCompressed3DOn            = false;
  m_bApplyInterpolation        = false;

  m_sModality                  = "UNKNOWN";
  m_bOriginalVolExist          = true;

	m_iErrorCode                 = 0;
  m_bIsNewVolCoordinateFormat  = true;

  m_fResiduePercent            = 0.02F;

  // =======================================
  // Attempt to set up the messaging manager
  // =======================================
  try
  {

    m_pManager = messaging::MessagingManager::Create( m_cmdLine.ToString("-messageHost"), m_cmdLine.ToInt("-messagePort") );

    LogFyi(util::Strings::Format("Messaging manager created, messages will be sent to (%s,%d)", m_pManager->GetServer().c_str(), m_pManager->GetPort() ), "vxPipeline", "vxPipeline" );

    // =============================================
    // Create the message to be sent to work queue
    // =============================================
    try
    {
      m_pMessage = messaging::ProcessingMessage::Create(m_cmdLine.ToString("-job"), "pipeline" );
    }
    catch ( ex::VException )
    {
      m_pMessage = __nullptr;
    }


  }
  catch ( ex::VException )
  {
    LogFyi("Messaging manager could not be created, no host and/or port specified", "vxPipeline", "vxPipeline" );
    m_pManager = __nullptr;
  }


} // constructor


/**
 * Execute the pipeline from beginning to end.
 */
void vxPipeline::Run()
{ 
#ifdef _WIN32
  SetThreadPriority(GetCurrentThread (),THREAD_PRIORITY_BELOW_NORMAL);
  SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif

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
  
  tt << "V3D-Colon Processor " << V2K_REVISION;
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
		    m_sVoxelType = "int2";

			/// get modality
	    ph->GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum::DataModality, m_sModality);

			/// get body position
			ph->GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum::PatientPosition, m_sBodyPosition);

	    VarMap map(ph->GetVariableVolumeHeader());
			int iVersionNum(0);
	    map.GetHdrVar( "version", iVersionNum );
			
			if( iVersionNum < 4 )
				m_bIsNewVolCoordinateFormat = false;

      delete ph;
    }
    else
    {
      delete pVolFileCheck;
      LogErr("The volume file cannot be found", "vxPipeline.C", "Run");
			m_iErrorCode = 1;
      return;
    } // if-else

    if( !((m_sModality == "CT")||(m_sModality == "MR")||(m_sModality == "MRI")) )
    {
      LogErr("Image modality is not supported", "vxPipeline.C", "Run");
			m_iErrorCode = 1;
      return;
    }
    else if( ((m_sModality == "MR")||(m_sModality == "MRI"))&&(m_sVoxelType == "int2") )
    {
      LogErr("Voxel type is not UINT2 for MRIVC", "vxPipeline.C", "Run");
			m_iErrorCode = 1;
      return;
    }
  } // end of checking if volume files is available or not

  //// get the system level settings, which is independent to vol
  //CoInitialize(NULL);
  //// get the system level settings  
  //{ // get the path of the VCSetting.XML
  //  char sExePath[ _MAX_PATH ];
  //  GetModuleFileName( NULL, sExePath, _MAX_PATH );

  //  // get the drive letter and directory of executable
  //  char sDrive[_MAX_DRIVE];
  //  char sDirectory[_MAX_DIR ];
  //  _splitpath( sExePath, sDrive, sDirectory, NULL, NULL );

  //  // format the image path
  //  char sSettingsPath[ _MAX_PATH];
  //  wsprintf( sSettingsPath, "%s%s%s", sDrive, sDirectory, "config\\VCSettings.xml" );

  //  VCSettings systemLevelSettingInfo( sSettingsPath );
  //  m_bCRESOn = systemLevelSettingInfo.GetBoolValue(VCSettings::ProcessorXML(), VCSettings::ElectronicCleanse());
  //  if( systemLevelSettingInfo.GetIntValue(VCSettings::ProcessorXML(), VCSettings::DisplayQuality2D()) <= 8 )
	 //   m_bHighQuality2DOn = false;
	 // m_bApplyInterpolation        = systemLevelSettingInfo.GetBoolValue(VCSettings::ProcessorXML(), VCSettings::ApplyInterpolation());
	 // m_bCompressed3DOn            = systemLevelSettingInfo.GetBoolValue(VCSettings::ProcessorXML(), VCSettings::Compressed3DOn());
	 // m_iSliceBoundForCompressed3D = systemLevelSettingInfo.GetIntValue (VCSettings::ProcessorXML(), VCSettings::Compressed3DSliceBound());
	 // m_fResiduePercent            = systemLevelSettingInfo.GetFloatValue (VCSettings::ProcessorXML(), VCSettings::ResiduePercent());
  //  // if value is not availabel, use the default value
	 // if( m_iSliceBoundForCompressed3D < 0 )
	 //   m_iSliceBoundForCompressed3D = 481;
  //}
  //CoUninitialize();


  // get the system level settings, which is independent to vol
  // get the system level settings  
  { // read in parameter set from app.config file
		// singleton pattern
    const VCSettings & settings = VCSettings::GetInstance();

    m_bCRESOn = settings.GetBoolValue(VCSettings::ProcessorXML(), VCSettings::ElectronicCleanse());

		if( settings.GetIntValue(VCSettings::ProcessorXML(), VCSettings::DisplayQuality2D()) <= 8 )
	    m_bHighQuality2DOn = false;
	  m_bApplyInterpolation        = settings.GetBoolValue  (VCSettings::ProcessorXML(), VCSettings::ApplyInterpolation());
	  m_bCompressed3DOn            = settings.GetBoolValue  (VCSettings::ProcessorXML(), VCSettings::Compressed3DOn());
	  m_iSliceBoundForCompressed3D = settings.GetIntValue   (VCSettings::ProcessorXML(), VCSettings::Compressed3DSliceBound());
	  m_fResiduePercent            = settings.GetFloatValue (VCSettings::ProcessorXML(), VCSettings::ResiduePercent());
    // if value is not availabel, use the default value
	  if( m_iSliceBoundForCompressed3D < 0 )
	    m_iSliceBoundForCompressed3D = 481;
  } // end reading in parameter set from app.config file

  if( stage == "all" )
  { // apply cleansing 
    float8 eTime = m_servCmdLine.ToDouble("-eTime");
    timer.SetElapsedTime(eTime);
    if( m_sModality == "CT" )
    {
	    m_iErrorCode = RunAllStages( timer, sourceFilePath, destFilePath );
    }
    else
    {
      m_iErrorCode = RunAllStagesMR( timer, sourceFilePath, destFilePath );
    }
  } 
  else if( stage == "allnoec" )
  { // cleansing is not applied
    float8 eTime = m_servCmdLine.ToDouble("-eTime");
    timer.SetElapsedTime(eTime);
    if( m_sModality == "CT" )
    {
	    m_bCRESOn = false;
      m_iErrorCode = RunAllStages( timer, sourceFilePath, destFilePath );
    }
    else if( (m_sModality == "MR")||(m_sModality == "MRI") ) 
    {
      m_iErrorCode = RunAllStagesMR( timer, sourceFilePath, destFilePath );
    }
  } 
	else if( stage == "anonym" )
	{ // This is for anonymize volume file. Only for internal use, not for release use
		vxBlockVolume<uint2> vol;
		vol.Read(sourceFilePath+".vol");

		vxVolumeHeader ph( vol.GetHeader() );

		VarMap map( ph.GetVariableVolumeHeader() );

		std::string sName         ("MDCT64S");
		std::string sInstituteName("Viatronix");
		std::string sPhysicianName("Radiologist");
		std::string sPatientID    ("360884672");
		std::string sStudyDate    ("20060808");
		std::string sPatientAge   ("100");

		map.SetHdrVar("patientName",     sName,          "Patient Name"               );
		map.SetHdrVar("institutionName", sInstituteName, "ID Institution"             );
		map.SetHdrVar("physicianName",   sPhysicianName, "ID Referring physician name");
		map.SetHdrVar("patientID",       sPatientID,     "Patient ID/Medical ID"      );
		map.SetHdrVar("studyDate",       sStudyDate,     "Study Date"                 );
		map.SetHdrVar("patientAge",      sPatientAge,    "Age of the patient"         );

		ph.SetVariableVolumeHeader( map );
		vol.SetHeader( ph );

		vol.Write(sourceFilePath+"_n.vol");
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
int4 vxPipeline::RunAllStages( Timer & timer, const string & sourceFilePath, const string & destFilePath )
{  
	/// This is only for beta version
	bool bTurnOffDebugger(true);
	//bool bTurnOffDebugger(false);

  //Hazards::m_hazardList.clear();
  Timer stageTime;
  LogFyi( "Starting Complete VC Pipeline.", "vxPipeline", "RunAllStages" );
  stringstream tt;
    
  tt << "Starting - Handling input volume " + sourceFilePath + ".vol..."; 
  LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");
  SendState( 1 );

  //===================================================================================
  vxBlockVolume<uint2> *pVol = new vxBlockVolume<uint2>;
  // Read in the input volume
  // If it is vxBlockVolume in UINT2 voxel type, shift intensity based on the Bias value
  // If it is LinVol, create a UINT2 vxBlockVolume and write it out to overwrite 
  // the original vol file on disk
  if( !HandleInputVol(pVol, sourceFilePath) )
  {
	  SendInformation( "AN ERROR occurred IN HANDLING VOLUME"  );
	  LogErr( "Handling the input volume fail", "vxPipeline", "RunAllStages" );
    return(1);
  }

  // check if interpolation is necessary
  if( m_bApplyInterpolation )
  {
	  LogFyi( "Apply interpolation", "vxPipeline.C", "RunAllStages" );
	  if( !InterpolationForImatronCase(pVol, sourceFilePath) )
	  {
	    SendInformation( "AN ERROR occurred IN INTERPOLATING VOLUME" );
	    LogErr( "Fail interpolation", "vxPipeline.C", "RunAllStages" );
	    return(1);
	  }
  } // end of interpolation
  tt << "Finished - Handling input volume " + sourceFilePath + ".vol. Elapsed load " << stageTime; stageTime.Reset(); 
  LogFyi( tt.str(), "vxPipeline", "RunAllStages" ); tt.str("");
  SendState( 20 );
  
  Triple<int2>   dim  ( pVol->GetHeader().GetVolDim() );
  Triple<float4> units( pVol->GetHeader().GetVoxelUnits() );

  // Condition is unneeded since the Hazards statement was commeneted anyway
  //if( (units.m_z > 1.5)||(units.m_y > 1.0)||(units.m_x > 1.0) )
  // ;//Hazards::AddHazard(Hazards::THICK_SLICE_INTERVAL);

  //===================================================================================
	//if( !bTurnOffDebugger )
	//{
	//	tt << "Starting - THR stage..."; LogFyi(tt.str(),"vxPipeline","RunAllStages");tt.str(""); 
	//	// This stage must be got rid of. The thresholds should be stored in the Viewer! Dongqing 02/18/2006
	//	SendState( "THR", 1, 20 );
	//	ComputeThresholds( destFilePath );
	//	SendState( "THR", 100, 30 );
	//	tt << "Finished - THR stage. Elapsed stage " << stageTime; stageTime.Reset(); 
	//	LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");
	//} // This code block is no longer used in the release codes! Dongqing 10/22/2007

	//===================================================================================
  tt << "Starting - CLA stage..."; 
  LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); 
  tt.str("");
  SendState(   30, "CLA" );
  // this mask will store labels for each cluster of ROI found by segmentation
  vxBlockVolume<uint1> *pMask = new vxBlockVolume<uint1>(vxVolumeHeader(dim, units, 8, 0));
  if( pMask == NULL )
  {
    LogErr("Fail to initialize pMask", "RunAllStages", "vxPipeline.C");
	  return(1);
  }
  if( !ComputeBodyRegion(pVol, pMask, destFilePath) )
  {
    LogErr("Fail body segmentation", "RunAllStages", "vxPipeline.C");
    return(1);
  }
  tt << "Finished - CLA stage. Elapsed stage " << stageTime; stageTime.Reset(); 
  LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  

  //===================================================================================
  tt << "Starting - CRES stage..."; LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");
  SendState(  40 , "CRES" );
  if( m_bCRESOn )
  { // colonic residue electronic subtraction stage
    if( !ComputeCRES(pVol, pMask, sourceFilePath, destFilePath) )
    {
	    LogErr("Error occurs in the CRES stage", "vxPipeline.C", "RunAllStages");
	    return(1);
    }
  } // if
  else
  {
    LogFyi("Skip CRES stage", "vxPipeline.C", "RunAllStages");
  } // else
  tt << "Finished - CRES stage. Elapsed stage " << stageTime; stageTime.Reset(); 
  LogFyi(tt.str(),"vxPipeline","RunAllStages"); tt.str("");

  //===================================================================================
  tt << "Starting - INS stage..."; LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");    
  SendState( 50, "INS" );
  vxBlockVolume<uint1> *pIns = new vxBlockVolume<uint1>(vxVolumeHeader(dim, units, 8*sizeof(uint1), 0));
  if( pIns == NULL )
  {
    LogErr("Fail to initialize pIns", "RunAllStages", "vxPipeline.C");
	  return(1);
  }    
  if( !ComputeInsideSegments(pVol, pMask, pIns, sourceFilePath, destFilePath) )
  {
    LogErr("Fail to compute INS volume", "RunAllStages", "vxPipeline.C");
	  return(1);
  }
	//$$ reformat the old layout to new for output
	ReformatVolFromOldToNewLayout( m_sBodyPosition, *pVol );
	ReformatVolFromOldToNewLayout( m_sBodyPosition, *pMask );
  pVol->Write(destFilePath+".cdv");

  ////===========////////==============//////////////==========////////////==========////////////////
  //delete pMask;         // release memory

	//$$ reformat the old layout to new for output
	ReformatVolFromOldToNewLayout( m_sBodyPosition, *pIns );
	pIns->Write(destFilePath+".ins");

  vxBlockVolume<uint1> *pInsNoCRES = new vxBlockVolume<uint1>(vxVolumeHeader(dim, units, 8*sizeof(uint1), 0));
	if( m_bCRESOn )
	{ /// creat the INS without CRES
    std::pair<float4, float4> ff(0.0, 0.0);
		// read in the non-cleansed original volume
		vxBlockVolumeFileReader<uint2>::Read( sourceFilePath+".vol", *pVol, ff );

		vxBlockVolumeIterator<uint2> itv(*pVol);
		vxBlockVolumeIterator<uint1> it (*pIns);
		vxBlockVolumeIterator<uint1> iti(*pInsNoCRES);

	  const uint2 uAirTh( vxIntensityConverter::ModalityToRaw(-760, pVol->GetHeader()) );   // hard-coded threshold
		for( it.SetPos(0,0,0); it.IsNotAtEnd(); it.NextBlockZYX() )
		{
			for( ; it.IsNotAtEndOfBlock(); it.NextZYXinsideBlock() )
			{
				if( it.GetVoxel() > 0x00 )
				{
					itv.SetPos(it);
					if( itv.GetVoxel() < uAirTh )
					{
						iti.SetPos( it );
						iti.SetVoxel( it.GetVoxel() );
					} // if
				} // if
			} // for inside the block
		} // for all blocks

		pInsNoCRES->Compact();
	} // if m_bCRESOn
	delete pVol;          // release A LOT of memory
	
  tt << "Finished - INS stage. Elapsed stage " << stageTime; stageTime.Reset(); 
  LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");    

  //===================================================================================
  tt << "Starting - DFB stage..."; LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");       
  SendState(   55, "DFB" );
  vxBlockVolume<uint1> *pSld = new vxBlockVolume<uint1>(vxVolumeHeader(dim, units, 8, 0));
  vxBlockVolume<uint1> *pUld = new vxBlockVolume<uint1>(vxVolumeHeader(dim, units, 8, 0));
  if( pUld == NULL || pSld == NULL )
  {
    LogErr("Fail to initialize pSld", "RunAllStages", "vxPipeline.C");
	  return(1);
  }    
  if( !ComputeDFB( pInsNoCRES, pIns, pUld, pSld, sourceFilePath, destFilePath, bTurnOffDebugger ) )
  {
    LogErr("Fail to compute DFB volume", "RunAllStages", "vxPipeline.C");
    return(1);
  }
	if( m_bCRESOn )
	{
	  pUld->Write(destFilePath+".uld");
	  pSld->Write(destFilePath+".sld");
	}
	else
	{
    pSld->Write(destFilePath+".uld");
	}
	delete pInsNoCRES;
	delete pUld;
  tt << "Finished - DFB stage. Elapsed stage " << stageTime; stageTime.Reset(); 
  LogFyi(tt.str(), "vxPipeline.C", "RunAllStages"); tt.str("");       
    
  //===================================================================================
  tt << "Starting - TRI stage..."; LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");          
  SendState(   75, "TRI");
  if( !ComputeTriangles(pMask, sourceFilePath, destFilePath) )     /// this is for good overview looking
  //if( !ComputeTriangles(pIns, sourceFilePath, destFilePath) )
  {
    LogErr("Fail to compute TRI", "RunAllStages", "vxPipeline.C");
	  return(1);
  }
    
  delete pMask;         // release memory

  tt << "Finished - TRI stage. Elapsed stage " << stageTime; stageTime.Reset(); 
  LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");       

  //===================================================================================
  tt << "Starting - SKE stage..."; LogFyi(tt.str(),"vxPipeline","RunAllStages");tt.str("");       
  SendState(   85, "SKE");
  if( !ComputeSkeleton(pIns, pSld, sourceFilePath, destFilePath ) )
  {
    LogErr("Fail to compute SKI", "RunAllStages", "vxPipeline.C");
	  return(1);
  }
  delete pIns;
  delete pSld;

  //Hazards::Write(destFilePath);
  tt << "Finished - SKE stage. Elapsed stage " << stageTime; stageTime.Reset(); 
  //LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");       
  tt << "\n\nvxPipeline Completed. Overall elapsed " << timer; 
  LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");  

  /// We are done
  SendState(100);

  //===================================================================================
	//if( !bTurnOffDebugger )
	//{	// This code is only for beta testing and is not for release product!!!
	//	Mutex peakMemoryUsageMutex("C253F9D4-EEFE-49aa-AAC8-A3F756FD84E4");
 //   peakMemoryUsageMutex.Lock();
	//	try
	//	{
	//		if( !CreateOldFormatOutput(sourceFilePath, destFilePath) )
	//		{
	//		  peakMemoryUsageMutex.Unlock();
	//			LogErr("Fail to generate old format output", "vxPipeline", "RunAllStages");
	//			return(1);
	//		}
	//	} 
	//	catch( ... )
	//	{
	//		if( peakMemoryUsageMutex.IsLocked() )
 //       peakMemoryUsageMutex.Unlock();

	//		LogErr("Fail to generate old format output", "RunAllStages", "vxPipeline.C");
	//		return(1);
	//	}
	//} // if

  //tt << "Finished create old output format files. Elapsed stage " << stageTime; stageTime.Reset(); 
  //LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");       
  //tt << "\nvxPipeline Completed. Overall elapsed " << timer; 
  //LogFyi(tt.str(), "vxPipeline", "RunAllStages"); tt.str("");           

	m_iErrorCode = 0;
  return(0);
} // RunAllStages


/**
 * The Complete vxPipeline for MR images.
 * 
 * @param timer         The timer that was started at the beginning of the Dicom stage.
 * @param sourceFilePath the path for the input volumes
 * @param destFilePath the path for the output files
 * @return True iff the vxPipeline has executed successfully.
 */
int4 vxPipeline::RunAllStagesMR( Timer & timer, const string & sourceFilePath, const string & destFilePath )
{ 
  // after the volume coordinate system changed, the MRI pipeline has not got rewrite yet. Dongqing 3-6-07
  LogErr("This part of code has not been updated to the new volume coordinate system","","");
  return(1);
} // RunAllStagesMR


#undef FILE_REVISION
  
// $Log: vxPipeline.C,v $
// Revision 1.37.2.4  2010/02/12 20:38:33  dongqing
// comments out debug output
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.3  2008/09/23 15:12:00  dongqing
// the tagged fluid ratio should get using GetFloatValue rather than GetIntValue
//
// Revision 1.37.2.2  2007/10/24 14:58:17  dongqing
// add post INS dilation for recovering the lumen lose
// that should fix the skinny overview only. It should not
// affect anything else
//
// Revision 1.37.2.1  2007/08/30 12:53:57  dongqing
// make the residue percent threshold editable in the VCSetting.XML
//
// Revision 1.37  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.36  2006/10/13 13:56:23  dongqing
// add -stage annoym for vxBlockVolume annoymization
//
// Revision 1.35  2006/09/06 21:02:43  dongqing
// change the Series version from 5.2 to 6.0 for generation of ULD file
//
// Revision 1.34  2006/09/05 15:30:01  dongqing
// write out ULD for fast flying when "s" key on
//
// Revision 1.33  2006/08/31 20:57:57  dongqing
// made change to use new vxBlockVolume coordinate system
// That is the patient natural coordinate system
//
// Revision 1.32  2006/08/29 18:38:29  gchin
// Changes from Dongqing for Dr. Cadi
//
// Revision 1.31  2006/07/07 22:40:39  romy
// Turned off debug messages
//
// Revision 1.30  2006/06/23 14:21:58  dongqing
// turn on debugger again
//
// Revision 1.29  2006/06/22 19:28:26  dongqing
// add vxBlockVolume::Compact to save memory space
//
// Revision 1.28  2006/06/15 19:55:09  dongqing
// write out CDV for non cleansed situation
//
// Revision 1.27  2006/06/14 15:03:25  dongqing
// 2nd fine tune per Diane's validation
//
// Revision 1.26  2006/06/09 17:25:51  dongqing
// Old console has -sourceFilePath. Fix path check for old console.
//
// Revision 1.25  2006/06/09 13:14:47  dongqing
// make the processor compatible with old Console
//
// Revision 1.24  2006/06/09 12:35:39  dongqing
// add series version XML
//
// Revision 1.23  2006/06/08 20:08:27  dongqing
// add Processor Version XML file
//
// Revision 1.22  2006/06/06 20:14:02  dongqing
// fix fill cavity bug for the vxSegmentFinder
//
// Revision 1.21  2006/06/02 20:14:19  dongqing
// improve CRES quality per Diane's request.
//
// Revision 1.20  2006/05/30 13:13:45  dongqing
// switch to vxBlockVolumeReader
//
// Revision 1.19  2006/05/25 19:37:19  dongqing
// set the "-stage" default as "" other than "all"
//
// Revision 1.18  2006/05/25 19:23:55  dongqing
// set the "-stage" default as "" other than "all"
//
// Revision 1.17  2006/04/24 15:42:48  dongqing
// add time check for new output format
//
// Revision 1.16  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.15  2006/04/19 19:53:45  dongqing
// bug free beta version
//
// Revision 1.14  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.13  2006/04/06 15:08:57  dongqing
// rewrite the iterator
//
// Revision 1.12  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.11  2006/03/14 21:22:45  dongqing
// 1st step tune up
//
// Revision 1.10  2006/03/13 18:39:24  dongqing
// finialize SKE codes
//
// Revision 1.9  2006/03/10 16:39:05  dongqing
// replace the iterator for better speed
//
// Revision 1.8  2006/03/10 14:37:57  dongqing
// fix the bug in the initialization of a new vxBlockVolume
//
// Revision 1.7  2006/03/09 21:36:22  dongqing
// add EC switch check for writing out CDV
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxPipeline.C,v 1.37.2.4 2010/02/12 20:38:33 dongqing Exp $
// $Id: vxPipeline.C,v 1.37.2.4 2010/02/12 20:38:33 dongqing Exp $
