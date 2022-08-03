// $Id: vxRaytracerSettings.h,v 1.12 2007/03/07 21:05:28 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

// pragma declarations
#pragma once


// include declarations
#include "vxRenderingMode.h"
#include "vxBlockVolume.h"
#include "vxDataRendererSoftware.h"
#include "Illuminator.h"
#include "Jitter.h"
#include "vxComponent.h"
#include "vxColormap.h"
#include "vxCropBox.h"
#include "vxWinRect.h"
#include "vxRayGenerator.h"
#include "vxCamera.h"


// forward declarations
class vxComponentArray;


/**
 * Contains the data and settings for the raytracer
 */
class VX_VIEWER_DLL vxRaytracerSettings
{
// member functions
public:
  
  /// constructor
  vxRaytracerSettings();
  
  /// destructor
  virtual ~vxRaytracerSettings();

  ///////////////////////////////////////////////////////////////////////
  // settings

   /// gets the render size
  inline const Triple< uint4 > & GetRenderSize() const { return m_renderSize; }

  /// sets the render size
  inline void SetRenderSize( const Triple< uint4 > & renderSize ){ m_renderSize = renderSize; }

  /// gets if slice shadows should to clip away the volume
  inline const bool GetClipSliceShadow() const { return m_bClipSliceShadow; }

  /// sets if slice shadows should to clip away the volume
  inline void SetClipSliceShadow( const bool bClipSliceShadow ) { m_bClipSliceShadow = bClipSliceShadow; }

  /// gets if slice shadows are enabled
  inline const bool GetSliceShadowsEnabled() const { return m_bSliceShadowsEnabled; }

  /// sets if slice shadows are enabled
  inline void SetSliceShadowsEnabled( const bool bSliceShadowsEnabled ) { m_bSliceShadowsEnabled = bSliceShadowsEnabled ; }

  /// gets if the intensity range should be shown
  inline const bool GetShowIntensityRange() const { return m_bShowIntensityRange; }

  /// sets if the intensity range should be shown
  inline void SetShowIntensityRange( const bool bShowIntensityRange ) { m_bShowIntensityRange = bShowIntensityRange; }

  /// gets low intensity range value
  inline const int4 GetIntensityRangeLow() const { return m_iIntensityRangeLow; }

  /// sets low intensity range value
  inline void SetIntensityRangeLow( const int4 iIntensityRangeLow ) { m_iIntensityRangeLow = iIntensityRangeLow; }

  /// gets high intensity range value
  inline const int4 GetIntensityRangeHigh() const { return m_iIntensityRangeHigh; }

  /// sets high intensity range value
  inline void SetIntensityRangeHigh( const int4 iIntensityRangeHigh ) { m_iIntensityRangeHigh = iIntensityRangeHigh; }

  /// gets if preintegratation is enabled
  inline const bool GetPreIntegrationEnabled() const { return m_bPreIntegrationEnabled; }

  /// sets if preintegratation is enabled
  inline void SetPreIntegrationEnabled( const bool bPreIntegrationEnabled ) { m_bPreIntegrationEnabled = bPreIntegrationEnabled; }

  /// gets if every pixel should be sampled
  inline const bool GetSampleEveryPixel() const { return m_bSampleEveryPixel; }

  /// sets if every pixel should be sampled
  inline void SetSampleEveryPixel( const bool bSampleEveryPixel ) { m_bSampleEveryPixel = bSampleEveryPixel; }

  /// gets if shading is enabled
  inline const bool GetShadingEnabled() const { return m_bShadingEnabled; }

  /// sets if shading is enabled
  inline void SetShadingEnabled( const bool bShadingEnabled ) { m_bShadingEnabled = bShadingEnabled; }

  /// gets if shadowing is enabled
  inline const bool GetShadowingEnabled() const { return m_bShadowingEnabled; }

  /// sets if shadowing is enabled
  inline void SetShadowingEnabled( const bool bShadowingEnabled ) { m_bShadowingEnabled = bShadowingEnabled; }
  
  /// gets if jittering is enabled
  inline const bool GetJitteringEnabled() const { return m_bJitteringEnabled; }

  /// sets if jittering is enabled
  inline void SetJitteringEnabled( const bool bJitteringEnabled ) { m_bJitteringEnabled = bJitteringEnabled; }
  
  /// gets if cuts are shown shaded
  inline const bool GetShowCutsShaded() const { return m_bShowCutsShaded; }

  /// sets if cuts are shown shaded
  inline void SetShowCutsShaded( const bool bShowCutsShaded ) { m_bShowCutsShaded = bShowCutsShaded; }

