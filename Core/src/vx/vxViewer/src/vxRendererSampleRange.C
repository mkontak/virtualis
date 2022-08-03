// $Id: vxRendererSampleRange.C,v 1.2 2004/04/06 16:27:40 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererSampleRange.h"
#include "vxEnvironment.h"
#include "GLEnv.h"
#include "GLFontGeorge.h"
#include "ResourceImage.h"
#include "vxUtils.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerType3D.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 * @param environment the environment
 */
vxRendererSampleRange::vxRendererSampleRange( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererSampleRange()


/**
 * Simply draws an OpenGL circle around current mouse position.
 * Currently used only for segmentation to display sample region.
 */
void vxRendererSampleRange::DrawCircle()
{

  // get the information from the environment on where to draw the circle
  const Triplef & units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );

  // get the total number of slices
  const uint4 uIndex( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  const uint2 uNumberOfSlices( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim()[ uIndex ] ); 

  // get the current slice index
  const int4 iSliceNum( vxUtils::CalculateOffsetDistance( 0.5F + GetEnvironment().GetPlaneOrtho()->GetDistance()/units[uIndex],
                                                          uNumberOfSlices, 
                                                          GetEnvironment().GetOffset(), 
                                                          GetEnvironment().GetMaximumOffset() ) );
  Point3Df volumeIndexCoord( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler().GetSeedPoint() );
  Triplef center( volumeIndexCoord[0]*units[0], volumeIndexCoord[1]*units[1], volumeIndexCoord[2]*units[2] );
  bool bIsWithinPlaneDistance( fabs( center[uIndex] - iSliceNum * units[uIndex] ) < 0.5F );

  if ( bIsWithinPlaneDistance )
  {

    glBegin(GL_LINE_STRIP);

    const float4 fDeltaDeg( static_cast< float4 >( M_PI / 90.0F ) );
    for (float4 fAngle(0.0F); fAngle<=(2.0F*M_PI+fDeltaDeg); fAngle+=fDeltaDeg)
    {
      float4 x, y, z;
      const float4 & fRadius( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampleRadius() );
      if (uIndex == 2)
      {
        x = center[0] + fRadius*cos(fAngle);
        y = center[1] + fRadius*sin(fAngle);
        z = center[2];
      }
      else if (uIndex == 1)
      {
        x = center[0] + fRadius*cos(fAngle);
        y = center[1];
        z = center[2] + fRadius*sin(fAngle);
      }
      else
      {
        x = center[0];
        y = center[1] + fRadius*cos(fAngle);
        z = center[2] + fRadius*sin(fAngle);
      }
      glVertex3f(x,y,z);
    } 
    glEnd();
  } // are we within the range?

} // DrawCircle()


/**
 * draw a circle in 3D within the given oblique quadrilateral
 *
 * @param quadrilateral the quadrilateral within which to draw the circle
 */
void vxRendererSampleRange::DrawObliqueCircle( const Quadrilateral & quadrilateral )
{

  const Point3Df centerPoint( quadrilateral.GetPoint( Quadrilateral::MIDDLE ) );
  const Vector3Df uRadius( 0.5F * ( quadrilateral.GetPoint( Quadrilateral::BOTTOM_RIGHT ) - quadrilateral.GetPoint( Quadrilateral::BOTTOM_LEFT ) ) );
  const Vector3Df vRadius( 0.5F * ( quadrilateral.GetPoint( Quadrilateral::TOP_LEFT ) - quadrilateral.GetPoint( Quadrilateral::BOTTOM_LEFT ) ) );

  glBegin(GL_LINE_LOOP);
  {
    const uint4 uSteps( 36 );
    for ( int4 i(0); i<uSteps; ++i )
    {
      const float4 fAngleRadians( 2 * M_PI * float4(i) / uSteps );
      Point3Df vertex( centerPoint + uRadius * cos( fAngleRadians ) + vRadius * sin( fAngleRadians ) );
      glVertex3f( vertex.m_x, vertex.m_y, vertex.m_z );
    }
  }
  glEnd();

} // DrawObliqueCircle()


/** 
 * Has something changed that makes rendering necessary?
 * @return if redraw is necessary
 */
bool vxRendererSampleRange::IsRedrawNecessary()
{

  bool bRedrawNecessary(false);

  if (GetEnvironment().GetInteractiveSegmentation() != NULL)
  {
    bRedrawNecessary |= GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowMethod() == SegParams::SAMPLE_INTENSITY_RANGE;
  }

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Renders the circle from which the sample range is determined
 */  
void vxRendererSampleRange::Render()
{
  if (GetEnvironment().GetInteractiveSegmentation() != NULL)
  {
    if (GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowMethod() == SegParams::SAMPLE_INTENSITY_RANGE)
    {

      // save the OpenGL state
      glPushAttrib(GL_ALL_ATTRIB_BITS);

      glDisable(GL_LIGHTING);
      glDisable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_TEXTURE_2D);

      // draw the circle: This is done drawing three circle 
      // with a black line and then a green one in the middle
      // of these three circles (sort of a shadow effect)
      glEnable(GL_LINE_SMOOTH);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

      const RegionSampler & regionSampler( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler() );

      // orthographic MPR
      if ( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) != NULL &&
           ( regionSampler.GetSamplingPlane() == RegionSampler::XY_PLANE ||
             regionSampler.GetSamplingPlane() == RegionSampler::XZ_PLANE ||
             regionSampler.GetSamplingPlane() == RegionSampler::YZ_PLANE ) )
      {

        // draw shadow circle
        glColor3f(0,0,0);
        glLineWidth(4.0);
        DrawCircle();

        // draw actual circle
        glColor3f(0,1,0);
        glLineWidth(1.5);
        DrawCircle();

      } // MPR ortho viewer type

      // oblique MPR
      else if ( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL &&
                regionSampler.GetSamplingPlane() == RegionSampler::OBLIQUE_PLANE )
      {

        // draw shadow circle
        glColor3f(0,0,0);
        glLineWidth(4.0);
        DrawObliqueCircle( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler().GetObliqueQuadrilateral() );

        // draw actual circle
        glColor3f(0,1,0);
        glLineWidth(1.5);
        DrawObliqueCircle( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler().GetObliqueQuadrilateral() );

        
      } // MPR oblique viewer type

      // 3D viewer
      else if ( dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() ) != NULL &&
                regionSampler.GetSamplingPlane() == RegionSampler::SPHERICAL )
      {

        const Point3Df volumeIndexCoord( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampler().GetSeedPoint() );
        const Triplef & units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
        const Point3Df center( volumeIndexCoord[0]*units[0], volumeIndexCoord[1]*units[1], volumeIndexCoord[2]*units[2] );
        const float4 & fRadius( GetEnvironment().GetInteractiveSegmentation()->GetRegionSampleRadius() );

        glColor3f(0,1,0);
        glLineWidth(1.5);
        RenderSphereWireframe( center, fRadius );

      } // 3D viewer

      // restore the OpenGL state
      glPopAttrib();

    } // SAMPLE_INTENSITY as region grow method?
  } // interactive segmentation object != NULL?
  
} // Render()


