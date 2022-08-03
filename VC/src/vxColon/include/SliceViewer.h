// $Id: SliceViewer.h,v 1.8.2.1 2009/01/21 22:02:59 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.net)


// pragmas
#pragma once


// includes
#include "StudyLoad.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "vxSliceRenderer.h"
#include "MeasureLineList.h"
#include "MeasureAreaList.h"


// namespaces
namespace ReaderLib
{

// Global enums
namespace SliceOrientations
{
  /// An enumerated type describing the slice orientation
  enum TypeEnum { eAXIAL, eSAGITTAL, eCORONAL, eCROSS_SECTION, eVIEWDIR };
}

// Forward declarations
class VolumeRender;
class Dataset;


/**
 * Description: A class to support viewing of volume slices
 */
class SliceViewer
{
// inner classes
public:

  /// store some statistics on the slices
  class SliceStats
  {
  public:

    // constructor
    SliceStats() : m_uFullVolumeSlice(0), m_uSliceDiff(0) {}

    /// the slice number in the full volume
    uint4 m_uFullVolumeSlice;

    /// the difference in the number of slices
    uint4 m_uSliceDiff;

    /// the dimension of the slice
    Point2D<uint4> m_sliceDims;

  }; // class SliceStats

private:

  /**
   * Bit vector for current characteristics of displayed slice
   * (to intelligently determine whether slice needs update)
   */
  class SliceOptions
  {
  public:

    /// Positions of state enum values
    enum { eShowUncleansed };

  public:
    
    /// Constructor
    SliceOptions() : m_uCurrOptions(0) { }
    
    /// Constructor
    SliceOptions(const SliceViewer& viewer) : m_uCurrOptions(0) { UpdateOptions(viewer); }
    
    /// Type cast operator (mainly for comparisons)
    operator uint4() const { return m_uCurrOptions; }
    
    /// Update to current options
    inline void UpdateOptions(const SliceViewer& viewer) { m_uCurrOptions = (viewer.m_bShowUncleansed?1:0) << eShowUncleansed; }

  private:

    /// Bit-vector for options at last update
    uint4 m_uCurrOptions;

  }; // class SliceOptions


// member functions
public:

  /// Constructor
  SliceViewer();

  /// Destructor
  ~SliceViewer();

  /// Copy constructor
  SliceViewer(const SliceViewer & other);

  /// Assignment operator
  SliceViewer & operator =(const SliceViewer & other);

  /// Sets the slice orientation
  void SetOrientation(const SliceOrientations::TypeEnum eSliceOrientation);

  /// Sets the slice orientation
  inline SliceOrientations::TypeEnum GetOrientation() const;

  /// Get the currently displayed view slice number
  uint4 GetCurrentDisplayedSlice() const;

  /// Get the current orthogonal slice number for this orientation
  uint4 GetCurrentOrthoSlice() const;

  /// Set the orthogonal position to the current volume position for this orientation
  void ResetCurrentVolumePosition();

  /// Initializes Volume-dependant parameters (from .ecv volume)
  void InitVolumeLoaded();

  /// Sets up the OpenGL rendering context
  void SetupOpenGLContext();

  /// Resizes the display window
  void ResizeDisplay(const uint4 uWidth, const uint4 uHeight);

  /// Returns the dimensions based on the current viewer orientation
  Point<uint4> GetOrientationDim() const;

  /// Gets a buffer for the render slice
  void GetRenderSlice(SliceStats & sliceStats, const bool bAutoFly, const bool bPixelMap);

  /// Render the slice
  void RenderSlice(const bool bUseCachedImaged, const bool bMainWindow, const Rect<int4> & rect, const float4 fLeft,
                   const float4 fBottom, const bool bPixelMap,
                   const bool bAutoFly, const bool bInverseImage);

  /// render the paint volume
  void RenderPaint(const float4 fLeft = 0, const float4 fBbottom = 0, bool bInverseImage = false);

  /// render the ins volume
  void RenderIns(const float4 fLeft = 0, const float4 fBottom = 0, bool bInverseImage = false);

