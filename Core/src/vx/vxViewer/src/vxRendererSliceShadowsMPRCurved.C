// $Id: vxRendererSliceShadowsMPRCurved.C,v 1.7 2007/03/02 21:19:53 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererSliceShadowsMPRCurved.h"
#include "vxEnvironment.h"
#include "vxEnvironmentList.h"
#include "vxViewerTypeVesselDetailEndoluminal.h"
#include "vxViewerTypeVesselDetailMPROrtho.h"
#include "vxViewerTypeVesselDetailMPROblique.h"
#include "vxRibbon.h"
#include "vxRendererUtils.h"
#include "vxVesselUtilities.h"
#include "Line.h"
#include "PrimitiveUtilities.h"

// Defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererSliceShadowsMPRCurved::vxRendererSliceShadowsMPRCurved(vxEnvironment & environment) : vxRendererSliceShadows( environment )
{
} // vxRendererSliceShadowsMPRCurved()


/**
 * Renders the slice shadows
 */
void vxRendererSliceShadowsMPRCurved::Render()
{ 
  // early return if cliece shadows aren't enabled
  if ( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_NONE )
    return;

  // make sure we have a ribbon
  if ( dynamic_cast< vxRibbon * >( GetEnvironment().GetElement( vxIDs::Ribbon ) ) == NULL )
    return;

  // save the OpenGL state
  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glDisable( GL_DEPTH_TEST );
  glColor3f( 1.0F, 1.0F, 1.0F );

 
  if( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_ACTIVE )
  {
    const vxEnvironment *  pActiveEnvironment = vxEnvironment::GetActiveEnvironment();
    if( pActiveEnvironment && pActiveEnvironment != &GetEnvironment() )
    {
       vxViewerType * pViewerType = pActiveEnvironment->GetViewerType();

      if( dynamic_cast< vxViewerTypeVesselDetailEndoluminal* >( pViewerType ) )
      {
        RenderVesselDetailEndoluminal( *pActiveEnvironment );
      }
      else if( dynamic_cast< vxViewerTypeVesselDetailMPROrtho* >( pViewerType ) || dynamic_cast< vxViewerTypeVesselDetailMPROblique* >( pViewerType ) )
      {
        RenderVesselDetailPlane( *pActiveEnvironment );
      }
    }
  }
  else
  {
    vxEnvironmentList * pEnvironmentList( reinterpret_cast< vxEnvironmentList * >( GetEnvironment().GetElement( vxIDs::EnvironmentList )));
    for( vxEnvironmentList::EnvironmentList::iterator iter = pEnvironmentList->GetEnvironmentList().begin(); iter != pEnvironmentList->GetEnvironmentList().end(); ++iter )
    {
      vxEnvironment * pOtherEnvironment( * iter );
      
      vxViewerType * pOtherViewerType = pOtherEnvironment->GetViewerType();
      
      if( dynamic_cast< vxViewerTypeVesselDetailEndoluminal* >( pOtherViewerType ) )
      {
        RenderVesselDetailEndoluminal( *pOtherEnvironment );
      }
      else if( dynamic_cast< vxViewerTypeVesselDetailMPROrtho* >( pOtherViewerType ) || dynamic_cast< vxViewerTypeVesselDetailMPROblique* >( pOtherViewerType ) )
      {
        RenderVesselDetailPlane( *pOtherEnvironment );
      }
    }
  }

  glPopAttrib();


} // Render()


/**
 * Renders the endoluminal camera position and direction 
 *
 * @param   otherEnvironment    other environment
 */
