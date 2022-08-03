// $Id: VolumeRender.h,v 1.7.2.2 2010/02/01 19:33:15 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.net)


/**
 * Performs the volume rendering with all the associated speedups
 */


// pragmas
#pragma once


// includes
#include <string>
#include "ReaderLib.h"
#include "Global.h"
#include "Volume.h"
#include "Jitter.h"
#include "SliceViewer.h"
#include "RenderContext.h"
#include "RenderThread.h"
#include "Timer.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"


// namespaces
namespace ReaderLib
{

class RenderThread;

class VolumeRender : public LinVolUC, Sec_ReaderLib
{
public:

  /// Different types of views
  enum ProjectionEnum { PERSPECTIVE = 0, MERCATUR, SPHERICAL, FRANKS, PARALLEL };

  /// image mode
  enum ImageModeEnum { ADAPTIVE = 0, FIXED };

public:

  /// Constructor:
  VolumeRender();

  /// Destructor: Releases all data structures
  virtual ~VolumeRender();

  /// Resets all variables that change whenever a new study is loaded
  void Reset();

  /// Create the render threads early while we still have memory
  void PreLaunchRenderThreads(Dataset* dataset);

  /// Read sets up some things after the Volume::Read
  bool ReadFile(const std::string & sFilename);

  /// Get the current setting of the visible image
  inline uint4 GetDisplaySize() const;

  /// Set the size of the final image on the screen
  inline void SetDisplaySize(const uint4 uDisplaySize);

  /// Get the number of actual rays cast in each dimension
  inline uint4 GetRenderSize() const;

  /// Set the number of actual rays cast in each dimension
  void SetRenderSize(const uint4 uRenderSize);

  /// Get the (power of 2) size of the render buffer
  inline uint4 GetRenderBufferSize() const;

  /// Get the Depth Buffer for for the image
  inline float4 * GetDepthBuffer() const;

  /// Move the translucent magic waindow around the screen
  void MoveWindow(const int4 iDeltaX, const int4 iDeltaY);

  /// Create an image
  void Render(Dataset & dataset, int iMaxWindowSize, bool bInverseImg);

  /// Create an image
  void RenderIntoBitmap(Dataset & dataset);

  /// Create an image
  uint1 * GetRenderBuffer() {return m_puImageBuffer;};

  /// gets the number of render threads
  uint4 GetNumberOfRenderThreads() const { return m_threads.size(); }

  /// Gets Minimum render size
  static int4 GetMinRenderSize() { return m_iMinRenderSize; }

  /// Gets Maximum render size
  static int4 GetMaxRenderSize() { return m_iMaxRenderSize; }

  /// Gets Minimum frame rate
  static float4 GetMinFrameRate() { return m_fMinFrameRate; }

  /// Gets Maximum frame rate
  static float4 GetMaxFrameRate() { return m_fMaxFrameRate; }

  /// gets the last time it took render an entire frame (including other views, i.e. overview, slices )
  float4 GetLastRenderTime() { return m_fLastRenderTime; }

  /// sets the last time it took render an entire frame (including other views, i.e. overview, slices )
  void SetLastRenderTime( float4 fTime ) { m_fLastRenderTime = fTime; }

  /// gets the image mode
  ImageModeEnum GetImageMode() const { return m_eImageMode; }

  /// sets the image mode
  void SetImageMode( ImageModeEnum eImageMode ) { m_eImageMode = eImageMode; }

  /// Perspective Endoscopic Raycast (assumes the viewpoint is within the volume)
  void Raycast1(int4 id, Dataset & dataset);
  void Raycast2(int4 id, Dataset & dataset, const Rect<int4> & renderingBounds );

  /// Perspective Raycasting (assumes the viewpoint is outside the volume)
  void RaycastASExternal(int4 id, RenderContext& rc, Dataset & dataset);

  /// Raycast an arbitrarily oriented 2D slice
  void RayCastObliqueSlice( Dataset & dataset, const Point<float4> & vBottomLeft, const Point<float4> & vBottomRight,
                            const Point<float4> & vTopLeft,    const Point<float4> & vTopRight,
                            const Vector<float4> & vViewVector, const Normal<float4> & vUpNormal,
                            uint1 * puImageBuffer, const uint4 uImageWidth, const uint4 uImageHeight,
                            const uint4 uDimBorderPixelsX,
                            const uint4 uSubImageStartX, const uint4 uSubImageStartY,
                            const uint4 uSubImageWidth, const uint4 uSubImageHeight, 
                            const bool bRadialProjection, const bool bSampleUncleansed, const bool bSampleDepth,
                            const bool bSampleActualDepth, Normal3D<float4> & depthSampleNormal, Point3D<float4> & depthSampleInOut );