  /// render the dfb volume
  void RenderDdb(const float4 fScale, const float4 fLeft = 0, const float4 fBottom = 0, bool bInverseImage = false);

  /// render the csi volume
  void RenderCsi(const float4 fLeft = 0, const float4 fBottom = 0, bool bInverseImage = false);

  /// render the pdr volume
  void RenderPdr(const float4 fLeft = 0, const float4 fBottom = 0, bool bInverseImage = false);

  /// render a biopsy image
  void RenderBiopsy(const float4 fLeft = 0, const float4 fBottom = 0, bool bInverseImage = false);

  /// render the camera
  void RenderCamera( const Rect<int4> & rect );

  /// render the 2D annotations
  void Render2DAnnotations(const Rect<int4> & rect);

  /// Render the CAD findings in a window, scaling the arrow to the given window size
  void RenderCadFindings( const Rect<int4> & viewRect );

  /// Adjust the current slice
  int4 Scroll(const int4 iCount, const bool bMainView = true);

  /// scroll to the end of the segment
  int4 ScrollToSegmentEnd(bool bAutoFly);  

  /// scroll to a specific slice
  void ScrollTo(const int4 iCount);

  /// begin 2D auto flight
  int4 AutoFlight2D();

  /// go one step in 2D flight
  int4 StepFlight2D(const bool bStepForward);

  /// set the slice number
  void SetSliceNumber();

  /// get the viewport rectangle
  Rect<int4> GetViewportRect(const float4 fLeft, const float4 fBottom);

  /// is the volume loaded (duh)
  bool IsVolumeLoaded() const;

  /// Get the number of slices for this orientation
  inline uint4 GetNumberOfSlices() const;

  /// is this an axial slice
  inline bool IsAxial() const;

  /// is this an orthogonal slice
  inline bool IsOrthogonal() const;

  /// is this a cross-sectional slice
  inline bool IsCrossSectional() const;

  /// Get the number of the last slice for caching purposes.
  inline int4 GetSliceNumber() const;

  /// Set the number of the last slice for caching purposes.
  inline void SetSliceNumber(const int4 iNum);

  /// switch between rendering of 'blocked' or 'smoothed' pixels.
  inline void SwitchBlockAndSmooth();

  /// Returns whether or not block pixels are being used
  inline bool GetBlockPixels() const;

  /// Get value of 'm_bDisplayOverviewSliceGrasp'
  inline bool GetValueOfDisplayOverviewSliceGrasp() const;

  /// Set value of 'm_bDisplayOverviewSliceGrasp'
  inline void SetValueOfDisplayOverviewSliceGrasp(const bool bValue);

  /// Toggle view position indicator
  inline static void ToggleRenderCamera();

  /// Get value of 'm_bRenderCamera'
  inline static bool GetStatusOfRenderCamera();

  /// Set value of 'm_bRenderCamera'
  inline static void SetStatusOfRenderCamera(const bool bValue);

  /// Select the desired study orientation.
  inline void SetDatasetType(const DatasetOrientations::TypeEnum eOrientation);

  /// Get zoom factor.
  inline const int4 GetZoomPercentage() const;

  /// Text to be displayed on top of the window
  inline const int1 * TopText() const;

  /// Text to be displayed on bottom of the window
  inline const int1 * BottomText() const;

  /// Text to be displayed on right of the window
  inline const int1 * RightText() const;

  /// Text to be displayed on left of the window
  inline const int1 * LeftText() const;

  /// Get a picked location in world coordinates
  Point<float4> GetPickedLocationWorldCoord(const float4 fPosX, const float4 fPosY) const;

  /// Get a picked location in volume coordinates
  Point<float4> GetPickedLocationVolumeCoord(const float4 fPosX, const float4 fPosY) const;

  /// move the picked location to the center of the display
  void MovePickedLocationToVoxelCenter(float4 & fPosX, float4 & fPosY) const;

  /// Retrieve the current study for this viewer.
  Dataset * GetDataset() const;

  /// Retrieve the current dataset type for this viewer.
  DatasetOrientations::TypeEnum GetDatasetType() const;

