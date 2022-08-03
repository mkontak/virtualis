// $Id: vxRaytracerRenderArgs.h,v 1.15 2007/04/12 14:11:27 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: George Economos (mailto:george@viatronix.com)
//         


// pragma declarations
#pragma once

// include declarations
#include "vxBlockVolume.h"
#include "vxComponent.h"
#include "vxDataRendererSoftware.h"
#include "Box.h"
#include "Vector3D.h"
#include "Vector4d.h"
#include "Normal.h"

// forward declarations
class vxDataRendererSoftware;
class vxRayGenerator;


/**
 * Contains all the required data for a raytracer rendering thread.  
 */
struct VX_VIEWER_DLL vxRaytracerRenderArgs
{
  /// for multi-threading purposes
  uint1 uThreadID;

  /// number of bits to shift to get to the 8 bit lookup
  int4 iShiftTo8Bit;

  /// colormap, preintegrated 2D
  float4 * pfColormap2d8bitPreIntegrated;

  /// RendererCpu3dData
  vxRayGenerator * pRayGenerator;

  /// alpha premultiplied sample color
  PixelRGBAf fPreMultSample;

  /// alpha premultiplied accumulated color
  PixelRGBAf fPreMultAccum;

  /// plain rgba display color (a==1)
  PixelRGBAu iDisplayColor;

  /// the foreground color as float
  PixelRGBAf fForegroundColor;

  /// background color as float
  PixelRGBAf fBackgroundColor;

  /// background color as uint1
  PixelRGBAu uBackgroundColor;

  /// the depth at which the ray intersects the slice shadow plane
  float4 fShadowPlaneDepth;

  /// the bounding box for the visible data area in volume coordinates
  Box<float4> fCropBoxVC; 

  /// the bounding box for the area of interest
  Box<float4> fBoxOfInterest; 

  /// where the ray first intersects the bounding box in volume coordinates
  Vector4d<float4> fRayIntersectBoundingBoxBgnVC;

  /// where the ray last intersects the bounding box in volume coordinates
  Vector4d<float4> fRayIntersectBoundingBoxEndVC;

  /// the view direction in volume coordinates
  Vector3D<float4> fViewDirectionVC;

  /// the direction of light 2 in volume coordinates
  Vector3D<float4> fLight2DirVC;

  /// the halfway vector of the light in volume coordinates
  Vector3D<float4> fHalfwayLight1VC;

  /// anisotropic correction gradient scaling factor for y
  float4 fGradientScaleY;

  /// anisotropic correction gradient scaling factor for z
  float4 fGradientScaleZ;

  /// voxel units of the intensity volume
  Vector3D<float4> fVoxelUnits;

  /// the reciprocal of the volume units of the intensity volume
  Vector3D<float4> fVoxelUnitsReciprocal;

  /// unused
  float4 fVolumeDiagonal;

  /// pointer to the RGBA image buffer
  uint1 * pImageBuffer;

  /// pointer to the depth buffer
  float4 * pDepthBufferWC;

  /// pointer to the depth buffer
  float4 * pDepthBufferVC;

  /// the current offset into the pixel buffer
  int4 iPixelOffset;

  /// the current offset into the depth buffer
  int4 iDepthOffset;

  /// the width of the RGBA image
  int4 iImageWidth;

  /// the HALF height of the RGBA image
  int4 iImageHeight;

  /// the current pixel coordinate (u,v)
  int4 u; 

  /// the current pixel coordinate (u,v)
  int4 v;

  /// the ray iterator for the intensity volume
  vxBlockVolumeRayIterator<uint2> * pRayIterIntensity;

  /// the ray iterator for the label volume
  vxBlockVolumeRayIterator<uint2> * pRayIterLabel;

  /// the ray iterator for the space leap distance volume
  vxBlockVolumeIterator<int1>     * pVolIterSLD;

  /// the ray iterator for shadow rays
  vxBlockVolumeRayIterator<uint2> * pRayIterShadow;

  /// the iterator for sampling the label in shadow rays
  vxBlockVolumeIterator<uint2> * pIterLabelShadow;

