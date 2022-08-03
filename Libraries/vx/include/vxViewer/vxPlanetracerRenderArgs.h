// $Id: vxPlanetracerRenderArgs.h,v 1.7 2007/04/12 14:10:59 geconomos Exp $
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


// include declarations
#include "Triple.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxPlaneOrtho.h"
#include "BoundingRect.h"
#include "vxRenderingMode.h"
#include "vxCropBox.h"


// forward declarations
class vxEnvironment;
class vxComponent;
class vxBlockVisibilityTester;


/**
 * Contains all the data required for a planetracer rendering thread.  
 */
struct VX_VIEWER_DLL vxPlanetracerRenderArgs
{
  /// thread Id
  uint1 m_uThreadId;
  
  /// current render size
  Triple<uint4> m_imageSize;

  /// instance of block visibilty teset
  vxBlockVisibilityTester * m_pBlockVisibility;

  /// show labels
  bool m_bShowLabels;

  /// show color
  bool m_bShowColor;

  /// show threshholds?
  bool m_bShowThresholds;

  /// threshold low
  uint4 m_uThresholdLow;

  /// threshold high
  uint4 m_uThresholdHigh;

  /// offset for plane distance calculation
  int4 m_iOffset;
  
  /// maximum offset for plane distance calculation
  int4 m_iMaximumOffset;

  /// pointer to lookup table
  float4 * m_pfRGBA;

  /// pointer to components
  const std::vector<vxComponent > * m_pvComponents;

  /// rendering mode
  vxRenderingMode * m_pRenderingMode;

  /// crop box
  Box<float4> m_cropbox;
  
  /// crop rectangle
  BoundingRect<float4> m_croprect;

  /// orthogonal plane
  vxPlaneOrtho< float4 > * m_pPlaneOrtho;

  /// image pixels
  uint1 * m_pImagePixels;

  /// axis index
  uint1 m_uAxisIndex;

  /// min and max slab pair
  std::pair<uint2, uint2> m_minMaxSlicePair;

  /// number of slices in slab
  int4 m_iNumSlicesInSlab;

  /// volume dimensions
  Triple<int4> m_volDim;

  /// volume units
  Triple<float4> m_volUnits;

  /// ortho plane distance
  int4 m_iPlaneOrthoDistance;

  /// screen space rendering window
  BoundingRect<uint2> m_renderWindow;

  /// iterator for intensity volume
  vxBlockVolumeIterator<uint2> * m_pIterVol;

  /// iterator for label volume
  vxBlockVolumeIterator<uint2> * m_pIterLabel;

  /// needs lables
  bool m_bNeedLabels;

  /// number of bits to shift on alpha
  int4 m_iShiftBitsFor8bitAlpha;

  /// block size
  static const uint4 m_uBlockSize = 16;

  /// debugging color
  PixelRGBA< uint1 > m_debugColor;

}; // struct vxPlanetracerRenderArgs


// $Log: vxPlanetracerRenderArgs.h,v $
// Revision 1.7  2007/04/12 14:10:59  geconomos
// added m_uThreadId
//
// Revision 1.6  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.5  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.4  2007/02/22 16:39:20  geconomos
// code cleanup
//
// Revision 1.3  2006/09/21 14:58:33  geconomos
// switch datatype for components from vxComponentArray to std::vector<vxComponent>
//
// Revision 1.2  2006/08/03 14:16:08  geconomos
// cosmetics
//
// Revision 1.1  2006/07/21 14:59:15  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlanetracerRenderArgs.h,v 1.7 2007/04/12 14:10:59 geconomos Exp $
// $Id: vxPlanetracerRenderArgs.h,v 1.7 2007/04/12 14:10:59 geconomos Exp $
