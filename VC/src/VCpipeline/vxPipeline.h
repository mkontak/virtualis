// $Id: vxPipeline.h,v 1.15.2.1 2007/08/30 12:53:57 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen  dongqing@viatornix.com
//
// Complete History at bottom of file.


#ifndef vxPipeline_h
#define vxPipeline_h

#include "Global.h"
#include "Thread.h"
#include "CmdLineSwitches.h"
#include "Volume.h"
#include "DatabaseConst.h"
#include "vxVolumeHeader.h"


class Timer;

/**
* The virtual colonoscopy backend processing Pipeline.
* 
* The Pipeline connects all automatic preprocessing stages of the Viatron 2000 together
* and hands the data from one stage to the other.
* The Pipeline further enables running the complete preprocessing as well as
* just individual steps to allow a technician to track down unusual behavior.
*/
class vxPipeline : public Thread
{
  // member functions
public:

  /// Constructor with commadline parameters                         
  vxPipeline(CmdLineSwitches cmdLine);
  
  /// run the Pipeline according to current commad line switches. 
  void Run();                                                           

  /// run the Pipeline after the dicom stage. Read, hand through and store all volume data.
  int4 RunAllStages( Timer & timer, const std::string & sourceFilePath, const std::string & destFilePath );           

  /// Send State Information
  void SendState( const std::string & sStage, const int4 iStateProgress, const int4 iOverallProgress );  

  /// Send Information
  void SendInformation( const std::string & sStage, const std::string & sMessage );

  /// This is for executing another application in the pipeline
  bool LaunchProcessAndWait( const std::string & sCommandLine );


public:

	/// write out the CDV volume
  void WriteOutCDVvols( vxBlockVolume<uint2> * pv, uint2 * plv, uint1 * pIns, vxVolumeHeader & vHeader, 
		const std::string & sImageOrientation, const std::string & destFilePath );

	/// write out the INS volume
	void WriteOutINSvol( uint1 * pIns, vxBlockVolume<uint1> & insVol, const std::string & destFilePath );

  /// Gets the Error Code
  int4 GetErrorCode() { return m_iErrorCode; }

  std::string & GetErrorMessage() { return m_sErrorMessage; }

  // member variables
private:

  /// Error Code
  int4 m_iErrorCode;

  /// Error Message
  std::string m_sErrorMessage;

  /// Message port
  int4 m_iMessagePort;

  /// Message
  std::string m_sMessageHost;

  // manages parsing and storing of command line parameters
  CmdLineSwitches m_cmdLine;
  CmdLineSwitches m_servCmdLine; // switches only known by Dicom stage

  // input voxel type 
  std::string m_sVoxelType;
	
  // system level setting switch
  bool m_bCRESOn;

  // image modality
  std::string m_sModality;

	/// series body position info containor
	std::string m_sBodyPosition;

  /// colon residues percent threshold for skip CRES
  float4 m_fResiduePercent;

}; // end of Pipeline

#endif // vxPipeline_h


// $Log: vxPipeline.h,v $
// Revision 1.0 2011/06/21  dongqing
// make the residue percent threshold editable in the VCSetting.XML
//
// $Header: Viatronix.v3D.Processor.Plugins.Colon/vxPipeline.h,v 1.0 2011/06/21 dongqing Exp $
// $Id: vxPipeline.h,v 1.0 2011/06/21 dongqing Exp $
