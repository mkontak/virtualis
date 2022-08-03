// $Id: Segment3DViewer.C,v 1.12.2.3 2010/02/01 19:32:58 dongqing Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  Segment3DViewer.h
// Description: A class to support viewing of segments along the centerline
// Created: June 10, 2002
// Author: Kevin Kreeger  (kevin@viatronix.net)


// includes
#include "stdafx.h"
#include "Dataset.h"
#include "Segment3DViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "Plane.h"
#include "Line.h"


// defines
#define FILE_REVISION "$Revision: 1.12.2.3 $"


// namespaces
using namespace ReaderLib;
using namespace ReaderLib::SliceOrientations;


/**
 * Constructor
 */
Segment3DViewer::Segment3DViewer() :
m_puSliceImage( NULL )
{
  Initialize();
} // Constructor()


/**
 * Destructor
 */
Segment3DViewer::~Segment3DViewer()
{
  if (m_puSliceImage) delete [] m_puSliceImage;
} // Destructor().


/**
 * Copy constructor
 *
 * @param const Segment3DViewer & other   Source
 */
Segment3DViewer::Segment3DViewer(const Segment3DViewer & other) : m_puSliceImage(NULL)
{
  Initialize();
  *this = other;
} // Copy constructor().


/**
 * Initialize this Segment3DViewer.
 */
void Segment3DViewer::Initialize()
{
  m_uDisplayWidth = m_uDisplayHeight = 0;
  m_eDatasetType = DatasetOrientations::ePRIMARY;

  m_bBlockedPixel = false;
  m_bShowUncleansed = false;

  m_uMaxSegmentLength = 128;
  m_fExtraLength = 0.0;
  m_uSubSliceWidth = rdrGlobal.GetSettings()->GetIntValue( VCSettings::ViewerXML(), VCSettings::CutRenderSize() );
  m_uSubSliceHeight = m_uSubSliceWidth / 2;
  m_uSliceWidth = m_uSubSliceWidth*2;
  m_uSliceHeight = m_uSubSliceHeight*2;
  uint4 uSliceSize = m_uSliceWidth * m_uSliceHeight;
  if (m_puSliceImage)  delete [] m_puSliceImage;
  m_puSliceImage = new uint1 [ uSliceSize * 3 ];
  memset( m_puSliceImage, 0, uSliceSize * 3 );
  m_iCurrentPlaneAngle = -1;
  m_iDesiredPlaneAngle = 0;
  m_uCurrentSegmentIndex = -1;
  m_bDirtyImage = true;
  m_bInvertImage = false;
  
  m_bRadialProjection = false;
  m_fExtraLengthRatio = 1.0F;

} // Initialize().


/**
 * Sets up the OpenGL rendering context
 */
void Segment3DViewer::SetupOpenGLContext()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Initialize the projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
} // SetupOpenGLContext().


/**
 * Resizes the display window
 *
 * @param   width, height   dimension of the display window.
 */
void Segment3DViewer::ResizeDisplay(const uint4 uWidth, const uint4 uHeight)
{
  // save the display size for later
  m_uDisplayWidth = uWidth;
  m_uDisplayHeight = (uHeight == 0) ? 1 : uHeight;
} // ResizeDisplay()


/**
 * set viewport (tall narrow or wide short viewport: no good!!!)
 *
 * @param fLeft  
 * @param fBottom
 */
