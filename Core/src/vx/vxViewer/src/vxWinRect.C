// $Id: vxWinRect.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxWinRect.h"
#include <utility>

// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Constructor
 * @param type of win rectangle
 * @param bound information
 */
vxWinRect::vxWinRect(TypeEnum eType, BoundEnum eBound):
//*******************************************************************
m_eType(eType),
m_eBound(eBound),
m_viewportSize(0,0),
m_proportion(100, 100),
m_corner(0,0),
m_windowSize(0,0),
m_border(0, 0, 0, 0)
{
}


/** 
 * Copy Constructor
 * @param other win rectangle
 */
vxWinRect::vxWinRect(const vxWinRect & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_eType      = other.m_eType;
    m_eBound     = other.m_eBound;
    m_viewportSize       = other.m_viewportSize;
    m_proportion = other.m_proportion;
    m_corner     = other.m_corner; 
    m_windowSize = other.m_windowSize;
    m_border     = other.m_border;
  }
} // copy constructor

 
/** 
 * Assignment Operator
 * @param other win rectangle
 * @return wconst reference to this
 */
const vxWinRect & vxWinRect::operator=(const vxWinRect & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_eType        = other.m_eType;
    m_eBound       = other.m_eBound;
    m_viewportSize = other.m_viewportSize;
    m_proportion   = other.m_proportion;
    m_corner       = other.m_corner; 
    m_windowSize   = other.m_windowSize;
    m_border       = other.m_border;
  }

  return *this;
} // vxWinRect(...)


/**
 * Equality operator
 * @param other the other rectangle
 * @return ==
 */
bool vxWinRect::operator ==(const vxWinRect & other) const
//*******************************************************************
{
  return (m_eType        == other.m_eType       ) &&
         (m_eBound       == other.m_eBound      ) &&
         (m_viewportSize == other.m_viewportSize) &&
         (m_proportion   == other.m_proportion  ) &&
         (m_corner       == other.m_corner      ) &&
         (m_windowSize   == other.m_windowSize  ) &&
         (m_border       == other.m_border      );
} // operator ==()


/**
 * Inequality operator
 * @param other the other rectangle
 * @return !=
 */
bool vxWinRect::operator !=(const vxWinRect & other) const
//*******************************************************************
{
  return !(*this == other);
} // operator !=()


/**
 * Calculate the proportional width of the viewport area.
 * @return width
 */
uint2 vxWinRect::GetProportionalWidth() const
//*******************************************************************
{
  return (m_proportion.first * m_windowSize.first) / 100;
} // GetProportionalWidth()


/** 
 * Calculate the proportional height of the viewport area.
 * @return height
 */
uint2 vxWinRect::GetProportionalHeight() const
//*******************************************************************
{
  return (m_proportion.second * m_windowSize.second) / 100;
} // GetProportionalHeight()


  
/** 
 * Resize the drawing area
 * @param size (pair)
 */
void vxWinRect::SetViewportSize(const std::pair<uint2, uint2> & size)
//*******************************************************************
{
  m_viewportSize = size;
  m_viewportSize.first  -= (m_border.m_left + m_border.m_right);
  m_viewportSize.second -= (m_border.m_top  + m_border.m_bottom);
  UpdateReferencePoint();
} // SetViewportSize()


/** 
 * Resize the drawing area proportion
 * @param proportion (pair)
 */
void vxWinRect::SetProportion(const std::pair<uint2, uint2> & proportion)
//*******************************************************************
{
  m_proportion.first  = Bound(0, int(proportion.first), 100);
  m_proportion.second = Bound(0, int(proportion.second), 100);
  SetViewportSize(std::make_pair(GetProportionalWidth(), GetProportionalHeight()));
} // SetProportion()


/**
 * Set the bound of the drawing area relative to the window size
 * @param bounding type
 */
void vxWinRect::SetBound(BoundEnum eBound)
//*******************************************************************
{
  m_eBound = eBound;
  UpdateReferencePoint();
} // SetBound()


/** 
 * Set the overall window size.
 * @param window size (pair)
 */
void vxWinRect::SetWindowSize(const std::pair<uint2, uint2> & windowSize)  
//*******************************************************************
{
  m_windowSize = windowSize;
  switch(m_eType)
  {
  case FIXED:        
    UpdateReferencePoint(); 
    break;
  case PROPORTIONAL: 
    SetViewportSize(std::make_pair(GetProportionalWidth(), GetProportionalHeight())); 
    break;
  default:
    break; // should never ever happen :-)
  }
} // SetWindowSize()


/**
 * Sets a pixel border around the client rectangle.
 * @param top
 * @param bottom
 * @param left
 * @param right
 */
void vxWinRect::SetBorder(uint2 uTop, uint2 uBottom, uint2 uLeft, uint2 uRight)
//*******************************************************************
{
  m_border.m_left   = uLeft;
  m_border.m_top    = uTop;
  m_border.m_right  = uRight;
  m_border.m_bottom = uBottom;
} // SetBorder()


