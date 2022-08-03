// $Id: SkeletonRender.C,v 1.3 2006/01/31 14:05:19 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.net)


// includes
#include "StdAfx.h"
#include "SkeletonRender.h"
#include "ReaderGlobal.h"
#include "VolumeRender.inl"
#include "RenderUtil.h"
#include "Rect.h"


// namespaces
using namespace ReaderLib;


/*
 * Draw the skeleton.
 *
 * @param colorTriple   The color in which to draw the centerline.
 */
void SkeletonRender::DrawWithHiddenLines(const float4 * pfDepthBuffer, const uint4 uRenderSize, const uint4 uRenderBufferSize,
                                         const Point<float4> viewpoint, const Point<float4> obliquePoint, const bool bShowObliquePoint,
                                         const Triple<uint1> & colorTriple)
{
  // get the current OpenGL matrices
  GLdouble mModelMatrix[16]; glGetDoublev(GL_MODELVIEW_MATRIX,mModelMatrix);
  GLdouble mProjMatrix[16]; glGetDoublev(GL_PROJECTION_MATRIX,mProjMatrix);
  const GLint vViewport[4] = { 0, 0, uRenderSize, uRenderSize };
  const Rect<GLint> viewportRect(vViewport[0], vViewport[1], vViewport[2], vViewport[3]);

  // OpenGL setup
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glLineWidth(3.0F);
  RenderUtil::GlColor3ub(colorTriple);

  Point<float8> winCoordsPoint(0.0,0.0,0.0);
  Point<float8> lastWinCoordsPoint(0.0,0.0,0.0);
  Point<float4> point(0.0,0.0,0.0);
  Point<float4> lastPoint(GetSkelNode(0).m_vWorldPos);

  for (uint4 i(0); i<GetNumOfElements(); i++)
  {
    // get the current point in world coords and window coords
    point = GetSkelNode(i).m_vWorldPos;
    gluProject(float8(point.m_x), float8(point.m_y), float8(point.m_z),
      mModelMatrix, mProjMatrix, vViewport,
      &winCoordsPoint.m_x, &winCoordsPoint.m_y, &winCoordsPoint.m_z);

    // determine whether to draw the current line segment
    bool bDraw(false);
    // if EITHER point is visible (within viewport and in front of organ wall)
    const bool bThisPointInside( viewportRect.PtInRect( winCoordsPoint.m_x, winCoordsPoint.m_y ) );
    if ( bThisPointInside )
    {
      float4 fDepthZ(pfDepthBuffer[((int4)(winCoordsPoint.m_y))*uRenderBufferSize + ((int4)(winCoordsPoint.m_x))]);
      float4 fDistSquared(Vector<float4>(point-viewpoint).GetLengthSquared());
      // if in front of organ wall
      if (fDistSquared <= fDepthZ*fDepthZ)
      {
        bDraw = true;
      } // if in front of organ wall
    } // if within viewport
    const bool bLastPointInside( viewportRect.PtInRect( lastWinCoordsPoint.m_x, lastWinCoordsPoint.m_y ) );
    if ( bLastPointInside )
    {
      float4 fDepthZ(pfDepthBuffer[((int4)(lastWinCoordsPoint.m_y))*uRenderBufferSize + ((int4)(lastWinCoordsPoint.m_x))]);
      float4 fDistSquared(Vector<float4>(lastPoint-viewpoint).GetLengthSquared());
      // if in front of organ wall
      if (fDistSquared <= fDepthZ*fDepthZ)
      {
        bDraw = true;
      } // if in front of organ wall
    } // if within viewport

    // unless BOTH points are within the window and EITHER is behind the organ wall
    if ( bDraw && bThisPointInside && bLastPointInside )
    {
      float4 fDepthZ(pfDepthBuffer[((int4)(winCoordsPoint.m_y))*uRenderBufferSize + ((int4)(winCoordsPoint.m_x))]);
      float4 fDistSquared(Vector<float4>(point-viewpoint).GetLengthSquared());
      // if behind organ wall
      if (fDistSquared > fDepthZ*fDepthZ)
      {
        bDraw = false;
      } // if behind organ wall
      fDepthZ =  pfDepthBuffer[((int4)(lastWinCoordsPoint.m_y))*uRenderBufferSize + ((int4)(lastWinCoordsPoint.m_x))];
      fDistSquared = Vector<float4>(lastPoint-viewpoint).GetLengthSquared();
      // if behind organ wall
      if (fDistSquared > fDepthZ*fDepthZ)
      {
        bDraw = false;
      } // if behind organ wall
    } // if within viewpoint

    if (bDraw == true)
    {
      glBegin(GL_LINES);
      {
        RenderUtil::GlVertex(point);
        RenderUtil::GlVertex(lastPoint);
      } // glBegin
      glEnd();
    } // if draw == true

    // save the previous point
    lastWinCoordsPoint = winCoordsPoint;
    lastPoint = point;

  } // for all points on skeleton
  // get the current point in world coords and window coords

  // Draw the oblique marker
  glColor3f(1.0F, 1.0F, 1.0F);
  if ((rdrGlobal.m_bObliqueShadow == true) &&
      (rdrGlobal.m_bAlignCrossSectional)   &&
      bShowObliquePoint)
  {
    // prepare to draw the markers
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    const uint4 uSphereLongitudes(10);
    const uint4 uSphereLatitudes(4);

    GLUquadricObj * pQuadObject = gluNewQuadric();
    gluProject(float8(obliquePoint.m_x), float8(obliquePoint.m_y), float8(obliquePoint.m_z),
               mModelMatrix, mProjMatrix, vViewport,
               &winCoordsPoint.m_x, &winCoordsPoint.m_y, &winCoordsPoint.m_z);

    // determine whether to draw the current marker
    if (viewportRect.PtInRect(winCoordsPoint.m_x, winCoordsPoint.m_y))
    {
      float4 fDepthZ(pfDepthBuffer[((int4)(winCoordsPoint.m_y))*uRenderBufferSize + ((int4)(winCoordsPoint.m_x))]);
      float4 fDistSquared(Vector<float4>(point-viewpoint).GetLengthSquared());
      if (fDistSquared <= fDepthZ*fDepthZ)
      {
        glPushMatrix();
        {
          glTranslated(point.m_x,point.m_y,point.m_z);
          const float4 fSphereRadius(0.25F);
          gluSphere(pQuadObject, fSphereRadius, uSphereLongitudes, uSphereLatitudes);
        } // glPushMatrix
        glPopMatrix();
      } // if in front of organ wall
    } // if within viewport
    gluDeleteQuadric(pQuadObject);

    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
  } // endif draw the oblique marker

} // DrawWithHiddenLines()


