// $Id: vxViewerTypeMPROrtho.C,v 1.17.2.1 2009/09/08 21:24:44 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxIntensityConverter.h"
#include "vxUtils.h"
#include "vxAnnotation.h"
#include "vxAnnotationPoint.h"
#include "vxAnnotationArrow.h"
#include "vxAnnotationDistance.h"
#include "vxAnnotationAngle.h"
#include "vxAnnotationBox.h"
#include "vxAnnotationEllipse.h"
#include "vxAnnotationComponent.h"
#include "vxHandle.h"
#include "vxBlockVolumeIterator.h"
#include "vxRenderingNodePluginPlanetracer.h"



// defines
#define FILE_REVISION "$Revision: 1.17.2.1 $"


/**
 * Constructor
 */
vxViewerTypeMPROrtho::vxViewerTypeMPROrtho(vxEnvironment & environment)
:vxViewerType(environment)
//*******************************************************************
{
}


/**
 * Initializes the view
 */
void vxViewerTypeMPROrtho::InitializeView()
//*******************************************************************
{
  GetEnvironment().GetCamera()->SetFovDegrees( 45.0f );

  uint1 uIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

  // find the actual camera setup (reference, eye, up)
  //////////////////////////////////////////////////////
  vxUtils::StandardCameraViewEnum eCameraView(vxUtils::INFERIOR);

  if (GetEnvironment().GetPlaneOrtho() != NULL)
  {  
    switch(uIndex)
    {
    case 0:
      eCameraView = vxUtils::LEFT;
      break;
    case 1:
      eCameraView = vxUtils::ANTERIOR;
      break;
    case 2:
      eCameraView = vxUtils::INFERIOR;
      break;
    }
  }
  
  // get original values
  vxZoomFactor zoomFactor(*GetEnvironment().GetZoomFactor());
  Point3Df center(0.5F * GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  float4 fWidthFull(0.5F * GetEnvironment().GetZoomFactor()->GetSize());

  // go ahead ...
  float4 fWidthHalf(0.5F * fWidthFull);
  Box<float4> box(center[0] - fWidthHalf, center[1] - fWidthHalf, center[2] - fWidthHalf, 
                  center[0] + fWidthHalf, center[1] + fWidthHalf, center[2] + fWidthHalf);
  vxUtils::SetStandardCameraView(*(GetEnvironment().GetCamera()), eCameraView, box, true);

  vxWinRect winRect(GetEnvironment().GetWinRect());
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  float4 fWidthX(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisX]);
  float4 fHeightY(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fAspectRatioVolume(fHeightY/fWidthX);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());
  float4 fWidth(fWidthX);

  if (fAspectRatioWindow < fAspectRatioVolume)
  {
    fWidth = fHeightY;
  }

  // reset the zoom factor
  GetEnvironment().GetZoomFactor()->SetCenter(center);
  GetEnvironment().GetZoomFactor()->SetSize(fWidth);
  GetEnvironment().GetZoomFactor()->Modified(vxModInfo(vxZoomFactor, 0));
} // InitializeView()


/**
 * This method is responsable to initialize the OpenGL viewing
 */
void vxViewerTypeMPROrtho::SetupView()
//*******************************************************************
{
  vxWinRect winRect(GetEnvironment().GetWinRect());
  vxCamera camera(*GetEnvironment().GetCamera());
  const Point<float4> eye(camera.GetEye());
  const Point<float4> ref(camera.GetViewReference());
  const Normal<float4> up(camera.GetViewUp());

  Point3Df center(GetEnvironment().GetZoomFactor()->GetCenter());

  // set the viewport
  /////////////////////
  glViewport(winRect.GetReferencePoint().first, 
             winRect.GetReferencePoint().second, 
             winRect.GetViewportSize().first, 
             winRect.GetViewportSize().second);

  // just setup the view using the camera
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();     
  glOrtho(GetLeftPlane()  , GetRightPlane(),
          GetBottomPlane(), GetTopPlane(), 
          GetNearPlane()  , GetFarPlane());
          //0, 10000); // THIS is a HACK for RSNA 2002
 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(center.m_x + eye.m_x - ref.m_x, 
            center.m_y + eye.m_y - ref.m_y, 
            center.m_z + eye.m_z - ref.m_z, 
            center.m_x, center.m_y, center.m_z, 
            up.X(),  up.Y(),  up.Z());

  return;
} // SetupView()


