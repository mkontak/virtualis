// $Id: vxUtils.C,v 1.35 2008/05/02 20:22:39 dongqing Exp $
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
#include "vxUtils.h"
#include "Point2D.h"
#include "Vector3D.h"
#include "Vector4d.h"
#include "GLEnv.h"
#include "vxBlockVolumeIterator.h"
#include "Timer.h"
#include "Box.h"
#include "vxComponentArray.h"
#include "vxEnvironment.h"
#include "vxCropBox.h"
#include "RaySegment.h"
#include "vxMathUtils.h"
#include "vxIntensityConverter.h"
#include "vxVolumeHeader.h"
#include "vxBlockVisibilityTester.h"
#include "vxInput.h"
#include "Quadrilateral.h"
#include "vxViewerType3D.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxIntensityStatistic.h"
#include "vxZoomFactor.h"
#include "vxVisualizationPreset.h"
#include "Line.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxMessageBox.h"
#include "vxUndoActionPreset.h"
#include "vxHistogramUpdaterIntensity.h"



// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.35 $"


/**
 * Compute the four corner points out of the two given ones
 * @param environment environment
 * @param handle0     point of first handle
 * @param handle1     point of diagonal handle
 * @param pCorners    pointer to four points
 */
void vxUtils::ComputeROIcorners(vxEnvironment & environment, 
                                Point3Df & handle0, 
                                Point3Df & handle1, 
                                Point3Df * pCorners)
{
  // assume that the camera (viewing coordinate system) defines
  // the top/bottom (right vector) left/right (up vector) edges
  // of the rectangle. If this chances, we need to adjust the
  // code here!!!
  ////////////////////////////////////////////////////////////////
  Normal3Df normalRight(environment.GetCamera()->GetViewRight()); 
  Vector3Df vectorRight(normalRight); 
  Normal3Df normalUp(environment.GetCamera()->GetViewUp()); 
  Vector3Df vectorUp(normalUp); 

  Point3Df vHandles[2];
  vHandles[0] = handle0;
  vHandles[1] = handle1;
  vxPlaneOblique<float4> planeHandle0(normalRight, vHandles[0]);
  float4 fDistanceHandle1(planeHandle0.GetDistance(vHandles[1]));
  vxPlaneOblique<float4> planeHandle1(normalRight, vHandles[1]);
  float4 fDistanceHandle0(planeHandle1.GetDistance(vHandles[0]));

  pCorners[0] = vHandles[0];
  pCorners[1] = vHandles[0] + vectorRight * fDistanceHandle1;
  pCorners[2] = vHandles[1];
  pCorners[3] = vHandles[1] + vectorRight * fDistanceHandle0;

  return;
} // ComputeROIcorners()


/**
 * Read values from the OpenGL framebuffer
 *
 * @param pfRGBA       pointer for color
 * @param pfXYZ        pointer for depth/XYZ coordinate
 * @param bDepthOnly   read depth values only?
 * @return             bool to choose between depth value and XYZ world coordinate to be returned
 *
 */
bool vxUtils::ReadFramebuffer(float4 * pfRGBA, float4 * pfXYZ, bool bDepthOnly)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble mvmatrix[16], projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  uint4 uSizeX(viewport[2]);
  uint4 uSizeY(viewport[3]);

  GLfloat *pRGBA  = new GLfloat[4*uSizeX*uSizeY];
  GLfloat *pDepth = new GLfloat[uSizeX*uSizeY];

  // throw out previous OpenGL errors
  while (glGetError() != GL_NO_ERROR)
    ;

  // read color values from framebuffer
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGBA, GL_FLOAT, pRGBA);
  if (glGetError() != GL_NO_ERROR)
    return false; //throw ex::AbortOperationException(LogRec("OpenGL error ocurred", "vxUtils", "ReadFramebuffer"));

  // read depth values from framebuffer
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, pDepth);
  if (glGetError() != GL_NO_ERROR)
    return false; //throw ex::AbortOperationException(LogRec("OpenGL error ocurred", "vxUtils", "ReadFramebuffer"));

  // pack the values
  ////////////////////
  GLdouble X, Y, Z, Depth;

  for (int4 j(0); j<uSizeY; j++)
  {
    for (int4 i(0); i<uSizeX; i++)
    {
      // copy color values
      *(pfRGBA++) = pRGBA[4*(j * uSizeX + i) + 0];
      *(pfRGBA++) = pRGBA[4*(j * uSizeX + i) + 1];
      *(pfRGBA++) = pRGBA[4*(j * uSizeX + i) + 2];
      *(pfRGBA++) = pRGBA[4*(j * uSizeX + i) + 3];
      Depth = (GLdouble)(pDepth[j * uSizeX + i]);

      // valid???
      if (gluUnProject(i, j, Depth, mvmatrix, projmatrix, viewport, &X, &Y, &Z) == GL_FALSE)
      {
        // Quick HACK, FIX ME, should be thought through
        delete[] pRGBA;
        delete[] pDepth;
        if (bDepthOnly == true)
          *(pfXYZ++) = -4711;
        else
        {
          *(pfXYZ++) = -4711;
          *(pfXYZ++) = -4711;
          *(pfXYZ++) = -4711;
        }
        return false;
      } // if gluUnProject succesful

      if (bDepthOnly == true)
        *(pfXYZ++) = Depth;
      else
      {
        *(pfXYZ++) = X;
        *(pfXYZ++) = Y;
        *(pfXYZ++) = Z;
      }
    } // for all pixels
  } // for all scanlines

  delete[] pRGBA;
  delete[] pDepth;

  return true;
} // ReadFramebuffer()


/**
 * read values from the OpenGL framebuffer
 * 
 * @param position     position to read values from
 * @param fRed         red by reference
 * @param fGreen       green by reference
 * @param fBlue        blue by reference
 * @param fAlpha       alpha by reference
 * @param pResult      pointer for depth/XYZ coordinate
 * @param bDepthOnly   read depth only?
 * @return             bool to choose between depth value and XYZ world coordinate to be returned
 */
bool vxUtils::ReadFramebuffer(Point2D<uint4> & position, 
                              float4 & fRed, float4 & fGreen, float4 & fBlue, float4 & fAlpha,
                              float4 * pResult, bool bDepthOnly)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble mvmatrix[16], projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  uint4 uSizeX = viewport[2];
  uint4 uSizeY = viewport[3];

  if (position.X() <  viewport[0] || position.X() >= viewport[0]+viewport[2] || 
      position.Y() <  viewport[1] || position.Y() >= viewport[1]+viewport[3])
  {
    //return false; //throw ex::AbortOperationException(LogRec("position is outside the set viewport", "vxUtils", "ReadFramebuffer"));    
  }
  GLfloat RGBA[4];
  GLfloat Depth(0);

  // throw out previous OpenGL errors
  while (glGetError() != GL_NO_ERROR)
    ;

  // read color values from framebuffer
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glReadPixels(position.X(), position.Y(), 1, 1, GL_RGBA, GL_FLOAT, RGBA);
  if (glGetError() != GL_NO_ERROR)
    return false; //throw ex::AbortOperationException(LogRec("OpenGL error ocurred", "vxUtils", "ReadFramebuffer"));

  // read depth values from framebuffer
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(position.X(), position.Y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);
  if (glGetError() != GL_NO_ERROR)
    return false; //throw ex::AbortOperationException(LogRec("OpenGL error ocurred", "vxUtils", "ReadFramebuffer"));

  // pack the values
  ////////////////////
  GLdouble X, Y, Z;
  // copy color values
  fRed   = RGBA[0];
  fGreen = RGBA[1];
  fBlue  = RGBA[2];
  fAlpha = RGBA[3];

  // valid???
  if (gluUnProject((GLdouble)position.X(), (GLdouble)position.Y(), Depth, mvmatrix, projmatrix, viewport, &X, &Y, &Z) == GL_FALSE)
  {
    return false; //throw ex::AbortOperationException(LogRec("Error in gluUnProject", "vxUtils", "ReadFramebuffer"));
  }
  // copy coordinate values
  if (bDepthOnly == true)
    *(pResult++) = Depth;
  else
  {
    *(pResult++) = X;
    *(pResult++) = Y;
    *(pResult++) = Z;
  }

  return true;
} // ReadFramebuffer()


/**
 * View the whole object
 * @param camera  camera
 * @param volume  intensity volume
 */
void vxUtils::CameraViewAll(vxCamera & camera,
                            const Box<float4> & roi)
{
  float4 fDist(camera.GetEye().GetEuclideanDistance(camera.GetViewReference()));  
  fDist -= roi.GetOuterRadius() / tan((camera.GetFovDegrees()/ 2.0F) * (M_PI/180.0F));

  Vector3D<float4> viewDirVector( camera.GetViewReference() - camera.GetEye() );
  Normal<float4> viewDir( 0, 0, 1 );
  if ( viewDirVector.GetLength() > 0 )
    viewDir = Normal<float4>( viewDirVector );
  Vector<float4> translate(viewDir.X()*fDist, viewDir.Y()*fDist, viewDir.Z()*fDist);

  camera.SetEye(camera.GetEye() + translate);
} // ViewAll()


/**
 * Set to one of the standard views
 *
 * @param camera     camera
 * @param eView      the standard viewpoint enumeration
 * @param volume     intensity volume
 * @param bViewAll   whether or not to view all.
 */
void vxUtils::SetStandardCameraView(vxCamera & camera, 
                                    const enum StandardCameraViewEnum eView,
                                    const Box<float4> & roi,
                                    bool bViewAll)
{
  if (roi.IsEmpty())
  {
    LogDbg("No valid volume loaded -> camera cannot be set", "vxUtils", "SetStandardCameraView");
    return;
  }

  Point3Df midPoint(roi.GetMidPoint());

  if (bViewAll == true)
  {
    Triplef size(roi.GetDim());

    switch (eView)
    {
    case ANTERIOR:
      camera.SetEye(Point<float4>(0,-size[1],0));
      camera.SetViewUp(Normal<float4>(0,0,1));
      break;

    case POSTERIOR:
      camera.SetEye(Point<float4>(0,size[1],0));
      camera.SetViewUp(Normal<float4>(0,0,1));
      break;

    case LEFT:
      camera.SetEye(Point<float4>(size[0],0,0));
      camera.SetViewUp(Normal<float4>(0,0,1));
      break;

    case RIGHT:
      camera.SetEye(Point<float4>(-size[0],0,0));
      camera.SetViewUp(Normal<float4>(0,0,1));
      break;

    case SUPERIOR:
      camera.SetEye(Point<float4>(0,0,size[2]));
      camera.SetViewUp(Normal<float4>(0,-1,0));
      break;

    case INFERIOR:
      camera.SetEye(Point<float4>(0,0,-size[2]));
      camera.SetViewUp(Normal<float4>(0,-1,0));
      break;

    default:
      throw ex::IllegalArgumentException(LogRec("This view does not exist", "vxUtils", "SetStandardCameraView"));
    }

    camera.TranslateEyePoint(Vector3Df(midPoint.X(), midPoint.Y(), midPoint.Z()));
    camera.SetViewReference(Point<float4>(midPoint));
    CameraViewAll(camera, roi);
  }
  else
  {
    // get the current distance from the center
    float4 fCurrentDistance(camera.GetEye().GetEuclideanDistance(midPoint));
    camera.SetViewReference(midPoint);

    switch (eView)
    {
    case ANTERIOR:
      camera.SetEye(midPoint + Vector3Df(0, -1, 0) * fCurrentDistance);
      camera.SetViewUp(Normal3Df(0, 0, 1));
      break;
    case POSTERIOR:
      camera.SetEye(midPoint + Vector3Df(0, 1, 0) * fCurrentDistance);
      camera.SetViewUp(Normal3Df(0, 0, 1));
      break;
    case LEFT:
      camera.SetEye(midPoint + Vector3Df(1, 0, 0) * fCurrentDistance);
      camera.SetViewUp(Normal3Df(0, 0, 1));
      break;
    case RIGHT:
      camera.SetEye(midPoint + Vector3Df(-1, 0, 0) * fCurrentDistance);
      camera.SetViewUp(Normal3Df(0, 0, 1));
      break;
    case SUPERIOR:
      camera.SetEye(midPoint + Vector3Df(0, 0, 1) * fCurrentDistance);
      camera.SetViewUp(Normal3Df(0, -1, 0));
      break;
    case INFERIOR:
      camera.SetEye(midPoint + Vector3Df(0, 0, -1) * fCurrentDistance);
      camera.SetViewUp(Normal3Df(0, -1, 0));
      break;
    default:
      throw ex::IllegalArgumentException(LogRec("This view does not exist: " + ToAscii(int4(eView)), "vxUtils", "SetStandardCameraView"));
      break;
    }
  }

  return;
} // SetStandardCameraView()