void Segment3DViewer::SetViewport(const float4 fLeft, const float4 fBottom)
{

  // setup the viewport so the aspect ratio is correct
  float4 fSliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
  float4 fDisplayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (fDisplayAspectRatio < fSliceAspectRatio) 
  {
    // tall and narrow slice
    int4 iViewportHeight = m_uDisplayHeight;
    int4 iViewportWidth = iViewportHeight / fSliceAspectRatio;
    int4 iMinX = (m_uDisplayWidth-iViewportWidth)/2 + fLeft;
    int4 iMinY = fBottom;
    m_viewport.Set( iMinX, iMinY, iMinX + iViewportWidth, iMinY + iViewportHeight );
  }
  else
  {
    // wide and short slice
    int4 iViewportWidth = m_uDisplayWidth;
    int4 iViewportHeight = iViewportWidth * fSliceAspectRatio;
    int4 iMinX = fLeft;
    int4 iMinY = (m_uDisplayHeight-iViewportHeight)/2 + fBottom;
    m_viewport.Set( iMinX, iMinY, iMinX + iViewportWidth, iMinY + iViewportHeight );
  } // endif(displayAspectRatio<sliceAspectRatio)

  glViewport( m_viewport.GetMinX(), m_viewport.GetMinY(), m_viewport.GetDimX(), m_viewport.GetDimY() );

} // SetViewport()


/**
 * scroll slice
 * 
 * @param count scroll amount;
 * @param bAutoFly  true when auto-(centerline)flight.
 * @return only used in 2D flights, -1: stop auto-fly
 */
int4 Segment3DViewer::Scroll(const int4 iCount)
{
  Dataset & myDataset = * GetDataset();

  // scroll 20 degrees at a time, hence only 18 possible angles
  m_iDesiredPlaneAngle += iCount;
  while(m_iDesiredPlaneAngle>17) m_iDesiredPlaneAngle -= 18;
  while(m_iDesiredPlaneAngle<0 ) m_iDesiredPlaneAngle += 18;
  m_bDirtyImage = true;
  return 0;
} // Scroll().


