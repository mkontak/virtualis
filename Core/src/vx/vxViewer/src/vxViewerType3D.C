// $Id: vxViewerType3D.C,v 1.18 2007/06/28 19:33:55 jmeade Exp $
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
#include "vxViewerType3D.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxIntensityConverter.h"
#include "vxUtils.h"
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


// defines
#define FILE_REVISION "$Revision: 1.18 $"


/**
 * Default constructor
 * @param environment
 */
vxViewerType3D::vxViewerType3D(vxEnvironment & environment)
:vxViewerType(environment)
//*******************************************************************
{
}


/**
 * Destructor
 */
vxViewerType3D::~vxViewerType3D()
{
} // ~vxViewerType3D()


/**
 * Initialize the view
 */
void vxViewerType3D::InitializeView()
//*******************************************************************
{
  GetEnvironment().GetCamera()->SetFovDegrees( 45.0f );
}


/**
 * This method is responsible for initializing the OpenGL viewing
 */
void vxViewerType3D::SetupView()
//*******************************************************************
{
  // bound input to the window area
  vxWinRect winRect(GetEnvironment().GetWinRect());
  vxCamera & camera(*GetEnvironment().GetCamera());
  const Point<float4> eye(camera.GetEye());
  const Point<float4> ref(camera.GetViewReference());
  const Normal<float4> up(camera.GetViewUp());
  float4 fAspectScale((float4)winRect.GetViewportSize().second/(float4)winRect.GetViewportSize().first);
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

/*
  // set the viewport
  /////////////////////
  vxWinRect winRect(GetEnvironment().GetWinRect());
  glViewport(winRect.GetReferencePoint().first, 
              winRect.GetReferencePoint().second, 
              winRect.GetViewportSize().first, 
              winRect.GetViewportSize().second);

  float4 fAspectRatioWindow((float4) winRect.GetWindowSize().second / (float4) winRect.GetWindowSize().first);
  vxCamera & camera(*GetEnvironment().GetCamera());
  float4 fAspectScale(fAspectRatioWindow);

  if (fAspectRatioWindow < 1) 
  {
    fAspectScale = 1.0F / fAspectRatioWindow;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (camera.GetProjectionType() == vxCamera::PERSPECTIVE)
  {
    glFrustum(GetLeftPlane()                 , GetRightPlane(),
              GetBottomPlane() * fAspectScale, GetTopPlane() * fAspectScale,
              GetNearPlane()                 , GetFarPlane());
  }
  else
  {
    glOrtho(GetLeftPlane()                 , GetRightPlane(),
            GetBottomPlane() * fAspectScale, GetTopPlane() * fAspectScale,
            GetNearPlane()                 , GetFarPlane());
  }

  const Point<float4> eye(camera.GetEye());
  const Point<float4> ref(camera.GetViewReference());
  const Normal<float4> up(camera.GetViewUp());

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye.m_x, eye.m_y, eye.m_z, 
            ref.m_x, ref.m_y, ref.m_z, 
            up.X(),  up.Y(),  up.Z());

  return;
*/
} // SetupView()


/**
 * Convert the mouse coordinates into screen coordinates
 * @param mouseCoord  mouse coordinates
 * @param screenCoord screen coordinates
 * @return succesful
 */
Point2D<int2> vxViewerType3D::ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const
//*******************************************************************
{
  Point2D<int2> screenCoord(vxViewerType::ConvertMouseToScreen(mouseCoord));

  screenCoord.m_y = GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screenCoord.m_y;

  return screenCoord;
} // ConvertMouseToScreen()


/**
 * Convert screen to world using a ray rather than a 
 * @param screenCoord screen coordinate
 * @param worldCoord  world coordinate
 * @return succesful
 */
bool vxViewerType3D::ConvertScreenToWorld(const Point2D<int2> & screenCoord, 
                                          Point<float4> & worldCoord, 
                                          bool bConsiderRenderingMode) const
