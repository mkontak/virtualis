// $Id: vxRendererOverlayIncrementalGrowth.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererOverlayIncrementalGrowth.h"
#include "vxUtils.h"
#include "vxWindowLevel.h"
#include "vxBlockVolume.h"
#include "vxEnvironment.h"
#include "GLFontGeorge.h"
#include "vxBlockVolumeIterator.h"
#include "ValueRange.h"
#include "vxIncrementalGrowth.h"
#include "vxManipulatorIncrementalGrowth.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlayIncrementalGrowth::vxRendererOverlayIncrementalGrowth( vxEnvironment & environment )
: vxRendererOverlay( environment ),
m_uNumVoxelsProcessed( 0 )
{
  m_growthImage.Initialize( GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight() );
} // Constructor


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayIncrementalGrowth::IsRedrawNecessary()
{

  bool bRedrawNecessary( false );

  // has the slice plane changed?
  bRedrawNecessary |= GetEnvironment().IsModified( vxEnvironment::PLANE_MPR );

  // has the incremental growth object changed?
  bRedrawNecessary |= GetEnvironment().IsModified( vxIDs::IncrementalGrowth );

  return bRedrawNecessary;
    
} // IsRedrawNecessary()


/**
 * Render vessel overlay.
 */  
void vxRendererOverlayIncrementalGrowth::Render()
{

  // perform all processing
  ProcessVoxels();

  // set up OpenGL environment
  SetupRenderState();
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // set up antialiasing
  glEnable(GL_BLEND);
  glEnable( GL_LINE_SMOOTH );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glLineWidth( 1.0F );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

  // render elements
  RenderIncrementalGrowthImage();
  RenderSlider();

  // restore OpenGL environment
  glPopAttrib();
  RestoreRenderState();

} // Render()


/**
 * Process all the voxels computed so far and splat to image
 */
void vxRendererOverlayIncrementalGrowth::ProcessVoxels()
{
  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    return;
  }

  // see if the image size has changed (the user may have made the viewer full screen or toggled back)
  if ( GetEnvironment().GetWinRect().GetViewportWidth () != m_growthImage.GetImage().GetDim().m_x ||
       GetEnvironment().GetWinRect().GetViewportHeight() != m_growthImage.GetImage().GetDim().m_y )
  {
    m_growthImage.Initialize( GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight() );
    m_growthImage.Clear();
    m_uNumVoxelsProcessed = 0;
  }

  // recompute if the slice plane has changed
  if ( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) )
  {
    m_growthImage.Clear();
    m_uNumVoxelsProcessed = 0;
  }

  // recompute if the user has gone backwards and selected less than the prior frame
  if ( m_uNumVoxelsProcessed > pIncrementalGrowth->GetComponentVoxels().size() )
  {
    m_growthImage.Clear();
    m_uNumVoxelsProcessed = 0;
  }

  // project more voxels if the user has asked for more
  while ( m_uNumVoxelsProcessed < pIncrementalGrowth->GetComponentVoxels().size() )
  {
    m_growthImage.ProjectToImage( pIncrementalGrowth->GetComponentVoxels()[ m_uNumVoxelsProcessed ],
                                  m_uNumVoxelsProcessed,
                                  pIncrementalGrowth->GetVoxelsProcessed()[ m_uNumVoxelsProcessed ],
                                  GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits(),
                                  * GetEnvironment().GetViewerType(), GetEnvironment() );
    ++m_uNumVoxelsProcessed;
  }

} // ProcessVoxels()


/**
 * Render the incremental growth image
 */
void vxRendererOverlayIncrementalGrowth::RenderIncrementalGrowthImage()
{

  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    return;
  }

  // render points
  std::vector< Point2D< int2 > > & points = m_growthImage.GetPoints();
  if ( points.size() == 0 )
    return;

  // begin drawing yellow points
  glColor3f( 1, 1, 0 );
  glBegin( GL_POINTS );
  {
    // loop over all points
    uint4 uDisplayableVoxels( m_growthImage.GetNumberPointsToDisplay( pIncrementalGrowth->GetIncrementalSlider() ) );
    for ( uint4 i(0); i<uDisplayableVoxels; ++i )
    {
      glVertex2i( points[i].m_x, points[i].m_y );
    } // loop over all points
     
  } // glBegin
  glEnd();

} // RenderIncrementalGrowthImage()


