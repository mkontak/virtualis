// $Id: vxUtils.h,v 1.20.2.1 2009/07/28 16:16:49 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxBlockVolumeIterator.h"
#include "Box.h"
#include "vxComponent.h"
#include <map>
#include <vector>


// forward declarations
class GLcontext;
template <class T> class Point2D;
template <class T> class Point;
template <class VoxelType> class vxBlockVolume;
template <class T> class Vector3D;
template <class T> class Vector4d;
template <class T> class Triple;
template <class T> class Box;
template <class T> class Component;
template <class T> class vxPlaneOblique;
template <class T> class vxPlaneOrtho;
class vxComponentArray;
class vxEnvironment;
class vxCamera;
class vxColormap;
class RaySegment;
class Quadrilateral;
class vxIntensityStatistic;
class vxZoomFactor;
class vxVisualizationPreset;
class vxViewerType;
class vxRenderingMode;
class vxCropBox;
class InteractiveSegmentation;
class vxComponentArray;


// class definition
class VX_VIEWER_DLL vxUtils
{
// enums
public:

  /// camera view types
  enum StandardCameraViewEnum 
  {
    ANTERIOR  = 0,
    POSTERIOR = 1,
    LEFT      = 2,
    RIGHT     = 3,
    SUPERIOR  = 4,
    INFERIOR  = 5,
  }; // enum StandardCameraViewEnum 


  /// slice orientations
  enum SliceOrientationEnum
  {
    SAGITTAL = 0, // index for x-axis
    CORONAL  = 1, // index for y-axis
    AXIAL    = 2, // index for z-axis
    OBLIQUE  = 3,
    CURVED   = 4
  }; // enum SliceOrientationEnum


  /// volume class
  enum vxEventVolumeEnum
  {
    VOLUME_INTENSITY_MODIFIED               = 0x00000001,
    VOLUME_INTENSITY_INITIALIZED            = 0x00000002,
    VOLUME_INTENSITY_BASEHISTOGRAM_MODIFIED = 0x00000004,
    VOLUME_LABEL_MODIFIED                   = 0x00000008,
    VOLUME_LABEL_INITIALIZED                = 0x00000010,
  }; // enum vxEventVolumeEnum


  /// colormap class
  enum vxEventColormapEnum
  {
    COLORMAP_MODIFIED              = 0x00000001,
    COLORMAP_MODE_CHANGED          = 0x00000002,
    COLORMAP_NEW                   = 0x00000004,
    COLORMAP_ALPHA_MODIFIED        = 0x00000008,
    COLORMAP_COLOR_MODIFIED        = 0x00000010,
    COLORMAP_WINDOW_LEVEL_MODIFIED = 0x00000020,
  }; // enum vxEventColormapEnum


  /// colormapEditorData class
  enum vxEventDataColormapEditorEnum
  {
    DATACOLORMAPEDITOR_MODIFIED = 0x00000001
  }; // enum vxEventDataColormapEditorEnum


  /// contour class
  enum vxEventContourEnum
  {
    CONTOUR_MODIFIED = 0x00000001
  }; // enum vxEventContourEnum


  /// camera class
  enum vxEventCameraEnum
  {
    CAMERA_INITIALIZED = 0x00000001,
    CAMERA_MODIFIED    = 0x00000001,
    CAMERA_NEW         = 0x00000002,
  }; // enum vxEventCameraEnum


  /// events supported by this class
  enum vxEventComponentArrayEnum
  {
    COMPONENTARRAY_ADDED         = 0x00000001,
    COMPONENTARRAY_REMOVED       = 0x00000002,
    COMPONENTARRAY_MODIFIED      = 0x00000004,
    COMPONENTARRAY_SELECTED      = 0x00000008,
    COMPONENTARRAY_NAME_MODIFIED = 0x00000010,
  }; // enum vxEventComponentArrayEnum


  /// events for the plane
  enum vxEventPlaneEnum
  {
    PLANE_INITIALIZED         = 0x00000001,
    PLANE_MODIFIED            = 0x00000002,
    PLANE_ORIENTATION_CHANGED = 0x00000004,
  }; // enum vxEventPlaneEnum


