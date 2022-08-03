// $Id: vxRendererVesselDetailMPROblique.C,v 1.3 2007/10/03 16:14:06 cqian Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererVesselDetailMPROblique.h"
#include "vxEnvironment.h"
#include "vxVesselSection.h"
#include "vxElementFLoat4.h"
#include "vxRendererSoftwareMPROblique.h"
#include "Point.h"
#include "vxRendererUtils.h"
#include "ZeroOne.h"
#include "vxOverlayStrings.h"
#include "vxRendererOverlay.h"
#include "Quadrilateral.h"
#include "vxVesselUtilities.h"
#include "vxVesselPreferences.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererVesselDetailMPROblique::vxRendererVesselDetailMPROblique( vxEnvironment & environment ) :
vxRendererOverlay( environment )
{
} // vxRendererVesselDetailMPROblique()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererVesselDetailMPROblique::IsRedrawNecessary()
{
  const bool bPreferencesChanged( GetEnvironment().IsModified( vxIDs::VesselPreferences ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );

  return bPreferencesChanged || bVesselChanged;

} // IsRedrawNecessary()


/**
 * render the vessel using OpenGL
 */
void vxRendererVesselDetailMPROblique::Render()
{
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
    throw ex::NullPointerException( LogRec( "Preferences pointer is NULL", "vxRendererVesselDetailMPROblique", "Render" ) );

  vxElementFloat4 * pCurrentVesselSection = dynamic_cast< vxElementFloat4 * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
  if( pCurrentVesselSection == NULL )
    throw ex::NullPointerException( LogRec( "Current vessel section pointer is NULL", "vxRendererVesselDetailMPROblique", "Render" ) );

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel != NULL )
  {
    Rect<float4> viewPortRect(0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable( GL_LIGHTING );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_TEXTURE_2D );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height() );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();


    if( pPreferences->GetDisplayOutline() )
    {
      RenderContourLines( pCurrentVesselSection->GetValue(), Quad<float4>( 1.0F, 0.5F, 0.0F, 1.0F ), false );
      RenderContourLines( pCurrentVesselSection->GetValue(), Quad<float4>( 0.0F, 1.0F, 0.5F, 1.0F ), true );
      RenderDiameterGlyphs();
    }

    if( pPreferences->GetDisplayCenterline() )
    {
      RenderCenterline();
    }

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();  


    glPopAttrib();
  }
} // Render()


/**
 * Render the centerline
 */
void vxRendererVesselDetailMPROblique::RenderCenterline()
{
  // make sure vessel has data
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel != NULL && pVessel->size() > 0 )
  {

    vxVesselSection & section = pVessel->front();
    Point3D<float4> pos( section.GetPosition() );

    glColor4f( 0.0F, 1.0F, 0.0F, 0.75F );

    glBegin( GL_LINES );

    const float4 x( GetEnvironment().GetWinRect().GetViewportWidth() / 2 );
    const float4 y( GetEnvironment().GetWinRect().GetViewportHeight() / 2 );
    const float fSize( 5.0F );

    glVertex2f( x - fSize, y );
    glVertex2f( x + fSize, y );

    glVertex2f( x, y - fSize );
    glVertex2f( x, y + fSize );

    glEnd();
  }


  // use the following for debugging to display the current voxel (e.g., to double check the HU value)
  const bool bShowCurrentVoxel( false );
  if ( bShowCurrentVoxel )
  {
    Quadrilateral quad( GetObliqueQuadrilateral() );
    Vector3Df posVolume( * GetEnvironment().GetCurrentVoxelPos() );
    Point3Df posWorld( posVolume.ScaleAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ) );
    Point2D<float4> currentPos( quad.GetProjectedPosition( posWorld ) );

    glBegin( GL_LINES );
    glVertex2f( currentPos.m_x-.02, currentPos.m_y-.02 );
    glVertex2f( currentPos.m_x+.02, currentPos.m_y+.02 );
    glVertex2f( currentPos.m_x-.02, currentPos.m_y+.02 );
    glVertex2f( currentPos.m_x+.02, currentPos.m_y-.02 );
    glEnd();
  }
} // RenderCenterline()


/**
 * Render a contour as lines
 *
 * @param fSectionIndex the section index to render
 * @param baseColor     the color to draw it in, which may be modified somewhat depending on selection or visibility
 * @param bDrawLumen    if to draw the lumen contour instead of the wall contour
 */
