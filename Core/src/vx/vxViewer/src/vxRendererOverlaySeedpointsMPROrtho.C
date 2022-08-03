// $Id: vxRendererOverlaySeedpointsMPROrtho.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

/**
 * Renders the vessel centerline and outline in 3D.
 */


// includes
#include "StdAfx.h"
#include "vxRendererOverlaySeedpointsMPROrtho.h"
#include "vxSeedpoints.h"
#include "vxViewerTypeMPROrtho.h"
#include "PixelRGB.h"
#include "vxOverlayStrings.h"
#include "vxEnvironment.h"

// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererOverlaySeedpointsMPROrtho::vxRendererOverlaySeedpointsMPROrtho(vxEnvironment & environment)
:vxRendererOverlay(environment)
{
} // Constructor


/**
 * Destructor
 */
vxRendererOverlaySeedpointsMPROrtho::~vxRendererOverlaySeedpointsMPROrtho()
{
} // Destructor


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlaySeedpointsMPROrtho::IsRedrawNecessary()
{

  return GetEnvironment().IsModified( vxIDs::Seedpoints );

} // IsRedrawNecessary()


/**
 * Render the vessel using OpenGL
 */
void vxRendererOverlaySeedpointsMPROrtho::Render()
{
  // save the rendering projection state for proper projection later
  glGetIntegerv( GL_VIEWPORT, m_viewport );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_mModelviewMatrix );
  glGetDoublev( GL_PROJECTION_MATRIX, m_mProjectionMatrix );

  // save the transormation for the detail volume
  m_vDetailTransform = GetDetailTranslation();

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // set up antialiasing
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glLineWidth( 1.0F );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

  SetupRenderState();

  RenderSeedpoints();

  RestoreRenderState();

  // restore the OpenGL state
  glPopAttrib();

} // Render()


/**
 * Get the translation necessary to move a point from original world to detail world coordinates.
 *
 * @return the requestion translation vector
 */
Vector3Df vxRendererOverlaySeedpointsMPROrtho::GetDetailTranslation() const
{
  Vector3Df translation;
  Point3Df centerPoint;

  if( GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar( "VolumeCenter", centerPoint ) )
  {
    // get the location of (0,0,0) in the original volume
    Vector<float4> worldDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
    Point<float4> volMinPoint( centerPoint - worldDim / 2.0F );

    translation = Vector3Df( -volMinPoint.m_x, -volMinPoint.m_y, -volMinPoint.m_z );
  }

  return translation;
} // TransformDetail()


/**
 * Render the seedpoints
 */
void vxRendererOverlaySeedpointsMPROrtho::RenderSeedpoints() const
{
  const float4 fRadius( 5.0F );
  const float4 fTrackPointRadius( 2.0F );

  vxSeedpoints * pSeedpoints = dynamic_cast< vxSeedpoints * >( GetEnvironment().GetElement( vxIDs::Seedpoints ) );
  if ( pSeedpoints == NULL )
    return;

  // convert the points to world coordinates
  const Triple<float4> & volUnits = GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits();

  // render seedpoints
  for ( int4 s(0); s < pSeedpoints->GetNumClicks(); ++s )
  {
    // choose a color
    const PixelRGB<float4> color( 0, 1, 0 );

    Point3Df pointWorld( pSeedpoints->GetSeedPointAsFloat(s).GetScaledAnIso( volUnits ) + m_vDetailTransform );
    RenderSeedpoint( s + 1, pointWorld, fRadius, color );

    const std::vector< Point3Df > & path0( pSeedpoints->GetTrackPath0(s) );
    for ( int4 t(0); t < path0.size(); ++t )
    {
      Point3Df pointWorld( path0[t].GetScaledAnIso( volUnits ) + m_vDetailTransform );
      RenderSeedpoint( -1, pointWorld, fTrackPointRadius, color );
    }
    const std::vector< Point3Df > & path1( pSeedpoints->GetTrackPath1(s) );
    for ( int4 t(0); t < path1.size(); ++t )
    {
      Point3Df pointWorld( path1[t].GetScaledAnIso( volUnits ) + m_vDetailTransform );
      RenderSeedpoint( -1, pointWorld, fTrackPointRadius, color );
    }

  }

  // render the cursor point
  if ( pSeedpoints->GetCursorSeedpoint().IsSeedpointValid() )
  {
    const Point3Df pointWorld( pSeedpoints->GetCursorSeedpoint().GetSeedpoint().GetScaledAnIso( volUnits ) + m_vDetailTransform );
    RenderSeedpoint( pSeedpoints->GetNumClicks() + 1, pointWorld, fRadius * 0.75F, PixelRGB<float4>( 1.0F, 1.0F, 1.0F ) );
  }

} // RenderSeedpoints()


/**
 * Render a seedpoint and label
 *
 * @param uSeedNumber which seedpoint to render
 * @param worldPos    the world position of the seedpoint
 * @param fRadius     the radius of the circle
 * @param color       the color of the circle
 *
 * @return 
 */
