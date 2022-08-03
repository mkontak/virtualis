// $Id: vxControlPoint.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// 
// Copyright © 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: dima dmitry@viatronix.com

/**
 * define the control points
 */

#ifndef vxControlPoint_h
#define vxControlPoint_h


// includes
#include "Point2D.h"
#include "ZeroOne.h"
#include "enums.h"


// class definition
class vxControlPoint
{
public:
  /// type
  enum Property 
  {
    SPECULA = 0, 
    DIFFUSE = 1, 
    AMBIENT = 2
  };

public:
  /// constructor
  vxControlPoint()
  {
  }

  /// constructor
  vxControlPoint(Point2D<ZeroOne>& control, Point2D<ZeroOne>& left, Point2D<ZeroOne>& right, 
    const float4 red = 1.0f, const float4 green = 1.0f, const float4 blue = 1.0F, Property eProperty = SPECULA, float4 gradient = 0.0f):
  m_control(control),
  m_left(left),
  m_ldelta(0,0),
  m_right(right),
  m_rdelta(0,0),
  m_continuity(0.0),
  m_red(red),
  m_green(green),
  m_blue(blue),
  m_eProperty(eProperty),
  m_gradient(gradient)  
  {
  }

  /// constructor
  vxControlPoint(Point2D<ZeroOne>& control):
  m_control(control),
  m_left(control),
  m_ldelta(0,0),
  m_right(control),
  m_rdelta(0,0),
  m_continuity(0.0),
  m_red(1.0f),
  m_green(1.0f),
  m_blue(1.0f),
  m_eProperty(SPECULA),
  m_gradient(0.0f)  
  {
  }

  /// get control
  inline const Point2D<ZeroOne>& GetControl() const;
  /// get left
  inline Point2D<ZeroOne> GetLeft(const bool& bSet = false) const;
  /// get right
  inline Point2D<ZeroOne> GetRight(const bool& bSet = false) const;
  /// get continuity
  inline ZeroOne GetContinuity() const;
  /// get color
  inline const ZeroOne GetColor(const nsCOLOR::COLOR_CHANNEL eColorIndex) const;
  /// get red
  inline ZeroOne GetRed() const;
  /// get green
  inline ZeroOne GetGreen() const;
  /// get blue
  inline ZeroOne GetBlue() const;
  /// get alpha
  inline ZeroOne GetAlpha() const;
  /// get property
  inline Property GetProperty() const;
  /// set control
  inline void SetControl(const Point2D<ZeroOne>& control);
  /// set left
  inline void SetLeft(const Point2D<ZeroOne>& left, const bool& bSet = false);
  /// set right
  inline void SetRight(const Point2D<ZeroOne>& right, const bool& bSet = false);
  /// set continuity
  inline void SetContinuity(const ZeroOne& cotinuity);
  /// set color
  inline void SetColor(const ZeroOne& red, const ZeroOne& green, const ZeroOne& blue);
  /// set color
  inline void SetColor(const ZeroOne& value, const nsCOLOR::COLOR_CHANNEL eColorIndex);
  /// set alpha
  inline void SetAlpha(const ZeroOne& alpha);
  /// set property
  inline void SetProperty(const Property eProperty);
  
private:
  Point2D<ZeroOne> m_control;   // Control Point
  Point2D<ZeroOne> m_left;      // Left Curve Control
  Point2D<float4>  m_ldelta;    // Left set position from control point
  Point2D<ZeroOne> m_right;     // Right Curve Control
  Point2D<float4>  m_rdelta;    // Right set position from control point
  ZeroOne          m_continuity;// Degree of continuity
  ZeroOne          m_red;       // Amount of Red Color
  ZeroOne          m_green;     // Amount of Green Color
  ZeroOne          m_blue;      // Amount of Blue Color
  Property         m_eProperty; // Physical Light Property
  float4           m_gradient;  // Gradient Magnitude
};

#include "vxControlPoint.inl"

#endif

// $Log: vxControlPoint.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.6  2003/05/22 16:34:26  dongqing
// code review
//
// Revision 1.5  2003/05/16 13:19:56  frank
// formatting
//
// Revision 1.4  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.3  2003/01/09 00:30:31  ingmar
// added missing include
//
// Revision 1.2  2002/09/25 22:08:21  ingmar
// removed unimplemented and unuswed functions
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 3.11  2002/05/07 18:32:30  frank
// Removed unneccessary includes.
//
// Revision 3.10  2002/05/02 13:15:38  michael
// took out unused VLIColorTable code
//
// Revision 3.9  2001/12/18 17:15:27  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.8  2001/11/06 12:39:10  geconomos
// Defaulted control points color to white
//
// Revision 3.7  2001/11/01 16:29:08  dmitry
// Added explicit default constructor.
//
// Revision 3.6  2001/10/25 20:12:44  dmitry
// Continuity percentile saved and retrieved.
//
// Revision 3.5  2001/10/22 18:39:38  dmitry
// Updated Constructor
//
// Revision 3.4  2001/10/22 16:09:17  dmitry
// Fixed delta get and set.
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
//    Rev 1.0   Oct 12 2001 15:36:44   liwei
// Initial revision.
// 
//    Rev 1.0   Oct 10 2001 16:55:10   dmitry
// Initial Revision
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControlPoint.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxControlPoint.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