  /// events for annotations
  enum vxEventAnnotationsEnum
  {
    ANNOTATIONS_ADDED         = 0x00000001,
    ANNOTATIONS_COMPLETED     = 0x00000002,
    ANNOTATIONS_REMOVED       = 0x00000004,
    ANNOTATIONS_MODIFIED      = 0x00000008,
    ANNOTATIONS_SELECTED      = 0x00000040,
    ANNOTATIONS_UPDATE_PLANE  = 0x00000080,
  }; // enum vxEventAnnotationsEnum


  /// events for crop box
  enum vxEventCropBoxEnum
  {
    CROPBOX_MODIFIED = 0x00000001,
  }; // enum vxEventCropBoxEnum


  /// events for cursor 3d
  enum vxEventCursor3dEnum
  {
    CURSOR3D_MODIFIED = 0x00000001,
  }; // enum vxEventCursor3dEnum


  /// events for preset
  enum vxEventPresetEnum
  {
    PRESET_MODIFIED = 0x00000001,
  }; // enum vxEventPresetEnum


  /// events for preset
  enum vxEventMomentumEnum
  {
    MOMENTUM_MODIFIED = 0x00000001,
  }; // enum vxEventMomentumEnum


  /// events for data of renderer
  enum vxEventDataRendererEnum
  {
    DATARENDERER_MODIFIED
  }; // enum vxEventDataRendererEnum


  /// events for vxEnvironment
  enum vxEventEnvironmentEnum
  {
    ENVIRONMENT_MODIFIED
  }; // enum vxEventEnvironmentEnum
  

  /// events for vxRenderingMode
  enum vxEventRenderingModeEnum
  {
    RENDERINGMODE_ALGORITHM_MODIFIED,
    RENDERINGMODE_SLAB_THICKESSS_MODIFIED,
    RENDERINGMODE_DISPLAY_MODE_MODIFIED,
    RENDERINGMODE_SLICESHADOW_MODE_MODIFIED,
  }; // enum vxEventRenderingModeEnum


  /// events for vxEventTimer
  enum vxEventTimer
  {
    EVENTTIMER_STARTED,
    EVENTTIMER_KILLED,
    EVENTTIMER_MODIFIED
  }; // enum vxEventTimer
  
  /// events for vxEventTimer
  enum vxEventFontInfo
  {
    FONT_SIZE_CHANGED,
    FONT_FAMILY_NAME_CHANGED,
  }; // enum vxEventFontInfo

  /// events for vxZoomFactor
  enum vxEventZoomFactorEnum
  {
    ZOOMFACTOR_MODIFIED
  }; // enum vxEventZoomFactorEnum


  /// events for vxSliceShadow
  enum vxSliceShadowEnum
  {
    SLICESHADOW_MODIFIED
  }; // enum vxSliceShadowEnum

  // data structure Intersections
  typedef struct 
  {
    std::pair< Point3Df, Point3Df > m_centerline;
    std::pair< Point3Df, Point3Df > m_firstline;
    std::pair< Point3Df, Point3Df > m_secondline;
  } SliceShadowIntersections;

// functions
public:

  /// compute corners of ROI from two given diagonal points
  static void ComputeROIcorners(vxEnvironment & environment, Point3Df & handle0, Point3Df & handle1, Point3Df * pCorners);
  
  /// read the frame buffer of the win rect assuming data pointer is already allocated
  static bool ReadFramebuffer(float4 * pfRGBA, float4 * pfResult, bool bDepthOnly = false);

  /// read the frame buffer of the position within the win rect assuming data pointer is already allocated
  static bool ReadFramebuffer(Point2D<uint4> & position, 
                              float4 & fRed, float4 & fGreen, float4 & fBlue, float4 & fAlpha, 
                              float4 * pResult, bool bDepthOnly = false);

  /// calculates the absolute distance based on boundry conditions and offset.
  static uint4 CalculateOffsetDistance(uint4 fDistance, int4 iMaxDistance, 
                                       uint4 iOffset, uint4 iMaxOffset);


  /// returns how much to crop from all sides of a box when adjusting the initial zoom on an oblique view
  static float4 GetObliqueZoomFactor() { return 0.2F; }

  /// adjust camera to see all of the volume
  static void CameraViewAll(vxCamera & camera, const Box<float4> & roi);

  /// set camera to a medical standard view
  static void SetStandardCameraView(vxCamera & camera,
                                    const enum StandardCameraViewEnum eView, 
                                    const Box<float4> & roi,
                                    bool bViewAll);

  /// adjust camera to look at other bounding box proportionally
  static void AdjustCameraProportionally(vxCamera & camera, const Box<float4> & boxPreset, const Box<float4> & boxVolume);

