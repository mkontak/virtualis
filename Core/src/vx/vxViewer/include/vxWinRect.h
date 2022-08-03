// $Id: vxWinRect.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)

#ifndef vxWinRect_h
#define vxWinRect_h


// includes
#include "Rect.h"


/**
 * set rectangle window
 */
class VX_VIEWER_DLL vxWinRect
{
// functions
public:

  enum TypeEnum
  {
    FIXED,
    PROPORTIONAL
  }; // TypeEnum

  enum BoundEnum
  {
    UPPER_LEFT,
    UPPER_RIGHT,
    LOWER_LEFT,
    LOWER_RIGHT
  }; // BoundEnum

  /// constructor
  vxWinRect(TypeEnum eType = PROPORTIONAL, BoundEnum eBound = LOWER_LEFT);

  /// copy constructor
  vxWinRect(const vxWinRect & other);
  
  /// assignment Operator
  const vxWinRect & operator=(const vxWinRect & other);

  /// equality operator
  bool operator ==(const vxWinRect & other) const;

  /// inequality operator
  bool operator !=(const vxWinRect & other) const;

  /// retrieve the  overall window size
  const std::pair<uint2, uint2> & GetWindowSize() const { return m_windowSize; };

  /// set the overall window size
  void SetWindowSize(const std::pair<uint2, uint2> & windowSize);  

  /// retrieve the viewport area
  const std::pair<uint2, uint2> & GetViewportSize() const { return m_viewportSize; }

  /// resize the viewport area
  void SetViewportSize(const std::pair<uint2, uint2> & viewportSize);

  /// retrieves the height of the viewport
  uint4 GetViewportHeight() const { return GetViewportSize().second; }

  /// retrieves the width of the viewport
  uint4 GetViewportWidth() const { return GetViewportSize().first; }

  /// retrieve the viewport area proportion
  const std::pair<uint2, uint2> & GetProportion() const { return m_proportion; }

  /// resize the viewport area proportion
  void SetProportion(const std::pair<uint2, uint2> & proportion);

  /// retrieve the bound of the viewport area relative to the window size
  BoundEnum GetBound() const { return m_eBound; }

  /// set the bound of the viewport area relative to the window size
  void SetBound(BoundEnum);

  /// retrieve the reference corner of the viewport area relative to the window size
  const std::pair<int2, int2> & GetReferencePoint() const { return m_corner; }

  /// sets a pixel border around the client rectangle
  void SetBorder(uint2 uTop, uint2 uBottom, uint2 uLeft, uint2 uRight);

  /// determines whether the given point is inside rectangular area
  bool IsInside(const Point2D<uint2> & point) const;

  /// clamps a possibly outside position to the border of the window rectangle
  Point2D<uint2> Clamp(const Point2D<uint2> & point) const;

private:

  /// repositions the lower left point based on current settings.
  void UpdateReferencePoint();
  
  /// calculate the proportional width of the viewport area.
  uint2 GetProportionalWidth() const;

  /// calculate the proportional height of the viewport area.
  uint2 GetProportionalHeight() const;

// data
private:
  
  /// type of object
  TypeEnum m_eType;

  /// bound of the viewport area
  BoundEnum m_eBound;

  /// overall window size
  std::pair<uint2, uint2> m_windowSize;

  /// viewport size
  std::pair<uint2, uint2> m_viewportSize;

  /// viewport area proportion
  std::pair<uint2, uint2> m_proportion;

  /// lower left corner of the viewport area
  std::pair<int2, int2> m_corner;

  /// pixel border around the client rectangle
  Rect<uint2> m_border;
}; // class vxWinRect


#endif // vxWinRect_h


// Revision History:
// $Log: vxWinRect.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.9  2004/01/13 14:52:48  frank
// Fixed problem with lack of update when toggling full screen mode of curved mpr view
//
// Revision 1.8  2003/08/20 14:08:50  michael
// added Clamp() to clamp a point to the window rectangle area
//
// Revision 1.7  2003/04/25 19:16:10  dongqing
// code review
//
// Revision 1.6  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.5  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.4  2003/04/16 13:42:25  michael
// performed initial code review
//
// Revision 1.3  2003/01/28 15:02:08  frank
// Const access functions should be const!
//
// Revision 1.2  2002/12/16 20:08:26  michael
// added IsInside() test for point
//
// Revision 1.1  2002/07/30 20:21:03  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.8  2002/07/19 16:27:04  dmitry
// added some temporary functions
//
// Revision 1.7  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.6  2002/05/07 18:20:59  frank
// Added proper include file.
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
// $Id: vxWinRect.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxWinRect.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
