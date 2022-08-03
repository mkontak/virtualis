// $Id: vxViewerTypeMPROblique.C,v 1.15 2007/06/28 19:33:55 jmeade Exp $
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
#include "vxViewerTypeMPROblique.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"
#include "vxUtils.h"
#include "vxIntensityConverter.h"
#include "vxAnnotationPoint.h"
#include "vxAnnotationArrow.h"
#include "vxAnnotationDistance.h"
#include "vxAnnotationAngle.h"
#include "vxAnnotationBox.h"
#include "vxAnnotationEllipse.h"
#include "vxAnnotationComponent.h"
#include "vxHandle.h"
#include "vxRenderingNodePluginRaytracer.h"
#include "vxRayGenerator.h"
#include "Plane.h"

// defines
#define FILE_REVISION "$Revision: 1.15 $"


/**
 * Constructor
 */
vxViewerTypeMPROblique::vxViewerTypeMPROblique(vxEnvironment & environment)
:vxViewerType(environment)
{
  SetRenderCoordinateSystem( vxViewerType::eScreenSpace );
}


/**
 * Initializes the view
 */
void vxViewerTypeMPROblique::InitializeView()
//*******************************************************************
{

  GetEnvironment().GetCamera()->SetFovDegrees( 45.0f );

  // zoom in a bit on the volume so it's not so far away and have to zoom it in every time
  const float4 fZoomIn( vxUtils::GetObliqueZoomFactor() );
  Vector3Df size( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
  Box<float4> box( Point3Df( size * fZoomIn ), Point3Df( size * ( 1 - fZoomIn ) ) );

  // show an inferior view
  vxUtils::SetStandardCameraView(*(GetEnvironment().GetCamera()), vxUtils::INFERIOR, box, true);
  GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  // set the plane at the volume center
  * GetEnvironment().GetPlaneOblique() = vxPlaneOblique<float4>( GetEnvironment().GetCamera()->GetViewDirection(), box.GetMidPoint() );
  GetEnvironment().GetPlaneOblique()->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );

} // InitializeView()


/**
 * This method is responsable to initialize the OpenGL viewing
 */
void vxViewerTypeMPROblique::SetupView()
//*******************************************************************
{
  // bound input to the window area
  vxWinRect winRect(GetEnvironment().GetWinRect());
  vxCamera & camera(*GetEnvironment().GetCamera());
  const Point<float4> eye(camera.GetEye());
  const Point<float4> ref(camera.GetViewReference());
  const Normal<float4> up(camera.GetViewUp());
  float4 fAspectScale((float4)winRect.GetWindowSize().second/(float4)winRect.GetWindowSize().first);
  float4 fAspectScaleX(1.0F), fAspectScaleY(1.0F);

  if (fAspectScale > 1)
  {
    fAspectScaleY = fAspectScale;
  }
  else
  {
    fAspectScaleX = 1.0F/fAspectScale;
  }

  glViewport(winRect.GetReferencePoint().first, 
             winRect.GetReferencePoint().second, 
             winRect.GetViewportSize().first, 
             winRect.GetViewportSize().second);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (camera.GetProjectionType() == vxCamera::PERSPECTIVE) // perspective projection
  { 
    glFrustum(fAspectScaleX*GetLeftPlane(),   fAspectScaleX*GetRightPlane(),
              fAspectScaleY*GetBottomPlane(), fAspectScaleY*GetTopPlane(),
                            GetNearPlane(),                 GetFarPlane());
  }
  else // parallel projection 
  { 
    glOrtho(fAspectScaleX*GetLeftPlane(),   fAspectScaleX*GetRightPlane(),
            fAspectScaleY*GetBottomPlane(), fAspectScaleY*GetTopPlane(),
                          GetNearPlane(),                 GetFarPlane());
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye.m_x, eye.m_y, eye.m_z, 
            ref.m_x, ref.m_y, ref.m_z, 
            up.X(),  up.Y(),  up.Z());
} // SetupView()


