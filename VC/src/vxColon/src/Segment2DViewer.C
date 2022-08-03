// $Id: Segment2DViewer.C,v 1.3 2006/09/26 19:37:24 frank Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  Segment2DViewer.h
// Description: A class to support viewing of segments along the centerline
// Created: June 10, 2002
// Author: Kevin Kreeger  (kevin@viatronix.net)


#include "stdafx.h"
#include "Dataset.h"
#include "Segment2DViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "Plane.h"

#define FILE_REVISION "$Revision: 1.3 $"

using namespace ReaderLib;
using namespace ReaderLib::SliceOrientations;

/**
 * Constructor
 */
Segment2DViewer::Segment2DViewer() : m_puSliceImage(NULL)
{
  Initialize();
} // Constructor()


/**
 * Destructor
 */
Segment2DViewer::~Segment2DViewer()
{
  if (m_puSliceImage) delete [] m_puSliceImage;
} // Destructor().


/**
 * Copy constructor
 *
 * @param const Segment2DViewer & other   Source
 */
Segment2DViewer::Segment2DViewer(const Segment2DViewer & other) : m_puSliceImage(NULL)
{
  Initialize();
  *this = other;
} // Copy constructor().


/**
 * Initialize this Segment2DViewer.
 */
void Segment2DViewer::Initialize()
{
  m_uDisplayWidth = m_uDisplayHeight = 0;
  m_eDatasetType = DatasetOrientations::ePRIMARY;

  m_bBlockedPixel = false;
  m_bShowUncleansed = false;

  m_uMaxSegmentLength = 128;
  m_uSliceWidth = 512;
  m_uSliceHeight = 256;
  m_uSliceSize = m_uSliceWidth * m_uSliceHeight;
  if (m_puSliceImage)  delete [] m_puSliceImage;
  m_puSliceImage = new uint1 [m_uSliceSize];
  m_iCurrentPlaneAngle = -1;
  m_iDesiredPlaneAngle = 0;
  m_uCurrentSegmentIndex = -1;

  m_rSelect = Rect<uint4>(9999, 9999, 9999, 9999);
  m_viStartMovePt = Point2D<int4>(9999, 9999);
  m_vfAnchorPt = Point2D<float4>(-1.0f, -1.0f);

} // Initialize().


/**
 * Sets up the OpenGL rendering context
 */
void Segment2DViewer::SetupOpenGLContext()
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
void Segment2DViewer::ResizeDisplay(const uint4 uWidth, const uint4 uHeight)
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
void Segment2DViewer::SetViewport(const float4 fLeft, const float4 fBottom)
{
  /// no, no, this viewport setting is definitely no-good -> lots of problems may cause.

  // setup the viewport so the aspect ratio is correct
  float4 fSliceAspectRatio = float4(m_uSliceHeight) / float4(m_uSliceWidth);
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
 * Scrolls segment
 * 
 * @param count scroll amount;
 * @param bAutoFly  true when auto-(centerline)flight.
 * @return only used in 2D flights, -1: stop auto-fly
 */
int4 Segment2DViewer::Scroll(const int4 iCount)
{
  Dataset & myDataset = * GetDataset();

  m_iDesiredPlaneAngle += iCount;
  while(m_iDesiredPlaneAngle>360) m_iDesiredPlaneAngle -= 360;
  while(m_iDesiredPlaneAngle<0)   m_iDesiredPlaneAngle += 360;
  return 0;
} // Scroll().


/** 
 * Construct slice image from volume
 *
 * @param eSliceOrientation   Axial/Sagittal/Coronal
 */
void Segment2DViewer::CreateSliceImageFromVol(const int4 iCurrentPlaneAngle, const bool bShowUncleansed)
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
  m.Rotate( nSegmentAxis, iCurrentPlaneAngle * M_PI / 180.0F );
  Vector<float4> vThisImageUp = m * vUp;

  // Get the "normal" vector for this image (e.g. ray dorection if raycasting)
  Normal<float4> nThisImageNormal = Normal<float4>(nSegmentAxis.Cross(vThisImageUp));
  
  // Compute 4 corners of the entire plane in world coordinates
  float4 fExtraLength = (m_uMaxSegmentLength<fLength) ? 0 : ((m_uMaxSegmentLength-fLength)/2.0);
  Vector<float4> vExtraLength = Vector<float4>(nSegmentAxis)*fExtraLength;
  myDataset.m_vObliqueBottomLeft  = Point1 - vExtraLength + vThisImageUp * -40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueBottomRight = Point2 + vExtraLength + vThisImageUp * -40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueTopLeft     = Point1 - vExtraLength + vThisImageUp *  40;//m_fSizeWorldUnits;
  myDataset.m_vObliqueTopRight    = Point2 + vExtraLength + vThisImageUp *  40;//m_fSizeWorldUnits;
  myDataset.m_uObliqueViewIdentifier = myDataset.m_submarine.GetViewIdentifier();
  
  // save the slice definition for later volume rendering of the shadow
  myDataset.m_vObliqueLocation = Point1;
  myDataset.m_vObliqueOrientation = nThisImageNormal;
  myDataset.m_vObliqueUp = Normal<float4>(vThisImageUp);
  
  uint4 uPixelOffset = m_uSliceWidth*fExtraLength/m_uMaxSegmentLength;
  myDataset.m_fields.m_volumeRender.CreateObliqueSlice(myDataset.m_vObliqueBottomLeft, myDataset.m_vObliqueBottomRight, 
	                                                     myDataset.m_vObliqueTopLeft, myDataset.m_vObliqueTopRight, 
                                                       m_puSliceImage, m_uSliceWidth, m_uSliceHeight, uPixelOffset, m_bShowUncleansed);
} // CreateSliceImageFromVol()