void vxRendererOverlaySeedpointsMPROrtho::RenderSeedpoint( const int4 uSeedNumber, const Point3Df & worldPos,
                                                    const float4 fRadius, const PixelRGB<float4> & color ) const
{
  if ( ! GetEnvironment().GetViewerType()->IsVisible( worldPos ) )
    return;

  // check if on screen
  Point2D<int2> screenPos;
  const GLint * piViewport = & m_viewport[0];

  if ( ! GetEnvironment().GetViewerType()->ConvertWorldToScreen( worldPos, screenPos, piViewport, m_mModelviewMatrix, m_mProjectionMatrix ) )
    return;

  RenderCircle( worldPos, fRadius, color );

  if ( uSeedNumber >= 0 )
  {
    // display the point number
    vxOverlayStrings::vxOverlayElement overlayElement( ToAscii( uSeedNumber ), vxOverlayStrings::vxOverlayElement::Random );
    overlayElement.SetRandomScreenPosition( Point2D<float4>( screenPos.m_x + 10, screenPos.m_y + 5 ) );

    // retrieve the overlay strings
    vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
    if ( pOverlayStrings == NULL )
    {
      LogWrn( "Unable to find overlay strings", "", "" );
      return;
    }

    // queue it up for drawing
    pOverlayStrings->AddElement( overlayElement );  
  }

} // RenderSeedpoint()


/**
 * Render a circle
 *
 * @param position the center position in world coordinates
 * @param fRadius  the radius of the circle
 */
void vxRendererOverlaySeedpointsMPROrtho::RenderCircle( const Point3Df & worldPos, const float4 fRadius, const PixelRGB<float4> & color ) const
{
  // project to screen
  Point2D<int2> screenPos;
  const GLint * piViewport = & m_viewport[0];
  const Triple<float4> & volUnits( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  if ( GetEnvironment().GetViewerType()->ConvertWorldToScreen( worldPos, screenPos, piViewport, m_mModelviewMatrix, m_mProjectionMatrix ) == false )
    return;

  // pre-compute a circle path
  const uint4 uNumSteps( 36 );
  std::vector< Point3Df > circlePoints;
  for ( int i=0; i<uNumSteps; ++i )
  {
    const float4 fAngleRadians( M_PI * 2 * float4( i ) / float4( uNumSteps ) );
    circlePoints.push_back( Point3Df( fRadius * cos( fAngleRadians ), fRadius * sin( fAngleRadians ), 0 ) );
  }

  /// draw it with a shadow
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {

    // draw circle at location
    glTranslatef( screenPos.m_x, screenPos.m_y, 0 );

    // draw a black shadow underneath
    glLineWidth( 2.0F );
    glColor3f( 0, 0, 0 );
    glBegin(GL_LINE_LOOP);
    std::vector< Point3Df >::const_iterator it( circlePoints.begin() );
    for ( ; it != circlePoints.end(); ++it )
      glVertex3f( it->m_x, it->m_y, 0 );
    glEnd();

    // draw the primary color on top
    glLineWidth( 1.0F );
    glColor3f( color.R(), color.G(), color.B() );
    glBegin(GL_LINE_LOOP);
    for ( it = circlePoints.begin(); it != circlePoints.end(); ++it )
      glVertex3f( it->m_x, it->m_y, 0 );
    glEnd();

  }
  glPopMatrix();
} // RenderCircle()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlaySeedpointsMPROrtho.C,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/09/20 14:35:09  geconomos
// moved definition of "SeedPoints" id to vxViewer
//
// Revision 1.4  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.3  2005/06/08 15:01:32  vxconfig
// formatting
//
// Revision 1.2  2004/10/23 10:25:38  frank
// made some functions not static
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1  2004/05/26 12:23:15  frank
// initial version
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.13.2.1  2004/02/25 15:00:16  frank
// formatting
//
// Revision 1.13  2004/01/30 16:52:06  wenli
// Fixed: IsVisible returns true always.
//
// Revision 1.12  2004/01/26 16:40:23  wenli
// Seed point number Y-axis flipped -- fixed
//
// Revision 1.11  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.10  2004/01/13 19:14:35  frank
// code review
//
// Revision 1.9  2004/01/05 20:22:55  frank
// formatting
//
// Revision 1.8  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.7  2003/10/30 17:20:31  wenli
// Issue # 3544 : The centerline numbers can hide from the 2D images on Vessel select layout..
//
// Revision 1.6  2003/10/13 20:23:43  wenli
// vxSeedPoint data structure are changed
//
// Revision 1.5  2003/10/07 21:08:47  wenli
// render the tracking seeds
//
// Revision 1.4  2003/10/07 17:04:16  frank
// Issue #3533: Added seedpoint numbers
//
// Revision 1.3  2003/06/10 12:06:56  frank
// Code review of pixel classes
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.6  2003/05/01 20:25:57  frank
// code review
//
// Revision 1.5  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.4  2003/04/23 13:54:39  wenli
// multi-seed points
//
// Revision 1.3  2003/04/18 17:49:12  frank
// Added 2D preview of seedpoints
//
// Revision 1.2  2003/03/28 12:55:31  frank
// Clarified parameter name
//
// Revision 1.1  2003/03/20 21:29:44  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlaySeedpointsMPROrtho.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlaySeedpointsMPROrtho.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
