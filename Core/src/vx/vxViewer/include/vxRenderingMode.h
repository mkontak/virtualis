// $Id: vxRenderingMode.h,v 1.13 2007/08/21 19:42:38 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

// pramgas
#pragma once


// includes
#include "Box.h"


// class definition
class VX_VIEWER_DLL vxRenderingMode 
{
// enums
public:

  enum vxOptionBitEnum // this needs to be HEXADECIMAL values!!!
  {
    // general "overlay" stuff
    SHOW_PATIENT_INFORMATION  = 0x00000001,
    SHOW_IMAGE_INFORMATION    = 0x00000002,
    SHOW_VESSEL_CENTERLINE    = 0x00000004,
    SHOW_ANNOTATIONS          = 0x00000008,

    // MPR stuff
    SHOW_COMPONENT_IN_MPR     = 0x00000010,
    SHOW_COLORMAP_IN_MPR      = 0x00000020,
    SHOW_CURSOR_3D_IN_MPR     = 0x00000040,
    
    // other rendering stuff
    SHOW_INTENSITY_RANGE      = 0x00000100,
    //UNUSED                  = 0x00000200,
    SHOW_LABEL_ON_TOP         = 0x00000400,
    SHOW_CROP_BOX             = 0x00000800,
    SHOW_BOX_OF_INTEREST      = 0x00001000,
    //UNUSED                  = 0x00002000,

    // 3D stuff
    AUTO_FLY_MODE             = 0x00008000,
    SHOW_JITTERING            = 0x00010000,
    SHOW_PRE_INTEGRATION      = 0x00020000,
    SHOW_SHADING              = 0x00040000,
    SHOW_SHADOWS              = 0x00080000,
    SHOW_EVERY_PIXEL          = 0x00100000,
    SHOW_CUTS_SHADED          = 0x00200000,
    SHOW_CUTS_GRAYSCALE       = 0x00400000,
  }; // vxOptionBitEnum
  
  enum vxRenderingAlgorithmEnum
  {
    DEFAULT   = 0,
    XRAY      = 1,
    THIN_SLAB = 2,
    MIP       = 3,    
  }; // vxRenderingAlgorithmEnum


  enum vxSliceShadowModeEnum
  {
    SHADOWS_NONE           = 0,
    SHADOWS_ALL            = 1,
    SHADOWS_ACTIVE         = 2,
    SHADOWS_ACTIVE_IMAGE   = 3,
    SHADOWS_ACTIVE_CLIPPED = 4,
  }; // enum vxSliceShadowEnum


  enum vxRenderingQualityEnum
  {
    QUALITY_DEFAULT         = 0,
    QUALITY_ENDOLUMINAL     = 1,
    QUALITY_HIGHEST         = 2,
  }; // enum vxRenderingQualityEnum
 
// functions
public:

  /// constructor
  vxRenderingMode();

  /// copy constructor
  ~vxRenderingMode() {}

  /// copy constructor
  vxRenderingMode(const vxRenderingMode & other);

  /// assignment operator
  const vxRenderingMode & operator =(const vxRenderingMode & other);

  /// set the bound of the viewport area relative to the window size.
  void SetEnabled(vxRenderingMode::vxOptionBitEnum eOptionBit, bool bFlag);

  /// retrieve the bound of the viewport area relative to the window size.
  bool IsEnabled(vxRenderingMode::vxOptionBitEnum eOptionBit) const;
  
  /// sets the rendering algorithm
  void SetRenderingAlgorithm( vxRenderingAlgorithmEnum eMode ) { m_eMode = eMode; }
  
  /// gets the rendering algorithm
  vxRenderingAlgorithmEnum GetRenderingAlgorithm() const { return m_eMode; }
  
  /// set the bound of the viewport area relative to the window size.
  void SetSlabThickness(float4 fThickness) { m_fSlabThickness = fThickness; };

  /// retrieve the bound of the viewport area relative to the window size.
  float4 GetSlabThickness()  const{ return m_fSlabThickness; };

  /// sets the slice shadow mode
  void SetSliceShadowMode( vxSliceShadowModeEnum eSliceShadowMode ) { m_eSliceShadowMode = eSliceShadowMode; }