/**
 * Adjust camera to look at other bounding box proportionally.
 * Presets are generated based on a given volume but can be 
 * applied to any volume. This method serves as helper function
 * to adjust the camera for a differently sized volume to still
 * look at the object from a proportionally similar view.
 *
 * @param camera    the one to be adjusted
 * @param boxPreset bounding box the preset was created for
 * @param boxVolume bounding box of the current volume
 */
void vxUtils::AdjustCameraProportionally(vxCamera & camera,
                                         const Box<float4> & boxPreset, 
                                         const Box<float4> & boxVolume)
{
  Box<float4> boxSrc(boxPreset);
  if (boxSrc.IsEmpty())
  {
    boxSrc = Box<float4>(0,0,0, 1,1,1);
  }
  Point<float4> centerSrc(boxSrc.GetMidPoint());
  Point<float4> centerTarget(boxVolume.GetMidPoint());
  Triple<float4> relativeDistanceFromCenter;

  // 1.) get current eye position relative to center and size and set new eye correspondingly
  /////////////////////////////////////////////////////////////////////////////////////////////
  relativeDistanceFromCenter[0] = (camera.GetEye().X() - centerSrc[0])/boxSrc.GetLength()[0];
  relativeDistanceFromCenter[1] = (camera.GetEye().Y() - centerSrc[1])/boxSrc.GetLength()[1];
  relativeDistanceFromCenter[2] = (camera.GetEye().Z() - centerSrc[2])/boxSrc.GetLength()[2];

  Point3D<float4> eye;
  eye[0] = centerTarget[0] + relativeDistanceFromCenter[0] * boxVolume.GetLength().X();
  eye[1] = centerTarget[1] + relativeDistanceFromCenter[1] * boxVolume.GetLength().Y();
  eye[2] = centerTarget[2] + relativeDistanceFromCenter[2] * boxVolume.GetLength().Z();
  camera.SetEye(eye);

  // 2.) get current reference point relative to center and size and set new reference point
  ////////////////////////////////////////////////////////////////////////////////////////////
  relativeDistanceFromCenter[0] = (camera.GetViewReference().X() - centerSrc[0])/boxSrc.GetLength()[0];
  relativeDistanceFromCenter[1] = (camera.GetViewReference().Y() - centerSrc[1])/boxSrc.GetLength()[1];
  relativeDistanceFromCenter[2] = (camera.GetViewReference().Z() - centerSrc[2])/boxSrc.GetLength()[2];

  Point3D<float4> ref;
  ref[0] = centerTarget[0] + relativeDistanceFromCenter[0] * boxVolume.GetLength().X();
  ref[1] = centerTarget[1] + relativeDistanceFromCenter[1] * boxVolume.GetLength().Y();
  ref[2] = centerTarget[2] + relativeDistanceFromCenter[2] * boxVolume.GetLength().Z();
  camera.SetViewReference(ref);

  return;
} // AdjustCameraProportionally()


/// The following funcion should be got rid of if we could do the colormap and vxBlockVolume bias design better. Dongqing 20080502
/**
 * Adjust colormap to reflect data range on intensity volume
 * @param colormap colormap to be adjusted
 * @param volume   intensity volume that colormap is to be applied to
 */
void vxUtils::AdjustColormapValuesToDataRange(vxColormap & colormap, 
                                              vxBlockVolume<uint2> & volume)
{
  vxVolumeHeader header(volume.GetHeader());

  string sModality;
  header.GetHeaderEntry(vxVolumeHeader::DataModality, sModality);
  string sBias;
  header.GetHeaderEntry(vxVolumeHeader::Bias, sBias);
  string sBitStored;
  header.GetHeaderEntry(vxVolumeHeader::BitStored, sBitStored);
  //if (sBitStored != "FOO")

  if (sModality == "CT" && sBitStored == "12")
  {
    // need to correct the CT based window/level preset since it was generated for a Bias == 1000
    vxWindowLevel windowlevel = colormap.GetWindowLevel();

    int4 iBias(atoi(sBias.c_str()));
    int4 iDiff(iBias-1000);

    float4 fLevel(windowlevel.GetLevel());
    float4 fDiff(iDiff/(float4)vxIntensityConverter::GetMaxValue(header, false));

    windowlevel.SetLevel(fLevel + fDiff);
    colormap.SetWindowLevel(windowlevel);
  }

  return;
} // AdjustColormapValuesToDataRange()


/**
 * Adjust colormap from adaptive value to range on intensity volume back to standard CT range (bias = 1000)
 * @param colormap colormap to be adjusted
 * @param volume   intensity volume that colormap is to be applied to
 */
void vxUtils::AdjustColormapValuesFromAdaptiveToCTStandard(vxColormap & colormap, 
                                              vxBlockVolume<uint2> & volume)
{
  vxVolumeHeader header(volume.GetHeader());

  string sModality;
  header.GetHeaderEntry(vxVolumeHeader::DataModality, sModality);
  string sBias;
  header.GetHeaderEntry(vxVolumeHeader::Bias, sBias);
  string sBitStored;
  header.GetHeaderEntry(vxVolumeHeader::BitStored, sBitStored);
  //if (sBitStored != "FOO")

  if (sModality == "CT" && sBitStored == "12")
  {
    // need to correct the CT based window/level preset since it was generated for a Bias == 1000
    vxWindowLevel windowlevel = colormap.GetWindowLevel();

    int4 iBias(atoi(sBias.c_str()));
    int4 iDiff(iBias-1000);

    float4 fLevel(windowlevel.GetLevel());
    float4 fDiff(iDiff/(float4)vxIntensityConverter::GetMaxValue(header, false));

    windowlevel.SetLevel(fLevel - fDiff);
    colormap.SetWindowLevel(windowlevel);
  }

  return;
} // AdjustColormapValuesFromAdaptiveToCTStandard


/**
 * Calculate the offset distance for plane. This is used in case 
 * subsequent slices are shown using one plane description. The 
 * offset uDistance is different for each viewer.
 *
 * @param iMaxDistance distance of plane
 * @param uOffset      maximum of plane distance
 * @param offset       of plane
 * @param uMaxOffset   maximum offset of plane
 * @return             offset value
 */
uint4 vxUtils::CalculateOffsetDistance(uint4 uDistance, 
                                       int4 iMaxDistance, 
                                       uint4 uOffset, 
                                       uint4 uMaxOffset)
{
  // number of slices less then maximum offset
  if (iMaxDistance <= uMaxOffset)
  {
    return(Bound(uint4(0), uOffset, uint4(iMaxDistance - 1)));
  }

  uint4 uDeltaDist(iMaxDistance - 1 - uDistance);
  uint4 uShift((uMaxOffset > uDeltaDist)?(uMaxOffset - uDeltaDist): 0);

  return uDistance + uOffset - uShift;
} // CalculateOffsetDistance()



/**
 * Test if the ray hits the bounding box, if not it is not necessary to send a ray.
 *
 * @param boxBound          min and max values of bounding box
 * @param rayStart          ray origin
 * @param rayDirection      ray direction
 * @param rayIntersection   intersection vector of ray in case it hits the boundding box
 * @return                  true if ray intersects otherwise false
 */
bool vxUtils::HitBoundingBox(const Box<float4> & boxBound, 
                             const Point3D<float4> & rayStart, 
                             const Vector3D<float4> & rayDirection, 
                             Point3D<float4> & rayIntersection)
{
  static const int4 RIGHT(0);
  static const int4 LEFT(1);
  static const int4 MIDDLE(2);

  int4 i;
  int4 iWhichPlane;
  uint4 uQuadrant[3];
  float4 fMaxT[3];
  float4 fCandidatePlane[3]; 

  // Find candidate planes
  // assume perspective view: this loop can be avoided if rays cast all from the eye
  // assume parallel    view: this loop has to be done
  {
    if (rayStart.X() < boxBound.GetMinX())
    {
      uQuadrant[0] = LEFT;
      fCandidatePlane[0] = boxBound.GetMinX();
    }
    else if (rayStart.X() > boxBound.GetMaxX())
    {
      uQuadrant[0] = RIGHT;
      fCandidatePlane[0] = boxBound.GetMaxX();
    }
    else
    {
      uQuadrant[0] = MIDDLE;
    }

    if (rayStart.Y() < boxBound.GetMinY())
    {
      uQuadrant[1] = LEFT;
      fCandidatePlane[1] = boxBound.GetMinY();
    }
    else if (rayStart.Y() > boxBound.GetMaxY())
    {
      uQuadrant[1] = RIGHT;
      fCandidatePlane[1] = boxBound.GetMaxY();
    }
    else
    {
      uQuadrant[1] = MIDDLE;
    }

    if (rayStart.Z() < boxBound.GetMinZ())
    {
      uQuadrant[2] = LEFT;
      fCandidatePlane[2] = boxBound.GetMinZ();
    }
    else if (rayStart.Z() > boxBound.GetMaxZ())
    {
      uQuadrant[2] = RIGHT;
      fCandidatePlane[2] = boxBound.GetMaxZ();
    }
    else
    {
      uQuadrant[2] = MIDDLE;
    }
  }

  // is ray origin already inside Volume
  if (uQuadrant[0] == MIDDLE && uQuadrant[1] == MIDDLE && uQuadrant[2] == MIDDLE)
  {
    rayIntersection.X() = rayStart.X();
    rayIntersection.Y() = rayStart.Y();
    rayIntersection.Z() = rayStart.Z();
    return(true);
  }

  // calculate distances to planes
  //////////////////////////////////
  if (uQuadrant[0] != MIDDLE && rayDirection.X() !=0.0f)
  {
    fMaxT[0] = (fCandidatePlane[0]-rayStart.X()) / rayDirection.X();
  }
  else
  {
    fMaxT[0] = -1.;
  }

  if (uQuadrant[1] != MIDDLE && rayDirection.Y() !=0.0f)
  {
    fMaxT[1] = (fCandidatePlane[1]-rayStart.Y()) / rayDirection.Y();
  }
  else
  {
    fMaxT[1] = -1.;
  }

  if (uQuadrant[2] != MIDDLE && rayDirection.Z() !=0.0f)
  {
    fMaxT[2] = (fCandidatePlane[2]-rayStart.Z()) / rayDirection.Z();
  }
  else
  {
    fMaxT[2] = -1.;
  }

  // get largest of the fMaxT's for final choice of intersection
  ////////////////////////////////////////////////////////////////
  iWhichPlane = 0;
  for (i=1; i<3; i++)
  {
    if (fMaxT[iWhichPlane] < fMaxT[i])
    {
      iWhichPlane = i;
    }
  }  

  // are we casting away from the Volume (in all three dimensions)?
  if (fMaxT[iWhichPlane] < 0.0f)
  {
    return(false);
  }

  // compute intersection
  /////////////////////////
  if (iWhichPlane != 0)
  {
    rayIntersection.X() = rayStart.X() + fMaxT[iWhichPlane] *rayDirection.X();
    if (rayIntersection.X() < boxBound.GetMinX() || rayIntersection.X() > boxBound.GetMaxX())
    {
      return(false);
    }
  }
  else
  {
    rayIntersection.X() = fCandidatePlane[0];
  }

  if (iWhichPlane != 1)
  {
    rayIntersection.Y() = rayStart.Y() + fMaxT[iWhichPlane] *rayDirection.Y();
    if (rayIntersection.Y() < boxBound.GetMinY() || rayIntersection.Y() > boxBound.GetMaxY())
    {
      return(false);
    }
  }
  else
  {
    rayIntersection.Y() = fCandidatePlane[1];
  }

  if (iWhichPlane != 2)
  {
    rayIntersection.Z() = rayStart.Z() + fMaxT[iWhichPlane] *rayDirection.Z();
    if (rayIntersection.Z() < boxBound.GetMinZ() || rayIntersection.Z() > boxBound.GetMaxZ())
    {
      return(false);
    }
  }
  else
  {
    rayIntersection.Z() = fCandidatePlane[2];
  }

  // intersection exists?
  /////////////////////////
  if (rayIntersection.X() < 0.0 || rayIntersection.Y() < 0.0 || rayIntersection.Z() < 0.0)
  {
    return(false);
  }

  return(true); // ray hits box
} // HitBoundingBox()


