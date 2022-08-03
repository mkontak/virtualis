// $Id: Segment2DViewer.h,v 1.3 2006/06/01 20:25:16 frank Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger  (kevin@viatronix.net)


// pragmas
#pragma once


// includes
#include "StudyLoad.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "SliceViewer.h"


namespace ReaderLib
{

class VolumeRender;
class Dataset;


// Description: A class to support viewing of segments along the centerline
class Segment2DViewer
{
// Member functions
public:

  /// Constructor
  Segment2DViewer();

  /// Destructor
  ~Segment2DViewer();

  /// Copy constructor
  Segment2DViewer(const Segment2DViewer & other);

  /// Sets up the OpenGL rendering context
  void SetupOpenGLContext();

  /// Resizes the display window
  void ResizeDisplay(const uint4 uWidth, const uint4 uHeight);
  void SetViewport(const float4 fLeft, const float4 fBottom);

  /// Render the slice
  void RenderSegment(const float4 fLeft, const float4 fBottom);
  
  /// Adjust the current slice
  int4 Scroll(const int4 iCount);

  /// Get a picked location in various coordinate systems.
  Point<float4> GetPickedLocationWorldCoord(const float4 fPosX, const float4 fPosY) const;
  Point<float4> GetPickedLocationVolumeCoord(const float4 fPosX, const float4 fPosY) const;

  /// Retrieve the current study for this viewer.
  Dataset * GetDataset() const;

  /// Retrieve the current dataset type for this viewer.
  inline DatasetOrientations::TypeEnum GetDatasetType() const;

  /// Get/Set the number of the last slice for caching purposes.
  inline int4 GetCurrentPlaneAngle() const;
  inline void SetDesiredPlaneAngle(const int4 iAngle);
  inline void ResetAngle();

  // switch between rendering of 'blocked' or 'smoothed' pixels.
  inline void SwitchBlockAndSmooth();

  /// Select the desired study orientation.
  inline void SetDatasetType(const DatasetOrientations::TypeEnum eOrientation);

  /// Get zoom factor.
  inline const int4 GetZoomPercentage() const;

  /// set/get the status of ShowUncleansed.
  inline void SetShowUncleansedFlag(const bool bShowUncleansed) { m_bShowUncleansed = bShowUncleansed; }
  inline const bool IsShowUncleansedSet() const { return m_bShowUncleansed; }
  
protected:

  void Initialize();

private:

  /// construct slice image.
  void CreateSliceImageFromVol(const int4 iCurrentPlaneAngle, const bool bShowUncleansed);

// Data members
//-------------
public:
  // An enumerated type describing the dataset orientation
  DatasetOrientations::TypeEnum m_eDatasetType;

  // the dimension of display window.
  uint4 m_uDisplayWidth, m_uDisplayHeight;

  // start point and end point to mark a select area for zooming.
  Rect<uint4> m_rSelect;   // left-top and right-bottom corner.

  // anchor point that controls the moving of for 2D slice. 
  Point2D<int4> m_viStartMovePt;

  // anchor point for zooming around it. 
  Point2D<float4> m_vfAnchorPt;

  MeasureLineList m_measureLineList;
  MeasureAreaList m_measureAreaList;
  
private:

  // the iamge buffer (Axial/Sagittal/Coronal: basic size 512X512)
  uint1 * m_puSliceImage;

  // current slice No of the slice.
  int4 m_iCurrentPlaneAngle;
  
  // current slice No of the slice.
  int4 m_iDesiredPlaneAngle;

  // Rendering 'blocked' or 'smoothed' pixels.
  bool m_bBlockedPixel;

  /// flag: show uncleansed ...
  bool m_bShowUncleansed;

  /// Currently displayed straight segment index
  uint4 m_uCurrentSegmentIndex;

  uint4 m_uSliceWidth, m_uSliceHeight, m_uSliceSize;
  uint4 m_uMaxSegmentLength;

}; // class Segment2DViewer

#include "Segment2DViewer.inl"

} // namespace ReaderLib


// $Log: Segment2DViewer.h,v $
// Revision 1.3  2006/06/01 20:25:16  frank
// updated to visual studio 2005
//
// Revision 1.2  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.4.2.1  2004/04/07 21:10:13  jmeade
// Removing unused enums.
//
// Revision 1.4  2002/11/11 14:32:37  kevin
// Dim the border for segmetnal view
//
// Revision 1.3  2002/09/02 20:38:15  kevin
// Render a specific sized segment and mark which part the user should focus on
//
// Revision 1.2  2002/08/28 19:21:40  kevin
// Lots of updates to segmental viewing
//
// Revision 1.1  2002/08/20 19:21:50  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:53:52  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Segment2DViewer.h,v 1.3 2006/06/01 20:25:16 frank Exp $
// $Id: Segment2DViewer.h,v 1.3 2006/06/01 20:25:16 frank Exp $
