// $Id: vxPlanetracer.h,v 1.7 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: Frank Dachille  (mailto:frank@viatronix.com)

// pragma declarations
#pragma once


// include declarations
#include "vxPlanetracerRenderArgs.h"
#include "vxPlanetracerSettings.h"

// forward declarations
class vxRenderBuffer;


// class declaration
class VX_VIEWER_DLL vxPlanetracer
{
// member functions
public:

  /// consturctor
  vxPlanetracer();
  
  /// destructor  
  virtual ~vxPlanetracer();

  /// gets the settings
  vxPlanetracerSettings & GetSettings() { return m_settings; }

  /// gets the render buffer
  vxRenderBuffer & GetRenderBuffer() { return * m_pRenderBuffer; }

  /// performs initialization prior to rendering
  virtual void PreRender();
  
  /// renders the specified window into the buffer
  virtual void RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId );

private:

  /// initializes the raytracer arguments structure
  void InitializeRenderArguments();

  /// Determines the image size based on the orientation of the PlaneOrtho
  void SetImageSize();
  
  /// create single slice image for RenderWindow(). 
  void RenderSingleSlice(vxPlanetracerRenderArgs & args);

  /// create MIP image for RenderWindow()
  void RenderMIP(vxPlanetracerRenderArgs & args);
  
  /// create an x-ray rendered image
  int4 RenderXRay(vxPlanetracerRenderArgs & args);

  /// create MIP image for RenderWindow() 
  void RenderMIPBlock(vxPlanetracerRenderArgs & args);

  /// create MIP image for RenderWindow() 
  void RenderThinSlab(uint1 * pImagePixels, const uint1 & uAxisIndex);

  /// create ThinSlab without label voxel 
  void RenderThinSlabBlock(vxPlanetracerRenderArgs & args);

  /// create ThinSlab with label voxel
  void RenderThinSlabBlockWithLabelVoxel(vxPlanetracerRenderArgs & args);

  /// function pointer to CalcL1dotG_Axial/CalcL1dotG_Sagittal/CalcL1dotG_Coronal
  typedef float4 (vxPlanetracer::*CalcL1dotGFuncPtr)(vxBlockVolumeIterator<uint2> & iterVol);

  /// MIP rendering: process 4 pixels for efficiency
  void MIPComposition_4Rays(const uint2 vx, const uint2 vy, const std::pair<uint2, uint2> MinMaxSliceNo,
                            vxPlanetracerRenderArgs & env, PixelRGBA<float4> pixelColor[]);

  /// X-ray rendering: process 4 pixels for efficiency
  void XRayComposite_4Rays(const uint2 vx, const uint2 vy, const std::pair<uint2, uint2> MinMaxSliceNo,
                           vxPlanetracerRenderArgs & env, PixelRGBA<float4> pixelColor[]);

  /// an auxiliary to MIP: composite with labled voxels
  PixelRGBA<float4> MIPAuxiliary(const uint2 vx, const uint2 vy, const std::pair<uint2, uint2> MinMaxSliceNo,
                                 vxPlanetracerRenderArgs & env);

  /// get cropbox
  BoundingRect<uint2> GetCropBoxWindow(const BoundingRect<uint2> & window, vxPlanetracerRenderArgs & args );

  /// make window a multiple of blocksize
  void ExpandWindowToMultipleOfBlockSize(BoundingRect<uint2> & window, const uint4 uBlockSize);

  /// clears the borders of the image that are not a mutiple of 16
  void ClearBorders( vxPlanetracerRenderArgs & args );

  /// return the bounding rectangle of the crop box in image coordinates
  BoundingRect<uint2> GetCropRectImage(  vxPlanetracerRenderArgs & args  );

  /// accumulate RGB data into a pixel
  void SumPixel( PixelRGBA<float4> & destinationPixel, int2 iIntensity, vxPlanetracerRenderArgs & args, const Vector3Di & voxelOffset );

// member variables
private:

  /// settings
  vxPlanetracerSettings m_settings;

  /// common render arguments
  vxPlanetracerRenderArgs m_arguments;

  // render buffer
  vxRenderBuffer * m_pRenderBuffer;
 
	/// maximum value for voxel value clamping
	int4 m_iMaxVoxelValue;

}; // class vxPlanetracer


// $Log: vxPlanetracer.h,v $
// Revision 1.7  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.6  2007/02/22 16:39:20  geconomos
// code cleanup
//
// Revision 1.5  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.4  2006/09/14 20:09:04  geconomos
// commenting
//
// Revision 1.3  2006/08/04 19:25:57  geconomos
// implemented ClearBorder for when the crop box is visible
//
// Revision 1.2  2006/08/03 14:17:25  geconomos
// updated for switch to vxRenderingNodePlugin usage
//
// Revision 1.1  2006/07/21 14:59:15  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlanetracer.h,v 1.7 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxPlanetracer.h,v 1.7 2007/03/02 19:17:42 geconomos Exp $
