// $Id: vxRendererCpu3D.h,v 1.16 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar Bitter (mailto:ingmar@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererSoftware.h"
#include "vxBlockVolume.h"
#include "Timer.h"
#include "Illuminator.h"
#include "vxComponent.h"
#include "Box.h"
#include "Vector3D.h"
#include "Vector4d.h"
#include "vxRenderingMode.h"
#include "vxDataRendererSoftware.h"
#include "Jitter.h"
#include "vxCamera.h"


// forwards
class vxSliceShadowData;
class vxComponent;
template <class T> class vxBlockVolumeIterator;
template <class T> class vxBlockVolumeRayIterator;


/**
 * Software ray casting renderer.
 */
class VX_VIEWER_DLL vxRendererCpu3D : public vxRendererSoftware
{
// inner classes
protected:

  /**
   * structure of information used by each rendering thread.
   */
  struct Args
  {
    /// for multi-threading purposes
    uint1 uThreadID;

    /// number of bits to shift to get to the 8 bit lookup
    int4 iShiftTo8Bit;

    /// colormap, preintegrated 2D
    float4 * pfColormap2d8bitPreIntegrated;

    /// RendererCpu3dData
    vxDataRendererSoftware rendererCpu3dData;

    /// alpha premultiplied sample color
    PixelRGBAf fPreMultSample;

    /// alpha premultiplied accumulated color
    PixelRGBAf fPreMultAccum;

    /// plain rgba display color (a==1)
    PixelRGBAu iDisplayColor;

    /// the foreground color as float
    PixelRGBAf fForegroundColor;

    /// background color as float
    PixelRGBAf fBackgroundColor;

    /// background color as uint1
    PixelRGBAu uBackgroundColor;

    /// the depth at which the ray intersects the slice shadow plane
    float4 fShadowPlaneDepth;

    /// the bounding box for the visible data area
    Box<float4> fCropBox; 

    /// the bounding box for the area of interest
    Box<float4> fBoxOfInterest; 

    /// where the ray first intersects the bounding box in volume coordinates
    Vector4d<float4> fRayIntersectBoundingBoxBgnVC;

    /// where the ray last intersects the bounding box in volume coordinates
    Vector4d<float4> fRayIntersectBoundingBoxEndVC;

    /// the view direction in volume coordinates
    Vector3D<float4> fViewDirectionVC;

    /// the direction of light 2 in volume coordinates
    Vector3D<float4> fLight2DirVC;

    /// the halfway vector of the light in volume coordinates
    Vector3D<float4> fHalfwayLight1VC;

    /// anisotropic correction gradient scaling factor for y
    float4 fGradientScaleY;

    /// anisotropic correction gradient scaling factor for z
    float4 fGradientScaleZ;

    /// voxel units of the intensity volume
    Vector3D<float4> fVoxelUnits;

    /// the reciprocal of the volume units of the intensity volume
    Vector3D<float4> fVoxelUnitsReciprocal;

    /// unused
    float4 fVolumeDiagonal;

    /// pointer to the RGBA image buffer
    uint1 * pImageBuffer;

    /// pointer to the depth buffer
    float4 * pDepthBuffer;

    /// the current offset into the pixel buffer
    int4 iPixelOffset;

    /// the width of the RGBA image
    int4 iImageWidth;

    /// the HALF height of the RGBA image
    int4 iImageHeight;

    /// the current pixel coordinate (u,v)
    int4 u; 

    /// the current pixel coordinate (u,v)
    int4 v;

    /// the ray iterator for the intensity volume
    vxBlockVolumeRayIterator<uint2> * pRayIterIntensity;

    /// the ray iterator for the label volume
    vxBlockVolumeRayIterator<uint2> * pRayIterLabel;

    /// the ray iterator for the space leap distance volume
    vxBlockVolumeIterator<int1>     * pVolIterSLD;

    /// the ray iterator for shadow rays
    vxBlockVolumeRayIterator<uint2> * pRayIterShadow;

    /// the iterator for sampling the label in shadow rays
    vxBlockVolumeIterator<uint2> * pIterLabelShadow;

    /// the distance between samples (MAKE THIS A GLOBAL CONST IF IT DOES NOT CHANGE)
    float4 fInterSampleDistance;

    /// the component array
    const std::vector<vxComponent *> * pvComponents;

    /// the weight of the two gradients on each side of the current sample when using the pre-integrated transfer function
    float4 fGradientWeight;

    /// the sampled intensity data
    int4 iSample;

    /// the index of the last sample into the pre-integrated transfer function lookup table
    int4 iPrevIndex;

    /// the label voxel
    int4 iLabel;

    /// true when the ray should continue traversing
    bool bContinueRay;

    /// true when the intensity range should be blended into the each sample
    bool bIsShowIntensityRange;

    /// true when volume rendering mode is active (not MIP or X-ray)
    bool bIsVRMode;

    /// true when MIP mode is active
    bool bIsMipMode;

    /// true when X-ray mode is active
    bool bIsXRayMode;

