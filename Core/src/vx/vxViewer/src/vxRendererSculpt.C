// $Id: vxRendererSculpt.C,v 1.7 2006/05/31 16:18:22 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxRendererSculpt.h"
#include "vxEnvironment.h"
#include "vxContour.h"
#include "vxUtils.h"


// Defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Constructor
 *
 * @param environment the environment
 */ 
vxRendererSculpt::vxRendererSculpt( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererSculpt()


/**
 * Draw the sculpt lasso
 */
void vxRendererSculpt::Render()
{
  if ( GetEnvironment().GetSculptContour() == NULL )
    return;

  // save OpenGl settings.
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  RenderContour( GetEnvironment().GetSculptContour(), true, false );

  // restore OpenGL settings
  glPopAttrib();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

} // Render()


/**
 * render lasso measurement in 2D slices
 *
 * @param areaContourIter current measurement
 * @param bPrimary        flag telling whether to draw a primary contour and statistics
 * @param bFlag           flag indicating whether to display the label
 */
void vxRendererSculpt::RenderContour( vxContour * pContour, bool bPrimary, bool bFlag )
{
  // set up projection for direct pixel access
  const vxWinRect & winRect = GetEnvironment().GetWinRect(); 
  glViewport( winRect.GetReferencePoint().first, winRect.GetReferencePoint().second,
              winRect.GetViewportSize().first, winRect.GetViewportSize().second );
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0, winRect.GetViewportSize().first-1, winRect.GetViewportSize().second-1, 0 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
 
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  
  // draw the shadow for the contour.
  glColor3f(0.0F, 0.0F, 0.0F);
  glLineWidth(4.0);
  // would prefer to use LINE_LOOP, but that has bugs on some hardware so we use LINE_STRIP instead
  glBegin(GL_LINE_STRIP);
  Point<float4> drawPoint;
  for (int4 i=0; pContour->GetPointNumOfContour()>0 && i<=pContour->GetPointNumOfContour(); i++)
  {
    glVertex3f( pContour->GetPoint(i % pContour->GetPointNumOfContour()).m_x * GetEnvironment().GetWinRect().GetViewportWidth(),
                pContour->GetPoint(i % pContour->GetPointNumOfContour()).m_y * GetEnvironment().GetWinRect().GetViewportHeight(), 0.0F );
  }
  glEnd();

  // get the color for the contour.
  PixelRGBA<uint1> contourColor( 255, 255, 0, 255 );
  if ( bPrimary == false ) 
    contourColor.ScaleRGB( 0.8F );
  glColor4f( contourColor.R() / 255.0, contourColor.G() / 255.0, contourColor.B() / 255.0, contourColor.A() / 255.0 );
  
  // draw the contour of the area 
  glLineWidth( 2.0F );
  glBegin( GL_LINE_STRIP );
  for ( int4 i=0; pContour->GetPointNumOfContour()>0 && i<=pContour->GetPointNumOfContour(); i++ )
  {
    glVertex3f( pContour->GetPoint(i % pContour->GetPointNumOfContour()).m_x * GetEnvironment().GetWinRect().GetViewportWidth(),
                pContour->GetPoint(i % pContour->GetPointNumOfContour()).m_y * GetEnvironment().GetWinRect().GetViewportHeight(), 0.0F );
  }
  glEnd();

} // RenderContour()


#undef FILE_REVISION


// $Log: vxRendererSculpt.C,v $
// Revision 1.7  2006/05/31 16:18:22  geconomos
// porting to visual studio 2005
//
// Revision 1.6  2005/07/25 17:42:49  frank
// Issue #4305: Unable to use line loop on some hardware if there are too many points, so used line strip instead.
//
// Revision 1.5  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.4  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.3  2004/03/12 20:45:22  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 19:51:52  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/01/19 20:31:54  frank
// fixed sculpting in fullscreen mode
//
// Revision 1.15  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.14  2003/05/19 14:40:40  frank
// Removed unused return parameter
//
// Revision 1.13  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.12  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.11  2003/03/24 14:11:44  frank
// Fixed rendering of multiple aborted lassos
//
// Revision 1.10  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.9  2003/01/28 15:04:46  frank
// Const access functions should be const!
//
// Revision 1.8  2002/12/20 15:57:44  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.7  2002/05/17 14:58:08  dmitry
// Got rid of unused stuff, Push/pop opengl matrices
//
// Revision 1.6  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.5  2002/04/30 21:33:41  jenny
// vxBlockVolume stuff
//
// Revision 1.4  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.3  2002/04/24 20:47:25  manju
// Using PixelRGBA to represent color.
//
// Revision 1.2  2002/04/18 23:06:03  frank
// Changed it to use screen pixels instead of world coordinates.
//
// Revision 1.1  2002/04/18 18:42:03  frank
// Initial version.
//
// $Id: vxRendererSculpt.C,v 1.7 2006/05/31 16:18:22 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSculpt.C,v 1.7 2006/05/31 16:18:22 geconomos Exp $