/**
 * Render a centerline branch.
 *
 * @param colorVector   The color of the rendered branch.
 * @param lineWidth     The width of the strip lines.
 */
void SkeletonRender::DrawBranch(const Vector<float4> & colorVector, const float4 fLineWidth)
{
  glLineWidth(fLineWidth);
  glColor3f(colorVector.m_x, colorVector.m_y, colorVector.m_z);

  glBegin(GL_LINE_STRIP);
  //glBegin(GL_POINTS);
  {
    for (uint4 i=0; i<GetNumOfElements(); i++)
    {
      RenderUtil::GlVertex(GetSkelNode(i).m_vWorldPos);
    } // end loop over vertices
  } // glBegin
  glEnd();
} // DrawBranch()


/**
 * Render a centerline branch.
 *
 * @param colorVector   The color of the rendered branch.
 * @param lineWidth     The width of the strip lines.
 */
void SkeletonRender::DrawBranch(const Vector<float4> & colorVector, const float4 fLineWidth, uint4 start, uint4 stop)
{
  glLineWidth(fLineWidth);
  glColor3f(colorVector.m_x, colorVector.m_y, colorVector.m_z);

  glBegin(GL_LINE_STRIP);
  //glBegin(GL_POINTS);
  {
    for (uint4 i=start; i<stop; i++)
    {
      RenderUtil::GlVertex(GetSkelNode(i).m_vWorldPos);
    } // end loop over vertices
  } // glBegin
  glEnd();
} // DrawBranch()


/**
 * Render a centerline branch in Picking/Verify Mode.
 *
 * @param colorVector   The color of the rendered branch.
 * @param lineWidth     The width of the strip lines.
 */