//*******************************************************************
{
  try
  {
    // bound input to the window area
    vxWinRect winRect(GetEnvironment().GetWinRect());

    RaySegment segment(Vector4Df(0, 0, 0, 0), Vector4Df(1, 0, 0, 1));
    bool bIntersects(vxUtils::GetIntersectionRayVolume(segment, GetEnvironment(), Point2D<float4>(screenCoord.m_x, screenCoord.m_y)));
    if (bIntersects == true)
    {
      const float4 fInterSampleDistance(0.5F);
      Vector4Df sampleIncVector(segment.GetDirection().GetNormalizedXYZ() * fInterSampleDistance);
      vxBlockVolumeRayIterator<uint2> dataIter (*GetEnvironment().GetIntensityVolume(), segment, sampleIncVector);
      vxBlockVolumeRayIterator<uint2> labelIter(*GetEnvironment().GetLabelVolume    (), segment, sampleIncVector);

      return ConvertRayToWorld(dataIter, labelIter, worldCoord);

    } // if intersects view volume
  } // try
  catch (...)
  {
    LogWrn("Exception in GetWorldCoordinate", "vxViewerType3D", "GetWorldCoordinate");
  }
  return false;
} // GetWorldFromScreen()


/**
 * Get world coordinates of the center of the vessel lumen given a screen location
 * @param mouseCoord mouse coordinates
 * @param worldPos 3D point of the center of the vessel lumen
 * @param rayOpacityProfile the profile of opacities along the ray
 * @return true if success
 */
bool vxViewerType3D::GetVesselLumenWorldFromMouse(const Point2D<int2> & mouseCoord, 
                                                  Point<float4> & worldCoord, 
                                                  std::vector<float4> & rayOpacityProfile) const
//*******************************************************************
{
  return GetVesselLumenWorldFromScreen(ConvertMouseToScreen(mouseCoord), worldCoord, rayOpacityProfile);
}


/**
 * Get world coordinates of the center of the vessel lumen given a screen location
 * @param screenPos 2D point of screen position
 * @param worldPos 3D point of the center of the vessel lumen
 * @param rayOpacityProfile the profile of opacities along the ray
 * @return true if success
 */