/**
 * Test if the ray hits the bounding box, if not it is not necessary to send a ray
 *
 * @param boxBound          min and max values of bounding box
 * @param rayStart          ray origin
 * @param rayDirection      ray direction
 * @param rayIntersection   intersection vector of ray in case it hits the boundding box
 * @return                  true if ray intersects otherwise false
 */
bool vxUtils::HitBoundingBox4d(const Box<float4> & boxBound, 
                               const Vector4d<float4> rayStart, 
                               const Vector4d<float4> rayDirection, 
                               Vector4d<float4> & rayIntersection)
{
  static const int4 RIGHT(0);
  static const int4 LEFT(1);
  static const int4 MIDDLE(2);

  int4 i;
  int4 iWhichPlane;
  uint4 uQuadrant[3];
  float4 fMaxT[3];
  float4 fCandidatePlane[3]; 

  // Find candidate planes
  // assume perspective view: this loop can be avoided if rays cast all from the eye
  // assume parallel    view: this loop has to be done
  ////////////////////////////////////////////////////////////////////////////////////
  if (rayStart.X() < boxBound.GetMinX())
  {
    uQuadrant[0] = LEFT;
    fCandidatePlane[0] = boxBound.GetMinX();
  }
  else if (rayStart.X() > boxBound.GetMaxX())
  {
    uQuadrant[0] = RIGHT;
    fCandidatePlane[0] = boxBound.GetMaxX();
  }
  else
  {
    uQuadrant[0] = MIDDLE;
  }

  if (rayStart.Y() < boxBound.GetMinY())
  {
    uQuadrant[1] = LEFT;
    fCandidatePlane[1] = boxBound.GetMinY();
  }
  else if (rayStart.Y() > boxBound.GetMaxY())
  {
    uQuadrant[1] = RIGHT;
    fCandidatePlane[1] = boxBound.GetMaxY();
  }
  else
  {
    uQuadrant[1] = MIDDLE;
  }

  if (rayStart.Z() < boxBound.GetMinZ())
  {
    uQuadrant[2] = LEFT;
    fCandidatePlane[2] = boxBound.GetMinZ();
  }
  else if (rayStart.Z() > boxBound.GetMaxZ())
  {
    uQuadrant[2] = RIGHT;
    fCandidatePlane[2] = boxBound.GetMaxZ();
  }
  else
  {
    uQuadrant[2] = MIDDLE;     
  }

  // is ray origin already inside Volume
  if (uQuadrant[0] == MIDDLE && uQuadrant[1] == MIDDLE && uQuadrant[2] == MIDDLE)
  {
    rayIntersection.X() = rayStart.X();
    rayIntersection.Y() = rayStart.Y();
    rayIntersection.Z() = rayStart.Z();
    rayIntersection.T() = 0;
    return(true);
  }

  // calculate T distances to candidate planes
  //////////////////////////////////////////////
  if (uQuadrant[0] != MIDDLE && rayDirection.X() !=0.0f)
  {
    fMaxT[0] = (fCandidatePlane[0]-rayStart.X()) / rayDirection.X();
  }
  else
  {
    fMaxT[0] = -1;
  }

  if (uQuadrant[1] != MIDDLE && rayDirection.Y() !=0.0f)
  {
    fMaxT[1] = (fCandidatePlane[1]-rayStart.Y()) / rayDirection.Y();
  }
  else
  {
    fMaxT[1] = -1;
  }

  if (uQuadrant[2] != MIDDLE && rayDirection.Z() !=0.0f)
  {
    fMaxT[2] = (fCandidatePlane[2]-rayStart.Z()) / rayDirection.Z();
  }
  else
  {
    fMaxT[2] = -1;
  }

  // get largest of the fMaxT's for final choice of intersection
  ////////////////////////////////////////////////////////////////
  iWhichPlane = 0;
  for (i=1; i<3; i++)
  {
    if (fMaxT[iWhichPlane] < fMaxT[i])
    {
      iWhichPlane = i;
    }
  }  

  // are we casting away from the Volume (in all three dimensions)?
  if (fMaxT[iWhichPlane] < 0.0f)
  {
    return(false);
  }

  // compute the intersection
  /////////////////////////////
  if (iWhichPlane != 0)
  {
    rayIntersection.X() = rayStart.X() + fMaxT[iWhichPlane] *rayDirection.X();
    if (rayIntersection.X() < boxBound.GetMinX() || rayIntersection.X() > boxBound.GetMaxX())
    {
      return(false);
    }
  }
  else
  {
    rayIntersection.X() = fCandidatePlane[0];
  }

  if (iWhichPlane != 1)
  {
    rayIntersection.Y() = rayStart.Y() + fMaxT[iWhichPlane] *rayDirection.Y();
    if (rayIntersection.Y() < boxBound.GetMinY() || rayIntersection.Y() > boxBound.GetMaxY())
    {
      return(false);
    }
  }
  else
  {
    rayIntersection.Y() = fCandidatePlane[1];
  }

  if (iWhichPlane != 2)
  {
    rayIntersection.Z() = rayStart.Z() + fMaxT[iWhichPlane] *rayDirection.Z();
    if (rayIntersection.Z() < boxBound.GetMinZ() || rayIntersection.Z() > boxBound.GetMaxZ())
    {
      return(false);
    }
  }
  else
  {
    rayIntersection.Z() = fCandidatePlane[2];
  }

  // does intersection exist?
  /////////////////////////////
  if (rayIntersection.X() < 0.0 || rayIntersection.Y() < 0.0 || rayIntersection.Z() < 0.0)
  {
    return(false);
  }

  rayIntersection.T() = fMaxT[iWhichPlane];

  return true; // ray hits box
} // HitBoundingBox4d()


/**
 * Clamps point to viewport
 *
 * @param pt   unclamped point
 * @return     clamped point
 */
Point2D<int2> vxUtils::ClampToViewport(Point2D<int2> pt)
{
  // x, y, width, height
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  pt.m_x = Bound(viewport[0], (GLint) pt.X(), viewport[0] + viewport[2]);
  pt.m_y = Bound(viewport[1], (GLint) pt.Y(), viewport[1] + viewport[3]);

  return pt;
} // ClampToViewport()


/**
 * get the slab bounds in voxel coordinates
 *
 * @param environment the environment
 *
 * @return the slab bounds in voxel coordinates
 */
