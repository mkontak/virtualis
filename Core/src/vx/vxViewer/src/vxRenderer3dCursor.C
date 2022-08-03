// $Id: vxRenderer3dCursor.C,v 1.10 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Ingmar Bitter (mailto:ingmar@viatronix.com)


// includes
#include <math.h> 
#include "stdafx.h"
#include "vxRenderer3dCursor.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.10 $"


/**
 * Constructor
 * @param environment
 */ 
vxRendererCursor3d::vxRendererCursor3d(vxEnvironment & environment)
:vxRenderer(environment)
//*******************************************************************
{
}


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererCursor3d::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(false);

  // return false if slice shadow aren't enabled
  if( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() != vxRenderingMode::SHADOWS_NONE )
    return false;

  bool bChangedCursor3d(GetEnvironment().IsModified(vxEnvironment::CURSOR3D ));
        
  bRedrawNecessary |= bChangedCursor3d;
  bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::WIN_RECT);

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Draw the Cursor3d
 */
void vxRendererCursor3d::Render()
//*******************************************************************
{
  if( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() != vxRenderingMode::SHADOWS_NONE )
    return;

  if (GetEnvironment().GetCursor3d()->IsCrossHairEnabled() == true)
  {
    RenderCrossHair();
  }

  // Draw Cursor3d if present in environment.
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CURSOR_3D_IN_MPR) &&
      GetEnvironment().GetCursor3d() != NULL) 
  {
    RenderCursor3d();
  }
} // Render()


/**
 * Render Cursor3d
 */
