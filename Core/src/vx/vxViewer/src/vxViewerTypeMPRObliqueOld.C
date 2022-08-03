// $Id: vxViewerTypeMPRObliqueOld.C,v 1.2 2007/03/23 14:34:38 romy Exp $
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
#include "vxViewerTypeMPRObliqueOld.h"
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
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
vxViewerTypeMPRObliqueOld::vxViewerTypeMPRObliqueOld(vxEnvironment & environment)
:vxViewerType(environment)
{
}


/**
 * Initializes the view
 */
void vxViewerTypeMPRObliqueOld::InitializeView()
//*******************************************************************
{

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
void vxViewerTypeMPRObliqueOld::SetupView()
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
Point2D<int2> vxViewerTypeMPRObliqueOld::ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const
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
bool vxViewerTypeMPRObliqueOld::ConvertScreenToWorld( const Point2D<int2> & screenCoord, Point<float4> & worldCoord, bool bConsiderRenderingMode) const
{
 GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble mvmatrix[16], projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  GLdouble X, Y, Z;

  // get near point
  if (gluUnProject((GLdouble)screenCoord.X(), (GLdouble)screenCoord.Y(), 0, mvmatrix, projmatrix, viewport, &X, &Y, &Z) == GL_FALSE)
    return false;

  // copy coordinate values
  Point<float4> point(X, Y, Z);

  // get far point
  if (gluUnProject((GLdouble)screenCoord.X(), (GLdouble)screenCoord.Y(), 1, mvmatrix, projmatrix, viewport, &X, &Y, &Z) == GL_FALSE)
    return false;

  // copy coordinate values
  Normal<float4> normal(point[0] - X, point[1] - Y, point[2] - Z);

  // compute intersection with oblique plane
  if( GetEnvironment().GetPlaneOblique() != NULL )
    GetEnvironment().GetPlaneOblique()->GetIntersection(point, normal, worldCoord);
  else
    return false;

  return true;
} // GetWorldFromScreen()



/**
 * is this annotation type supported by this viewer type?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeMPRObliqueOld::IsAnnotationSupported(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  if (dynamic_cast<const vxHandle *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationPoint *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationArrow *>(&handlesObject))
    return true;
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
bool vxViewerTypeMPRObliqueOld::InitializeFrom(vxAnnotation & annotation)
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
bool vxViewerTypeMPRObliqueOld::IsVisible(const vxHandlesObj & handlesObject) const
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
bool vxViewerTypeMPRObliqueOld::IsVisible(const Point3Df & position) const
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
void vxViewerTypeMPRObliqueOld::OnModified(const type_info & type, 
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
bool vxViewerTypeMPRObliqueOld::CanDragDrop(const DragDropEnum eSourceType)
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
float4 vxViewerTypeMPRObliqueOld::GetLeftPlane() const
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
float4 vxViewerTypeMPRObliqueOld::GetRightPlane() const
//*******************************************************************
{
  return -GetLeftPlane();
} // GetRightPlane()


/**
 * Return the bottom plane
 * @return Bottom plane
 */
float4 vxViewerTypeMPRObliqueOld::GetTopPlane() const
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
float4 vxViewerTypeMPRObliqueOld::GetBottomPlane() const
//*******************************************************************
{
  return -GetTopPlane();
} // GetTopPlane()


/**
 * Get near plane
 * @return near plane
 */
float4 vxViewerTypeMPRObliqueOld::GetNearPlane() const
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
float4 vxViewerTypeMPRObliqueOld::GetFarPlane() const
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
vxRenderingNodePlugin * vxViewerTypeMPRObliqueOld::CreateRenderingEnginePlugin()
{
  return new vxRenderingNodePluginRaytracer( GetEnvironment() );
}

// Revision History:
// $Log: vxViewerTypeMPRObliqueOld.C,v $
// Revision 1.2  2007/03/23 14:34:38  romy
// new DragDropEnum added
//
// Revision 1.1  2007/02/13 21:02:28  romy
// fixed issue with old oblique renderer and associated viewer type
//

// $Id: vxViewerTypeMPRObliqueOld.C,v 1.2 2007/03/23 14:34:38 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeMPRObliqueOld.C,v 1.2 2007/03/23 14:34:38 romy Exp $