/** 
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void Segment3DViewer::SetupPlaneCorners( const float4 fRotationAngle )
{
  Dataset & myDataset = * GetDataset();

#ifdef _DEBUG
  if ( !myDataset.IsLoadingComplete() )
    return;
#endif

  // Get the two skeleton nodes at opposite ends of the current segment
  uint4 uIndex1 = myDataset.m_straightSegmentNodes[myDataset.m_currentSegment];
  uint4 uIndex2 = myDataset.m_straightSegmentNodes[myDataset.m_currentSegment+1];
  Point<float4> Point1 = myDataset.m_joinedSkeleton.GetWorldPos(uIndex1);
  Point<float4> Point2 = myDataset.m_joinedSkeleton.GetWorldPos(uIndex2);
  float4 fLength = (Vector<float4>(Point2-Point1)).GetLength();
  
  // Calculate the line between these two points
  Normal<float4> nSegmentAxis(Point2-Point1);

  // Get the "up" vector of the first point in relation to the dataset. BUT, make sure it is orthogonal to the rotation axis
  Plane rotationPlane(Point1,nSegmentAxis);
  Normal<float4> nUp = Normal<float4>(rotationPlane.GetProjectionParallelToPlane(Vector<float4>(myDataset.m_joinedSkeleton.GetSkelNode(uIndex1).m_vWorldUp)));
  Vector<float4> vUp(nUp);

  // Get the "up" vector for this image (i.e. may be rotated)
  Matrix<float4> m;
  m_fAdjustedPlaneAngle = 20 * m_iCurrentPlaneAngle + fRotationAngle;
  while ( m_fAdjustedPlaneAngle >= 360.0 )
    m_fAdjustedPlaneAngle -= 360.0;
  m.Rotate( nSegmentAxis, m_fAdjustedPlaneAngle * M_PI / 180.0F );
  Vector<float4> vThisImageUp = m * vUp;

  // Get the "normal" vector for this image (e.g. ray direction if raycasting)
  Normal<float4> nThisImageNormal = Normal<float4>(nSegmentAxis.Cross(vThisImageUp));
  
  m_fExtraLengthRatio = m_uMaxSegmentLength / fLength;
  // Compute 4 corners of the entire plane in world coordinates
  m_fExtraLength = (m_uMaxSegmentLength<fLength) ? 0 : ((m_uMaxSegmentLength-fLength)/2.0);
  Vector<float4> vExtraLength = Vector<float4>(nSegmentAxis)*m_fExtraLength;
  myDataset.m_vObliqueBottomLeft  = Point2 + vExtraLength + vThisImageUp * -40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueBottomRight = Point1 - vExtraLength + vThisImageUp * -40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueTopLeft     = Point2 + vExtraLength + vThisImageUp *  40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueTopRight    = Point1 - vExtraLength + vThisImageUp *  40;//m_fSizeWorldUnits;
  myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();
  
  // save the slice definition for later volume rendering of the shadow
  myDataset.m_vObliqueLocation = Point1;
  myDataset.m_vObliqueOrientation = nThisImageNormal;
  myDataset.m_vObliqueUp = Normal<float4>(vThisImageUp);
  
} // SetupPlaneCorners()


/** 
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void Segment3DViewer::CreateSliceImageFromVol( const uint4 uSubSliceNumber )
{
  Dataset & myDataset = * GetDataset();
  
  uint4 uPixelOffset = m_uSubSliceWidth * m_fExtraLength / m_uMaxSegmentLength;
  switch ( uSubSliceNumber )
  {
  case 0:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                           0, 0, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, m_bShowUncleansed, false, false, Normal3Df(), Point3Df() );
    break;
  case 1:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                           m_uSubSliceWidth, 0, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, m_bShowUncleansed, false, false, Normal3Df(), Point3Df() );
    break;
  case 2:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                           0, m_uSubSliceHeight, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, m_bShowUncleansed, false, false, Normal3Df(), Point3Df() );
    break;
  case 3:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                           m_uSubSliceWidth, m_uSubSliceHeight, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, m_bShowUncleansed, false, false, Normal3Df(), Point3Df() );
    break;
  }
} // CreateSliceImageFromVol()


/// select the segment nearest to the 3D (submarine) location
void Segment3DViewer::SelectBestSegment()
{

  // select the best segment and store it in the dataset
  float4 fMinDistance( (std::numeric_limits<float>::max)() );
  int4 iMinNodeIndex = 0;
  Dataset & myDataset = * GetDataset();
  for ( int i=0; i<myDataset.m_straightSegmentNodes.size() - 1; i++ )
  {
    Point3Df startPos = myDataset.m_joinedSkeleton.GetWorldPos( myDataset.m_straightSegmentNodes[ i ] );
    Point3Df endPos = myDataset.m_joinedSkeleton.GetWorldPos( myDataset.m_straightSegmentNodes[ i + 1 ] );
    Line line( startPos, endPos );
    const float4 fDistance = line.ComputeDistance( myDataset.m_submarine.GetPositionWorld() );
    if ( fDistance < fMinDistance )
    {
      fMinDistance = fDistance;
      iMinNodeIndex = i;
    }
  }

  // possibly update the segment
  if ( myDataset.m_currentSegment != iMinNodeIndex )
  {
    myDataset.m_currentSegment = iMinNodeIndex;

    // make sure we dirty the image
    m_bDirtyImage = true;
  }

} // SelectBestSegment()


/**
 * Render the slice
 */