/**
 * Constructor
 */
vxRendererSampleRange::vxCirclePath::vxCirclePath( const uint4 uNumSteps, const float4 fRadius )
{
  for ( int i=0; i<uNumSteps; ++i )
  {
    const float4 fAngleRadians( M_PI * 2 * float4( i ) / float4( uNumSteps ) );
    m_circlePoints.push_back( Point2D< float4 >( fRadius * cos( fAngleRadians ), fRadius * sin( fAngleRadians ) ) );
  }
} // CirclePath()

/**
 * Render the circle in the XY plane
 */
void vxRendererSampleRange::vxCirclePath::RenderXY() const
{
  glBegin(GL_LINE_LOOP);
  for ( std::vector< Point2D< float4 > >::const_iterator it( m_circlePoints.begin() ); it != m_circlePoints.end(); ++it )
    glVertex3f( it->m_x, it->m_y, 0 );
  glEnd();
} // RenderXY()


/**
 * Render the circle in the YZ plane
 */
void vxRendererSampleRange::vxCirclePath::RenderYZ() const
{
  glBegin(GL_LINE_LOOP);
  for ( std::vector< Point2D< float4 > >::const_iterator it( m_circlePoints.begin() ); it != m_circlePoints.end(); ++it )
    glVertex3f( 0, it->m_x, it->m_y );
  glEnd();
} // RenderYZ()