    /// the lower range of the blended-in intensity threshold
    int4 iShowIntensityRangeLower;

    /// the upper range of the blended-in intensity threshold
    int4 iShowIntensityRangeUpper;

    /// z-buffer depth
    float4 fDepthToSurface;

    /// labels on top mode
    bool bIsLabelsOnTopMode;

    /// the x transformation from image to viewport coordinates
    float4 fImageToViewportScaleX;

    /// the y transformation from image to viewport coordinates
    float4 fImageToViewportScaleY;

    /// the OpenGL viewport
    GLint viewport[4];

    /// the OpenGL modelview matrix
    GLdouble mvmatrix[16];
    
    /// the OpenGL projection matrix
    GLdouble projmatrix[16];

    /// if this is the first sample
    bool bFirstSample;

    /// if the ray should start sampling at the neighbor depth
    bool bUseNeighborDepth;

    /// the depth of a neighbor
    float4 fNeighborDepth;

  }; // struct Args

// functions
public:

  ///////////////////////////////////////////////////////////////////////
  // constructors and destructors

  /// Constructor
  vxRendererCpu3D(vxEnvironment & environment);

  /// Destructor
  virtual ~vxRendererCpu3D();


  ///////////////////////////////////////////////////////////////////////
  // initialization and cleanup

  /// perform pre-rendering operations
  virtual void Initialize();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform pre-rendering operations
  virtual int4 PreRender();

  /// perform post-rendering operations
  virtual int4 PostRender();


  ///////////////////////////////////////////////////////////////////////
  // making the image

  /// perform rendering operations
  virtual void Render();

  /// perform rendering for a window. This may be a subregion of the image given to a thread.
  virtual int4 RenderWindow(const BoundingRect<uint2> & window, const uint1 & uThreadID);

protected:

  ///////////////////////////////////////////////////////////////////////
  // rendering helper functions

  /// distribute rendering of the current image across threads
  void LaunchThreads();

  /// sets the size of the volume rendered image (the texture content)
  void SetImageSize(int4 iSizeIndex);

  /// create a texture
  virtual void CreateTexture();

  /// set the texture of the image generated by the algorithm
  virtual void DrawTexture();


private:

  ///////////////////////////////////////////////////////////////////////
  // restricted functions

  /// copy constructor
  vxRendererCpu3D(const vxRendererCpu3D &);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererCpu3D & operator=(const vxRendererCpu3D &); // should only be public if really implemented!


protected:

  ///////////////////////////////////////////////////////////////////////
  // Cpu3D renderer specific rendering helper functions 

  /// rendererCpu3dData changed (being sensitive to specific flags only)
  bool IsRendererCpu3dDataChanged();

  /// rendering mode changed (being sensitive to specific flags only)
  bool IsRenderingModeChanged();

  /// cast one ray through the volume
  void CastOneRay(Args & args);

  /// space leap
  virtual void SpaceLeap(Args & args);

  /// space leaps through all 16^3 blocks
  inline void RaySpaceLeapLargeBlocks(Args & args);

  /// space leap over blocks of inivisible labels
  inline void RaySkipDensityInvisible(Args & args);

  /// space leap over blocks of invisible intensity
  inline void RaySkipAlphaInvisible(Args & args);

  /// sample the intensity and label at the current ray position
  inline void RayGetSampleAndLabel(Args & args);

  /// blend the label color over the sample
  inline void RayBlendLabelColor(Args & args);

  /// blend the intensity range over the sample if blending is enabled
  inline void RayBlendIntensityRange(Args & args);

  /// blend the slice shadows over the sample if shadow blending is enabled
  inline void RayBlendSliceShadows(Args & args);

  /// blend the slice shadows at the end of sampling the ray
  inline void RayBlendSliceShadowsAtEndOfRay(Args & args);

  /// retrieve the next sample in MIP projection
  inline void RayNextMipSample(Args & args);

  /// retrieve the next sample in X-ray projection
  inline void RayNextXRaySample(Args & args);

  /// shade the sample
  virtual void RayShadeSample(Args & args);

  /// color the ray based on the gradient direction (debug use only)
  inline void RayColorFromGradientDir(Args & args, const Vector3D<float4> vGradVector3D);

  /// composite the current sample over the accumulated ray
  inline void RayCompositeSample(Args & args);

  /// take the accumulated ray colors and convert them into displayable uint1 pixels
  inline void RaySetFinalColors(Args & args);

  /// composite sampling points along a single ray.
  virtual void SampleSingleRay(Args & args);

  /// decide if the pixels are close enough in color to interpolate rather than raycast
  inline bool IsOkToInterpolate(const uint1 * const pPixelA,  const uint1 * const pPixelB1,
                                 const uint1 * const pPixelB2, const uint1 * const pPixelB3,
                                 bool & bPixelsAreDifferent);

  /// test if Pixels are similar enough to just interpolate intead of sending a ray.
  bool IsPixelsAreSimilar(const uint1 * const pPixelA,  const uint1 * const pPixelB1,
                           const uint1 * const pPixelB2, const uint1 * const pPixelB3);

