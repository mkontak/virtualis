// $Id: vxRendererVesselDetailMPROrtho.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererVesselDetailMPROrtho.h"
#include "vxEnvironment.h"
#include "vxVesselSection.h"
#include "vxElementFloat4.h"
#include "vxRendererSoftwareMPROblique.h"
#include "Point.h"
#include "vxRendererOverlay.h"
#include "vxVesselUtilities.h"
#include "vxVesselPreferences.h"
#include "vxVessel.h"

// Defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererVesselDetailMPROrtho::vxRendererVesselDetailMPROrtho( vxEnvironment & environment ) :
vxRendererOverlay( environment )
{
} // vxRendererVesselDetailMPROrtho()


/**
 * Destructor
 */
vxRendererVesselDetailMPROrtho::~vxRendererVesselDetailMPROrtho()
{
} // ~vxRendererVesselDetailMPROrtho()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererVesselDetailMPROrtho::IsRedrawNecessary()
{

  const bool bPreferencesChanged( GetEnvironment().IsModified( vxIDs::VesselPreferences ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );

  return bPreferencesChanged || bVesselChanged;

} // IsRedrawNecessary()


/**
 * render the vessel using OpenGL
 */
void vxRendererVesselDetailMPROrtho::Render()
{ 
  // check if there is a vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL )
    return;

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // setup the state
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // get the preferences
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
    throw ex::NullPointerException( LogRec( "Preferences pointer is NULL", "vxRendererVesselDetailMPROrtho", "Render" ) );

  if( pPreferences->GetDisplayCenterline() )
    RenderCenterline();

  glPopAttrib();
  
} // Render()


/**
 * Transform OpenGL for the detail volume
 *
 * @return the translation vector of the detail volume
 */
Vector3Df vxRendererVesselDetailMPROrtho::GetDetailTransform() const
{

  Vector3Df transform;
  Point<float4> centerPoint;
  if( GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar( "VolumeCenter", centerPoint ) )
  {
    // get the location of (0,0,0) in the original volume
    Vector<float4> worldDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
    Point<float4> volMinPoint( centerPoint - worldDim / 2.0F );

    transform = Vector3Df( -volMinPoint.m_x, -volMinPoint.m_y, -volMinPoint.m_z );
  }

  return transform;

} // GetDetailTransform()


/**
 * Render the centerline
 */
void vxRendererVesselDetailMPROrtho::RenderCenterline()
{
  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );

  // ensure there is some vessel data
  if( pVessel->size() == 0 )
    return;

  // get the vessel section
  vxElementFloat4 * pCurrentVesselSection = dynamic_cast< vxElementFloat4 * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
  if( pCurrentVesselSection == NULL )
  {
    throw ex::NullPointerException( LogRec( "Current vessel section pointer is NULL", "vxRendererVesselDetailMPROrtho", "Render" ) );
    return;
  }

  // get the 3D position
  Point< float4 > pos( pVessel->GetResampledPosition( pCurrentVesselSection->GetValue() ) );
  pos -= GetDetailTransform();

  // convert to screen coordinates
  Point2D<int2> screenPos;
  if (GetEnvironment().GetViewerType()->ConvertWorldToScreen( pos, screenPos ) )
  {

    // draw the crosshair

    SetupRenderState();

    // make it translucent green
    glColor4f( 0.0F, 1.0F, 0.0F, 0.75F );
    
    // invert the y coordinate
    glBegin( GL_LINES );
    {
      const float fSize( 5 );
      glVertex2f( screenPos.m_x - fSize, screenPos.m_y );
      glVertex2f( screenPos.m_x + fSize, screenPos.m_y );

      glVertex2f( screenPos.m_x, screenPos.m_y - fSize );
      glVertex2f( screenPos.m_x, screenPos.m_y + fSize );
    } // glBegin
    glEnd();

    vxRendererOverlay::RestoreRenderState();

  } // if the point can be transformed to the creen

} // RenderCenterline()


// undefines
#undef FILE_REVISION


// $Log: vxRendererVesselDetailMPROrtho.C,v $
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/06/08 15:02:26  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/10/23 10:26:23  frank
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
// Revision 1.8  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/25 15:00:32  frank
// formatting
//
// Revision 1.7  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.6  2003/12/22 19:59:36  frank
// removed dead code
//
// Revision 1.5  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.4  2003/09/16 13:59:59  dongqing
// Add glPopAttrib() to the Render(). This fixes the bug that resulted 3D detail
// view blackout after switching back from a 2D view.
//
// Revision 1.3  2003/05/27 13:24:37  frank
// Removed dead includes
//
// Revision 1.2  2003/05/19 17:24:58  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/05/05 13:58:19  frank
// code review
//
// Revision 1.3  2003/05/01 14:40:12  frank
// fixed centerline crosshair rendering
//
// Revision 1.2  2003/04/18 15:30:50  frank
// Working on vessel centerline
//
// Revision 1.1  2003/04/04 13:50:07  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererVesselDetailMPROrtho.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
// $Id: vxRendererVesselDetailMPROrtho.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