  /// adjust the level value to account for the dataset characteristics
  static void AdjustColormapValuesToDataRange(vxColormap & colormap, vxBlockVolume<uint2> & volume);
  
  /// Adjust colormap from adaptive value to range on intensity volume back to standard CT range (bias = 1000)
  static void AdjustColormapValuesFromAdaptiveToCTStandard(vxColormap & colormap, vxBlockVolume<uint2> & volume);

  /// returns whether a ray hits the bounding box or not
  static bool HitBoundingBox(const      Box<float4> & fBoundingBox, 
                             const Point3D<float4>  & fvRayOrigin, 
                             const Vector3D<float4> & fvRayDirection , 
                             Point3D<float4>        & fvIntersectionCoord);

  /// returns whether a ray hits the bounding box or not
  static bool HitBoundingBox4d(const      Box<float4> & fBoundingBox, 
                               const Vector4d<float4>   fvRayOrigin, 
                               const Vector4d<float4>   fvRayDirection , 
                               Vector4d      <float4> & fvIntersectionCoord);

  /// clamps vector to viewport
  static Point2D<int2> vxUtils::ClampToViewport(Point2D<int2> pt);

  /// get the slab bounds in voxel coordinates
  static std::pair<uint4, uint4> GetSlabPair(vxEnvironment & environment);

  /// get the slab bounds in voxel coordinates
  static std::pair<uint4, uint4> GetSlabPair(const Triple<int4  > & volDim, 
                                             const Triple<float4> & units, 
                                             const int4           iNumSlices, 
                                             const uint1          uAxisIndex, 
                                             const int4           iPlaneDistance);

  /// get the bounding box of the visible slab
  static Box<uint4> GetSlabBoundingBox(const Triple<int4  > & volDim, 
                                       const Triple<float4> & units, 
                                       const int4           iNumSlices, 
                                       const uint1          uAxisIndex, 
                                       const int4           iPlaneDistance);

  ///////////////////////////////////////////////////////////////////////////
  //
  // The following is for the binary block historgram to generate 32 bit
  // encoding for transfer functions, components, etc.
  //

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeForInterval(const int4 iLower, const int4 iUpper, int4 iMaxValue = 4095);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeForTransferFunction(vxEnvironment & environment);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeForTransferFunction(const float4 * pRGBA, int4 iSize);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeFor8BitTransferFunction(const float4 * pRGBA);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeFor12BitTransferFunction(const float4 * pRGBA);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeComponentsVisible(vxComponentArray & vpComponents);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeLabelColorVisible(vxComponentArray & vpComponents);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeComponentsVisible(const std::vector< vxComponent > & components);

  /// generate 32 bit code for transfer function
  static int4 GenerateBitCodeLabelColorVisible(const std::vector< vxComponent > & components);


  /// get the bounding box of the visible components and further crop with the crop box.
  static Box<float4> GetVisibleBoundingBox(vxEnvironment & environment);

  /// get the bounding box of the visible components and further crop with the crop box.
  static Box<float4> GetVisibleBoundingBox( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                                            vxRenderingMode * pRenderingMode, vxCropBox * pCropBox, vxColormap * pColormap,
                                            vxComponentArray * pComponentArray, InteractiveSegmentation * pInteractiveSegmentation );

  /// Get the active bounds of the volume.
  static Box<uint4> GetActiveBoundingBox( const bool bRestrictToVisible, vxBlockVolume<uint2> * pIntensityVolume,
                                          vxViewerType * pViewerType, vxPlaneOrtho<float4> * pPlaneOrtho,
                                          vxRenderingMode * pRenderingMode, const uint4 uOffset,
                                          const uint4 uMaximumOffset, const vxCropBox * pCropBox );

  /// write out as new volume
  static void WriteComponentsAsSeparateVolumes(vxBlockVolume<uint2> rIntensityVol, vxBlockVolume<uint2> rLabelVol, int4 iNumLabels);

  /// get the four corners of the oblique plane from the camera and the plane oblique through projection
  static bool ComputeObliqueQuadrilateralOpenGL(Point3Df worldPoints[4], const vxPlaneOblique<float4> & planeOblique );

  /// Compute the corners of the oblique quadrilateral using pure math
  static Quadrilateral ComputeObliqueQuadrilateralAnalytical(const vxCamera * pCamera, const vxPlaneOblique<float4> * pPlaneOblique, const vxViewerType * pViewerType);

