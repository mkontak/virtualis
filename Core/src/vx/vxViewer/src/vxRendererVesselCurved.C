// $Id: vxRendererVesselCurved.C,v 1.1 2006/08/17 14:01:12 gdavidson Exp $
//
// Copyrightv © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * Renders the vessel centerline and outline in 3D.
 */

// includes
#include "StdAfx.h"
#include "vxRendererVesselCurved.h"
#include "vxVesselPreferences.h"
#include "vxIDs.h"
#include "vxRendererUtils.h"
#include "vxRibbon.h"
#include "vxVesselUtilities.h"
#include "vxRendererOverlay.h"
#include "vxOverlayStrings.h"
#include "PrimitiveUtilities.h"
#include "Quad.h"
#include "vxEnvironment.h"
#include "Vector4d.h"
#include "RaySegment.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererVesselCurved::vxRendererVesselCurved( vxEnvironment & environment ) :
vxRendererOverlay( environment )
{
} // vxRendererVesselCurved()


/**
 * Destructor
 */
vxRendererVesselCurved::~vxRendererVesselCurved()
{
} // ~vxRendererVesselCurved()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererVesselCurved::IsRedrawNecessary()
{
  const bool bPreferencesChanged( GetEnvironment().IsModified( vxIDs::VesselPreferences ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );

  return bPreferencesChanged || bVesselChanged;
} // IsRedrawNecessary()


/**
 * Render the vessel using OpenGL
 */
void vxRendererVesselCurved::Render()
{
  // check if we have a vessel to render
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL ) return;

  // see if the user wishes to display the graphs
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
  {
    LogDbg( "Preferences pointer is NULL", "vxRendererVesselCurved", "RenderWindow" );
    return;
  }

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  SetupRenderState();

  glDisable( GL_LIGHTING );
  glEnable( GL_BLEND );
  glEnable( GL_LINE_SMOOTH );
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_TEXTURE_2D );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glLineWidth( 2.0F );

  // render outline
  if ( pPreferences->GetDisplayOutline() ) RenderContours();

  // render centerline
  if (pPreferences->GetDisplayCenterline() ) RenderCenterline();

  // render graph information
  if ( pPreferences->GetDisplayCenterline() || pPreferences->GetDisplayGraphs() ) RenderSelectionHandles();

  // restore the OpenGL state
  vxRendererOverlay::RestoreRenderState();
  glPopAttrib();

} // Render()


/**
 * Get the intersection of the centerline with an image scanline
 *
 * @param pRibbon  the ribbon data structure
 * @param uImageY the scanline location (y) in screen coordinates
 *
 * @return         the centerline location (x) in screen coordinates or -1 if no intersection
 */
float4 vxRendererVesselCurved::GetCenterlineXLocationFromImageY( vxRibbon * pRibbon, const uint4 uImageY ) const
{
  float4 fScreenX( -1.0F );

  // check bounds
  if ( uImageY < pRibbon->GetScanlineData().size() )
  {

    RaySegment scanlineSegment( Vector4Df( pRibbon->GetScanlineData()[uImageY].m_leftPoint , 0 ),
                                Vector4Df( pRibbon->GetScanlineData()[uImageY].m_rightPoint, 1 ) );

    RaySegment centerlineSegment( Vector4Df( pRibbon->GetScanlineData()[uImageY].m_leftPoint      , 0 ),
                                  Vector4Df( pRibbon->GetScanlineData()[uImageY].m_centerlinePoint, 1 ) );

    
    if ( scanlineSegment.GetLength() > 0.0F )
    {
      const float4 fPercent( PrimitiveUtilities::GetPercentAlongSegment( scanlineSegment,
        pRibbon->GetScanlineData()[ uImageY ].m_centerlinePoint ) );
      if ( fPercent >= 0.0 && fPercent <= 1.0F )
      {
        fScreenX = ( GetEnvironment().GetWinRect().GetViewportWidth() - 1 ) * fPercent;
      }
    }      
  }

  return fScreenX;
} // GetCenterlineXLocationFromImageY()