  /// Render an arbitrarily oriented 2D slice
  void CreateObliqueSlice(const Point<float4> & vBottomLeft, const Point<float4> & vBottomRight,
                          const Point<float4> & vTopLeft,    const Point<float4> & vTopRight,
                          uint1 * puImageBuffer, const uint4 uImageWidth, const uint4 uImageHeight, 
                          const uint4 uDimBorderPixels, const bool bSampleUncleansed = false);

  /// Reset the accumulation buffer
  inline void FlushAccumulationBuffer()
  {
    // -1 indicates that the accumulation buffer needs to be cleared
    m_iAccumulationCount = -1;
  } // FlushAccumulationBuffer

  /// Grab the uncleansed value for a voxel using the diff volume or the regular volume
  inline uint1 GetUncleansedValue( const Point<uint2> & position ) const;
  
  /// Set the link to the uncleansed-diff-block-volume
  void LinkToDiffVolume( vxBlockVolume<uint1> * pDiffVolume );

  /// Resample the uncleansed data: vPosition in volume coordinate.
  float4 GetUncleansedResampledValue(const Point<float4> & vPosition) const;

  /// Calculate the accumulated opacity between two points in the volume
  float4 CalculateAccumulatedOpacity( const Point3D<float4> & startPointVolume,
                                      const Point3D<float4> & endPointVolume,
                                      const float4 * pfOpacitySurfaceTransferFunction,
                                      const bool & bCleansed,
                                      Point<float4> & opaquePointOut );

private:

  /// Utility for washout math
  inline void ClampFloatTo255(float4& fValue);

  /// Utility for washout math
  inline void ClampFloatTo255RGB(float4& fRedValue, float4& fGreenValue, float4 &fBlueValue);

  /// Changes the ray direction for Mercator/Spherical/etc projections
  inline void SetupAlternativeProjection(const uint4 &u, const uint4 &v,
                                         const float4 &fDu, const float4 &fDv,
                                         const Point<float4> &flc_vViewpoint,
                                         const Vector<float4> &flc_vViewVector,
                                         const Vector<float4> &flc_vUpVector,
                                         const Vector<float4> &flc_vRightVector,
                                         const Vector<float4> &flc_vDownVector,
                                         const float4 &flc_fFovRadians,
                                         Normal<float4> &vRayNormal,
                                         Normal<float4> &vLight1Normal,
                                         Normal<float4> &vLight2Normal,
                                         Point<float4> &vPoint,
                                         float4 &fLeapDistance);

  /// Perform Volume Rendering speedup via leaping through balnk space
  inline bool SpaceLeap(const Normal<float4> &vRayNormal,
                        const uint4 &flc_iXStep, const uint4 &flc_iYStep, const uint4 &flc_iZStep,
                        const float4 &flc_fXScl, const float4 &flc_fYScl, const float4 &flc_fZScl,
                        const uint1 * const flc_puSLDvol, const float4 &flc_fDfbScale,
                        const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                        const float4 &fFirstLeapDistance, Point<float4> &vPoint, float4 &fDepth,
                        const uint4 flc_iVolMaxX, const uint4 flc_iVolMaxY, const uint4 flc_iVolMaxZ);

  /// Count and paint the newly visible surface voxels
  inline void VisibleSurfaceDetermination3D(const float4 &fDepth, uint1 * const &flc_puPaintVol, const uint1 &flc_uCurrentSeenValue,
                                            const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                            const uint4 &flc_uXYStep, const uint4 &flc_uYZStep, const uint4 &flc_uXZStep,
                                            const uint4 &flc_uXYZStep, const uint4 &uIndexOffset, uint4 &uSurfaceCount);


  /// Change the color of the current sample if it intersects with the shadow
  inline void ShadowSample(const float4 & fAbsDistance, const float4 & fHalfThickness, const float4 & fFalloff,
                           float4 & fSampleRed, float4 & fSampleGreen, float4 & fSampleBlue,
                           const float4 & fShadowRed, const float4 & fShadowGreen, const float4 & fShadowBlue);

  /// Change the color of the current sample if it intersects with the shadow
  inline void ShadowHalfPlane(const float4 & fAbsDistance, const float4 & fHalfThickness, const float4 & fFalloff,
                              float4 & fSampleRed, float4 & fSampleGreen, float4 & fSampleBlue,
                              const float4 & fShadowRed, const float4 & fShadowGreen, const float4 & fShadowBlue);

  /// Perform shadowing of the endoscopic view
  inline void AddShadows(const bool &flc_bObliqueShadow, const bool &flc_bGridShadow, const bool &flc_bAxialShadow,
                         const bool &flc_bSagittalShadow, const bool &flc_bCoronalShadow, const bool &flc_bSegmentHalfPlaneShadow,
                         const Point<float4> &vPoint,
                         const Normal<float4> flc_vObliqueNormal,
                         const float4 flc_fObliqueMinDistance,
                         const float4 flc_fObliqueMaxDistance,
                         const Point<float4> flc_vOrthogonalPoint,
                         float4 &fSampleRed, float4 &fSampleGreen, float4 &fSampleBlue);