  /// Compute the statistics for an generalized ellipse on a plane
  static void ComputeEllipseStatistics(vxIntensityStatistic & outStatistics, const Point3Df & firstCornerPosition,
                                        const Vector3Df & verticalDiameter, const Vector3Df & horizontalDiameter,
                                        vxBlockVolume<uint2> & intensityVolume);

  /// retrieve the ray segment through the volume using OpenGL projection and the current view
  static bool GetIntersectionRayVolume(RaySegment & outRaySegment, vxEnvironment & environment, Point2D<float4> winMousePos);

  /// Get a measurement that represents the RAO or LAO angulation as if it was a C-arm
  static float4 GetRaoLaoDegrees(const Normal3Df & viewDirection);

  /// Get a string that represents the cranial or caudal angulation as if it was a C-arm
  static float4 GetCranCaudDegrees(const Normal3Df & viewDirection);

  /// Applies a visualization preset to an environment
  static void ApplyVisualizationPreset( vxVisualizationPreset * pPreset, vxEnvironment * pEnvironment );

  /// Get the intersection of two planes
  static bool GetIntersectionsWorld( const vxEnvironment * pDisplay, const vxEnvironment * pCurrent, vxUtils::SliceShadowIntersections & intersections );

private:

  /// Get the intersection of two ortho planes
  static bool GetIntersectionsOrthoOnOrtho( const vxEnvironment * pDisplay, const vxEnvironment * pCurrent, vxUtils::SliceShadowIntersections & intersections  );

}; // class vxUtils