/**
 * Get the intersection of the centerline with a screen scanline
 *
 * @param pRibbon  the ribbon data structure
 * @param uScreenY the scanline location (y) in screen coordinates
 *
 * @return         the centerline location (x) in screen coordinates or -1 if no intersection
 */
float4 vxRendererVesselCurved::GetCenterlineXLocationFromScreenY( vxRibbon * pRibbon, const uint4 uScreenY ) const
{
  // convert from screen y to image y
  const int4 iScanline( Interpolate( float4( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ), float4( uScreenY ),
                                     0.0F, float4( pRibbon->GetScanlineData().size() - 1 ), 0.0F ) );

  return GetCenterlineXLocationFromImageY( pRibbon, iScanline );

} // GetCenterlineXLocationFromScreenY()


/**
 * Render the selection handles
 */
void vxRendererVesselCurved::RenderSelectionHandles() const
{
  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon(
    dynamic_cast< vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererVesselCurved", "RenderSelectionHandles" );
    return;
  }

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );

  // render the starting point
  RenderSelectionHandle( pRibbon, pVessel->GetMeasurementDomain().first, "Norm" );

  // render the ending point
  RenderSelectionHandle( pRibbon, pVessel->GetMeasurementDomain().second, "Sten" );

} // RenderSelectionHandles()


/**
 * Render a single selection handle
 *
 * @param pRibbon         the ribbon data structure
 * @param fSelectionIndex the selection index to display
 * @param sDisplayName    the name to display next to the handle
 */
void vxRendererVesselCurved::RenderSelectionHandle( vxRibbon * pRibbon, const float4 fSelectionIndex,
                                                    const std::string & sDisplayName ) const
{
  /// find the projection of the two selection points onto the screen

  // check the input
  if ( fSelectionIndex < 0 ) return;

  // get the location of the handle on the screen
  // -1 represents off-screen
  const float4 fScanline( vxVesselUtilities::GetScanlinePosition( fSelectionIndex, pRibbon ) );
  const int4 iScanline( fScanline + 0.5F );

  // see if we have found a valid scanline
  if ( iScanline > -1 )
  {
    
    // compute the direction of the centerline at this position
    // use a central difference if possible
    Point3Df beforePoint( GetCenterlineXLocationFromImageY( pRibbon, iScanline - 1 ), GetScreenYFromImageY( pRibbon, iScanline - 1 ), 0 );
    Point3Df centerPoint( GetCenterlineXLocationFromImageY( pRibbon, iScanline     ), GetScreenYFromImageY( pRibbon, iScanline     ), 0 );
    Point3Df afterPoint ( GetCenterlineXLocationFromImageY( pRibbon, iScanline + 1 ), GetScreenYFromImageY( pRibbon, iScanline + 1 ), 0 );
    if ( beforePoint.m_x < 0.0F ) beforePoint = centerPoint;
    if ( afterPoint .m_x < 0.0F ) afterPoint  = centerPoint;

    if ( beforePoint != afterPoint )
    {
      // fix the start
      if ( centerPoint.m_x < 0 && afterPoint.m_x > 0 )
      {
        centerPoint = afterPoint;
        beforePoint = Point3Df( GetCenterlineXLocationFromImageY( pRibbon, iScanline + 1 ), GetScreenYFromImageY( pRibbon, iScanline + 1 ), 0 );
        afterPoint  = Point3Df( GetCenterlineXLocationFromImageY( pRibbon, iScanline + 2 ), GetScreenYFromImageY( pRibbon, iScanline + 2 ), 0 );
      }

      if ( centerPoint.m_x < 0 && beforePoint.m_x > 0 )
      {
        centerPoint = beforePoint;
        beforePoint = Point3Df( GetCenterlineXLocationFromImageY( pRibbon, iScanline - 2 ), GetScreenYFromImageY( pRibbon, iScanline - 2 ), 0 );
        afterPoint  = Point3Df( GetCenterlineXLocationFromImageY( pRibbon, iScanline - 1 ), GetScreenYFromImageY( pRibbon, iScanline - 1 ), 0 );
      }

      // get the perpendicular direction
      Normal3Df centerlineDirection( afterPoint - beforePoint );
      Normal3Df perpendicularDirection( -centerlineDirection.Y(), centerlineDirection.X(), 0.0F );

      // compute a segment perpendicular to the centerline
      const float4 fLength( 100.0F );
      Point3Df startPoint( centerPoint - Vector3Df( perpendicularDirection ) * fLength / 2.0F );
      Point3Df   endPoint( centerPoint + Vector3Df( perpendicularDirection ) * fLength / 2.0F );

      vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
      if ( pPreferences->GetDisplayCenterline() )
      {
        // draw a segment perpendicular to the centerline
        glLineWidth( 1.5F );
        glColor3f( 0, 1, 0 );
        glBegin( GL_LINES );
        vxRendererUtils::GlVertex( startPoint );
        vxRendererUtils::GlVertex(   endPoint );
        glEnd();

        /// draw a string to the right of the segment
        if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) )
        {

          // pick the rightmost point
          Point3Df stringPoint( ( startPoint.m_x > endPoint.m_x ) ? startPoint : endPoint );

          // offset a bit
          stringPoint += Vector3Df( 5, 0, 0 );

          // draw the string
          vxOverlayStrings * pOverlayStrings( dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) ) );
          vxOverlayStrings::vxOverlayElement element( sDisplayName, vxOverlayStrings::vxOverlayElement::Random );
          element.SetRandomScreenPosition( Point2D< float4 >( stringPoint.m_x, stringPoint.m_y ) );
          pOverlayStrings->AddElement( element );

        } // if showing text onscreen

      } // if display centerline

      if ( pPreferences->GetDisplayGraphs() )
      {

        // draw a horizontal reference line to indicate where the current handle is on the graphs
        glLineWidth( 1.0F );
        glColor4f( 1, 1, 1, 0.5F );
        glBegin( GL_LINES );
        glVertex2f( centerPoint.m_x-GetEnvironment().GetWinRect().GetViewportWidth(), centerPoint.m_y );
        glVertex2f( centerPoint.m_x+GetEnvironment().GetWinRect().GetViewportWidth(), centerPoint.m_y );
        glEnd();

      } // if display graphs

    } // valid handle

  } // scanline visible on screen

} // RenderSelectionHandles()


