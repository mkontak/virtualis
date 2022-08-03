// $Id: SegmentCylindricalViewer.C,v 1.4.2.1 2010/02/01 19:32:58 dongqing Exp $
//
// Copyright © 2002-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)

//
// A class to support viewing of segments along the centerline
//


// includes
#include "stdafx.h"
#include <GL/gl.h>
#include <stack>
#include "Dataset.h"
#include "SegmentCylindricalViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "Plane.h"


// defines
#define FILE_REVISION "$Revision: 1.4.2.1 $"


// namespaces
using namespace ReaderLib;
using namespace ReaderLib::SliceOrientations;


/**
 * Constructor
 */
SegmentCylindricalViewer::SegmentCylindricalViewer() : m_puSliceImage(NULL)
{
  Initialize();
} // Constructor()


/**
 * Destructor
 */
SegmentCylindricalViewer::~SegmentCylindricalViewer()
{
  if ( m_puSliceImage != NULL )
  {
    delete [] m_puSliceImage;
    m_puSliceImage = NULL;
  }
} // Destructor()


/**
 * Copy constructor
 *
 * @param const SegmentCylindricalViewer & other   Source
 */
SegmentCylindricalViewer::SegmentCylindricalViewer(const SegmentCylindricalViewer & other) : m_puSliceImage(NULL)
{
  Initialize();
  *this = other;
} // Copy constructor()


/**
 * Initialize this SegmentCylindricalViewer.
 */
void SegmentCylindricalViewer::Initialize()
{
  m_uDisplayWidth = m_uDisplayHeight = 0;
  m_eDatasetType = DatasetOrientations::ePRIMARY;

  m_bBlockedPixel = false;
  m_bShowUncleansed = false;

  m_uMaxSegmentLength = 128;
  m_fExtraLength = 0.0;
  m_uSubSliceWidth = 256;
  m_uSubSliceHeight = 128;
  m_uSubSliceSize = m_uSubSliceWidth * m_uSubSliceHeight;
//  m_uSliceWidth = m_uSubSliceWidth;
//  m_uSliceHeight = m_uSubSliceHeight*4;
  m_uSliceWidth = m_uSubSliceWidth*2;
  m_uSliceHeight = m_uSubSliceHeight*2;
  m_uSliceSize = m_uSliceWidth * m_uSliceHeight;
  if ( m_puSliceImage != NULL )
  {
    delete [] m_puSliceImage;
    m_puSliceImage = NULL;
  }
  m_puSliceImage = new uint1 [m_uSliceSize*3];
  memset(m_puSliceImage,0,m_uSliceSize*3);
  m_iCurrentPlaneAngle = -1;
  m_iDesiredPlaneAngle = 0;
  m_uCurrentSegmentIndex = -1;
  
  m_bRadialProjection = false;
} // Initialize()


/**
 * Sets up the OpenGL rendering context
 */
void SegmentCylindricalViewer::SetupOpenGLContext()
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
void SegmentCylindricalViewer::ResizeDisplay(const uint4 uWidth, const uint4 uHeight)
{
  // save the display size for later
  m_uDisplayWidth = uWidth;
  m_uDisplayHeight = (uHeight == 0) ? 1 : uHeight;
} // ResizeDisplay().


/**
 * set viewport (tall narrow or wide short viewport: no good!!!)
 *
 * @param fLeft  
 * @param fBottom
 */
void SegmentCylindricalViewer::SetViewport(const float4 fLeft, const float4 fBottom)
{
  /// no, no, this viewport setting is definitely no-good -> lots of problems may cause.

  // setup the viewport so the aspect ratio is correct
  float4 fSliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
//  float4 fSliceAspectRatio = float4(m_uSliceWidth) / float4(m_uSliceHeight);
  float4 fDisplayAspectRatio = float4(m_uDisplayHeight) / float4(m_uDisplayWidth);
  if (fDisplayAspectRatio < fSliceAspectRatio) 
  {
    // tall and narrow slice
    int4 iViewportHeight = m_uDisplayHeight;
    int4 iViewportWidth = iViewportHeight / fSliceAspectRatio;
    glViewport((m_uDisplayWidth-iViewportWidth)/2 + fLeft, fBottom, iViewportWidth, iViewportHeight);
  } 
  else 
  {
    // wide and short slice
    int4 iViewportWidth = m_uDisplayWidth;
    int4 iViewportHeight = iViewportWidth * fSliceAspectRatio;
    glViewport(fLeft, (m_uDisplayHeight-iViewportHeight)/2 + fBottom, iViewportWidth, iViewportHeight);
  } // endif(displayAspectRatio<sliceAspectRatio)
} // SetViewport()


