// $Id: vxRendererPath3D.C,v 1.8 2006/09/25 13:22:53 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * Renders the vessel centerline and outline in 3D.
 */

// Includes
#include "stdafx.h"
#include "vxRendererPath3D.h"
#include "vxEnvironment.h"
#include "vxVessel.h"
#include "vxVesselSection.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerType3D.h"
#include "vxRendererUtils.h"
#include "Line.h"
#include "Plane.h"
#include "RaySegment.h"
#include "Quad.h"
#include "Box.h"
#include "vxRenderingMode.h"


// Defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererPath3D::vxRendererPath3D(vxEnvironment & environment)
: vxRenderer(environment)
{
} // Constructor


/**
 * Destructor
 */
vxRendererPath3D::~vxRendererPath3D()
{
} // Destructor


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererPath3D::IsRedrawNecessary()
{

  bool bRedrawNecessary(false);

  if (GetPath() != NULL)
  {
    bRedrawNecessary |= GetEnvironment().IsModified(vxIDs::Path) &&
                        GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_VESSEL_CENTERLINE) == true;

    bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE);
    bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::WIN_RECT);
    
  }

  return bRedrawNecessary;

} // IsRedrawNecessary()



/**
 * Render the vessel using OpenGL
 */
void vxRendererPath3D::Render()
{

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // possibly scale for detail volume
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // set up antialiasing
  //glEnable(GL_LINE_SMOOTH);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glLineWidth(2.0F);
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // render centerline and glyphs
  if (GetPath() != NULL &&
       GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_VESSEL_CENTERLINE) == true)
  {
    RenderCenterline();
  }

  // restore the OpenGL state
  glPopMatrix();
  glPopAttrib();

} // Render()


/**
 * Retrieve the current vessel
 * @return a pointer to the current vessel or NULL if none current
 */
vxVessel * vxRendererPath3D::GetPath() const
{

  // get the vessel
  vxVessel * pVessel(dynamic_cast<vxVessel *>(GetEnvironment().GetElement(vxIDs::Path)));
  if (pVessel == NULL)
  {
    //LogDbg("Vessel pointer is NULL", "vxRendererPath3D", "GetPath");
    return NULL;
  }

  return pVessel;

} // GetPath()


/**
 * Render the centerline
 */
void vxRendererPath3D::RenderCenterline() const
{
  const Box<float4> & visibleBox(GetEnvironment().GetCropBox()->GetBox());
  const bool bCropBox(GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX));
  const float4 fResampleInverval(0.1F);

  // 2D orthogonal MPR
  //////////////////////
  if (dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()))
  {
    const uint1 uAxisIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
    float4 fDistanceBound(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ]);
    const vxPlaneOblique<float4> * pVisiblePlane(GetEnvironment().GetPlaneOrtho());

    // if slab
    if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() != vxRenderingMode::DEFAULT)
      fDistanceBound += GetEnvironment().GetRenderingMode()->GetSlabThickness() / 2;

    RenderResampledCenterline(true, pVisiblePlane, fDistanceBound, bCropBox, visibleBox, fResampleInverval);
  } // 2D orthogonal MPR

  // 2D oblique MPR
  ///////////////////
  else if (dynamic_cast<vxViewerTypeMPROblique *>(GetEnvironment().GetViewerType()))
  {
    float4 fDistanceBound(2.5F);
    const vxPlaneOblique<float4> * pVisiblePlane(GetEnvironment().GetPlaneOblique());

    // if slab
    if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP ||
         GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::XRAY)
      fDistanceBound += GetEnvironment().GetRenderingMode()->GetSlabThickness() / 2;

    RenderResampledCenterline(true, pVisiblePlane, fDistanceBound, bCropBox, visibleBox, fResampleInverval);
  } // 2D oblique MPR

  // full 3D view
  /////////////////
  if (dynamic_cast<vxViewerType3D *>(GetEnvironment().GetViewerType()))
  {
    RenderResampledCenterline(false, NULL, 0.0F, bCropBox, visibleBox, fResampleInverval);
  } // end if 3D view

} // RenderCenterline()


/** 
 * Resample and render the centerline
 *
 * @param bBoundPlane      if the visibility should be constrained to nearby a plane
 * @param visiblePlane     the plane to constrain visibility
 * @param fPlaneDistance   the distance from a plane that should be visible
 * @param bBoundBox        if the visibility should be constrained to within a box
 * @param visibleBox       the box to constrain visibility
 * @param resampleInterval how close to make individual sample points
 */