/**
 * Convert the mouse coordinates into screen coordinates
 * @param mouseCoord  mouse coordinates
 * @param screenCoord screen coordinates
 * @return succesful
 */
Point2D<int2> vxViewerTypeMPROblique::ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const
//*******************************************************************
{
  Point2D<int2> screenCoord(vxViewerType::ConvertMouseToScreen(mouseCoord));

  screenCoord.m_y = GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screenCoord.m_y;

  return screenCoord;
} // GetScreenFromMouse()


/**
 * @param screenCoord screen coordinates
 * @param worldCoord  world coordinates
 * @return succesful
 */
bool vxViewerTypeMPROblique::ConvertScreenToWorld( const Point2D<int2> & screenCoord, Point<float4> & worldCoord, bool bConsiderRenderingMode) const
{
  const Triple< uint4 > & s = GetEnvironment().GetRenderBuffer().GetRenderSize();

  const std::pair< int2, int2 > & size = GetEnvironment().GetWinRect().GetViewportSize();
  const std::pair< int2, int2 > & ref  = GetEnvironment().GetWinRect().GetReferencePoint();

  const float4 x = Interpolate( ref.first,  static_cast< float4 >( screenCoord.m_x ), static_cast< float4 >( size.first  - 1 ), 0.0F, 1.0F );
  const float4 y = Interpolate( ref.second, static_cast< float4 >( screenCoord.m_y ), static_cast< float4 >( size.second - 1 ), 0.0F, 1.0F );

  vxRayGenerator r( GetEnvironment(), vxRayGenerator::OBLIQUE );

  RaySegment rs1 = r.GenerateRay( 0,       0 );
  const Point3Df bl = Point3Df(  ( rs1.GetStartPoint() + rs1.GetEndPoint() ) / 2.0F );
  
  RaySegment rs2 = r.GenerateRay( s.m_x-1, 0 );
  const Point3Df br = Point3Df( ( rs2.GetStartPoint() + rs2.GetEndPoint() ) / 2.0F );
  
  RaySegment rs3 = r.GenerateRay( s.m_x-1, s.m_y-1 );
  const Point3Df tr = Point3Df( ( rs3.GetStartPoint() + rs3.GetEndPoint() ) / 2.0F );

  worldCoord.m_x = bl.m_x + ( x * ( br.m_x - bl.m_x ) ) + ( y * ( tr.m_x - br.m_x ) ); 
  worldCoord.m_y = bl.m_y + ( x * ( br.m_y - bl.m_y ) ) + ( y * ( tr.m_y - br.m_y ) ); 
  worldCoord.m_z = bl.m_z + ( x * ( br.m_z - bl.m_z ) ) + ( y * ( tr.m_z - br.m_z ) ); 
 
  return true;
} // GetWorldFromScreen()


/**
 * Get the world position of the given screen position
 *
 * @param worldPos  the input world position
 * @param screenPos the output screen position
 *
 * @return succesful
 */
