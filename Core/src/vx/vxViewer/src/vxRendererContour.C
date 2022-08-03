// $Id: vxRendererContour.C,v 1.7 2006/03/13 13:54:16 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include <list>
#include "vxRendererContour.h"
#include "vxContourList.h"
#include "vxEnvironment.h"
#include "vxContour.h"
#include "vxManipulatorContourAdjust.h"
#include "vxRendererUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Constructor
 * @param environment the environement
 */ 
vxRendererContour::vxRendererContour( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererContour()


/**
 * Render the lassos
 */
void vxRendererContour::Render()
{  
  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  RenderLassoVector();
  RenderAdjustmentContour();

  glPopAttrib();
} // Render()


/**
 * Draw the lassos in the vector
 */
void vxRendererContour::RenderLassoVector()
{

  if ( GetEnvironment().GetContourList() == NULL )
    return;

  if ( ! GetEnvironment().GetContourList()->GetLassoList().empty() )
  {
    vxContourList::LassoList::iterator lassoIter = GetEnvironment().GetContourList()->GetLassoList().begin();
    for ( ; lassoIter != GetEnvironment().GetContourList()->GetLassoList().end(); lassoIter++ )
    {  
      vxContour * pRoi = * lassoIter;
      const Vector3Df units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
      const Vector3Di volDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
      if ( IsInSlice( pRoi->GetPoint(0), units, volDim ) )
      {
        RenderSingleLasso( pRoi );
      }
    } // end for
  } // if list not empty
  
} // RenderLassoVector()


/**
 * Render the adjustment contours
 */
void vxRendererContour::RenderAdjustmentContour()
{

  // set up antialiasing
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  {
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // draw the shadow for the contour.
    glColor3f( 0.0F, 0.0F, 0.0F );
    glLineWidth( 2.0F );

    // render the adjustment contour
    glColor3f( 1, 1, 0 );
    glBegin( GL_POINTS );
    for ( std::list< Point3Df >::iterator it = GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().begin();
          it != GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().end();
          ++it )
    {
      vxRendererUtils::GlVertex( * it );
    }
    glEnd();
  }
  glPopAttrib();

} // RenderAdjustmentContour()


/**
 * Draw each lasso from the vector
 *
 * @param pRoi the lasso to draw
 */
void vxRendererContour::RenderSingleLasso( vxContour * pRoi )
{
  // convert the measure points from volume co-ord to screen co-ord.
  int4 numPoints(pRoi->GetPointNumOfContour());
  Point<float4> drawPoint;

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
  // draw the shadow for the contour.
  glColor3f(0.0F, 0.0F, 0.0F);
  glLineWidth(4.0);
  glBegin(GL_LINE_STRIP);
  uint2 i(0);
  for ( i=0; numPoints>0 && i<=numPoints; i++ )
  {
    drawPoint = pRoi->GetPoint(i % numPoints);
    glVertex3f(drawPoint.m_x, drawPoint.m_y, drawPoint.m_z);
  }
  glEnd();

  // draw the contour with yellow color.
  glColor4f(1.0F, 1.0F, 0.0F, 1.0F);
  
  // draw the contour of the area 
  glLineWidth(2.0);
  glBegin(GL_LINE_STRIP);
  for ( i=0; numPoints>0 && i<=numPoints; i++ )
  {
    drawPoint = pRoi->GetPoint(i % numPoints);
    glVertex3f(drawPoint.m_x, drawPoint.m_y, drawPoint.m_z);
  }
  glEnd();
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);  
} // RenderSingleLasso()


/**
 * Check if the current slice is the one in which the annotation was created in
 *
 * @param handlePoint end/handle point of the annotation
 * @param units volume units
 * @volDim volume dimnesions
 * @return true if in slice
 */
bool vxRendererContour::IsInSlice(const Point3Df & handlePoint, 
                                  const Vector3D<float4> & units,
                                  const Vector3D<int4> & volDim)
{
  // get the slice orientation.
  uint4 uIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

  // get the current slice index
  uint4 iCurrentSliceIndex( vxUtils::CalculateOffsetDistance( GetEnvironment().GetPlaneOrtho()->GetDistance() / units[ uIndex ] + 0.5F,
                                                              volDim[uIndex],
                                                              GetEnvironment().GetOffset(),
                                                              GetEnvironment().GetMaximumOffset() ) );

  // get the actual lasso slice number by dividing by the volume units.
  int4 iCurrentLassoSliceNumber( handlePoint[ uIndex ] / units[ uIndex ] + 0.5F );

  return iCurrentLassoSliceNumber == iCurrentSliceIndex;

} // IsInSlice()


// undefines
#undef FILE_REVISION


// $Log: vxRendererContour.C,v $
// Revision 1.7  2006/03/13 13:54:16  frank
// added adjustment contours
//
// Revision 1.6  2005/07/25 17:42:49  frank
// Issue #4305: Unable to use line loop on some hardware if there are too many points, so used line strip instead.
//
// Revision 1.5  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.4  2004/04/06 16:26:30  frank
// unified environment access naming
//
// Revision 1.3  2004/03/12 19:51:37  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 18:04:22  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/01/19 20:31:54  frank
// fixed sculpting in fullscreen mode
//
// Revision 1.11  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.10  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.9  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.8  2003/05/15 20:09:59  frank
// Moving away from old point class
//
// Revision 1.7  2003/05/13 17:43:24  frank
// code review
//
// Revision 1.6  2003/04/10 19:32:15  frank
// Issue #3062: Fixed display of contours.
//
// Revision 1.5  2003/01/22 22:05:29  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.4  2002/11/12 21:29:22  geconomos
// Made vxContour  a first class citizen of vxEnvironment ( frank really did it ).
//
// Revision 1.3  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.2  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.5  2002/09/17 21:38:13  dmitry
// Switched some stuff to vxID (s).
//
// Revision 1.4  2002/09/16 20:20:39  dmitry
// New vxID (s) used.
//
// Revision 1.3  2002/08/21 18:32:05  michael
// removed redundant include (GLFontGeorge)
//
// Revision 1.2  2002/07/11 21:09:11  manju
// Speed up.
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.1  2002/06/26 17:26:15  manju
// Renderer to draw the restricted lasso.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererContour.C,v 1.7 2006/03/13 13:54:16 frank Exp $
// $Id: vxRendererContour.C,v 1.7 2006/03/13 13:54:16 frank Exp $
