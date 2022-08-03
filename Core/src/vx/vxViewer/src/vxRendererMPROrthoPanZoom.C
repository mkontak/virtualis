// $Id: vxRendererMPROrthoPanZoom.C,v 1.1 2004/04/26 21:25:07 vxconfig Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Uday Chebrolu (mailto:uday@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererMPROrthoPanZoom.h"
#include "vxEnvironment.h"
#include "GLEnv.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Constructor
 * @param environment
 */
vxRendererMPROrthoPanZoom::vxRendererMPROrthoPanZoom(vxEnvironment & environment)
:vxRenderer(environment)
//*******************************************************************
{
}


/**
 * Render the pan zoom box
 */
void vxRendererMPROrthoPanZoom::Render()
//*******************************************************************
{
  vxWinRect winRect(GetEnvironment().GetWinRect());
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  uint2 uAxisZ(GetEnvironment().GetCamera()->GetViewDirection().GetMaxAbsComponentCoord());
  Triple<float4> volDim(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  float4 fHeight(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fDepth(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisZ]);
  float4 fAspectRatioVolume(volDim[uAxisY]/volDim[uAxisX]);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());
  float4 fWidthMax(max(volDim[uAxisX], volDim[uAxisY]));

  vxZoomFactor zoomFactor(*static_cast<vxZoomFactor *>(GetEnvironment().GetZoomFactor()));
  Point3Df center(zoomFactor.GetCenter());
  float4 fSizeHalf(0.5F * zoomFactor.GetSize());

  float4 afBoxScreenMin[3];
  float4 afBoxScreenMax[3];
  afBoxScreenMin[uAxisZ] = center[uAxisZ] - fSizeHalf;
  afBoxScreenMax[uAxisZ] = center[uAxisZ] + fSizeHalf;

  if (fAspectRatioWindow < fAspectRatioVolume)
  {
    // x-axis adjustment
    afBoxScreenMin[uAxisX] = center[uAxisX] - fSizeHalf / fAspectRatioWindow;
    afBoxScreenMax[uAxisX] = center[uAxisX] + fSizeHalf / fAspectRatioWindow;
    afBoxScreenMin[uAxisY] = center[uAxisY] - fSizeHalf;
    afBoxScreenMax[uAxisY] = center[uAxisY] + fSizeHalf;
  }
  else
  {
    // y-axis adjustment
    afBoxScreenMin[uAxisX] = center[uAxisX] - fSizeHalf;
    afBoxScreenMax[uAxisX] = center[uAxisX] + fSizeHalf;
    afBoxScreenMin[uAxisY] = center[uAxisY] - fSizeHalf * fAspectRatioWindow;
    afBoxScreenMax[uAxisY] = center[uAxisY] + fSizeHalf * fAspectRatioWindow;
  }


  Box<float4> boxVolume(0, 0, 0, volDim[0], volDim[1], volDim[2]);
  Box<float4> boxZoom(afBoxScreenMin[0], afBoxScreenMin[1], afBoxScreenMin[2],
                      afBoxScreenMax[0], afBoxScreenMax[1], afBoxScreenMax[2]);
  //boxZoom.BoundWithBox(boxVolume);

  // push the matrices of the working view volume
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glColor3f(1,0,0);
  glLineWidth(2.0);

  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(boxZoom.GetCorner(0).m_x, boxZoom.GetCorner(0).m_y, boxZoom.GetCorner(0).m_z);
    glVertex3f(boxZoom.GetCorner(1).m_x, boxZoom.GetCorner(1).m_y, boxZoom.GetCorner(1).m_z);
    glVertex3f(boxZoom.GetCorner(3).m_x, boxZoom.GetCorner(3).m_y, boxZoom.GetCorner(3).m_z);
    glVertex3f(boxZoom.GetCorner(2).m_x, boxZoom.GetCorner(2).m_y, boxZoom.GetCorner(2).m_z);
  }
  glEnd();
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(boxZoom.GetCorner(4).m_x, boxZoom.GetCorner(4).m_y, boxZoom.GetCorner(4).m_z);
    glVertex3f(boxZoom.GetCorner(5).m_x, boxZoom.GetCorner(5).m_y, boxZoom.GetCorner(5).m_z);
    glVertex3f(boxZoom.GetCorner(7).m_x, boxZoom.GetCorner(7).m_y, boxZoom.GetCorner(7).m_z);
    glVertex3f(boxZoom.GetCorner(6).m_x, boxZoom.GetCorner(6).m_y, boxZoom.GetCorner(6).m_z);
  }
  glEnd();
  glBegin(GL_LINES);
  {
    glVertex3f(boxZoom.GetCorner(0).m_x, boxZoom.GetCorner(0).m_y, boxZoom.GetCorner(0).m_z);
    glVertex3f(boxZoom.GetCorner(4).m_x, boxZoom.GetCorner(4).m_y, boxZoom.GetCorner(4).m_z);
    
    glVertex3f(boxZoom.GetCorner(1).m_x, boxZoom.GetCorner(1).m_y, boxZoom.GetCorner(1).m_z);
    glVertex3f(boxZoom.GetCorner(5).m_x, boxZoom.GetCorner(5).m_y, boxZoom.GetCorner(5).m_z);
    
    glVertex3f(boxZoom.GetCorner(3).m_x, boxZoom.GetCorner(3).m_y, boxZoom.GetCorner(3).m_z);
    glVertex3f(boxZoom.GetCorner(7).m_x, boxZoom.GetCorner(7).m_y, boxZoom.GetCorner(7).m_z);
    
    glVertex3f(boxZoom.GetCorner(2).m_x, boxZoom.GetCorner(2).m_y, boxZoom.GetCorner(2).m_z);
    glVertex3f(boxZoom.GetCorner(6).m_x, boxZoom.GetCorner(6).m_y, boxZoom.GetCorner(6).m_z);
  }
  glEnd();   

  // draw the volume borders
  glColor3f(1,1,0);
  glLineWidth(1.0);

  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(boxVolume.GetCorner(0).m_x, boxVolume.GetCorner(0).m_y, boxVolume.GetCorner(0).m_z);
    glVertex3f(boxVolume.GetCorner(1).m_x, boxVolume.GetCorner(1).m_y, boxVolume.GetCorner(1).m_z);
    glVertex3f(boxVolume.GetCorner(3).m_x, boxVolume.GetCorner(3).m_y, boxVolume.GetCorner(3).m_z);
    glVertex3f(boxVolume.GetCorner(2).m_x, boxVolume.GetCorner(2).m_y, boxVolume.GetCorner(2).m_z);
  }
  glEnd();
  glBegin(GL_LINE_LOOP);
  {
    glVertex3f(boxVolume.GetCorner(4).m_x, boxVolume.GetCorner(4).m_y, boxVolume.GetCorner(4).m_z);
    glVertex3f(boxVolume.GetCorner(5).m_x, boxVolume.GetCorner(5).m_y, boxVolume.GetCorner(5).m_z);
    glVertex3f(boxVolume.GetCorner(7).m_x, boxVolume.GetCorner(7).m_y, boxVolume.GetCorner(7).m_z);
    glVertex3f(boxVolume.GetCorner(6).m_x, boxVolume.GetCorner(6).m_y, boxVolume.GetCorner(6).m_z);
  }
  glEnd();
  glBegin(GL_LINES);
  {
    glVertex3f(boxVolume.GetCorner(0).m_x, boxVolume.GetCorner(0).m_y, boxVolume.GetCorner(0).m_z);
    glVertex3f(boxVolume.GetCorner(4).m_x, boxVolume.GetCorner(4).m_y, boxVolume.GetCorner(4).m_z);
    
    glVertex3f(boxVolume.GetCorner(1).m_x, boxVolume.GetCorner(1).m_y, boxVolume.GetCorner(1).m_z);
    glVertex3f(boxVolume.GetCorner(5).m_x, boxVolume.GetCorner(5).m_y, boxVolume.GetCorner(5).m_z);
    
    glVertex3f(boxVolume.GetCorner(3).m_x, boxVolume.GetCorner(3).m_y, boxVolume.GetCorner(3).m_z);
    glVertex3f(boxVolume.GetCorner(7).m_x, boxVolume.GetCorner(7).m_y, boxVolume.GetCorner(7).m_z);
    
    glVertex3f(boxVolume.GetCorner(2).m_x, boxVolume.GetCorner(2).m_y, boxVolume.GetCorner(2).m_z);
    glVertex3f(boxVolume.GetCorner(6).m_x, boxVolume.GetCorner(6).m_y, boxVolume.GetCorner(6).m_z);
  }
  glEnd();

  // draw focus center
  glColor3f(0.0, 1.0, 0.0);
  glPointSize(3.0);
  glBegin(GL_POINTS);
    glVertex3f(boxZoom.GetMidPoint().m_x, boxZoom.GetMidPoint().m_y, boxZoom.GetMidPoint().m_z);
  glEnd();

  // restore the OpenGL state
  glPopAttrib();

} // Render()