  /// the distance between samples (MAKE THIS A GLOBAL CONST IF IT DOES NOT CHANGE)
  float4 fInterSampleDistance;

  /// the component array
  std::vector<vxComponent> * pvComponents;

  /// the weight of the two gradients on each side of the current sample when using the pre-integrated transfer function
  float4 fGradientWeight;

  /// the sampled intensity data
  int4 iSample;

  /// the index of the last sample into the pre-integrated transfer function lookup table
  int4 iPrevIndex;

  /// the label voxel
  int4 iLabel;

  /// true when the ray should continue traversing
  bool bContinueRay;

  /// true when the intensity range should be blended into the each sample
  bool bIsShowIntensityRange;

  /// true when volume rendering mode is active (not MIP or X-ray)
  bool bIsVRMode;

  /// true when MIP mode is active
  bool bIsMipMode;

  /// true when X-ray mode is active
  bool bIsXRayMode;

  /// the lower range of the blended-in intensity threshold
  int4 iShowIntensityRangeLower;

  /// the upper range of the blended-in intensity threshold
  int4 iShowIntensityRangeUpper;

  /// z-buffer depth in world coordinates
  float4 fDepthToSurfaceWC;

  /// z-buffer depth in volume coordinates
  float4 fDepthToSurfaceVC;

  /// labels on top mode
  bool bIsLabelsOnTopMode;
  
  /// if this is the first sample
  bool bFirstSample;

  /// if the ray should start sampling at the neighbor depth
  bool bUseNeighborDepth;

  /// the depth of a neighbor
  float4 fNeighborDepth;

  /// if all rendering should be grayscale
  bool bGrayscale;
  
  /// if slice shadows are enabled
  bool bSliceShadowsEnabled;

  /// clips the volume starting at the slice shadow plane
  bool bClipSliceShadow;

  /// the eye position in world coordinates
  Point3D<float4> eyePositionWorld;

  /// the camera's view direction in world coordinates
  Normal3Df viewDirectionWC;

  /// indicates if labels are to be considered
  bool bShowLabels;

  /// color for debugging
  PixelRGBA< uint1 > debugColor;

}; // struct vxRaytracerRenderArgs


// $Log: vxRaytracerRenderArgs.h,v $
// Revision 1.15  2007/04/12 14:11:27  geconomos
// added debug color for analyzing thread usage
//
// Revision 1.14  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.13  2007/02/28 20:10:44  geconomos
// spelling
//
// Revision 1.12  2006/12/11 16:58:46  geconomos
// Issue #5158: compnent display is incorrect
//
// Revision 1.11  2006/10/23 01:38:29  geconomos
// using two depth buffers for world and volume coordinates
//
// Revision 1.10  2006/10/20 20:51:02  geconomos
// added camera eype position and view direction
//
// Revision 1.9  2006/09/21 14:59:06  geconomos
// switch datatype for components from td::vector<vxComponent*>to std::vector<vxComponent>
//
// Revision 1.8  2006/08/03 14:15:56  geconomos
// cosmetics
//
// Revision 1.7  2006/07/18 16:19:59  geconomos
// added bClipSliceShadow
//
// Revision 1.6  2006/07/12 19:41:17  geconomos
// added "bSliceShadowsEnabled"
//
// Revision 1.5  2006/07/07 13:39:24  geconomos
// added grayscale flag
//
// Revision 1.4  2006/06/27 20:14:03  geconomos
// change fCropBox -> fCropBoxVC
//
// Revision 1.3  2006/06/21 18:46:53  geconomos
// added vxRayGenerator instance pointer
//
// Revision 1.2  2006/06/21 14:07:49  geconomos
// + code cleanup
// + exported classes
//
// Revision 1.1  2006/06/20 20:05:48  geconomos
// refactored vxSoftwareRendererCpu3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRaytracerRenderArgs.h,v 1.15 2007/04/12 14:11:27 geconomos Exp $
// $Id: vxRaytracerRenderArgs.h,v 1.15 2007/04/12 14:11:27 geconomos Exp $