void SkeletonRender::DrawBranch(const Vector<float4> & vStartColor, const Vector<float4> & vEndColor, const float4 fLineWidth)
{
  const float4 fMinTransitionRegion = 0.3F;
  const float4 fMaxTransitionRegion = 0.7F;
  const float4 fGamma = 1.0F;
  const int4	iNumOfElements = GetNumOfElements();

  glLineWidth(fLineWidth);

  // get first point.
  const SkeletonNode & node = GetSkelNode(0);	// first.
  Point<float4> vPoint1, vPoint0 = node.m_vWorldPos;
  float4 fPercent = 0.0F;
  Vector<float4> vColor0(vStartColor * pow(1.0F - fPercent, fGamma) + vEndColor * pow((fPercent), fGamma));

  glBegin(GL_LINES); 
  {
    for (uint4 i(1); i<iNumOfElements; i++) 
    {
      const SkeletonNode & node = GetSkelNode(i);
      vPoint1 = node.m_vWorldPos;
      glColor3f (vColor0.m_x, vColor0.m_y, vColor0.m_z);
      glVertex3f(vPoint0.m_x, vPoint0.m_y, vPoint0.m_z);
      glVertex3f(vPoint1.m_x, vPoint1.m_y, vPoint1.m_z);
      fPercent = Interpolate(fMinTransitionRegion, float4(i) / float4(iNumOfElements - 1), 
                             fMaxTransitionRegion, 0.0F, 1.0F);
      fPercent = Bound(0.0F, fPercent, 1.0F);
      vColor0 = Vector<float4>(vStartColor * pow(1.0F - fPercent, fGamma) + vEndColor * pow(fPercent, fGamma));
      vPoint0 = vPoint1;
    }
  } 
  glEnd();
} // DrawBranch()