std::pair< uint4, uint4 > vxUtils::GetSlabPair(vxEnvironment & environment)
{
  // get some constants
  const Triplei & volDim(environment.GetIntensityVolume()->GetHeader().GetVolDim());
  const Triplef & units(environment.GetIntensityVolume()->GetHeader().GetVoxelUnits());
  const uint4 uAxisIndex(environment.GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

  // determine the slab bounds, taking into account the potential offset distance
  int4 iNumSlices(environment.GetRenderingMode()->GetSlabThickness() / units[ uAxisIndex ]);
  if (environment.GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::DEFAULT) iNumSlices = 1;
  uint4 uSliceIndex(CalculateOffsetDistance(environment.GetPlaneOrtho()->GetDistance() /
                                            units[ uAxisIndex ] + 0.5F,
                                            volDim[ uAxisIndex ], environment.GetOffset(),
                                            environment.GetMaximumOffset()));
  
  return GetSlabPair(volDim, units, iNumSlices, uAxisIndex, uSliceIndex);

} // GetSlabPair()


/**
 * Get the min and max slice index of a thin slab
 *
 * @param volDim           volume dimension
 * @param units            units of volume
 * @param iNumSlices       number of slices
 * @param uAxisIndex       index of axis (ortho slab)
 * @param iPlaneDistance   distance of plane to origin
 * @return                 pair of min and max value
 */
std::pair<uint4, uint4> vxUtils::GetSlabPair(const Triple<int4> & volDim, const Triple<float4> & units,
                                              const int4 iNumSlices, const uint1 uAxisIndex, const int4 iPlaneDistance)
{

  const int4 iMaxSlice(volDim[ uAxisIndex ] - 1);
  const int4 iAdjustedPlaneDistance(Bound(int4(0), iPlaneDistance, iMaxSlice));

  // special case for single slice - just
  int4 iFirstSlice(iAdjustedPlaneDistance);
  int4 iLastSlice (iAdjustedPlaneDistance);

  if (iNumSlices > 1)
  {

    // the total number of slices will be 1, 3, 5, 7, etc.
    int4 iAdjustedNumSlices(Bound(int4(0), iNumSlices, iMaxSlice));
    int4 iHalfThickness(iAdjustedNumSlices / 2);

    // compute the first and last slices
    iFirstSlice = Bound(int4(0), iAdjustedPlaneDistance - iHalfThickness, iMaxSlice);
    iLastSlice  = Bound(int4(0), iAdjustedPlaneDistance + iHalfThickness, iMaxSlice);

  } // if (iNumSlices > 1)

  return std::pair< uint4, uint4 >(iFirstSlice, iLastSlice);

} // GetSlabPair()


/**
 * Get the bounding box of the visible slab
 *
 * @param volDim           volume dimensions
 * @param units            volume units
 * @param iNumSlices       number of slices
 * @param uAxisIndex       axis index
 * @param iPlaneDistance   plane distance
 * @return                 bounding box
 */
Box<uint4> vxUtils::GetSlabBoundingBox(const Triple<int4  > & volDim, 
                                       const Triple<float4> & units, 
                                       const int4 iNumSlices, 
                                       const uint1 uAxisIndex, 
                                       const int4 iPlaneDistance)
{
  std::pair<uint4, uint4> slabPair(GetSlabPair(volDim, units, iNumSlices, uAxisIndex, iPlaneDistance));
  Box<uint4> boxBound;
  switch (uAxisIndex)
  {
  case vxUtils::SAGITTAL:
    boxBound = Box<uint4>(Point<uint4>(slabPair.first, 0, 0), Point<uint4>(slabPair.second, volDim.m_y-1, volDim.m_z-1));
    break;
  case vxUtils::CORONAL:
    boxBound = Box<uint4>(Point<uint4>(0, slabPair.first, 0), Point<uint4>(volDim.m_x-1, slabPair.second, volDim.m_z-1));
    break;
  case vxUtils::AXIAL:
    boxBound = Box<uint4>(Point<uint4>(0, 0, slabPair.first), Point<uint4>(volDim.m_x-1, volDim.m_y-1, slabPair.second));
    break;
  }
  return boxBound;
} // GetSlabBoundingBox()


#define NUMBER_OF_BINS 32


/**
 * GenerateBitCodeForInterval
 * @param iLower lower bound of interval
 * @param iUpper upper bound of interval
 * @param iMaxValue maximum (index) value, e.g. 4095 for 12 bit data
 * @return mask
 */
int4 vxUtils::GenerateBitCodeForInterval(const int4 iLower, 
                                         const int4 iUpper,
                                         const int4 iMaxValue)
{
  int4 iPowerOfTwo(GetNextPowerOfTwo(iMaxValue+1));

  // check for visible/contributing blocks based on Window/Level
  int4 iMaskIntensity(0);
  int4 iStart(floor(iLower/(float)(iPowerOfTwo/NUMBER_OF_BINS)));
  int4 iEnd(ceil(iUpper/(float)(iPowerOfTwo/NUMBER_OF_BINS)));

  for (int4 iBinIndex(iStart); iBinIndex<=iEnd; iBinIndex++)
  {
    iMaskIntensity |= 1 << iBinIndex;
  }

  return iMaskIntensity;
} // GenerateBitCodeForInterval()


/**
* GenerateBitCodeTransferFunction
* @param environment environment
* @return mask
*/
int4 vxUtils::GenerateBitCodeForTransferFunction(vxEnvironment & environment)
{
  const vxVolumeHeader & header(environment.GetIntensityVolume()->GetHeader());
  float4 * pfRGBA(environment.GetColormap()->GetTablePointerRGBA(header));
  int4 iMaxIndex(vxIntensityConverter::GetMaxValue(header));

  return GenerateBitCodeForTransferFunction(pfRGBA, iMaxIndex);
} // GenerateBitCodeForTransferFunction()


/**
 * GenerateBitCodeTransferFunction
 * @param pRGBA pointer to RGBA field, assuming 8 bit
 * @param iMax maximum index
 * @return mask
 */
int4 vxUtils::GenerateBitCodeForTransferFunction(const float4 * pRGBA, 
                                                 int4 iMaxIndex)
{
  int4 iMax(iMaxIndex);
  int4 iPowerOfTwo(GetNextPowerOfTwo(iMax+1));

  // check for visible/contributing blocks based on Window/Level
  int4 iMaskIntensity(0);
  const int4 iBinSize((int4)(iPowerOfTwo/(float)NUMBER_OF_BINS));

  for (int4 iBinIndex(0); iBinIndex<NUMBER_OF_BINS; ++iBinIndex)
  {
    const int4 iOffset(iBinIndex*iBinSize);
    int4 iValue(0);
    for (int4 iIndex(0); iIndex<iBinSize; ++iIndex)
    {
      if (pRGBA[4*(iOffset + iIndex) + 3] > 0)
      {
        iValue = 1;
        break;
      }
    }
    iMaskIntensity |= iValue << iBinIndex;
  }

  return iMaskIntensity;
} // GenerateBitCodeForTransferFunction()


/**
 * GenerateBitCodeTransferFunction
 *
 * @param pRGBA   pointer to RGBA field, assuming 8 bit
 * @return        mask
 */
int4 vxUtils::GenerateBitCodeFor8BitTransferFunction(const float4 * pRGBA)
{
  // check for visible/contributing blocks based on Window/Level
  int4 iMaskIntensity(0);
  const int4 iBinSize((int4)(256/(float)NUMBER_OF_BINS));

  for (int4 iBinIndex(0); iBinIndex<NUMBER_OF_BINS; ++iBinIndex)
  {
    const int4 iOffset(iBinIndex*iBinSize);
    int4 iValue(0);
    for (int4 iIndex(0); iIndex<iBinSize; ++iIndex)
    {
      if (pRGBA[4*(iOffset + iIndex) + 3] > 0)
      {
        iValue = 1;
      }
    }
    iMaskIntensity |= iValue << iBinIndex;
  }

  return iMaskIntensity;
} // GenerateBitCodeFor8BitTransferFunction()


/**
 * GenerateBitCodeTransferFunction
 *
 * @param pRGBA   pointer to RGBA field, assuming 12 bit
 * @return        mask
 */
int4 vxUtils::GenerateBitCodeFor12BitTransferFunction(const float4 * pRGBA)
{
  // check for visible/contributing blocks based on Window/Level
  int4 iMaskIntensity(0);
  const int4 iBinSize((int4)(4096/(float)NUMBER_OF_BINS));

  for (int4 iBinIndex(0); iBinIndex<NUMBER_OF_BINS; ++iBinIndex)
  {
    const int4 iOffset(iBinIndex*iBinSize);
    int4 iValue(0); 
    for (int4 iIndex(0); iIndex<iBinSize; ++iIndex)
    {
      if (pRGBA[4*(iOffset + iIndex) + 3] > 0)
      {
        iValue = 1;
      }
    }
    iMaskIntensity |= iValue << iBinIndex;
  }

  return iMaskIntensity;
} // GenerateBitCodeFor12BitTransferFunction()


/**
 * GenerateBitCodeTransferFunction
 *
 * @param vpComponents   pointer to components
 * @return               mask
 */
int4 vxUtils::GenerateBitCodeComponentsVisible(vxComponentArray & vpComponents)
{
  // check for visible/contributing blocks based on first 31 components
  int4 iMaskLabel(0);

  for (int4 k(0); k<vpComponents.GetCount(); ++k)
  {
    const uint2 uLabel(vpComponents[k]->GetLabel());
    if (vpComponents[k]->IsIntensityVisible())
    {
      iMaskLabel |= (1 << min(uLabel, 31));
    }
  }

  return iMaskLabel;
} // GenerateBitCodeComponentsVisible()


/**
 * GenerateBitCodeTransferFunction
 *
 * @param components   components
 * @return               mask
 */
int4 vxUtils::GenerateBitCodeComponentsVisible( const std::vector< vxComponent > & components )
{
  // check for visible/contributing blocks based on first 31 components
  int4 iMaskLabel = 0;

  for (int4 k(0); k<components.size(); ++k)
  {
    if (components[k].IsIntensityVisible())
    {
      iMaskLabel |= (1 << min(k, 31));
    }
  }
  return iMaskLabel;
} // GenerateBitCodeComponentsVisible()


/**
 * GenerateBitCodeTransferFunction
 *
 * @param vpComponents   pointer to components
 * @return               mask
 */
int4 vxUtils::GenerateBitCodeLabelColorVisible(vxComponentArray & vpComponents)
{
  // check for visible/contributing blocks based on first 31 components
  int4 iMaskLabelColor(0);

  for (int4 k(0); k<vpComponents.GetCount(); ++k)
  {
    uint2 uLabel = vpComponents[k]->GetLabel();
    if (vpComponents[k]->IsLabelVisible())
    {
      iMaskLabelColor |= (1 << min(uLabel, 31));
    }
  }

  return iMaskLabelColor;
} // GenerateBitCodeLabelColorVisible()


/**
 * GenerateBitCodeTransferFunction
 *
 * @param components   components
 * @return               mask
 */
int4 vxUtils::GenerateBitCodeLabelColorVisible(const std::vector< vxComponent > & components)
{
  // check for visible/contributing blocks based on first 31 components
  int4 iMaskLabelColor(0);

  for (int4 k(0); k<components.size(); ++k)
  {
    if (components[k].IsLabelVisible())
    {
      iMaskLabelColor |= (1 << min(k, 31));
    }
  }

  return iMaskLabelColor;
} // GenerateBitCodeLabelColorVisible()

/**
 * Write separate volume from components
 *
 * @param volIntensity intensity volume 
 * @param volLabel     label volume
 * @param iNumLabels   number of labels
 */
void vxUtils::WriteComponentsAsSeparateVolumes(vxBlockVolume<uint2> volIntensity, 
                                               vxBlockVolume<uint2> volLabel, 
                                               int4 iNumLabels)
{

  for (int4 iLabel(0);  iLabel<iNumLabels;  ++iLabel)
  {
    vxBlockVolume<uint2> vol(volIntensity.GetHeader());
    vxBlockVolumeIterator<uint2> iterLabel(volLabel);
    vxBlockVolumeIterator<uint2> iterVolSrc(volIntensity);
    vxBlockVolumeIterator<uint2> iterVolDest(vol);

    for (; iterVolDest.IsNotAtEnd(); iterVolDest.NextBlockZYX())
    {
      for (; iterVolDest.IsNotAtEndOfBlock(); iterVolDest.NextXZYinsideBlock())
      {
        if (iterLabel.GetVoxel(iterVolDest) == iLabel)
        {
          iterVolDest.SetVoxel(iterVolSrc.GetVoxel(iterVolDest));
        }
      }
    }
    char sFilename[50];
    sprintf(sFilename, "D:/ds/label_%02i.vol", iLabel);
    vol.Write(sFilename);
  } // for all labels

} // WriteComponentsAsSeparateVolumes()

 
/**
 * Get visible bounding box. Determines the bonding box of what is 
 * visible, taking into account colormap, label selection, etc.
 *
 * @param environment   environment
 * @return              visible bounding box
 */
Box<float4> vxUtils::GetVisibleBoundingBox(vxEnvironment & environment)
{
  return GetVisibleBoundingBox( environment.GetIntensityVolume(), environment.GetLabelVolume(),
                                environment.GetRenderingMode(), environment.GetCropBox(),
                                environment.GetColormap(), environment.GetComponents(),
                                environment.GetInteractiveSegmentation() );
} // GetVisibleBoundingBox()


/**
 * Get visible bounding box. Determines the bonding box of what is 
 * visible, taking into account colormap, label selection, etc.
 *
 * @param pIntensityVolume the intensity volume
 * @param pLabelVolume     the label volume
 * @param pRenderingMode   the rendering mode
 * @param pCropBox         the crop box
 * @return                 visible bounding box
 */
Box<float4> vxUtils::GetVisibleBoundingBox( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                                            vxRenderingMode * pRenderingMode, vxCropBox * pCropBox, vxColormap * pColormap,
                                            vxComponentArray * pComponentArray, InteractiveSegmentation * pInteractiveSegmentation )
{
  // first determine the current data boundiung box at block16 granularity
  const Vector3D<int4> iVolDim( pIntensityVolume->GetHeader().GetVolDim() );
  Box<int4> iBoundingBoxData(iVolDim.X(), iVolDim.Y(), iVolDim.Z(), 0, 0, 0);
  
  vxBlockVisibilityTester blockVisibilityTester( pIntensityVolume, pLabelVolume, pRenderingMode, pColormap, pComponentArray, pInteractiveSegmentation );
  
  vxBlockVolumeIterator<uint2> iterVol( * pIntensityVolume );
  vxBlockVolumeIterator<uint2> iterLabel( * pLabelVolume );

  for (; iterVol.IsNotAtEnd(); iterVol.NextBlockZYX(), iterLabel.NextBlockZYX())
  {
    if ( blockVisibilityTester.IsBlockVisibleRaw( iterVol.GetCurrentBlock().GetBinaryHistogram(), iterLabel.GetCurrentBlock().GetBinaryHistogram()))
    {
      iBoundingBoxData.Union( iterVol.GetPos() );
    }
  }
  // ensure that the min coordinates are <= compared to the max coordinates
  iBoundingBoxData.Normalize();

  // include the remainder of the max block
  iBoundingBoxData.Union(Vector3D<int4>(iBoundingBoxData.GetMaxPoint()) + Vector3D<int4>(15,15,15));

  // make sure we did not expand past the volume boundaries
  Box<int4> completeVolumeBox(0,0,0, iVolDim.X()-1,iVolDim.Y()-1,iVolDim.Z()-1); 
  iBoundingBoxData.BoundWithBox(completeVolumeBox);

  // shrink further according to environment crop box
  if ( pRenderingMode->IsEnabled( vxRenderingMode::SHOW_CROP_BOX ) )
  {
    Box<float4> cropBoxWC( pCropBox->GetBox() );

    // Get the crop box in volume coordinates.
    const Triple<float4> & units( pIntensityVolume->GetHeader().GetVoxelUnits() );
    cropBoxWC.ScaleAnIso( Triplef( 1 / units.X(), 1 / units.Y(), 1 / units.Z() ) );
    Box<float4> cropBoxVC( cropBoxWC );

    // reduce it to the visible data area
    iBoundingBoxData.BoundWithBox( cropBoxVC );
  }

  return iBoundingBoxData;
} // GetVisibleBoundingBox()


/**
 * Get the active bounds of the volume.
 * It does not take into account the visibility of individual voxels.
 * It takes into account the viewer type, the rendering mode (e.g., thin
 * slab, MIP, single slice), and the crop box.
 *
 * @param bResrictToVisible restricts the active volume to only the visible bounds
 *
 * @return the active bounds of the volume (inclusive)
 */
Box<uint4> vxUtils::GetActiveBoundingBox( const bool bRestrictToVisible, vxBlockVolume<uint2> * pIntensityVolume,
                                          vxViewerType * pViewerType, vxPlaneOrtho<float4> * pPlaneOrtho,
                                          vxRenderingMode * pRenderingMode, const uint4 uOffset,
                                          const uint4 uMaximumOffset, const vxCropBox * pCropBox )
{

  // initialize to the entire volume
  const Triple<int4> & iVolDim( pIntensityVolume->GetHeader().GetVolDim() );
  Box<uint4> visibleBounds( Point<uint4>(0,0,0), Point<uint4>( iVolDim.m_x-1, iVolDim.m_y-1, iVolDim.m_z-1 ) );

  // get the current slab thickness to limit sculpting to the visible voxels, if 2D view
  const Triple<float4> & units( pIntensityVolume->GetHeader().GetVoxelUnits() );
  const bool bDerivedFromOblique( dynamic_cast< vxViewerTypeMPROblique * >( pViewerType ) != NULL );
  const bool bIsThreeDimensionalView( dynamic_cast< vxViewerType3D * >( pViewerType ) != NULL );

  if ( bRestrictToVisible &&
       ! bDerivedFromOblique &&
       ! bIsThreeDimensionalView )
  {
    const uint1 uAxisIndex( pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord() );
    const float4 fPlaneDistance( pPlaneOrtho->GetDistance() );
    const int4 iPlaneDistance( vxUtils::CalculateOffsetDistance(0.5F + fPlaneDistance / units[ uAxisIndex ],
                               iVolDim[ uAxisIndex ], uOffset, uMaximumOffset ) );
    int4 iNumSlices( 0.5F + pRenderingMode->GetSlabThickness() / units[ uAxisIndex ] );
    if ( pRenderingMode->GetRenderingAlgorithm() ==  vxRenderingMode::DEFAULT )
    {
      // single slice mode
      iNumSlices = 1;
    }
    visibleBounds = Box<uint4>( GetSlabBoundingBox( iVolDim, units, iNumSlices, uAxisIndex, iPlaneDistance ) );

  } // if (bRestrictToVisible && ! bDerivedFromOblique && ! bIsThreeDimensionalView)

  // restrict to the bounding box, if enabled
  if ( bRestrictToVisible && pRenderingMode->IsEnabled(vxRenderingMode::SHOW_CROP_BOX ) )
  {
    const Box<float4> & validBoxWorld = pCropBox->GetBox();
    
    // scale down to get from world coordinates to volume coordinates
    const Triple<float4> & volUnits( pIntensityVolume->GetHeader().GetVoxelUnits() );
    Box<float4> validBoxVolume( validBoxWorld.GetMinPoint().GetDividedAnIso( volUnits ),
                                validBoxWorld.GetMaxPoint().GetDividedAnIso( volUnits ) );

    visibleBounds.SetMinX( Max( static_cast< uint4 >( floor( validBoxVolume.GetMinX() ) ), visibleBounds.GetMinX() ) );
    visibleBounds.SetMaxX( Min( static_cast< uint4 >( ceil ( validBoxVolume.GetMaxX() ) ), visibleBounds.GetMaxX() ) );
    visibleBounds.SetMinY( Max( static_cast< uint4 >( floor( validBoxVolume.GetMinY() ) ), visibleBounds.GetMinY() ) );
    visibleBounds.SetMaxY( Min( static_cast< uint4 >( ceil ( validBoxVolume.GetMaxY() ) ), visibleBounds.GetMaxY() ) );
    visibleBounds.SetMinZ( Max( static_cast< uint4 >( floor( validBoxVolume.GetMinZ() ) ), visibleBounds.GetMinZ() ) );
    visibleBounds.SetMaxZ( Min( static_cast< uint4 >( ceil ( validBoxVolume.GetMaxZ() ) ), visibleBounds.GetMaxZ() ) );
  }

  return visibleBounds;

} // GetActiveBoundingBox()


/**
 * Get the four corners of the oblique plane from the camera and the plane oblique through projection.
 *
 * @param worldPoints  computed intersection points in world coordinates
// * @param fNear        distance of near plane
// * @param fFar         distance of far plane
 * @param planeOblique plane with which the frustum is intersected
 *
 * @return             success
 */
bool vxUtils::ComputeObliqueQuadrilateralOpenGL(Point3Df worldPoints[4],
//                                                float4 fNear, 
//                                                float4 fFar, 
                                                const vxPlaneOblique<float4> & planeOblique)
{
  // (1) read values from OpenGL for the four corner points
  ///////////////////////////////////////////////////////////
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble mvmatrix[16], projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  uint4 uSizeX(viewport[2]);
  uint4 uSizeY(viewport[3]);

  GLfloat Depth(0);
  GLdouble X, Y, Z;

  // throw out previous OpenGL errors
  while (glGetError() != GL_NO_ERROR) ;

  for (int4 i(0); i<4; ++i)
  {
    int4 x(viewport[0]); if (i == 2 || i == 3) x += uSizeX-1; // add width  if on right
    int4 y(viewport[1]); if (i == 1 || i == 2) y += uSizeY-1; // add height if on top

    // read depth values from framebuffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);
    if (glGetError() != GL_NO_ERROR)
      throw ex::AbortOperationException(LogRec("OpenGL error ocurred (1)", "vxUtils", "ComputeObliqueQuadrilateralOpenGL"));

    // get two projection points
    if (gluUnProject((GLdouble)x, (GLdouble)y, 0.0F, mvmatrix, projmatrix, viewport, &X, &Y, &Z) == GL_FALSE)
      throw ex::AbortOperationException(LogRec("OpenGL error ocurred (2)", "vxUtils", "ComputeObliqueQuadrilateralOpenGL"));

    // copy coordinate values
    Point<float4> cornerPoint(X, Y, Z);

    if (gluUnProject((GLdouble)x, (GLdouble)y, 1.0F, mvmatrix, projmatrix, viewport, &X, &Y, &Z) == GL_FALSE)
      throw ex::AbortOperationException(LogRec("OpenGL error ocurred (3)", "vxUtils", "ComputeObliqueQuadrilateralOpenGL"));

    // copy coordinate values
    Normal<float4> cornerNormal(cornerPoint[0] - X, cornerPoint[1] - Y, cornerPoint[2] - Z);

    // find intersection point with plane oblique
    ///////////////////////////////////////////////
    Point<float4> point;
    if (planeOblique.GetIntersection(cornerPoint, cornerNormal, point) == false)
      return false;

    worldPoints[i] = point;
  }

  return true;
} // ComputeObliqueQuadrilateral()


/**
 * Compute the corners of the oblique quadrilateral using a pure analytical approach.
 * For use when you do not want to rely on OpenGL numerical approximation or
 * the OpenGL state is not set up for viewing.
 *
 * @param pCamera       the camera
 * @param pPlaneOblique the oblique plane
 *
 * @return the quadrilateral formed by the intersection of the camera frustum and the plane
 */
Quadrilateral vxUtils::ComputeObliqueQuadrilateralAnalytical(const vxCamera * pCamera, 
                                                             const vxPlaneOblique<float4> * pPlaneOblique, const vxViewerType * pViewerType )
{
  // prepare a default quadrilateral in case of error
  Quadrilateral emptyQuad(Point<float4>(0,0,0), Normal<float4>(1,0,0), Normal<float4>(0,1,0), 1.0F);

  // get the view direction without mistakenly normalizing a zero length vector
  Vector3Df viewDirection(pCamera->GetViewReference() - pCamera->GetEye());
  if (viewDirection.GetLength() > 0.0F)
    viewDirection.Normalize();
  else
    viewDirection = Vector3Df(1, 0, 0);
  Normal3Df viewDirNormal(viewDirection);

  if( pCamera->GetProjectionType() == vxCamera::PERSPECTIVE )
  {
    // get the field of view angle
    const float4 fAngle(pCamera->GetFovDegrees() / 2 * M_PI / 180);

    // rotate the view direction to the left
    Matrix<float4> mLeft;
    mLeft.Rotate(pCamera->GetViewUp(), -fAngle);
    Normal3Df leftNormal(mLeft * viewDirNormal);

    // rotate the view direction and to the right
    Matrix<float4> mRight;
    mRight.Rotate(pCamera->GetViewUp(), fAngle);
    Normal3Df rightNormal(mRight * viewDirNormal);

    // now rotate the left and right vectors up to form the top two corners
    Matrix<float4> mUp;
    mUp.Rotate(pCamera->GetViewRight(), fAngle);
    Normal3Df upLeftNormal(mUp * leftNormal);
    Normal3Df upRightNormal(mUp * rightNormal);

    // now rotate the left and right vectors down to form the bottom two corners
    Matrix<float4> mDown;
    mDown.Rotate(pCamera->GetViewRight(), -fAngle);
    Normal3Df downLeftNormal(mDown * leftNormal);
    Normal3Df downRightNormal(mDown * rightNormal);

    // compute the four intersections
    Point3Df upLeftPoint, upRightPoint, downLeftPoint, downRightPoint;
    if (pPlaneOblique->GetIntersection(pCamera->GetEye(),    upLeftNormal,    upLeftPoint) == false) return emptyQuad;
    if (pPlaneOblique->GetIntersection(pCamera->GetEye(),   upRightNormal,   upRightPoint) == false) return emptyQuad;
    if (pPlaneOblique->GetIntersection(pCamera->GetEye(),  downLeftNormal,  downLeftPoint) == false) return emptyQuad;
    if (pPlaneOblique->GetIntersection(pCamera->GetEye(), downRightNormal, downRightPoint) == false) return emptyQuad;

    return Quadrilateral(upLeftPoint, upRightPoint, downLeftPoint, downRightPoint);
  }
  else if( pCamera->GetProjectionType() == vxCamera::ORTHOGRAPHIC )
  {
    // define the starting point of the rays (that correspond to the four corners of the image)
    Point3Df topLeftStartPoint  = Point3Df( pCamera->GetEye() - pCamera->GetViewRight() * pViewerType->GetLeftPlane() + pCamera->GetViewUp() * pViewerType->GetTopPlane() );
    Point3Df topRightStartPoint = Point3Df( pCamera->GetEye() + pCamera->GetViewRight() * pViewerType->GetLeftPlane() + pCamera->GetViewUp() * pViewerType->GetTopPlane() );
    Point3Df bottomLeftStartPoint  = Point3Df( pCamera->GetEye() - pCamera->GetViewRight() * pViewerType->GetLeftPlane() - pCamera->GetViewUp() * pViewerType->GetTopPlane() );
    Point3Df bottomRightStartPoint = Point3Df( pCamera->GetEye() + pCamera->GetViewRight() * pViewerType->GetLeftPlane() - pCamera->GetViewUp() * pViewerType->GetTopPlane() );

    // compute the four intersections
    Point3Df topLeftPoint, topRightPoint, bottomLeftPoint, bottomRightPoint;
    if ( pPlaneOblique->GetIntersection(     topLeftStartPoint, viewDirNormal,     topLeftPoint ) == false ) return emptyQuad;
    if ( pPlaneOblique->GetIntersection(    topRightStartPoint, viewDirNormal,    topRightPoint ) == false ) return emptyQuad;
    if ( pPlaneOblique->GetIntersection(  bottomLeftStartPoint, viewDirNormal,  bottomLeftPoint ) == false ) return emptyQuad;
    if ( pPlaneOblique->GetIntersection( bottomRightStartPoint, viewDirNormal, bottomRightPoint ) == false ) return emptyQuad;

    return Quadrilateral( topLeftPoint, topRightPoint, bottomLeftPoint, bottomRightPoint );
  }
  else
    throw ex::UnsupportedDataTypeException( LogRec( "Invalid projection type", "vxUtils", "ComputeObliqueQuadrilateralAnalytical" ));

  return emptyQuad;

} // ComputeObliqueQuadrilateral()


/**
 * Compute the statistics for an generalized ellipse on a plane
 *
 * @param outStatistics       the output statistics
 * @param firstCornerPosition one of the corners
 * @param verticalEdge        the first direction away from the first corner
 * @param horizontalEdge      the second direction away from the first corner
 * @param intensityVolume     the intensity volume
 */
void vxUtils::ComputeEllipseStatistics(vxIntensityStatistic & outStatistics, const Point3Df & firstCornerPosition,
                                        const Vector3Df & verticalEdge, const Vector3Df & horizontalEdge,
                                        vxBlockVolume<uint2> & intensityVolume)
{

  // make mutable copies
  Vector3Df verticalDiameter(verticalEdge);
  Vector3Df horizontalDiameter(horizontalEdge);

  Triple<float4> units(intensityVolume.GetHeader().GetVoxelUnits());
  float4 fMinUnit = Min(units[0], Min(units[1], units[2]));
  float4 fLengthHorizontal(2.0F*horizontalDiameter.GetLength()/fMinUnit);
  float4 fLengthVertical(2.0F*verticalDiameter.GetLength()/fMinUnit);
  int4 iSizeHorizontal(1 + fLengthHorizontal);
  int4 iSizeVertical(1 + fLengthVertical);
  horizontalDiameter.Normalize();
  horizontalDiameter *= 0.5F * fMinUnit;
  verticalDiameter.Normalize();
  verticalDiameter *= 0.5F * fMinUnit;

  vxBlockVolumeIterator<uint2> iter(intensityVolume);
  Point<float4> point;
  Vector3D<float4> vector;

  // go through the rectangle and compute max and average voxel values
  //////////////////////////////////////////////////////////////////////
  for (int j=0; j<iSizeVertical; j++)
  {
    float4 fJ(0.0F);
    if (iSizeVertical > 1)
      fJ = 2*j/(float)(iSizeVertical-1.0F) - 1.0F;

    point  = firstCornerPosition - (float4)j * verticalDiameter;
    vector = (Vector3D<float4>)point;

    Vector4Df rayStart(point[0]/units[0], point[1]/units[1], point[2]/units[2], 0);
    Vector4Df rayEnd(rayStart + Vector4Df((float4)iSizeHorizontal * Vector3Df(horizontalDiameter[0]/units[0], 
      horizontalDiameter[1]/units[1], 
      horizontalDiameter[2]/units[2]), 1));
    RaySegment raySegment(rayStart, rayEnd);
    float4 fNumSteps(1);
    if (iSizeHorizontal > 1)
      fNumSteps = iSizeHorizontal-1.0F;

    Vector4Df rayDir(raySegment.GetDirection());
    Vector4Df rayDirection(raySegment.GetDirection()/fNumSteps);

    vxBlockVolumeRayIterator<uint2> iterRay(intensityVolume, raySegment, rayDirection);

    for (int i(0); !(iterRay.IsAtEnd()) ; ++iterRay, ++i)
    { 
      float4 fI(0.0F);
      if (iSizeHorizontal > 1)
      {
        fI = 2*i/(float)(iSizeHorizontal-1.0F) - 1.0F;
      }

      if (Vector3Df(fI, fJ, 0).GetLength() < 1.0F)
      {
        if (iterRay.IsInsideVolume())
        {
          const int2 iVoxel(iterRay.GetVoxel());
          outStatistics.Add(iVoxel);
        }
      }
    } // for all positions in scanline
  } // for all scanlines

  // compute statistics
  outStatistics.Compute();

} // ComputeEllipseStatistics()


/**
 * Retrieve the ray segment through the volume using OpenGL projection and the current view.
 *
 * @param outRaySegmentVC  the outgoing ray segment in volume coordinates
 * @param environment      the vxEnvironment
 * @param winMousePos      ???
 * @return                 ???
 */
bool vxUtils::GetIntersectionRayVolume(RaySegment & outRaySegmentVC, 
                                       vxEnvironment & environment, 
                                       Point2D<float4> screenCoord)
{
  /// The following is copied almost verbatim from vxRendererCpu3d.  In that way, we can guarantee that
  /// the picking ray is generated identically to the traced ray used to form the image.

  // get the GL state
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  Vector3D<float4> units(environment.GetIntensityVolume()->GetHeader().GetVoxelUnits());
  const Vector3D<float4> fVoxelUnitsReciprocal(1.0F/units.X(), 1.0F/units.Y(), 1.0F/units.Z());

  // find screen coordinates of the current point
  //Point2D<int2> screen(environment.GetViewerType()->ConvertMouseToScreen(mouseCoord));
  GLdouble x(screenCoord.m_x);
  GLdouble y(screenCoord.m_y);

  // find corresponding volume index coordinates
  GLdouble xWorld, yWorld, zWorld;
  if (gluUnProject(x, y, 0, mvmatrix, projmatrix, viewport, &xWorld, &yWorld, &zWorld) == GL_FALSE) 
    throw ex::VException(LogRec("gluUnProject failed", "vxUtils", "GetIntersectionRayVolume"));

  Vector4d<float4> rayBgnVC(xWorld * fVoxelUnitsReciprocal.X(), 
                            yWorld * fVoxelUnitsReciprocal.Y(), 
                            zWorld * fVoxelUnitsReciprocal.Z(), 0);
  if (gluUnProject(x, y, 1, mvmatrix, projmatrix, viewport, &xWorld, &yWorld, &zWorld) == GL_FALSE) 
    throw ex::VException(LogRec("gluUnProject failed", "vxUtils", "GetIntersectionRayVolume"));

  Vector4d<float4> rayEndVC(xWorld * fVoxelUnitsReciprocal.X(), 
                            yWorld * fVoxelUnitsReciprocal.Y(), 
                            zWorld * fVoxelUnitsReciprocal.Z(), 1);
  rayEndVC.T() = (rayEndVC - rayBgnVC).GetLengthXYZ();

  Vector4d<float4> rayDirVC((rayEndVC - rayBgnVC).GetNormalizedXYZ());
  Vector4d<float4> rayBgnPosVC = rayBgnVC;
  Vector4d<float4> rayEndPosVC;

  Box<float4> boxBound(vxUtils::GetVisibleBoundingBox(environment));
  bool bIntersection(vxUtils::HitBoundingBox4d(boxBound, rayBgnVC, rayDirVC, rayBgnPosVC));

  if (bIntersection == true)
  {
    // also compute ray exit point
    vxUtils::HitBoundingBox4d(boxBound, rayEndVC, -rayDirVC, rayEndPosVC);
    // set the exit value of T to the matching OpenGL depth buffer value
    rayEndPosVC.T() = rayEndVC.T() - rayEndPosVC.T();

    // move from outside bounding box into valid data area
    rayBgnPosVC += rayDirVC * 0.01F;

    outRaySegmentVC = RaySegment(rayBgnPosVC, rayEndPosVC);
  }

  return bIntersection;
} // GetIntersectionRayVolume()


/**
 * Get a measurement that represents the RAO or LAO angulation as if it was a C-arm
 *
 * @param viewDirection the view direction
 *
 * @return the degrees RAO (LAO if negative)
 */
float4 vxUtils::GetRaoLaoDegrees(const Normal3Df & viewDirection)
{
  float4 fDegrees(0);

  Vector3Df viewDirectionVector(viewDirection);
  viewDirectionVector.m_z = 0;
  if (viewDirectionVector.GetLength() == 0)
  {
    fDegrees = 0;
  }
  else
  {
    const Normal3Df viewNormal(viewDirectionVector);
    const Normal3Df towardTable(0, 1, 0);
    const double leftRightRadians(acos(viewNormal.Dot(towardTable)));
    const double leftRightDegrees(leftRightRadians * 180 / M_PI);
    fDegrees = leftRightDegrees;

    const Normal3Df towardRight(-1, 0, 0);
    if (towardRight.Dot(viewNormal) > 0) fDegrees *= -1;
  }

  return fDegrees;

} // GetRaoLaoDegrees()


/**
 * Get a string that represents the cranial or caudal angulation as if it was a C-arm
 *
 * @param viewDirection the view direction
 *
 * @return the degrees cranial angulation
 */
float4 vxUtils::GetCranCaudDegrees(const Normal3Df & viewDirection)
{

  const float4 fSign = (viewDirection.Z() > 0) ? -1 : 1;
  const double headAngleDegrees(acos(viewDirection.Z()) * 180 / M_PI);
  return fabs(90 - headAngleDegrees) * fSign;

} // GetCranCaudDegrees()


/**
 * Applies a visualization preset to an environment
 *
 * @param preset      the visualization preset
 * @param environment the environment
 */
void vxUtils::ApplyVisualizationPreset( vxVisualizationPreset * pPreset, vxEnvironment * pEnvironment )
{

  * pEnvironment->GetColormap() = * pPreset->GetColormap();
  vxUtils::AdjustColormapValuesToDataRange( * pEnvironment->GetColormap(), * pEnvironment->GetIntensityVolume() );
  pEnvironment->GetColormap()->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );

  // set the background color
  pEnvironment->GetBackgroundColor()->SetRGBA(
    pPreset->GetBackgroundColor().R() * 255,
    pPreset->GetBackgroundColor().G() * 255,
    pPreset->GetBackgroundColor().B() * 255,
    255 );
  pEnvironment->GetBackgroundColor()->Modified( vxModInfo( PixelRGBA< uint1 >, 0 ) );

  // set the rendering mode
  pEnvironment->GetRenderingMode()->SetRenderingQuality( pPreset->GetRenderingMode()->GetRenderingQuality() );
  pEnvironment->GetRenderingMode()->SetRenderingAlgorithm( pPreset->GetRenderingMode()->GetRenderingAlgorithm() );
  pEnvironment->GetRenderingMode()->SetSliceShadowMode( pPreset->GetRenderingMode()->GetSliceShadowMode() );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_JITTERING      , pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_JITTERING       ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION, pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_SHADING        , pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SHADING         ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_SHADOWS        , pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SHADOWS         ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_EVERY_PIXEL    , pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_EVERY_PIXEL     ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_CUTS_SHADED    , pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CUTS_SHADED     ) );
  pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE , pPreset->GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE  ) );
  pEnvironment->GetRenderingMode()->SetSlabThickness( pPreset->GetRenderingMode()->GetSlabThickness() );
  pEnvironment->GetRenderingMode()->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_ALGORITHM_MODIFIED ) );
  pEnvironment->GetRenderingMode()->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_DISPLAY_MODE_MODIFIED ) );
  pEnvironment->GetRenderingMode()->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_SLAB_THICKESSS_MODIFIED ) );

} // ApplyVisualizationPreset()


