// $Id: vxRenderingMode.C,v 1.16 2007/08/21 19:42:38 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// include declarations
#include "StdAfx.h"
#include "vxRenderingMode.h"


/**
 * Constructor
 */
vxRenderingMode::vxRenderingMode()
:m_fSlabThickness(10.0F),
m_fQualitySpeedScaleFactor(1.0F),
m_iLowPassFilterFastFrames(-1),
m_bFixedImageSizeRendering(false),
m_eSliceShadowMode( vxRenderingMode::SHADOWS_NONE ),
m_eRenderingQuality( vxRenderingMode::QUALITY_DEFAULT )
//*******************************************************************
{
  Reset();
} // constructor


/**
 * Resets the rendering mode to a default state.
 */
void vxRenderingMode::Reset()
//*******************************************************************
{
  m_eMode = vxRenderingMode::DEFAULT;

  // set all modes off, then turn on a few select ones
  SetOptions(0);
  SetEnabled(vxRenderingMode::SHOW_PATIENT_INFORMATION, true);
  SetEnabled(vxRenderingMode::SHOW_IMAGE_INFORMATION, true);
  SetEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR, true);
  SetEnabled(vxRenderingMode::SHOW_SHADING, true);
  SetEnabled(vxRenderingMode::SHOW_PRE_INTEGRATION, true);
  SetEnabled(vxRenderingMode::SHOW_CUTS_GRAYSCALE, true);
  SetEnabled(vxRenderingMode::SHOW_ANNOTATIONS, true);

  // set light source to middle of image
  m_relativeMousePosition = std::pair<float4, float4>(0.5F, 0.5F);
  m_imageSize = std::pair<uint2, uint2>(512, 512);
} // Reset()


/**
 * Copy constructor
 * @param other the other rendering mode
 */
vxRenderingMode::vxRenderingMode(const vxRenderingMode & other)
//*******************************************************************
{
  *this = other;
} // copy constructor


/**
 * Assignment operator=
 * @param rendering mode
 * @return const this reference
 */
const vxRenderingMode & vxRenderingMode::operator=(const vxRenderingMode & rhs)
//*******************************************************************
{
  if( this != &rhs )
  {
    m_eMode                    = rhs.m_eMode;
    m_uOptions                 = rhs.m_uOptions;
    m_fSlabThickness           = rhs.m_fSlabThickness;
    m_fQualitySpeedScaleFactor = rhs.m_fQualitySpeedScaleFactor;
    m_imageSize                = rhs.m_imageSize;
    m_relativeMousePosition    = rhs.m_relativeMousePosition;
    m_iLowPassFilterFastFrames = rhs.m_iLowPassFilterFastFrames;
    m_eSliceShadowMode         = rhs.m_eSliceShadowMode;
	m_bFixedImageSizeRendering = rhs.m_bFixedImageSizeRendering;
  }
  return * this;
} // operator=(const vxRenderingMode & rhs)


/**
 * Equality operator
 *
 * @param   rhs   other vxRenderingMode instance
 */
bool vxRenderingMode::operator==( const vxRenderingMode & rhs )
{
  bool bAreEqual( true );
  if( this != &rhs )
  {
    bAreEqual = 
      m_eMode                    == rhs.m_eMode &&
      m_uOptions                 == rhs.m_uOptions && 
      m_fSlabThickness           == rhs.m_fSlabThickness &&
      m_fQualitySpeedScaleFactor == rhs.m_fQualitySpeedScaleFactor &&
      m_imageSize                == rhs.m_imageSize &&
      m_relativeMousePosition    == rhs.m_relativeMousePosition &&
	    m_iLowPassFilterFastFrames == rhs.m_iLowPassFilterFastFrames &&
      m_eSliceShadowMode         == rhs.m_eSliceShadowMode &&
      m_bFixedImageSizeRendering == rhs.m_bFixedImageSizeRendering;
  }
  return bAreEqual;
} // operator==( const vxRenderingMode & rhs )


/**
 * Non-Equality operator
 *
 * @param   rhs   other vxRenderingMode instance
 */
bool vxRenderingMode::operator!=( const vxRenderingMode & rhs )
{
  return !( *this == rhs );
} // operator!=( const vxRenderingMode & rhs )


/** 
 * Is mode enabled?
 * @param mode bit
 * @return success
 */
bool vxRenderingMode::IsEnabled( vxRenderingMode::vxOptionBitEnum eOptionBit ) const
//*******************************************************************
{
  return (GetOptions() & eOptionBit) > 0 ? true : false;
} // IsEnabled()


/** 
 * Is mode enabled?
 * @param mode bit
 * @param on/off
 */
void vxRenderingMode::SetEnabled(vxRenderingMode::vxOptionBitEnum eOptionBit, 
                                 bool bFlag)
//*******************************************************************
{
  if (bFlag == true)
  {
    SetOptions(GetOptions() | eOptionBit);
  }
  else
  {
    uint4 uMask(0xFFFFFFFF ^ eOptionBit);
    SetOptions(GetOptions() & uMask);
  }

  return;
} // SetEnabled()


/** 
 * Set rendering quality level
 */