  /// gets if cuts are shown in grayscale
  inline const bool GetShowCutsGrayscale() const { return m_bShowCutsGrayscale; }

  /// sets if cuts are shown in grayscale
  inline void SetShowCutsGrayscale( const bool bShowCutsGrayscale ) { m_bShowCutsGrayscale = bShowCutsGrayscale; }

  /// gets if the crop box is active
  inline const bool GetCropBoxActive() const { return m_bCropBoxActive; }

  /// sets if the crop box is active
  inline void SetCropBoxActive( const bool bCropBoxActive ) { m_bCropBoxActive = bCropBoxActive; }

  /// gets if the box of interest is active
  inline const bool GetBoxOfInterestActive() const { return m_bBoxOfInterestActive; }

  /// sets if the box of interest is active
  inline void SetBoxOfInterestActive( const bool bBoxOfInterestActive ) { m_bBoxOfInterestActive = bBoxOfInterestActive; }

  /// gets the background color
  inline const PixelRGBA<uint1> & GetBackgroundColor() const { return m_backgroundColor; }

  /// sets the backround color
  void SetBackgroundColor( PixelRGBA<uint1> & backgroundColor ) { m_backgroundColor = backgroundColor; }


  ///////////////////////////////////////////////////////////////////////
  // data

  /// gets the intensity volume
  inline vxBlockVolume< uint2 > & GetIntensityVolume() { return m_intensityVolume; }

  /// sets the intensity volume
  void SetIntensityVolume( vxBlockVolume< uint2 > & intensityVolume );

  /// gets the label volume
  inline vxBlockVolume< uint2 > & GetLabelVolume() { return m_labelVolume; }

  /// sets the label volume
  void SetLabelVolume( vxBlockVolume< uint2 > & intensityVolume );

  /// gets the space leap volume
  inline vxBlockVolume< int1 > & GetSpaceLeapVolume() { return m_spaceLeapVolume; }

  /// sets the space leap volume
  inline void SetSpaceLeapVolume( const vxBlockVolume< int1 > & spaceLeapVolume ) { m_spaceLeapVolume = spaceLeapVolume; }
  
  /// gets the camera
  inline const vxCamera & GetCamera() const { return m_camera; }

  /// sets the camera
  inline void SetCamera( const vxCamera & camera ) { m_camera = camera; }

  /// gets the components
  inline std::vector< vxComponent > & GetComponents() { return m_components; }

  /// sets the components
  void SetComponents( const vxComponentArray & components );

  /// gets the rendering mode
  inline vxRenderingMode & GetRenderingMode() { return m_renderingMode; }

  /// sets the rendering mode
  inline void SetRenderingMode( const vxRenderingMode & renderingMode ) { m_renderingMode = renderingMode; }

  /// gets the visibilty box
  inline const Box< float4 > &  GetVisibilityBox() const { return m_fVisibilityBox; }

  /// sets if the crop box is active
  inline void SetVisibilityBox( const Box< float4 > & fVisibilityBox ) { m_fVisibilityBox = fVisibilityBox; }

  /// gets the box of interest
  inline const Box< float4 > &  GetBoxOfInterest() const { return m_fBoxOfInterest; }

  /// sets the box of interest
  void SetBoxOfInterest( vxBlockVolume< uint2 > & intensityVolume, vxCropBox & cropbox );

  /// sets the colormap
  void SetTransferFunction( vxBlockVolume< uint2 > & intensityVolume, vxColormap & colormap );

  /// gets the slice shadow plane
  inline const vxPlaneOblique< float4 > & GetSliceShadowPlane() const { return m_sliceShadowPlane; }

  /// sets the slice shadow plane
  inline void SetSliceShadowPlane( vxPlaneOblique< float4 > & sliceShadowPlane ) { m_sliceShadowPlane = sliceShadowPlane; }

  /// gets the slice shadow color
  inline const PixelRGBA< uint1 > & GetSliceShadowColor() const { return m_sliceShadowColor; }

  /// sets the slice shadow color
  inline void SetSliceShadowColor( const PixelRGBA< uint1 > & sliceShadowColor ) { m_sliceShadowColor = sliceShadowColor; }

  /// gets if space leaping information should be used while sampling
  inline const bool GetUseSpaceLeaping() const { return m_bUseSpaceLeaping; }

  /// sets if space leaping information should be used while sampling
  inline void SetUseSpaceLeaping( const bool bUseSpaceLeaping ) { m_bUseSpaceLeaping = bUseSpaceLeaping; }

  /// gets the ortho plane
  inline const vxPlaneOrtho< float4 > & GetPlaneOrtho() const { return m_planeOrtho; }