/**
 * Returns a collection of points in World coordinates which are the intersection between two planes
 *
 * @param     pDisplay      vxEnvironment being displayed. 
 * @param     pCurrent      The current vxEnvironment
 *
 * @return  Returns a vector of points ( Points should be used like vector[0] with vector[1], vector[2] with vector[3] and so on. )
 */
bool vxUtils::GetIntersectionsWorld( const vxEnvironment * pDisplay, const vxEnvironment * pCurrent, vxUtils::SliceShadowIntersections & intersections )
{
  // both planes are ortho
  if(( dynamic_cast< vxViewerTypeMPROrtho * >( pDisplay->GetViewerType() ) != NULL ) && 
     ( dynamic_cast< vxViewerTypeMPROrtho * >( pCurrent->GetViewerType() ) != NULL ))
  {
    return GetIntersectionsOrthoOnOrtho( pDisplay, pCurrent, intersections );
  }

  // get four corners of ortho plane in image space
  vxWinRect winRect(pCurrent->GetWinRect());
  Point2D<int4> maxBound(winRect.GetReferencePoint().first  + winRect.GetViewportSize().first  - 1,
                         winRect.GetReferencePoint().second + winRect.GetViewportSize().second - 1);
  Point2D<int4> imagePosition[4];
  imagePosition[0].m_x = winRect.GetReferencePoint().first;
  imagePosition[0].m_y = winRect.GetReferencePoint().second;
  imagePosition[2].m_x = winRect.GetReferencePoint().first  + winRect.GetViewportSize().first  - 1;
  imagePosition[2].m_y = winRect.GetReferencePoint().second + winRect.GetViewportSize().second - 1;
  imagePosition[1].m_x = imagePosition[2].m_x;
  imagePosition[1].m_y = imagePosition[0].m_y;
  imagePosition[3].m_x = imagePosition[0].m_x;
  imagePosition[3].m_y = imagePosition[2].m_y;

  // convert corners to world space
  Point3Df worldPosition[4];
  for (int4 i(0); i<4; ++i)
  {
    if (pCurrent->GetViewerType()->ConvertScreenToWorld(imagePosition[i], worldPosition[i], false) == false)
      return false;
  }
  
  enum SlicePlanes
  {
    FIRSTLINE,
    CENTERLINE,
    SECONDLINE,
  };

  // iterate over all lines that make a sliceshadow (2 outerlines and a centerline)
  for( int intersectionIndex( FIRSTLINE ); intersectionIndex <= SECONDLINE; ++intersectionIndex )
  {
    float4 fThinSlabThickness = ((intersectionIndex - 1) * ( pDisplay->GetRenderingMode()->GetSlabThickness() * 0.5F ));
    if( pCurrent->GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::DEFAULT )
      fThinSlabThickness = 0.0F;

    // compute intersection of oblique plane with current ortho slice
    std::vector< Point3Df > intersectionPoints;
    for (int4 i(0); i<4; ++i)
    {
      Line line(worldPosition[i], worldPosition[(i+1) % 4]);
      Point3Df isectPoint;
      /// intersect the ray with the plane
      if( dynamic_cast< vxViewerTypeMPROrtho * >( pDisplay->GetViewerType() ) != NULL )
      {
        vxPlaneOrtho< float4 > displayPlane( * pDisplay->GetPlaneOrtho() );
        displayPlane.SetDistance( displayPlane.GetDistance() + fThinSlabThickness );
        if( displayPlane.GetIntersection( line.GetPoint(), line.GetDirection(), isectPoint) == true)
        {
          intersectionPoints.push_back( isectPoint );
        }
      }
      else if( dynamic_cast< vxViewerTypeMPROblique * >( pDisplay->GetViewerType() ) != NULL )
      {
        vxPlaneOblique< float4 > displayPlane( * pDisplay->GetPlaneOblique() );
        displayPlane.SetDistance( displayPlane.GetDistance() + fThinSlabThickness );
        if( displayPlane.GetIntersection(line.GetPoint(), line.GetDirection(), isectPoint) == true)
        {
          intersectionPoints.push_back( isectPoint );
        }
      }
    }

    // choose the shortest pair of intersections that is still visible
    std::pair< int4, int4 > shortestPair( -1, -1 );
    float4 fShortestLength(1e9);
    for ( int i(0); i<intersectionPoints.size(); ++i )
    {
      for ( int j(1); j<intersectionPoints.size(); ++j )
      {
        if ( i != j  )
        {
          // is center of span visible on the screen?
          Point3Df centerPoint( ( Vector3Df( intersectionPoints[i] ) + Vector3Df( intersectionPoints[j] ) ) / 2.0F );
          Point2D<int2> screenCoord;
          if( pCurrent->GetViewerType()->ConvertWorldToScreen( centerPoint, screenCoord ) == false )
            return false;

          bool bVisible = winRect.IsInside( Point2D<uint2>( screenCoord ) );
          const float4 fLength( Vector3Df( intersectionPoints[i] - intersectionPoints[j] ).GetLength() );
          if ( bVisible && fLength < fShortestLength )
          {
            fShortestLength = fLength;
            shortestPair = std::make_pair( i, j );
          }
        }
      }
    }

    if( shortestPair == std::make_pair( -1, -1 ) )
      return false;

    if( intersectionIndex == CENTERLINE )
    {
      intersections.m_centerline.first = intersectionPoints[ shortestPair.first ];
      intersections.m_centerline.second = intersectionPoints[ shortestPair.second ];
    }
    else if( intersectionIndex == FIRSTLINE )
    {
      intersections.m_firstline.first = intersectionPoints[ shortestPair.first ];
      intersections.m_firstline.second = intersectionPoints[ shortestPair.second ];
    }
    else if( intersectionIndex == SECONDLINE )
    {
      intersections.m_secondline.first = intersectionPoints[ shortestPair.first ];
      intersections.m_secondline.second = intersectionPoints[ shortestPair.second ];
    }
  }

  return true;
} // GetIntersectionsObliqueOnOrtho( pDisplay, pCurrent )


