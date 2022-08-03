// $Id: StripViewer.h,v 1.7 2007/03/05 11:35:44 frank Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
// 
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "StudyLoad.h"
#include "Point.h"
#include "Rect.h"
#include "BoundingRect.h"
#include "Image.h"
#include "StripProjection.h"


// namespaces
namespace ReaderLib {


// forward declarations
class VolumeRender;
class Dataset;


/**
 * A class to support viewing of all the segments along the colon centerline
 */
class StripViewer
{
// member functions
public:

// TODO debug remove me
std::string message;

  /// constructor
  StripViewer();

  /// destructor
  virtual ~StripViewer();

  /// sets up the OpenGL rendering context
  void SetupOpenGLContext();

  /// resizes the display window
  void ResizeDisplay( const uint4 uWidth, const uint4 uHeight );

  /// set the display viewport
  void SetViewport( const float4 fLeft, const float4 fBottom );

  /// render the strip
  void RenderStrip( const float4 fLeft, const float4 fBottom );

  /// setup the image plane shadow
  int4 SetupHalfPlaneShadow( const Point2D<int4> & point );

  /// adjust the current slice
  int4 Scroll( const int4 iCount );

  /// get a picked location in the world coordinate system
  Point<float4> GetPickedLocationWorldCoord( const float4 fPosX, const float4 fPosY,
                                             Normal3D<float4> & viewDirectionWorld,
                                             Normal3D<float4> & viewUpWorld );

  /// get a picked location in the volume coordinate system
  Point<float4> GetPickedLocationVolumeCoord( const float4 fPosX, const float4 fPosY,
                                              Normal3D<float4> & viewDirectionWorld,
                                              Normal3D<float4> & viewUpWorld );

  /// retrieve the current study for this viewer
  Dataset * GetDataset() const;

  /// retrieve the current dataset type for this viewer
  inline DatasetOrientations::TypeEnum GetDatasetType() const;

  /// get the number of the current slice
  inline int4 GetCurrentPlaneAngle() const { return m_iCurrentPlaneAngle; }

  /// set the number of the current slice
  inline void SetDesiredPlaneAngle( const int4 iAngle );

  /// select the desired study orientation
  inline void SetDatasetType( const DatasetOrientations::TypeEnum eOrientation );

  /// set the status of ShowCleansed
  inline void SetShowCleansedFlag( const bool bShowCleansed ) { m_bShowCleansed = bShowCleansed; }

  /// get the status of ShowUncleansed
  inline const bool IsShowCleansedSet() const { return m_bShowCleansed; }
  
  /// set if the image pixels should be inverted (e.g., when taking a snapshot)
  void SetInvertImage( const bool & bInvertImage );

  /// get if the image pixels should be inverted (e.g., when taking a snapshot)
  inline const bool & GetInvertImage() const { return m_bInvertImage; }

  /// get the current scroll position
  const int4 GetScrollPosition() const { return m_projection.GetScrollPosition(); }

  /// get the maximum scroll position
  const int4 GetMaxScrollPosition() const { return m_projection.GetMaxScrollPosition(); }

  /// set the scroll position
  void SetScrollPosition( const int4 iScrollPosition );

protected:

  /// inialize the view
  void Initialize();

private:

  /// setup the rendering parameters
  void SetupPlaneCorners( const int4 & iSegment, const float4 fRotationAngle );

  /// construct a slice image
  void CreateSliceImageFromVol( const int4 & iSegment, const uint4 uSubSliceNumber );

  /// copy constructor not implemented
  StripViewer( const StripViewer & );

// data members
public:

  /// an enumerated type describing the dataset orientation
  DatasetOrientations::TypeEnum m_eDatasetType;

  /// the width of the display window
  uint4 m_uDisplayWidth;

  /// the width of the display window
  uint4 m_uDisplayHeight;

  /// if the projection should be radial instead of orthographic
  bool m_bRadialProjection;

  /// if the image pixels should be inverted (e.g., when taking a snapshot)
  bool m_bInvertImage;

private:

  /// the image buffer
  Image< PixelRGB<uint1> > m_image;

  /// current slice No of the slice.
  int4 m_iCurrentPlaneAngle;
  
  /// current slice No of the slice.
  int4 m_iDesiredPlaneAngle;
  
  /// if should show cleansed or uncleansed
  bool m_bShowCleansed;

  /// currently displayed straight segment index
  uint4 m_uCurrentSegmentIndex;

  /// -1 if the image is dirty, == number of segments if clean
  int4 m_iImageClean;
  
  /// the width of the subimage within the main image
  uint4 m_uSubSliceWidth;

  /// the height of the subimage within the main image
  uint4 m_uSubSliceHeight;

  /// the extra length at the ends of a segment
  float4 m_fExtraLength;

  /// the actual viewport used for display to maintain the proper aspect ratio image
  BoundingRect<float4> m_viewport;

  /// the color scheme of the current image
  int4 m_iCurrentColorScheme;

  /// the last rendered orthogonal position
  Point3Df m_lastOrthogonalPosition;

  /// the projection between the segments and the strip
  StripProjection m_projection;

}; // class StripViewer


} // namespace ReaderLib


// $Log: StripViewer.h,v $
// Revision 1.7  2007/03/05 11:35:44  frank
// Working on stretched strip images
// Added centerline rendering
// Reversed strip segment direction
// Rendered strip incrementally to improve interactivity
//
// Revision 1.6  2007/02/14 21:01:21  frank
// fixed up scrolling and segment selection
//
// Revision 1.5  2007/01/16 10:44:14  frank
// working on scrolling
//
// Revision 1.4  2006/12/23 21:45:41  frank
// more refactoring
//
// Revision 1.3  2006/12/23 21:11:32  frank
// split out StripProjection class
//
// Revision 1.2  2006/12/23 20:55:47  frank
// refactoring
//
// Revision 1.1  2006/12/22 05:33:25  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/StripViewer.h,v 1.7 2007/03/05 11:35:44 frank Exp $
// $Id: StripViewer.h,v 1.7 2007/03/05 11:35:44 frank Exp $