void vxRenderingMode::SetRenderingQuality( vxRenderingQualityEnum eQuality )
{
  switch( eQuality )
  {
  case QUALITY_ENDOLUMINAL:
    SetEnabled( vxRenderingMode::SHOW_JITTERING, true );
    SetEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION, false );
    SetEnabled( vxRenderingMode::SHOW_EVERY_PIXEL, false );
    break;
  case QUALITY_HIGHEST:
    SetEnabled( vxRenderingMode::SHOW_JITTERING, false );
    SetEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION, true );
    SetEnabled( vxRenderingMode::SHOW_EVERY_PIXEL, true );
    break;
  case QUALITY_DEFAULT:
  default:
    SetEnabled( vxRenderingMode::SHOW_JITTERING, false );
    SetEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION, true );
    SetEnabled( vxRenderingMode::SHOW_EVERY_PIXEL, false );
    break;
  }
} // SetRenderingQuality()


// $Log: vxRenderingMode.C,v $
// Revision 1.16  2007/08/21 19:42:38  gdavidson
// Added SHOW_ANNOTATIONS
//
// Revision 1.15  2006/12/21 15:10:01  gdavidson
// Issue #5189 & 5106: Added RenderingQuality
//
// Revision 1.14  2006/12/11 15:50:45  geconomos
// changed default value for quality scale factor to be comptaible with new rendering manager
//
// Revision 1.13  2006/10/06 18:52:40  geconomos
// renabled fixed size rendering
//
// Revision 1.12  2006/10/03 20:05:14  geconomos
// added SliceShadowMode enumeration to replace old bit flag
//
// Revision 1.11  2006/07/18 16:18:59  geconomos
// removed unused "SampleDistanceScale" instance field
//
// Revision 1.10  2006/07/07 13:37:48  geconomos
// added operator== and operator!=
//
// Revision 1.9  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.8  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7.6.2  2005/07/19 12:17:01  frank
// added const to const function
//
// Revision 1.7.6.1  2005/07/08 14:58:54  frank
// added rendering modes for 3D cuts
//
// Revision 1.7  2004/11/24 03:53:07  michael
// 1 (lowest) and 10 (highest) are both not perfect, trying 7 as defaul speed/quality trade-off
//
// Revision 1.6  2004/11/04 00:23:34  michael
// Added flag to not only set the image size but also indicate that this should be
// used during rendering.
//
// Revision 1.5  2004/10/20 20:18:31  frank
// initialized the rendering quality to highest level (set back to 1 for lowest)
//
// Revision 1.4  2004/10/18 14:45:15  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.3  2004/10/05 14:35:17  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.2  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2004/01/19 19:07:36  michael
// components in MPR are now again on as default. This issue should be solved
// once we save a session including the last used rendering mode in 2D and 3D
// which will then preserve whatever was there.
//
// Revision 1.14  2003/11/25 17:35:12  michael
// made default of SHOW_COMPONENT_IN_MPR false
//
// Revision 1.13  2003/11/11 12:34:13  geconomos
// Addeed Reset() method
//
// Revision 1.12  2003/11/06 20:44:05  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.11  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.10  2003/09/22 15:54:30  michael
// cosmetics
//
// Revision 1.9  2003/08/05 13:36:51  frank
// Implemented copy constructor
//
// Revision 1.8  2003/07/29 18:34:29  michael
// added mode to be able to prevent progressive refinement and set image size fixed.
//
// Revision 1.7  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.6  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.5  2003/05/06 13:06:06  geconomos
// Code walkthru.
//
// Revision 1.4  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.3  2002/12/12 16:04:41  michael
// default no colors in 2D
//
// Revision 1.2  2002/12/09 21:42:50  ingmar
// new default presets with gray and color side by side
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.13  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.12  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.11  2002/07/10 11:44:30  geconomos
// exported classes.
//
// Revision 1.10.2.1  2002/07/09 19:53:25  dmitry
// default slab thickness changed to 10mm
//
// Revision 1.10  2002/06/18 20:59:10  michael
// changed default to be off (consistent with button)
//
// Revision 1.9  2002/04/29 16:09:19  michael
// fixed stupid initialization
//
// Revision 1.8  2002/04/29 16:08:03  michael
// Initializing slab thickness to 30 (in constructor)
//
// Revision 1.7  2002/04/27 04:18:51  frank
// Defaulted patient info on.
//
// Revision 1.6  2002/04/22 19:16:05  geconomos
// Fixed Compile bug.
//
// Revision 1.5  2002/04/22 18:59:47  michael
// Added further rendering modes
//
// Revision 1.4  2002/04/12 15:20:05  jaddonisio
// Coding standard stuff.
//
// Revision 1.3  2002/04/08 19:25:51  michael
// default is show labels
//
// Revision 1.2  2002/04/06 00:17:11  michael
// initialize mode in constructor
//
// Revision 1.1  2002/03/29 15:31:24  michael
// added rendering mode class
//
// $Id: vxRenderingMode.C,v 1.16 2007/08/21 19:42:38 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingMode.C,v 1.16 2007/08/21 19:42:38 gdavidson Exp $