/**
 * Tests whether a given point is inside the window rectangle
 * @param point position to be checked
 * @return true if inside, else false
 */
bool vxWinRect::IsInside(const Point2D<uint2> & point) const
//*******************************************************************
{
  return (point.X() >= GetReferencePoint().first) && 
         (point.X() < GetReferencePoint().first + GetViewportSize().first) && 
         (GetWindowSize().second - point.Y() >= GetReferencePoint().second) && 
         (GetWindowSize().second - point.Y() < GetReferencePoint().second + GetViewportSize().second);
} // IsInside()


/**
 * Clamp a position in case it is outside the window rectangle
 * @param point position to be checked
 * @return clamped point
 */
Point2D<uint2> vxWinRect::Clamp(const Point2D<uint2> & point) const
//*******************************************************************
{
  Point2D<uint2> clampedPoint(point);

  // check X
  if (point.X() < GetReferencePoint().first)
  {
    clampedPoint.X() = GetReferencePoint().first;
  }
  else if (point.X() >= GetReferencePoint().first + GetViewportSize().first)
  {
    clampedPoint.X() = GetReferencePoint().first + GetViewportSize().first - 1;
  }

  // check Y
  if (GetWindowSize().second - point.Y() < GetReferencePoint().second)
  {
    clampedPoint.Y() = GetReferencePoint().second;
  }
  else if (GetWindowSize().second - point.Y() >= GetReferencePoint().second + GetViewportSize().second)
  {
    clampedPoint.Y() = GetReferencePoint().second + GetViewportSize().second - 1;
  }

  return clampedPoint;
} // Clamp()


/**
 * Repositions the lower left point based on current settings.
 */
void vxWinRect::UpdateReferencePoint()
//*******************************************************************
{
  switch(m_eType)
  {
  case FIXED: 
    switch(m_eBound)
    {
    case LOWER_LEFT:  m_corner = std::make_pair(m_border.m_left, m_border.m_bottom); break;
    case LOWER_RIGHT: m_corner = std::make_pair(int2(m_windowSize.first) - m_viewportSize.first - m_border.m_right, m_border.m_bottom); break;
    case UPPER_LEFT:  m_corner = std::make_pair(m_border.m_left, int2(m_windowSize.second) - m_viewportSize.second - m_border.m_top); break;
    case UPPER_RIGHT: m_corner = std::make_pair(int2(m_windowSize.first) - m_viewportSize.first - m_border.m_right, int2(m_windowSize.second) - m_viewportSize.second - m_border.m_top); break;
    }
    break;
  case PROPORTIONAL: 
    switch(m_eBound)
    {
    case LOWER_LEFT:  m_corner = std::make_pair(m_border.m_left, m_border.m_bottom); break;
    case LOWER_RIGHT: m_corner = std::make_pair(int2(m_windowSize.first) - GetProportionalWidth() + m_border.m_left, m_border.m_bottom); break;
    case UPPER_LEFT:  m_corner = std::make_pair(m_border.m_left, int2(m_windowSize.second) - GetProportionalHeight() + m_border.m_bottom); break;
    case UPPER_RIGHT: m_corner = std::make_pair(int2(m_windowSize.first) - GetProportionalWidth() + m_border.m_left, int2(m_windowSize.second) - GetProportionalHeight() + m_border.m_bottom); break;
    }
    break;
  }
} // UpdateReferencePoint()


#undef FILE_REVISION


// Revision History:
// $Log: vxWinRect.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.10  2004/01/13 14:52:47  frank
// Fixed problem with lack of update when toggling full screen mode of curved mpr view
//
// Revision 1.9  2003/08/20 14:08:50  michael
// added Clamp() to clamp a point to the window rectangle area
//
// Revision 1.8  2003/04/28 17:00:05  michael
// added missing variable in #undef FILE_REVISION
//
// Revision 1.7  2003/04/25 19:16:10  dongqing
// code review
//
// Revision 1.6  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.5  2003/04/16 13:42:25  michael
// performed initial code review
//
// Revision 1.4  2003/01/28 15:02:08  frank
// Const access functions should be const!
//
// Revision 1.3  2002/12/16 20:08:27  michael
// added IsInside() test for point
//
// Revision 1.2  2002/09/24 16:01:32  michael
// added test for == this in copy constructor and assignment operator
//
// Revision 1.1  2002/07/30 20:21:03  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.5  2002/05/01 13:49:00  michael
// Started code review related stuff ...
//
// Revision 1.4  2002/03/27 16:01:49  michael
// adjusted members to be consistent with Get/Set calls
//
// Revision 1.3  2002/03/27 15:07:05  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.2  2002/03/22 00:53:01  geconomos
// added the border to winRect
//
// Revision 1.1  2002/03/19 17:14:43  dmitry
// Added initial version of 74_vxUtils
//
// Revision 1.1  2002/03/19 16:40:25  dmitry
// Moved to 75_vxUtils from 80_vxViewers
//
// Revision 1.1  2002/03/18 15:13:54  dmitry
// Initial revision.
//
// $Id: vxWinRect.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxWinRect.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