void vxRendererCursor3d::RenderCursor3d()
//*******************************************************************
{
  // get the center point of the sphere
  Point<float4> center(GetEnvironment().GetCursor3d()->GetPosition());

  // get the slice orientation, volume units, etc.
  if (GetEnvironment().GetViewerType()->IsVisible(center) == false)
    return;

  // make the overall length proportional to the window size
  Point2D<int2> screenPos( GetEnvironment().GetWinRect().GetViewportWidth() / 2,
                           GetEnvironment().GetWinRect().GetViewportHeight() / 2 );
  Point3Df worldCoord1( 0, 0, 0 );
  if ( GetEnvironment().GetViewerType()->ConvertScreenToWorld( screenPos, worldCoord1, false ) == false )
    return;
  Point3Df worldCoord2( 0, 0, 0 );
  const float4 fDesiredPixelSize( 15 );
  screenPos.m_x += fDesiredPixelSize;
  if ( GetEnvironment().GetViewerType()->ConvertScreenToWorld( screenPos, worldCoord2, false ) == false )
    return;

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // sphere parameters
  float4 radius(4.0);
  const uint4 uSphereLongitudes(12);
  const uint4 uSphereLatitudes(5);

  // draw the sphere
  GLUquadricObj * pQuadric = gluNewQuadric();
  gluQuadricOrientation(pQuadric, GLU_OUTSIDE);

  GLfloat mat_specular[] = { .72F, .8F, .93F, 1.0F};

  glShadeModel(GL_SMOOTH);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);

  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);

  // set the color of the sphere to green
  glColor4f(0,1,0,1);
  {
    // transform to the proper position.
    glTranslatef(center.m_x, center.m_y, center.m_z);
    gluQuadricDrawStyle(pQuadric, (GLenum)GLU_FILL);
    gluQuadricOrientation(pQuadric, (GLenum)GLU_OUTSIDE);
    gluQuadricNormals(pQuadric, (GLenum)GLU_SMOOTH);
#if 0
    gluSphere(pQuadric, radius, uSphereLongitudes, uSphereLatitudes);
#else
    const float fLength( ( worldCoord1 - worldCoord2 ).GetLength() ); // all the other sizes are proportions of this
    const float fBigRadiusToLength(0.2F);
    const float fBigRadius(fLength * fBigRadiusToLength);
    const float fProportionHead(0.5F);
    const float fHeadLength(fLength * fProportionHead);
    const float fOpeningRadius(1.5F);
    const unsigned int iSlices(16); // accuracy of cylinder approximation
    Vector<float4> vPlanePerpendicular(GetEnvironment().GetPlaneOrtho()->GetNormal());
    uint4 uAxis(vPlanePerpendicular.GetMaxAbsComponentCoord());

    switch(uAxis)
    {
    case 0:
      glPushMatrix();
      {
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(90,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(180,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(270,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();
      break;
    case 1:
      glPushMatrix();
      {
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(90,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(180,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(270,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();
      break;
    case 2:
      glPushMatrix();
      {
        glRotatef(90,1,0,0);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(90,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glRotatef(90,1,0,0);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(180,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glRotatef(90,1,0,0);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();

      glPushMatrix();
      {
        glRotatef(270,vPlanePerpendicular.m_x, vPlanePerpendicular.m_y, vPlanePerpendicular.m_z);
        glRotatef(90,1,0,0);
        glTranslatef(0.0F, 0.0F, fOpeningRadius);
        gluCylinder(pQuadric, 0.0F, fBigRadius, fHeadLength, iSlices, 1);
      } 
      glPopMatrix();
      break;
    }
#endif
  }

  gluDeleteQuadric(pQuadric);

  // restore the OpenGL state
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();  
} // RenderCursor3d()


/**
 * Render CrossHair
 */
void vxRendererCursor3d::RenderCrossHair()
//*******************************************************************
{
  if (dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()) == NULL &&
      dynamic_cast<vxViewerTypeMPROblique *>(GetEnvironment().GetViewerType()) == NULL)
  {
    // Sorry, cross hair rendering here only supported for MRP Ortho and Oblique. 
    // Will need to think about the other ones later
    return;
  }

  // get the center point of the sphere and its screen position
  Point3Df centerWorld(GetEnvironment().GetCursor3d()->GetPosition());
  Point2D<int2> centerScreen;

  if (GetEnvironment().GetViewerType()->ConvertWorldToScreen(centerWorld, centerScreen) == true)
  {
    std::pair<int2, int2> reference(GetEnvironment().GetWinRect().GetReferencePoint());
    std::pair<int2, int2> size(GetEnvironment().GetWinRect().GetViewportSize());

    const uint2 iRadius(30);
    Point2D<int2> vPoints[4][3];
    vPoints[0][0] = Point2D<int2>(min(centerScreen.X() - iRadius, reference.first),              centerScreen.Y());
    vPoints[0][2] = Point2D<int2>(centerScreen.X() - iRadius,                                    centerScreen.Y());
    vPoints[1][0] = Point2D<int2>(max(centerScreen.X() + iRadius, reference.first + size.first), centerScreen.Y());
    vPoints[1][2] = Point2D<int2>(centerScreen.X() + iRadius,                                    centerScreen.Y());
    vPoints[2][0] = Point2D<int2>(centerScreen.X(), min(centerScreen.Y() - iRadius, reference.second));
    vPoints[2][2] = Point2D<int2>(centerScreen.X(), centerScreen.Y() - iRadius);
    vPoints[3][0] = Point2D<int2>(centerScreen.X(), max(centerScreen.Y() + iRadius, reference.second + size.second));
    vPoints[3][2] = Point2D<int2>(centerScreen.X(), centerScreen.Y() + iRadius);

    // make each line into two segments so that it fades out towards the center
    float4 vOpacities[4][3][2];
    float4 iFadeLength(50);

    for (int i(0); i<4; i++)
    {
      const int iAxis((i<2) ? 0 : 1);

      vPoints[i][1][(iAxis+1)%2] = vPoints[i][0][(iAxis+1)%2];

      if (vPoints[i][2][iAxis] == vPoints[i][0][iAxis])
      {
        // point pair is already outside screen, just assign middle point to be there, too.
        vPoints[i][1][iAxis] = vPoints[i][2][iAxis];
      }
      else
      {
        // need to determine direction and add offset
        const int iSign((vPoints[i][2][iAxis] > vPoints[i][0][iAxis]) ? -1 : +1);
        vPoints[i][1][iAxis] = vPoints[i][2][iAxis] + iSign * iFadeLength;
      }

      // fading out underlying shadow line
      vOpacities[i][0][0] = 0.5f;
      vOpacities[i][1][0] = 0.5f;
      vOpacities[i][2][0] = 0.0f;

      // fading out the actual line
      vOpacities[i][0][1] = 1.0f;
      vOpacities[i][1][1] = 1.0f;
      vOpacities[i][2][1] = 0.0f;

      const int iLength(abs(sqrt((float4)(vPoints[i][2].X()*vPoints[i][2].X() + vPoints[i][0].Y()*vPoints[i][0].Y()))));
      if (iLength < iFadeLength)
      {
        const float4 fOpacityWeight(iLength/(float4)iFadeLength);
  
        // fading out underlying shadow line
        vOpacities[i][1][0] = vOpacities[i][1][0] + fOpacityWeight * (vOpacities[i][2][0] - vOpacities[i][1][0]);
        
        // fading out the actual line
        vOpacities[i][1][1] = vOpacities[i][1][1] + fOpacityWeight * (vOpacities[i][2][1] - vOpacities[i][1][1]);
      }
    }

    // save the OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // setup screen space viewport
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

    // new settings
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Rect<float4> viewPortRect(0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);

    gluOrtho2D(0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height());
    glDisable(GL_DEPTH_TEST);

    //glEnable(GL_LINE_STIPPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    PixelRGB<float4> color;

    for (int k(0); k<2; k++)
    {
      if (k == 0)
      {
        color = PixelRGB<float4>(0.0F, 0.0F, 0.0F);
        glLineWidth(2.5);
        glLineStipple(1, 0x0F0F);
      }
      else
      {
        color = PixelRGB<float4>(1.0F, 0.7F, 0.4F);
        glLineWidth(0.5);
        glLineStipple(1, 0x0606);
      }

      // draw the actual lines
      for (int i(0); i<4; i++)
      {
        for (int ii(0); ii<2; ii++)
        {
          glBegin(GL_LINES);
          glColor4f(color.R(), color.G(), color.B(), vOpacities[i][ii][k]);
          glVertex2i(vPoints[i][ii].X(), vPoints[i][ii].Y());
          glColor4f(color.R(), color.G(), color.B(), vOpacities[i][ii+1][k]);
          glVertex2i(vPoints[i][ii+1].X(), vPoints[i][ii+1].Y());
          glEnd();
        }
      }
    }

    // restore the OpenGL state
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();  
  }
} // RenderCrossHair()


#undef FILE_REVISION


// Revision History:
// $Log: vxRenderer3dCursor.C,v $
// Revision 1.10  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.9.2.1  2007/04/05 16:02:25  gdavidson
// Issue #5568: Don't consider rendering mode when converting screen to world coordinates
//
// Revision 1.9  2007/02/07 15:54:00  gdavidson
// Removed check for SHOW_CURSOR_3D_IN_MPR
//
// Revision 1.8  2006/10/03 20:06:49  geconomos
// not rendering 3d cursor crosshair  if slice shadows are enabled
//
// Revision 1.7  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.6  2005/05/24 16:15:44  frank
// adjusted the 3D cursor size better for small/large datasets
//
// Revision 1.5  2005/04/19 23:15:10  michael
// Fixed bug when line segment was outside screen but drawing wrapped it back in
//
// Revision 1.4  2005/03/21 20:48:14  michael
// Added function to render a cross hair on the MPR Ortho and MPR Oblique
//
// Revision 1.3  2004/04/06 16:26:11  frank
// unified environment access naming
//
// Revision 1.2  2004/03/31 17:46:06  frank
// unused include files
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/01/30 01:21:33  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.21  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.20  2003/08/27 14:35:42  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.19  2003/08/21 13:09:11  michael
// made rendering based on viewer type so that not only Ortho is supported
//
// Revision 1.18  2003/06/18 15:19:57  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.17  2003/05/19 14:37:24  frank
// Removed unused return parameter
//
// Revision 1.16  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.15  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.14  2003/05/14 16:10:30  michael
// code review
//
// Revision 1.13  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.12  2003/04/16 14:42:02  michael
// removed redundant include
//
// Revision 1.11  2003/01/22 21:59:48  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.10  2002/11/29 19:47:32  frank
// Made Ingmar's 3D cursor (Kevin K at Franks Machine)
//
// Revision 1.9  2002/11/14 14:49:00  geconomos
// Check of SHOW_3D_CURSOR rendering mode before rendering.
//
// Revision 1.8  2002/11/12 18:13:16  ingmar
// removed dead code
//
// Revision 1.7  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.6  2002/11/08 20:02:04  ingmar
// fixed sphere apearance
//
// Revision 1.5  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderer3dCursor.C,v 1.10 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxRenderer3dCursor.C,v 1.10 2007/06/28 19:33:55 jmeade Exp $