  /// gets the slice shadow mode
  const vxSliceShadowModeEnum GetSliceShadowMode( ) const { return m_eSliceShadowMode; }

  /// returns the option
  uint4 GetOptions() const { return m_uOptions; };

  /// returns the option
  void SetOptions(uint4 uOptions) { m_uOptions = uOptions; };
  
  /// resets the rendering mode to a default state
  void Reset();

  /// get low pass filter counter
  int4 GetLowPassFilterFastFrames() const { return m_iLowPassFilterFastFrames; }

  /// set low pass filter counter
  void SetLowPassFilterFastFrames(int4 iLowPassFilterFastFrames) { m_iLowPassFilterFastFrames = iLowPassFilterFastFrames; }

  /// is fixed size rendering
  bool IsFixedImageSizeRendering() { return m_bFixedImageSizeRendering; }

  /// set fixed size rendering
  void SetFixedImageSizeRendering(const bool & bFlag) { m_bFixedImageSizeRendering = bFlag; }

  /// get the image size used for fixe size rendering
  std::pair<uint2, uint2> GetFixedImageSize() { return m_imageSize; }

  /// set the image size used for fixe size rendering
  void SetFixedImageSize(std::pair<uint2, uint2> & size) { m_imageSize = size; }

  /// get quality speed scale factor
  float4 GetQualityScaleFactor() { return m_fQualitySpeedScaleFactor; }

  /// set quality speed scale factor (1.0 == normal interactive frame rate)
  void SetQualityScaleFactor(float4 fFactor) { m_fQualitySpeedScaleFactor = fFactor; }

  /// get visible data box
  Box<float4> & GetVisibleDataBox() { return m_fVisibeDataBox; }

  /// get current mouse position
  std::pair<float4, float4> GetRelativeMousePosition() const { return m_relativeMousePosition; }

  /// set current mouse position
  void SetRelativeMousePosition(const std::pair<float4, float4> & relativeMousePosition) { m_relativeMousePosition = relativeMousePosition; }

  /// equality operator
  bool operator==( const vxRenderingMode & rhs );
  
  /// non-equality operator
  bool operator!=( const vxRenderingMode & rhs );

  /// Returns the rendering quality
  vxRenderingQualityEnum GetRenderingQuality() const { return m_eRenderingQuality; } 

  /// Sets the rendering quality
  void SetRenderingQuality( vxRenderingQualityEnum eQuality );

// data
private:

  /// current rendering option settings
  uint4 m_uOptions;
  
  /// current rendering algorithm
  vxRenderingAlgorithmEnum m_eMode;

  /// thickness of slab (when using "sliding thin slab")
  float4 m_fSlabThickness;

  /// image size for fixed size rendering
  std::pair<uint2, uint2> m_imageSize; 

  /// ???
  int4 m_iLowPassFilterFastFrames;

  /// quality speed scale factor
  float4 m_fQualitySpeedScaleFactor;

  /// current mouse position
  std::pair<float4, float4> m_relativeMousePosition;

  /// crop box of the visible voxels in VolumeIndexCoordinates
  Box<float4> m_fVisibeDataBox;

  /// fixed image size
  bool m_bFixedImageSizeRendering;
  
  /// slice shadow mode
  vxSliceShadowModeEnum m_eSliceShadowMode;

  /// rendering quality
  vxRenderingQualityEnum m_eRenderingQuality;

}; // class vxRenderingMode