void vxRendererSliceShadowsMPRCurved::RenderVesselDetailEndoluminal( const vxEnvironment & otherEnvironment )
{
  // don't endoluminal camera if we are the active environment
  if( &GetEnvironment() == GetEnvironment().GetActiveEnvironment() )
    return;

  vxShareableObject< vxCamera > * pCamera = otherEnvironment.GetCamera();

  const float4 fArrowLength( 10.0F );
  const float4 fArrowLengthOnScreen( 30.0F );
  Point<float4> arrowBgn( pCamera->GetViewReference() );
  Point<float4> arrowEnd( pCamera->GetViewReference() - fArrowLength*pCamera->GetViewDirection() );

  const vxViewerType * pViewerType = GetEnvironment().GetViewerType();
  if( pViewerType->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace )
  {
    Point2D<int2> screenPos;

    pViewerType->ConvertWorldToScreen(arrowBgn, screenPos);
    arrowBgn.m_x = screenPos.m_x;
    arrowBgn.m_y = screenPos.m_y;
    arrowBgn.m_z = 0;

    pViewerType->ConvertWorldToScreen(arrowEnd, screenPos);
    arrowEnd.m_x = screenPos.m_x;
    arrowEnd.m_y = screenPos.m_y;
    arrowEnd.m_z = 0;
  }
  Vector3Df arrowDir( arrowBgn - arrowEnd );
  if ( arrowDir.GetLength() < 1.0F )
    arrowDir = Vector3Df( 0.0F, 0.0F, 1.0F );
  arrowDir.Normalize();
  Point3Df arrowHead( arrowBgn - 0.5 * fArrowLengthOnScreen * arrowDir );

  vxRendererUtils::RenderArrow( arrowHead, Normal3Df( arrowDir ), fArrowLengthOnScreen, true, PixelRGBA<float4>( 0.0F, 0.0F, 1.0F, 1.0F ) );

} // RenderVesselDetailEndoluminal( const vxEnvironment & otherEnvironment )


/**
 * Renders the projection of an oblique or ortho plane.
 *
 * @param   pOtherEnvironment       other environment
 */
void vxRendererSliceShadowsMPRCurved::RenderVesselDetailPlane( const vxEnvironment & otherEnvironment )
{

  const vxPlaneOblique<float4> * pPlane = NULL;
  if( dynamic_cast< vxViewerTypeVesselDetailMPROrtho* >( otherEnvironment.GetViewerType() ) )
  {
    pPlane = otherEnvironment.GetPlaneOrtho();
  }
  else if( dynamic_cast< vxViewerTypeVesselDetailMPROblique * >( otherEnvironment.GetViewerType() ) )
  {
    pPlane = otherEnvironment.GetPlaneOblique();
  }

  // make sure we have a ribbon
  vxRibbon * pRibbon = dynamic_cast< vxRibbon * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL || pRibbon->GetScanlineData().size() == 0 )
    return;

  /// loop over scanlines
  // keep track of whether we are inside the OpenGL begin()-end() context or not
  // because we only want to connect lines if they should are connected
  bool bInside( false );
  glColor3f( 0.0F, 1.0F, 1.0F );
  for ( int y=0; y<GetEnvironment().GetWinRect().GetViewportHeight(); ++y )
  {
    // determine the scanline in the ribbon
    float4 fScanline( 0.0F );
    if ( vxVesselUtilities::GetScanline( fScanline, y, GetEnvironment().GetWinRect(), pRibbon->GetScanlineData().size() ) )
    {

      // determine the x position on the scanline
      const vxRibbon::ScanlineData & scanline = pRibbon->GetScanlineData()[ fScanline ];
      if ( scanline.m_fSectionIndex >= 0.0F )
      {
        const Vector3Df scanlineVector( scanline.m_rightPoint - scanline.m_leftPoint );
        const Line scanlineLine( scanline.m_leftPoint, scanline.m_rightPoint );
        Point3Df isectPoint;
        if ( pPlane->GetIntersection( scanline.m_leftPoint, Normal3Df( scanlineVector ), isectPoint ) )
        {
          const RaySegment segment( scanline.m_leftPoint, scanline.m_rightPoint );
          const float4 fPercentage( PrimitiveUtilities::GetPercentAlongSegment( segment, isectPoint ) );
          float4 fX( fPercentage * GetEnvironment().GetWinRect().GetViewportWidth() );

          if ( fX >= 0 && fX <= GetEnvironment().GetWinRect().GetViewportWidth() - 1.0F )
          {
            if ( bInside == false )
            {
              glBegin( GL_LINE_STRIP );
              bInside = true;
            }
            vxRendererUtils::GlVertex2f( Point2D<float4>( fX, y ) );
          }
          else if ( bInside )
          {
            glEnd();
            bInside = false;
          }
        } // if there is an intersection
      } // if a valid section index
    } // if a valid scanline
  } // for all scanlines

  if ( bInside )
  {
    glEnd();
    bInside = false;
  }
} // RenderPlane( const vxEnvironment & otherEnvironment )