// $Log: vxUtils.h,v $
// Revision 1.20.2.1  2009/07/28 16:16:49  kchalupa
// Code Walkthrough
//
// Revision 1.20  2008/05/02 20:22:39  dongqing
// When we apply colormap, we adjust it to adaptive to volume intensity range.
// When we save current colormap to file, we have not restroe the standard
// colormap range back from the adaptive value. That was fixed this time.
// The fix looks silly. In future, we need to fix the entire color map and volume
// intensity range problem from groun up.
//
// Revision 1.19  2006/10/05 17:01:24  geconomos
// changed signature of ComputeObliqueQuadrilateralAnalytical
//
// Revision 1.18  2006/10/03 20:11:52  geconomos
// added new event for vxRenderingMode
//
// Revision 1.17  2006/09/21 14:56:19  geconomos
// created overloads for GenerateBitCodeComponentsVisible() and GenerateBitCodeLabelColorVisible() to take std::vector<vxComponent> as argument
//
// Revision 1.16  2006/06/23 19:18:39  frank
// breaking out utility functions to take individual elements instead of just the environment
//
// Revision 1.15  2006/04/03 18:43:20  geconomos
// removed create histogram method
//
// Revision 1.14  2006/04/03 16:57:27  geconomos
// removed CreateHistogram
//
// Revision 1.13  2006/03/27 18:57:30  gdavidson
// Added GetIntersectionsWorld for slice shadow operations
//
// Revision 1.12  2006/03/02 15:46:22  frank
// pulled the environment out of one of the utility functions
//
// Revision 1.11  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.10  2006/02/17 15:11:58  frank
// working on extricating component handling from overengineered component handling classes
//
// Revision 1.9  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// Revision 1.8  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7.2.3  2005/08/12 17:20:28  geconomos
// Added vxEventMomentumEnum
//
// Revision 1.7.2.2  2005/07/29 18:21:00  frank
// Issue #4318: Added standard orientations button to oblique view
//
// Revision 1.7.2.1  2005/06/24 13:50:21  frank
// refactored the application of visualization presets
//
// Revision 1.7  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.6  2005/03/21 20:51:09  michael
// Gave rendering mode enum elements leading "RENDERINGMODE_" to make
// it unique. The actual enums unfortunately do not quality the scope.
//
// Revision 1.5  2004/08/30 18:19:38  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.4  2004/05/25 14:30:33  frank
// added base class for camera manipulators
//
// Revision 1.3  2004/04/26 22:57:05  vxconfig
// removed redundant code
//
// Revision 1.2  2004/04/26 19:01:58  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.66  2004/03/02 01:10:37  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.65.2.1  2004/02/27 21:32:48  michael
// fixed ellipse annotation creation resulting in an empty structure that would cause
// an exception when normalizing its diameter vectors. Two initial points are now
// guaranteed to be different
//
// Revision 1.65  2004/02/11 14:06:48  frank
// fixed restricted segmentation and slice computations
//
// Revision 1.64  2004/02/03 22:57:56  michael
// added "mailto:"
//
// Revision 1.63  2004/02/03 12:27:31  michael
// being more type specific (points and vectors)
//
// Revision 1.62  2004/01/23 13:30:42  frank
// Added utility function to compute the intensity statistics for an ellipse
//
// Revision 1.61  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.60  2004/01/07 18:30:40  frank
// Moved utility code from Vascular to here
//
// Revision 1.59  2003/12/23 21:35:26  michael
// added adjustment function for colormap initialized from preset for CT
//
// Revision 1.58  2003/12/16 20:19:02  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 1.57  2003/11/20 18:25:48  frank
// Fixed new component creation procedure
//
// Revision 1.56  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.55  2003/10/02 10:23:46  geconomos
// Added vxEventFontInfo enumeration.
//
// Revision 1.54  2003/09/29 17:17:55  michael
// added event enum flags to vxEnvironment and new events to plane & camera
//
// Revision 1.53  2003/09/29 14:35:31  michael
// added Event enum for EventTimer
//
// Revision 1.52  2003/09/25 00:46:08  michael
// added environment event
//
// Revision 1.51  2003/09/22 15:53:38  michael
// added modified for event timer
//
// Revision 1.50  2003/08/04 19:30:01  frank
// RAO/LAO now takes a view normal
//
// Revision 1.49  2003/08/04 14:41:04  frank
// Separated visibility tester class out from vxUtils
// Added rao/lao and cran/caud degree utility functions
//
// Revision 1.48  2003/07/30 15:41:46  michael
// fixed typo hisogram -> histogram
//
// Revision 1.47  2003/07/30 13:16:02  geconomos
// Merged vxEventBaseHistogramEnum into vxEventVolumeEnum.
//
// Revision 1.46  2003/07/29 11:04:15  michael
// added modified enum for BaseHistogram and added comments to code
//
// Revision 1.45  2003/07/25 16:04:26  frank
// Added events for visualization presets
//
// Revision 1.44  2003/06/19 20:22:39  michael
// additional modify events
//
// Revision 1.43  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.42  2003/06/09 15:52:36  michael
// added additional helper function to create intensity mask
//
// Revision 1.41  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.40  2003/06/04 18:46:18  jmeade
// code review.
//
// Revision 1.39  2003/05/29 11:58:31  michael
// removed GetScreenCoordinate() since it is now part of viewer type
//
// Revision 1.38  2003/05/15 20:12:10  frank
// Moving away from old point class
//
// Revision 1.37  2003/04/29 21:56:54  michael
// moved event from actual class file to vxUtils
//
// Revision 1.36  2003/04/24 13:58:27  michael
// added event properly in vxUtils
//
// Revision 1.35  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.34  2003/04/18 16:04:09  michael
// initial code review
//
// Revision 1.33  2003/04/09 21:39:41  michael
// added a utility function to vxUtils to compact the label indices
//
// Revision 1.32  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.31  2003/03/21 15:40:09  geconomos
// Removed ViewerTypeEnum
//
// Revision 1.30  2003/02/25 18:20:21  frank
// Worked on comments
//
// Revision 1.29  2002/12/24 17:35:10  frank
// Cleaned up picking in 3D
//
// Revision 1.28  2002/12/23 22:48:14  ingmar
// removed redundant use of Triple copy constructor
//
// Revision 1.27  2002/12/19 21:25:26  ingmar
// HitBoundingBox now takes a Box and not 2 vectors as box argument
//
// Revision 1.26  2002/12/17 17:40:53  frank
// Moved oblique corner computation into shared area so others can use it.
//
// Revision 1.25  2002/12/17 14:33:40  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.24  2002/11/27 19:21:08  ingmar
// removed superfluous rage from interface to IsBlockVisibleSegmentationRange()
// (the range is automatically determined in the constructor)
//
// Revision 1.23  2002/11/27 19:13:45  ingmar
// added IsBlockVisibleSegmentationRange()
//
// Revision 1.22  2002/11/26 18:06:55  ingmar
// added HitBoundingBox4d
//
// Revision 1.21  2002/11/19 21:26:26  michael
// Made method start with capital letter (ReadFramebuffer)
//
// Revision 1.20  2002/11/18 16:47:37  ingmar
// added IsBlockVisibleRaw()
//
// Revision 1.19  2002/11/15 21:06:09  michael
// changed view types
//
// Revision 1.18  2002/11/15 04:06:58  frank
// Fixed component name, returned the new component label.
//
// Revision 1.17  2002/11/13 02:18:16  frank
// Moved unique component name function into vxUtils.
//
// Revision 1.16  2002/11/12 00:14:53  michael
// cosmetics
//
// Revision 1.15  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.14  2002/10/24 15:30:06  michael
// added helper method to align a camera to a new box proportionally
//
// Revision 1.13  2002/10/03 17:34:43  michael
// adjusted helper methods for setting the camera
//
// Revision 1.12  2002/10/02 02:37:28  michael
// Moved helper functions for camera initialization to 70_vxUtils
//
// Revision 1.11  2002/09/27 22:59:12  frank
// Issue #2668: Fixed black line in MIP/VR images.
//
// Revision 1.10  2002/09/19 14:25:25  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.9  2002/09/17 21:41:58  dmitry
// ClampToViewport added
//
// Revision 1.8  2002/09/17 13:01:38  michael
// tried to move ReadFromFile of transfer function ... but has to wait.
//
// Revision 1.7  2002/09/11 20:43:56  frank
// Working on transfer function event mechanism
//
// Revision 1.6  2002/09/05 22:37:46  jaddonisio
// New vxComponent.
//
// Revision 1.5  2002/08/26 14:40:28  ingmar
// made leadiong function comment single line so it will show up in visual assist
//
// Revision 1.4  2002/08/22 22:35:25  uday
// Added GetVisibleBoundingBox().
//
// Revision 1.3  2002/08/12 14:35:19  ingmar
// added WriteComponentsAsSeparateVolumes()
//
// Revision 1.2  2002/08/02 14:51:37  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.1  2002/07/30 20:21:03  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.21  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.20  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.19  2002/07/01 15:20:33  michael
// clean-up and fix of bug
//
// Revision 1.18  2002/06/27 18:08:48  uday
// Moved ViewTypeEnum from vxViewer.
//
// Revision 1.17.2.3  2002/07/08 15:22:15  frank
// Restricted the sculpting to the visible voxels.
//
// Revision 1.17.2.2  2002/07/01 18:53:16  uday
// Moved viewType to vxUtils.
//
// Revision 1.17.2.1  2002/07/01 16:44:31  michael
// fixed problems with reading values from depth buffer
//
// Revision 1.17  2002/06/26 21:55:31  uday
// Added GetSlabPair().
//
// Revision 1.16  2002/06/26 20:09:45  uday
// Added HitBoundingBox().
//
// Revision 1.15  2002/06/20 19:22:18  michael
// fixed update mechanism for label and intensity volume
//
// Revision 1.14  2002/06/12 13:30:49  manju
// Modified GetScreenCoordinates method.
//
// Revision 1.13  2002/06/11 15:17:49  manju
// Added enum for cropbox modified.
//
// Revision 1.12  2002/05/21 15:27:34  manju
// Added a method to convert the world coordinates to screen .
//
// Revision 1.11  2002/05/20 19:10:48  dmitry
// Camera Enum added.
//
// Revision 1.10  2002/05/09 21:05:34  jenny
// added CreateHistogram
//
// Revision 1.9  2002/05/09 17:35:43  uday
// Added SliceOrientationEnum
//
// Revision 1.8  2002/05/09 14:44:56  dmitry
// TransferFunction enums moved to vxUtils.
//
// Revision 1.7  2002/05/01 16:08:02  dmitry
// Volume state enum added.
//
// Revision 1.6  2002/05/01 13:49:00  michael
// Started code review related stuff ...
//
// Revision 1.5  2002/04/14 00:15:28  michael
// adjusted method name
//
// Revision 1.4  2002/04/09 21:20:23  dmitry
// CalcOffsetDistance method added.
//
// Revision 1.3  2002/03/21 18:23:57  michael
// minor changes to the backprojection, still to be debugged
//
// Revision 1.2  2002/03/20 20:03:09  michael
// added required doxygen section include file
//
// Revision 1.1  2002/03/20 19:43:31  michael
// added OpenGL based back-projection methods
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUtils.h,v 1.20.2.1 2009/07/28 16:16:49 kchalupa Exp $
// $Id: vxUtils.h,v 1.20.2.1 2009/07/28 16:16:49 kchalupa Exp $