  /// sets the ortho plane
  inline void SetPlaneOrtho( const vxPlaneOrtho<float4> & planeOrtho ) { m_planeOrtho = planeOrtho; }

  /// gets the oblique plane
  inline const vxPlaneOblique< float4 > & GetPlaneOblique() const { return m_planeOblique; }

  /// sets the oblique plane
  inline void SetPlaneOblique( const vxPlaneOblique< float4 > & planeOblique ) { m_planeOblique = planeOblique; }

  ///////////////////////////////////////////////////////////////////////
  // update routines

  /// updates the space leaping volume
  void UpdateSpaceLeapVolume();

  /// updates the voxel mask for the intensity volume
  bool UpdateMaskForIntensities( vxBlockVolume< uint2 > & intensityVolume, vxColormap & colormap );

  /// updates the voxel mask for the label volume
  bool UpdateMaskForLabels( vxComponentArray & components );

  ///////////////////////////////////////////////////////////////////////
  // ray generation
  
  /// gets the win rect
  const vxWinRect & GetWinRect() const  { return m_winrect; };
  
  /// sets the win rect
  void SetWinRect( const vxWinRect & winrect ) { m_winrect = winrect; }
  
  /// gets the ray generator
  vxRayGenerator & GetRayGenerator() { return m_rayGenerator; }


  ///////////////////////////////////////////////////////////////////////
  // sampling and illumination

  /// gets the jitterer
  inline Jitter & GetJitter() { return m_jitter; }

  /// gets the illumintator
  inline Illuminator & GetIlluminator() { return m_illuminator; }

  /// gets the intersample distance along a ray
  inline const float4 & GetInterSampleDistance() const { return m_fInterSampleDistance; }
  
  /// sets the intersample distance along a ray
  inline void SetInterSampleDistance( const float4 & fInterSampleDistance ) { m_fInterSampleDistance = fInterSampleDistance; }

  /// get number of bits to shift alpha to a 8 bit value
  inline const int4 GetNumberOfShiftBitsFor8bitAlpha() const { return m_iShift8Bit; }

  /// set number of bits to shift alpha to a 8 bit value
  inline void SetNumberOfShiftBitsFor8bitAlpha(int4 iShift) { m_iShift8Bit = iShift; }

  /// gets the 1d colormap
  inline const float * GetColormap1d() const { return m_pfColormap1d; }
  
  /// gets the 1d  alpha corrected
  inline const float * GetColormap1dAlphaCorrected() const { return m_pfColormap1dAlphaCorrected; }
 
  /// gets the 1d 8bit colormap
  inline const float * GetColormap1d8bit() const { return m_pfColormap1d8bit; }

  /// gets the 1d 8bit pre-integrated colormap
  inline const float * GetColormap1d8bitPreIntegrated() const { return m_pfColormap1d8bitPreIntegrated; }

  /// gets the 1d 8bit alpha corrected colormap
  inline const float * GetColormap1d8bitAlphaCorrected() const { return m_pfColormap1d8bitAlphaCorrected; }

  /// gets the 2d 8bit pre-integrated colormap
  inline const float * GetColormap2d8bitPreIntegrated() const { return m_pfColormap2d8bitPreIntegrated; }

  /// gets the 2d 8bit pre-integrated out of 1d colormap
  inline const float * GetColormap2d8bitPreIntegratedOutOf1d() const { return m_pfColormap2d8bitPreIntegratedOutOf1d; }



  ///////////////////////////////////////////////////////////////////////
  // masks for space leap creation

  /// gets intensities that are not set by the transfer function
  inline const int4 GetVisibleMaskIntensity() const { return m_iVisibleMaskIntensity; }

  /// sets intensities that are not set by the transfer function
  inline void SetVisibleMaskIntensity( int4 iVisibleMaskIntensity ) { m_iVisibleMaskIntensity = iVisibleMaskIntensity; }

  /// gets labels that are not set by the transfer function
  inline const int4 GetVisibleMaskLabel() const { return m_iVisibleMaskLabel; }

  /// sets labels that are not set by the transfer function
  inline void SetVisibleMaskLabel( int4 iVisibleMaskLabel ) { m_iVisibleMaskLabel = iVisibleMaskLabel; }


// member variables
private:

  ///////////////////////////////////////////////////////////////////////
  //  settings

  /// render size
  Triple< uint4 > m_renderSize;

  /// if slice shadows should to clip away the volume
  bool m_bClipSliceShadow;

  /// slice shadows enabled
  bool m_bSliceShadowsEnabled;