// $Log: vxRendererSliceShadowsMPRCurved.C,v $
// Revision 1.7  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.6  2006/11/14 16:28:37  geconomos
// updated to check the the current viewer isn't the active viewer
//
// Revision 1.5  2006/11/13 14:39:08  geconomos
// made sure that slice shadows always update
//
// Revision 1.4  2006/10/26 13:05:09  geconomos
// fixed crash
//
// Revision 1.3  2006/10/05 17:03:45  geconomos
// still reworking
//
// Revision 1.2  2006/10/04 14:59:07  geconomos
// not drawing endoluminal camera if curved mpr is the active viewer
//
// Revision 1.1  2006/10/03 20:04:16  geconomos
// continued reworking of slice shadows
//
// Revision 1.3  2006/09/25 20:08:51  geconomos
// reworking (stay tuned)
//
// Revision 1.2  2006/09/25 13:24:06  geconomos
// using ViewerType instead of DetailViewMode to determine detail viewer type
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.3  2005/06/08 15:01:52  vxconfig
// formatting
//
// Revision 1.2  2005/05/18 19:28:41  vxconfig
// slice shadows were moved into their own directory
//
// Revision 1.1  2001/01/03 20:29:49  michael
// Renamed vxRenderer{*}SliceShadows to vxRenderersliceShadows{*} so that
// it is consistent with the overall hierarchical class naming convention.
//
// Revision 1.3  2001/01/03 19:30:23  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.2  2004/10/23 10:25:01  frank
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
// Revision 1.19  2004/03/02 01:10:31  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.18.2.1  2004/02/25 14:59:36  frank
// formatting
//
// Revision 1.18  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.17  2004/01/12 13:00:14  frank
// formatting
//
// Revision 1.16  2003/12/22 20:00:57  frank
// updated slice shadow data
//
// Revision 1.15  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.14  2003/12/10 14:05:13  frank
// Fixed display of oblique shadow on curved MPR view
//
// Revision 1.13  2003/12/09 16:08:37  frank
// code cleanup
//
// Revision 1.12  2003/08/20 18:37:00  frank
// Trying to remove intermittent crash
//
// Revision 1.11  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.10  2003/07/09 19:58:16  wenli
// Issue #2792: showed camera arrow only in Endoluminal mode
//
// Revision 1.9  2003/07/09 15:22:51  frank
// Issue #2792: showed shadows only when they should show up
//
// Revision 1.8  2003/06/12 20:09:56  wenli
// render the camera and oblique plane to solve the Track#2792
//
// Revision 1.7  2003/05/23 15:46:22  frank
// moved to new oblique renderer structure
//
// Revision 1.6  2003/05/22 15:06:48  frank
// Changed inheritance
//
// Revision 1.5  2003/05/20 16:37:27  frank
// Showed the new oblique slice shadow
//
// Revision 1.4  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.3  2003/05/12 20:07:17  frank
// Moved to alternate oblique viewer
//
// Revision 1.2  2003/05/08 21:06:55  geconomos
// Fixed Michael's stuff.
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.9  2003/05/01 20:00:29  frank
// code review
//
// Revision 1.8  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.7  2003/04/09 13:56:38  frank
// Cleaned up includes
//
// Revision 1.6  2003/03/26 19:44:08  frank
// Fixed up viewer type
//
// Revision 1.5  2003/03/21 21:17:12  frank
// Added extra safety check
//
// Revision 1.4  2003/03/11 18:43:44  geconomos
// Enabled anti-aliased line drawing.
//
// Revision 1.3  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.2  2003/02/27 22:33:43  frank
// Cleaned up shadow lines when the line curves on and off the screen
//
// Revision 1.1  2003/01/28 23:20:57  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSliceShadowsMPRCurved.C,v 1.7 2007/03/02 21:19:53 geconomos Exp $
// $Id: vxRendererSliceShadowsMPRCurved.C,v 1.7 2007/03/02 21:19:53 geconomos Exp $
