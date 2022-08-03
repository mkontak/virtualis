// $Id: vxRendererSoftwareMPROrtho.h,v 1.5 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

#ifndef vxRendererSoftwareMPROrtho_h
#define vxRendererSoftwareMPROrtho_h


// includes
#include "vxRendererSoftwareMPR.h"
#include "vxBlockVolumeIterator.h"
#include "vxEnvironment.h"


// forward declarations
template <class VoxelType> class vxBlockVolumeIterator;


/**
 * Defines the software render MPR orthogonal views
 */
class VX_VIEWER_DLL vxRendererSoftwareMPROrtho : public vxRendererSoftwareMPR
{
// inner classes
private:

  /**
   * Class definition: structure to support efficient computation of pixels
   */
  class RenderEnvironment
  {
  // member functions
  public:

    /// constructor
    RenderEnvironment(vxEnvironment & environment);

  // data members
  public:

    /// show labels?
    bool m_bShowLabels;

    /// show color?
    bool m_bShowColor;

    /// show threshholds?
    bool m_bShowThresholds;

    /// threshold low
    uint4 m_uThresholdLow;

    /// threshold high
    uint4 m_uThresholdHigh;

    /// axis index
    uint1 m_uAxisIndex;

    /// min and max slab pair
    std::pair<uint2, uint2> m_minMaxSlicePair;

    /// number of slices in slab
    int4 m_iNumSlicesInSlab;

    /// pointer to lookup table
    float * m_pfRGBA;

    /// pointer to components
    const std::vector<vxComponent *> * m_pvComponents;

    /// image pixels
    uint1 * m_pImagePixels;

    /// volume dimensions
    Triple<int4> m_volDim;

    /// volume units
    Triple<float4> m_volUnits;

    /// ortho plane
    vxPlaneOrtho<float4> * m_pPlaneOrtho;

    /// ortho plane distance
    int4 m_iPlaneOrthoDistance;

    /// screen space rendering window
    BoundingRect<uint2> m_renderWindow;

    /// iterator for intensity volume
    vxBlockVolumeIterator<uint2> m_iterVol;

    /// iterator for label volume
    vxBlockVolumeIterator<uint2> m_iterLabel;

    /// block size
    static const uint4 m_uBlockSize = 16;

    /// label volume available?
    bool m_bLabelVolumeAvailable;

    /// needs lables?
    bool m_bNeedLabels;

  }; // class RenderEnvironment

// member functions
public:

  /// constructor
  vxRendererSoftwareMPROrtho(vxEnvironment & environment);
 
  /// destructor
  virtual ~vxRendererSoftwareMPROrtho() {}

  /// perform pre-rendering operations
  virtual int4 PreRender();
  
  /// this is ONLY for image space parallel algorithms
  virtual int4 RenderWindow(const BoundingRect<uint2> &, const uint1 & uThreadID);

  /// create a texture
  virtual void CreateTexture();

  /// called right before CreateTexture()
  virtual void PreCreateTexture(const BoundingRect<uint2> & window);

  /// returns the size of the image to be computed
  virtual Triple<uint2> GetImageSize();

private:
  