void vxRendererPath3D::RenderResampledCenterline(const bool bBoundPlane, 
                                                 const vxPlaneOblique<float4> * pVisiblePlane, 
                                                 const float4 fPlaneDistance,
                                                 const bool bBoundBox, 
                                                 const Box<float4> & visibleBox, 
                                                 const float4 fResampleInterval) const
{
  try
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // loop over all pairs of points along vessel
    for (int4 i(0); i<GetPath()->size()-1; ++i)
    {
      const Point3Df startPoint(GetPath()->at(i + 0).GetPosition());
      const Point3Df   endPoint(GetPath()->at(i + 1).GetPosition());
      const float4 fLength((endPoint - startPoint).GetLength() - fResampleInterval);

      // sample along the line segment
      for (float4 j(0.0F); fLength> 0.0F && j <fLength; j += fResampleInterval)
      {
        const Normal3Df directionVector(endPoint - startPoint);
        const Point3Df samplePos(startPoint + Vector3Df(directionVector) * j);
        const Point3Df samplePosNext(startPoint + Vector3Df(directionVector) * (j+fResampleInterval));

        // check the proximity to the plane to determine the color
        if (bBoundPlane == false || pVisiblePlane->GetAbsDistance(samplePos) <fPlaneDistance)
        {
          glColor4f(0.0F, 1.0F, 0.0F, 1.0F);
          if (bBoundPlane == false) //3D
            glLineWidth(1.0);
          else
            glLineWidth(4.0);
        }
        else
        {
          glColor4f(0.0F, 1.0F, 0.0F, 0.4F);
          glLineWidth(1.0);
        }

        // check the visibility with the crop box
        if (bBoundBox == false || visibleBox.IsInside(Triplef(samplePos)))
        {
          glBegin(GL_LINES);
          vxRendererUtils::GlVertex(samplePos);
          vxRendererUtils::GlVertex(samplePosNext);
          glEnd();
        }
      } // for all sample points along line
    } // for all pairs of points along vessel

    // draw the final point
    //if (GetPath()->size()> 0)
    //  vxRendererUtils::GlVertex(GetPath()->back().GetPosition());

  }
  catch (...)
  {
    LogDbg("out of range: " + ToAscii(GetPath()->size()), "", "");
  }

} // DrawResampledCenterline()


// undefines
#undef FILE_REVISION


// Revision History:
// $Log: vxRendererPath3D.C,v $
// Revision 1.8  2006/09/25 13:22:53  geconomos
// commented out unneeded  log debugging
//
// Revision 1.7  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.6  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.6.1  2005/07/19 11:50:33  frank
// missing include
//
// Revision 1.5  2004/10/25 13:30:16  frank
// was sampling too far into path
//
// Revision 1.4  2004/10/05 14:32:54  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.3  2004/07/28 19:37:33  michael
// adjusted the path drawing to show it in a more understandable fashion
//
// Revision 1.2  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.1  2004/04/06 18:19:41  frank
// renamed vessel to path
//
// Revision 1.2  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2004/01/30 01:21:35  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.12  2004/01/16 13:19:13  frank
// Issue #3525: Added display of path on oblique slices/slabs
//
// Revision 1.11  2004/01/07 17:13:25  frank
// fixed re-render problem when you change the visualization option to render the centerline.
//
// Revision 1.10  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.9  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.8  2003/11/05 01:18:49  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.7  2003/08/21 20:38:11  dongqing
// remove redundant codes
//
// Revision 1.6  2003/08/21 19:46:55  dongqing
// finialize the centerline renderer
//
// Revision 1.5  2003/08/20 21:17:23  dongqing
// improving the path rendering effect
//
// Revision 1.4  2003/08/19 19:35:10  dongqing
// let the path only showing the intersection within [-5, 5] mm interval
//
// Revision 1.3  2003/08/14 16:59:59  frank
// Checked the rendering mode when deciding whether to draw the centerline
//
// Revision 1.2  2003/08/13 11:27:07  frank
// Removed pointers to vessel sections
//
// Revision 1.1  2003/08/12 16:57:31  frank
// Moved the vessel tools down to vxBase
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererPath3D.C,v 1.8 2006/09/25 13:22:53 geconomos Exp $
// $Id: vxRendererPath3D.C,v 1.8 2006/09/25 13:22:53 geconomos Exp $