bool vxViewerTypeMPROblique::ConvertWorldToScreen(const Point<float4> & worldPos, Point2D<int2> & screenPos ) const
{
  const Triple< uint4 > & s = GetEnvironment().GetRenderBuffer().GetRenderSize();

  vxRayGenerator r( GetEnvironment(), vxRayGenerator::OBLIQUE );
  
  RaySegment rs1 = r.GenerateRay( 0,       0 );
  const Point3Df bl = Point3Df(  ( rs1.GetStartPoint() + rs1.GetEndPoint() ) / 2.0F );
  
  RaySegment rs2 = r.GenerateRay( s.m_x-1, 0 );
  const Point3Df br = Point3Df( ( rs2.GetStartPoint() + rs2.GetEndPoint() ) / 2.0F );
  
  RaySegment rs3 = r.GenerateRay( s.m_x-1, s.m_y-1 );
  const Point3Df tr = Point3Df( ( rs3.GetStartPoint() + rs3.GetEndPoint() ) / 2.0F );

  const Vector< float4 > rightVector = br - bl;
  const Plane leftPlane( bl, Normal<float4>( rightVector ) );
  const float4 fDistanceX( leftPlane.GetDistance( worldPos ) );
  const float4 fX( fDistanceX / rightVector.GetLength() );

  const Vector<float4> upVector( tr - br );
  const Plane bottomPlane( br, Normal<float4>( upVector ) );
  const float4 fDistanceY( bottomPlane.GetDistance( worldPos ) );
  const float4 fY( fDistanceY / upVector.GetLength() );

  const std::pair< int2, int2 > & size = GetEnvironment().GetWinRect().GetViewportSize();
  const std::pair< int2, int2 > & ref  = GetEnvironment().GetWinRect().GetReferencePoint();

  screenPos.m_x = ref.first  + ( fX * ( size.first - 1 ));
  screenPos.m_y = ref.second + ( fY * ( size.second- 1 ));

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
bool vxViewerTypeMPROblique::ConvertWorldToScreen(const Point<float4> & worldPosition, 
                                                 Point2D<int2> & screenPos,
                                                 const GLint * piViewport,
                                                 const GLdouble * pfModelviewMatrix,
                                                 const GLdouble * pfProjectionMatrix ) const
{
  return ConvertWorldToScreen(worldPosition, screenPos);
} // GetScreenCoordinateFromParams()


/**
 * is this annotation type supported by this viewer type?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeMPROblique::IsAnnotationSupported(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  if (dynamic_cast<const vxHandle *>(&handlesObject))
    return true;
  //else if (dynamic_cast<const vxAnnotationPoint *>(&handlesObject))
  //  return true;
  //else if (dynamic_cast<const vxAnnotationArrow *>(&handlesObject))
  //  return true;
  else if (dynamic_cast<const vxAnnotationDistance *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationAngle *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationComponent *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationBox *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationEllipse *>(&handlesObject))
    return true;

  return false;
} // IsAnnotationSupported()


/**
 * Initialize view/objects based on annotation
 * @param annotation annotation
 * @return success
 */
bool vxViewerTypeMPROblique::InitializeFrom(vxAnnotation & annotation)
//*******************************************************************
{
  bool bPossible(false);

  if (annotation.GetNumHandles() == 1)
  {
    vxPlaneOblique<float4> plane(GetEnvironment().GetCamera()->GetViewDirection(), annotation.GetHandle(0).GetPosition());
    *GetEnvironment().GetPlaneOblique() = plane;
    GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
  }
  else if (annotation.GetNumHandles() > 1)
  {
    // find two different points!!!
    Point3Df point1(annotation.GetHandle(0).GetPosition());
    Point3Df point2(annotation.GetHandle(1).GetPosition());

    // try to see if the other ones are different
    int4 i;
    for (i=1; i<annotation.GetNumHandles(); i++)
    {
      point2 = annotation.GetHandle(i).GetPosition();
      if (point2 != point1)
      {
        break;
      }
    }

    // did we fail in finding two different ones
    if (point1 == point2)
    {
      point2 = point1 + Vector3Df(GetEnvironment().GetCamera()->GetViewRight());
    }

    // find a third point
    Point3Df point3(point2);
    for (i=2; i<annotation.GetNumHandles(); i++)
    {
      point3 = annotation.GetHandle(i).GetPosition();
      Normal3Df normal1(point1 - point2);
      Normal3Df normal2(point3 - point2);
      Normal3Df normal2inverted(-1.0F*normal2);

      if (point3 != point2 && normal1 != normal2 && normal1 != normal2inverted)
      {
        break;
      }
    }

    // did we fail in finding two different ones
    if (point3 == point2)
    {
      point3 = point1 + Vector3Df(GetEnvironment().GetCamera()->GetViewUp());
    }

    // create plane by using the two vectors defining the plane
    const Point3Df center(0.33333F * (point1.X()+point2.X()+point3.X()),
                          0.33333F * (point1.Y()+point2.Y()+point3.Y()),
                          0.33333F * (point1.Z()+point2.Z()+point3.Z()));
    const Normal3Df normal1(point3 - point2);
    const Normal3Df normal2(point1 - point2);
    Normal3Df normal(normal1.Cross(normal2));
    vxPlaneOblique<float4> plane(normal, point2);

    // now adjust the camera to have same distance as previous view
    float4 fDistPrev(GetEnvironment().GetPlaneOblique()->GetAbsDistance(GetEnvironment().GetCamera()->GetEye()));
    vxCamera * pCamera(GetEnvironment().GetCamera());
    pCamera->SetEye(center - fDistPrev*normal);
    pCamera->SetViewReference(center);
    const Normal3Df normalUp(normal1.X()+normal2.X(), normal1.Y()+normal2.Y(), normal1.Z()+normal2.Z());
    pCamera->SetViewUp(normalUp);

    *GetEnvironment().GetPlaneOblique() = plane;
    GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
    GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));

    bPossible = true;
  }

  return bPossible;
} // InitializeFrom()