  /// test if Pixels are similar enough to just interpolate intead of sending a ray.
  bool IsPixelTransparent(const unsigned char * pPixelA);

  /// interpolates between two pixels.
  void PixelInterpolate(const uint1 * const pPixelA, const uint1 * const pPixelB, uint1 * pPixelC);

  /// draw progress of (threaded) rendering
  void DrawProgressBar();

  /// fills in all render arguments that are common amoung all threads
  void SetCommonRenderArguments();

  ///////////////////////////////////////////////////////////////////////
  // update state after environment change

  /// calculates the box of interest
  void UpdateBoxOfInterest();

  /// update the mask for intensities
  bool UpdateMaskForIntensities();

  /// update the volume histogram
  bool UpdateMaskForLabels();

  /// update render environment according to new transfer function
  void UpdateRenderVolumeIntensity();

  /// updates the render label volume according to current label volume
  void UpdateRenderVolumeLabel();

  /// update the volume for space leaping
  void UpdateVolumeSpaceLeap();

  /// updates the preintegrated table
  void UpdateTFunc2D();

  /// writes out the preintegrated table to a file
  void WriteTFunc1Dand2D();

  /// clamp the intensity volume to the valid range
  void ClampIntensityVolume();


  ///////////////////////////////////////////////////////////////////////
  // debug image creation

  /// draws the adaptive sampling pattern
  void DrawAdaptiveSamplingPatternPixel(Args & args, int4 u, int4 v, uint1 eSamplingInfo);

  /// draws space leap pattern line
  void DrawSpaceLeapPatternLine(Args & args, uint1 eSamplingInfo, bool bNeedToUpdateLabelPos = false);

// data
protected:

  ///////////////////////////////////////////////////////////////////////
  // progressive refinement

  /// resolution dependent scaling of sample distance
  float4 m_fQualitySampleDistanceScaleResolutionDependent;

  /// if progressive refinement is currently ongoing
  bool m_bIsProgressiveRefinement;

  /// jitter object
  Jitter m_jitter;

  /// is there any thread working
  bool m_bAnyThreadBusy;

  /// are we in the first step of refinement (after starting it)
  bool m_bIsFirstRefinementStep;

  /// was the previous image the first step of refinement
  bool m_bIsFirstRefinementStepPrevious;

  /// time of last frame
  float4 m_fLastFrameTime;

  /// if rendering should be cancelled at the next opportune moment
  bool m_bCancelRendering;

  /// if we need to start the quality-speed tradeoff timer
  bool m_bNeedToStartTimer;

  /// the vector of image sizes
  std::vector<int4> m_viImageSize;

  /// indicate if the frame rate should be adjusted
  bool m_bIsAdjustFrameRate;

  /// dynamically adjusting image size setting
  int4 m_iImageSizeIndexResponsive;

  /// the current index into the image size setting
  int4 m_iImageSizeIndex;

  /// maximum index for image size
  int4 m_iImageSizeIndexMax;

  ////////////////////////////////////////////////////////////////////////
  // progress metering

  /// number of threads used during progressive refinement
  uint1 m_uCurrentlyUsedThreads;

  /// overall progress [0,1]
  float4 m_fRenderingProgress;

  /// the fraction of rays that compose the border
  float4 m_fFractionRaysBorder;

  /// the fraction of rays that fill the interior
  float4 m_fFractionRaysFill;

  /// the fraction of rays that will probably be missed and need to be filled in later
  float4 m_fFractionRaysMissed;

  /// the fraction of rays that will be necessary to render the labels on top
  float4 m_fFractionRaysLabelsOnTop;

  ///////////////////////////////////////////////////////////////////////
  // cached visualization settings

  /// if slice shadows should to clip away the volume
  bool m_bClipSliceShadow;

  /// slice shadow position
  bool m_bIsSliceShadowEnabled;

  /// slice shadow position
  int4 m_iSliceShadowOrientation;

  /// whether to show the intensity range
  bool m_bIsShowIntensityRange;

  /// if pre integration
  bool m_bPreIntegration;

  /// if sampling every pixel
  bool m_bSampleEveryPixel;

  /// if shading
  bool m_bShading;

  /// if shadowing
  bool m_bShadowing;

  /// if jittering
  bool m_bJittering;

  /// if showing cuts shaded
  bool m_bShowCutsShaded;

  /// if showing cuts in grayscale
  bool m_bShowCutsGrayscale;

  /// rendering mode
  vxRenderingMode m_renderingMode;

  /// rendering mode previous
  vxRenderingMode m_renderingModePrevious;

  /// rendererCpu3dData
  vxDataRendererSoftware m_rendererCpu3dData;

  /// rendererCpu3dData previous
  vxDataRendererSoftware m_rendererCpu3dDataPrevious;

  /// slice shadow information
  vxSliceShadowData * m_pSliceShadowData;

  ///////////////////////////////////////////////////////////////////////
  // local volumes

