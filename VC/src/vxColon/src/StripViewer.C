// $Id: StripViewer.C,v 1.10.2.1 2010/02/01 19:32:58 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include <algorithm>
#include "Dataset.h"
#include "StripViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "Plane.h"
#include "Line.h"
#include "RaySegment.h"
#include "PrimitiveUtilities.h"


// defines
#define FILE_REVISION "$Revision: 1.10.2.1 $"


// namespaces
using namespace ReaderLib;
using namespace ReaderLib::SliceOrientations;



/**
 * Constructor
 */
StripViewer::StripViewer() :
m_image( PixelRGB<uint1>( 0, 0, 0 ) )
{
  Initialize();
} // StripViewer()


/**
 * Destructor
 */
StripViewer::~StripViewer()
{
} // ~StripViewer()


/**
 * Initialize this StripViewer.
 */
void StripViewer::Initialize()
{
  m_uDisplayWidth = m_uDisplayHeight = 0;
  m_eDatasetType = DatasetOrientations::ePRIMARY;

  m_bShowCleansed = true;

  m_fExtraLength = 0.0F;
  m_uSubSliceHeight = GetNextPowerOfTwo( rdrGlobal.GetSettings()->GetIntValue( VCSettings::ViewerXML(), VCSettings::CutRenderSize() ) );
  m_projection = StripProjection( * GetDataset(), m_uSubSliceHeight );
  m_image.Resize( Triple<uint4>( m_projection.GetImageWidthPixels(), m_uSubSliceHeight * 2, 0 ) );
  // m_image.Clear() is already called inside Resize()
  m_iCurrentPlaneAngle = -1;
  m_iDesiredPlaneAngle = 0;
  m_uCurrentSegmentIndex = -1;
  m_iImageClean = -1;
  m_bInvertImage = false;
  
  m_bRadialProjection = false;
  m_iCurrentColorScheme = -1;
} // Initialize()


/**
 * Sets up the OpenGL rendering context
 */
void StripViewer::SetupOpenGLContext()
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
void StripViewer::ResizeDisplay(const uint4 uWidth, const uint4 uHeight)
{
  // save the display size for later
  m_uDisplayWidth = uWidth;
  m_uDisplayHeight = (uHeight == 0) ? 1 : uHeight;

  // determine the maximum scroll position - guess for now until we get the real data
  m_projection.SetWindowWidth( uWidth );

} // ResizeDisplay()


/**
 * setup the viewport so the aspect ratio is correct
 *
 * @param   fLeft     the screen offset to the left of the window
 * @param   fBottom   the screen offset to the top of the window
 */