/**
 * Render the slice
 * Changed the parameter list: JRM
 * We need to specify the viewport for the more general (not aligned at (0,0) case)
 */
void Segment2DViewer::RenderSegment(const float4 fLeft, const float4 fBottom)
{
  Dataset & myDataset = * GetDataset();
  SetViewport(fLeft, fBottom);

  // Lazy load the Diff volume if required
  if(m_bShowUncleansed && !GetDataset()->m_fields.IsDiffVolLoaded()) GetDataset()->m_fields.LazyLoadDiffVolume();
  
  /// Gets a buffer for the render slice
  if(m_iDesiredPlaneAngle != m_iCurrentPlaneAngle || m_uCurrentSegmentIndex != myDataset.m_currentSegment)
  {
    CreateSliceImageFromVol(m_iDesiredPlaneAngle, m_bShowUncleansed);
    m_iCurrentPlaneAngle = m_iDesiredPlaneAngle;
    m_uCurrentSegmentIndex = myDataset.m_currentSegment;
  }
  
  // draw the image
  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); 
  {
    gluOrtho2D(0, 1, 0, 1);
    
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_uSliceWidth);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, m_uSliceWidth, m_uSliceHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*) m_puSliceImage);
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
 * Get a picked location in world coordinates.
 */
Point<float4> Segment2DViewer::GetPickedLocationWorldCoord(const float4 fPosX, const float4 fPosY) const
{
  Point<float4> vPosVolumeCoord = GetPickedLocationVolumeCoord(fPosX, fPosY);
  return Point<float4>(vPosVolumeCoord.m_x * GetDataset()->m_vUnits.m_x,
                       vPosVolumeCoord.m_y * GetDataset()->m_vUnits.m_y,
                       vPosVolumeCoord.m_z * GetDataset()->m_vUnits.m_z);
} // GetPickedLocationWorldCoord()


/**
 * Get a picked location in volume coordinates.
 */
Point<float4> Segment2DViewer::GetPickedLocationVolumeCoord(const float4 fPosX, const float4 fPosY) const
{
  Point<float4> vPosVolumeCoord;

  return vPosVolumeCoord;
}


Dataset * Segment2DViewer::GetDataset() const
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
    LogRec("bad dataset case","Segment2DViewer", "GetDataset");
    return & rdrGlobal.m_nullDataset;
    break;
  }
  ex::IllegalArgumentException(LogRec("bad dataset case","Segment2DViewer", "GetDataset"));
  return & rdrGlobal.m_nullDataset;
}


// $Log: Segment2DViewer.C,v $
// Revision 1.3  2006/09/26 19:37:24  frank
// replaced hardcoded numbers with constants
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
// Revision 1.7.2.1.4.1  2001/01/02 11:22:44  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 1.7.2.1  2004/04/07 21:10:13  jmeade
// Removing unused enums.
//
// Revision 1.7  2003/01/24 20:26:25  jmeade
// Plane is no longer a template class.
//
// Revision 1.6  2003/01/22 22:26:42  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.5  2002/11/24 17:49:51  kevin
// Renamed ProjectOntoPlane
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Segment2DViewer.C,v 1.3 2006/09/26 19:37:24 frank Exp $
// $Id: Segment2DViewer.C,v 1.3 2006/09/26 19:37:24 frank Exp $