/**
 * Initialize view/objects based on annotation
 * @param annotation
 * @return success
 */
bool vxViewerTypeMPROrtho::InitializeFrom(vxAnnotation & annotation)
//*******************************************************************
{
  Triple<float4> units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
  float4 fHalfUnitDistance(0.5F*fabs(units[GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord()]));
  vxPlaneOrtho<float4> plane(GetEnvironment().GetPlaneOrtho()->GetNormal(), annotation.GetHandle(0).GetPosition());
  bool bPossible(true);

  for (int i(1); i<annotation.GetNumHandles(); i++)
  {
    float4 fDistance(plane.GetDistance(annotation.GetHandle(i).GetPosition()));
    if (fabs(plane.GetDistance(annotation.GetHandle(i).GetPosition())) >= fHalfUnitDistance)
    {
      bPossible = false;
      break;
    }
  }

  if (bPossible == true)
  {
    *GetEnvironment().GetPlaneOrtho() = plane;
    GetEnvironment().GetPlaneOrtho()->Modified(vxModInfo(vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED));
  }

  return bPossible;
} // InitializeFrom()


/**
 * Handles modified events.
 * @param type the type of the modified object
 * @param info the detailed information
 */
void vxViewerTypeMPROrtho::OnModified(const type_info & type, 
                                      const vxModifiedInfo & info)
//*******************************************************************
{

  // has the viewer been created yet?
  if (GetEnvironment().GetElement(vxIDs::WindowHandle) != NULL)
  {
    // was the camera initialized or the plane orientation changed?
    const bool bCameraInitialized(info.GetFlag() == vxUtils::CAMERA_INITIALIZED && type == typeid(vxCamera));
    const bool bOrientationChanged(info.GetFlag() == vxUtils::PLANE_ORIENTATION_CHANGED && type == typeid(vxPlaneOrtho<float4>));

    if (bCameraInitialized == true || bOrientationChanged == true) 
    {
      InitializeView();
    }
  }

  return;
} // OnModified()


/**
 * Assumes intersample distance in Euclidean space
 * @param raySegment
 * @param fIntersampleDistance
 * @return vector
 */
Vector4Df GetSampleIncrementVector(const RaySegment & raySegment, 
                                   const float4 fIntersampleDistance)
//*******************************************************************
{
  return Vector4Df(raySegment.GetDirection().GetNormalizedXYZ() * fIntersampleDistance);
} // GetSampleIncrementVector()


/**
 * Convert the mouse coordinates into screen coordinates
 * @param mouseCoord  mouse coordinates
 * @param screenCoord screen coordinates
 * @return succesful
 */
Point2D<int2> vxViewerTypeMPROrtho::ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const
//*******************************************************************
{
  Point2D<int2> screenCoord(vxViewerType::ConvertMouseToScreen(mouseCoord));

  screenCoord.m_y -= GetEnvironment().GetWinRect().GetReferencePoint().second;
  screenCoord.m_y = GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screenCoord.m_y;
  screenCoord.m_y += GetEnvironment().GetWinRect().GetReferencePoint().second;

  return screenCoord;
} // GetScreenFromMouse()


/**
 * Convert from screen to world
 * @param screenCoord screen coordinates
 * @param worldCoord  world coordinates
 * @return succesful
 */
bool vxViewerTypeMPROrtho::ConvertScreenToWorld(const Point2D<int2> & screenCoord, Point<float4> & worldCoord,
                                                bool bConsiderRenderingMode) const