/**
 * Render the centerline
 */
void vxRendererVesselCurved::RenderCenterline() const
{

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon(
    dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererVesselCurved", "RenderCenterline" );
    return;
  }

  glBegin( GL_LINE_STRIP );
  glColor4f( 0.0F, 1.0F, 0.0F, 1.0F );

  // loop over all scanlines
  for( int4 iScreenY( 0 ); iScreenY < GetEnvironment().GetWinRect().GetViewportHeight(); ++ iScreenY )
  {

    const float4 fScreenX( GetCenterlineXLocationFromScreenY( pRibbon, iScreenY ) );

    if ( ( fScreenX >= 0.0F ) && ( fScreenX <= ( GetEnvironment().GetWinRect().GetViewportWidth() - 1.0F ) ) )
    {
      vxRendererUtils::GlVertex( Point3Df( fScreenX, iScreenY, 0 ) ) ;
    }

  } // loop over all scanlines

  glEnd();

} // RenderCenterline()


/**
 * Render the contours
 */
void vxRendererVesselCurved::RenderContours() const
{

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  std::pair< uint4, uint4 > diameterIndices( pVessel->GetDiameterRangeIndices( pVessel->GetMeasurementDomain() ) );

  // loop over centerline points
  for( uint4 i(0); i<pVessel->size(); ++i )
  {

    // select a color
    Quad<float4> baseColor;
    if ( i >= pVessel->GetMeasurementDomain().GetMinimum() && i <= pVessel->GetMeasurementDomain().GetMaximum() )
    {
      baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.75F );
    }
    else
    {
      baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.25F );
    }

    // draw the connection to the next section
    if ( false )
    {
      if ( i >= pVessel->GetMeasurementDomain().GetMinimum() && i+1 <= pVessel->GetMeasurementDomain().GetMaximum() )
      {
        baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.75F );
      }
      else
      {
        baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.25F );
      }
    }
  }
} // RenderCenterline()