  /// Get the color of the current sample from the painted volume
  inline void SamplePaintVol(const uint1 * const &flc_puPaintVol, const uint1 flc_uCurrentSeenVal,
                             const uint4 &flc_uXStep, const uint4 &flc_uYStep,
                             const uint4 &flc_uZStep, const uint4 &uIndexOffset,
                             const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                             const float4 &flc_fNotSeenRed, const float4 &flc_fNotSeenGreen, const float4 &flc_fNotSeenBlue,
                             const float4 &flc_fSeenRed, const float4 &flc_fSeenGreen, const float4 &flc_fSeenBlue,
                             float4 &fSampleRed, float4 &fSampleGreen, float4 &fSampleBlue);

  /// Update the color of the current sample according to the test volume.
  inline void SampleTestVol(const uint1 * const flc_puTestVol, const uint1 flc_uTestThreshVal,
                            const uint4 &flc_uXStep, const uint4 &flc_uYStep,
                            const uint4 &flc_uZStep, const uint4 &uIndexOffset,
                            const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                            const float4 &flc_fRed, const float4 &flc_fGreen, const float4 &flc_fBlue,
                            float4 &fSampleRed, float4 &fSampleGreen, float4 &fSampleBlue, float4 &fOpacity);

  /// Calculate the gradient vector of the volume
  inline void CalcCentralDifferenceGradient(const uint1 * const &puVol, const uint4 &flc_uXStep,
                                            const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                            const uint4 &flc_uXYStep, const uint4 &flc_uYZStep,
                                            const uint4 &flc_uXZStep, const uint4 &flc_uXYZStep,
                                            const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                            Vector<float4> &vGradVector);

  /// Calculate the gradient vector of the volume
  inline void CalcCentralDifferenceGradient(Point<uint2> &vVolCoord,
                                            const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                            Vector<float4> &vGradVector);

  /// Calculate the phong lighting equation for two lights
  inline void ShadeTwoLightPhong(const Normal<float4> &vLight1Normal, const Normal<float4> &vLight2Normal,
                                 Vector<float4> &vGradVector, const Vector<float4> &vHalfwayVector,
                                 float4& fSampleRed, float4& fSampleGreen, float4 &fSampleBlue);

  /// All the work for each ray (assumes the ray is inside the volume)
  inline void CastSingleRay(const uint4 &u, const uint4 &v,
                            const bool &flc_bObliqueShadow, const bool &flc_bGridShadow, const bool &flc_bAxialShadow,
                            const bool &flc_bSagittalShadow, const bool &flc_bCoronalShadow, const bool &flc_bSegmentHalfPlaneShadow,
                            const bool &flc_bShadow, const bool &flc_bBiopsyMode, const bool &flc_bTestVol,
                            const bool &flc_bRaycastPaintVol,
                            const bool &flc_bSpaceLeaping,
                            const bool &flc_bUncleansed,
                            const Normal<float4> flc_vObliqueNormal,
                            const float4 flc_fObliqueMinDistance,
                            const float4 flc_fObliqueMaxDistance,
                            const Point<float4> flc_vOrthogonalPoint,
                            Normal<float4> &vRayNormal,
                            Normal<float4> &vLight1Normal,
                            Normal<float4> &vLight2Normal,
                            Point<float4> &vPoint,
                            float4 &fLeapDistance,
                            const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                            const uint4 &flc_uXYStep, const uint4 &flc_uYZStep, const uint4 &flc_uXZStep,
                            const uint4 &flc_uXYZStep,
                            const uint4 &flc_uXStep3, const uint4 &flc_uYStep3, const uint4 &flc_uZStep3,
                            const uint4 &flc_uXYStep3, const uint4 &flc_uYZStep3, const uint4 &flc_uXZStep3,
                            const uint4 &flc_uXYZStep3,
                            const float4 &flc_fXScl, const float4 &flc_fYScl, const float4 &flc_fZScl,
                            const uint4 &flc_uVolMaxX, const uint4 &flc_uVolMaxY, const uint4 &flc_uVolMaxZ,
                            const uint1 * const flc_puECVvol,
                            const uint1 * const flc_puSLDvol,
                            const uint1 * const flc_puTestVol,
                            uint1 * const &flc_puPaintVol, const uint1 &flc_uCurrentSeenVal,
                            const float4 &flc_fNotSeenRed, const float4 &flc_fNotSeenGreen, const float4 &flc_fNotSeenBlue,
                            const float4 &flc_fSeenRed, const float4 &flc_fSeenGreen, const float4 &flc_fSeenBlue,
                            const float4 * &flc_pfOpacitySurfaceTransferFunction,
                            const float4 * &flc_pfRedBiopsyTransferFunction, const float4 * &flc_pfGreenBiopsyTransferFunction,
                            const float4 * &flc_pfBlueBiopsyTransferFunction, const float4 * &flc_pfOpacityBiopsyTransferFunction,
                            float4 &fRedValue, float4 &fGreenValue, float4 &fBlueValue, float4 &fAlphaValue, float4 &fDepthValue,
                            const float4 &flc_fDfbScale,
                            const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                            uint4 &uSurfaceCount,
                            Vector<float4> &vGradVector, Vector<float4> &vHalfwayVector, Vector<float4> &vIntervalVector,
                            const bool bReverseNormal = false);