//*******************************************************************
{

  // bound input to the window area
  vxWinRect winRect(GetEnvironment().GetWinRect());
  Point2D<int2> maxBound(winRect.GetReferencePoint().first  + winRect.GetViewportSize().first  - 2,
                          winRect.GetReferencePoint().second + winRect.GetViewportSize().second - 2);

  // clamp mouse position to current viewer area
  Point2D<int2> position
  (
    Bound(winRect.GetReferencePoint().first,  screenCoord.X(), maxBound.X()),
    Bound(winRect.GetReferencePoint().second, screenCoord.Y(), maxBound.Y())
  );

  // set ray start and direction
  Point3Df pointOnRay;
  if (ConvertScreenToWorldUsingOpenGL(position, pointOnRay) == false) return false;
    
  // determine the slab bounds, taking into account the potential offset distance
  const std::pair<uint4, uint4> thinSlab(vxUtils::GetSlabPair(GetEnvironment()));
  const Triplef & units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
  const uint4 uAxisIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

  // check the rendering mode
  vxRenderingMode::vxRenderingAlgorithmEnum eAlgorithm(GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm());
  if (bConsiderRenderingMode && (eAlgorithm == vxRenderingMode::MIP ||
                                 eAlgorithm == vxRenderingMode::THIN_SLAB ||
                                 eAlgorithm == vxRenderingMode::XRAY))
  {

    // set up the sampling ray end points
    Point3Df slabStartPointVolume(pointOnRay.GetDividedAnIso(units));
    Point3Df slabEndPointVolume  (pointOnRay.GetDividedAnIso(units));
    slabStartPointVolume[ uAxisIndex ] = thinSlab.first;
    slabEndPointVolume  [ uAxisIndex ] = thinSlab.second;

    // create the sampling ray
    if (slabStartPointVolume == slabEndPointVolume) slabEndPointVolume += Vector3Df(0, 0, 0.01F);
    RaySegment raySegment(slabStartPointVolume, slabEndPointVolume);
    Vector4Df sampleIncVector(GetSampleIncrementVector(raySegment, 0.5F));
    vxBlockVolumeRayIterator<uint2>  dataIter(* GetEnvironment().GetIntensityVolume(), raySegment, sampleIncVector);
    vxBlockVolumeRayIterator<uint2> labelIter(* GetEnvironment().GetLabelVolume(), raySegment, sampleIncVector);

    // cast the ray and sample appropriately
    return ConvertRayToWorld(dataIter, labelIter, worldCoord);

  }
  else
  {

    // create the exact intersection point
    worldCoord = pointOnRay;
    worldCoord[ uAxisIndex ] = thinSlab.first * units[ uAxisIndex ];

    return true;
/*
    // forget about clamping!!!
    Vector<float4> volBound(units[0]*(volDim[0]-1), units[1]*(volDim[1]-1), units[2]*(volDim[2]-1));
    intersectionPoint[0] = Bound(0.0F, intersectionPoint[0], volBound[0]);
    intersectionPoint[1] = Bound(0.0F, intersectionPoint[1], volBound[1]);
    intersectionPoint[2] = Bound(0.0F, intersectionPoint[2], volBound[2]);

    worldCoord = intersectionPoint;
    return true;
*/
  }

  return false;

} // ConvertScreenToWorld()


/**
 * Is this annotation type supported by this viewer type?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeMPROrtho::IsAnnotationSupported(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  if (dynamic_cast<const vxHandle *>(&handlesObject))
    return true;
  if (dynamic_cast<const vxAnnotationPoint *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationArrow *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationDistance *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationAngle *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationBox *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationEllipse *>(&handlesObject))
    return true;
  else if (dynamic_cast<const vxAnnotationComponent *>(&handlesObject))
    return true;

  return false;
} // IsAnnotationSupported()


/**
 * Is the handlesSet visible and should be drawn?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeMPROrtho::IsVisible(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::DEFAULT)
  {
    for (int i(0); i<handlesObject.GetNumHandles(); i++)
    {
      if (IsVisible(handlesObject.GetHandle(i).GetPosition()) == false)
      {
        return false;
      }
    }

    return true;
  }
  else
  {
    // at this point, we are not supporting anything in MIP and thin slab
    return false;
  }
} // IsVisible()


/**
 * Is this position visible in the current view?
 * @param position the query position in world coordinates
 * @return true if visible
 */
