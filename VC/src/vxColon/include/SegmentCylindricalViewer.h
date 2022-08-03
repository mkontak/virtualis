// $Id: SegmentCylindricalViewer.h,v 1.6 2006/09/26 20:04:55 frank Exp $
//
// Copyright © 2002-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)


// pragmas
#pragma once


// includes
#include "StudyLoad.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "SliceViewer.h"


// namespaces
namespace ReaderLib {

class VolumeRender;
class Dataset;

/// A class to support viewing of segments along the centerline
class SegmentCylindricalViewer
{
// Member functions
public:

  /// Constructor
  SegmentCylindricalViewer();

  /// Destructor
  ~SegmentCylindricalViewer();

  /// Copy constructor
  SegmentCylindricalViewer(const SegmentCylindricalViewer & other);

  /// Sets up the OpenGL rendering context
  void SetupOpenGLContext();

  /// Resizes the display window
  void ResizeDisplay(const uint4 uWidth, const uint4 uHeight);
  void SetViewport(const float4 fLeft, const float4 fBottom);

  /// Render the slice
  void RenderSegment(const float4 fLeft, const float4 fBottom);
  void SetupHalfPlaneShadow(const Point2D<int4> & point);

  // Adjust the current slice
  int4 Scroll(const int4 iCount);

  /// Get a picked location in world coordinate
  Point<float4> GetPickedLocationWorldCoord(const float4 fPosX, const float4 fPosY, Normal3D<float4> & viewDirection) const;

  /// Get a picked location in volume coordinates
  Point<float4> GetPickedLocationVolumeCoord(const float4 fPosX, const float4 fPosY, Normal3D<float4> & viewDirection) const;

  /// Retrieve the current study for this viewer.
  Dataset * GetDataset() const;

  /// Retrieve the current dataset type for this viewer.
  inline ReaderLib::DatasetOrientations::TypeEnum GetDatasetType() const { return GetDataset()->GetOrientation(); }

  /// Get the current plane angle
  inline int4 GetCurrentPlaneAngle() const { return m_iCurrentPlaneAngle; }

  /// Set the current plane angle
  inline void SetDesiredPlaneAngle( const int4 iAngle ) { m_iDesiredPlaneAngle = iAngle; }

  /// Set the number of the last slice
  inline void ResetAngle() { m_iDesiredPlaneAngle = 0; }

  // switch between rendering of 'blocked' or 'smoothed' pixels
  inline void SwitchBlockAndSmooth() { m_bBlockedPixel = ! m_bBlockedPixel; }

  /// Select the desired study orientation
  inline void SetDatasetType(const ReaderLib::DatasetOrientations::TypeEnum eOrientation) { m_eDatasetType = eOrientation; }

  /// Get zoom factor
  inline const int4 GetZoomPercentage() const { return int(1); }

  /// set the status of ShowUncleansed
  inline void SetShowUncleansedFlag(const bool bShowUncleansed) { m_bShowUncleansed = bShowUncleansed; }

  /// get the status of ShowUncleansed
  inline const bool IsShowUncleansedSet() const { return m_bShowUncleansed; }
  
protected:

  void Initialize();

private:

  /// construct slice image.
  void SetupPlaneCorners(const float4 fRotationAngle);
  void CreateImageFromVol();

// Data members
public:
 
  // An enumerated type describing the dataset orientation
  ReaderLib::DatasetOrientations::TypeEnum m_eDatasetType;

  // the dimension of display window.
  uint4 m_uDisplayWidth, m_uDisplayHeight;

  bool m_bRadialProjection;

private:
  // the image buffer (Axial/Sagittal/Coronal: basic size 512X512)
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
  uint4 m_uSubSliceWidth, m_uSubSliceHeight, m_uSubSliceSize;
  uint4 m_uMaxSegmentLength;
  float4 m_fExtraLength;

}; // class SegmentCylindricalViewer


// namespaces
} // namespace ReaderLib


// $Log: SegmentCylindricalViewer.h,v $
// Revision 1.6  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.5  2006/09/22 18:27:47  frank
// removed unused inline file
//
// Revision 1.4  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.3  2006/06/01 20:25:30  frank
// updated to visual studio 2005
//
// Revision 1.2  2006/01/31 14:44:38  frank
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
// Revision 1.1.2.1  2004/04/07 21:10:13  jmeade
// Removing unused enums.
//
// Revision 1.1  2002/11/11 14:29:55  kevin
// Initial version -- still needs some polishing
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SegmentCylindricalViewer.h,v 1.6 2006/09/26 20:04:55 frank Exp $
// $Id: SegmentCylindricalViewer.h,v 1.6 2006/09/26 20:04:55 frank Exp $