/**
 * Is the handlesSet visible and should be drawn?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeMPROblique::IsVisible(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  Triple<float4> units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
  float fDelta(0.5 * Min(units[0], Min(units[1], units[2])));

  for (int i(0); i<handlesObject.GetNumHandles(); i++)
  {
    if (fabs(GetEnvironment().GetPlaneOblique()->GetDistance(handlesObject.GetHandle(i).GetPosition())) > fDelta)
    {
      return false;
    }
  }

  return true;
} // IsVisible()


/**
 * Is this position visible in the current view?
 * @param position query position
 * @return true if visible
 */
bool vxViewerTypeMPROblique::IsVisible(const Point3Df & position) const
//*******************************************************************
{
  Triple<float4> units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
  float fDelta(0.5 * Min(units[0], Min(units[1], units[2])));

  return (fabs(GetEnvironment().GetPlaneOblique()->GetDistance(position)) <= fDelta);
} // IsVisible()


/**
 * Handles modified events
 * @param type type information
 * @param info modified info list
 */
void vxViewerTypeMPROblique::OnModified(const type_info & type, 
                                        const vxModifiedInfo & info)
//*******************************************************************
{
  // has the viewer been created yet?
  if (GetEnvironment().GetElement(vxIDs::WindowHandle) != NULL)
  {
    // has the environment been modified because the plane flag was set?
    if (type == typeid(vxEnvironment) && info.GetFlag() == vxUtils::PLANE_INITIALIZED)
    {
      InitializeView(); // update the camera
    }
  }

  return;
} // OnModified()


/**
 * Returns if the viewer supports dropping of the specified type
 * @param eSourceType drop type
 * @return true if element can be fropped, false otherwise
 */
bool vxViewerTypeMPROblique::CanDragDrop(const DragDropEnum eSourceType)
//*******************************************************************
{
  if (eSourceType == vxViewerType::COLORMAP || eSourceType == vxViewerType::DATASET || eSourceType == vxViewerType::PRESET )
  {
    return true;
  }

  return false;
} // CanDragDrop()


/**
 * Get left plane
 * @param tight fit or outer circle
 * @return left plane
 */
float4 vxViewerTypeMPROblique::GetLeftPlane() const
//*******************************************************************
{
  const float fAspectRatio(GetEnvironment().GetCamera()->GetAspectRatio());
  const float fFovVerticalDegrees(GetEnvironment().GetCamera()->GetFovDegrees());
  float fResult(0.0), fScale;

  if (GetEnvironment().GetCamera()->GetProjectionType() == vxCamera::PERSPECTIVE)
  {
    fScale = GetNearPlane();
  }
  else
  {
    fScale = GetEnvironment().GetCamera()->GetViewReference().GetEuclideanDistance(GetEnvironment().GetCamera()->GetEye());
  }

  if (fAspectRatio >= 1)
  {
    return -tan((fFovVerticalDegrees / 2.0F) * (M_PI/180.0F)) * fScale * fAspectRatio;
  }
  else
  {
    return -tan((fFovVerticalDegrees / 2.0F) * (M_PI/180.0F)) * fScale / fAspectRatio;
  }
} // GetLeftPlane()


