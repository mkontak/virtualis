// $Id: Segment3DViewer.h,v 1.11.2.1 2010/02/01 19:33:15 dongqing Exp $
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
#include "BoundingRect.h"


// namespaces
namespace ReaderLib {


// forward declarations
class VolumeRender;
class Dataset;


/**
 * A class to support viewing of segments along the centerline
 */
class Segment3DViewer
{
// Member functions
public:

  /// Constructor
  Segment3DViewer();

  /// Destructor
  ~Segment3DViewer();

  /// Copy constructor
  Segment3DViewer(const Segment3DViewer & other);

  /// Sets up the OpenGL rendering context
  void SetupOpenGLContext();

  /// Resizes the display window
  void ResizeDisplay(const uint4 uWidth, const uint4 uHeight);
  void SetViewport(const float4 fLeft, const float4 fBottom);

  /// Render the slice
  void RenderSegment(const float4 fLeft, const float4 fBottom);

  /// ??
  void SetupHalfPlaneShadow(const Point2D<int4> & point);

  // Adjust the current slice
  int4 Scroll(const int4 iCount);

  /// Get a picked location in various coordinate systems.
  Point<float4> GetPickedLocationWorldCoord( const float4 fPosX, const float4 fPosY, const bool & bActualDepth,
                                             Normal3D<float4> & viewDirectionWorld, Normal3D<float4> & viewUpWorld );
  Point<float4> GetPickedLocationVolumeCoord( const float4 fPosX, const float4 fPosY, const bool & bActualDepth,
                                              Normal3D<float4> & viewDirectionWorld, Normal3D<float4> & viewUpWorld );

  /// Retrieve the current dataset for this viewer.
  Dataset * GetDataset() const;

  /// sets the dirty flag to true so image will update on next render.
  void SetImageDirty();

  /// Retrieve the current dataset type for this viewer.
  inline DatasetOrientations::TypeEnum GetDatasetType() const;

  /// Get/Set the number of the current slice
  inline int4 GetCurrentPlaneAngle() const { return m_iCurrentPlaneAngle; }
  inline void SetDesiredPlaneAngle( const int4 iAngle );

  // switch between rendering of 'blocked' or 'smoothed' pixels.
  inline void SwitchBlockAndSmooth();

  /// Select the desired study orientation.
  inline void SetDatasetType(const DatasetOrientations::TypeEnum eOrientation);

  /// set the status of ShowUncleansed.
  inline void SetShowUncleansedFlag(const bool bShowUncleansed) { m_bShowUncleansed = bShowUncleansed; }
  /// get the status of ShowUncleansed.
  inline const bool IsShowUncleansedSet() const { return m_bShowUncleansed; }
  
  /// set if the image pixels should be inverted (e.g., when taking a snapshot)
  void SetInvertImage( const bool & bInvertImage );

  /// get if the image pixels should be inverted (e.g., when taking a snapshot)
  inline const bool & GetInvertImage() const { return m_bInvertImage; }

  /// select the segment nearest to the 3D (submarine) location
  void SelectBestSegment();

  /// construct slice image.
  void SetupPlaneCorners(const float4 fRotationAngle);

  /// gets ratio of quadtrant maximum width over actual
  inline float4 GetExtraLengthRatio() const { return m_fExtraLengthRatio; }

protected:

  void Initialize();

private:

  void CreateSliceImageFromVol(const uint4 uSubSliceNumber);

// Data members
public:

  // An enumerated type describing the dataset orientation
  DatasetOrientations::TypeEnum m_eDatasetType;

  // the dimension of display window.
  uint4 m_uDisplayWidth, m_uDisplayHeight;

  /// if the projection should be radial instead of orthographic
  bool m_bRadialProjection;

  /// if the image pixels should be inverted (e.g., when taking a snapshot)
  bool m_bInvertImage;

public:

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

  /// if the image is dirty
  bool m_bDirtyImage;
  
  uint4 m_uSliceWidth, m_uSliceHeight;
  uint4 m_uSubSliceWidth, m_uSubSliceHeight;
  uint4 m_uMaxSegmentLength;
  float4 m_fExtraLength;

  /// the actual viewport used for display to maintain the proper aspect ratio image
  BoundingRect<float4> m_viewport;

  /// the last rendered orthogonal position
  Point3Df m_lastOrthogonalPosition;

  /// the ratio the quadrant width over the actual required with
  float4 m_fExtraLengthRatio;

  /// plane angle after adjustment
  float4 m_fAdjustedPlaneAngle;

}; // class Segment3DViewer


#include "Segment3DViewer.inl"


} // namespace ReaderLib


// $Log: Segment3DViewer.h,v $
// Revision 1.11.2.1  2010/02/01 19:33:15  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11  2007/01/30 16:58:48  geconomos
// initial work on draw cad markings
//
// Revision 1.10  2007/01/10 18:31:46  jmeade
// Issue 5217: flag image for dirty on InvalidateView().
//
// Revision 1.9  2006/11/07 19:04:48  frank
// moved the segment selection to a separate function
//
// Revision 1.8  2006/10/31 21:42:54  frank
// working on synchronizing the segments to 3D
//
// Revision 1.7  2006/10/09 19:15:22  frank
// Issue #5048: Fixed scrollbar behavior of UD segments view
//
// Revision 1.6  2006/10/09 17:55:17  frank
// Issue #5048: updated the segment image when changing the color scheme
//
// Revision 1.5  2006/09/28 15:59:42  frank
// added image inversion (for taking snapshots)
//
// Revision 1.4  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.3  2006/06/01 20:25:30  frank
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
// Revision 1.4  2002/11/11 14:32:38  kevin
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Segment3DViewer.h,v 1.11.2.1 2010/02/01 19:33:15 dongqing Exp $
// $Id: Segment3DViewer.h,v 1.11.2.1 2010/02/01 19:33:15 dongqing Exp $