/**
 * scroll segment
 *
 * @param count scroll amount;
 * @param bAutoFly  true when auto-(centerline)flight.
 * @return only used in 2D flights, -1: stop auto-fly
 */
int4 SegmentCylindricalViewer::Scroll(const int4 iCount)
{
  Dataset & myDataset = * GetDataset();

  m_iDesiredPlaneAngle += 60 * iCount;
  while(m_iDesiredPlaneAngle>359) m_iDesiredPlaneAngle -= 360;
  while(m_iDesiredPlaneAngle<0)   m_iDesiredPlaneAngle += 360;
  return 0;
} // Scroll().


/** 
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void SegmentCylindricalViewer::SetupPlaneCorners(const float4 fRotationAngle)
{
  Dataset & myDataset = * GetDataset();

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
  float4 fAdjustedPlaneAngle = m_iCurrentPlaneAngle + fRotationAngle; while(fAdjustedPlaneAngle >=360.0) fAdjustedPlaneAngle -= 360.0;
  m.Rotate( nSegmentAxis, fAdjustedPlaneAngle * M_PI / 180.0F );
  Vector<float4> vThisImageUp = m * vUp;

  // Get the "normal" vector for this image (e.g. ray direction if raycasting)
  Normal<float4> nThisImageNormal = Normal<float4>(nSegmentAxis.Cross(vThisImageUp));
  
  // Compute 4 corners of the entire plane in world coordinates
  m_fExtraLength = (m_uMaxSegmentLength<fLength) ? 0 : ((m_uMaxSegmentLength-fLength)/2.0);
  Vector<float4> vExtraLength = Vector<float4>(nSegmentAxis)*m_fExtraLength;
  myDataset.m_vObliqueBottomLeft  = Point1 - vExtraLength + vThisImageUp * -40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueBottomRight = Point2 + vExtraLength + vThisImageUp * -40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueTopLeft     = Point1 - vExtraLength + vThisImageUp *  40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueTopRight    = Point2 + vExtraLength + vThisImageUp *  40;//m_fSizeWorldUnits;
  myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();
  
  // save the slice definition for later volume rendering of the shadow
  myDataset.m_vObliqueLocation = Point1;
  myDataset.m_vObliqueOrientation = nThisImageNormal;
  myDataset.m_vObliqueUp = Normal<float4>(vThisImageUp);
  
} // CreateSliceImageFromVol()


/** 
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void SegmentCylindricalViewer::CreateImageFromVol()
{
  Dataset & myDataset = * GetDataset();
  
  uint4 uPixelOffset = m_uSubSliceWidth * m_fExtraLength / m_uMaxSegmentLength;
  myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset,
                                                         myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight,
                                                         myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                         Vector<float4>( myDataset.m_vObliqueOrientation ), myDataset.m_vObliqueUp,
                                                         m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                         0, 0, m_uSliceWidth, m_uSliceHeight,
                                                         true, m_bShowUncleansed, false, false, Normal3Df(), Point3Df() );
} // CreateSliceImageFromVol()


/**
 * Render the slice
 * Changed the parameter list: JRM
 * We need to specify the viewport for the more general (not aligned at (0,0) case)
 */