bool vxViewerType3D::GetVesselLumenWorldFromScreen( const Point2D<int2> & screenCoord, 
                                                    Point<float4> & worldCoord, 
                                                    std::vector<float4> & rayOpacityProfile ) const
{
  // bound input to the window area
  if (GetEnvironment().GetWinRect().IsInside(screenCoord) == false)
    return false;

  // see if the ray intersects the volume
  RaySegment segment(Vector4Df(0, 0, 0, 0), Vector4Df(0, 0, 1, 1));
  bool bResult(vxUtils::GetIntersectionRayVolume(segment, GetEnvironment(), Point2D<float4>(screenCoord.m_x, screenCoord.m_y)));
  if (bResult == true)
  {
    // check rendering mode
    const bool bMIPRendering(GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::MIP );
    const bool bXRay        (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::XRAY);
    if (bMIPRendering == true || bXRay == true)
    {
      // just use the regular method of choosing the voxel with max intensity along the ray
      return ConvertScreenToWorld(screenCoord, worldCoord, true);
    }
    else
    {
      // selects the middle of the first opacity region along the ray that exceeds 20% opacity
      enum 
      { 
        BeforeFirstRegion, 
        InFirstRegion, 
        AfterFirstRegion 
      } eRegion = BeforeFirstRegion;
      Vector4d<float4> startOfRegion(0, 0, 0, 0);
      Vector4d<float4> centerOfRegion(0, 0, 0, 0);
      Vector4d<float4> reachedOpaqueLocation(0, 0, 0, 0);
      const float4 fVesselThreshold( 0.2F );
      const float4 fAccumulationThreshold( 0.5F );

      // set up the rays
      const float4 fIdealInterSampleDistance( 0.5F );
      const float4 fActualInterSampleDistance( 0.05F );
      Vector4Df sampleIncVector(segment.GetDirection().GetNormalizedXYZ() * fActualInterSampleDistance);
      vxBlockVolumeRayIterator<uint2>  dataIter(* GetEnvironment().GetIntensityVolume(), segment, sampleIncVector);
      vxBlockVolumeRayIterator<uint2> labelIter(* GetEnvironment().GetLabelVolume    (), segment, sampleIncVector);

      // prepare the colormap
      float4 * pfRGBAUncorrected(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

      // the components in convenient sorted order
      const std::vector< vxComponent * > & vpComponentInfo(GetEnvironment().GetComponents()->ArrayByLabel());

		  int4 iMaxVoxelValue4Clamp(4094);
      std::string sModality("");
      sModality = GetEnvironment().GetIntensityVolume()->GetHeader().GetDicomFieldValue("0008", "0060", sModality);
      if( sModality != "CT" )
		  {
	      int4 iHeaderVersion(0);
	      GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar("version", iHeaderVersion);

	      if( iHeaderVersion >= 5 )
	      {
	        GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar("maxValueRange", iMaxVoxelValue4Clamp);
		      if( iMaxVoxelValue4Clamp > 4095 ) iMaxVoxelValue4Clamp = 65534;
	      }
	      else
	      {
          iMaxVoxelValue4Clamp = 65534;
	      }
		  } // if non CT modality

      // loop over voxels
      float4 fAccumulatedAlpha( 0.0F );
      for (; ! dataIter.IsAtEnd() ; ++dataIter, ++labelIter)
      {
        // sample the data
        int4 iSample(min(iMaxVoxelValue4Clamp, dataIter.GetSampleInt()));

        // check if the label is visible
        float4 fAlphaUncorrected(float(pfRGBAUncorrected[ 4 * iSample + 3 ]) / float(16 * 255));
        float4 fAlphaCorrected( ( 1 - pow( 1- fAlphaUncorrected, fIdealInterSampleDistance / fActualInterSampleDistance ) ) );
        if (vpComponentInfo[ labelIter.GetVoxel() ]->IsIntensityVisible() == false)
          fAlphaCorrected = 0;

        // accumulate opacity
        fAccumulatedAlpha += ( 1.0F - fAccumulatedAlpha ) * fAlphaCorrected;
        if ( fAccumulatedAlpha > fAccumulationThreshold )
        {
          reachedOpaqueLocation = dataIter.GetPosFloat();
        }

        // store the result in the profile
        rayOpacityProfile.push_back( fAlphaCorrected );

        // work on different regions
        switch (eRegion)
        {
        case BeforeFirstRegion:
          if (fAlphaCorrected >= fVesselThreshold)
          {
            startOfRegion = dataIter.GetPosFloat();
            eRegion = InFirstRegion;
          }
          break;
        case InFirstRegion:
          if (fAlphaCorrected < fVesselThreshold)
          {
            centerOfRegion = Vector4d<float4>((startOfRegion + dataIter.GetPosFloat()) / 2.0F);
            eRegion = AfterFirstRegion;
          }
          break;
        default:
          // do nothing
          break;
        }
      }

      // if we just exited the first region, find the center
      const Vector3D<float4> units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
      if (eRegion == AfterFirstRegion)
      {
        worldCoord = Point<float4>(centerOfRegion.GetScaledUnIso(Vector4d<float4>(units, 0)));
        return true;
      }

      // if we have entered but never left the solid region, choose the center
      else if (eRegion == InFirstRegion)
      {
        centerOfRegion = Vector4d<float4>((startOfRegion + dataIter.GetPosFloat()) / 2.0F);
        worldCoord = Point<float4>(centerOfRegion.GetScaledUnIso(Vector4d<float4>(units, 0)));
        return true;
      }

      // if we never reach the minimum opacity, see if we accumulated enough opacity and choose the
      // spot where we reached the threshold opacity.
      else if (eRegion == BeforeFirstRegion)
      {
        if ( fAccumulatedAlpha > fAccumulationThreshold )
        {
          worldCoord = Point<float4>(reachedOpaqueLocation.GetScaledUnIso(Vector4d<float4>(units, 0)));
          return true;
        }
      }

    } // if volume rendering
  } // if intersects volume

  // ray doesn't intersect volume
  return false;
} // GetVesselLumenWorldCoordinates()


/**
 * Is this annotation type supported by this viewer type?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerType3D::IsAnnotationSupported(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  if (dynamic_cast<const vxHandle *>(&handlesObject))
    return true;

  // at this point, annotations in 3D are not available but in some future version ...
  return false;
}


/**
 * Is the handlesSet visible and should be drawn?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerType3D::IsVisible(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  // at this point, handles object in 3D are not available but in some future version ...
  return true;
}


/**
 * Is this position visible in the current view?
 * @param position the query position
 * @return true if visible
 */
bool vxViewerType3D::IsVisible(const Point3Df & position) const
//*******************************************************************
{
  // points are currently not visible in 3D but in some future version ...
  return true;
} // IsVisible()


/**
 * Returns if the viewer supports dropping of the specified type
 * @param eSourceType drop type
 * @return true if element can be fropped, false otherwise
 */
bool vxViewerType3D::CanDragDrop(const DragDropEnum eSourceType)
//*******************************************************************
{
  return (eSourceType == vxViewerType::CAMERA   || 
           eSourceType == vxViewerType::COLORMAP ||
           eSourceType == vxViewerType::DATASET ||
           eSourceType == vxViewerType::PRESET );

} // CanDragDrop()


/**
* Get left plane
* @param tight fit or outer circle
* @return left plane
*/
float4 vxViewerType3D::GetLeftPlane() const
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
float4 vxViewerType3D::GetRightPlane() const
//*******************************************************************
{
  return -GetLeftPlane();
} // GetRightPlane()


/**
* Return the bottom plane
* @return Bottom plane
*/
float4 vxViewerType3D::GetTopPlane() const
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
float4 vxViewerType3D::GetBottomPlane() const
//*******************************************************************
{
  return -GetTopPlane();
} // GetTopPlane()


/**
* Get near plane
* @param tight fit or outer circle
* @return near plane
*/
float4 vxViewerType3D::GetNearPlane() const
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
* @param tight fit or outer circle
* @return far plane
*/
float4 vxViewerType3D::GetFarPlane() const
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
 * @return    rendering engine instance
 */
vxRenderingNodePlugin * vxViewerType3D::CreateRenderingEnginePlugin()
{
  return new vxRenderingNodePluginRaytracer( GetEnvironment() );
}


// Revision History:
// $Log: vxViewerType3D.C,v $
// Revision 1.18  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.17  2007/03/23 14:34:38  romy
// new DragDropEnum added
//
// Revision 1.16.2.1  2007/04/05 18:25:38  geconomos
// reset if camera field of view
//
// Revision 1.16  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.15  2007/01/16 16:48:58  gdavidson
// Changed vxRenderingNodePlugin
//
// Revision 1.14  2006/12/04 20:30:19  romy
// Eliminated the Rendering Engine's excess Threads
//
// Revision 1.13  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.12  2006/10/20 20:47:08  geconomos
// corrected SetupView to use viewport dimensions instead of window dimensions
//
// Revision 1.11  2006/08/03 14:07:50  geconomos
// added "rendering engine" support
//
// Revision 1.10  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8.2.2  2005/08/05 14:47:00  frank
// Issue #3749: Made it easier to select points in 3D - increased sampling rate and introduced alpha correction and accumulated opacity.
//
// Revision 1.8.2.1  2005/08/01 19:12:32  frank
// changed function name according to coding standards
//
// Revision 1.8  2005/05/20 12:13:18  frank
// cleaning up slice shadows in 3D
//
// Revision 1.7  2004/10/05 14:36:11  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.6  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.5  2004/08/05 13:53:51  michael
// reworked (OpenGL) setup of 3D
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
// Revision 1.28  2004/03/02 01:10:37  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.27.2.1  2004/02/17 15:10:07  frank
// Issue #3749: Fixed selection of objects on the edge of the volume
//
// Revision 1.27  2004/02/05 18:15:17  michael
// alignment
//
// Revision 1.26  2004/01/23 04:00:05  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.25  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.24  2003/11/21 16:17:09  michael
// cosmetics
//
// Revision 1.23  2003/11/06 20:44:05  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.22  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.21  2003/09/22 21:14:17  michael
// added new handle type to be drawable
//
// Revision 1.20  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.19  2003/06/09 16:10:16  michael
// took out redundant include
//
// Revision 1.18  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.17  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.16  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.15  2003/04/24 14:01:45  frank
// code review
//
// Revision 1.14  2003/04/24 13:48:18  frank
// Further code review
//
// Revision 1.13  2003/04/16 14:34:51  michael
// performed initial code review
//
// Revision 1.12  2003/04/11 17:59:51  frank
// Refactored 2D/3D selection code so that the same algorithm is used regardless of 2D or 3D and the mode (x-ray, MIP, thin slab, normal)
//
// Revision 1.11  2003/04/09 16:14:11  frank
// Fixed for invisible components
//
// Revision 1.10  2003/04/08 14:44:56  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.9  2003/04/07 13:20:05  frank
// Respected component visiblity during selection
//
// Revision 1.8  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.7  2003/03/30 21:03:23  michael
// formatting and cosmetics
//
// Revision 1.6  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.5  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.4  2003/03/17 19:47:34  geconomos
// Issue #2914. Added DropType property.
//
// Revision 1.3  2003/03/04 15:34:17  geconomos
// Added methods for processing parent environment's modified queue.
//
// Revision 1.2  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.1  2003/02/28 17:53:28  michael
// added initial version
//
// $Id: vxViewerType3D.C,v 1.18 2007/06/28 19:33:55 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerType3D.C,v 1.18 2007/06/28 19:33:55 jmeade Exp $
