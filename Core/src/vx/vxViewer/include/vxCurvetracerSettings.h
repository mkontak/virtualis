// $Id: vxCurvetracerSettings.h,v 1.5 2007/03/02 19:17:42 geconomos Exp $
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
#include "vxComponentArray.h"
#include "vxColormap.h"
#include "vxWinRect.h"
#include "vxVessel.h"
#include "vxRibbon.h"


/**
 * Contains the data and settings for the curvetracer
 */
class VX_VIEWER_DLL vxCurvetracerSettings
{
// member functions
public:

  /// constructor
  vxCurvetracerSettings();

  /// destructor
  virtual ~vxCurvetracerSettings();

  /// sets the ribbon
  void SetRibbon( const vxRibbon & ribbon ) { m_ribbon = ribbon; }

  /// gets the vessel
  const vxVessel & GetVessel() const { return m_vessel; }
  
  /// sets the vessel
  void SetVessel( const vxVessel & vessel ) { m_vessel = vessel; }

  /// gets the intensity volume
  inline const vxBlockVolume< uint2 > & GetIntensityVolume() const { return m_intensityVolume; }

  /// sets the intensity volume
  void SetIntensityVolume( vxBlockVolume< uint2 > & intensityVolume );

  /// gets the rendering mdoe
  inline const vxRenderingMode & GetRenderingMode() const { return m_renderingMode; }

  /// sets the rendering mdoe
  inline void SetRenderingMode( const vxRenderingMode & renderingMode ) { m_renderingMode = renderingMode; }

  /// gets the colormap
  inline const vxColormap & GetColormap() const { return m_colormap; }

  /// sets the colormap
  void SetTransferFunction( vxBlockVolume< uint2 > & volume , vxColormap & colormap );

  /// gets the colortable
  inline const float4 * GetColorTable() const { return m_pfRGBA; }

  /// gets for alpha shift bit count
  inline int4 GetNumberOfShiftBitsFor8bitAlpha() const { return m_iShiftFor8bitAlpha; }

  /// gets the win rect
  const vxWinRect & GetWinRect() const { return m_winrect; }
  
  /// sets the win rect
  void SetWinRect( const vxWinRect & winrect ) { m_winrect = winrect; }

  /// gets the ribbon
  const vxRibbon & GetRibbon() const { return m_ribbon ; }
  

// member variables
private:

  /// ribbon
  vxRibbon m_ribbon;

  /// vessel
  vxVessel m_vessel;

  /// intensity volume
  vxBlockVolume< uint2 > m_intensityVolume;

  /// rendering mode
  vxRenderingMode m_renderingMode;

  /// components
  vxComponentArray m_components;

  /// win rect
  vxWinRect m_winrect;

  /// colormap
  vxColormap m_colormap;

  /// color table
  float4 * m_pfRGBA;

  /// bits to shift intensity value on color table lookup
  int4 m_iShiftFor8bitAlpha;

}; // class vxCurvetracerSettings


// $Log: vxCurvetracerSettings.h,v $
// Revision 1.5  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.4  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.3  2007/02/22 18:27:15  geconomos
// code cleanup
//
// Revision 1.2  2006/09/21 14:57:31  geconomos
// removed unused vxComponentArray
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCurvetracerSettings.h,v 1.5 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxCurvetracerSettings.h,v 1.5 2007/03/02 19:17:42 geconomos Exp $