void SegmentCylindricalViewer::RenderSegment(const float4 fLeft, const float4 fBottom)
{
  Dataset & myDataset = * GetDataset();
  SetViewport(fLeft, fBottom);

  /// Gets a buffer for the render slice
  if(m_iDesiredPlaneAngle != m_iCurrentPlaneAngle || m_uCurrentSegmentIndex != myDataset.m_currentSegment)
  {
    m_iCurrentPlaneAngle = m_iDesiredPlaneAngle;
    SetupPlaneCorners( 0 );
    CreateImageFromVol();
    m_uCurrentSegmentIndex = myDataset.m_currentSegment;
  }
  
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

void SegmentCylindricalViewer::SetupHalfPlaneShadow(const Point2D<int4> & point)
{

  if(point.m_x < m_uDisplayWidth/2 && point.m_y < m_uDisplayHeight/2)
  {
    SetupPlaneCorners(180);
  }
  else if (point.m_y < m_uDisplayHeight/2)
  {
    SetupPlaneCorners(270);
  }
  else if (point.m_x < m_uDisplayWidth/2)
  {
    SetupPlaneCorners(0);
  }
  else
  {
    SetupPlaneCorners(90);
  }
}


/**
 * Get a picked location in world coordinates.
 */
Point<float4> SegmentCylindricalViewer::GetPickedLocationWorldCoord( const float4 fPosX, const float4 fPosY,
                                                                     Normal3D<float4> & viewDirection ) const
{
  Point<float4> vPosVolumeCoord = GetPickedLocationVolumeCoord( fPosX, fPosY, viewDirection );
  return Point<float4>( vPosVolumeCoord.m_x * GetDataset()->m_vUnits.m_x,
                        vPosVolumeCoord.m_y * GetDataset()->m_vUnits.m_y,
                        vPosVolumeCoord.m_z * GetDataset()->m_vUnits.m_z );
} // GetPickedLocationWorldCoord()


/**
 * Get a picked location in volume coordinates.
 */
Point<float4> SegmentCylindricalViewer::GetPickedLocationVolumeCoord( const float4 fPosX, const float4 fPosY,
                                                                      Normal3D<float4> & viewDirection ) const
{
  Point<float4> vPosVolumeCoord;

  // we call the rendering with the "sample depth" parameter set to true
  // this will cause the rayCast function to simply cast one ray at the specified image
  // position and return the world position at 3D position the ray first intersects the colon wall
  Dataset & myDataset = * GetDataset();
  uint4 uPixelOffset = m_uSubSliceWidth * m_fExtraLength / m_uMaxSegmentLength;
  Point3Df intersectionPoint( fPosX * m_uSliceWidth, ( 1.0F - fPosY ) * m_uSliceHeight, 0.0F );
  myDataset.m_fields.m_volumeRender.RayCastObliqueSlice( myDataset,
                                                         myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight,
                                                         myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight,
                                                         Vector<float4>( myDataset.m_vObliqueOrientation ), myDataset.m_vObliqueUp,
                                                         m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset,
                                                         0, 0, m_uSliceWidth, m_uSliceHeight,
                                                         true, m_bShowUncleansed, true, false, viewDirection, intersectionPoint );

  // convert from world to volume coordinates
  return intersectionPoint.GetDividedAnIso( myDataset.m_vUnits );
}


Dataset * SegmentCylindricalViewer::GetDataset() const
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
    LogRec("bad dataset case","SegmentCylindricalViewer", "GetDataset");
    return & rdrGlobal.m_nullDataset;
    break;
  }
  ex::IllegalArgumentException(LogRec("bad dataset case","SegmentCylindricalViewer", "GetDataset"));
  return & rdrGlobal.m_nullDataset;
}


// $Log: SegmentCylindricalViewer.C,v $
// Revision 1.4.2.1  2010/02/01 19:32:58  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
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
// Revision 1.4.2.1.4.1  2001/01/02 11:23:40  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 1.4.2.1  2004/04/07 21:10:13  jmeade
// Removing unused enums.
//
// Revision 1.4  2003/01/24 20:26:25  jmeade
// Plane is no longer a template class.
//
// Revision 1.3  2003/01/22 22:27:10  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.2  2002/11/24 17:49:51  kevin
// Renamed ProjectOntoPlane
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SegmentCylindricalViewer.C,v 1.4.2.1 2010/02/01 19:32:58 dongqing Exp $
// $Id: SegmentCylindricalViewer.C,v 1.4.2.1 2010/02/01 19:32:58 dongqing Exp $