/**
 * Get right plane
 * @param tight fit or outer circle
 * @return right plane
 */
float4 vxViewerTypeMPROblique::GetRightPlane() const
//*******************************************************************
{
  return -GetLeftPlane();
} // GetRightPlane()


/**
 * Return the bottom plane
 * @return Bottom plane
 */
float4 vxViewerTypeMPROblique::GetTopPlane() const
//*******************************************************************
{
  const float fFovVerticalDegrees(GetEnvironment().GetCamera()->GetFovDegrees());
  float fResult(0.0), fScale;

  if (GetEnvironment().GetCamera()->GetProjectionType() == vxCamera::PERSPECTIVE)
  {
    fScale = GetNearPlane();
  }
  else
  {
    fScale = GetEnvironment().GetCamera()->GetViewReference().GetEuclideanDistance(GetEnvironment().GetCamera()->GetEye());
  }

  return tan((fFovVerticalDegrees / 2.0F) * (M_PI/180.0F)) * fScale;
} // GetBottomPlane()


/**
 * Return the top plane
 * @return top plane
 */
float4 vxViewerTypeMPROblique::GetBottomPlane() const
//*******************************************************************
{
  return -GetTopPlane();
} // GetTopPlane()


/**
 * Get near plane
 * @return near plane
 */