bool vxViewerTypeMPROrtho::IsVisible(const Point3Df & point) const
//*******************************************************************
{
  uint1 uAxisIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord()); // 0, 1, 2
  const int4 iPlaneOrthoDistance(0.5F + vxUtils::CalculateOffsetDistance(GetEnvironment().GetPlaneOrtho()->GetDistance() / GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] + 0.5F,
                                                                         GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim()[ uAxisIndex ],
                                                                         GetEnvironment().GetOffset(),
                                                                         GetEnvironment().GetMaximumOffset()));
  const int4 iPointOrthoDistance(0.5F + point.GetDividedAnIso(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits())[ uAxisIndex ]);
  bool bVisible(iPlaneOrthoDistance == iPointOrthoDistance);
  
  // a serious flaw in the 1.0 version of the .NET framework -- donot remove the line below
#ifndef _WIN64
  __asm mov eax,100
#endif

  return bVisible;
} // IsVisible()


/**
 * Returns if the viewer supports dropping of the specified type.
 *
 * @param eSourceType drop type
 * @return true if element can be fropped, false otherwise
 */
bool vxViewerTypeMPROrtho::CanDragDrop(const DragDropEnum eSourceType)
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
float4 vxViewerTypeMPROrtho::GetLeftPlane() const
//*******************************************************************
{
  vxWinRect winRect(GetEnvironment().GetWinRect());
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  float4 fWidthX(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisX]);
  float4 fHeightY(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fAspectRatioVolume(fHeightY/fWidthX);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());

  float4 fResult(-0.5F * GetEnvironment().GetZoomFactor()->GetSize());

  if (fAspectRatioWindow < fAspectRatioVolume)
  {
    fResult *= 1.0F/fAspectRatioWindow;
  }

  return fResult;
} // GetLeftPlane()


/**
 * Get right plane
 * @param tight fit or outer circle
 * @return right plane
 */
float4 vxViewerTypeMPROrtho::GetRightPlane() const
//*******************************************************************
{
  return -GetLeftPlane();
} // GetRightPlane()


/**
 * return the bottom plane
 * @param tight fit or outer circle
 * @return Bottom plane
 */
float4 vxViewerTypeMPROrtho::GetBottomPlane() const
//*******************************************************************
{
  vxWinRect winRect(GetEnvironment().GetWinRect());
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());

  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  float4 fWidthX(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisX]);
  float4 fHeightY(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fAspectRatioVolume(fHeightY/fWidthX);

  float4 fResult(-0.5F * GetEnvironment().GetZoomFactor()->GetSize());

  if (fAspectRatioWindow > fAspectRatioVolume)
  {
    fResult *= fAspectRatioWindow;
  }

  return fResult;
} // GetBottomPlane()


/**
 * return the top plane
 * @param tight fit or outer circle
 * @return top plane
 */
float4 vxViewerTypeMPROrtho::GetTopPlane() const
//*******************************************************************
{
  return -GetBottomPlane();
} // GetTopPlane()


/**
 * Get near plane
 * @param tight fit or outer circle
 * @return near plane
 */
float4 vxViewerTypeMPROrtho::GetNearPlane() const
//*******************************************************************
{
  float4 fNearDist(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(GetEnvironment().GetZoomFactor()->GetCenter()));
  fNearDist -= sqrt(3.0F) * GetEnvironment().GetZoomFactor()->GetSize();

  return Bound(0.001f, fNearDist, (std::numeric_limits<float>::max)());
} // GetNearPlane()


/**
 * Get far plane
 * @param tight fit or outer circle
 * @return far plane
 */
