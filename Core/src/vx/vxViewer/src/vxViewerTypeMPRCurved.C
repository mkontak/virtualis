// $Id: vxViewerTypeMPRCurved.C,v 1.10.2.1 2009/06/15 19:58:08 gdavidson Exp $
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
#include "vxViewerTypeMPRCurved.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"
#include "vxUtils.h"
#include "vxAnnotationPoint.h"
#include "vxAnnotationArrow.h"
#include "vxAnnotationDistance.h"
#include "vxAnnotationAngle.h"
#include "vxRibbon.h"
#include "vxIDs.h"
#include "Line.h"
#include "PrimitiveUtilities.h"
#include "vxManipulatorVesselCurvedProjected.h"
#include "vxManipulatorVesselCurved.h"
#include "vxRendererSoftwareMPRCurved.h"
#include "vxVesselUtilities.h"
#include "RaySegment.h"
#include "vxRenderingNodePluginCurvetracer.h"


// Defines
#define FILE_REVISION "$Revision: 1.10.2.1 $"


/**
 * Constructor
 */
vxViewerTypeMPRCurved::vxViewerTypeMPRCurved(vxEnvironment & environment)
:vxViewerType(environment)
{
  SetRenderCoordinateSystem( vxViewerType::eScreenSpace );
} // vxViewerTypeMPRCurved()


/**
 * Destructor
 */
vxViewerTypeMPRCurved::~vxViewerTypeMPRCurved()
{
} // ~vxViewerTypeMPRCurved()


/**
 * Initialize viewer.
 * This method is responsible for initializing the OpenGL viewing
 */
void vxViewerTypeMPRCurved::SetupView()
{
  glViewport(GetEnvironment().GetWinRect().GetReferencePoint().first, GetEnvironment().GetWinRect().GetReferencePoint().second, 
             GetEnvironment().GetWinRect().GetViewportSize  ().first, GetEnvironment().GetWinRect().GetViewportSize  ().second);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0.0F, GetEnvironment().GetWinRect().GetViewportWidth(),
           0.0F, GetEnvironment().GetWinRect().GetViewportHeight(), 
           -20, 20 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glColor3f(1.0F, 1.0F, 1.0F);
} // SetupView()


/**
 * initializes the view
 */
void vxViewerTypeMPRCurved::InitializeView()
{
  // This function appears to be deprecated, it incorrectly zooms the 
  // window to the length of the vessel (not the full image size which is what we desire)
  /*vxManipulatorVesselCurved::ZoomEntireVolume( GetEnvironment(),
    Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(),
                   GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );*/

  vxVesselUtilities::OrientCameraToVessel( GetEnvironment(), 'v' );

} // InitializeView()


/**
 * initializes the view/object in view based on an annotation
 *
 * @param annotation the annotation to initialize from
 *
 * @return if initialized
 */
