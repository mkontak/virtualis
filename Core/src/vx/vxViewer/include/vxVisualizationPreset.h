// $Id: vxVisualizationPreset.h,v 1.6 2007/06/06 18:22:26 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxVisualizationPreset_h
#define vxVisualizationPreset_h


// includes
#include "vxColormap.h"
#include "vxRenderingMode.h"
#include "Enums.h"
#include "PixelRGB.h"
#include "vxIserialize.h"


/**
 * This class encomapasses the visualization settings that define how a
 * rendered view should look.  It includes things such as the colormap
 * the rendering mode, and the background and foreground colors
 */
class VX_VIEWER_DLL vxVisualizationPreset
{
// functions
public:

  /// constructor
  vxVisualizationPreset();

  /// constructor
  vxVisualizationPreset( vxColormap * pColormap, vxRenderingMode * pRenderingMode );

  /// copy constructor
  vxVisualizationPreset( const vxVisualizationPreset & other );

  /// destructor
  ~vxVisualizationPreset();

  /// assignment operator
  const vxVisualizationPreset & operator =( const vxVisualizationPreset & other );

// properties
public:

  /// get title
  const std::string & GetName() const { return m_sName; }

  /// set title
  void SetName(std::string & name) { m_sName = name; }

    /// get Preset Image File
  const std::string & GetPresetImageFile() const { return m_sPresetImageFile; }

  /// set Preset Image File
  void SetPresetImageFile(std::string & file) { m_sPresetImageFile = file; }
	
	/// get Preset Image File display option
  const bool IsShowPresetImage() const { return m_bShowPresetImage; }

  /// set Preset Image File display option
  void SetShowPresetImage(bool bShow) { m_bShowPresetImage = bShow; }

  /// get colormap
  vxColormap * GetColormap() const { return m_pColormap; }

  /// set colormap
  void SetColormap( vxColormap * pColormap ) { m_pColormap = pColormap; }

  /// set the preferred rendering mode
  void SetRenderingMode( vxRenderingMode * pRenderingMode ) { m_pRenderingMode = pRenderingMode; }

  /// get the preferred rendering mode
  vxRenderingMode * GetRenderingMode() const { return m_pRenderingMode; }

  /// get the background color
  const PixelRGB<float4> & GetBackgroundColor() const { return m_backgroundColor; }

  /// set the background color
  void SetBackgroundColor( const PixelRGB<float4> & backgroundColor ) { m_backgroundColor = backgroundColor; }

  /// get the foreground color
  const PixelRGB<float4> & GetForegroundColor() const { return m_foregroundColor; }

  /// set the foreground color
  void SetForegroundColor( const PixelRGB<float4> & foregroundColor ) { m_foregroundColor = foregroundColor; }

// data members
private:

  /// name
  std::string m_sName;

  /// the Preset Image file
  std::string m_sPresetImageFile;

	///Flag indicates whether to display the Image Icon or not.
	bool m_bShowPresetImage;

  /// colormap
  vxColormap * m_pColormap;

  /// the rendering mode
  vxRenderingMode * m_pRenderingMode;

  /// background color
  PixelRGB<float4> m_backgroundColor;

  /// foreground color
  PixelRGB<float4> m_foregroundColor;

}; // class vxVisualizationPreset


#endif // vxVisualizationPreset_h


// $Log: vxVisualizationPreset.h,v $
// Revision 1.6  2007/06/06 18:22:26  romy
// added a new flag to Indicate whether we want to display the Preset Image or not
//
// Revision 1.5  2007/03/23 14:34:55  romy
// Property presetImageFile added
//
// Revision 1.4  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.6.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.3  2004/05/17 19:08:13  frank
// moved visualization preset serialization to its own class
//
// Revision 1.2  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/05 13:34:20  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVisualizationPreset.h,v 1.6 2007/06/06 18:22:26 romy Exp $
// $Id: vxVisualizationPreset.h,v 1.6 2007/06/06 18:22:26 romy Exp $