// $Log: SkeletonRender.C,v $
// Revision 1.3  2006/01/31 14:05:19  frank
// code review
//
// Revision 1.2  2005/10/04 19:06:07  vxconfig
// optimized rendering code a little bit
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
// Revision 3.3  2003/01/22 22:27:28  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.2  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.1  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
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
//    Rev 1.0   Sep 16 2001 17:27:12   ingmar
// Initial revision.
// Revision 1.41  2001/06/14 19:50:11  binli
// show each segment(skeleton) with gradient (color: red->green)
//
// Revision 1.40  2001/06/12 12:58:32  frank
// Changes after code review.
//
// Revision 1.39  2001/06/11 15:04:46  frank
// Improved efficiency, removed dead code.
//
// Revision 1.38  2001/06/11 13:40:04  frank
// Removed unused skeleton rendering parameter.
//
// Revision 1.37  2001/06/11 12:20:48  frank
// Renamed variables for coding standards.
//
// Revision 1.36  2001/06/04 18:27:06  frank
// Removed dead code, beautified.
//
// Revision 1.35  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.34  2001/05/02 20:12:10  ingmar
// BookmarkList.C
//
// Revision 1.33  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.32.2.1  2001/05/01 23:42:50  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.32  2001/04/19 19:11:57  binli
// adjust the default GL status in overview class
//
// Revision 1.31  2001/04/18 16:26:38  binli
// 1. set back to default status: lighting on in DrawBranch()
// 2. code standard.
//
// Revision 1.30  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.29  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.28  2001/03/19 11:59:22  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float48).
//
// Revision 1.27  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.26  2001/02/07 19:37:17  frank
// Fixed color interpolation.
//
// Revision 1.25  2001/01/31 16:54:58  binli
// fixed flasing of centerline color in translucent: disable gl_lighting.
//
// Revision 1.24  2001/01/29 17:44:36  binli
// defect 000188: centerline color bugs. replaced GL_LINE_STRIPS with GL_LINES.
//
// Revision 1.23  2001/01/24 15:55:19  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.22  2001/01/16 19:07:38  binli
// the skeleton is rendered in arrows in verification
//
// Revision 1.21  2001/01/16 15:50:15  binli
// get a node in the skeleton (for Verification)
//
// Revision 1.20  2001/01/12 23:00:27  binli
// added a ball to represents the middle of skeleton (used in vertification).
//
// Revision 1.19  2001/01/12 17:59:10  frank
// Transitioned the two colors differently along the skeleton.
//
// Revision 1.18  2001/01/11 21:16:34  frank
// Added color gradient rendering.
//
// Revision 1.17  2001/01/11 15:30:00  frank
// Added function to load checker texture into OpenGL.
//
// Revision 1.16  2001/01/09 18:23:28  frank
// Animated centerline.
//
// Revision 1.15  2001/01/09 16:50:49  frank
// Animated texture.
//
// Revision 1.14  2001/01/05 15:09:32  binli
// skeleton comes back again in translucent mode: set the color differently from the background.
//
// Revision 1.13  2000/12/27 17:50:41  kevin
// CHanges to remove Mprviewer
//
// Revision 1.12  2000/11/25 04:31:28  kevin
// CHanged size of x-sec cneterline point
//
// Revision 1.11  2000/11/22 17:14:59  binli
// fixed display list for strips rendering.
//
// Revision 1.10  2000/11/17 21:47:09  binli
// changed the color of centerline when overview is rendered in translucent mode.
//
// Revision 1.9  2000/11/17 21:27:13  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.8  2000/11/17 04:23:02  kevin
// Added resizing of raycasting window to non-power-of-two
//
// Revision 1.7  2000/11/16 20:46:05  kevin
// Made some volume render stuff inline
//
// Revision 1.6  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.5  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.4  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.3  2000/10/29 19:42:04  kevin
// Added X-Sec alignment functionality
//
// Revision 1.2  2000/09/11 19:03:11  frank
// Restricted shadow display to static scenes only
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 23    9/09/00 1:55p Kevin
// Added angular measurement and hooks for volume measurement
//
// 22    9/03/00 6:43p Kevin
// Draw a thinner skeleton so that it is not so overpowering
//
// 21    8/31/00 8:51a Frank
// Updated comments
//
// 20    8/28/00 11:55a Antonio
// updated header
//
//*****************  Version 19  *****************
//User: Kevin           Date:  8/20/00  Time:  1:48p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed centerline coloring
//
//*****************  Version 18  *****************
//User: Kevin           Date:  8/18/00  Time:  2:53p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added direction to skeleton coloring
//
//*****************  Version 17  *****************
//User: Frank           Date:  8/11/00  Time:  4:28p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated skelton index because csi volume
//appears to restart numbering each segment
//
//*****************  Version 16  *****************
//User: Kevin           Date:  8/07/00  Time:  6:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed CSI problems with new segment data structures
//
//*****************  Version 15  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 14  *****************
//User: Kevin           Date:  8/04/00  Time:  6:55p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed the drawing of the first skeleton point
//
//*****************  Version 13  *****************
//User: Kevin           Date:  8/04/00  Time:  2:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed  m_wrongWayFrameThreshold
//
//*****************  Version 12  *****************
//User: Kevin           Date:  8/04/00  Time: 11:02a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Made the directional indicator be centerline color
//
//*****************  Version 11  *****************
//User: Kevin           Date:  8/04/00  Time:  9:32a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added directional arrows to the cetnerline
//
//*****************  Version 10  *****************
//User: Kevin           Date:  8/03/00  Time:  4:47p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added branches back
//
//*****************  Version 9  *****************
//User: Kevin           Date:  8/03/00  Time:  9:36a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed first skeleton point error
//
//*****************  Version 8  *****************
//User: Kevin           Date:  8/02/00  Time: 11:47a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colors
//
//*****************  Version 7  *****************
//User: Kevin           Date:  8/02/00  Time:  2:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Begining of multi-segmnet handling
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/02/00  Time:  1:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Aligned centerline with polygons and cleaned up overview a little
//
//*****************  Version 5  *****************
//User: Kevin           Date:  7/26/00  Time:  6:32p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed the Skeleton scaleing
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/26/00  Time:  4:21p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added MPR slicing along the centerline
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/26/00  Time:  2:01p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Got the new skeleton format to draw
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 1  *****************
//User: Frank           Date:  7/24/00  Time:  2:30p
//Created SkeletonRender.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SkeletonRender.C,v 1.3 2006/01/31 14:05:19 frank Exp $
// $Id: SkeletonRender.C,v 1.3 2006/01/31 14:05:19 frank Exp $