  /// set the current slice number
  inline void SetCurrentSliceNo(const int4 iSliceNo) { m_iCurrentSliceNo = iSliceNo; }

  /// get the distance from the current slice
  uint4 DistFromCurrentSlice(const Point<float4>& ptVolumeCoord) const;

  /// calculate the manhattan distance within the plane
  float4 ManhattanDistInPlane(const Point<float4>& pt1, const Point<float4>& pt2) const;

  /// Convert a 3D volume Position to 2D GL 0..1 coordinates.
  Point<float4> Calc2DGLPosFromVolumeCoord(const Point<float4> &pos);

  /// sets the skeleton index
  inline void SetSkeletonIndex(const float4 fSkeletonIndex) { m_fSkeletonIndex = fSkeletonIndex; }

  /// gets the skeleton index
  inline float4 GetSkeletonIndex() const { return m_fSkeletonIndex; }

protected:

  /// initialize the class
  void Initialize();

private:

  /// draw the selection frame
  void DrawSelectionFrame();

  /// Draw 2D frame
  void Draw2DOverviewSliceFrame();
  
  /// Draw 2D frame
  void Draw2DOverviewSliceFrame(Rect<float4> & vRect);

  /// test if the camera is visible.
  void AdjustImageAreaForArrowPos(const bool bAutoFly);

  /// adjust zoom area for better zooming (centering, repeable...).
  inline void ZoomAdjust();

  /// construct slice image
  void CreateSliceImageFromVol(const uint4 uSliceWidth, const uint4 uSliceHeight, const uint4 uSliceNo, 
                               const SliceOrientations::TypeEnum eSliceOrientation, 
                               const bool bShowUncleansed = false);

  /// construct a slice image from the test volume
  void CreateSliceImageFromTestVol(const uint4 uSliceWidth, const uint4 uSliceHeight, const uint4 uSliceNo, 
                                   const SliceOrientations::TypeEnum eSliceOrientation, 
                                   const bool bShowUncleansed = false);

  /// is the volume initialized?
  inline const bool IsVolumeInitialed() const { return m_bVolumeInitialed; }

  /// sets the volume initialized flag
  inline bool SetVolumeInitialed(const bool bFlag) { return m_bVolumeInitialed = bFlag; }

  /// initializes the texture coordinates array for the 8-bit renderer
  void InitTextureCoords8Bit( Point<float4> pCorners[4] );

  /// initializes the texture coordinates array for the 12-bit renderer
  void InitTextureCoords12Bit( Point<float4> pCorners[4] );

  /// Gets values associated with non-square images
  void GetAspectRatioStats(Point2D<float4>& shiftPt, Point2D<float4>& ratioPt) const;

  /// step forward along the 2D auto flight path
  int4 StepForward(const bool bFlip);

  /// step backward along the 2D auto flight path
  int4 StepBackward(const bool bFlip);

  /// set the viewport
  void SetViewport(const float4 fLeft, const float4 fBottom);

  /// set the viewport (alternate version)
  void SetViewport(const float4 fLeft, const float4 fBottom, bool bDummy);

  /// enumeration for reverse image processing
  enum ReverseVideoImageEnum { eRVI_Gray, eRVI_RGB, eRVI_RGBA };

  /// do image reverse inverse processing
  void ReverseVideoImage(const ReverseVideoImageEnum eDataFormat, const uint4 uImgWidth, const uint4 uImgHeight, void* pImg);

  /// get min power of 2 larger or equal to nValue
  uint4 GetMaxPowerOf2(const uint4 uValue);

// data members
public:
  
  /// An enumerated type describing the dataset orientation
  DatasetOrientations::TypeEnum m_eDatasetType;

  /// Slice difference threshold for displaying the current camera
  static const uint4 m_uCameraSliceDiffThreshold;

  /// Slice difference threshold for displaying anotations
  static const uint4 m_uAnnotationSliceDiffThreshold;

  /// Units length of the camera position-indication arrow
  static const float4 m_uCameraArrowLength;

  /// flag: show uncleansed ...
  static bool m_bShowUncleansed;

  /// flag: flight along centerline.
  static bool m_bFlightAlongCenterline;