void vxRendererVesselDetailMPROblique::RenderContourLines( const float4 fSectionIndex, const Quad<float4> & baseColor, const bool bDrawLumen ) const
{
  // make sure we have a valid section index
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel != NULL &&
       fSectionIndex >= 0 &&
       fSectionIndex <= pVessel->size()-1 )
  {

    // interpolate the contour
    vxVesselSection::SectionContour interpolatedContour;
    const uint4 uSectionIndexLower( fSectionIndex );
    const uint4 uSectionIndexUpper( Bound( uint4( 0 ), uint4( uSectionIndexLower + 1 ), uint4( pVessel->size()-1 ) ) );
    const ZeroOne fWeightA( 1.0F - fSectionIndex + uSectionIndexLower );
    if ( bDrawLumen )
    {
      vxVesselUtilities::GetInterpolatedContour( interpolatedContour, pVessel->at( uSectionIndexLower ).GetLumenContour(),
                                                 pVessel->at( uSectionIndexUpper ).GetLumenContour(), fWeightA );
    }
    else
    {
      vxVesselUtilities::GetInterpolatedContour( interpolatedContour, pVessel->at( uSectionIndexLower ).GetWallContour(),
                                                 pVessel->at( uSectionIndexUpper ).GetWallContour(), fWeightA );
    }

    bool bScreenSpace = GetEnvironment().GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;

    // prepare to render
    Quadrilateral quad( GetObliqueQuadrilateral() );
    glColor4f( baseColor.m_x, baseColor.m_y, baseColor.m_z, baseColor.m_t );
    glLineWidth( 1.0F );
    glBegin( GL_LINE_LOOP );

    // loop over the points
    std::vector< Point<float4> >::const_iterator contourIt( interpolatedContour.begin() );
    for( contourIt = interpolatedContour.begin(); contourIt != interpolatedContour.end(); ++contourIt )
    {
      if( bScreenSpace )
      {
        Point2D<int2> pos;
        GetEnvironment().GetViewerType()->ConvertWorldToScreen( *contourIt, pos );
        glVertex2i( pos.m_x, pos.m_y );
      }
      else
      {
        vxRendererUtils::GlVertex( *contourIt );
      }
    }

    glEnd();

  } // if valid section index

} // RenderContourLines()


/**
 * Render the diameter glyphs
 */
void vxRendererVesselDetailMPROblique::RenderDiameterGlyphs() const
{
  Quadrilateral quad( GetObliqueQuadrilateral() );

  // render the minimum diameter ( Qian Added: the segment should be represented in world coordinates )
  RaySegment minDiam = GetCurrentVesselSection()->GetDiameterPoints().first;
  glColor4f( 1.0F, 0.0F, 0.0F, 1.0F );
  glLineWidth( 1.0F );
  RenderDiameterGlyph( RaySegment( minDiam.GetStartPoint(), minDiam.GetEndPoint() ) );

  // render the maximum diameter
  glColor3f( 0.0F, 1.0F, 0.0F );
  RaySegment maxDiam = GetCurrentVesselSection()->GetDiameterPoints().second;
  glColor4f( 0.0F, 1.0F, 0.5F, 1.0F );
  RenderDiameterGlyph( RaySegment( maxDiam.GetStartPoint(), maxDiam.GetEndPoint() ) );

} // RenderDiameterGlyphs()


/**
 * Render one diameter glyph
 *
 * @param diameter a line segment to render
 */
void vxRendererVesselDetailMPROblique::RenderDiameterGlyph( const RaySegment & diameter ) const
{
  bool bScreenSpace = GetEnvironment().GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;
  if( bScreenSpace )
  {
    Point2D<int2> p1;
    GetEnvironment().GetViewerType()->ConvertWorldToScreen( Point3Df( diameter.GetStartPoint() ), p1 );

    Point2D<int2> p2;
    GetEnvironment().GetViewerType()->ConvertWorldToScreen( Point3Df( diameter.GetEndPoint() ), p2 );
    glBegin( GL_LINES );
    {
      glVertex2i( p1.m_x, p1.m_y );
      glVertex2i( p2.m_x, p2.m_y );
    }
    glEnd();
  }
  else
  {
    glBegin( GL_LINES );
    {
      vxRendererUtils::GlVertex( Point3Df( diameter.GetStartPoint() ) );
      vxRendererUtils::GlVertex( Point3Df( diameter.GetEndPoint  () ) );
    }
    glEnd();
  }

} // RenderDiameterGlyph()

/**
 * Retrieve the oblique view quadrilateral.
 *
 * @return a quadrilateral describing the current view
 */
Quadrilateral vxRendererVesselDetailMPROblique::GetObliqueQuadrilateral() const
{

  // get the data
  Point<float4> points[4];
  Quadrilateral quad( Point<float4>(0,0,0), Normal<float4>(1,0,0), Normal<float4>(0,1,0), 1.0F );
  if( vxUtils::ComputeObliqueQuadrilateralOpenGL( points, /* *GetEnvironment().GetCamera(), */ *GetEnvironment().GetPlaneOblique() ) == true )
    quad.SetPoints( points );

  return quad;

} // GetObliqueQuadrilateral()



