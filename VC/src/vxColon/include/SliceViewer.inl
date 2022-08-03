// $Id: SliceViewer.inl,v 1.3.2.3 2009/05/21 13:46:35 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.net)

// SliceViewer.inl: inline functions in Class SliceViewer.

/**
 * Returns the number of slices
 *
 * @return   the number of slices for this orientation
 */
inline uint4 SliceViewer::GetNumberOfSlices() const
{
  if (IsOrthogonal() || IsCrossSectional()) 
  {
    return m_uNumOfSlices;
  }
  else 
  {
    // Maximum distance from xsec/view dir
    return 100;
  }
} // GetNumberOfSlices()


/**
 * get the orientation of the slice: Axial/Sagittal/Coronal.
 *
 * @return the orientation of the slice
 */
SliceOrientations::TypeEnum SliceViewer::GetOrientation() const
{
  return m_eSliceOrientation;
} // GetOrientation().


/**
 * Current slice orientation is Axial. 
 *
 * @return true when orientation is axial. 
 */
inline bool SliceViewer::IsAxial() const
{
  // should we use the current viewpoint
  return (m_eSliceOrientation== SliceOrientations::eAXIAL);
} // IsAxial()


/*
 * Current slice orientation is Orthogonal.
 *
 * @return true when orientation is Orthogonal. 
 */
inline bool SliceViewer::IsOrthogonal() const
{
  // should we use the current viewpoint
  return ( (m_eSliceOrientation == SliceOrientations::eAXIAL) 
    || (m_eSliceOrientation == SliceOrientations::eSAGITTAL) 
    || (m_eSliceOrientation == SliceOrientations::eCORONAL));
} // IsOrthogonal()


/*
 * Current slice orientation is Cross sectional.
 *
 * @return true when orientation is cross-sectonal. 
 */
inline bool SliceViewer::IsCrossSectional() const
{
  // should we use the current viewpoint
  return (m_eSliceOrientation== SliceOrientations::eCROSS_SECTION);
} // IsCrossSectional()


/**
 * Get the number of the last slice for caching purposes.
 *
 * @return the number of slices.
 */
inline int4 SliceViewer::GetSliceNumber() const
{ 
  return m_iSliceNumber; 
} // GetSliceNumber()


/**
 * Set the number of the last slice for caching purposes.
 *
 * @param num the slice number to be set,
 */
inline void SliceViewer::SetSliceNumber(const int4 iNum) 
{ 
  m_iSliceNumber = iNum; 
} // SetSliceNumber()


/**
 * switch between rendering of 'blocked' or 'smoothed' pixels.
 */
inline void SliceViewer::SwitchBlockAndSmooth() 
{ 
  m_bBlockedPixel = !m_bBlockedPixel; 
} // SwitchBlockAndSmooth()


/**
 * Returns whether or not the pixels are blocked
 */
inline bool SliceViewer::GetBlockPixels() const
{
  return m_bBlockedPixel;
} // GetBlockPixels()


/**
 * Get value of 'm_bDisplayOverviewSliceGrasp'.
 *
 * @return   the value of m_bDisplayOverviewSliceGrasp
 */
/// Get value of 'm_bDisplayOverviewSliceGrasp'.
inline bool SliceViewer::GetValueOfDisplayOverviewSliceGrasp() const
{ 
  return m_bDisplayOverviewSliceGrasp; 
} // GetValueOfDisplayOverviewSliceGrasp()


/**
 * Set value of 'm_bDisplayOverviewSliceGrasp'.
 *
 * @param bValue  the value to be set.
 */
inline void SliceViewer::SetValueOfDisplayOverviewSliceGrasp(const bool bValue) 
{
  m_bDisplayOverviewSliceGrasp = bValue; 
} // SetValueOfDisplayOverviewSliceGrasp()


/**
 * Toggle view position indicator
 */