/**
 * Get the screen coordinate corresponding to an image position
 *
 * @param pRibbon the ribbon data structure
 * @param imageY  the image Y coordinate
 *
 * @return the screen Y coordinate
 */
float4 vxRendererVesselCurved::GetScreenYFromImageY( vxRibbon * pRibbon, const float4 imageY ) const
{
  return Interpolate( 0.0F, imageY, float4( pRibbon->GetScanlineData().size() - 1 ), 0.0F,
                      float4( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ) );
} // GetScreenFromImage()


/**
 * Get the image coordinate corresponding to a screen position
 *
 * @param pRibbon the ribbon data structure
 * @param screenY the screen Y coordinate
 *
 * @return the image Y coordinate
 */
float4 vxRendererVesselCurved::GetImageYFromScreenY( vxRibbon * pRibbon, const float4 screenY ) const
{
  return Interpolate( 0.0F, screenY, float4( GetEnvironment().GetWinRect().GetViewportHeight() - 1 ), 0.0F,
                      float4( pRibbon->GetScanlineData().size() - 1 ) );
} // GetImageFromScreen()


// undefines
#undef FILE_REVISION


// $Log: vxRendererVesselCurved.C,v $
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2005/06/08 15:02:17  vxconfig
// formatting
//
// Revision 1.4  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.3  2004/12/22 12:48:57  frank
// formatting
//
// Revision 1.2  2004/10/23 10:25:53  frank
// made some functions not static
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.12.2.1  2004/02/25 15:00:25  frank
// formatting
//
// Revision 1.12  2004/02/02 15:07:52  frank
// smoothed the centerline drawing
//
// Revision 1.11  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.10  2004/01/05 20:23:29  frank
// formatting
//
// Revision 1.9  2003/12/22 20:24:33  frank
// Fixed problem in which the vessel centerline could "bounce" off the left edge of the screen when you zoom in too much.
//
// Revision 1.8  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.7  2003/08/13 16:37:06  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.6  2003/05/29 14:05:41  frank
// Fixed debug messages
//
// Revision 1.5  2003/05/27 13:24:22  frank
// Throttled the dislay of image information
//
// Revision 1.4  2003/05/19 17:24:58  frank
// Removed unused return parameter
//
// Revision 1.3  2003/05/15 16:57:23  frank
// Fixed display of elements according to preferences
//
// Revision 1.2  2003/05/08 21:06:55  geconomos
// Fixed Michael's stuff.
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.17  2003/05/02 18:11:59  frank
// code review
//
// Revision 1.16  2003/04/11 18:40:01  frank
// Removed dead code
//
// Revision 1.15  2003/04/09 13:58:40  frank
// Cleaned up includes
//
// Revision 1.14  2003/03/28 14:44:32  frank
// added horizontal correspondence line
//
// Revision 1.13  2003/03/24 19:34:43  frank
// Refactored code
//
// Revision 1.12  2003/03/21 17:10:05  frank
// Flipped the scanlines in the curved view so that up is always up
//
// Revision 1.11  2003/03/13 14:11:59  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.10  2003/03/03 18:32:46  frank
// formatting
//
// Revision 1.9  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.8  2003/02/28 19:25:55  frank
// Fixed handle selection past the ends of the centerline
//
// Revision 1.7  2003/02/26 00:03:55  frank
// N -> Norm   S -> Sten
//
// Revision 1.6  2003/02/18 17:58:06  frank
// removed dead code
//
// Revision 1.5  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.4  2003/02/11 21:53:40  frank
// Fixed normal/stenotic section labels
//
// Revision 1.3  2003/02/11 21:47:37  frank
// Rendered selection handles
//
// Revision 1.2  2003/02/11 18:15:45  frank
// Cleaned up centerline computation.
//
// Revision 1.1.2.1  2003/02/12 23:10:19  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.1  2003/01/28 15:06:00  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererVesselCurved.C,v 1.1 2006/08/17 14:01:12 gdavidson Exp $
// $Id: vxRendererVesselCurved.C,v 1.1 2006/08/17 14:01:12 gdavidson Exp $