/**
 * Retrieve the current vessel section
 *
 * @return the current vessel section
 */
vxVesselSection * vxRendererVesselDetailMPROblique::GetCurrentVesselSection() const
{
  vxElementFloat4 * pCurrentVesselSection = dynamic_cast< vxElementFloat4 * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
  if( pCurrentVesselSection == NULL )
  {
    throw ex::NullPointerException( LogRec( "Current vessel section pointer is NULL", "vxRendererVesselDetailMPROblique", "Render" ) );
    return NULL;
  }

  // round to the nearest vessel section
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  int4 iVesselSection( Bound( int4( 0 ), int4( pCurrentVesselSection->GetValue() + 0.5F ), int4( pVessel->size() - 1 ) ) );

  return & pVessel->at( iVesselSection );

} // GetCurrentVesselSection()


/**
 * Transform OpenGL for the detail volume.
 * If we are rendering from the detail volume, it is shifted away from the origin of the original volume,
 * and the vessel data all uses original world coordinates.
 */
void vxRendererVesselDetailMPROblique::TransformDetail() const
{
  Point<float4> centerPoint;
  if( GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar( "VolumeCenter", centerPoint ) )
  {
    // get the location of (0,0,0) in the original volume
    Vector<float4> worldDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
    Point<float4> volMinPoint( centerPoint - worldDim / 2.0F );

    glTranslatef( -volMinPoint.m_x, -volMinPoint.m_y, -volMinPoint.m_z );
  }
} // TransformDetail()



// $Log: vxRendererVesselDetailMPROblique.C,v $
// Revision 1.3  2007/10/03 16:14:06  cqian
// make it able to handle anisotropic blockvolume
//
// Revision 1.2  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/06/08 15:02:17  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/10/23 10:26:09  frank
// made some functions not static
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.13.2.1  2004/02/25 15:00:32  frank
// formatting
//
// Revision 1.13  2004/02/02 13:00:05  frank
// Issue #3708: Fixed bold lines on lumen and vessel outline when adjusting measurements
//
// Revision 1.12  2004/01/07 18:55:25  frank
// Updated utility function names
//
// Revision 1.11  2003/12/22 19:59:47  frank
// removed dead code
//
// Revision 1.10  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.9  2003/09/25 03:08:03  frank
// Merged oblique views into one
//
// Revision 1.8  2003/05/21 20:43:29  wenli
// change the colors of the contours
//
// Revision 1.7  2003/05/19 17:24:58  frank
// Removed unused return parameter
//
// Revision 1.6  2003/05/14 17:00:21  frank
// fixed overlay for new oblique view
//
// Revision 1.5  2003/05/14 13:54:28  frank
// Added the wall contour
//
// Revision 1.4  2003/05/14 12:02:35  frank
// Adding a new contour to the vessel section
//
// Revision 1.3  2003/05/12 20:08:21  frank
// Moved to alternate oblique viewer
//
// Revision 1.2  2003/05/08 21:06:55  geconomos
// Fixed Michael's stuff.
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.20  2003/05/02 20:15:17  frank
// code review
//
// Revision 1.19  2003/05/02 19:50:49  jmeade
// Code walkthru.
//
// Revision 1.18  2003/04/11 18:39:45  frank
// Interpolated the oblique lumen cross section
//
// Revision 1.17  2003/03/28 14:45:14  frank
// Added visualization of current voxel
//
// Revision 1.16  2003/03/26 19:45:53  frank
// Fixed diameters and contours
//
// Revision 1.15  2003/03/13 14:10:45  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.14  2003/03/07 15:52:34  frank
// Brightened the centerline crosshair
//
// Revision 1.13  2003/02/26 21:40:05  frank
// Removed red points on contour
//
// Revision 1.12  2003/02/12 17:46:00  frank
// Fixed debug message
//
// Revision 1.11  2003/01/24 20:49:50  frank
// Fixed vessel overlays.
//
// Revision 1.10  2003/01/24 18:24:36  frank
// getting ready for oblique new
//
// Revision 1.9  2003/01/22 22:19:37  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.8  2003/01/21 22:18:04  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.7  2003/01/20 12:20:21  frank
// Fixed Point2D/Point problem.
//
// Revision 1.6  2003/01/14 16:16:58  dongqing
// Code review
//
// Revision 1.5  2002/11/25 23:21:15  frank
// Made it safer
//
// Revision 1.4  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.3  2002/11/13 20:11:55  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.2  2002/11/12 19:37:09  frank
// Defaulted to Z axis view
//
// Revision 1.1  2002/11/12 16:13:59  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererVesselDetailMPROblique.C,v 1.3 2007/10/03 16:14:06 cqian Exp $
// $Id: vxRendererVesselDetailMPROblique.C,v 1.3 2007/10/03 16:14:06 cqian Exp $