bool vxViewerTypeMPRCurved::InitializeFrom( vxAnnotation & annotation )
{
  vxMouseCursor waitCursor( "CURSOR_WAIT" );
  bool bSuccess( false );

  // get the curved plane
  vxPlaneCurved * pPlaneCurved = dynamic_cast< vxPlaneCurved * >( GetEnvironment().GetPlaneCurved() );
  if ( pPlaneCurved == NULL )
  {
    LogDbg( "Curved Plane pointer is NULL", "vxViewerTypeMPRCurved", "InitializeFrom" );
    return bSuccess;
  }

  // keep track of the original section index to restore it if we can't find a more suitable one
  float4 fDesiredAngle(  pPlaneCurved->GetAngle() );

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxViewerTypeMPRCurved", "InitializeFrom" );
    return bSuccess;
  }

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxViewerTypeMPRCurved", "InitializeFrom" );
    return bSuccess;
  }

  // try to find a suitable angle
  const float4 fMaxAngle( 360.0F );
  const float4 fAngleIncrement( 1.0F );
  const Triple<uint4> imageSize( 
    vxRendererSoftwareMPRCurved::GetDesiredImageSize( GetEnvironment().GetWinRect().GetViewportWidth(),
                                                      GetEnvironment().GetWinRect().GetViewportHeight() ) );
  float4 fSmallestDistance( (std::numeric_limits<float>::max)() );
  float4 fBestAngle( 0.0F );
  for ( float4 fAngle( 0.0F ); fAngle <= fMaxAngle; fAngle += fAngleIncrement )
  {

    // set the angle and compute the ribbon scanlines
    pPlaneCurved->SetAngle( fAngle );
    pRibbon->ComputeScanlinesCurved( * pVessel, GetEnvironment(), imageSize );

    // make sure all the handles are visible
    if ( GetSumDistance( annotation ) < fSmallestDistance )
    {
      fSmallestDistance = GetSumDistance( annotation );
      bSuccess = true;
      fBestAngle = fAngle;
    }
  }

  // set it to the desired angle (either the original one or a visible one)
  pPlaneCurved->SetAngle( fBestAngle );
  GetEnvironment().GetPlaneCurved()->Modified( vxModInfo( vxPlaneCurved, vxUtils::PLANE_MODIFIED ) );
  pRibbon->ComputeScanlinesCurved( * pVessel, GetEnvironment(), imageSize );

  return bSuccess;

} // InitializeFrom()


/**
 * Convert the mouse coordinates into screen coordinates
 * @param mouseCoord  mouse coordinates
 * @return succesful
 */
Point2D<int2> vxViewerTypeMPRCurved::ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const
//*******************************************************************
{
  Point2D<int2> screenCoord(vxViewerType::ConvertMouseToScreen(mouseCoord));

  screenCoord.m_y = GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screenCoord.m_y;

  return screenCoord;
} // ConvertMouseToScreen()


/**
 * Get the world position of the given screen position
 * @param screenPos the screen position
 * @param worldPos  the world position
 * @param bConsiderRenderingMode the rendering mode indicator
 *
 * @return succesful
 */