inline void SliceViewer::ToggleRenderCamera() 
{
  SliceViewer::m_bRenderCamera = !m_bRenderCamera; 
} // ToggleRenderCamera()


/**
 * Get value of 'm_bRenderCamera'.
 *
 * @return current setting of boolean variable m_bRenderCamera
 *         true: the camera is shown, else, false.
 */
inline bool SliceViewer::GetStatusOfRenderCamera()
{
  return SliceViewer::m_bRenderCamera; 
} // GetStatusOfRenderCamera()


/**
 * Set value of 'm_bRenderCamera'.
 * @param value boolean variable indicates showing arrow(camera) or not.
 */
inline void SliceViewer::SetStatusOfRenderCamera(const bool bValue)
{
  SliceViewer::m_bRenderCamera = bValue; 
} // SetStatusOfRenderCamera()


/**
 * Get the zoom factor.
 * @return the zoom factor
 */
inline const int4 SliceViewer::GetZoomPercentage() const
{
  return int((m_rZoom.Height()) * 100.0f);
} // GetZoomPercentage()


/**
 * Set dataset type
 * @param eOrientation  dataset orientation
 */
inline void SliceViewer::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_eDatasetType = eOrientation;
} // SetDatasetType()


/**
 * Text to display on top of the window.
 * @return a character to be displayed on top of window.
 */
inline const int1 * SliceViewer::TopText() const
{
  switch (m_eSliceOrientation) 
  {
  case SliceOrientations::eAXIAL:
    return "A";
    break;
  case SliceOrientations::eSAGITTAL:
    return "S";
    break;
  case SliceOrientations::eCORONAL:
    return "S";
    break;
  case SliceOrientations::eCROSS_SECTION:
  case SliceOrientations::eVIEWDIR:
    return("");
    break;
  default:
    return ("t");
    break;
  } // endswitch m_eSliceOrientation
} // TopText()


/**
 * Text to display on bottom of the window.
 * @return a character to be displayed on bottwm of window.
 */
inline const int1 * SliceViewer::BottomText() const
{
  switch (m_eSliceOrientation) 
  {
  case SliceOrientations::eAXIAL:
    return "P";
    break;
  case SliceOrientations::eSAGITTAL:
    return "I";
    break;
  case SliceOrientations::eCORONAL:
    return "I";
    break;
  case SliceOrientations::eCROSS_SECTION:
  case SliceOrientations::eVIEWDIR:
    return("");
    break;
  default:
    return ("b");
    break;
  } // endswitch m_eSliceOrientation
} // BottomText()


/** 
 * Text to display on right of the window.
 * @return a character to be displayed on Right of window.
 */
inline const int1 * SliceViewer::RightText() const
{
  switch (m_eSliceOrientation) 
  {
  case SliceOrientations::eAXIAL:
    return "L";
    break;
  case SliceOrientations::eSAGITTAL:
    return "P";
    break;
  case SliceOrientations::eCORONAL:
    return "L";
    break;
  case SliceOrientations::eCROSS_SECTION:
  case SliceOrientations::eVIEWDIR:
    return("");
    break;
  default:
    return ("r");
    break;
  } // endswitch m_eSliceOrientation
} // RightText()


/** 
 * Text to display on left of the window.
 * @return a character to be displayed on Left of window.
 */
inline const int1 * SliceViewer::LeftText() const
{
  switch (m_eSliceOrientation) 
  {
  case SliceOrientations::eAXIAL:
    return "R";
    break;
  case SliceOrientations::eSAGITTAL:
    return "A";
    break;
  case SliceOrientations::eCORONAL:
    return "R";
    break;
  case SliceOrientations::eCROSS_SECTION:
  case SliceOrientations::eVIEWDIR:
    return("");
    break;
  default:
    return ("l");
    break;
  } // endswitch m_eSliceOrientation
} // LeftText()


/** 
 * adjust m_rZoom: more visible.
 */