  /// copy constructor
  vxRendererSoftwareMPROrtho(const vxRendererSoftwareMPROrtho & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererSoftwareMPROrtho & operator=(const vxRendererSoftwareMPROrtho & other); // should only be public if really implemented!

  /// create single slice image for RenderWindow(). 
  void RenderSingleSlice(RenderEnvironment & environment);

  /// create MIP image for RenderWindow()
  void RenderMIP(RenderEnvironment & environment);
  
  /// create an x-ray rendered image
  int4 RenderXRay(RenderEnvironment & environment);

  /// create MIP image for RenderWindow() 
  void RenderMIPBlock(RenderEnvironment & environment);

  /// create MIP image for RenderWindow() 
  void RenderThinSlab(uint1 * pImagePixels, const uint1 & uAxisIndex);

  /// create ThinSlab without label voxel 
  void RenderThinSlabBlock(RenderEnvironment & environment);

  /// create ThinSlab with label voxel
  void RenderThinSlabBlockWithLabelVoxel(RenderEnvironment & environment);

  /// function pointer to CalcL1dotG_Axial/CalcL1dotG_Sagittal/CalcL1dotG_Coronal
  typedef float4 (vxRendererSoftwareMPROrtho::*CalcL1dotGFuncPtr)(vxBlockVolumeIterator<uint2> & iterVol);

  /// MIP rendering: process 4 pixels for efficiency
  void MIPComposition_4Rays(const uint2 vx, const uint2 vy, const std::pair<uint2, uint2> MinMaxSliceNo,
                            RenderEnvironment & env, PixelRGBA<float4> pixelColor[]);

  /// X-ray rendering: process 4 pixels for efficiency
  void XRayComposite_4Rays(const uint2 vx, const uint2 vy, const std::pair<uint2, uint2> MinMaxSliceNo,
                           RenderEnvironment & env, PixelRGBA<float4> pixelColor[]);

  /// an auxiliary to MIP: composite with labled voxels
  PixelRGBA<float4> MIPAuxiliary(const uint2 vx, const uint2 vy, const std::pair<uint2, uint2> MinMaxSliceNo,
                                 RenderEnvironment & env);

  /// cast a ray along Axial/Sagittal/Coronal direction. -- shading.
  /// if 'shading' needed in the future, this function should be parallelized, just as 'non-shading' did.
  PixelRGBA<float4> SimpleRayCastingWithShading(const uint2 vx, const uint2 vy, vxBlockVolumeIterator<uint2> & iterVol, 
                                                const std::pair<uint2, uint2> MinMaxSliceNo, 
                                                const uint1 uAxisIndex, const bool bInverseDir = false);

  /// accumulate RGB data into a pixel.
  void SumPixel( PixelRGBA<float4> & destinationPixel, int2 iIntensity, vxRendererSoftwareMPROrtho::RenderEnvironment & env,
                 const Vector3Di & voxelOffset );

  /// accumulation along dedicate view direction (with shading effect).
  inline bool AccumulationOfIntensityAndOpacityWithShading(float4 & fAccuIntensity, float4 & fAccuOpacity, 
                                                           vxBlockVolumeIterator<uint2> & iterVol, bool & bNoShadeOnFirstSample, 
                                                           CalcL1dotGFuncPtr pCalcL1DotGFunc);

  /// axial dot product of Light and Normal(gradient)
  inline float4 CalcL1dotG_Axial(vxBlockVolumeIterator<uint2> & iterVol);

  /// coronal dot product of Light and Normal(gradient)
  inline float4 CalcL1dotG_Coronal(vxBlockVolumeIterator<uint2> & iterVol);

  /// sagittal dot product of Light and Normal(gradient)
  inline float4 CalcL1dotG_Sagittal(vxBlockVolumeIterator<uint2> & iterVol);

  /// get cropbox
  BoundingRect<uint2> GetCropBoxWindow(const BoundingRect<uint2> & window, const uint1 & uAxisIndex);

  /// make window a multiple of blocksize
  void ExpandWindowToMultipleOfBlockSize(BoundingRect<uint2> & window, const uint4 uBlockSize);

  /// clears the borders of the image that are not a mutiple of 16
  void ClearBorders(const BoundingRect<uint2> & window, const BoundingRect<uint2> & cropRect);

  /// return the bounding rectangle of the crop box in image coordinates
  BoundingRect<uint2> vxRendererSoftwareMPROrtho::GetCropRectImage();

  /// retrieve the setting of minMaxSlicePair
  const inline std::pair<int4, int4> & GetMinMaxSlicePair() { return m_minMaxSlicePair; }

  /// setting the minMaxSlicePair
  inline void SetMinMaxSlicePair(const std::pair<int4, int4> & slabPair) { m_minMaxSlicePair = slabPair; }

// data members
private:

  /// slab pair
  std::pair<int4, int4> m_minMaxSlicePair;

}; // class vxRendererSoftwareMPROrtho


#endif // vxRendererSoftwareMPROrtho_h


// $Log: vxRendererSoftwareMPROrtho.h,v $
// Revision 1.5  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.4.12.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.4  2004/10/04 14:59:58  frank
// removed unused return values
//
// Revision 1.3  2004/10/01 12:05:24  frank
// moved static member initialization to header
//
// Revision 1.2  2004/05/17 13:02:03  frank
// fixed various problems with label volumes and components under different rendering modes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.67  2004/02/11 14:06:01  frank
// removed dead code
//
// Revision 1.66  2003/12/18 17:03:24  frank
// doxygen likes three slashes for data members
//
// Revision 1.65  2003/07/29 11:52:09  michael
// took out CineSeries stuff since it does not belong to this class/object and will
// be provided by the movie tool.
//
// Revision 1.64  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.63  2003/06/18 15:19:59  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.62  2003/06/04 18:22:57  michael
// added namespace declaration
//
// Revision 1.61  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.60  2003/05/16 20:46:41  dongqing
// code review
//
// Revision 1.59  2003/05/16 18:38:39  michael
// code review
//
// Revision 1.58  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.57  2003/05/14 17:28:25  michael
// code review
//
// Revision 1.56  2003/03/07 13:42:57  frank
// Added ability to create a new cine series
//
// Revision 1.55  2002/12/24 18:11:10  frank
// Blended labels in x-ray mode
//
// Revision 1.54  2002/12/23 17:06:48  frank
// Removed even more redundant code
//
// Revision 1.53  2002/12/23 15:11:03  frank
// Removed more redundant code
//
// Revision 1.52  2002/12/23 14:32:36  frank
// Removed redundant code
//
// Revision 1.51  2002/12/23 13:54:59  frank
// Encapsulating the common rendering environment variables and constants.
//
// Revision 1.50  2002/12/20 16:14:00  frank
// Added x-ray projection
//
// Revision 1.49  2002/12/11 22:31:23  ingmar
// removed obsolete RenderThinSlab() and dependent functions
//
// Revision 1.48  2002/12/11 21:13:53  ingmar
// removed window resizing to multiple of blocksize from crop box compute into its own function ExpandWindowToMultipleOfBlockSize()
//
// Revision 1.47  2002/12/09 15:32:29  michael
// Added comments and code clean-up
//
// Revision 1.46  2002/11/15 18:12:29  frank
// Fixed border clearing fiasco, re-implemented PreCreateTexture()
//
// Revision 1.45  2002/11/08 13:53:18  michael
// removed redundant method
//
// Revision 1.44  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.43  2002/09/28 02:19:36  frank
// Fixed border area.
//
// Revision 1.42  2002/09/27 16:06:22  frank
// Completed merge from XP_1.0 branch.
//
// Revision 1.41  2002/09/05 23:01:46  jaddonisio
// New vxComponent.
//
// Revision 1.40  2002/08/14 22:28:17  uday
// MIP and VR mode updates for CropBox.
//
// Revision 1.39  2002/08/09 15:39:06  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.38  2002/08/08 15:56:49  uday
// Added SetBorders(), cropBox stuff.
//
// Revision 1.37  2002/07/08 17:33:30  binli
// Cropbox
//
// Revision 1.36  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.35.2.5  2002/07/10 21:34:17  binli
// MIP rendering (no label voxel): block-based method
//
// Revision 1.35.2.4  2002/07/09 21:23:27  binli
// applied same approach as that in Revision 1.54.2.8 to ThinSlab rendering with labeled voxles.
//
// Revision 1.35.2.3  2002/07/08 21:13:46  binli
// volume rendering: block-based accumulation
// (+double the speed!)
//
// Revision 1.35.2.2  2002/07/03 15:10:40  binli
// CropBox stuff
//
// Revision 1.35.2.1  2002/07/02 15:55:51  uday
// Moved GetSlabPair() to vxUtils.
//
// Revision 1.35  2002/06/25 16:29:18  binli
// a little acceleration: 5%
//
// Revision 1.34  2002/06/20 19:17:59  binli
// faster MIP: cast 4 rays per loop. (quickpeek in vxBlockVolume)
//
// Revision 1.33  2002/06/20 17:27:32  binli
// issue 2455: fixed in MIPComposition3.
//
// Revision 1.32  2002/06/17 16:28:14  binli
// inline function
//
// Revision 1.31  2002/06/13 20:37:38  binli
// test of ThinSlabRendering
//
// Revision 1.30  2002/06/13 15:23:37  binli
// Parallel ThinSlab rendering: cast 4 rays per loop.
//
// Revision 1.29  2002/06/11 21:46:46  frank
// Speeding up MIP rendering.
//
// Revision 1.28  2002/06/11 16:24:35  binli
// issue 2739: added a post-processing.
//
// Revision 1.27  2002/06/11 04:26:59  frank
// Fixed threshold display in MIP mode.
//
// Revision 1.26  2002/06/10 15:17:42  dmitry
// Working on MIP w/ Threshold display?
//
// Revision 1.25  2002/06/05 17:46:27  binli
// MIP & Single Slice rendering: show component or not.
//
// Revision 1.24  2002/05/14 15:40:10  binli
// little improvement of rendering efficiency: use function pointer avoid 'switch'
//
// Revision 1.23  2002/05/13 21:22:33  binli
// MIP composion method 1,2,3
//
// Revision 1.22  2002/05/10 19:32:03  binli
// code cleaned
//
// Revision 1.21  2002/05/10 18:56:11  binli
// 1> MIP rendering, new rule
// 2> code adjusted
//
// Revision 1.20  2002/05/07 20:32:33  binli
// code cleaned
//
// Revision 1.19  2002/05/07 17:57:22  binli
// MIP/ThinSlab rendering: show lable
//
// Revision 1.18  2002/05/06 14:51:55  binli
// fixed a bug when rendering ThinSlab: new transfer function(float & int)
//
// Revision 1.17  2002/05/03 22:25:49  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.16  2002/04/30 21:04:49  binli
// 1> viewing from positive/negative (with shading)
// 2> switch between MIP/ThinSlab
//
// Revision 1.15  2002/04/30 21:02:57  binli
// 1> viewing from positive/negative (with shading)
// 2> switch between MIP/ThinSlab
//
// Revision 1.14  2002/04/29 18:20:21  binli
// 1> code adjusted
// 2> SlidingThinSlab: viewing from positive/nagative direction.
//
// Revision 1.13  2002/04/23 21:36:16  jenny
// not a template class any more
//
// Revision 1.12  2002/04/23 20:52:27  binli
// SlidingThinSlab: version of no shading.
//
// Revision 1.11  2002/04/23 20:10:25  binli
// code cleaned
//
// Revision 1.10  2002/04/23 16:17:43  binli
// optimization for SlidingThinSlab volume rendering.
//
// Revision 1.9  2002/04/23 15:42:04  jenny
// got rid of CreateBlockVolumeForTest
//
// Revision 1.8  2002/04/22 20:36:00  binli
// slab thickness control
//
// Revision 1.7  2002/04/22 19:05:01  binli
// First time MIP: resume calculation.
//
// Revision 1.6  2002/04/22 17:57:50  binli
// SlidingThinSlab: Coronal & Sagittal
//
// Revision 1.5  2002/04/20 18:01:49  michael
// Removed the burden of generating the block volume when MIP is not
// used. Lazy mechanism generates in on demand. Also replaced include
// by forward declarations
//
// Revision 1.4  2002/04/19 15:28:32  binli
// MIP rendering/Sliding Thin Slice
//
// Revision 1.3  2002/04/08 13:36:08  michael
// re-added missing privately non implemented copy constructor and assignment operator
//
// Revision 1.2  2002/03/15 21:19:42  michael
// further progress on the 2D renderers
//
// Revision 1.1  2002/03/15 01:27:55  michael
// added initial version of software renderers (still buggy)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSoftwareMPROrtho.h,v 1.5 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererSoftwareMPROrtho.h,v 1.5 2007/02/15 00:08:24 jmeade Exp $