float4 vxViewerTypeMPROrtho::GetFarPlane() const
//*******************************************************************
{
  float4 fFarDist(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(GetEnvironment().GetZoomFactor()->GetCenter()));
  fFarDist += sqrt(3.0F) * GetEnvironment().GetZoomFactor()->GetSize();

  return fFarDist;
} // GetFarPlane()


/**
 * Creates an instance of a rendering engine plugin for this particular viewer type
 *
 * @return    rendering engine plugin instance
 */
vxRenderingNodePlugin * vxViewerTypeMPROrtho::CreateRenderingEnginePlugin()
{
  return new vxRenderingNodePluginPlanetracer( GetEnvironment() );
}//CreateRenderingEnginePlugin


// Revision History:
// $Log: vxViewerTypeMPROrtho.C,v $
// Revision 1.17.2.1  2009/09/08 21:24:44  dongqing
// Replace the Hecked code in SetupView
//
// Revision 1.17  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.16  2007/03/23 14:34:38  romy
// new DragDropEnum added
//
// Revision 1.15.2.1  2007/04/05 18:25:38  geconomos
// reset if camera field of view
//
// Revision 1.15  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.14  2007/01/16 16:48:58  gdavidson
// Changed vxRenderingNodePlugin
//
// Revision 1.13  2006/12/04 20:40:27  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.12  2006/08/03 14:07:50  geconomos
// added "rendering engine" support
//
// Revision 1.11  2006/02/09 14:08:07  geconomos
// 64-bit conditionals
//
// Revision 1.10  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8.2.1  2005/08/01 19:12:32  frank
// changed function name according to coding standards
//
// Revision 1.8  2005/03/16 18:04:53  frank
// fixed problem with IsVisible not properly rounding
//
// Revision 1.7  2004/10/05 14:36:11  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.6  2004/04/27 14:22:46  vxconfig
// fixed issue of GetLeft()/Right() plane
//
// Revision 1.5  2004/04/26 22:56:57  vxconfig
// removed redundant code
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
// Revision 1.62  2004/03/02 01:10:37  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.61  2004/02/20 23:33:43  michael
// added new zoom factor class
//
// Revision 1.60.2.1  2004/02/13 20:23:09  wenli
// put '_asm mov EAX, 100' before return bool  (a flaw of .Net Framework)
//
// Revision 1.60  2004/02/11 14:06:48  frank
// fixed restricted segmentation and slice computations
//
// Revision 1.59  2004/02/06 19:37:17  frank
// Issue #3726: Fixed projection logic
//
// Revision 1.58  2004/02/05 18:14:44  michael
// Moved InitializeFrom() down
//
// Revision 1.57  2004/02/05 03:13:36  michael
// initialization without assignment ...
//
// Revision 1.56  2004/02/03 12:32:57  michael
// fixed issue #3696
//
// Revision 1.55  2004/01/30 16:51:37  wenli
// Fixed: IsVisible returns true always.
//
// Revision 1.54  2004/01/23 04:00:05  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.53  2004/01/20 20:28:56  frank
// Reset the zoom factor in the environment when the zoom is reset during camera initialization
//
// Revision 1.52  2004/01/15 17:40:48  michael
// fixing issue #3608
//
// Revision 1.51  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.50  2004/01/05 19:01:18  michael
// added ellipse and box as annotations to the oblique MPRs
//
// Revision 1.49  2003/11/10 21:03:43  frank
// Fixed initialization from oblique view
//
// Revision 1.48  2003/11/07 19:55:16  frank
// Fixed zoom reset on mouse click. The modified queue handler was not checking the type of event, but only the argument, and was therefore erronously acting on 3D cursor events.
//
// Revision 1.47  2003/11/06 20:44:05  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.46  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.45  2003/10/21 17:09:47  frank
// Fixed setting the standard views so that it does not view all if you do not ask it to
//
// Revision 1.44  2003/09/29 17:18:44  michael
// accounting for the new event types when initializing the camera
//
// Revision 1.43  2003/09/29 14:36:11  michael
// added code to account for change of the vxEnvironment setting the plane flag
//
// Revision 1.42  2003/09/22 21:14:17  michael
// added new handle type to be drawable
//
// Revision 1.41  2003/09/11 14:06:25  frank
// Bounded edge of window to the real edge
//
// Revision 1.40  2003/08/20 14:17:25  michael
// added doxygen @param
//
// Revision 1.39  2003/06/24 19:26:34  frank
// Issue #3215: Fixed contour in 3x3 layout - plane distance now takes into account the offset distance
//
// Revision 1.38  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.37  2003/05/29 13:58:49  michael
// nneded to reverse simplifications in camera initialization since it breaks. In
// order to fix it, the pan/zoom manipulator also would need to take the changes
// into account -> postponed.
//
// Revision 1.36  2003/05/29 11:39:46  michael
// Simplified the camera initialization and the view setup, correctly handling the
// different aspect ratios without modifying the bounding box of the camera.
//
// Revision 1.35  2003/05/28 16:06:40  michael
// initialization of some variables in constructor, saves assignment
//
// Revision 1.34  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.33  2003/05/15 15:06:52  geconomos
// Code walkthru.
//
// Revision 1.32  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.31  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.30  2003/04/18 16:04:09  michael
// initial code review
//
// Revision 1.29  2003/04/16 14:42:02  michael
// removed redundant include
//
// Revision 1.28  2003/04/11 17:59:51  frank
// Refactored 2D/3D selection code so that the same algorithm is used regardless of 2D or 3D and the mode (x-ray, MIP, thin slab, normal)
//
// Revision 1.27  2003/04/08 16:59:01  michael
// removed vxWindowHandle stuff
//
// Revision 1.26  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.25  2003/03/26 22:54:29  michael
// fixed issue #3003
//
// Revision 1.24  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.23  2003/03/18 21:23:02  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.22  2003/03/18 20:42:33  michael
// added method to initialize the view/plane based on a given annotation
//
// Revision 1.21  2003/03/17 19:47:34  geconomos
// Issue #2914. Added DropType property.
//
// Revision 1.20  2003/03/04 15:34:17  geconomos
// Added methods for processing parent environment's modified queue.
//
// Revision 1.19  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.18  2003/02/28 20:33:53  michael
// completed one more step to move to vxViewerType
//
// Revision 1.17  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.16  2003/02/13 17:01:58  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.15  2003/02/07 00:29:58  michael
// added annotation for components
//
// Revision 1.14.2.1  2003/02/12 22:48:16  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.14  2003/01/22 21:49:23  ingmar
// the RayIterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.13  2003/01/22 15:07:02  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.12  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.11  2003/01/20 04:52:14  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.10  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.9  2003/01/15 16:29:51  michael
// made methods const that do not alter the state
//
// Revision 1.8  2003/01/14 23:35:34  michael
// adjusted code to determine whether a annotation is visible
//
// Revision 1.7  2003/01/13 23:19:35  michael
// added get/set for the environment (protected member in base class)
//
// Revision 1.6  2003/01/09 14:56:04  michael
// adjusted methods to take new annotation classes
//
// Revision 1.5  2002/12/20 16:13:49  frank
// Added x-ray projection
//
// Revision 1.4  2002/12/19 21:28:41  ingmar
// HitBoundingBox now takes a Box and not 2 vectors as box argument
//
// Revision 1.3  2002/12/18 18:22:02  michael
// moved OpenGL frame buffer reading to base class
//
// Revision 1.2  2002/12/18 14:52:38  geconomos
// implemented missing destructors
//
// Revision 1.1  2002/12/17 14:41:40  michael
// initial version of viewer type base class and the MPROrtho implementation
//
// $Id: vxViewerTypeMPROrtho.C,v 1.17.2.1 2009/09/08 21:24:44 dongqing Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeMPROrtho.C,v 1.17.2.1 2009/09/08 21:24:44 dongqing Exp $