void Segment3DViewer::RenderSegment(const float4 fLeft, const float4 fBottom)
{
  Dataset & myDataset = * GetDataset();
  SetViewport(fLeft, fBottom);

  /// Gets a buffer for the render slice
  if ( m_lastOrthogonalPosition != myDataset.m_vOrthogonalPtWorld )
  {
    m_lastOrthogonalPosition = myDataset.m_vOrthogonalPtWorld;

    // enable this to update the 3D segment position to the location every time you synchronize with 3D
    //SelectBestSegment();
    // the image is dirtied inside SelectBestSegment() if neccessary
  }
  if ( m_uCurrentSegmentIndex != myDataset.m_currentSegment )
  {
    m_uCurrentSegmentIndex = myDataset.m_currentSegment;
    m_bDirtyImage = true;
  }
  if ( m_bDirtyImage )
  {
    // clear the image
    const uint4 uPixelSize = 3 * m_uSliceWidth * m_uSliceHeight;
    memset( m_puSliceImage, 0, uPixelSize );

    // render
    SetupPlaneCorners(  0); CreateSliceImageFromVol(0);
    SetupPlaneCorners( 90); CreateSliceImageFromVol(1);
    SetupPlaneCorners(180); CreateSliceImageFromVol(2);
    SetupPlaneCorners(270); CreateSliceImageFromVol(3);

    // save the state
    m_iCurrentPlaneAngle = m_iDesiredPlaneAngle;
    m_bDirtyImage = false;
  }

  // possibly invert the image pixels
  if ( m_bInvertImage )
  {
    const uint4 uPixelSize = 3 * m_uSliceWidth * m_uSliceHeight;
    for ( uint4 i=0; i<uPixelSize; i+=3 )
      std::swap( m_puSliceImage[i], m_puSliceImage[i+2] );
  } // if inverted image
  
  // draw the image
  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); 
  {
    gluOrtho2D(0, 1, 0, 1);
    
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_uSliceWidth);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, m_uSliceWidth, m_uSliceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*) m_puSliceImage);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_bBlockedPixel ? GL_NEAREST : GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bBlockedPixel ? GL_NEAREST : GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glTranslatef(0,0,-1);

    glBegin(GL_QUADS);
    {
      glTexCoord2f(0,0); glVertex2f(0,0);
      glTexCoord2f(1,0); glVertex2f(1,0);
      glTexCoord2f(1,1); glVertex2f(1,1);
      glTexCoord2f(0,1); glVertex2f(0,1);
    }
    glEnd();

  } 
  glPopMatrix();
  glFlush();
} // RenderSlice()


/**
 * set if the image pixels should be inverted (e.g., when taking a snapshot)
 * @param bInvertImage if the image pixels should be inverted
 */
void Segment3DViewer::SetInvertImage( const bool & bInvertImage )
{
  if ( bInvertImage != m_bInvertImage )
    m_bDirtyImage = true;

  m_bInvertImage = bInvertImage;
} // SetInvertImage()


/**
 * ??
 * @param point   ??
 */
void Segment3DViewer::SetupHalfPlaneShadow( const Point2D<int4> & point )
{

  if ( point.m_x < m_uDisplayWidth/2 && point.m_y < m_uDisplayHeight/2 )
    SetupPlaneCorners( 180 );
  else if ( point.m_y < m_uDisplayHeight/2 )
    SetupPlaneCorners( 270 );
  else if ( point.m_x < m_uDisplayWidth/2 )
    SetupPlaneCorners(  0 );
  else
    SetupPlaneCorners( 90 );

} // SetupHalfPlaneShadow()


/**
 * Get a picked location in world coordinates.
 *
 * @param fPosX the X coordinate of 2D view
 * @param fPosY the Y coordiante of 2D view
 * @param bActualDepth ture if you need to pick the true depth along the ray, false if you need the start point of the ray
 * @param viewDirectionWorld the direction normal of the selected point from camera
 * @param viewUpWorld the Up directional normal
 */
Point<float4> Segment3DViewer::GetPickedLocationWorldCoord( const float4 fPosX, const float4 fPosY, const bool & bActualDepth,
                                                            Normal3D<float4> & viewDirectionWorld, Normal3D<float4> & viewUpWorld )
{
  return GetPickedLocationVolumeCoord( fPosX, fPosY, bActualDepth, viewDirectionWorld, viewUpWorld ).GetScaledAnIso( GetDataset()->m_vUnits );
} // GetPickedLocationWorldCoord()


/**
 * Get a picked location in volume coordinates.
 */