  /// intensity volume
  vxBlockVolume<uint2> m_intensityRenderVol;

  /// label volume
  vxBlockVolume<uint2> m_labelRenderVol;

  /// space Leap volume
  vxBlockVolume<int1> m_spaceLeapInfoVol;


  ///////////////////////////////////////////////////////////////////////
  // local evironment element copies

  /// copy of the component vector
  std::vector<vxComponent *> m_vComponents;

  /// copy of camera
  vxCamera m_camera;


  ///////////////////////////////////////////////////////////////////////
  // masks for space leap creation

  /// indicates which intensities are not set by the transfer function
  int4 m_iIsVisibleMaskIntensity;

  /// indicates which labels are invisible
  int4 m_iIsVisibleMaskLabel;


  ///////////////////////////////////////////////////////////////////////
  // quality-speed tradeoff timers

  /// a way to get unique timer names for each instance of the class
  static uint4 m_uNumberOfTimers;

  /// the name of the quality-speed tradeoff timer for this instance
  std::string m_sTimerName;

  ///////////////////////////////////////////////////////////////////////
  // crop box, BoxOfInterest

  /// indicates if the crop box was active in the previous frame
  bool m_bIsCropBoxActive;

  /// indicates if the box of interest was active in the previous frame
  bool m_bIsBoxOfInterestActive;

  /// crop box of the visible voxels in VolumeIndexCoordinates
  Box<float4> m_fVisibilityBox;

  /// box of interest (all voxels outside are decreased in intensity)
  Box<float4> m_fBoxOfInterest;


  ///////////////////////////////////////////////////////////////////////
  // sampling and illumination

  /// the illuminator for Phong lighting
  Illuminator m_illuminator;

  /// the intersample distance along a ray
  float4 m_fInterSampleDistance;

  /// colormap, 1D
  float4 * m_pfColormap1d;
  
  /// colormap, 1D alpha corrected
  float4 * m_pfColormap1dAlphaCorrected;

  /// colormap, 1D, 8bit
  float4 * m_pfColormap1d8bit;

  /// colormap, 1D 8bit, alpha corrected
  float4 * m_pfColormap1d8bitAlphaCorrected;

  /// colormap, preintegrated 1D
  float4 * m_pfColormap1d8bitPreIntegrated;

  /// colormap, preintegrated 2D
  float4 * m_pfColormap2d8bitPreIntegrated;

  /// colormap, preintegrated 2D
  float4 * m_pfColormap2d8bitPreIntegratedOutOf1d;

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

  /// the common rendering arguments
  Args m_commonRenderArguments;

  /// if the intensity volume has been clamped yet
  bool m_bIntensityVolumeClamped;

  /// was momentum on previously
  bool m_bPreviousMomentum;

}; // class vxRendererCpu3D


