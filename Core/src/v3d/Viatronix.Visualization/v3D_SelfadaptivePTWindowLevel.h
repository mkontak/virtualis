// $Id: v3D_SelfadaptivePTWindowLevel.h,v 1.3 2007/02/28 22:16:49 dongqing Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dongqing Chen (mailto:dongqing@viatronix.com)

/** 
 *  This class is to automaticall adjusting display colormap for PET case.
 *  It based on statistical analysis to the volume.
 */

 
 // pragmas
#pragma once

#include "vxShareableObject.h"
#include "v3D_Volume.h"

// namespaces
OPEN_VISUALIZATION_NS

// class definition
public ref class SelfadaptivePTWindowLevel
{
// functions
public:

  /// default constructor
  SelfadaptivePTWindowLevel( Viatronix::Visualization::Volume ^ intensityVolume );

  /// compute the window level
  bool IsAvailable();

  /// gets the 2d window value
  property float Window2d{ float get() { return m_fWindow2d; }}

  /// gets the 2d window value
  property float Window2dInverse{ float get() { return m_fWindow2dInverse; }}

  /// gets the 2d window level
  property float Level2d { float get() { return m_fLevel2d; }}

  /// gets the 3d window value
  property float Window3d { float get() { return m_fWindow3d; }}

  /// gets the 3d window level
  property float Level3d { float get() { return m_fLevel3d; }}

// data
private:

  // intensity volume
  Viatronix::Visualization::Volume ^ m_piVolume;

  // 2d window for the colormap
  float m_fWindow2d;

  // 2d window for the colormap
  float m_fWindow2dInverse;

  // 2d level for the colormap
  float m_fLevel2d;

  // 3d window for the colormap
  float m_fWindow3d;

  // 3d window for the colormap
  float m_fLevel3d;

}; // SelfadaptivePTWindowLevel


CLOSE_VISUALIZATION_NS


// Revision History:
// $Log: v3D_SelfadaptivePTWindowLevel.h,v $
// Revision 1.3  2007/02/28 22:16:49  dongqing
// update PT and MRI W/L computation due to the change of using
// significantBits to determine maximum value in volume.
//
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/08/11 15:34:09  michael
// code review
//
// Revision 1.2  2003/08/08 21:24:14  dongqing
// finalize PET preset
//
// Revision 1.1  2003/08/07 19:26:46  dongqing
// new
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SelfadaptivePTWindowLevel.h,v 1.3 2007/02/28 22:16:49 dongqing Exp $
// $Id: v3D_SelfadaptivePTWindowLevel.h,v 1.3 2007/02/28 22:16:49 dongqing Exp $