/**
 * Render the slider
 */
void vxRendererOverlayIncrementalGrowth::RenderSlider()
{

  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    return;
  }

  // check if it's active
  const vxIncrementalSlider & slider = pIncrementalGrowth->GetIncrementalSlider();
  if ( slider.GetActive() == false )
    return;

  // see if the window is active
  if ( GetEnvironment().GetActiveEnvironment() != & GetEnvironment() )
    return;

  // precompute the endpoints
  const float4 fWindowHeight( GetEnvironment().GetWinRect().GetViewportHeight() - 1 );
  const float4 fHeight( slider.GetNumberSteps() );
  const float4 left( 50.0F );
  const float4 fSlope( 0.1F * pIncrementalGrowth->GetIncrementalSlider().GetVoxelsPerStep() / vxManipulatorIncrementalGrowth::GetVoxelsPerPixel() );
  const float4 right( left + slider.GetNumberStepsAbs() * fSlope );
  const Point2D<int2> anchor( left, slider.GetAnchorPoint().m_y );
  const Point2D<int2> top( left, anchor.m_y + fHeight );
  const Point2D<int2> corner( right, top.m_y );

  // render filled background
  glColor4f( 1, 1, 1, 0.5F );
  glBegin( GL_TRIANGLES );
  {
    glVertex2f( anchor.m_x, fWindowHeight - anchor.m_y );
    glVertex2f( top   .m_x, fWindowHeight - top   .m_y );
    glVertex2f( corner.m_x, fWindowHeight - corner.m_y );
  }
  glEnd();

  // render outline
  glColor4f( 1, 1, 1, 1 );
  glBegin( GL_LINE_LOOP );
  {
    glVertex2f( anchor.m_x, fWindowHeight - anchor.m_y );
    glVertex2f( top   .m_x, fWindowHeight - top   .m_y );
    glVertex2f( corner.m_x, fWindowHeight - corner.m_y );
  }
  glEnd();

} // RenderIncrementalGrowthImage()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayIncrementalGrowth.C,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.7  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.1  2005/06/16 18:43:08  frank
// removed unnecessary warnings
//
// Revision 1.6  2005/06/08 15:00:49  vxconfig
// formatting
//
// Revision 1.5  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.4  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.3  2001/01/03 19:30:23  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.2  2004/10/23 10:25:19  frank
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
// Revision 1.11  2004/03/02 01:10:31  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.10.2.1  2004/02/25 14:59:52  frank
// formatting
//
// Revision 1.10  2004/02/06 19:43:18  frank
// Issue #3726: Fixed Easy Select on first and last slices
//
// Revision 1.9  2004/02/02 20:02:04  frank
// fixed a problem with toggling full screen during growth
//
// Revision 1.8  2004/01/29 18:03:03  frank
// Made the slider draw differently depending on the accelerator keys currently held down
//
// Revision 1.7  2004/01/20 21:35:54  wenli
// remove the Y-axis flip in RenderIncrementalGrowthImage()
//
// Revision 1.6  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.5  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.4  2003/10/27 20:01:01  frank
// fixed debug message
//
// Revision 1.3  2003/10/20 19:28:02  frank
// fixed warning message
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.8  2003/05/06 16:21:22  frank
// Removed HUConverter
//
// Revision 1.7  2003/05/01 20:08:43  frank
// code review
//
// Revision 1.6  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.5  2003/04/07 14:26:55  frank
// Modified the visualization
//
// Revision 1.4  2003/04/07 14:08:51  frank
// Early return if no data
//
// Revision 1.3  2003/04/07 13:18:33  frank
// Made it only segment the number of voxels desired according to the slider.
//
// Revision 1.2  2003/04/04 21:27:45  frank
// Made the number of voxels in the region interactive
//
// Revision 1.1  2003/04/04 13:01:29  frank
// Added the ability to incrementally grow a new component.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayIncrementalGrowth.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayIncrementalGrowth.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