Point<float4> Segment3DViewer::GetPickedLocationVolumeCoord( const float4 fPosX, const float4 fPosY, const bool & bActualDepth,
                                                             Normal3D<float4> & viewDirectionWorld, Normal3D<float4> & viewUpWorld )
{
  //Point<float4> vPosVolumeCoord;

  // adjust the selection point to the actual viewport (which has been adjusted for the aspect ratio)
  const float4 fSelectionX = Bound( 0.0F, Interpolate( m_viewport.GetMinX(), fPosX * m_uDisplayWidth, m_viewport.GetMaxX(), 0.0F, 1.0F ), 1.0F );
  const float4 fSelectionY = Bound( 0.0F, Interpolate( m_viewport.GetMinY(), fPosY * m_uDisplayHeight, m_viewport.GetMaxY(), 0.0F, 1.0F ), 1.0F );

  // figure out which quadrant we are in and where the intersection point is in the image
  uint4 uSubSliceNumber = 0;
  Point3Df intersectionPoint;
  if ( fSelectionX < 0.5F && fSelectionY < 0.5F )
  {
    uSubSliceNumber = 2;
    intersectionPoint = Point3Df( Interpolate( 0.0F, fSelectionX, 0.5F, 0.0F, static_cast< float4 >( m_uSubSliceWidth - 1 ) ),
                                  Interpolate( 0.0F, fSelectionY, 0.5F, static_cast< float4 >( m_uSubSliceHeight - 1 ), 0.0F ),
                                  0.0F );
    SetupPlaneCorners( 180 );
  }
  else if ( fSelectionY < 0.5F )
  {
    uSubSliceNumber = 3;
    intersectionPoint = Point3Df( Interpolate( 0.5F, fSelectionX, 1.0F, 0.0F, static_cast< float4 >( m_uSubSliceWidth - 1 ) ),
                                  Interpolate( 0.0F, fSelectionY, 0.5F, static_cast< float4 >( m_uSubSliceHeight - 1 ), 0.0F ),
                                  0.0F );
    SetupPlaneCorners( 270 );
  }
  else if ( fSelectionX < 0.5F )
  {
    uSubSliceNumber = 0;
    intersectionPoint = Point3Df( Interpolate( 0.0F, fSelectionX, 0.5F, 0.0F, static_cast< float4 >( m_uSubSliceWidth - 1 ) ),
                                  Interpolate( 0.5F, fSelectionY, 1.0F, m_uSubSliceHeight - 1, 0.0F ),
                                  0.0F );
    SetupPlaneCorners( 0 );
  }
  else
  {
    uSubSliceNumber = 1;
    intersectionPoint = Point3Df( Interpolate( 0.5F, fSelectionX, 1.0F, 0.0F, static_cast< float4 >( m_uSubSliceWidth - 1 ) ),
                                  Interpolate( 0.5F, fSelectionY, 1.0F, m_uSubSliceHeight - 1, 0.0F ),
                                  0.0F );
    SetupPlaneCorners( 90 );
  }
  Dataset & myDataset = * GetDataset();
  viewUpWorld = Normal3Df( myDataset.m_vObliqueTopLeft - myDataset.m_vObliqueBottomLeft );

  // we call the rendering with the "sample depth" parameter set to true
  // this will cause the rayCast function to simply cast one ray at the specified image
  // position and return the world position at 3D position the ray first intersects the colon wall
  uint4 uPixelOffset = m_uSubSliceWidth * m_fExtraLength / m_uMaxSegmentLength;
  switch ( uSubSliceNumber )
  {
  case 0:
    if( !bActualDepth )
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             0, 0, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, false, Normal3Df(), intersectionPoint );
    }
    else
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             0, 0, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, true, Normal3Df(), intersectionPoint );
    }
    break;
  case 1:
    if( !bActualDepth )
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             m_uSubSliceWidth, 0, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, false, Normal3Df(), intersectionPoint );
    }
    else
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             m_uSubSliceWidth, 0, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, true, Normal3Df(), intersectionPoint );
    }
    break;
  case 2:
    if( !bActualDepth )
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             0, m_uSubSliceHeight, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, false, Normal3Df(), intersectionPoint );
    }
    else
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             0, m_uSubSliceHeight, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, true, Normal3Df(), intersectionPoint );
    }
    break;
  case 3:
    if( !bActualDepth )
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             m_uSubSliceWidth, m_uSubSliceHeight, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, false, Normal3Df(), intersectionPoint );
    }
    else
    {
      myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                             Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                             m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                             m_uSubSliceWidth, m_uSubSliceHeight, m_uSubSliceWidth-1, m_uSubSliceHeight-1,
                                                             m_bRadialProjection, m_bShowUncleansed, true, true, Normal3Df(), intersectionPoint );
    }
    break;
  }
  viewDirectionWorld = myDataset.m_vObliqueOrientation;

  // convert from world to volume coordinates
  return intersectionPoint.GetDividedAnIso( myDataset.m_vUnits );
} // GetPickedLocationVolumeCoord()