void StripViewer::SetViewport( const float4 fLeft, const float4 fBottom )
{

  // assume that the strip is wider than the window can show correctly
  // i.e., it will always fit the height and scroll to see the entire width

  // setup the viewport so the aspect ratio is correct
  float4 fSliceAspectRatio = static_cast< float4 >( m_image.GetDim().m_y ) / static_cast< float4 >( m_image.GetDim().m_x );
  float4 fDisplayAspectRatio = static_cast< float4 >( m_uDisplayHeight ) / static_cast< float4 >( m_uDisplayWidth );
  if ( fDisplayAspectRatio < fSliceAspectRatio )
  {
    // tall image
    throw ex::IndexOutOfBoundsException( LogRec( "Did not expect a strip this short - giving up!", "StripViewer", "SetViewport" ) );
  }
  else
  {
    // wide and short image
    m_viewport.Set( fLeft, fBottom, fLeft + m_uDisplayWidth, fBottom + m_uDisplayHeight );
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
int4 StripViewer::Scroll( const int4 iCount )
{
  Dataset & myDataset = * GetDataset();

  // scroll 20 degrees at a time, hence only 18 possible angles
  m_iDesiredPlaneAngle += iCount;
  while( m_iDesiredPlaneAngle > 17 ) m_iDesiredPlaneAngle -= 18;
  while( m_iDesiredPlaneAngle < 0  ) m_iDesiredPlaneAngle += 18;
  m_iImageClean = -1;
  return 0;
} // Scroll()


/**
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void StripViewer::SetupPlaneCorners( const int4 & iSegment, const float4 fRotationAngle )
{
  Dataset & myDataset = * GetDataset();

  // Get the two skeleton nodes at opposite ends of the current segment
  const uint4 uIndex1 = myDataset.m_straightSegmentNodes[ iSegment     ];
  const uint4 uIndex2 = myDataset.m_straightSegmentNodes[ iSegment + 1 ];
  Point<float4> endpoint1 = myDataset.m_joinedSkeleton.GetWorldPos( uIndex1 );
  Point<float4> endpoint2 = myDataset.m_joinedSkeleton.GetWorldPos( uIndex2 );
  const float4 fLength = ( endpoint2 - endpoint1 ).GetLength();
  
  // Calculate the line between these two points
  Normal<float4> nSegmentAxis( endpoint2 - endpoint1 );

  // Get the "up" vector of the first point in relation to the dataset. BUT, make sure it is orthogonal to the rotation axis
  Plane rotationPlane( endpoint1, nSegmentAxis );
  const Normal<float4> nUp = Normal<float4>( rotationPlane.GetProjectionParallelToPlane( Vector<float4>( myDataset.m_joinedSkeleton.GetSkelNode( uIndex1 ).m_vWorldUp ) ) );

  // Get the "up" vector for this image (i.e. may be rotated)
  Matrix<float4> m;
  float4 fAdjustedPlaneAngle = 20 * m_iCurrentPlaneAngle + fRotationAngle;
  while ( fAdjustedPlaneAngle >= 360.0 )
    fAdjustedPlaneAngle -= 360.0;
  m.Rotate( nSegmentAxis, fAdjustedPlaneAngle * M_PI / 180.0F );
  Vector<float4> vThisImageUp = m * static_cast< Vector3Df >( nUp );

  // Get the "normal" vector for this image (e.g., ray direction if raycasting)
  Normal<float4> nThisImageNormal = Normal<float4>(nSegmentAxis.Cross(vThisImageUp));
  
  // Compute 4 corners of the entire plane in world coordinates
  m_fExtraLength = 0;//( m_projection.GetMinimumSegmentLengthMM() < fLength ) ? 0 : ( ( m_projection.GetMinimumSegmentLengthMM() - fLength ) / 2.0F );
  Vector<float4> vExtraLength = Vector<float4>(nSegmentAxis)*m_fExtraLength;
  myDataset.m_vObliqueBottomLeft  = endpoint1 - vExtraLength + vThisImageUp * -m_projection.GetImageHeighthMM();
  myDataset.m_vObliqueBottomRight = endpoint2 + vExtraLength + vThisImageUp * -m_projection.GetImageHeighthMM();
  myDataset.m_vObliqueTopLeft     = endpoint1 - vExtraLength + vThisImageUp *  m_projection.GetImageHeighthMM();
  myDataset.m_vObliqueTopRight    = endpoint2 + vExtraLength + vThisImageUp *  m_projection.GetImageHeighthMM();
  myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();
  
  // save the slice definition for later volume rendering of the shadow
  myDataset.m_vObliqueLocation = endpoint1;
  myDataset.m_vObliqueOrientation = nThisImageNormal;
  myDataset.m_vObliqueUp = Normal<float4>(vThisImageUp);
  
} // SetupPlaneCorners()


/** 
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void StripViewer::CreateSliceImageFromVol( const int4 & iSegment, const uint4 uSubSliceNumber )
{
  Dataset & myDataset = * GetDataset();
  
  const uint4 uStartX = m_projection.GetSegmentBounds( iSegment ).first;
  const uint4 uWidthX = m_projection.GetSegmentBounds( iSegment ).second - uStartX;

  // Get the two skeleton nodes at opposite ends of the current segment
  const uint4 uIndex1 = myDataset.m_straightSegmentNodes[ iSegment     ];
  const uint4 uIndex2 = myDataset.m_straightSegmentNodes[ iSegment + 1 ];
  Point<float4> endpoint1 = myDataset.m_joinedSkeleton.GetWorldPos( uIndex1 );
  Point<float4> endpoint2 = myDataset.m_joinedSkeleton.GetWorldPos( uIndex2 );

  switch ( uSubSliceNumber )
  {
  case 0:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           static_cast< Vector<float4> >( myDataset.m_vObliqueOrientation ), myDataset.m_vObliqueUp,
                                                           & m_image.GetDataPtr()[0].R(), m_image.GetDim().m_x, m_image.GetDim().m_y, 0,
                                                           uStartX, 0, uWidthX, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, ! m_bShowCleansed, false, false, Normal3Df(), Point3Df() );
    break;
  case 1:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           & m_image.GetDataPtr()[0].R(), m_image.GetDim().m_x, m_image.GetDim().m_y, 0,
                                                           uStartX, m_uSubSliceHeight, uWidthX, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, ! m_bShowCleansed, false, false, Normal3Df(), Point3Df() );
    break;
  }

  // render the centerline
  for ( uint4 uIndex = uIndex1; uIndex <= uIndex2 - 1; ++ uIndex )
  {
    // get the centerline points
    Point3Df centerline1 = myDataset.m_joinedSkeleton.GetWorldPos( uIndex );
    Point3Df centerline2 = myDataset.m_joinedSkeleton.GetWorldPos( uIndex + 1 );

    // project to the image
    // uv are visible in the range (0,1)
    RaySegment leftRight( myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight );
    RaySegment bottomTop( myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueTopLeft );
    float u1 = PrimitiveUtilities::GetPercentAlongSegment( leftRight, centerline1 );
    float v1 = PrimitiveUtilities::GetPercentAlongSegment( bottomTop, centerline1 );
    float u2 = PrimitiveUtilities::GetPercentAlongSegment( leftRight, centerline2 );
    float v2 = PrimitiveUtilities::GetPercentAlongSegment( bottomTop, centerline2 );

    // render as points
    const int4 iStartY = ( uSubSliceNumber == 0 ) ? 0 : m_uSubSliceHeight;
    Point3Df pixel1( uStartX + uWidthX * u1, iStartY + (m_uSubSliceHeight-1) * v1, 0 );
    Point3Df pixel2( uStartX + uWidthX * u2, iStartY + (m_uSubSliceHeight-1) * v2, 0 );

    // render the line as a series of points
    const float4 fLength = ( pixel2 - pixel1 ).GetLength();
    for ( int4 i = 0; i < fLength; ++ i )
    {
      const float fPercent = i / fLength;
      Point3Df renderPosition = static_cast< Point3Df >( static_cast< Vector3Df >( pixel1 ) * ( 1 - fPercent ) +
                                                         static_cast< Vector3Df >( pixel2 ) *       fPercent );

      // if visible
      if ( renderPosition.m_x >= 0 && renderPosition.m_x <= m_image.GetDim().m_x - 1 &&
           renderPosition.m_y >= 0 && renderPosition.m_y <= m_image.GetDim().m_y - 1 )
      {
        // write pixel
        uint1 * pPixel = & m_image.GetDataPtr()[ static_cast< int4 >( renderPosition.m_y ) * m_image.GetDim().m_x ].R() +
                         3 * static_cast< int4 >( renderPosition.m_x );
        pPixel[0] = 0;
        pPixel[1] = 255;
        pPixel[2] = 0;
      } // if visible
    } // for all the pixels along the length of this centerline segment
  } // render the centerline

} // CreateSliceImageFromVol()


/**
 * Render the strip
 */
void StripViewer::RenderStrip(const float4 fLeft, const float4 fBottom)
{
  Dataset & myDataset = * GetDataset();
  SetViewport(fLeft, fBottom);

  //
  // First, check if the image needs to be rendered (if it is "dirty")
  //
  if ( m_iCurrentColorScheme != rdrGlobal.m_iColorScheme )
  {
    m_iCurrentColorScheme = rdrGlobal.m_iColorScheme;
    m_iImageClean = -1;
  }
  if ( m_lastOrthogonalPosition != myDataset.m_vOrthogonalPtWorld )
    m_lastOrthogonalPosition = myDataset.m_vOrthogonalPtWorld;

  //
  // Render the image if dirty
  //
  if ( m_iImageClean != m_projection.GetNumberSegments() - 1 )
  {
    if ( m_iImageClean == -1 )
      m_image.Clear();

    // render the next segment
    ++m_iImageClean;
    SetupPlaneCorners( m_iImageClean,   0 ); CreateSliceImageFromVol( m_iImageClean, 0 );
    SetupPlaneCorners( m_iImageClean, 180 ); CreateSliceImageFromVol( m_iImageClean, 1 );

    // possibly invert the image pixels
    if ( m_bInvertImage )
    {
      // swap red and blue bytes
      const uint4 uNumBytes = 3 * m_image.GetDim().m_x * m_image.GetDim().m_y;
      uint1 * pImageBuffer = & m_image.GetDataPtr()[0].R();
      for ( uint4 i = 0; i < uNumBytes; i += 3 )
        std::swap( pImageBuffer[ i ], pImageBuffer[ i + 2 ] );
    } // if inverted image
    
    // save the state
    m_iCurrentPlaneAngle = m_iDesiredPlaneAngle;
  }

  //
  // draw the image
  //
  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); 
  {
    gluOrtho2D(0, m_uDisplayWidth-1, 0, 1);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glEnable( GL_TEXTURE_2D );

    // do this in groups of the maximum texture size
    // do this also to improve performance by eliminating texture loads for off-screen portions
    const int4 iNumTextures = static_cast<float4>( m_image.GetDim().m_x ) / static_cast<float4>( m_projection.GetTextureSize() );
    for ( int i=iNumTextures-1; i>=0; i-- )
    {

      // determine the extents of visibility for this texture
      // the textures were rendered isotropically, so render the tiles that way on-screen
      const float4 fIsotropic = static_cast< float4 >( m_uDisplayHeight ) / static_cast< float4 >( m_image.GetDim().m_y );
      const float4 fStartPixel = fIsotropic * static_cast< float4 >( i * static_cast< float4 >( m_projection.GetTextureSize() ) - m_projection.GetScrollPosition() );
      const float4 fEndPixel = fStartPixel + fIsotropic * m_projection.GetTextureSize();

      // skip this texture if it's not on the screen right now
      if ( fStartPixel < m_uDisplayWidth && fEndPixel > 0 )
      {

        // store to texture
        glPixelStorei( GL_UNPACK_ROW_LENGTH, m_image.GetDim().m_x );
        GLvoid * pPixels = static_cast< GLvoid * >( & m_image.GetDataPtr()[ i * m_projection.GetTextureSize() ].R() );
        glTexImage2D( GL_TEXTURE_2D, 0, 3, m_projection.GetTextureSize(), m_image.GetDim().m_y, 0, GL_RGB, GL_UNSIGNED_BYTE, pPixels );

        glBegin( GL_QUADS );
        {
          glTexCoord2f( 0, 0 ); glVertex3f( fStartPixel, 0, -1 );
          glTexCoord2f( 1, 0 ); glVertex3f( fEndPixel  , 0, -1 );
          glTexCoord2f( 1, 1 ); glVertex3f( fEndPixel  , 1, -1 );
          glTexCoord2f( 0, 1 ); glVertex3f( fStartPixel, 1, -1 );
        }
        glEnd();

      } // if this tile is visible

    } // foreach texture tile

  } // glPushMatrix
  glPopMatrix();
  glFlush();

} // RenderSlice()


/**
 * set if the image pixels should be inverted (e.g., when taking a snapshot)
 * @param bInvertImage if the image pixels should be inverted
 */
void StripViewer::SetInvertImage( const bool & bInvertImage )
{
  if ( bInvertImage != m_bInvertImage )
    m_iImageClean = -1;

  m_bInvertImage = bInvertImage;
}


/**
 * set the scroll position
 *
 * @param uScrollPosition   the new scroll position
 */
void StripViewer::SetScrollPosition( const int4 iScrollPosition )
{
  if ( iScrollPosition == m_projection.GetScrollPosition() )
    return;

  m_projection.SetScrollPosition( min( m_projection.GetMaxScrollPosition(), max( 0, iScrollPosition ) ) );
} // SetScrollPosition()


/**
 * Setup the oblique plane shadow for subsequent rendering in the overview
 *
 * @param   point   the input screen position
 *
 * @return          the current segment number
 */
int4 StripViewer::SetupHalfPlaneShadow( const Point2D<int4> & point )
{

  // determine which segment we are over (based on x)
  const int4 iSegment = m_projection.GetSegment( point.m_x, m_uDisplayHeight, m_image.GetDim().m_y );

  // determine which half plane we are in (based on y)
  const int4 iRotationAngle = ( point.m_y < m_uDisplayHeight / 2 ) ? 0 : 180;
  SetupPlaneCorners( iSegment, iRotationAngle );

  return iSegment;

} // SetupHalfPlaneShadow()


/**
 * Get a picked location in world coordinates.
 */
Point<float4> StripViewer::GetPickedLocationWorldCoord( const float4 fPosX, const float4 fPosY,
                                                        Normal3D<float4> & viewDirectionWorld, Normal3D<float4> & viewUpWorld )
{
  return GetPickedLocationVolumeCoord( fPosX, fPosY, viewDirectionWorld, viewUpWorld ).GetScaledAnIso( GetDataset()->m_vUnits );
} // GetPickedLocationWorldCoord()


/**
 * Get a picked location in volume coordinates.
 */
Point<float4> StripViewer::GetPickedLocationVolumeCoord( const float4 fPosX, const float4 fPosY,
                                                         Normal3D<float4> & viewDirectionWorld, Normal3D<float4> & viewUpWorld )
{

  Point<float4> vPosVolumeCoord;

  // figure out which quadrant we are in and where the intersection point is in the image
  uint4 uSubSliceNumber = 0;
  Point3Df intersectionPoint;
  const int4 iSegment = m_projection.GetSegment( fPosX * m_uDisplayWidth, m_uDisplayHeight, m_image.GetDim().m_y );
  const int4 iTexturePixelX = m_projection.GetTexturePixelX( fPosX * m_uDisplayWidth, m_uDisplayHeight, m_image.GetDim().m_y );
  if ( fPosY < 0.5F )
  {
    uSubSliceNumber = 0;
    intersectionPoint = Point3Df( iTexturePixelX,
                                  Interpolate( 0.0F, fPosY, 0.5F, 0.0F, static_cast< float4 >( m_uSubSliceHeight - 1 ) ),
                                  0.0F );
    SetupPlaneCorners( iSegment, 0 );
  }
  else
  {
    uSubSliceNumber = 1;
    intersectionPoint = Point3Df( iTexturePixelX,
                                  Interpolate( 0.5F, fPosY, 1.0F, 0.0F, static_cast< float4 >( m_uSubSliceHeight - 1 ) ),
                                  0.0F );
    SetupPlaneCorners( iSegment, 180 );
  }
  Dataset & myDataset = * GetDataset();
  viewUpWorld = Normal3Df( myDataset.m_vObliqueTopLeft - myDataset.m_vObliqueBottomLeft );

  // we call the rendering with the "sample depth" parameter set to true
  // this will cause the rayCast function to simply cast one ray at the specified image
  // position and return the world position at 3D position the ray first intersects the colon wall
  const uint4 uStartX = m_projection.GetSegmentBounds( iSegment ).first;
  const uint4 uWidthX = m_projection.GetSegmentBounds( iSegment ).second - uStartX;
  // convert the intersection point into sub-image coordinates
  intersectionPoint.m_x -= uStartX;
  switch ( uSubSliceNumber )
  {
  case 0:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           & m_image.GetDataPtr()[0].R(), m_image.GetDim().m_x, m_image.GetDim().m_y, 0,
                                                           uStartX, 0, uWidthX, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, ! m_bShowCleansed, true, false, Normal3Df(), intersectionPoint );
    break;
  case 1:
    myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset, myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                           Vector<float4>(myDataset.m_vObliqueOrientation), myDataset.m_vObliqueUp,
                                                           & m_image.GetDataPtr()[0].R(), m_image.GetDim().m_x, m_image.GetDim().m_y, 0,
                                                           uStartX, 0, uWidthX, m_uSubSliceHeight-1,
                                                           m_bRadialProjection, ! m_bShowCleansed, true, false, Normal3Df(), intersectionPoint );
    break;
  default:
    LogWrn( "bad subslice number: " + ToAscii( uSubSliceNumber ), "StripViewer", "GetPickedLocationVolumeCoord" );
    break;
  }
  viewDirectionWorld = -myDataset.m_vObliqueOrientation;

  // convert from world to volume coordinates
  return intersectionPoint.GetDividedAnIso( myDataset.m_vUnits );

} // GetPickedLocationVolumeCoord()