// $Log: vxRendererCpu3D.h,v $
// Revision 1.16  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.15  2006/04/10 19:45:52  frank
// rendering acceleration 30%+
// leaped to min distance of nearest left and right eighth column in scanline as well to above and below pixels
//
// Revision 1.14  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.13  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.12  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.11.2.2  2005/07/19 15:33:17  frank
// fixed some rendering bugs
//
// Revision 1.11.2.1  2005/07/08 15:01:23  frank
// added rendering option for unshaded and uncolored edge, crop, slice cuts
//
// Revision 1.11.6.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.11  2005/05/20 12:13:18  frank
// cleaning up slice shadows in 3D
//
// Revision 1.10  2004/10/25 23:49:29  michael
// made RayShadeSample() virtual
//
// Revision 1.9  2004/10/05 14:29:13  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.8  2004/09/21 15:51:02  frank
// fixed rendering at thread boundaries
//
// Revision 1.7  2004/09/09 14:55:24  michael
// Added frame-time to vxDataRenderer
//
// Revision 1.5  2004/08/05 13:56:15  michael
// Took out du and dv in rendering arguments. Was never used.
//
// Revision 1.4  2004/05/20 12:50:13  frank
// removed UpdateBoundingBox and moved functionality to a controller
//
// Revision 1.3  2004/05/18 15:01:42  vxconfig
// renamed lookup tables, cleaned up the writing of lookup tables, and added
// initialization for sample and previosu sample is args.
//
// Revision 1.2  2004/03/26 21:16:04  frank
// fixed rendering timers so that two renderers can co-exist in a single environment without stepping on each other with the timers
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.155  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.154  2003/11/16 23:41:14  michael
// Changed frame-rate adjustment to allow for quality increase if possible
//
// Revision 1.153  2003/09/29 17:19:37  michael
// accounting for the new event flags in vxEnvironment as well as starting
// progressive refinement when it is switched off.
//
// Revision 1.152  2003/09/22 16:01:17  michael
// modified the refinement to be triggered on IsModified() of the EventTimer and
// not the RendererCpu3dData. RendererCpu3dData is now used to set parameters
// for the 3D rendering such as jittering, shading, preintegration, and sampling
// rate.
//
// Revision 1.151  2003/09/12 14:24:41  frank
// Implemented the oblique slice shadow
//
// Revision 1.150  2003/09/08 17:42:01  frank
// Added slice shadow clipping
//
// Revision 1.149  2003/08/27 14:37:32  michael
// made 3D renderer more sophisticated with respect to which rendering mode
// flags it is sensitive to
//
// Revision 1.148  2003/08/01 21:34:58  michael
// renamed TFunc to Colormap since this is what we call it (non academic)
//
// Revision 1.147  2003/08/01 12:25:15  michael
// removed logs, set responsive size to lowest, and added frame time log flag (global)
//
// Revision 1.146  2003/07/24 17:56:22  michael
// Changed multi-threaded rendering by creating the first image in blocking
// fashion so that there is an immediate image and the subsequent refinement
// images are created in the background. Also added that the thread priority
// is set to low if it is not the active viewer and to normal if it is the active viewer.
// This way, the rendering of multiple 3D images is intuitively prioritized.
//
// Revision 1.145  2003/07/15 12:12:01  frank
// Issue #3303: Improved quality of x-ray rendering, allowed inverting colors
//
// Revision 1.144  2003/07/07 12:40:06  frank
// Improved x-ray rendering
//
// Revision 1.143  2003/06/20 17:09:57  frank
// Fixed MIP rendering for increased quality
// unified calls to 8-bit-shift function
// cleaned out dead code
// took global, unchanging values out of args and into member variables
//
// Revision 1.142  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.141  2003/06/09 18:13:53  michael
// fixed last hard coded bit-shift in raycaster (hopefully)
//
// Revision 1.140  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.139  2003/06/03 19:03:29  frank
// code review
//
// Revision 1.138  2003/05/19 14:38:33  frank
// Removed unused return parameter
//
// Revision 1.137  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.136  2003/05/13 14:25:30  frank
// code review in progress
//
// Revision 1.135  2003/04/07 18:25:15  michael
// fixed background color for MPR views
//
// Revision 1.134  2003/01/27 21:28:53  ingmar
// added capability to have non-black background
//
// Revision 1.133  2003/01/23 21:21:57  ingmar
// reduced slice shadow intensity outside visible data to 50%
//
// Revision 1.132  2003/01/23 17:14:23  ingmar
// brought slice shadows back in front of background
//
// Revision 1.131  2003/01/22 22:01:11  ingmar
// better variable naming
//
// Revision 1.130  2002/12/23 21:43:15  frank
// Added x-ray projection.
//
// Revision 1.129  2002/12/20 19:02:13  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.128  2002/12/19 21:26:00  ingmar
// HitBoundingBox now takes a Box and not 2 vectors as box argument
//
// Revision 1.127  2002/12/19 20:45:08  ingmar
// PixelRGBA member data is now private -> need to use access functions
//
// Revision 1.126  2002/12/12 17:05:29  ingmar
// removed dead code from old 4^3 block histogram
//
// Revision 1.125  2002/12/10 19:58:24  ingmar
// removed  obsolete m_iCropBoxDataBlocks16 (now we always have the complete volume in memory)
//
// Revision 1.124  2002/12/09 23:36:06  ingmar
// switched to using 8bit color table
// improved WriteTFunc1Dand2D()
//
// Revision 1.123  2002/11/29 20:10:13  ingmar
// fixed initiation of refinement mode after hitting preset button
//
// Revision 1.122  2002/11/26 18:11:20  ingmar
// now using Vector4d<float4> to keep the current position as well as offset
// this now also allows for slice shadows as planes
//
// Revision 1.121  2002/11/21 21:25:30  ingmar
// removed hack to draw text as part of the Cpu3D renderer
//
// Revision 1.120  2002/11/14 19:12:18  ingmar
// synchronized ray setup with current OpenGL state
//
// Revision 1.119  2002/11/12 21:49:25  ingmar
// removed redundand UpdateTexture() function (CreateTexture() does it all)
//
// Revision 1.118  2002/11/08 19:00:23  ingmar
// moved the visible data bounding box into the environment to be accessable by the manip CamFly
//
// Revision 1.117  2002/11/08 17:19:12  ingmar
// added computation of depth image
//
// Revision 1.116  2002/11/07 22:46:52  michael
// added float values for the parts that contribute to the progression. They are
// initialized differently in PreRender(), e.g. for TagsOnTop
//
// Revision 1.115  2002/11/05 00:39:17  ingmar
// added m_iImageSizeIndexResponsive to keep trak of las responsive image size to be used when progressive rendering is being canceled
//
// Revision 1.114  2002/11/04 23:42:35  ingmar
// added RenderImageInTiles() to seperate image generation from texture update
//
// Revision 1.113  2002/11/04 15:20:40  ingmar
// made ivImgSize a member variable
//
// Revision 1.112  2002/10/23 23:13:09  ingmar
// RendererQualitySpeedTradeoff now only keeps if high or low framrerate is desired (fly vs examine) the Cpu3D rederer keeps internally what the curent image size is and decides no its own in PstRender() how to change the resolution. the "f" key toggles between the two frame rate modes
// I also cleand out the too general sliders of the RendererQualitySpeedTradeoff. The class now only contains concrete elements that are actually used
//
// Revision 1.111  2002/10/23 16:01:28  ingmar
// removed RaySkipSmallBlocks() because computing how far to advance costs more time than just jumping 2 voxels at a time and going back if one went to far -> 10% speedup for segmented component rendering
//
// Revision 1.110  2002/10/18 17:52:14  ingmar
// local copy of vxComponents array to be used during rendering.
// This avoids problems when other threads change components during rendering
//
// Revision 1.109  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.108  2002/09/25 23:12:36  ingmar
// changes from intel  tuning visit
//
// Revision 1.107  2002/09/25 22:09:21  wenli
// Add BoxOfInterest
//
// Revision 1.106  2002/09/05 22:37:47  jaddonisio
// New vxComponent.
//
// Revision 1.105  2002/08/22 15:21:22  ingmar
// added bool member to remember crop box state, now turning the crop box on or off is reflected properly by the rendering
//
// Revision 1.104  2002/08/21 22:05:33  ingmar
// now differentiating between visibleVoxelCropBox and enclosing multipleOf16BlockCropBox
//
// Revision 1.103  2002/08/09 15:49:17  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.102  2002/08/08 18:12:57  ingmar
// finetuned when to switch between resolutions
//
// Revision 1.101  2002/08/07 22:26:46  ingmar
// now only looking for intensity range bounds in environment when the show range mode is on
// -> apps like CS will no longer segfault if they do not activate(have) this mode
//
// Revision 1.100  2002/08/07 17:19:57  ingmar
// removed redundant redraw events
//
// Revision 1.99  2002/08/07 14:37:54  michael
// Added progress bar wth different colors for each level
//
// Revision 1.98  2002/08/06 23:50:00  ingmar
// added TagsOnTop render mode.
// Currently set on/off per RenderMode::ShowPatientInfo,
// as we do not yet have TagsOnTop in the RenderMode settings
//
// Revision 1.97  2002/08/06 23:04:24  michael
// added m_fRenderProgressCompleteFraction
// update it after each scan line of adaptive sampling tiles
// display it in upper right corner as part of DrawText()
//
// Revision 1.96  2002/08/06 13:24:25  ingmar
// initial version of MIP rendering
// fixed slice shadow setup
//
// Revision 1.95  2002/08/05 17:53:57  ingmar
// reduced progressive refinement rendering to one thread
//
// Revision 1.94  2002/08/05 15:05:13  ingmar
// added preemptive termination of progresive rendering in case that something changed in the environment
//
// Revision 1.93  2002/08/02 20:27:47  ingmar
// final (?) adaptive sampling solution: now anything lrger than 1 pixel diameter will not be missed.
// Vessels of only 1 pixel width may be missed sometimes. This reduced the framerate by 3% (giuaranteeing to see everything would cost 25%()
//
// Revision 1.92  2002/08/01 22:18:48  ingmar
// improved vessel tracking as part of adaptive sampling
//
// Revision 1.91  2002/07/30 18:52:44  ingmar
// completed bi-directional dilate in image space of need to send rays -> 2% speed loss, but fewer CTA arteries popping
//
// Revision 1.90  2002/07/29 16:57:18  ingmar
// completed code cleanup, all major sections of CastSingleRay now have their own inline function
//
// Revision 1.89  2002/07/29 15:50:39  ingmar
// moved accumulation colors into args array
//
// Revision 1.88  2002/07/29 15:31:22  ingmar
// code cleanup, new inline functions for the major parts of empty space skipping in SampleSingleRay()
//
// Revision 1.87  2002/07/26 21:00:36  ingmar
// removed jittering (no longer needed, as preintegration is doing a better job)
//
// Revision 1.86  2002/07/26 20:41:42  ingmar
// removed update and use of 1D transfer function (is less quality and less speed than 2D preintegrated TFunc)
//
// Revision 1.85  2002/07/26 16:44:01  ingmar
// added moreinfo to the space leap paters image
// fimplified the use of the call to draw the image
// added nonlinear stretch of the section close to the end of the ray
// temporarely back to lnearest neighbor interpolation
//
// Revision 1.84  2002/07/25 22:25:09  ingmar
// pulled Benchmark code out of Render() into seperate function RunBenchmark()
// cleaned up frame rate counter and display
//
// Revision 1.83  2002/07/25 21:34:50  ingmar
// made preintegrated TFunc the default
// added preintegrated gradient interpolation threshold to the 2D TFunc
// Removed GetEnvironment().GEtComponentPtr() from SampleSingleRay() -> now uses pComponent in Args
//
// Revision 1.82  2002/07/24 20:32:09  ingmar
// code beautify
//
// Revision 1.81  2002/07/23 20:16:07  ingmar
// coding standard compliant variable naming
// and proper blending of slice and indensity range shadows
//
// Revision 1.80  2002/07/22 21:31:19  ingmar
// variable naming
// use of m_pfTFunc1D to generate m_pfTFunc2D
// TFunc image file now consideres changed alpha of TFunc1D
//
// Revision 1.79  2002/07/22 21:13:56  ingmar
// added alpha correction for 1D tFunc
//
// Revision 1.78  2002/07/22 20:05:09  ingmar
// variable naming
// removal of obsolete args.iDepth
//
// Revision 1.77  2002/07/22 18:23:12  ingmar
// better display of the plain and integrated TFunc images on disk
//
// Revision 1.76  2002/07/17 17:30:53  ingmar
// added jittering, but disabled it for now
//
// Revision 1.75  2002/07/16 22:22:10  ingmar
// com[pleted preintegrated classification, it needs to have its own entry in the args structure
//
// Revision 1.74  2002/07/16 20:48:09  ingmar
// now with toggle between preintegrated and normal TF and with color from gradients
//
// Revision 1.73  2002/07/16 16:30:51  ingmar
// reenabled preintegraded transfer function, now with 0.5 intersample distance as the default that does not modify the alpha values
//
// Revision 1.72  2002/07/15 23:40:27  ingmar
// cleanec up parameters for slice shadow mode
//
// Revision 1.71  2002/07/15 23:19:56  ingmar
// In slice shadow mode set rendering necessarrry only if significant paramwters have changed
//
// Revision 1.70  2002/07/15 20:44:30  ingmar
// added show intensity range (frame rate 3.8)
//
// Revision 1.69  2002/07/11 23:36:53  ingmar
// rev-enabled frane rate benchmark
//
// Revision 1.68  2002/07/11 21:33:03  ingmar
// worked on progressive refinement
//
// Revision 1.67  2002/07/11 20:56:54  ingmar
// worked on progressive refinement
//
// Revision 1.66  2002/07/11 16:35:10  ingmar
// added timer and events for QualitySpeedTradeoff to maintain 5 to 15 fps
//
// Revision 1.65  2002/07/10 18:45:49  ingmar
// added m_bIsViewChangeing to se if we need to adjust the image size
//
// Revision 1.64  2002/07/09 20:57:49  ingmar
// added 4^3 intensity histogram and adjusted rendering to
// first spaceleap in 16^3 SLD and than to skipp over 4^3 blocks not "seen" by the transfer function
//
// Revision 1.63  2002/07/09 17:11:10  ingmar
// changed Sld block width to 16
// several cosmetic improvements
//
// Revision 1.62  2002/07/08 20:35:09  ingmar
// seperated the histogram information into two volumes, one for intensity and one for label, each now with 32 bits
//
// Revision 1.61  2002/07/08 15:34:28  ingmar
// added UpdateTextFields() and added comments
//
// Revision 1.60  2002/07/05 20:42:41  ingmar
// removed depth buffer from adaptive sampling code (this was the only place where it was used, so no depth image was created before)
//
// Revision 1.59  2002/07/05 20:06:42  ingmar
// more compact representation of the offsets
// middle point now interpolated diagonally (made 25% speed improvement)
// similarity test and interpolation now from same set of pixels
// removed unused depth from similarity test
//
// Revision 1.58  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.57  2002/06/27 20:03:48  soeren
// added new very fast updateBoundingBox implementation, needs more testing hence it is disabled.
//
// Revision 1.56  2002/06/27 16:39:19  soeren
// renamed calculate BoundingBox, removed waiting on Texture generation and added more timings
//
// Revision 1.55  2002/06/25 21:53:47  soeren
// Added SetCommonRenderArguments() function, rendering is now slightly faster
//
// Revision 1.54  2002/06/20 20:10:18  ingmar
// cleaned up all functions called by PreRender()
//
// Revision 1.53  2002/06/20 15:23:30  ingmar
// moved private functions to the bottom of the file
//
// Revision 1.52  2002/06/19 21:36:02  soeren
// added more comments and removed unnecessary functions
//
// Revision 1.51  2002/06/18 21:31:55  soeren
// removed obsolete ShadeTwoLights function
//
// Revision 1.50  2002/06/18 21:29:17  soeren
// added additional illuminator
//
// Revision 1.49  2002/06/17 17:08:42  soeren
// fixed UpdateLabelRenderVolume
//
// Revision 1.48  2002/06/17 15:34:12  soeren
// added UpdateLabelRenderVolume
//
// Revision 1.47  2002/06/14 21:56:06  soeren
// added global Framerate timer
//
// Revision 1.46  2002/06/13 22:01:31  ingmar
// new SetImageSize() function
// it does consider vxRenderMOde::VIEW_CHANGING for progressive refinement
//
// Revision 1.45  2002/06/13 21:04:05  ingmar
// now using rayIter->GetDistanceToEndOfBlock()
// also ray sampling patters code fixes
//
// Revision 1.44  2002/06/13 00:10:34  ingmar
// made intersampleDistyance adjustable (in sync with image res)
//
// Revision 1.43  2002/06/12 20:03:10  ingmar
// cleaned up debug image display
//
// Revision 1.42  2002/06/12 15:08:17  ingmar
// towards seperate DrawAdaptiveSamplingPatternPixel()
//
// Revision 1.41  2002/06/12 14:46:50  soeren
// may optimized updated of transferfunction
//
// Revision 1.40  2002/06/11 23:08:11  ingmar
// improved DrawSpaceLeapPatternLine()
//
// Revision 1.39  2002/06/11 15:09:12  ingmar
// added DrawSpaceLeapVolume()
//
// Revision 1.38  2002/06/10 20:42:43  soeren
// Cleaned up code
//
// Revision 1.37  2002/06/06 22:39:20  ingmar
// towards fewer function parameters
//
// Revision 1.36  2002/06/06 18:46:20  soeren
// temp compile fix
//
// Revision 1.35  2002/06/05 22:45:58  soeren
// added SetVolumeBoundaryOfLabelVolumeToMaxLabel()
//
// Revision 1.34  2002/06/04 13:03:44  soeren
// Improved update of final Texture image through glSubimage2D
//
// Revision 1.33  2002/05/31 00:48:40  soeren
// clean up the code and worked in ShowSpaceLeopPattern
//
// Revision 1.32  2002/05/30 19:15:28  soeren
// moved spaceleapIterator to RenderWindow
//
// Revision 1.31  2002/05/30 17:02:06  soeren
// added space leaping though turned off components
//
// Revision 1.30  2002/05/30 15:00:31  soeren
// changed parameters of SampleSingleRay
//
// Revision 1.29  2002/05/30 13:11:39  soeren
// changed to sampling pattern pointer to real array
//
// Revision 1.28  2002/05/30 00:57:24  ingmar
// changed 255 to 1.0 in SampleSingleRay
//
// Revision 1.27  2002/05/29 22:39:47  ingmar
// finished drawSpaceleapPatern
//
// Revision 1.26  2002/05/29 19:49:13  soeren
// added more space leap debug information and enabled broken space leaping
//
// Revision 1.25  2002/05/29 19:02:41  uday
// uint1 array for storing ray information.
//
// Revision 1.24  2002/05/28 21:53:58  soeren
// added new spaceleap method
//
// Revision 1.23  2002/05/28 14:25:32  ingmar
// made abel volume iterator a dlc_pointer and flipped specular light to above the dataset
//
// Revision 1.22  2002/05/08 14:09:07  soeren
// fixed calling updatetf 4 times
//
// Revision 1.21  2002/05/07 18:35:59  frank
// Removed unneccessary includes.
//
// Revision 1.20  2002/05/06 09:07:35  michael
// removed old include
//
// Revision 1.19  2002/05/03 22:25:49  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.18  2002/05/01 21:12:15  jenny
// no m_OriginalBlkVol
//
// Revision 1.17  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.16  2002/04/05 19:12:37  ingmar
// fixed space leap info computation
//
// Revision 1.15  2002/04/04 14:05:20  ingmar
// changed all point to vector3D and switch to new BlockVolume
//
// Revision 1.14  2002/04/03 18:01:23  ingmar
// made Vec3Dfloat4 templetized and renamed to Vector3D
//
// Revision 1.13  2002/04/03 15:38:11  ingmar
// now using Vec3Dfloat4 m_iCropBoxDataBlocks16Min/Max
//
// Revision 1.12  2002/04/02 22:44:09  ingmar
// added m_iIsoValue
//
// Revision 1.11  2002/04/02 20:07:12  ingmar
// added RecBlkVol option
//
// Revision 1.10  2002/04/01 22:30:14  ingmar
// added Isovalue adaptive space leap info computation
//
// Revision 1.9  2002/04/01 15:36:24  ingmar
// removed virtual from DrawText()
// added UpdateBoundingBox()
//
// Revision 1.8  2002/03/29 23:48:25  ingmar
// first step to enableing multithreading in CPU raycaster
//
// Revision 1.7  2002/03/28 23:13:00  ingmar
// code cleanup and more comments
//
// Revision 1.6  2002/03/28 21:00:10  ingmar
// added DrawText()
//
// Revision 1.5  2002/03/27 22:43:21  ingmar
// more cleanup and slickedit formating
//
// Revision 1.4  2002/03/27 22:24:15  ingmar
// zoomed image to full viewport and removed unneeded functions
//
// Revision 1.3  2002/03/27 21:36:19  ingmar
// added Initialize() function to resize the image
//
// Revision 1.2  2002/03/27 19:57:14  ingmar
// transfered lots of code from AAA CPU renderer, no image yet
//
// Revision 1.1  2002/03/27 16:24:45  ingmar
// initial version
//
// Revision 1.2  2002/03/15 21:19:42  michael
// further progress on the 2D renderers
//
// Revision 1.1  2002/03/15 01:27:55  michael
// added initial version of software renderers (still buggy)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererCpu3D.h,v 1.16 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxRendererCpu3D.h,v 1.16 2007/02/15 00:08:23 jmeade Exp $