  /// flag: arrow in slice center.
  static bool m_bArrowInSliceCenter;

  /// the dimension of display window.
  uint4 m_uDisplayWidth, m_uDisplayHeight;

  /// start point and end point to mark a select area for zooming.
  Rect<uint4> m_rSelect;   // left-top and right-bottom corner.

  /// anchor point that controls the moving of for 2D slice. 
  Point2D<int4> m_viStartMovePt;

  /// anchor point for zooming around it. 
  Point2D<float4> m_vfAnchorPt;

  /// corners of the slice image, may change after zooming.
  Rect<float4> m_rZoom;

  /// the list of measurement lines
  MeasureLineList m_measureLineList;
  
  /// the list of measurement areas
  MeasureAreaList m_measureAreaList;

  /// Flag for whether display overview slice grasp.
  bool m_bDisplayOverviewSliceGrasp;

  /// vxSliceRenderer
  vxSliceRenderer m_sliceRenderer;

  /// render mode same for all renderers
  static vxSliceRenderer::vxSliceRenderingModeEnum m_eGlobalRenderMode;

  /// thin slab/mip thickness same for all renderers
  static float4 m_fGlobalSlabThickness;

  /// indicates if the vxSlice renderer should be used to render the 2D slices
  bool m_bSliceRendererEnabled;

  /// indicates if the slice should be re-rendered
  bool m_bForceSliceRender;

protected:

  /// An enumerated type describing the slice orientation
  SliceOrientations::TypeEnum m_eSliceOrientation;

  /// Sizes of the slice window.
  uint4 m_uRenderWidth, m_uRenderHeight;

  /// volume size in screen coord system
  uint4 m_uVolumeWidth, m_uVolumeHeight; 

  /// width of the displayed volume in world coordinates (usually mm)
  float4 m_fWorldWidth;

  /// height of the displayed volume in world coordinates (usually mm)
  float4 m_fWorldHeight;

private:

  /// A pointer to an rgb image for the ins vol
  uint1 m_uRGBSegImage[512*512*3];

  /// number of slices.
  uint4 m_uNumOfSlices;

  /// the image buffer (CrossSection/ViewDir: basic size: 128X128)
  uint1 * m_puObliqueImage;

  /// the image buffer (Axial/Sagittal/Coronal: basic size 512X512)
  uint1 * m_puSliceImage;

  /// max width (i.e. number of slices) of the A/S/C slice images.
  const uint4 m_uSliceWidthMax;

  /// max height (i.e. number of slices) of the A/S/C slice images.
  const uint4 m_uSliceHeightMax;

  /// current slice No of the slice.
  int4 m_iCurrentSliceNo;

  /// The size of the image
  float4 m_fSizeWorldUnits;

  /// The 4 current points on the MPR window
  Point<float4> m_bottomLeft, m_bottomRight, m_topLeft, m_topRight;

  /// the distance from the camera of the current view-direction oblique slice
  float4 m_fViewDirDistance;

  /// the size of the oblique image buffer
  const uint4 m_uObliqueImageSize;

  /// The number of the last slice for caching purposes
  int4 m_iSliceNumber;

  /// Rendering 'blocked' or 'smoothed' pixels.
  bool m_bBlockedPixel;

  /// set this flag after the volume is loaded.
  bool m_bVolumeInitialed;

  /// the current skeleton index
  float4 m_fSkeletonIndex;

  /// the slice width
  uint4 m_uSliceWidth;
  
  /// the slice height
  uint4 m_uSliceHeight;
  
  /// the size of the slice
  uint4 m_uSliceSize;

  /// the scale of the slice
  Triple<float4> m_scale;

  /// the cropped area of the slice
  Triple<float4> m_crop;

  /// the grow slice image
  Image<bool> m_growSliceImage;

  /// Saves the currently-displayed slice view options
  SliceOptions m_currSliceOptions;

  /// Flag for whether display view position indicator.
  static bool m_bRenderCamera;

// friends, relatives, employees, associates, and enemies