  /// if pre integration
  bool m_bPreIntegrationEnabled;

  /// if sampling every pixel
  bool m_bSampleEveryPixel;

  /// if shading
  bool m_bShadingEnabled;

  /// if shadowing
  bool m_bShadowingEnabled;

  /// if jittering
  bool m_bJitteringEnabled;

  /// if showing cuts shaded
  bool m_bShowCutsShaded;

  /// if showing cuts in grayscale
  bool m_bShowCutsGrayscale;

  /// indicates if the crop box was active in the previous frame
  bool m_bCropBoxActive;

  /// indicates if the box of interest was active in the previous frame
  bool m_bBoxOfInterestActive;

  /// background color
  PixelRGBA<uint1> m_backgroundColor;
  
  /// indicates if space leaping should be used
  bool m_bUseSpaceLeaping;

  /// whether to show the intensity range
  bool m_bShowIntensityRange;

  /// low value for intensity range
  int4 m_iIntensityRangeLow;

  /// high value for intensity range
  int4 m_iIntensityRangeHigh;


  ///////////////////////////////////////////////////////////////////////
  // data

  /// intensity volume
  vxBlockVolume< uint2 > m_intensityVolume;

  /// label volume
  vxBlockVolume< uint2 > m_labelVolume;

  /// space Leap volume
  vxBlockVolume< int1 > m_spaceLeapVolume;

  /// components
  std::vector< vxComponent > m_components;

  /// camera
  vxCamera m_camera;

  /// rendering mode
  vxRenderingMode m_renderingMode;

  /// crop box of the visible voxels in VolumeIndexCoordinates
  Box< float4 > m_fVisibilityBox;

  /// box of interest (all voxels outside are decreased in intensity)
  Box< float4 > m_fBoxOfInterest;

  /// slice shadow plane
  vxPlaneOblique< float4 > m_sliceShadowPlane;

  /// slice shadow color
  PixelRGBA< uint1 > m_sliceShadowColor;

  /// ortho plane
  vxPlaneOrtho< float4 > m_planeOrtho;

  /// oblique plane
  vxPlaneOblique< float4 > m_planeOblique;

  ///////////////////////////////////////////////////////////////////////
  // ray generation

  /// ray generator
  vxRayGenerator  m_rayGenerator;
  
  /// winrect
  vxWinRect m_winrect;

  ///////////////////////////////////////////////////////////////////////
  // sampling and illumination

  /// jitter object
  Jitter m_jitter;

  /// the illuminator for Phong lighting
  Illuminator m_illuminator;

  /// the intersample distance along a ray
  float4 m_fInterSampleDistance;

  // TODO: encapsulate colormaps
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

  /// bits to shift intensity value on color table lookup
  int4 m_iShift8Bit;


  ///////////////////////////////////////////////////////////////////////
  // masks for space leap creation

  /// indicates which intensities are not set by the transfer function
  int4 m_iVisibleMaskIntensity;

  /// indicates which labels are invisible
  int4 m_iVisibleMaskLabel;

  ///indicates the quality sample distance scale resolution
  float4 m_fQualitySampleDistanceScaleResolutionDependent;

}; // vxRaytracerSettings

#include "vxRaytracerSettings.inl"

// $Log: vxRaytracerSettings.h,v $
// Revision 1.12  2007/03/07 21:05:28  romy
// added comments - part of code review
//
// Revision 1.11  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.10  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.9  2006/10/04 14:57:47  geconomos
// removed unused vxDataRendererSoftware
//
// Revision 1.8  2006/09/21 14:59:06  geconomos
// switch datatype for components from td::vector<vxComponent*>to std::vector<vxComponent>
//
// Revision 1.7  2006/08/03 14:15:42  geconomos
// vxRayGenerator related changes
//
// Revision 1.6  2006/07/12 19:40:27  geconomos
// + added segmentation intenisty range properties
// + added slice shadow related properties
//
// Revision 1.5  2006/07/07 13:43:52  geconomos
// added "render size" and "ray generator" class variables
//
// Revision 1.4  2006/06/23 19:16:19  frank
// formatting
//
// Revision 1.3  2006/06/21 18:46:53  geconomos
// added vxRayGenerator instance pointer
//
// Revision 1.2  2006/06/21 14:07:49  geconomos
// + code cleanup
// + exported classes
//
// Revision 1.1  2006/06/20 20:05:48  geconomos
// refactored vxSoftwareRendererCpu3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRaytracerSettings.h,v 1.12 2007/03/07 21:05:28 romy Exp $
// $Id: vxRaytracerSettings.h,v 1.12 2007/03/07 21:05:28 romy Exp $
