// $Id: v3D_SelfadaptiveMRWindowLevel.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dongqing Chen (mailto:dongqing@viatronix.com)

/** 
 *  This class is to automaticall adjusting display colormap for MRI case.
 *  It based on statistical analysis on the center slice of the volume.
 */
 
// pragmas
#pragma once

#include "vxShareableObject.h"
#include "v3D_Volume.h"

// namespaces
OPEN_VISUALIZATION_NS

// class definition
public ref class SelfadaptiveMRWindowLevel
{
// functions
public:
  
  /// default constructor
  SelfadaptiveMRWindowLevel( Viatronix::Visualization::Volume ^ intensityVolume );
  
  /// Compute the window level
  bool IsAvailable();

  /// gets the window value
  property float Window2dGeneral { float get() { return m_fWindow2dGeneral; }}

  /// gets the window value
  property float Level2dGeneral { float get() { return m_fLevel2dGeneral; }}

  /// gets the window value
  property float Window3dGeneral { float get() { return m_fWindow3dGeneral; }}

  /// gets the window value
  property float Level3dGeneral { float get() { return m_fLevel3dGeneral; }}

  /// gets the window value
  property float Window2dMRA { float get() { return m_fWindow2dMRA; }}

  /// gets the window value
  property float Level2dMRA { float get() { return m_fLevel2dMRA; }}

  /// gets the window value
  property float Window3dMRA { float get() { return m_fWindow3dMRA; }}

  /// gets the window value
  property float Level3dMRA { float get() { return m_fLevel3dMRA; }}

  /// gets the window value
  property float Window2dMIP { float get() { return m_fWindow2dMIP; }}

  /// gets the window value
  property float Level2dMIP { float get() { return m_fLevel2dMIP; }}

  /// gets the window value
  property float Window3dMIP { float get() { return m_fWindow3dMIP; }}

  /// gets the window value
  property float Level3dMIP { float get() { return m_fLevel3dMIP; }}

  /// gets the window value
  property float Window2dEndoView { float get() { return m_fWindow2dEndoView; }}

  /// gets the window value
  property float Level2dEndoView { float get() { return m_fLevel2dEndoView; }}

  /// gets the window value
  property float Window3dEndoView { float get() { return m_fWindow3dEndoView; }}

  /// gets the window value
  property float Level3dEndoView { float get() { return m_fLevel3dEndoView; }}

  /// gets the window value
  property float Window2dfMR { float get() { return m_fWindow2dfMR; }}

  /// gets the window value
  property float Level2dfMR { float get() { return m_fLevel2dfMR; }}

  /// gets the window value
  property float Window3dfMR { float get() { return m_fWindow3dfMR; }}

  /// gets the window value
  property float Level3dfMR { float get() { return m_fLevel3dfMR; }}

private:

  /// determine the lower bound for the MRTOF cases
  void DetermineLowBoundForMRTOF( float & fLowBound, uint2 uMean, uint2 uMax );

// data
private:

  /// intensity volume
  Viatronix::Visualization::Volume ^ m_piVolume;

  // 2d window for the colormap
  float m_fWindow2dGeneral;
  
  // 2d level for the colormap
  float m_fLevel2dGeneral;

  // 3d window for the colormap
  float m_fWindow3dGeneral;

  // 3d window for the colormap
  float m_fLevel3dGeneral;

  // 2d window for the colormap
  float m_fWindow2dMRA;

  // 2d level for the colormap
  float m_fLevel2dMRA;

  // 3d window for the colormap
  float m_fWindow3dMRA;

  // 3d window for the colormap
  float m_fLevel3dMRA;

  // 2d window for the colormap
  float m_fWindow2dMIP;

  // 2d level for the colormap
  float m_fLevel2dMIP;

  // 3d window for the colormap
  float m_fWindow3dMIP;

  // 3d window for the colormap
  float m_fLevel3dMIP;

  // 2d window for the colormap
  float m_fWindow2dfMR;

  // 2d level for the colormap
  float m_fLevel2dfMR;

  // 3d window for the colormap
  float m_fWindow3dfMR;

  // 3d window for the colormap
  float m_fLevel3dfMR;

  // 2d window for the colormap
  float m_fWindow2dEndoView;

  // 2d level for the colormap
  float m_fLevel2dEndoView;

  // 3d window for the colormap
  float m_fWindow3dEndoView;

  // 3d window for the colormap
  float m_fLevel3dEndoView;

}; // SelfadaptiveMRWindowLevel


CLOSE_VISUALIZATION_NS


// Revision History:
// $Log: v3D_SelfadaptiveMRWindowLevel.h,v $
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/02/06 21:16:30  dongqing
// add MIP preset for MRA
//
// Revision 1.4  2003/08/11 15:34:09  michael
// code review
//
// Revision 1.3  2003/07/10 12:47:23  dongqing
// add automatic lower bound estimation for the MRTOF
//
// Revision 1.2  2003/06/20 14:50:19  dongqing
// adjust MR preset group
//
// Revision 1.1  2003/06/13 18:59:44  dongqing
// initialization
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SelfadaptiveMRWindowLevel.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_SelfadaptiveMRWindowLevel.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