#undef FILE_REVISION 


// Revision History:
// $Log: vxRendererMPROrthoPanZoom.C,v $
// Revision 1.1  2004/04/26 21:25:07  vxconfig
// added renamed classes
//
// Revision 1.3  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.2  2004/04/06 16:27:32  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.18  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.17  2003/05/19 14:40:40  frank
// Removed unused return parameter
//
// Revision 1.16  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.15  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.14  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.13  2002/10/14 15:36:37  michael
// use constructor of Box instead of assignment for initialization
//
// Revision 1.12  2002/05/17 14:58:42  dmitry
// Removed Prerender(), PostRender().
//
// Revision 1.11  2002/05/10 20:36:04  uday
// Corrected coronal orientation.
//
// Revision 1.10  2002/04/24 15:00:37  jenny
// vxBlockVolume stuff
//
// Revision 1.9  2002/04/23 20:12:22  michael
// replaced Density by Intensity
//
// Revision 1.8  2002/04/23 15:39:57  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.7  2002/04/10 20:25:06  uday
// Removed mid point
//
// Revision 1.6  2002/04/10 18:10:51  uday
// camera parameters for windows with arbitrary aspect ratio.
//
// Revision 1.5  2002/04/08 13:34:47  michael
// cosmetics and added author
//
// Revision 1.4  2002/04/04 17:01:22  uday
// Corrected Panning (Michael)
//
// Revision 1.3  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.2  2002/03/29 00:12:40  uday
// Removed translation
//
// Revision 1.1  2002/03/26 21:15:28  uday
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererMPROrthoPanZoom.C,v 1.1 2004/04/26 21:25:07 vxconfig Exp $
// $Id: vxRendererMPROrthoPanZoom.C,v 1.1 2004/04/26 21:25:07 vxconfig Exp $
