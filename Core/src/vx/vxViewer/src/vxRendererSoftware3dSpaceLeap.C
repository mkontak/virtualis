// $Id: vxRendererSoftware3dSpaceLeap.C,v 1.3 2006/04/10 19:46:09 frank Exp $
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
#include "vxRendererSoftware3dSpaceLeap.h"
#include "vxIDs.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxEnvironment.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/** 
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererSoftware3dSpaceLeap::vxRendererSoftware3dSpaceLeap(vxEnvironment & environment)
:vxRendererCpu3D(environment)
//*******************************************************************
{
  m_pIterDOB = NULL;
} // Default constructor


/**
 * Destructor.
 */
vxRendererSoftware3dSpaceLeap::~vxRendererSoftware3dSpaceLeap()
//*******************************************************************
{
  if (m_pIterDOB != NULL)
  {
    delete m_pIterDOB;
    m_pIterDOB = NULL;
  }
} // Destructor


/**
 * PreRender routine
 * @return success
 */
int4 vxRendererSoftware3dSpaceLeap::PreRender()
//*******************************************************************
{
  //SetNumberOfThreads(1);

  // call base class
  vxRendererCpu3D::PreRender();

  // distance from boundary volume
  m_pVolumeDOB = dynamic_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetElement(vxIDs::DistanceFromBoundary));

  if (m_pIterDOB == NULL)
  {
    m_pIterDOB = new vxBlockVolumeIterator<uint2>(*m_pVolumeDOB);
  }

  return true;
} // PreRender()


/**
 * PostRender routine.
 * @return 1 in case of success
 */
int4 vxRendererSoftware3dSpaceLeap::PostRender()
//*******************************************************************
{
  vxRendererCpu3D::PostRender();

  return true;
} // PostRender()


/**
 * Composite sampling points along a single ray.
 * @param args the ray data structure
 */
void vxRendererSoftware3dSpaceLeap::SampleSingleRay(Args & args)
//*******************************************************************
{
  //vxRendererSW3D::SampleSingleRay(args);
  //return;

  // initialize the colors that are about to be accumulated along the ray
  args.fPreMultAccum.R() = args.fPreMultAccum.G() = args.fPreMultAccum.B() = args.fPreMultAccum.A() = 0;
  args.fDepthToSurface = (std::numeric_limits<float>::max)();
  args.iPrevIndex = 0;

  //// Composite the sampling points along a ray.
  while (args.bContinueRay == true)
  {
    vxRendererCpu3D::SpaceLeap(args);

    while (args.bContinueRay == true)
    {
      if (args.pRayIterIntensity->GetPosFloat().T() <= args.fRayIntersectBoundingBoxEndVC.T()) 
      {
        RayGetSample(args);

        // composite only if alpha is > 0
        if (args.fPreMultSample.A() > 0)
        {
          RayShadeSample(args);
          RayCompositeSample(args);

          if (args.fPreMultAccum.A() > 0.95)
          {
            args.bContinueRay = false;
          }
        } // composite only if alpha is > 0
        else
        {
          ++(*args.pRayIterIntensity);
          break;
        }
      }
      else
      {
        // we actually went outside and the iterator is outside volume 
        args.bContinueRay = false;
      }

      ++(*args.pRayIterIntensity);
    } // end sampling
  } // while not early ray termination

  RayBlendSliceShadowsAtEndOfRay(args);
  RaySetFinalColors(args);
} // SampleSingleRay()


/**
 * Space Leap
 * @param args the ray data structure
 */
void vxRendererSoftware3dSpaceLeap::SpaceLeap(Args & args)
//*******************************************************************
{
  //// traverse empty space
  //RaySpaceLeapLargeBlocks(args);
  RaySpaceLeapDistanceVolume(args);
} // SpaceLeap()


/**
 * Space leaps through all 16^3 blocks that have at least one layer of empty 16^3 
 * blocks around them. After that it jumps top the end of the border 16^3 blocks.
 * @param args ray data structure
 */
inline void vxRendererSoftware3dSpaceLeap::RaySpaceLeapDistanceVolume(Args & args)
//*******************************************************************
{
  if (m_pVolumeDOB != NULL)
  {
    m_pIterDOB->SetPos(args.pRayIterIntensity->GetPosInt());
    int4 iSpaceLeapInfo(0);

    while (0 < (iSpaceLeapInfo = m_pIterDOB->GetVoxel()))
    {
      float4 fSpaceLeapDist(iSpaceLeapInfo);
      args.pRayIterIntensity->SpaceLeapMM(0.25F * fSpaceLeapDist); 
      m_pIterDOB->SetPos(args.pRayIterIntensity->GetPosInt());
    }
  }
  else
  {
    LogDbg("Missing distance from boundary volume!!!", "", "");
  }

  return;
} // RaySpaceLeapLargeBlocks()