bool vxViewerTypeMPRCurved::ConvertScreenToWorld(const Point2D<int2> & screenPos, 
                                                 Point<float4> & worldPos,
                                                 bool bConsiderRenderingMode ) const
{

  // bound input to the window area
  vxWinRect winRect( GetEnvironment().GetWinRect() );

  Point2D<int2> maxBound;
  maxBound.m_x = winRect.GetReferencePoint().first  + winRect.GetViewportSize().first  - 1;
  maxBound.m_y = winRect.GetReferencePoint().second + winRect.GetViewportSize().second - 1;
  Point2D<int2> position;
  position.m_x = Bound(winRect.GetReferencePoint().first,  screenPos.X(), maxBound.X());
  position.m_y = Bound(winRect.GetReferencePoint().second, (int2)(GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screenPos.Y()), maxBound.Y());

  // get the ribbon data structure
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >(
    GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
    return false;
  if ( pRibbon->GetScanlineData().size() == 0 )
    return false;

  // determine the scanline
  float4 fScanline( Interpolate( float4( winRect.GetViewportHeight() ), float4( position.m_y ), 0.0F, 0.0F,
                                 float4( pRibbon->GetScanlineData().size() ) ) );

  // interpolate
  if ( fScanline > pRibbon->GetScanlineData().size()-1 )
    return false;

  const Point3Df leftPoint ( pRibbon->GetScanlineData()[ fScanline ].m_leftPoint  );
  const Point3Df rightPoint( pRibbon->GetScanlineData()[ fScanline ].m_rightPoint );
  const float4 fPercentX( float4( screenPos.m_x ) / float4( winRect.GetViewportWidth()-1 ) );
  worldPos = Point3Df( Vector3Df( leftPoint ) * ( 1.0F - fPercentX ) + Vector3Df( rightPoint ) * fPercentX );

  return true;

} // ConvertScreenToWorld()


/**
 * Get the world position of the given screen position
 *
 * @param worldPos  the input world position
 * @param screenPos the output screen position
 *
 * @return succesful
 */
bool vxViewerTypeMPRCurved::ConvertWorldToScreen(const Point<float4> & worldPos, 
                                                 Point2D<int2> & screenPos ) const
{

  // get the ribbon data structure
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
  {
    LogDbg("ribbon is null", " ", " " );
    return false;
  }

  // compute the location
  float4 fNearestScanline( 0.0F );
  float4 fPercentAlongScanline( 0.0F );
  float4 fNearestDistance( 0.0F );
  ComputeNearestCurvedPoint( pRibbon, worldPos, fNearestScanline, fPercentAlongScanline, fNearestDistance );

  // convert to screen coordinates
  const float4 fImageY( Interpolate( 0.0F, fNearestScanline, float4( pRibbon->GetScanlineData().size() - 1 ), 
                                     0.0F, float4( GetEnvironment().GetWinRect().GetViewportHeight() - 1 )) );
  const float4 fImageX( fPercentAlongScanline * ( GetEnvironment().GetWinRect().GetViewportWidth() - 1 ) );

  // save result
  screenPos.m_x = fImageX;
  screenPos.m_y = fImageY; //GetEnvironment().GetWinRect().GetViewportHeight() - 1 - fImageY;

  return true;

} // ConvertWorldToScreen()


/**
 * Get the world position of the given screen position.
 * Call this version when you already have the OpenGL matrices and you need efficiency,
 * or if you do not want to read it from the current OpenGL settings, but from another.
 *
 * @param worldPosition      the input world position
 * @param screenPos          the output screen position
 * @param pfModelviewMatrix  the model view matrix
 * @param pfProjectionMatrix the projection matrix
 *
 * @return succesful
 */
bool vxViewerTypeMPRCurved::ConvertWorldToScreen(const Point<float4> & worldPosition, 
                                                 Point2D<int2> & screenPos,
                                                 const GLint * piViewport,
                                                 const GLdouble * pfModelviewMatrix,
                                                 const GLdouble * pfProjectionMatrix ) const
{
  return ConvertWorldToScreen(worldPosition, screenPos);
} // GetScreenCoordinateFromParams()


/**
 * is this annotation type supported by this viewer type?
 *
 * @param handlesObject an object with handles
 *
 * @return succesful
 */
bool vxViewerTypeMPRCurved::IsAnnotationSupported(const vxHandlesObj & handlesObject) const
{

  //if ( dynamic_cast< const vxAnnotationPoint * >( & handlesObject ) )
  //  return true;
  //else if ( dynamic_cast< const vxAnnotationArrow * >( & handlesObject ) )
  //  return true;
  if ( dynamic_cast< const vxAnnotationDistance * >( & handlesObject ) )
    return true;
  else if ( dynamic_cast< const vxAnnotationAngle * >( & handlesObject ) )
    return true;

  return false;

} // IsSupported()


/**
 * Is the handlesSet visible and should it be drawn?
 *
 * @param handlesObject an object with handles
 *
 * @return succesful
 */
bool vxViewerTypeMPRCurved::IsVisible(const vxHandlesObj & handlesObject) const
{

  // get the ribbon data structure
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
    return false;

  // loop over all handles
  for ( int4 i(0); i<handlesObject.GetNumHandles(); ++i )
  {
    // select nearest scanline
    const Point<float4> & handlePoint = handlesObject.GetHandle( i ).GetPosition();

    float4 fNearestScanline( 0.0F );
    float4 fPercentAlongScanline( 0.5F );
    float4 fNearestDistance( 0.0F );
    if ( ComputeNearestCurvedPoint( pRibbon, handlePoint, fNearestScanline, fPercentAlongScanline, fNearestDistance ) == false )
      return false;
  }

  return true;

} // IsVisible()


/**
 * Compute the sum of all distances of the handle with the given scanline
 *
 * @param handlesObject an object with handles
 *
 * @return sum of all distances
 */
float4 vxViewerTypeMPRCurved::GetSumDistance( const vxHandlesObj & handlesObject ) const
{

  // get the ribbon data structure
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
    return false;

  // loop over all handles
  float4 fSumDistance( 0.0F );
  for ( int4 i(0); i<handlesObject.GetNumHandles(); ++i )
  {
    // select nearest scanline
    const Point<float4> & handlePoint(handlesObject.GetHandle(i).GetPosition());

    float4 fNearestScanline( 0.0F );
    float4 fPercentAlongScanline( 0.5F );
    float4 fNearestDistance( 0.0F );
    ComputeNearestCurvedPoint( pRibbon, handlePoint, fNearestScanline, fPercentAlongScanline, fNearestDistance );
    fSumDistance = fNearestDistance;
  }

  return fSumDistance;

} // GetSumDistance()


/**
 * Is this position visible in the current view?
 *
 * @param position the query position
 *
 * @return true if visible
 */
bool vxViewerTypeMPRCurved::IsVisible( const Point3Df & position ) const
{

  // get the ribbon data structure
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
    return false;

  // select nearest scanline
  float4 fNearestScanline( 0.0F );
  float4 fPercentAlongScanline( 0.5F );
  float4 fNearestDistance( 0.0F );
  if ( ComputeNearestCurvedPoint( pRibbon, position, fNearestScanline, fPercentAlongScanline, fNearestDistance ) == false )
    return false;

  return true;

} // IsVisible()


/**
 * Compute the nearest point on the curved plane.
 *
 * @param pRibbon               the ribbon data structure
 * @param queryPosition         the input position of interest
 * @param fNearestScanline      the nearest scanline
 * @param fPercentAlongScanline the distance from left to right along the scanline [0,1]
 *
 * @return                      true if within this curved plane
 */
bool vxViewerTypeMPRCurved::ComputeNearestCurvedPoint( vxRibbon * pRibbon, const Point<float4> & queryPosition,
                                                       float4 & fNearestScanline, float4 & fPercentAlongScanline,
                                                       float4 & fNearestDistance )
{

  if ( pRibbon == NULL )
    return false;

  // how close the nearest scanline is
  fNearestDistance = (std::numeric_limits<float>::max)();

  // loop over all the scanlines
  for ( int4 j(0); j<pRibbon->GetScanlineData().size(); ++j )
  {

    const vxRibbon::ScanlineData & data = pRibbon->GetScanlineData()[ j ];
    if ( data.m_leftPoint != data.m_rightPoint )
    {
      Line scanline( data.m_leftPoint, data.m_rightPoint );
      const float4 fDistance( scanline.ComputeDistance( queryPosition ) );
      if ( fDistance < fNearestDistance )
      {
        fNearestDistance = fDistance;
        fNearestScanline = j;

        // compute the distance along the scanline
        RaySegment segment( data.m_leftPoint, data.m_rightPoint );
        fPercentAlongScanline = PrimitiveUtilities::GetPercentAlongSegment( segment, queryPosition );
      }
    }
  }

  // the threshold distance for deciding if we are close enough to the curved plane
  const float4 fDelta( 1.0F ); // in world units (mm)

  // check distance from nearest scanline
  return fNearestDistance < fDelta;

} // ComputeNearestCurvedPoint()


/**
 * Creates an instance of a rendering engine plugin for this particular viewer type
 */
vxRenderingNodePlugin * vxViewerTypeMPRCurved::CreateRenderingEnginePlugin()
{
  return new vxRenderingNodePluginCurvetracer( GetEnvironment() );
}//CreateRenderingEnginePlugin


/**
 * Returns if the viewer supports dropping of the specified type
 * @param eSourceType drop type
 * @return true if element can be fropped, false otherwise
 */
bool vxViewerTypeMPRCurved::CanDragDrop(const DragDropEnum eSourceType)
//*******************************************************************
{
	if (eSourceType == vxViewerType::COLORMAP || eSourceType == vxViewerType::DATASET || eSourceType == vxViewerType::PRESET )
  {
    return true;
  }
  return false;
} // CanDragDrop()



// $Log: vxViewerTypeMPRCurved.C,v $
// Revision 1.10.2.1  2009/06/15 19:58:08  gdavidson
// Issue 6306: Removed call to vxManipulatorVesselCurved::ZoomEntireVolume from InitializeView
//
// Revision 1.10  2007/09/13 19:16:14  romy
// Since vessel view is the one which is high in quality we decided to sets it as default
//
// Revision 1.9  2007/08/09 19:24:40  romy
// added drag and drop support
//
// Revision 1.8  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.7.2.1  2007/04/12 20:44:29  geconomos
// Removed support for point and arrow annotations
//
// Revision 1.7  2007/03/02 21:40:54  geconomos
// cleanup
//
// Revision 1.6  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.5  2007/01/16 16:48:58  gdavidson
// Changed vxRenderingNodePlugin
//
// Revision 1.4  2006/12/04 20:41:25  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.3  2006/09/25 13:25:04  geconomos
// commented out unneeded  log debugging
//
// Revision 1.2  2006/09/14 20:10:50  geconomos
// added rendering engine
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2006/06/05 15:08:19  frank
// updated for visual studio 2005
//
// Revision 1.4  2006/01/19 20:54:08  frank
// got rid of typedef
//
// Revision 1.3  2005/06/08 15:02:49  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:15:03  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.4  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.3  2004/04/05 20:31:29  frank
// fixed problem with ellipse annotation showing up on curved view
//
// Revision 1.2  2004/03/02 19:19:35  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.11.2.1  2004/02/25 15:00:49  frank
// formatting
//
// Revision 1.11  2004/01/23 03:58:06  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.10  2004/01/14 15:45:14  michael
// introduced y-flip from mouse (windows) to screen (OpenGL) and a compensating
// flip in screen to world to account for curve working with y=0 being on top.
//
// Revision 1.9  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.8  2004/01/07 18:03:37  frank
// Fixed problem of the curved MPR not re-displaying when you initialize from an annotation.
//
// Revision 1.7  2004/01/07 16:50:27  wenli
// Issue # 3570 : Adjusting an arrow annotation from the Curved MPR is increasing the annotation size
//
// Revision 1.6  2003/12/23 23:02:09  wenli
// Add GetBoundingRect() to slove the ssue #3167: missing ablility to move the annotation text in curved
//
// Revision 1.5  2003/12/15 16:09:10  frank
// Issue #3569: Initialized the oblique and curved views from annotations
//
// Revision 1.4  2003/11/19 14:45:47  frank
// Initialized curved view properly
//
// Revision 1.3  2003/10/22 14:47:35  frank
// Added camera initialization to the viewer type
//
// Revision 1.2  2003/05/20 16:37:06  frank
// Name change in PrimitiveUtilities
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.15  2003/04/30 16:38:31  frank
// Fixed file revision define
//
// Revision 1.14  2003/04/29 14:31:31  frank
// Removed IsManipulatorSupported
//
// Revision 1.13  2003/04/21 15:54:44  dongqing
// code review
//
// Revision 1.12  2003/04/09 13:59:33  frank
// Fixed crash if there is no vessel data
//
// Revision 1.11  2003/03/20 21:28:46  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.10  2003/03/18 21:27:50  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.9  2003/03/07 18:18:58  frank
// code cleanup
//
// Revision 1.8  2003/03/06 15:59:10  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.7  2003/03/05 12:51:56  frank
// Extended interface to include the ability to render in either screen or world space.
//
// Revision 1.6  2003/03/04 18:00:06  frank
// removed excess debug
//
// Revision 1.5  2003/03/03 18:28:24  frank
// got projection working
//
// Revision 1.4  2003/02/28 20:55:15  frank
// Updated to reflect changes in base class
//
// Revision 1.3  2003/02/28 19:24:56  frank
// Added projection function
//
// Revision 1.2  2003/02/27 16:09:14  frank
// First tests
//
// Revision 1.1  2003/02/18 22:32:10  frank
// Initial version
//
// $Id: vxViewerTypeMPRCurved.C,v 1.10.2.1 2009/06/15 19:58:08 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeMPRCurved.C,v 1.10.2.1 2009/06/15 19:58:08 gdavidson Exp $
