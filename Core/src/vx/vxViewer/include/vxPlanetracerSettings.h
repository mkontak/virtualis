// $Id: vxPlanetracerSettings.h,v 1.7.8.2 2010/08/05 14:17:35 dongqing Exp $
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
#include "vxBlockVolume.h"
#include "vxRenderingMode.h"
#include "vxPlaneOrtho.h"
#include "vxCropbox.h"
#include "vxColormap.h"
#include "vxComponentArray.h"


/**
 * Contains the data and settings for the planetracer
 */
class VX_VIEWER_DLL vxPlanetracerSettings
{
// member functions
public:
  
  /// constructor
  vxPlanetracerSettings();
  
  /// destructor
  virtual ~vxPlanetracerSettings();

  /// gets the need labels flag
  inline const bool GetNeedLabels() const { return m_bNeedLabels; }

  /// sets the need labels flag
  inline void SetNeedLabels( const bool bNeedLabels ) { m_bNeedLabels = bNeedLabels; }

  /// gets the show labels flag
  inline const bool GetShowLabels() const { return m_bShowLabels; }

  /// sets the show labels flag
  inline void SetShowLabels( const bool bShowLabels) { m_bShowLabels = bShowLabels; }

  /// gets the show color flag
  inline const bool GetShowColor() const { return m_bShowColor; }

  /// sets the show color flag
  inline void SetShowColor( const bool bShowColor) { m_bShowColor = bShowColor; }

  /// gets the show threshold flag
  inline const bool GetShowThreshold() const { return m_bShowThresholds; }

  /// sets the show threshold flag
  inline void SetShowThreshold( const bool bShowThresholds) { m_bShowThresholds = bShowThresholds; }

  /// gets the threshold low value
  inline const uint4 GetThresholdLow() const { return m_uThresholdLow; }

  /// sets the threshold low value
  inline void SetThresholdLow( const uint4 uThresholdLow) { m_uThresholdLow = uThresholdLow; }

  /// gets the threshold high value
  inline const uint4 GetThresholdHigh() const { return m_uThresholdHigh; }

  /// sets the threshold high value
  inline void SetThresholdHigh( const uint4 uThresholdHigh) { m_uThresholdHigh = uThresholdHigh; }

  /// gets the offset
  inline const int4 GetOffset() const { return m_iOffset; }

  /// sets the offset
  inline void SetOffset( const uint4 iOffset) { m_iOffset = iOffset; }

  /// gets the maximum offset
  inline const int4 GetMaximumOffset() const { return m_iMaximumOffset; }

  /// sets the maximum offset
  inline void SetMaximumOffset( const uint4 iMaximumOffset) { m_iMaximumOffset = iMaximumOffset; }

  /// retrieve the setting of minMaxSlicePair
  const inline std::pair<int4, int4> & GetMinMaxSlicePair() { return m_minMaxSlicePair; }

  /// setting the minMaxSlicePair
  inline void SetMinMaxSlicePair(const std::pair<int4, int4> & slabPair) { m_minMaxSlicePair = slabPair; }

  /// gets the intensity volume
  inline vxBlockVolume< uint2 > & GetIntensityVolume() { return m_intensityVolume; }

  /// sets the intensity volume
  void SetIntensityVolume( vxBlockVolume< uint2 > & intensityVolume );

  /// gets the label volume
  inline vxBlockVolume< uint2 > & GetLabelVolume() { return m_labelVolume; }

  /// sets the label volume
  void SetLabelVolume( vxBlockVolume< uint2 > & intensityVolume );

  /// gets the rendering mdoe
  inline const vxRenderingMode & GetRenderingMode() const { return m_renderingMode; }

  /// sets the rendering mdoe
  inline void SetRenderingMode( const vxRenderingMode & renderingMode ) { m_renderingMode = renderingMode; }

  /// gets the crop box
  inline const Box<float4> & GetCropBox() const { return m_cropbox; }

  /// sets the crop box
  inline void SetCropBox( const Box<float4> & cropbox ) { m_cropbox = cropbox; }

  /// gets the crop rect
  inline const BoundingRect<float4> & GetCropRect() const { return m_croprect; }

  /// sets the crop rect
  inline void SetCropRect( const BoundingRect<float4> & croprect ) { m_croprect = croprect; }

  /// gets the plane ortho
  inline const vxPlaneOrtho< float4 > & GetPlaneOrtho() const { return m_planeOrtho; }

  /// sets the plane ortho
  inline void SetPlaneOrtho( const vxPlaneOrtho< float4 > & planeOrtho ) { m_planeOrtho = planeOrtho; }

  /// gets the components
  inline std::vector< vxComponent > & GetComponents() { return m_components; }

  /// sets the components
  void SetComponents( const vxComponentArray & components );

  /// gets the colormap
  inline const vxColormap & GetColormap() const { return m_colormap; }

  /// gets the colortable
  inline const float4 * GetColorTable() { return m_pfRGBA; }

  /// gets the number of bits for alpha shift
  inline const int4 GetNumberOfShiftBitsFor8bitAlpha() const { return m_iShiftFor8bitAlpha; }

  /// sets the colormap
  void SetTransferFunction( vxBlockVolume< uint2 > & volume , vxColormap & colormap );

/// member variables
private:

  /// show labels
  bool m_bShowLabels;

  /// need labels
  bool m_bNeedLabels;

  /// show color
  bool m_bShowColor;

  /// show threshholds
  bool m_bShowThresholds;

  /// threshold low
  uint4 m_uThresholdLow;

  /// threshold high
  uint4 m_uThresholdHigh;

  /// offset for plane distance calculation
  int4 m_iOffset;
  
  /// maximum offset for plane distance calculation
  int4 m_iMaximumOffset;

  /// slab pair
  std::pair<int4, int4> m_minMaxSlicePair;

   /// intensity volume
  vxBlockVolume< uint2 > m_intensityVolume;

  /// label volume
  vxBlockVolume< uint2 > m_labelVolume;

  /// rendering mode
  vxRenderingMode m_renderingMode;

  /// crop box
  Box<float4> m_cropbox;

  /// crop rect
  BoundingRect<float4> m_croprect;

  /// plane ortho
  vxPlaneOrtho< float4 > m_planeOrtho;

  /// components
  std::vector< vxComponent > m_components;

  /// colormap
  vxColormap m_colormap;

  /// color table
  float4 * m_pfRGBA;

  /// bits to shift intensity value on color table lookup
  int4 m_iShiftFor8bitAlpha;

}; // vxPlanetracerSettings


// $Log: vxPlanetracerSettings.h,v $
// Revision 1.7.8.2  2010/08/05 14:17:35  dongqing
// fix the memory leak in vxColormap = operator. and roll back the previous
// fix in vxPlanetracerSetting.C/h
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.8.1  2010/08/05 13:50:15  dongqing
// fix the colormap memory leak (m_colormap)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.6  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.5  2007/02/22 16:39:20  geconomos
// code cleanup
//
// Revision 1.4  2006/10/03 20:03:09  geconomos
// modified GetComponents() to return non-const reference to vector of components
//
// Revision 1.3  2006/09/21 14:58:33  geconomos
// switch datatype for components from vxComponentArray to std::vector<vxComponent>
//
// Revision 1.2  2006/08/03 14:16:30  geconomos
// cosmetics
//
// Revision 1.1  2006/07/21 14:59:15  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlanetracerSettings.h,v 1.7.8.2 2010/08/05 14:17:35 dongqing Exp $
// $Id: vxPlanetracerSettings.h,v 1.7.8.2 2010/08/05 14:17:35 dongqing Exp $