  /// Special case for external raycasting (still assumes the ray is inside the volume)
  inline bool CastSingleRaySp(const uint4 &u, const uint4 &v,
                              const bool &flc_bSpaceLeaping,
                              Normal<float4> &vRayDirection,
                              Point<float4> &point,
                              float4 &leapDistance,
                              const uint4 &flc_xStep, const uint4 &flc_yStep, const uint4 &flc_zStep,
                              const uint4 &flc_xyStep, const uint4 &flc_yzStep, const uint4 &flc_xzStep,
                              const uint4 &flc_xyzStep,
                              const float4 &flc_xScl, const float4 &flc_yScl, const float4 &flc_zScl,
                              const uint4 &flc_volMaxX, const uint4 &flc_volMaxY, const uint4 &flc_volMaxZ,
                              uint1 * const flc_pSLDvol,
                              unsigned char * const flc_pOpacityVol,
                              float4 &alphaValue, float4 &depthValue,
                              const float4 &flc_dfbScale,
                              const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                              Vector<float4> &vInterval);

  /// Intersect a ray with the volume dataset if it starts outside the volume
  inline bool CastSingleRayExternal(Point<float4>& ptWC,
                                    const Point<float4>& viewpointVC,
                                    Normal<float4>& vRayDirection,
                                    bool bOutside,
                                    const int4 vol_min_x,
                                    const int4 vol_min_y,
                                    const int4 vol_min_z,
                                    const int4 vol_max_x,
                                    const int4 vol_max_y,
                                    const int4 vol_max_z,
                                    int4 flagX, int4 flagY, int4 flagZ,
                                    const float4 flc_xScl,
                                    const float4 flc_yScl,
                                    const float4 flc_zScl,
                                    const float4 v2wXScl, const float4 v2wYScl, const float4 v2wZScl,
                                    uint4 pixelOffset,
                                    uint4 depthOffset,
                                    float4& firstLeapDistance,
                                    const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                                    uint1 * const flc_pSLDvol,
                                    const uint4 flc_xStep,
                                    const uint4 flc_yStep,
                                    const uint4 flc_zStep,
                                    const float4 flc_dfbScale);

  ///=============================================================================
  /// Please see the notes in the VolumeRender.inl
  /// All the work for each ray (assumes the ray is inside the volume)
  inline void CastSingleRayForUDview(const uint4 &u, const uint4 &v,
                            const bool &flc_bObliqueShadow, const bool &flc_bGridShadow, const bool &flc_bAxialShadow,
                            const bool &flc_bSagittalShadow, const bool &flc_bCoronalShadow, const bool &flc_bSegmentHalfPlaneShadow,
                            const bool &flc_bShadow, const bool &flc_bBiopsyMode, const bool &flc_bTestVol,
                            const bool &flc_bRaycastPaintVol,
                            const bool &flc_bSpaceLeaping,
                            const bool &flc_bUncleansed,
                            const Normal<float4> flc_vObliqueNormal,
                            const float4 flc_fObliqueMinDistance,
                            const float4 flc_fObliqueMaxDistance,
                            const Point<float4> flc_vOrthogonalPoint,
                            Normal<float4> &vRayNormal,
                            Normal<float4> &vLight1Normal,
                            Normal<float4> &vLight2Normal,
                            Point<float4> &vPoint,
                            float4 &fLeapDistance,
                            const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                            const uint4 &flc_uXYStep, const uint4 &flc_uYZStep, const uint4 &flc_uXZStep,
                            const uint4 &flc_uXYZStep,
                            const uint4 &flc_uXStep3, const uint4 &flc_uYStep3, const uint4 &flc_uZStep3,
                            const uint4 &flc_uXYStep3, const uint4 &flc_uYZStep3, const uint4 &flc_uXZStep3,
                            const uint4 &flc_uXYZStep3,
                            const float4 &flc_fXScl, const float4 &flc_fYScl, const float4 &flc_fZScl,
                            const uint4 &flc_uVolMaxX, const uint4 &flc_uVolMaxY, const uint4 &flc_uVolMaxZ,
                            const uint1 * const flc_puECVvol,
                            const uint1 * const flc_puSLDvol,
                            const uint1 * const flc_puTestVol,
                            uint1 * const &flc_puPaintVol, const uint1 &flc_uCurrentSeenVal,
                            const float4 &flc_fNotSeenRed, const float4 &flc_fNotSeenGreen, const float4 &flc_fNotSeenBlue,
                            const float4 &flc_fSeenRed, const float4 &flc_fSeenGreen, const float4 &flc_fSeenBlue,
                            const float4 * &flc_pfOpacitySurfaceTransferFunction,
                            const float4 * &flc_pfRedBiopsyTransferFunction, const float4 * &flc_pfGreenBiopsyTransferFunction,
                            const float4 * &flc_pfBlueBiopsyTransferFunction, const float4 * &flc_pfOpacityBiopsyTransferFunction,
                            float4 &fRedValue, float4 &fGreenValue, float4 &fBlueValue, float4 &fAlphaValue, float4 &fDepthValue,
                            const float4 &flc_fDfbScale,
                            const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                            uint4 &uSurfaceCount,
                            Vector<float4> &vGradVector, Vector<float4> &vHalfwayVector, Vector<float4> &vIntervalVector,
                            const bool bReverseNormal = false);
///==============================================================================

public:

  /// Field of view for rendering.
  static float4 m_fFovDegrees;

  /// Performance Measurement
  uint4  m_uRayCount;
  /// Performance Measurement
  uint4  m_uSampleCount;
  /// Performance Measurement
  uint4  m_uSpaceLeapCount;
  /// Performance Measurement
  float4 m_fSumSpaceLeapDistance;
  /// Performance Measurement
  float4 m_fMaxSpaceLeapDistance;

  /// Performance Measurement
  Timer m_frameRateTimer;
  /// Performance Measurement
  float4 m_fFramesPerSecond;
  /// Performance Measurement
  uint4 m_uRenderSizeSum;
  /// Performance Measurement
  uint4 m_uRenderSizeCount;

  /// time taken render frame (including other views i.e. overview, slices)
  float4 m_fLastRenderTime;

  /// e.g mercator
  ProjectionEnum m_eProjectionType;

  /// For moving lights.
  float4 m_fMoveLightVertDirection;

  /// For moving lights.
  float4 m_fMoveLightHorizDirection;

  /// Upper left corner of the translucent display window
  Point<int4> m_magicWindowStartPt;

  /// Size of the translucent display window
  uint4 m_uMagicWindowSize;

  /// Removes temporal anti-aliasing
  Jitter m_jitter;

private:

  /// Length of each step along the ray
  static const float4 m_fInterSampleDistance;

  /// Opacity value above which a ray is consdered "virtually opaque"
  static const float4 m_fOpacityThreshold;

  /// Opacity value below which a sample is consdiered "virtually clear"
  static const float4 m_fMinOpacity;

  /// Ambient coefficient for lighting
  static const float4 m_fKA;

  /// Diffuse coefficient for lighting
  static const float4 m_fKD;

  /// Specular coefficient for lighting
  static const float4 m_fKS;

  /// Specular "sharpness"
  static const float4 m_fSpecularPower;

  /// Distance from shadow plane
  static const float4 m_fHalfShadowThickness;

  /// Distance from the eye that counts towards visualized surface counting
  static const float4 m_fDepthVisibilityThresh;

  /// "Fudge factor" to keep from leaping into the wall
  //static const float4 m_fDfbSpaceLeapConservationFactor;

  /// Leap distance below which it is more efficient to sample
  //static const float4 m_fDfbSpaceLeapThreshold;

  /// Leap distance below which it is more efficient to sample
  static const uint4  m_uColor_Threshold;

  /// Leap distance below which it is more efficient to sample
  static const float4 m_fDepth_Threshold;

  /// Minimum render size
  static int4 m_iMinRenderSize;

  /// Maximum render size
  static int4 m_iMaxRenderSize;

  /// Minimum frame rate
  static float4 m_fMinFrameRate;

  /// Maximum frame rate
  static float4 m_fMaxFrameRate;

  /// render threads (multi-processing rendering)
  std::vector< RenderThread * > m_threads;

  /// this is the only "writable" value shared between the two rendering threads, these two remove the need for locks
  std::vector< uint4 > m_surfaceCounts;

  /// Image from ray casting (which is usually smaller than that displayed)
  uint1  * m_puImageBuffer;

  /// Depth value for each pixel in m_pImageBuffer
  float4 * m_pfDepthBuffer;