/**
 * Returns a collection of points in World coordinates which are the intersection between an ortho plane and another ortho plane
 *
 * @param     pDisplay      vxEnvironment being displayed. 
 * @param     pCurrent      The current vxEnvironment
 *
 * @return  Returns a vector of points ( Points should be used like vector[0] with vector[1], vector[2] with vector[3] and so on. )
 */
bool vxUtils::GetIntersectionsOrthoOnOrtho( const vxEnvironment * pDisplay, const vxEnvironment * pCurrent, vxUtils::SliceShadowIntersections & intersections )
{
  const uint1 uIndex( pCurrent->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  float4 fDistance( pCurrent->GetPlaneOrtho()->GetDistance() );
  float4 fSliceShadow( pDisplay->GetPlaneOrtho()->GetDistance() );

  float4 fThinSlabThickness( 0.5F * pDisplay->GetRenderingMode()->GetSlabThickness() );
  if ( pDisplay->GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::DEFAULT )
    fThinSlabThickness = 0.0;
  
  const Triple<float4> size( pCurrent->GetIntensityVolume()->GetHeader().GetWorldDim() );

  switch( uIndex )
  {

    // sagittal
    case 0:
      if ( pDisplay->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 1 )
      {
        // calculate line for coronal slice
        intersections.m_centerline.first  = Point3Df( fDistance, fSliceShadow, 0 );
        intersections.m_centerline.second = Point3Df( fDistance, fSliceShadow, size.m_z );
        intersections.m_firstline.first   = Point3Df( fDistance, fSliceShadow - fThinSlabThickness, 0 );
        intersections.m_firstline.second  = Point3Df( fDistance, fSliceShadow - fThinSlabThickness, size.m_z );
        intersections.m_secondline.first  = Point3Df( fDistance, fSliceShadow + fThinSlabThickness, 0        );
        intersections.m_secondline.second = Point3Df( fDistance, fSliceShadow + fThinSlabThickness, size.m_z );
      }
      if ( pDisplay->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 2 )
      {
        // calculate line for axial slice
        intersections.m_centerline.first  = Point3Df( fDistance, 0       , fSliceShadow );
        intersections.m_centerline.second = Point3Df( fDistance, size.m_y, fSliceShadow );
        intersections.m_firstline.first   = Point3Df( fDistance, 0       , fSliceShadow - fThinSlabThickness );
        intersections.m_firstline.second  = Point3Df( fDistance, size.m_y, fSliceShadow - fThinSlabThickness );
        intersections.m_secondline.first  = Point3Df( fDistance, 0       , fSliceShadow + fThinSlabThickness );
        intersections.m_secondline.second = Point3Df( fDistance, size.m_y, fSliceShadow + fThinSlabThickness );
      }
      break;

    // coronal
    case 1:
      if ( pDisplay->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 0 )
      {
        // calculate line for sagittal slice
        intersections.m_centerline.first  = Point3Df( fSliceShadow,                      fDistance, 0        );
        intersections.m_centerline.second = Point3Df( fSliceShadow,                      fDistance, size.m_z );
        intersections.m_firstline.first   = Point3Df( fSliceShadow - fThinSlabThickness, fDistance, 0        );
        intersections.m_firstline.second  = Point3Df( fSliceShadow - fThinSlabThickness, fDistance, size.m_z );
        intersections.m_secondline.first  = Point3Df( fSliceShadow + fThinSlabThickness, fDistance, 0        );
        intersections.m_secondline.second = Point3Df( fSliceShadow + fThinSlabThickness, fDistance, size.m_z );
      }
      if( pDisplay->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 2 )
      {
        // calculate line for axial slice
        intersections.m_centerline.first  = Point3Df( 0       , fDistance, fSliceShadow );
        intersections.m_centerline.second = Point3Df( size.m_x, fDistance, fSliceShadow );
        intersections.m_firstline.first   = Point3Df( 0       , fDistance, fSliceShadow - fThinSlabThickness );
        intersections.m_firstline.second  = Point3Df( size.m_x, fDistance, fSliceShadow - fThinSlabThickness );
        intersections.m_secondline.first  = Point3Df( 0       , fDistance, fSliceShadow + fThinSlabThickness );
        intersections.m_secondline.second = Point3Df( size.m_x, fDistance, fSliceShadow + fThinSlabThickness );
      }
      break;

    // axial
    case 2:
      if ( pDisplay->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 0 )
      {
        // calculate line for sagittal slice
        intersections.m_centerline.first  = Point3Df( fSliceShadow                     , 0       , fDistance );
        intersections.m_centerline.second = Point3Df( fSliceShadow                     , size.m_y, fDistance );  
        intersections.m_firstline.first   = Point3Df( fSliceShadow - fThinSlabThickness, 0       , fDistance );
        intersections.m_firstline.second  = Point3Df( fSliceShadow - fThinSlabThickness, size.m_y, fDistance );          
        intersections.m_secondline.first  = Point3Df( fSliceShadow + fThinSlabThickness, 0       , fDistance );
        intersections.m_secondline.second = Point3Df( fSliceShadow + fThinSlabThickness, size.m_y, fDistance );
      }
      if ( pDisplay->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 1 )
      {
        // calculate line for coronal slice
        intersections.m_centerline.first  = Point3Df( 0       , fSliceShadow                     , fDistance );
        intersections.m_centerline.second = Point3Df( size.m_x, fSliceShadow                     , fDistance ); 
        intersections.m_firstline.first   = Point3Df( 0       , fSliceShadow - fThinSlabThickness, fDistance );
        intersections.m_firstline.second  = Point3Df( size.m_x, fSliceShadow - fThinSlabThickness, fDistance );  
        intersections.m_secondline.first  = Point3Df( 0       , fSliceShadow + fThinSlabThickness, fDistance );
        intersections.m_secondline.second = Point3Df( size.m_x, fSliceShadow + fThinSlabThickness, fDistance );
      }
      break;

    default:
      break; 
  } // switch

  return true;
} // GetIntersectionsOrthoOnOrtho( pDisplay, pCurrent )

// undefines
#undef FILE_REVISION


// $Log: vxUtils.C,v $
// Revision 1.35  2008/05/02 20:22:39  dongqing
// When we apply colormap, we adjust it to adaptive to volume intensity range.
// When we save current colormap to file, we have not restroe the standard
// colormap range back from the adaptive value. That was fixed this time.
// The fix looks silly. In future, we need to fix the entire color map and volume
// intensity range problem from groun up.
//
// Revision 1.34  2007/08/28 15:20:59  gdavidson
// Fixed GetIntersectionsOrthoOnOrtho to use the display environments slice thickness.
//
// Revision 1.33  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.32  2006/12/21 15:09:15  gdavidson
// Issue #5189 & 5106: Sets the rendering quality contained in the preset
//
// Revision 1.31  2006/10/20 20:55:00  romy
// Fixed issue 4612 . Now taking the voxel values instead of the average values
//
// Revision 1.30  2006/10/05 17:01:24  geconomos
// changed signature of ComputeObliqueQuadrilateralAnalytical
//
// Revision 1.29  2006/10/03 20:09:29  geconomos
// modified ApplyVisualizationPreset to handle new SliceShadowMode enumeration
//
// Revision 1.28  2006/09/25 20:09:24  geconomos
// removed uses debug out from AdjuistColormap
//
// Revision 1.27  2006/09/21 19:12:33  gdavidson
// Removed unnecessary glPixelStorei calls
//
// Revision 1.26  2006/09/21 14:56:19  geconomos
// created overloads for GenerateBitCodeComponentsVisible() and GenerateBitCodeLabelColorVisible() to take std::vector<vxComponent> as argument
//
// Revision 1.25  2006/07/21 15:06:23  geconomos
// added compiler warning in GenerateBitCodeForTransferFunction()
//
// Revision 1.24  2006/06/23 19:18:39  frank
// breaking out utility functions to take individual elements instead of just the environment
//
// Revision 1.23  2006/06/01 13:41:38  frank
// more C++ compliant
//
// Revision 1.22  2006/04/27 19:16:53  romy
// UnDoAction Reset method
//
// Revision 1.21  2006/04/03 18:43:20  geconomos
// removed create histogram method
//
// Revision 1.20  2006/04/03 16:57:27  geconomos
// removed CreateHistogram
//
// Revision 1.19  2006/03/29 20:02:42  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.18  2006/03/27 18:57:18  gdavidson
// Added GetIntersectionsWorld for slice shadow operations
//
// Revision 1.17  2006/03/02 15:46:23  frank
// pulled the environment out of one of the utility functions
//
// Revision 1.16  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.15  2006/02/24 20:12:48  frank
// added undo for visualization presets
//
// Revision 1.14  2006/02/17 15:11:58  frank
// working on extricating component handling from overengineered component handling classes
//
// Revision 1.13  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// Revision 1.12  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.11  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.10.2.6  2005/08/19 19:45:25  frank
// Issue #4358: Fixed problem with 3D detail on small vessels
//
// Revision 1.10.2.5  2005/08/01 19:12:32  frank
// changed function name according to coding standards
//
// Revision 1.10.2.4  2005/07/29 14:34:51  frank
// dead code
//
// Revision 1.10.2.3  2005/07/19 12:15:43  frank
// upgraded rendering mode xml for easier use and extensibility
//
// Revision 1.10.2.2  2005/07/18 18:01:28  frank
// applied more bits in the rendering mode from the preset
//
// Revision 1.10.2.1  2005/06/24 13:50:21  frank
// refactored the application of visualization presets
//
// Revision 1.10  2005/05/23 11:15:38  michael
// Reworked Scale and Bias to always be returned
//
// Revision 1.9  2005/05/10 20:00:24  michael
// Made BitStored part of the enum of header vars instead of having a dedicated
// access function.
//
// Revision 1.8  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.7.2.1  2005/02/11 13:49:17  frank
// Issue #3866: Distinct label map ended up with a zero entry if the blocks were filled with zero, but contained only zero entries. This ruined the subsequent compaction and segmentation volumes resulting in shifted, broken components.
//
// Revision 1.7  2004/11/02 18:08:06  michael
// getting cached bias instead of the Dicom field
//
// Revision 1.6  2004/10/05 14:35:59  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.5  2004/05/25 14:30:44  frank
// added base class for camera manipulators
//
// Revision 1.4  2004/04/26 22:57:05  vxconfig
// removed redundant code
//
// Revision 1.3  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.2  2004/04/06 16:27:47  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.100  2004/03/02 01:10:37  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.99  2004/02/25 14:32:17  michael
// coding standard: assign NULL after deletion
//
// Revision 1.98.2.1  2004/02/27 21:32:48  michael
// fixed ellipse annotation creation resulting in an empty structure that would cause
// an exception when normalizing its diameter vectors. Two initial points are now
// guaranteed to be different
//
// Revision 1.98  2004/02/11 14:06:48  frank
// fixed restricted segmentation and slice computations
//
// Revision 1.97  2004/02/05 02:59:33  frank
// fixed sculpting in 3D with shift key
//
// Revision 1.96  2004/02/03 22:57:56  michael
// added "mailto:"
//
// Revision 1.95  2004/02/03 12:27:30  michael
// being more type specific (points and vectors)
//
// Revision 1.94  2004/01/23 13:30:42  frank
// Added utility function to compute the intensity statistics for an ellipse
//
// Revision 1.93  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.92  2004/01/13 13:15:38  frank
// Fixed so that the sculpting and segmentation works properly on views derived from the oblique viewer type
//
// Revision 1.91  2004/01/12 14:16:36  frank
// fixed problem with sculpting on oblique with shift held down
//
// Revision 1.90  2004/01/09 17:18:01  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.89  2004/01/07 18:30:40  frank
// Moved utility code from Vascular to here
//
// Revision 1.88  2003/12/23 21:35:26  michael
// added adjustment function for colormap initialized from Preset for CT
//
// Revision 1.86  2003/12/16 20:19:01  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 1.85  2003/11/20 18:25:48  frank
// Fixed new component creation procedure
//
// Revision 1.84  2003/10/21 17:03:15  frank
// Fixed setting the standard views so that it does not view all if you do not ask it to
//
// Revision 1.83  2003/08/06 15:22:29  michael
// removed log messages
//
// Revision 1.82  2003/08/04 19:30:01  frank
// RAO/LAO now takes a view normal
//
// Revision 1.81  2003/08/04 14:41:04  frank
// Separated visibility tester class out from vxUtils
// Added rao/lao and cran/caud degree utility functions
//
// Revision 1.80  2003/08/01 12:30:53  michael
// took out temporarily inserted logging messages
//
// Revision 1.79  2003/07/29 11:04:15  michael
// added modified enum for BaseHistogram and added comments to code
//
// Revision 1.78  2003/07/17 14:08:52  frank
// Clamped CreateHistogram in case of bad values outside the DICOM promised range.
//
// Revision 1.77  2003/07/07 17:10:35  geconomos
// Issue#3325: When appending a component the ADDED event was being raised instead of MODIFIED.
//
// Revision 1.76  2003/06/23 19:05:09  michael
// use const instead of non const variable
//
// Revision 1.75  2003/06/19 20:23:25  michael
// replaced hard coded value by true 16bit mask
//
// Revision 1.74  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.73  2003/06/11 11:50:07  frank
// Fixed histogram mask - now 3D rotates about the center of the visible object again!
//
// Revision 1.72  2003/06/09 16:10:16  michael
// took out redundant include
//
// Revision 1.71  2003/06/09 15:52:36  michael
// added additional helper function to create intensity mask
//
// Revision 1.70  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.69  2003/06/05 13:32:05  michael
// added check for empty volume
//
// Revision 1.68  2003/06/04 18:46:17  jmeade
// code review.
//
// Revision 1.67  2003/06/04 18:24:04  michael
// added namespace declaration
//
// Revision 1.66  2003/05/29 19:21:29  michael
// fixed issue with empty bounding box upon camera initialization
//
// Revision 1.65  2003/05/29 11:58:31  michael
// removed GetScreenCoordinate() since it is now part of viewer type
//
// Revision 1.64  2003/05/27 13:10:50  frank
// Removed dead references
//
// Revision 1.63  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.62  2003/05/15 20:12:10  frank
// Moving away from old point class
//
// Revision 1.61  2003/04/18 19:56:20  michael
// initial code review
//
// Revision 1.60  2003/04/09 21:39:41  michael
// added a utility function to vxUtils to compact the label indices
//
// Revision 1.59  2003/04/09 15:05:30  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.58  2003/04/07 20:21:40  michael
// changed maximum number of tries to find a unique name to 2**16-1 (uint2)
//
// Revision 1.57  2003/03/31 18:57:39  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.56  2003/03/31 18:04:38  michael
// we are now making use of homogeneous blocks when computing the histogram
//
// Revision 1.55  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.54  2003/02/25 18:20:53  frank
// Cleaned up visibility logic
//
// Revision 1.53  2003/02/13 17:01:58  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.52  2003/02/11 16:04:11  michael
// removed member number of bins that cached the vector size
//
// Revision 1.51  2003/02/10 23:32:53  michael
// fixed histogram bug ... should be revisited since it isn't smart to cache the
// lenght of a vector but allow a non-const reference return of the vector so
// that this one can be assigned using oerator= of std::vector, bypassing the
// cached value for the number of entries -> inconsistent!
//
// Revision 1.50.2.1  2003/02/12 22:47:25  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.50  2003/01/22 21:47:49  ingmar
// dimension and units are now Triples
//
// Revision 1.49  2003/01/22 15:07:17  michael
// cosmetics
//
// Revision 1.48  2003/01/07 00:29:04  ingmar
// made division by zero impossible
//
// Revision 1.47  2001/01/09 13:01:04  frank
// cleaned out dead code
//
// Revision 1.46  2002/12/24 17:35:10  frank
// Cleaned up picking in 3D
//
// Revision 1.45  2002/12/23 22:48:14  ingmar
// removed redundant use of Triple copy constructor
//
// Revision 1.44  2002/12/19 21:25:25  ingmar
// HitBoundingBox now takes a Box and not 2 vectors as box argument
//
// Revision 1.43  2002/12/17 17:40:53  frank
// Moved oblique corner computation into shared area so others can use it.
//
// Revision 1.42  2002/12/17 14:33:40  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.41  2002/12/16 23:45:11  frank
// Checked mask before creating component
//
// Revision 1.40  2002/12/09 22:02:32  ingmar
// changed the semantics of IsBlockVisibleLabel() such that if block is visible is checked this function returns true.
// usage of the label color no longer influences the return value
// --> drawing uniform blocks in 2D is now correct
//
// Revision 1.39  2002/12/06 19:19:35  ingmar
// vxRenderingMode::SHOW_LABEL -> SHOW_COMPONENT_IN_MPR
//
// Revision 1.38  2002/11/27 21:56:52  ingmar
// removed compiler warning
//
// Revision 1.37  2002/11/27 19:21:08  ingmar
// removed superfluous rage from interface to IsBlockVisibleSegmentationRange()
// (the range is automatically determined in the constructor)
//
// Revision 1.36  2002/11/27 19:13:45  ingmar
// added IsBlockVisibleSegmentationRange()
//
// Revision 1.35  2002/11/26 18:06:55  ingmar
// added HitBoundingBox4d
//
// Revision 1.34  2002/11/26 13:39:43  michael
// renamed method (GetHistoryFromLabel())
//
// Revision 1.33  2002/11/20 17:43:57  michael
// cometics
//
// Revision 1.32  2002/11/19 21:26:27  michael
// Made method start with capital letter (ReadFramebuffer)
//
// Revision 1.31  2002/11/18 16:48:06  ingmar
// added IsBlockVisibleRaw()
//
// Revision 1.30  2002/11/15 23:11:57  ingmar
// fixed vxBlockVisibilityTester
//
// Revision 1.29  2002/11/15 04:06:58  frank
// Fixed component name, returned the new component label.
//
// Revision 1.28  2002/11/14 18:03:20  ingmar
// removed time to generate histogram
//
// Revision 1.27  2002/11/14 17:33:49  ingmar
// code beautify
//
// Revision 1.26  2002/11/13 02:35:24  frank
// ComponentArray changed...
//
// Revision 1.25  2002/11/13 02:18:16  frank
// Moved unique component name function into vxUtils.
//
// Revision 1.24  2002/11/13 00:13:54  michael
// account for changes in vxComponent and vxComponentArray
//
// Revision 1.23  2002/11/12 00:14:53  michael
// cosmetics
//
// Revision 1.22  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.21  2002/11/07 00:02:58  ingmar
// code beautify
//
// Revision 1.20  2002/11/02 03:09:31  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.19  2002/10/24 15:30:06  michael
// added helper method to align a camera to a new box proportionally
//
// Revision 1.18  2002/10/13 21:41:20  michael
// accounting for the const Box<T> returned from the camera object
//
// Revision 1.17  2002/10/03 17:34:43  michael
// adjusted helper methods for setting the camera
//
// Revision 1.16  2002/10/02 02:37:28  michael
// Moved helper functions for camera initialization to 70_vxUtils
//
// Revision 1.15  2002/09/27 22:59:12  frank
// Issue #2668: Fixed black line in MIP/VR images.
//
// Revision 1.14  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.13  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.12  2002/09/19 14:25:25  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.11  2002/09/17 21:41:58  dmitry
// ClampToViewport added
//
// Revision 1.10  2002/09/17 13:01:38  michael
// tried to move ReadFromFile of transfer function ... but has to wait.
//
// Revision 1.9  2002/09/16 19:58:30  dmitry
// New vxID (s) used.
//
// Revision 1.8  2002/09/05 22:37:46  jaddonisio
// New vxComponent.
//
// Revision 1.7  2002/09/03 20:53:04  ingmar
// bounding box no longher extends past the volume boundary
//
// Revision 1.6  2002/08/26 20:56:44  ingmar
// better crop box computation when the environment crop box is not set
//
// Revision 1.5  2002/08/26 14:39:49  ingmar
// fixed cropBox to include all of the maximum block
//
// Revision 1.4  2002/08/22 22:35:25  uday
// Added GetVisibleBoundingBox().
//
// Revision 1.3  2002/08/12 14:35:19  ingmar
// added WriteComponentsAsSeparateVolumes()
//
// Revision 1.2  2002/08/02 14:51:37  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.1  2002/07/30 20:21:03  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.25  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.24  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.23  2002/07/05 21:43:25  ingmar
// comment cosmetics
//
// Revision 1.22  2002/07/05 15:24:38  ingmar
// code beautify and comment clarification
//
// Revision 1.21  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.20  2002/07/01 15:20:33  michael
// clean-up and fix of bug
//
// Revision 1.19.2.4  2002/07/11 14:03:55  jaddonisio
// Set low and high non-zero bins when creating histogram.
//
// Revision 1.19.2.3  2002/07/08 15:22:15  frank
// Restricted the sculpting to the visible voxels.
//
// Revision 1.19.2.2  2002/07/02 15:52:23  uday
// GetSlabPair() update.
//
// Revision 1.19.2.1  2002/07/01 16:44:31  michael
// fixed problems with reading values from depth buffer
//
// Revision 1.19  2002/06/26 21:55:31  uday
// Added GetSlabPair().
//
// Revision 1.18  2002/06/26 21:35:53  uday
// HitBoundingBox() update.
//
// Revision 1.17  2002/06/26 20:09:45  uday
// Added HitBoundingBox().
//
// Revision 1.16  2002/06/26 16:02:48  soeren
// optimized create histogram (10 % speed up), for some reason it is faster to create a temporary vector plus creation of a const iOffset.
//
// Revision 1.15  2002/06/12 13:30:49  manju
// Modified GetScreenCoordinates method.
//
// Revision 1.14  2002/05/21 15:27:34  manju
// Added a method to convert the world coordinates to screen .
//
// Revision 1.13  2002/05/15 18:39:42  soeren
// added try - catch in CreateHistogram
//
// Revision 1.12  2002/05/10 18:48:26  soeren
// optimized histogram
//
// Revision 1.11  2002/05/10 16:44:47  jenny
// clamped intensity values for the histogram
//
// Revision 1.10  2002/05/09 20:59:15  jenny
// put CreateHistogram function
//
// Revision 1.9  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.8  2002/04/29 13:21:55  michael
// Removed exception and return value outside vollume instead (FIX ME!)
//
// Revision 1.7  2002/04/26 13:38:22  michael
// Removed throwing exception (FIX ME)
//
// Revision 1.6  2002/04/25 19:24:07  michael
// removed exception throw ... for now (FIXME!)
//
// Revision 1.5  2002/04/14 00:15:28  michael
// adjusted method name
//
// Revision 1.4  2002/04/09 21:20:23  dmitry
// CalcOffsetDistance method added.
//
// Revision 1.3  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.2  2002/03/21 18:23:57  michael
// minor changes to the backprojection, still to be debugged
//
// Revision 1.1  2002/03/20 19:43:31  michael
// added OpenGL based back-projection methods
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUtils.C,v 1.35 2008/05/02 20:22:39 dongqing Exp $
// $Id: vxUtils.C,v 1.35 2008/05/02 20:22:39 dongqing Exp $