float4 vxViewerTypeMPROblique::GetNearPlane() const
//*******************************************************************
{
  Point3Df center(0.5 * GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  Point3Df size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  float4 fOuterRadius((size - center).GetLength());
  float4 fNearDist(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(center));
  fNearDist -= fOuterRadius;

  return Bound(0.001f, fNearDist, (std::numeric_limits<float>::max)());
} // GetNearPlane()


/**
 * Get far plane
 * @return far plane
 */
float4 vxViewerTypeMPROblique::GetFarPlane() const
//*******************************************************************
{
  Point3Df center(0.5 * GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  Point3Df size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  float4 fOuterRadius((size - center).GetLength());
  float4 fFarDist(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(center));
  fFarDist += fOuterRadius;

  return fFarDist;
} // GetFarPlane()


/**
 * Creates an instance of a rendering engine plugin for this particular viewer type
 *
 * @return    rendering engine plugin  instance
 */
vxRenderingNodePlugin * vxViewerTypeMPROblique::CreateRenderingEnginePlugin()
{
  return new vxRenderingNodePluginRaytracer( GetEnvironment() );
}

// Revision History:
// $Log: vxViewerTypeMPROblique.C,v $
// Revision 1.15  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.14  2007/03/23 14:34:38  romy
// new DragDropEnum added
//
// Revision 1.13.2.1  2007/04/05 18:25:38  geconomos
// reset if camera field of view
//
// Revision 1.13  2007/02/12 14:24:31  geconomos
// reworked MouseToScreen, WorldToScreen and ScreenToWorld methods
//
// Revision 1.12  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.11  2007/01/16 16:48:58  gdavidson
// Changed vxRenderingNodePlugin
//
// Revision 1.10  2006/12/04 20:40:52  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.9  2006/08/03 14:07:50  geconomos
// added "rendering engine" support
//
// Revision 1.8  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6  2005/09/23 20:10:35  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.5.6.1  2005/09/22 00:18:32  geconomos
// added check for null plane pointer in ComputeFourCornerPoints
//
// Revision 1.5.2.5  2005/08/15 19:33:44  geconomos
// initialized camera to the "inferior" standard position
//
// Revision 1.5.2.4  2005/08/11 15:12:14  frank
// Issue #4359: Fixed flipping of oblique view every time you select a distance annotation
//
// Revision 1.5.2.3  2005/08/01 19:12:32  frank
// changed function name according to coding standards
//
// Revision 1.5.2.2  2005/07/29 18:20:39  frank
// Issue #4318: Added standard orientations button to oblique view
//
// Revision 1.5.2.1  2005/07/29 14:35:07  frank
// dead code
//
// Revision 1.5  2005/05/23 12:21:41  frank
// removed unused variables
//
// Revision 1.4  2004/04/26 18:58:39  michael
// moved GetLeft() / GetRight() / etc. from vxCamera to vxViewerType
//
// Revision 1.3  2004/04/06 16:27:47  frank
// unified environment access naming
//
// Revision 1.2  2004/03/02 19:19:35  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.40  2004/01/23 04:00:05  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.39  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.38  2004/01/05 19:01:18  michael
// added ellipse and box as annotations to the oblique MPRs
//
// Revision 1.37  2003/09/29 17:18:44  michael
// accounting for the new event types when initializing the camera
//
// Revision 1.36  2003/09/22 21:21:28  michael
// added missing include for plain handle
//
// Revision 1.35  2003/09/22 21:14:17  michael
// added new handle type to be drawable
//
// Revision 1.34  2003/08/27 12:27:09  michael
// rearranged code to not have duplicate code
//
// Revision 1.33  2003/08/26 20:50:03  michael
// checked in fix for perspective camera
//
// Revision 1.32  2003/08/21 13:10:34  michael
// added functions for OpenGL setup; oblique now does not anymore use the
// camera's functions
//
// Revision 1.31  2003/05/29 14:10:50  michael
// restructured code in InitializeCamera()
//
// Revision 1.30  2003/05/29 13:58:04  michael
// need to add ViewAll() on initialization
//
// Revision 1.29  2003/05/29 11:39:46  michael
// Simplified the camera initialization and the view setup, correctly handling the
// different aspect ratios without modifying the bounding box of the camera.
//
// Revision 1.27  2003/05/23 15:45:06  michael
// fixed issue #3061
//
// Revision 1.26  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.25  2003/05/15 15:06:51  geconomos
// Code walkthru.
//
// Revision 1.24  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.23  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.22  2003/04/18 16:04:09  michael
// initial code review
//
// Revision 1.21  2003/04/16 14:42:02  michael
// removed redundant include
//
// Revision 1.20  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.19  2003/03/30 21:03:23  michael
// formatting and cosmetics
//
// Revision 1.18  2003/03/22 16:04:09  michael
// oblique now initializes from point annotations
//
// Revision 1.17  2003/03/21 00:11:00  michael
// Added the viewport initialization, fixing issue #2941 and #2910
//
// Revision 1.16  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.15  2003/03/18 21:23:02  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.14  2003/03/17 19:47:34  geconomos
// Issue #2914. Added DropType property.
//
// Revision 1.13  2003/03/04 15:34:17  geconomos
// Added methods for processing parent environment's modified queue.
//
// Revision 1.12  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.11  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.10  2003/01/30 00:36:28  michael
// using rayiterator and samples rather than voxels (in vxAnnotationBox)
//
// Revision 1.9  2003/01/28 23:41:38  michael
// fixed ellipse scaling ... dual scaling (corner handles) still to be done ...
//
// Revision 1.8  2003/01/22 18:00:26  michael
// added support of annotations in oblique views
//
// Revision 1.7  2003/01/22 15:07:02  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.6  2003/01/15 16:29:52  michael
// made methods const that do not alter the state
//
// Revision 1.5  2003/01/13 23:19:35  michael
// added get/set for the environment (protected member in base class)
//
// Revision 1.4  2003/01/09 14:56:04  michael
// adjusted methods to take new annotation classes
//
// Revision 1.3  2002/12/19 16:54:08  michael
// fixed bug in code (initialization of position)
//
// Revision 1.2  2002/12/18 18:22:02  michael
// moved OpenGL frame buffer reading to base class
//
// Revision 1.1  2002/12/18 18:12:46  michael
// added initial version of vxViewerTypeMPROblique
//
// Revision 1.2  2002/12/18 14:52:38  geconomos
// implemented missing destructors
//
// Revision 1.1  2002/12/17 14:41:40  michael
// initial version of viewer type base class and the MPROrtho implementation
//
// $Id: vxViewerTypeMPROblique.C,v 1.15 2007/06/28 19:33:55 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeMPROblique.C,v 1.15 2007/06/28 19:33:55 jmeade Exp $
