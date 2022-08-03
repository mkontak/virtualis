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

class Timer;
class Thresholds;

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

  /// run all stage for the MR images
  int4 RunAllStagesMR( Timer & timer, const std::string & sourceFilePath, const std::string & destFilePath );

  /// handling the input volume
  bool HandleInputVol( vxBlockVolume<uint2> * pVol, const std::string & sourceFilePath );

  /// interpolation of the volume for Imatron cases
  bool InterpolationForImatronCase( vxBlockVolume<uint2> * pVol, const std::string & sourceFilePath );

  /// Stage 1: compute volume data thresholds
  bool ComputeThresholds( const std::string & destFilePath );

  /// Stage 2: segment the volume into different classes
  bool ComputeBodyRegion( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask,
	                        const std::string & destFilePath );

  /// Stage 3: colonic residue electronic subtraction
  bool ComputeCRES( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask,
                     const std::string & sourceFilePath, const std::string & destFilePath );
	
  /// Stage 4: detect inside organ regions
  bool ComputeInsideSegments( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask,
	           vxBlockVolume<uint1> *pIns, const std::string & sourceFilePath, const std::string & destFilePath);

  /// Stage 6: compute distances from organ boundary
  bool ComputeDFB( vxBlockVolume<uint1> *pInsNoCRES, vxBlockVolume<uint1> *pIns, 
									 vxBlockVolume<uint1> *pUld, vxBlockVolume<uint1> *pSld, 
							const std::string & sourceFilePath, const std::string & destFilePath, const bool & bTurnOffDebugger );

  /// Stage 7: create organ overview triangulation
  bool ComputeTriangles( vxBlockVolume<uint1> * pIns, const std::string & sourceFilePath, 
                         const std::string & destFilePath );              

  /// Stage 8: compute the skeleton
  bool ComputeSkeleton( vxBlockVolume<uint1> * pIns, vxBlockVolume<uint1> * pSld, 
	                      const std::string & sourceFilePath, const std::string & destFilePath );

  /// Compute distance from boundary for MRI
  bool ComputeDFBMRI( BlockVolUC & insBVol, BlockVolUC & sldVol,
			                const std::string & destFilePath );

  /// Compute distance from boundary for a downsized volume.
  bool ComputeDFBSmallMRI( BlockVolUC & insBVol, const std::string & destFilePath );

  /// convertt prone volume into supine
  void FlipBodyPositionForMR( vxBlockVolume<uint2> & inVol, vxBlockVolume<uint2> & outVol );

  /// convertt prone volume into supine
  void FlipBodyPositionForMR( vxBlockVolume<uint1> & inVol, vxBlockVolume<uint1> & outVol );

  /// Send State Information
  void SendState(  const int4 iProgress, const std::string & sStage  = "" );  

  /// Send Information
  void SendInformation(  const std::string & sMessage, const std::string & sStage = "" );

  /// This is for executing another application in the pipeline
  bool LaunchProcessAndWait( const std::string & sCommandLine );

  // member functions
private:

  /// This is for beta version debugging
  bool CreateOldFormatOutput( const std::string & sourceFilePath, const std::string & destFilePath );

  /// reformat the volume from old to new layout
  void ReformatVolFromOldToNewLayout( const std::string & sBodyPosition,
	                                     vxBlockVolume<uint2> & vol );

  /// reformat the volume from old to new layout
  void ReformatVolFromOldToNewLayout( const std::string & sBodyPosition,
	                                     vxBlockVolume<uint1> & vol );

  /// reformat the volume from new to old layout
  void ReformatVolFromNewToOldLayout( const std::string & sBodyPosition,
	                                     vxBlockVolume<uint2> & vol );

  /// reformat the volume from new to old layout
  void ReformatVolFromNewToOldLayout( const std::string & sBodyPosition,
	                                     vxBlockVolume<uint1> & vol );

public:

  /// Gets the Error Code
  int4 GetErrorCode() { return m_iErrorCode; }

  std::string & GetErrorMessage() { return m_sErrorMessage; }

  // member variables
private:

  /// Error Code
  int4 m_iErrorCode;

  /// Error Message
  std::string m_sErrorMessage;

  // compute (or not compute) icons during Pipeline execution
  bool m_bComputeIcons;    

  // manages parsing and storing of command line parameters
  CmdLineSwitches m_cmdLine;
  CmdLineSwitches m_servCmdLine; // switches only known by Dicom stage

  // input voxel type 
  std::string m_sVoxelType;
	
  // slice bound for compressed 3D 
  int4 m_iSliceBoundForCompressed3D;

  // system level setting switch
  bool m_bCRESOn;
  bool m_bHighQuality2DOn;
  bool m_bCompressed3DOn;
  bool m_bApplyInterpolation;

  // image modality
  std::string m_sModality;

  // indicator of existance of the original volume
  bool m_bOriginalVolExist;

	/// volume header indicator
	bool m_bIsNewVolCoordinateFormat;

	/// series body position info containor
	std::string m_sBodyPosition;

  /// colon residues percent threshold for skip CRES
  float4 m_fResiduePercent;

  /// Messaging manager
  std::shared_ptr<messaging::MessagingManager> m_pManager;

  /// Message to be sent
  std::shared_ptr<messaging::ProcessingMessage> m_pMessage;

}; // end of Pipeline

#endif // vxPipeline_h


// $Log: vxPipeline.h,v $
// Revision 1.15.2.1  2007/08/30 12:53:57  dongqing
// make the residue percent threshold editable in the VCSetting.XML
//
// Revision 1.15  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.14  2006/09/05 15:29:54  dongqing
// write out ULD for fast flying when "s" key on
//
// Revision 1.13  2006/08/31 20:57:57  dongqing
// made change to use new vxBlockVolume coordinate system
// That is the patient natural coordinate system
//
// Revision 1.12  2006/06/22 19:28:26  dongqing
// add vxBlockVolume::Compact to save memory space
//
// Revision 1.11  2006/06/06 20:14:02  dongqing
// fix fill cavity bug for the vxSegmentFinder
//
// Revision 1.10  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.9  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.8  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.7  2006/03/14 21:22:45  dongqing
// 1st step tune up
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxPipeline.h,v 1.15.2.1 2007/08/30 12:53:57 dongqing Exp $
// $Id: vxPipeline.h,v 1.15.2.1 2007/08/30 12:53:57 dongqing Exp $
