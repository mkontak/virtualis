// $Id: vxRaytracer.h,v 1.10 2007/03/07 21:05:13 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille  (mailto:frank@viatronix.com)

// pragma declarations
#pragma once

// include declarations
#include "vxRaytracerRenderArgs.h"
#include "vxRaytracerSettings.h"
#include "vxRenderBuffer.h"
#include "Timer.h"

// class declaration
class VX_VIEWER_DLL vxRaytracer 
{
/// member functions
public:
  
  /// constructor
  vxRaytracer();
  
  /// destructor
  virtual ~vxRaytracer();
  
  /// renders an image into the buffer
  void Render();

  /// gets the settings
  vxRaytracerSettings & GetSettings() { return m_settings; }  

  /// gets the render buffer
  vxRenderBuffer & GetRenderBuffer() { return *m_pRenderBuffer; }

  /// sets the render buffer
  virtual void SetRenderBuffer( vxRenderBuffer * pRenderBuffer ) { m_pRenderBuffer = pRenderBuffer; }

  /// sets the render buffer
  virtual void SetRenderSize( const Triple< uint4 > & renderSize ) { m_settings.SetRenderSize( renderSize ); }

  /// Prerenderer
  virtual void PreRender();
  
  /// renders the specified window into the buffer
  virtual void RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId );
  
  /// aborts rendering
  virtual void Abort();


protected:

  /// space leap
  virtual void SpaceLeap( vxRaytracerRenderArgs & args );
  
private:

  /// initializes the raytracer arguments structure
  void InitializeRenderArguments();

  /// cast one ray through the volume
  void CastOneRay( vxRaytracerRenderArgs & args );

  /// space leaps through all 16^3 blocks
  inline void RaySpaceLeapLargeBlocks( vxRaytracerRenderArgs & args );

  /// space leap over blocks of inivisible labels
  inline void RaySkipDensityInvisible( vxRaytracerRenderArgs & args );

  /// space leap over blocks of invisible intensity
  inline void RaySkipAlphaInvisible( vxRaytracerRenderArgs & args );

  /// sample the intensity and label at the current ray position
  inline void RayGetSampleAndLabel( vxRaytracerRenderArgs & args );

  /// blend the label color over the sample
  inline void RayBlendLabelColor( vxRaytracerRenderArgs & args );

  /// blend the intensity range over the sample if blending is enabled
  inline void RayBlendIntensityRange( vxRaytracerRenderArgs & args );

  /// blend the slice shadows over the sample if shadow blending is enabled
  inline void RayBlendSliceShadows( vxRaytracerRenderArgs & args );

  /// blend the slice shadows at the end of sampling the ray
  inline void RayBlendSliceShadowsAtEndOfRay( vxRaytracerRenderArgs & args );

  /// retrieve the next sample in MIP projection
  inline void RayNextMipSample( vxRaytracerRenderArgs & args );

  /// retrieve the next sample in X-ray projection
  inline void RayNextXRaySample( vxRaytracerRenderArgs & args );

  /// shade the sample
  virtual void RayShadeSample( vxRaytracerRenderArgs & args );

  /// color the ray based on the gradient direction (debug use only)
  inline void RayColorFromGradientDir( vxRaytracerRenderArgs & args, const Vector3D<float4> vGradVector3D );

  /// composite the current sample over the accumulated ray
  inline void RayCompositeSample( vxRaytracerRenderArgs & args );

  /// take the accumulated ray colors and convert them into displayable uint1 pixels
  inline void RaySetFinalColors( vxRaytracerRenderArgs & args );

  /// composite sampling points along a single ray.
  virtual void SampleSingleRay( vxRaytracerRenderArgs & args );

  /// decide if the pixels are close enough in color to interpolate rather than raycast
  inline bool IsOkToInterpolate( const uint1 * const pPixelA,  const uint1 * const pPixelB1,
                                 const uint1 * const pPixelB2, const uint1 * const pPixelB3,
                                 bool & bPixelsAreDifferent);

  /// test if Pixels are similar enough to just interpolate intead of sending a ray.
  bool ArePixelsSimilar( const uint1 * const pPixelA,  const uint1 * const pPixelB1,
                         const uint1 * const pPixelB2, const uint1 * const pPixelB3);

  /// test if Pixels are similar enough to just interpolate intead of sending a ray.
  bool IsPixelTransparent( const unsigned char * pPixelA );

  /// interpolates between two pixels.
  void PixelInterpolate( const uint1 * const pPixelA, const uint1 * const pPixelB, uint1 * pPixelC );

  /// checks if the intensity of the specified label is visible
  inline bool IsIntensityVisible( const uint4 uLabelIndex );

  ///////////////////////////////////////////////////////////////////////
  // debug image creation

  /// draws the adaptive sampling pattern
  void DrawAdaptiveSamplingPatternPixel( vxRaytracerRenderArgs & args, int4 u, int4 v, uint1 eSamplingInfo );

  /// draws space leap pattern line
  void DrawSpaceLeapPatternLine( vxRaytracerRenderArgs & args, uint1 eSamplingInfo, bool bNeedToUpdateLabelPos = false );
  
// member variables
private:

  // settings
  vxRaytracerSettings m_settings;

  // render buffer
  vxRenderBuffer * m_pRenderBuffer;
  
  // render arguments
  vxRaytracerRenderArgs m_arguments;
  
  // indicates if rendering should be canceled
  bool m_bCancelRendering;

  ///////////////////////////////////////////////////////////////////////
  // debug info

  /// frame rate timer
  Timer m_frameRateTimer;

  /// array to indicate space leaping pattern.
  std::vector<uint1> m_evSamplingInfo;

  /// parallel array of associated ray sampling positions.
  std::vector<Vector4d<float4> > m_fvSamplingPos;

  /// sampling pattern index
  int4 m_iSamplingPatternIndex;

	/// maximum value for voxel value clamping
	int4 m_iMaxVoxelValue;

}; // class vxRaytracer 

#include "vxRaytracer.inl"


// $Log: vxRaytracer.h,v $
// Revision 1.10  2007/03/07 21:05:13  romy
// added comments - part of code review
//
// Revision 1.9  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.8  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.7  2006/12/11 16:58:46  geconomos
// Issue #5158: component display is incorrect
//
// Revision 1.6  2006/08/03 14:17:25  geconomos
// updated for switch to vxRenderingNodePlugin usage
//
// Revision 1.5  2006/07/21 15:03:01  geconomos
// cosmetics
//
// Revision 1.4  2006/07/07 13:44:52  geconomos
// + update for changes to vxRenderingEngineRenderer
// + working on rendering loop improve image quality (missing chuncks from image)
//
// Revision 1.3  2006/06/27 20:15:34  geconomos
// now inheriting from vxRenderingEngineRenderer
//
// Revision 1.2  2006/06/21 14:07:49  geconomos
// + code cleanup
// + exported classes
//
// Revision 1.1  2006/06/20 20:05:48  geconomos
// refactored vxSoftwareRendererCpu3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRaytracer.h,v 1.10 2007/03/07 21:05:13 romy Exp $
// $Id: vxRaytracer.h,v 1.10 2007/03/07 21:05:13 romy Exp $