// $Log: vxRenderingMode.h,v $
// Revision 1.13  2007/08/21 19:42:38  gdavidson
// Added SHOW_ANNOTATIONS
//
// Revision 1.12  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.11  2007/01/18 15:44:16  geconomos
// added new slice shadow mode
//
// Revision 1.10  2006/12/21 15:10:01  gdavidson
// Issue #5189 & 5106: Added RenderingQuality
//
// Revision 1.9  2006/10/06 18:52:40  geconomos
// renabled fixed size rendering
//
// Revision 1.8  2006/10/03 20:11:23  geconomos
// added SliceShadowMode enumeration to replace old bit flag
//
// Revision 1.7  2006/07/18 16:18:59  geconomos
// removed unused "SampleDistanceScale" instance field
//
// Revision 1.6  2006/07/07 13:37:48  geconomos
// added operator== and operator!=
//
// Revision 1.5  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.6.3  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.4.6.2  2005/07/19 12:17:01  frank
// added const to const function
//
// Revision 1.4.6.1  2005/07/08 14:58:54  frank
// added rendering modes for 3D cuts
//
// Revision 1.4  2004/11/04 00:23:34  michael
// Added flag to not only set the image size but also indicate that this should be
// used during rendering.
//
// Revision 1.3  2004/10/18 14:45:15  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.2  2004/10/05 14:29:13  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.23  2003/11/11 12:34:13  geconomos
// Addeed Reset() method
//
// Revision 1.22  2003/11/06 20:44:05  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.21  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.20  2003/09/02 15:27:08  frank
// Added slice shadow clipping mode
//
// Revision 1.19  2003/08/27 14:35:43  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.18  2003/08/13 18:42:16  frank
// Added vessel centerline display option
//
// Revision 1.17  2003/08/13 12:43:43  michael
// added derivation for 70_vxUtils
//
// Revision 1.16  2003/08/05 13:36:51  frank
// Implemented copy constructor
//
// Revision 1.15  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.14  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.13  2003/05/06 13:06:06  geconomos
// Code walkthru.
//
// Revision 1.12  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.11  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.10  2003/03/19 19:33:35  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.9  2003/03/07 20:32:31  geconomos
// Added box of interest rendering mode.
//
// Revision 1.8  2002/12/20 16:13:37  frank
// Added x-ray projection
//
// Revision 1.7  2002/12/06 19:20:00  ingmar
// vxRenderingMode::SHOW_LABEL -> SHOW_COMPONENT_IN_MPR
// and added SHOW_COLORMAP_IN_MPR
//
// Revision 1.6  2002/11/14 14:49:17  geconomos
// Added SHOW_3D_CURSOR rendering mode.
//
// Revision 1.5  2002/11/12 23:49:26  michael
// made const methods const
//
// Revision 1.4  2002/11/07 17:24:20  geconomos
// Added TRACK_CURSOR_3D rendering mode.
//
// Revision 1.3  2002/11/01 20:09:08  frank
// Added SHOW_CROP_BOX
//
// Revision 1.2  2002/08/15 22:25:12  frank
// Added LABEL_TOP_TOP mode.
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.15  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.14  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.13  2002/07/09 20:14:06  dmitry
// private assignment op commented out
//
// Revision 1.12  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.11.2.1  2002/07/02 13:45:18  michael
// removed SHOW_ANNOTATIONS_HISTOGRAM since it is not something
// that is embeddedly displayed by another renderer but independent
//
// Revision 1.11  2002/06/13 19:25:32  michael
// added further rendering mode to identify a dragging camera so that lower resolution
// rendering can be done while moving ...
//
// Revision 1.10  2002/05/13 20:55:34  dmitry
// Enum naming changed in RenderingMode.
//
// Revision 1.9  2002/04/26 19:42:59  michael
// added patient information mode and slab thickness
//
// Revision 1.8  2002/04/22 18:59:47  michael
// Added further rendering modes
//
// Revision 1.7  2002/04/22 13:03:30  michael
// changed name of mode
//
// Revision 1.6  2002/04/19 16:23:28  frank
// Added sliding thin slab mode.
//
// Revision 1.5  2002/04/12 15:20:05  jaddonisio
// Coding standard stuff.
//
// Revision 1.4  2002/04/08 18:04:02  jaddonisio
// Corrected render mode enums to all caps.
//
// Revision 1.3  2002/04/01 16:29:41  dmitry
// Added public to the enums.
//
// Revision 1.2  2002/03/29 16:35:02  michael
// added private non-implemented assignment operator and copy constructor
//
// Revision 1.1  2002/03/29 15:31:24  michael
// added rendering mode class
//
// $Id: vxRenderingMode.h,v 1.13 2007/08/21 19:42:38 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingMode.h,v 1.13 2007/08/21 19:42:38 gdavidson Exp $
