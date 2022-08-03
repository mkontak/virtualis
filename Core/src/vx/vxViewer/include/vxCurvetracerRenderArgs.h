// $Id: vxCurvetracerRenderArgs.h,v 1.4 2007/03/02 17:40:47 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


// forward declarations
template <typename T> class vxBlockVolume;
class vxRibbon;
class vxComponentArray;
class vxRenderingMode;
class vxVessel;
class vxWinRect;


/**
 * Contains all the data required for a curvetracer rendering thread.  
 */
struct VX_VIEWER_DLL vxCurvetracerRenderArgs
{
  /// ribbon
  vxRibbon * m_pRibbon;

  /// intensity volume
  vxBlockVolume<uint2> * m_pIntensityVolume;

  /// rendering mode
  vxRenderingMode * m_pRenderingMode;

  /// win rect
  vxWinRect * m_pWinRect;

  /// vessel
  vxVessel * m_pVessel;

  /// sampling step size
  float4 m_fMPRRenderSamplingStep;

  /// bits required for alpha shift
  int4 m_iShiftBitsFor8bitAlpha;

  /// color table
  float4 * m_pfRGBA;

}; // struct vxCurvetracerRenderArgs


// $Log: vxCurvetracerRenderArgs.h,v $
// Revision 1.4  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.3  2007/02/22 18:27:15  geconomos
// code cleanup
//
// Revision 1.2  2006/09/21 14:57:31  geconomos
// removed unused vxComponentArray
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCurvetracerRenderArgs.h,v 1.4 2007/03/02 17:40:47 geconomos Exp $
// $Id: vxCurvetracerRenderArgs.h,v 1.4 2007/03/02 17:40:47 geconomos Exp $
