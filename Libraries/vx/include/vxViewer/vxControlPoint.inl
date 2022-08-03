// $Id: vxControlPoint.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// 
// Copyright © 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: dima dmitry@viatronix.com

/**
 *
 */
const Point2D<ZeroOne>& vxControlPoint::GetControl() const
{
  return (m_control);
} // 

/**
 * @param bSet
 */
Point2D<ZeroOne> vxControlPoint::GetLeft(const bool& bSet) const
{
  if (bSet)
  {
    return Point2D<ZeroOne>(m_control.m_x + m_ldelta.m_x, m_control.m_y + m_ldelta.m_y);
  }
  return m_left;
} // 

/**
 * @param bSet
 */
Point2D<ZeroOne> vxControlPoint::GetRight(const bool& bSet) const
{
  if (bSet)
  {
    return Point2D<ZeroOne>(m_control.m_x + m_rdelta.m_x, m_control.m_y + m_rdelta.m_y);
  }
  return m_right;
} // 


/**
 * get continuity
 */
ZeroOne vxControlPoint::GetContinuity() const
{
  return m_continuity;
} // GetContinuity


/**
 * 
 */
ZeroOne vxControlPoint::GetRed() const
{
  return (m_red);
} // GetRed

/**
 *
 */
ZeroOne vxControlPoint::GetGreen() const
{
  return (m_green);
} // GetGreen

/**
 *
 */
ZeroOne vxControlPoint::GetBlue() const
{
  return (m_blue);
} // GetBlue

/**
 *
 */
ZeroOne vxControlPoint::GetAlpha() const
{
  float4 fAlpha(m_control.m_y);
  fAlpha = pow(fAlpha, 2.0f);
  return fAlpha;
} // GetAlpha

/**
 *
 */
vxControlPoint::Property vxControlPoint::GetProperty() const
{
  return m_eProperty;
} // GetProperty

/**
 * @param control
 */
void vxControlPoint::SetControl(const Point2D<ZeroOne>& control)
{
  m_control = control;
} // SetControl

/**
 * @param left
 * @param bSet
 */
void vxControlPoint::SetLeft(const Point2D<ZeroOne>& left, const bool& bSet)
{
  m_left = left;

  if (bSet)
  {
    m_ldelta.m_x = left.m_x - m_control.m_x;
    m_ldelta.m_y = left.m_y - m_control.m_y;
  }
} // SetLeft

/**
 * @param right
 * @param bSet
 */
void vxControlPoint::SetRight(const Point2D<ZeroOne>& right, const bool& bSet)
{
    m_right = right;

    if (bSet)
    {
      m_rdelta.m_x = right.m_x - m_control.m_x;
      m_rdelta.m_y = right.m_y - m_control.m_y;
    }
} // SetRight


/**
 * @param continuity
 */
void vxControlPoint::SetContinuity(const ZeroOne& continuity)
{
  m_continuity = continuity;
} // SetContinuity


/**
 * @param red
 * @param green
 * @param blue
 */
void vxControlPoint::SetColor(const ZeroOne& red, const ZeroOne& green, const ZeroOne& blue)
{
  m_red   = red;
  m_green = green;
  m_blue  = blue;
} // SetColor

/**
 * @param alpha
 */
void vxControlPoint::SetAlpha(const ZeroOne& alpha)
{
  m_control.m_y = alpha;
} // SetAlpha

/**
 * @param eProperty
 */
void vxControlPoint::SetProperty(const vxControlPoint::Property eProperty)
{
  m_eProperty = eProperty;
} // SetProperty

/**
 * @param eColorIndex
 */
const ZeroOne vxControlPoint::GetColor(const nsCOLOR::COLOR_CHANNEL eColorIndex) const
{
  switch(eColorIndex)
  {
  case nsCOLOR::RED:
    return GetRed();
  case nsCOLOR::GREEN:
    return GetGreen();
  case nsCOLOR::BLUE:
    return GetBlue();
  case nsCOLOR::ALPHA:
    return GetAlpha();
  default:
    return ZeroOne(); //return something
  }
} // GetColor

/**
 * set color
 * @param value
 * @param eColorIndex
 */
void vxControlPoint::SetColor(const ZeroOne& value, const nsCOLOR::COLOR_CHANNEL eColorIndex)
{
  switch(eColorIndex)
  {
  case nsCOLOR::RED:   m_red   = value;
  case nsCOLOR::GREEN: m_green = value;
  case nsCOLOR::BLUE:  m_blue  = value;
  default:;
    //do nothing
  }
} // SetColor


// $Log: vxControlPoint.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/22 16:34:27  dongqing
// code review
//
// Revision 1.4  2003/05/16 13:19:56  frank
// formatting
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2002/09/25 22:08:34  ingmar
// code beautify
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 3.8  2001/12/17 17:21:32  soeren
// Removed some warnings
//
// Revision 3.7  2001/10/25 20:12:44  dmitry
// Continuity percentile saved and retrieved.
//
// Revision 3.6  2001/10/23 22:26:28  dmitry
// GetAlpha updated, TBD: Update SetAlpha.
//
// Revision 3.5  2001/10/22 16:09:17  dmitry
// Fixed delta get and set.
//
// Revision 3.4  2001/10/19 22:22:33  geconomos
// Removed calls to adjustable curve control.
//
// Revision 3.3  2001/10/19 19:57:33  dmitry
// Added adjusted control to curve ctrl pt.
//
// Revision 3.2  2001/10/18 20:15:35  liwei
// Added SetColor() and GetColor().
//
// Revision 3.1  2001/10/17 17:27:54  dmitry
// debugged, changed all controls to ZeroOne, moved curve as control is reset
//
// 
//    Rev 1.1   Oct 12 2001 17:59:44   liwei
// Added control point list to the transfer function class according to the requirement of the transfer function editor.
// 
//    Rev 1.0   Oct 11 2001 16:27:02   liwei
// Initial Revision
// 
//    Rev 1.1   Oct 11 2001 11:16:46   dmitry
// Fixed infinite recursion error.
// 
//    Rev 1.0   Oct 10 2001 16:55:12   dmitry
// Initial Revision
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControlPoint.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxControlPoint.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
