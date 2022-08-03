/// $Id: vxCRES.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxCRES.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen, dongqing@viatronix.net
//
// Complete history at bottom of file.

/** 
*  This class is to apply colonic residue electronic subtraction to the fecal tagged studies
*/


#ifndef vxCRES_h
#define vxCRES_h


#include "vxBlockVolume.h"


class vxCRES
{
  // member functions
public :

  /// constructor
	vxCRES( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask, 
		      const std::string & sourceFilePath, const std::string destFilePath );

  /// destructor
  ~vxCRES();

  /// Apply colonic residue electronic subtraction procedure
  bool Run();

private :

  /// Determine air lumen
  void DelineatAirRegion();

  /// grwo out the air region 
  int4 GrowAirRegion( const Vector3D<int2> & viSeed, const uint1 & uROIlabel, const uint1 & uNewLabel, 
		                  const uint2 & uThreshold );

	/// Grow out 3D region with 6-connected 
  int4 Grow3D6NB( vxBlockVolume<uint1> * pMask, const Vector3D<int2> & viSeed, 
								  const uint1 & uROIlabel, const uint1 & uNewLabel );

	/// label the contrast enhanced residual regions
	void LabelTaggedRegions( std::vector<uint2> & vUBIntensityOfPuddle );
  
	/// grwo out the tagged region 
  int4 GrowTaggedRegion( const Vector3D<int2> & viSeed, const uint1 & uROIlabel, const uint1 & uNewLabel, 
		                     uint2 & uAverageIntensity );

  /// Determine air/contrast-enhanced boundary area
  void DetermineATB();

  /// probe analysis 
  bool ProbeAnalysis( const Vector3D<int2> & startPos, const Vector3D<int2> & directionStep, 
										  const int4 & iProbeLength, const uint1 & uNewLabel,
										  const uint2 & uStepTh, const uint2 & uTotalTh );

  /// reverse intensity for the contrast-enhanced residual region
  void IntensityReversion( std::vector<uint2> & vUBIntensityOfPuddle );

// member variables
private :

  /// volume
  vxBlockVolume<uint2> * m_pVol;

  /// temporary mask
  vxBlockVolume<uint1> * m_pMask;

  /// output file path
  std::string m_sourceFilePath;
  std::string m_destFilePath;

  // tissue label index
  uint1 m_uBGlabel;
	uint1 m_uAirLabel;
	uint1 m_uATBlabel;

  // the thresholds for cleansing
  uint2 m_uAirTh;
	uint2 m_uTissueHigh;
  uint2 m_uTagThHigh;
  uint2 m_uTagThLow;
	int4  m_iIntensityGapAtBoundary;

  /// maximum number of contrast enhanced region allowing for processing
  uint1 m_uMaximumNumOfPuddle;

  /// smallest bounds
	int4 m_uSmallestAirLumenVolume;
	int4 m_uSmallestPuddleVolume;

	float4 m_fSignificantPortion;
};

#endif

// complete revision history:
// $Log: vxCRES.h,v $
// Revision 1.7  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.6  2006/06/02 20:14:19  dongqing
// improve CRES quality per Diane's request.
//
// Revision 1.5  2006/04/24 13:26:53  dongqing
// fix the bug for generating CDV
//
// Revision 1.4  2006/04/20 20:41:34  dongqing
// add code for removal of floating debrises
//
// Revision 1.3  2006/04/19 19:53:45  dongqing
// bug free beta version
//
// Revision 1.2  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.1  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxCRES.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxCRES.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