  /// Number of rays cast in m_pImageBuffer (always square)
  uint4   m_uRenderSize;

  /// Size of m_pImageBuffer (always a power of 2 for texturing)
  uint4   m_uRenderBufferSize;

  /// Ratio of m_uRenderSize to m_uRenderBufferSize
  float4 m_fRenderBufferRatio;

  /// Size of the final image displayed to the screen
  uint4  m_uDisplaySize;

  /// Lessens the temporal aliasing from jittering
  uint2 * m_puAccumulationBuffer; // accumulation buffer

  /// Number of images accumulated into the buffer so far
  int4    m_iAccumulationCount;

  /// Uniquely identifies a view position and orientation
  uint4 m_uAccumulationViewIdentifier;

  /// The ecv-vol difference volume.
  vxBlockVolume<uint1> * m_pDiffVolume;

  /// An iterator for the difference volume
  vxBlockVolumeIterator<uint1> * m_pDiffVolumeIterator;

  /// The background value of the difference volume
  uint1 m_pDiffVolumeBackground;

  /// current image mode
  ImageModeEnum m_eImageMode;

}; // class VolumeRender

} // namespace ReaderLib


// $Log: VolumeRender.h,v $
// Revision 1.7.2.2  2010/02/01 19:33:15  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.1  2007/05/03 15:23:59  dongqing
// add function CastSingleRayForUDview() to fix lumen coverage problem
//
// Revision 1.7  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.6  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.5  2005/10/28 13:32:04  geconomos
// added ability to render into a fixed or adpative image buffer
//
// Revision 1.4  2005/10/25 13:48:11  geconomos
// reworked threading to support more than 4 threads
//
// Revision 1.3  2005/10/13 18:11:31  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.2  2005/09/29 13:33:13  vxconfig
// made the difference volume a new vx block volume
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/09/09 14:27:16  geconomos
// Issue# 4349: Image resolution not increasing at higher framerates.
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.24.2.5.4.3  2005/06/14 19:13:26  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.24.2.5.4.2  2005/06/10 18:19:41  geconomos
// Added mechansim to render a high quality image immediately after flight navigation has stopped.
//
// Revision 3.24.2.5.4.1  2005/04/22 01:36:14  frank
// Issue #4108: Re-working 2D/3D projection
//
// Revision 3.24.2.5  2003/05/22 19:59:43  kevin
// Tracker issue 2784: This is now fixed correctly. The  system will now use
// more memory and the external DFB volume is sotred in it's own volume
// so the internal and enternal will not "leak" together.
//
// This also indirectly fixes the black holes problem
//
// Revision 3.24.2.4  2003/04/04 20:13:35  kevin
// Issue 2835: The threads were crashing on creatiion. 2 things were
// done to fix this:
// (1) Handle rendierng if the threads are non-existant by switching
// to sequential rendering (this should have been in there nayway)
// (2) Create the threads early when there is plenty of memory blocks
//
// Revision 3.24.2.3  2003/03/31 18:00:40  jmeade
// Unused class.
//
// Revision 3.24.2.2  2003/02/21 21:40:01  jmeade
// One-line FlushAccumulationBuffer() method made inline.
//
// Revision 3.24.2.1  2003/02/21 19:14:56  jmeade
// One-line FlushAccumulationBuffer() method made inline.
//
// Revision 3.24  2003/01/22 22:30:13  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.23  2001/01/09 17:36:21  kevin
// Adjust Debug output for endoscopic frame rate to also include
// adaptive render size for the current frame and the average since the
// viewer started.
//
// Revision 3.22  2002/12/24 03:47:47  jmeade
// Bug: Measure list not saved in session b/c there were two copies of it!
//
// Revision 3.21  2002/12/23 22:28:09  kevin
// Adjust the rendersize based on the frame rate
//
// Revision 3.20  2002/11/24 17:43:57  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.19  2002/11/11 14:26:57  kevin
// Dim the border for segmetnal view
//
// Revision 3.18  2002/09/02 20:38:14  kevin
// Render a specific sized segment and mark which part the user should focus on
//
// Revision 3.17  2002/08/28 19:29:09  kevin
// Lots of updates to segmental viewing
//
// Revision 3.16  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.15  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.14  2002/04/12 02:20:41  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.13  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.12.2.3  2002/02/26 16:54:32  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.12.2.2  2002/02/10 20:22:29  kevin
// Major fixes for TTP rendering algorithm
//
// Revision 3.12.2.1  2002/02/06 23:52:51  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.12  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.11  2002/01/04 16:33:19  kevin
// Some cleanup after the hacking to get TTP rendering without ses,set,sec
//
// Revision 3.10  2002/01/03 02:30:28  kevin
// Cleanup related to new TTP work when ses, set and sec went away
//
// Revision 3.9  2001/12/28 19:51:54  ingmar
// moved Sec_ReaderLib to ReaderLib.h
//
// Revision 3.8  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.7  2001/11/07 16:57:37  binli
// Added a new version to get uncleansed sample method which utilizes the diff volume
// (position in Volume Coordinate/check if position is inside volume)
//
// Revision 3.5  2001/11/06 15:25:02  kevin
// Added a get uncleansed sample method which utilizes the diff volume
//
// Revision 3.4  2001/11/02 16:13:55  binli
// 2D render from block vol (.diff)
//
// Revision 3.3  2001/01/13 09:47:40  kevin
// Fixed the bug in uncleased rendering where both supine and prone datasets
// pointed the curent study for the block diff volume
//
// Revision 3.2  2001/11/01 19:52:59  kevin
// Added 3D Uncleansed DIff-Block_volume rendering
//
// Revision 3.1  2001/10/25 13:19:46  kevin
// Added hooks for avi/mpeg saving
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 10 2001 11:52:12   binli
// issue 1027: transparent rendering of overview reflect selection
// 
//    Rev 2.1   Oct 08 2001 21:37:52   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
// 
//    Rev 2.0   Sep 16 2001 23:40:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:22   ingmar
// Initial revision.
// Revision 1.46  2001/05/29 17:25:40  jmeade
// Fixed changes from (and continued) code review, particularly with translucent window
//
// Revision 1.45  2001/05/25 20:28:59  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.44  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.43  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.42  2001/04/29 22:42:41  kevin
// Variable naming for coding standards in VolumeRender Class
//
// Revision 1.41  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.40  2001/04/09 18:40:04  ingmar
// setup doxygen documentaion
//
// Revision 1.39  2001/04/05 23:24:40  jmeade
// use persistent thread for rendering
//
// Revision 1.38  2001/03/21 14:59:49  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.37  2001/02/12 21:17:52  liwei
// TTP rendering quality improved, with a little speed sacrifice
//
// Revision 1.36  2001/02/05 22:18:34  liwei
// Minor change, playing with jitter
//
// Revision 1.35  2001/02/02 12:43:01  frank
// Added space leaping threshold as member.  Tried space leaping with a higher threshold and no stepping back.
//
// Revision 1.34  2001/01/24 19:18:20  liwei
// Software volume rendering for overview
//
// Revision 1.33  2001/01/24 15:55:21  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.32  2001/01/23 18:26:09  frank
// Consolidated point data into Point<> data.
//
// Revision 1.31  2001/01/23 18:16:59  frank
// Saved a unique view identifier for view caching purposes.
//
// Revision 1.30  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.29  2000/12/20 14:34:38  kevin
// Initial chnges to make mpr viewer and slice viewer one 2d slice viewer class
//
// Revision 1.28  2000/12/19 14:56:35  kevin
// Added Grid Shadow
//
// Revision 1.27  2000/12/18 23:38:40  kevin
// Removed 2D unwrap-painting and clenaed up it's memory and flags.
// (NOTE; This seemed to be causing a weird memory crash where
// there was memory, but the call to new was creating an out-of-mem
// exception and there was plenty of memory left. Since this code never
// caused problems before, I am concerned that this is really some
// other problem rearing it's ugly head)
//
// Revision 1.26  2000/12/11 14:44:58  frank
// Added toggle for main image display
//
// Revision 1.25  2000/11/20 19:07:00  kevin
// Made INS block Again (finally not being used for painting)
//
// Revision 1.24  2000/11/20 15:55:30  frank
// Enabled toggling accumulation buffer
//
// Revision 1.23  2000/11/20 03:37:02  kevin
// Switched to "paint volume" schema.
//
// Revision 1.22  2000/11/19 23:47:05  kevin
// Changed RGBvol to PaintVol to reflect new meaning
//
// Revision 1.21  2000/11/18 18:38:22  kevin
// Removed #if for sld counting and painting (didn't work)
//
// Revision 1.20  2000/11/17 21:15:56  kevin
// FIxed some errors from changing the rendersize to be non-power-of-two
//
// Revision 1.19  2000/11/17 04:23:02  kevin
// Added resizing of raycasting window to non-power-of-two
//
// Revision 1.18  2000/11/16 22:15:32  kevin
// Begin to handle non-power-of-2 rendering
//
// Revision 1.17  2000/11/16 20:47:16  kevin
// Made some volume render stuff inline
//
// Revision 1.16  2000/11/15 14:04:11  frank
// Softened the slice shadow images
//
// Revision 1.15  2000/11/14 19:25:44  frank
// Improved the speed of the accumulation buffer
//
// Revision 1.14  2000/11/14 14:28:08  frank
// Added accumulation buffer
//
// Revision 1.13  2000/11/14 04:37:20  kevin
// Removed some uneeded variables (hopefully many more to come)
//
// Revision 1.12  2000/11/14 02:23:12  kevin
// Created inline CastSingleRay and numerous inlines for the pieces within
//
// Revision 1.11  2000/11/10 20:31:55  kevin
// no message
//
// Revision 1.10  2000/11/02 17:36:31  kevin
// Moved RGBVol to public so painting overview works
//
// Revision 1.9  2000/10/31 23:35:02  liwei
// Hybrid view added
//
// Revision 1.8  2000/10/31 14:37:41  frank
// Merged in new segment structure
//
// Revision 1.7  2000/10/30 13:36:40  kevin
// Added MOving the Magic Window around
//
// Revision 1.6  2000/10/29 17:40:26  kevin
// Added two surface count variables (So we don't have to
// put mutex locks around the critical increment code
// so that it remains quick. This is possible since we are forcing
// onlly ever 2 PEs)
//
// Revision 1.5  2000/10/27 20:36:58  michael
// add adaptive sampling for rendering
//
// Revision 1.4  2000/10/27 01:12:26  kevin
// Lots of changes to Marking
//
// Revision 1.3.2.1  2000/10/26 13:28:33  frank
// Added new segment structure
//
// Revision 1.3  2000/10/20 17:15:08  kevin
// Added multiple different projection methods
//
// Revision 1.2  2000/09/22 17:47:58  kevin
// Added perfromance measuremnet
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
//
// 25    9/07/00 4:02p Frank
// Added new colon unwrapping features:
// -green composited overlay
// -continuous examination confidence (compared to binary)
// -percent examined
//
// 24    9/06/00 3:12p Frank
// Fixed cvs compile bug
//
// 23    9/06/00 11:12a Kevin
// Fixed marking of unraveled image
//
// 22    9/05/00 1:34a Ingmar
// now works with new naming
//
// 21    9/02/00 4:20p Kevin
// Changed CastRay to work for the unwrapped colon (including setting of
// the (u,v) marking index volumes)
//
// 20    8/28/00 12:53p Antonio
// updated header and footer
//
//*****************  Version 19  *****************
//User: Kevin           Date:  8/18/00  Time: 11:51a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added MPR Measure
//
//*****************  Version 18  *****************
//User: Kevin           Date:  8/14/00  Time:  8:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added RGB marked volume writing for Revli viewing
//
//*****************  Version 17  *****************
//User: Frank           Date:  8/11/00  Time:  2:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated to reflect new Volume classes
//
//*****************  Version 16  *****************
//User: Kevin           Date:  8/09/00  Time: 11:43a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added parallel s/w volume rendering
//
//*****************  Version 15  *****************
//User: Kevin           Date:  8/08/00  Time:  9:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed RearView rendering hooks
//
//*****************  Version 14  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 13  *****************
//User: Kevin           Date:  7/31/00  Time:  3:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added leaps/ray and samples/ray debug
//
//*****************  Version 12  *****************
//User: Frank           Date:  7/26/00  Time: 10:45a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved MPR functionality into VolumeRender
//
//*****************  Version 11  *****************
//User: Frank           Date:  7/25/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added SkeletonRender class.
//Updated with STL string class.
//
//*****************  Version 10  *****************
//User: Kevin           Date:  7/23/00  Time:  4:47p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Better strucure for performing adaptive sampling. This one barely
//slows downat all. SO far only renders small image and does a bilnear blowup, no
//checking for where to cast more rays.
//
//*****************  Version 9  *****************
//User: Kevin           Date:  7/23/00  Time:  3:35p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  First stab at Adaptive Sampling. This version is very slow. I will
//try a second method next.
//
//*****************  Version 8  *****************
//User: Kevin           Date:  7/19/00  Time:  3:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added global Field-Of-View for rear and forward view
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 6  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 6
//Comment:
//
//
//*****************  Version 5  *****************
//User: Kevin           Date:  7/14/00  Time:  4:36p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Made the rear view point backwards and render at half the resolution
//
//*****************  Version 4  *****************
//User: Jeff            Date:  7/14/00  Time:  2:42p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Added preliminaries for forward OR rearview volume rendering
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/14/00  Time:  2:20p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Changed RGBA of floats to RGB of char and moved
//so it doesn't always grab large memory untill needed
//
//*****************  Version 2  *****************
//User: Kevin           Date:  7/13/00  Time: 12:43p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Added mulitple volume rendreing quality debug, perofrmance
//enhancemnts and code clean-ups.
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created VolumeRender.h
//Comment:
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/VolumeRender.h,v 1.7.2.2 2010/02/01 19:33:15 dongqing Exp $
// $Id: VolumeRender.h,v 1.7.2.2 2010/02/01 19:33:15 dongqing Exp $