inline void SliceViewer::ZoomAdjust()
{
  Rect<float4> boundsAdjust(0,0,0,0);

  /// if the slice is CrossSection/ViewDir, always zoom around center.
  if ( ((m_rZoom.m_right - m_rZoom.m_left)<1.0F || (m_rZoom.m_bottom - m_rZoom.m_top)<1.0F) 
     || (m_eSliceOrientation == SliceOrientations::eVIEWDIR || m_eSliceOrientation == SliceOrientations::eCROSS_SECTION) )
  {
    float4 centerX = (m_rZoom.m_right + m_rZoom.m_left)/2.0F;
    float4 centerY = (m_rZoom.m_bottom + m_rZoom.m_top)/2.0F;
    boundsAdjust.m_left = 0.5F - centerX;
    boundsAdjust.m_top  = 0.5F - centerY;
    m_rZoom.Offset(boundsAdjust.m_left, boundsAdjust.m_top);
  }
  else
  {
    if (m_rZoom.m_left > 0.0F)
    {
      boundsAdjust.m_left = -m_rZoom.m_left;
    }
    if (m_rZoom.m_top > 0.0F)
    {
      boundsAdjust.m_top = -m_rZoom.m_top;
    }
    m_rZoom.Offset(boundsAdjust.m_left, boundsAdjust.m_top);

    if (m_rZoom.m_right < 1.0F)
    {
      boundsAdjust.m_right = 1.0F - m_rZoom.m_right;
    }
    if (m_rZoom.m_bottom < 1.0F) 
    {
      boundsAdjust.m_bottom = 1.0F - m_rZoom.m_bottom;
    }
    m_rZoom.Offset(boundsAdjust.m_right, boundsAdjust.m_bottom);
  } // offset for better zooming.
} // ZoomAdjust().



// $Log: SliceViewer.inl,v $
// Revision 1.3.2.3  2009/05/21 13:46:35  dongqing
// after change the display orientation of sagittal view, we need to change the
// orientation indication text accordingly
//
// Revision 1.3.2.2  2009/01/21 22:02:59  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.3.2.1  2007/03/15 21:58:30  dongqing
// 80% done on X-section view enhancements. CAD mark in
//
// Revision 1.3  2006/11/21 23:45:25  jmeade
// Issue 5180: don't draw cad marks in x-sec/viewdir orientations.
//
// Revision 1.2  2006/02/28 17:06:56  jmeade
// code review.
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.5  2002/11/24 17:47:06  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.4  2002/04/12 02:15:09  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.3  2002/01/22 22:45:54  binli
// no change
//
// Revision 3.2  2001/12/21 03:53:26  jmeade
// CODING STANDARDS!
//
// Revision 3.1  2001/11/12 18:49:20  binli
// 2D (slice) flight: CrossSec/ViewDir slice, always zooming around the center
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:16   ingmar
// Initial revision.
// Revision 1.8  2001/08/22 20:49:22  sarang
// bug-fix: ZoomAdjust w/o type. gcc dosn't like this and this is not good coding!
//
// Revision 1.7  2001/08/21 20:23:46  binli
// bug 612: fixed
//
// Revision 1.6  2001/07/03 15:20:38  binli
// better 2D slice zooming.
//
// Revision 1.5  2001/06/27 15:06:46  jmeade
// oops, missed a const
//
// Revision 1.4  2001/06/27 15:04:04  jmeade
// coding conventions, const where appropriate, tabs to spaces
//
// Revision 1.3  2001/05/31 22:21:15  binli
// some works for code conventions and bug fixing (000489, 484)
//
// Revision 1.2  2001/03/27 21:33:20  binli
// continued code walkthrough.
//
// Revision 1.1  2001/03/20 18:48:41  binli
// moved inline functions from SliceViewer.C to .inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SliceViewer.inl,v 1.3.2.3 2009/05/21 13:46:35 dongqing Exp $
// $Id: SliceViewer.inl,v 1.3.2.3 2009/05/21 13:46:35 dongqing Exp $