/**
 * Retrieve the current dataset for this viewer.
 * @return   the dataset for this viewer
 */
Dataset * Segment3DViewer::GetDataset() const
{
  switch (m_eDatasetType) 
  {
  case DatasetOrientations::eSUPINE:
    return & rdrGlobal.m_supine;
    break;
  case DatasetOrientations::ePRONE:
    return & rdrGlobal.m_prone;
    break;
  case DatasetOrientations::ePRIMARY:
    return rdrGlobal.m_pCurrDataset;
    break;
  case DatasetOrientations::eSECONDARY:
    return rdrGlobal.m_pSecondaryDataset;
    break;
  default:
    LogRec("bad dataset case","Segment3DViewer", "GetDataset");
    return & rdrGlobal.m_nullDataset;
    break;
  }
  ex::IllegalArgumentException(LogRec("bad dataset case","Segment3DViewer", "GetDataset"));
  return & rdrGlobal.m_nullDataset;
} // GetDataset()


/**
 * sets the dirty flag to true so image will update on next render.
 */
void Segment3DViewer::SetImageDirty()
{
  m_bDirtyImage = true;
} // SetImageDirty()


// $Log: Segment3DViewer.C,v $
// Revision 1.12.2.3  2010/02/01 19:32:58  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12.2.2  2010/01/31 02:15:57  dongqing
// debug code
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12.2.1  2010/01/29 22:56:29  dongqing
// comment out unused code line
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12  2007/01/30 16:58:49  geconomos
// initial work on draw cad markings
//
// Revision 1.11  2007/01/10 18:31:46  jmeade
// Issue 5217: flag image for dirty on InvalidateView().
//
// Revision 1.10  2006/11/07 19:05:12  frank
// moved the segment selection to a separate function
//
// Revision 1.9  2006/10/31 21:42:54  frank
// working on synchronizing the segments to 3D
//
// Revision 1.8  2006/10/09 19:16:25  frank
// Issue #5048: Fixed scrollbar behavior of UD segments view
//
// Revision 1.7  2006/10/09 17:55:07  frank
// Issue #5048: updated the segment image when changing the color scheme
//
// Revision 1.6  2006/09/28 15:59:42  frank
// added image inversion (for taking snapshots)
//
// Revision 1.5  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.4  2006/09/19 18:49:11  frank
// rotated faster
//
// Revision 1.3  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.2  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.7.2.1.4.1  2001/01/02 11:23:40  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 1.7.2.1  2004/04/07 21:10:13  jmeade
// Removing unused enums.
//
// Revision 1.7  2003/01/24 20:26:25  jmeade
// Plane is no longer a template class.
//
// Revision 1.6  2003/01/22 22:26:54  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.5  2002/11/24 17:49:51  kevin
// Renamed ProjectOntoPlane
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Segment3DViewer.C,v 1.12.2.3 2010/02/01 19:32:58 dongqing Exp $
// $Id: Segment3DViewer.C,v 1.12.2.3 2010/02/01 19:32:58 dongqing Exp $