  /// Friend: VolumeRender class
  friend VolumeRender;

}; // class SliceViewer


#include "SliceViewer.inl"


} // namespace SliceViewer


// $Log: SliceViewer.h,v $
// Revision 1.8.2.1  2009/01/21 22:02:59  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.8  2006/11/21 23:45:25  jmeade
// Issue 5180: don't draw cad marks in x-sec/viewdir orientations.
//
// Revision 1.7  2006/02/28 17:06:56  jmeade
// code review.
//
// Revision 1.6  2006/02/13 12:57:59  frank
// code review
//
// Revision 1.5  2006/02/08 17:49:47  jmeade
// code review: comments.
//
// Revision 1.4  2006/01/31 14:44:38  frank
// split up volume measure file
//
// Revision 1.3  2005/11/03 13:41:04  frank
// Issue #4508: coding standards
//
// Revision 1.2  2005/09/23 03:19:56  vxconfig
// working on display of cad findings
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.24.2.5.2.4.6.1  2005/06/23 12:17:32  geconomos
// Issue# 4203: added parameter to RenderSlice indicating to use the cached image or not
//
// Revision 3.24.2.5.2.4  2005/04/13 18:33:21  frank
// spelling
//
// Revision 3.24.2.5.2.3  2005/04/05 19:21:38  frank
// Issue #4105: Stopped flying when overview is double clicked
//
// Revision 3.24.2.5.2.2  2005/03/26 19:46:06  frank
// revamped main application idle/processing loop - checkin for Jeff
//
// Revision 3.24.2.5.2.1  2005/03/14 23:23:00  jmeade
// Merged last change from 1.4 branch.
//
// Revision 3.24.2.4.2.4  2005/03/14 21:43:30  frank
// Issue #4024: Fixed slice display aspect ratio when slices > 512
//
// Revision 3.24.2.4.2.3  2001/01/05 18:54:52  jmeade
// removed spaces.
//
// Revision 3.24.2.4.2.2  2004/10/07 00:29:57  jmeade
// One method for aspect ratio/shift calculation.
//
// Revision 3.24.2.4.2.1  2004/10/06 02:16:48  jmeade
// Adjust draw rectangle for non-squared slice images.
//
// Revision 3.24.2.4  2004/09/22 21:56:49  jmeade
// Reverse-video imaging for new slice renderers.
//
// Revision 3.24.2.3  2004/09/10 18:42:04  jmeade
// Non-square display regions; Global slab thickness.
//
// Revision 3.24.2.2  2004/04/23 15:33:56  geconomos
// Initial work for integrating in newer 12-bit slice renderers.
//
// Revision 3.24.2.1  2004/04/07 19:04:13  jmeade
// Issue 3578: Restrict array access for datasets larger than 512 slices.
//
// Revision 3.24  2002/11/24 17:47:32  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.23  2002/11/15 16:48:16  kevin
// On 2D X-sect autoflight use force the use of the smoothed skelton upvectors
//
// Revision 3.22  2002/11/04 20:38:19  kevin
// Mad annotations 3D instead of 2D and added drawing of annotations in endoscopic view
//
// Revision 3.21  2002/09/27 14:21:15  ingmar
// replaced Size2D with Point2D
//
// Revision 3.20  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.19  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.18  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.17  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.16  2002/04/12 02:15:09  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.15  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.14.2.10  2002/08/13 15:28:41  jmeade
// Issue 2587:  Slice viewer updates slice based on changes to relevant modes.
//
// Revision 3.14.2.9  2002/08/01 14:48:15  jmeade
// Issue 2587:  Made a number of 2D slice options global.
//
// Revision 3.14.2.8  2002/06/02 01:02:29  jmeade
// Issue 2270: Expose camera arrow length value.
//
// Revision 3.14.2.7  2002/05/31 02:52:34  jmeade
// Issue 2270: Localize in one function the determination of current visible z-value.
//
// Revision 3.14.2.6  2002/05/29 18:16:17  jmeade
// Issue 2270:  Function for entire point (not just z-value) based on orientation.
//
// Revision 3.14.2.5  2002/05/25 05:39:40  jmeade
// Function to interpret z-value of a point based on a slice orientation.
//
// Revision 3.14.2.4  2002/05/23 16:44:40  jmeade
// Issue 2270:  Draw list of 2D annotations; Cleaning up mangled functions.
//
// Revision 3.14.2.3  2002/05/18 00:18:04  jmeade
// Issue 2248: Implemented scrolling to absolute slice in axial orientations.
//
// Revision 3.14.2.2  2002/03/27 15:57:41  binli
// append to Revision 3.24.2.2: center arrow when zooming.
//
// Revision 3.14.2.1  2002/02/26 16:54:31  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.14  2002/01/24 22:34:16  binli
// issue 1578: condition to call ZoomAdjust()
//
// Revision 3.13  2002/01/22 17:10:29  binli
// issue 1547: non-mainview, disable 'flight along centerline'
//
// Revision 3.12  2001/12/21 23:59:28  jmeade
// Use a structure for rendered slice statistics.
//
// Revision 3.11  2001/12/21 03:53:09  jmeade
// Split off functionality that renders slice data into internal buffer (for use elsewhere than in RenderSlice).
//
// Revision 3.10  2001/11/20 18:35:16  binli
// 2D step flight ('A'/'Z')
//
// Revision 3.9  2001/11/16 22:30:40  binli
// new speed control to 2D flight
//
// Revision 3.8  2001/11/12 17:29:49  binli
// 2D (slice) flight: default choice: adjust viewport for more visible.
//
// Revision 3.7  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.6  2001/11/09 22:42:24  binli
// slice flight.
//
// Revision 3.5  2001/11/08 20:43:19  binli
// 2D flight
//
// Revision 3.4  2001/11/06 21:50:11  binli
// 2D flight along centerline
//
// Revision 3.3  2001/11/06 15:26:32  kevin
// Removed ses, set, and sec volumes
//
// Revision 3.2  2001/11/02 16:13:55  binli
// 2D render from block vol (.diff)
//
// Revision 3.1  2001/11/01 17:11:01  binli
// 2D render from one vol. (.ecv]
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:40:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:14   ingmar
// Initial revision.
// Revision 1.79  2001/08/22 20:49:22  sarang
// bug-fix: ZoomAdjust w/o type. gcc dosn't like this and this is not good coding!
//
// Revision 1.78  2001/08/22 01:17:18  jmeade
// no code change
//
// Revision 1.77  2001/08/21 20:23:46  binli
// bug 612: fixed
//
// Revision 1.76  2001/07/24 16:40:01  ana
// Added generation of registration input file.
//
// Revision 1.75  2001/07/05 14:38:33  binli
// used Rect in Draw2DOverviewSlice()
//
// Revision 1.74  2001/07/03 15:20:38  binli
// better 2D slice zooming.
//
// Revision 1.73  2001/06/27 15:08:15  jmeade
// coding conventions, const where appropriate, tabs to spaces
//
// Revision 1.72  2001/06/14 20:46:46  binli
// bug: direction of prone slice in EndoOrth/StandardView.
//
// Revision 1.71  2001/05/31 22:21:15  binli
// some works for code conventions and bug fixing (000489, 484)
//
// Revision 1.70  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.69  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.68  2001/04/11 11:22:48  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.67  2001/04/05 16:27:06  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.66  2001/04/04 14:16:43  binli
// some Point -> Point2D
//
// Revision 1.65  2001/04/02 20:08:51  binli
// reflected "Typedef.h"
//
// Revision 1.64  2001/03/30 22:13:06  binli
// int-> int4
// float->float4
//
// Revision 1.63  2001/03/27 21:33:20  binli
// continued code walkthrough.
//
// Revision 1.62  2001/03/27 15:12:29  binli
// reflected Typedef.h
//
// Revision 1.61  2001/03/20 18:50:24  binli
// code walkthrough to reflect new code standard:
// 1> moved some inline functions to .inl
// 2> changed some variables name;
// 3> changed some variables properties from public to protected or private.
// 4> new bracket rules.
//
// Revision 1.60  2001/03/16 23:16:26  binli
// renamed some variables according to new code standard.
//
// Revision 1.59  2001/03/13 15:49:08  binli
// code walkthrough
//
// Revision 1.58  2001/03/07 20:51:26  jmeade
// removed change for axial 512x512 handling (was not an error);
// removed check for supine/prone in LeftText() and RightText() (always the same)
//
// Revision 1.57  2001/02/28 21:03:44  jmeade
// ScrollTo() method (for xsec/view); tabs to spaces; const & and Rect<int4> where there should be
//
// Revision 1.56  2001/02/12 22:59:33  binli
// code cleaned
//
// Revision 1.55  2001/02/06 14:13:48  frank
// Revamped study load progress display.
//
// Revision 1.54  2001/02/02 17:21:53  liwei
// not to dispaly arrow in Verify View,
//
// Revision 1.53  2001/01/30 02:07:11  jeff
// ResetCurrentVolumePosition()
//
// Revision 1.52  2001/01/29 16:24:43  kevin
// Added scaling of arrow and bookmark size
// to the view rect
//
// Revision 1.51  2001/01/25 19:35:46  frank
// Removed pointless caching.
//
// Revision 1.50  2001/01/25 16:37:55  frank
// Changed zoomed picking.
//
// Revision 1.49  2001/01/25 16:04:46  frank
// Removed m_pStudy
//
// Revision 1.48  2001/01/24 18:19:04  binli
// added control to switch display 'blocked' or 'smoothed' slice images.
//
// Revision 1.47  2001/01/24 17:23:37  binli
// corrected measurement
//
// Revision 1.46  2001/01/23 17:43:23  jeff
// enums should be capitalized
//
// Revision 1.45  2001/01/19 14:07:29  frank
// Added pixel color map features to slice rendering.
//
// Revision 1.44  2001/01/17 14:20:29  frank
// Added ability to display biopsy transfer function on slices.
//
// Revision 1.43  2001/01/12 21:38:33  binli
// changed 'm_startMovePt' from uint to int4.
//
// Revision 1.42  2001/01/09 15:20:48  frank
// Cleaned up study orientations in slice view.
//
// Revision 1.41  2001/01/08 21:01:17  binli
// did 'blocked'(nearest), not linear interpolation within the  texel after zooming.
//
// Revision 1.40  2001/01/08 16:41:09  frank
// No reason to perform an unsafe operation here.
//
// Revision 1.39  2001/01/05 19:42:17  jeff
// namespace syntax error in GetVoxelRaw()
//
// Revision 1.38  2001/01/05 18:34:22  binli
// zooming control: zooming around an anchor point.
//
// Revision 1.37  2001/01/05 00:52:57  jeff
// select and zoom rect now uses Rect class for better encapsulation, corresponding
// functionality completely unchanged!!; show thumbnail only when panning
//
// Revision 1.36  2001/01/04 19:11:39  binli
// code adjust and clean.
//
// Revision 1.35  2001/01/04 17:51:54  binli
// zooming control (10): display 2D overview slice image when select an area for zooming.
//
// Revision 1.34  2001/01/04 16:13:48  binli
// zooming control (8): display 2D overview slice imageon the right-left corner.
// zooming control (9): grasp the 2D slice and move.
//
// Revision 1.33  2001/01/03 19:44:37  kevin
// put ins debugging back in
//
// Revision 1.32  2001/01/03 19:36:19  jeff
// GetZoomPercentage()
//
// Revision 1.31  2001/01/03 19:10:01  frank
// Cleaned up some utility functions.
//
// Revision 1.30  2001/01/03 15:04:07  frank
// Used Math library classes to eliminate variables.
//
// Revision 1.29  2000/12/27 23:03:35  jeff
// TRANSVERSE ==> AXIAL, CROSSECT ==> CROSS_SECTION
//
// Revision 1.28  2000/12/27 21:09:52  kevin
// Set arrow scale size according to IsMainViewWindow
//
// Revision 1.27  2000/12/27 20:22:08  binli
// zooming (6):make sure the arrow is visible  when double click in Overview.
//
// Revision 1.26  2000/12/27 17:52:20  kevin
// CHanges to remove Mprviewer and fix the oblique plane rendring in the overview
//
// Revision 1.25  2000/12/22 20:39:08  binli
// zooming: preliminary result(2): select area + keyboard control.
//
// Revision 1.24  2000/12/22 16:06:54  binli
// slice zooming: step 1: preliminary result.
//
// Revision 1.23  2000/12/21 16:33:50  kevin
// SMall steps towards mreging slicesview and mpr view
//
// Revision 1.22  2000/12/20 14:34:38  kevin
// Initial chnges to make mpr viewer and slice viewer one 2d slice viewer class
//
// Revision 1.21  2000/12/15 18:03:05  jeff
// store number of slices (based on orientation)
//
// Revision 1.20  2000/12/12 21:38:37  jeff
// Added SliceViewer::GetOrientation() method
//
// Revision 1.19  2000/11/24 15:15:26  kevin
// Added R/L/A/P/I/S marking to 2D images
//
// Revision 1.18  2000/11/21 22:58:02  liwei
// 3D arrow is blended to slices if it is facing inside-out
//
// Revision 1.17  2000/11/21 05:24:00  jeff
// Added m_bRenderCamera
//
// Revision 1.16  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.15  2000/11/10 17:53:14  liwei
// Added a pointer to Study in SliceViewer
//
// Revision 1.14  2000/11/09 19:31:04  liwei
// A bug of hybrid view fixed
//
// Revision 1.13  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.12  2000/11/07 18:21:38  liwei
// Modified for hybrid view
//
// Revision 1.11  2000/11/03 22:29:13  dave
// debug conversion to uint
//
// Revision 1.10  2000/11/03 20:32:12  dave
// converted back to unsigned dim.
//
// Revision 1.9  2000/11/03 19:34:52  frank
// Added PDR and CSI examiners
//
// Revision 1.8  2000/11/02 20:09:12  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.7  2000/10/31 23:22:37  liwei
// Hybrid view added
//
// Revision 1.6  2000/10/14 18:54:56  kevin
// Added IsAxial and bounded GetVoxelCoord if point
// is outside window
//
// Revision 1.5  2000/10/03 17:35:23  jeff
// Bug fixes in GetDisplayVoxelCoord()
//
// Revision 1.4  2000/09/30 02:57:55  jeff
// GetDisplayedVoxelCoord function
//
// Revision 1.3  2000/09/26 02:58:14  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.2  2000/09/16 01:52:39  jeff
// Changed slice access
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 12    8/28/00 12:39p Antonio
// updated header and footer
//
//*****************  Version 11  *****************
//User: Kevin           Date:  8/23/00  Time:  1:41a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Really, Really fixed otrthogonalPoint to work with viewpoint
//color displaying whether point matches the viewpoint
//
//*****************  Version 10  *****************
//User: Kevin           Date:  8/22/00  Time: 11:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed slice positioning to utilize "orthogonalPoint"
//
//*****************  Version 9  *****************
//User: Kevin           Date:  8/21/00  Time: 12:29p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added ddb debug display in overview images
//
//*****************  Version 8  *****************
//User: Kevin           Date:  8/20/00  Time:  8:12p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed the ins axial viewers to show colonr AND the original slice
//
//*****************  Version 7  *****************
//User: Jeff            Date:  8/09/00  Time:  1:37p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/08/00  Time: 10:31a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Positional jumps and positional dragging on the 2D axial views
//
//*****************  Version 5  *****************
//User: Kevin           Date:  8/04/00  Time:  5:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed some slice scrolling problems
//
//*****************  Version 4  *****************
//User: Kevin           Date:  7/21/00  Time:  8:37p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added CDC drawing to place symbology over the axial slices and the
//distance measurements
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/21/00  Time:  6:38p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added initial axial slice scrolling
//
//*****************  Version 2  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 2
//Comment:
//  
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created SliceViewer.h
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SliceViewer.h,v 1.8.2.1 2009/01/21 22:02:59 jmeade Exp $
// $Id: SliceViewer.h,v 1.8.2.1 2009/01/21 22:02:59 jmeade Exp $