/**
 * Obtain the current dataset
 *
 * @return the current dataset
 */
Dataset * StripViewer::GetDataset() const
{
  switch ( m_eDatasetType )
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
    LogWrn( "bad dataset case", "StripViewer", "GetDataset" );
    return & rdrGlobal.m_nullDataset;
    break;
  }

  throw ex::IllegalArgumentException( LogRec( "bad dataset case", "StripViewer", "GetDataset" ) );
  return & rdrGlobal.m_nullDataset;
} // GetDataset()


// $Log: StripViewer.C,v $
// Revision 1.10.2.1  2010/02/01 19:32:58  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10  2007/03/05 11:35:43  frank
// Working on stretched strip images
// Added centerline rendering
// Reversed strip segment direction
// Rendered strip incrementally to improve interactivity
//
// Revision 1.9  2007/02/17 17:43:07  frank
// synchronized 3D view when dragging left mouse button on strip
//
// Revision 1.8  2007/02/14 21:01:21  frank
// fixed up scrolling and segment selection
//
// Revision 1.7  2007/01/16 10:49:33  frank
// scaled the strip to get an idea of how it will look after scrolling is done
//
// Revision 1.6  2007/01/16 10:43:41  frank
// working on scrolling
//
// Revision 1.5  2007/01/04 06:52:24  frank
// refactoring
//
// Revision 1.4  2006/12/23 21:45:41  frank
// more refactoring
//
// Revision 1.3  2006/12/23 21:11:32  frank
// split out StripProjection class
//
// Revision 1.2  2006/12/23 20:55:46  frank
// refactoring
//
// Revision 1.1  2006/12/22 05:33:25  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/StripViewer.C,v 1.10.2.1 2010/02/01 19:32:58 dongqing Exp $
// $Id: StripViewer.C,v 1.10.2.1 2010/02/01 19:32:58 dongqing Exp $