/**
 * Render the circle in the XZ plane
 */
void vxRendererSampleRange::vxCirclePath::RenderXZ() const
{
  glBegin(GL_LINE_LOOP);
  for ( std::vector< Point2D< float4 > >::const_iterator it( m_circlePoints.begin() ); it != m_circlePoints.end(); ++it )
    glVertex3f( it->m_x, 0, it->m_y );
  glEnd();
} // RenderXZ()


/**
 * Render a wireframe sphere
 *
 * @param position the center position in world coordinates
 * @param fRadius  the radius of the sphere
 */
void vxRendererSampleRange::RenderSphereWireframe( const Point3Df & position, const float4 fRadius ) const
{
  // pre-compute a circle path
  vxCirclePath path( 36, fRadius );

  /// draw it three times for the three major axes
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
    glTranslatef( position.m_x, position.m_y, position.m_z );

    // draw in the z=0 plane
    glPushMatrix();
    path.RenderXY();
    glPopMatrix();

    // draw in the x=0 plane
    glPushMatrix();
    path.RenderYZ();
    glPopMatrix();

    // draw in the y=0 plane
    glPushMatrix();
    path.RenderXZ();
    glPopMatrix();
  }
  glPopMatrix();
} // RenderSphereWireframe()


#undef FILE_REVISION 


// $Log: vxRendererSampleRange.C,v $
// Revision 1.2  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2004/01/23 16:23:56  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.19  2004/01/05 19:04:41  michael
// added "mailto:"
//
// Revision 1.18  2003/12/23 21:36:14  michael
// only draw sample range circle if we are MPROrtho
//
// Revision 1.17  2003/12/19 13:52:36  michael
// took out logging information
//
// Revision 1.16  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.15  2003/12/16 19:07:01  frank
// removed compile warning
//
// Revision 1.14  2003/09/23 22:08:27  michael
// added "mailto:"
//
// Revision 1.13  2003/08/20 15:48:18  michael
// using M_PI instead of typed in values for it
//
// Revision 1.12  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.11  2003/05/19 14:40:40  frank
// Removed unused return parameter
//
// Revision 1.10  2003/05/16 20:46:41  dongqing
// code review
//
// Revision 1.9  2003/05/16 18:38:39  michael
// code review
//
// Revision 1.8  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.7  2003/01/22 22:02:53  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.6  2002/11/12 00:11:53  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.5  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.4  2002/10/25 19:25:16  ingmar
// fixed comment
//
// Revision 1.3  2002/06/21 18:04:30  jaddonisio
// Changes to render sample circle(need to revisit because it doesn't seem quite right).
//
// Revision 1.2  2002/06/19 18:40:53  michael
// added saving of OpenGL state so that certain modes can be switched off savely
//
// Revision 1.1  2002/06/19 18:38:00  michael
// added renderer for the smaple range circle needed to display the region used
// by the interactive segmentation.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSampleRange.C,v 1.2 2004/04/06 16:27:40 frank Exp $
// $Id: vxRendererSampleRange.C,v 1.2 2004/04/06 16:27:40 frank Exp $