/**
 * Sample the intensity and label at the current ray position
 * @param args     the ray data structure
 */
inline void vxRendererSoftware3dSpaceLeap::RayGetSample(Args & args)
//*******************************************************************
{
  // Get sample intensity
  args.iSample = args.pRayIterIntensity->GetSampleInt();

  // VR mode
  const int4 iIndex((uint2(args.iSample)) >> args.iShiftTo8Bit);
  float4 * pTFunc2D(&args.pfColormap2d8bitPreIntegrated[5*(256*args.iPrevIndex + iIndex)]);
  args.iPrevIndex = iIndex;
  args.fPreMultSample.R() = *pTFunc2D++;
  args.fPreMultSample.G() = *pTFunc2D++;
  args.fPreMultSample.B() = *pTFunc2D++;
  args.fPreMultSample.A() = *pTFunc2D++;
  args.fGradientWeight = *pTFunc2D;
} // RayGetSample()


/**
 * Adjust (lower) the sample color intensity doe to shading.
 * Considers one diffuse and one specular light source.
 * Both operate on the data gradient as well as the inverted data gradient to
 * allow surface appearance for low to high density transitions as well as high to low
 * @param args     the ray data structure
 */
inline void vxRendererSoftware3dSpaceLeap::RayShadeSample(Args & args)
//*******************************************************************
{
  //if (m_bShading == true)
  {
    // get gradient from precomputed optimal position between previous and curent sample
    //if (m_bPreIntegration == true)
      args.pRayIterIntensity->SpaceLeap(-args.fGradientWeight * args.fInterSampleDistance);

    Vector3D<float4> vGradVector3D(-args.pRayIterIntensity->GetSampleGradientCentralDifference());
    //if (Global_bShowSpaceLeapPattern) DrawSpaceLeapPatternLine(args, eGradientLocation, true);

    //if (m_bPreIntegration == true)
      args.pRayIterIntensity->SpaceLeap(args.fGradientWeight * args.fInterSampleDistance);

    // anisotropic gradient direction correction
    vGradVector3D.Y() *= args.fGradientScaleY;
    vGradVector3D.Z() *= args.fGradientScaleZ;

    // illumination computation
    if (vGradVector3D.GetLengthSquared() > 10)
    {
      vGradVector3D.Normalize();
      float4 fIlluminationIntensity(m_illuminator.ShadePhongDiffuse (vGradVector3D, args.fLight2DirVC));
      float4 fSpecular(args.fPreMultSample.A() * 255 * m_illuminator.ShadePhongSpecular(vGradVector3D, args.fHalfwayLight1VC));

      // sample color intensity adjustment
      args.fPreMultSample.ScaleRGB(fIlluminationIntensity);
      args.fPreMultSample.AddRGB(fSpecular);
    }
  }
/*
  if (m_bIsBoxOfInterestActive == true)
  {
    Vector4d<float4> fPos(args.pRayIterIntensity->GetPosFloat());

    if( args.fBoxOfInterest.IsInside(Triple<float4>(fPos.X(), fPos.Y(), fPos.Z())))
    {
      PixelRGBA<float4> blend(0, 0, 255, 255);
      args.fPreMultSample.BlendRGB(blend, .40F);
      //args.fPreMultSample.ScaleRGB(0.75F);
    }
  }
*/
} // RayShadeSample()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererSoftware3dSpaceLeap.C,v $
// Revision 1.3  2006/04/10 19:46:09  frank
// rendering acceleration 30%+
// leaped to min distance of nearest left and right eighth column in scanline as well to above and below pixels
//
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1  2004/10/26 13:17:17  vxconfig
// moved from colon plugin
//
// Revision 1.4  2004/10/25 23:47:57  michael
// improved space leap raycaster
//
// Revision 1.3  2004/10/21 16:40:04  michael
// derive from ray caster in vxViewer dll, otherwise very slow!!!
//
// Revision 1.2  2004/10/20 21:49:50  michael
// Added copy of the raycaster base class so that we can play around without
// constantly recompiling vxViewer
//
// Revision 1.1  2004/10/20 13:17:34  geconomos
// renamed from viatronix.v3d.viewer.plugins.vc
//
// Revision 1.1  2004/09/09 18:22:32  michael
// added initial files
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftware3dSpaceLeap.C,v 1.3 2006/04/10 19:46:09 frank Exp $
// $Id: vxRendererSoftware3dSpaceLeap.C,v 1.3 2006/04/10 19:46:09 frank Exp $
