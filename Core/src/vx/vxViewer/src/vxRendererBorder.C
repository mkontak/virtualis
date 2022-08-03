// $Id: vxRendererBorder.C,v 1.2 2004/04/26 19:00:17 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: jenny hizver (jenny@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererBorder.h"
#include "vxEnvironment.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 * @param environment
 */
vxRendererBorder::vxRendererBorder(vxEnvironment & environment)
:vxRenderer(environment)
//*******************************************************************
{
} // constructor


/**
 * Renders the border
 */  
void vxRendererBorder::Render()
//*******************************************************************
{	
  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // render
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f, GetEnvironment().GetWinRect().GetViewportSize().first-1, 0.0f, GetEnvironment().GetWinRect().GetViewportSize().second-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // where you want to put the border: left corner, right corner and so on.
  vxWinRect::BoundEnum eBoundType(GetEnvironment().GetWinRect().GetBound());

  // width and height of the border.
  const float4 fWidthPixels(GetEnvironment().GetWinRect().GetViewportSize().first);
  const float4 fHeightPixels(GetEnvironment().GetWinRect().GetViewportSize().second);
  const float4 fRadius(4);
  const uint4 uSteps(4);

  // create a list of all points to be cpnnected to draw the border
  std::vector<Point<float4> > pointVector;

  // check where you want to draw the border
  switch (eBoundType)
  {
  case vxWinRect::LOWER_RIGHT:
    {
      pointVector.push_back(Point<float4>(0,              0,               0));
      pointVector.push_back(Point<float4>(fWidthPixels-1, 0,               0));
      pointVector.push_back(Point<float4>(fWidthPixels-1, fHeightPixels-1, 0));
      pointVector.push_back(Point<float4>(0,              fHeightPixels-1, 0));
      pointVector.push_back(Point<float4>(0,0,0));
    }
    break;
  case vxWinRect::LOWER_LEFT:
    {
      pointVector.push_back(Point<float4>(            -1, fHeightPixels-1, 0));
      pointVector.push_back(Point<float4>(fWidthPixels-1, fHeightPixels-1, 0));
      pointVector.push_back(Point<float4>(fWidthPixels-1, -1             , 0));
    }
    break;
  case vxWinRect::UPPER_LEFT:
  case vxWinRect::UPPER_RIGHT:
    throw ex::VException(LogRec("Currently unsupported border type.", "vxRendererBorder", "Render"));
    break;    
  default:
    break;
  }

  // draw the filled black background
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POINT_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(3.0F);
  glColor4f(0,0,0,0.0F);
  glBegin(GL_POLYGON);
  {
    // loop over all points
    for (uint4 i(0); i<pointVector.size(); ++i)
    {
      glVertex3f(pointVector[i].m_x, pointVector[i].m_y, pointVector[i].m_z);
    }
  }
  glEnd();

  // draw the white border
  glColor3f(1,1,1);
  glBegin(GL_LINE_STRIP);
  {
    // loop over all points
    for (uint4 i(0); i<pointVector.size(); ++i)
    {
      glVertex3f(pointVector[i].m_x, pointVector[i].m_y, pointVector[i].m_z);
    }
  }
  glEnd();
  
  // restore the OpenGL state
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
} // Render()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererBorder.C,v $
// Revision 1.2  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.15  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.14  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.13  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.12  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.11  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.10  2002/04/26 13:39:56  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.9  2002/04/22 21:50:16  uday
// DepthTest disabled;
//
// Revision 1.8  2002/04/10 20:11:16  subba
// Border Added.
//
// Revision 1.7  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.6  2002/03/27 16:08:35  michael
// fixed Push/Pop mistake in render()
//
// Revision 1.5  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.4  2002/03/21 20:12:31  jenny
// put comments
//
// Revision 1.3  2002/03/21 19:16:41  michael
// added log history lines
//
// Revision 1.2  2002/03/21 16:27:44  jenny
// commented out arch drawing.
//
// Revision 1.1  2002/03/21 15:26:45  jenny
// created new class vxRendererBorder
//
// $Id: vxRendererBorder.C,v 1.2 2004/04/26 19:00:17 michael Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererBorder.C,v 1.2 2004/04/26 19:00:17 michael Exp $
