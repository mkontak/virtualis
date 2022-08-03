// $Id: Rect.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Jeff Meade (jmeade@viatronix.com)
   
#ifndef Rect_h
#define Rect_h


// includes 
#include "Point2D.h"


// namespaces
namespace RectType 
{
	enum eEdgeType { BUMPED, ETCHED, RAISED, SUNKEN };
};


/**
 * Normal 2-D Rectangle class, fortified with some OpenGL convenience methods.
 * <a href="../../VxBase/3_math/Spec/MathDesignSpecification.htm">Design Specification</a>w 
 */
template <class T>
class Rect 
{
// member functions
public:

  /// Default constructor
  Rect<T>(const T & l = T(0), const T & t = T(0), const T & r = T(0), const T & b = T(0));

  /// Point/Size constructor
  Rect<T>(const Point2D<T> & point2D, const Point2D<T> & size2D);	

  /// Copy constructor
  template <class C> inline Rect<T>(const Rect<C> & rect) { *this = rect; }

  /// Set dimensions
  inline Rect<T> & Set(const T & l, const T & t, const T & r, const T & b);

  /// Assignment operator
  template <class C> inline Rect<T> & operator=(const Rect<C> & rect)
  {
	  m_left  = static_cast<T>(rect.m_left);	
	  m_right = static_cast<T>(rect.m_right);
	  m_top   = static_cast<T>(rect.m_top);		
	  m_bottom= static_cast<T>(rect.m_bottom);
	  return *this;
  }

  /// True if {x,y} is inside of rectangle
  inline bool PtInRect(const T & x, const T & y) const;

  /// True if pt is inside of rectangle
  inline bool PtInRect(const Point2D<T> & point2D) const;

  /// Rectangle's width
  inline const T Width() const;

  /// Rectangle's height
  inline const T Height() const;

  /// Rectangle's width
  inline const T WidthAbs() const;

  /// Rectangle's height
  inline const T HeightAbs() const;

  /// Bounding Point method: CenterPoint
  inline Point2D<T> CenterPoint() const;

  /// Bounding Point method: LeftTop
  inline Point2D<T> LeftTop() const;

  /// Bounding Point method: RightTop
  inline Point2D<T> RightTop() const;
  
  /// Bounding Point method: LeftBottom
  inline Point2D<T> LeftBottom() const;

  /// Bounding Point method: RightBottom
  inline Point2D<T> RightBottom() const;

  /// Offset left&right by x, top&bottom by y
  inline Rect<T> & Offset(const T & x, const T & y);

  /// Offset by Point2D
  inline Rect<T> & Offset(const Point2D<T> & point2D);

  /// Return offset left&right by x, top&bottom by y
  inline const Rect<T> GetOffset(const T & x, const T & y) const;

  /// Return offset by Point2D
  inline const Rect<T> GetOffset(const Point2D<T> &t) const;

  /// Intersection of 2 rects.
  inline const Rect<T> Intersect(const Rect<T> & anotherRect) const;

  /// Unary - operator
  inline Rect<T> operator-();

  /// Binary + operator: takes a Point2D
  inline Rect<T> operator+(const Point2D<T> & point2D) const;

  /// Binary + operator: takes anoter Rect
  inline Rect<T> operator+(const Rect<T> & rect) const;

  /// Binary - operator: takes a Point2D
  inline Rect<T> operator-(const Point2D<T> & point2D) const;

  /// Binary - operator: takes another Rect
  inline Rect<T> operator-(const Rect<T> & rect) const;

  /// Binary += operator: takes a Point2D
  inline Rect<T> & operator+=(const Point2D<T> & point2D);

  /// Binary += operator: takes a Rect
  inline Rect<T> & operator+=(const Rect<T> & rect);

  /// Binary -= operator: takes a Point2D
  inline Rect<T> & operator-=(const Point2D<T> & point2D);

  /// Binary -= operator: takes a Rect
  inline Rect<T> & operator-=(const Rect<T> & rect2D);

  /// Binary == operator: takes another Rect
  inline bool operator ==(const Rect<T> & rect) const;

  /// Binary != operator: takes another Rect
  inline bool operator !=(const Rect<T> & rect) const;

  /// Inflate left&right by w, top&bottom by h
  inline Rect<T> & Inflate(const T & w, const T & h);
  
  /// Inflate by Point2D
  inline Rect<T> & Inflate(const Point2D<T> & point2D);
  
  /// Inflate by Rect
  inline Rect<T> & Inflate(const Rect<T> & rect);
  
  /// Inflate by l,t,r,b coordinates
  inline Rect<T> & Inflate(const T & l, const T & t, const T & r, const T & b);

  /// Deflate left&right by w, top&bottom by h
  inline Rect<T> & Deflate(const T & w, const T & h);

  /// Deflate by Point2D
  inline Rect<T> & Deflate(const Point2D<T> & point2D);

  /// Deflate by Rect
  inline Rect<T> & Deflate(const Rect<T> & rect);

  /// Deflate by l,t,r,b coordinates
  inline Rect<T> & Deflate(const T & l, const T & t, const T & r, const T & b);

  /// Keep width and height positive by swapping left/right &/or top/bottom if necessary
  inline Rect<T> & Normalize();
  inline const Rect<T> GetNormalized() const;

  /// OpenGL Drawing convenience function Draw3dEtched
  inline void Draw3dEtched() const;

  /// OpenGL Drawing convenience function Draw3dRect
  inline void Draw3dRect() const;

  /// OpenGL Drawing convenience function Draw3dEdge
  inline void Draw3dEdge(const RectType::eEdgeType & type) const;

  /// OpenGL Drawing convenience function Draw3dEdge
  inline void Draw3dEdge(const RectType::eEdgeType & type, const float4 vfLightColor[3], const float4 vfDarkColor[3]) const;

  /// ConstraintPt
  inline const Rect<T> & ConstrainPt(T & x, T & y) const;

  /// Test function to verify and demonstrate the class functionality.
  static bool Test(std::ostream & os=std::cout);

  /// Test function to verify just one particular template instance 
  static bool TestByType(std::ostream & os=std::cout);

  inline const T & GetMinX() const { return (m_left < m_right) ? m_left : m_right; }

  inline const T & GetMaxX() const { return (m_left > m_right) ? m_left : m_right; }
  
  inline const T & GetMinY() const { return (m_bottom < m_top) ? m_bottom: m_top; }

  inline const T & GetMaxY() const { return (m_bottom > m_top) ? m_bottom: m_top; }

public:

  /// the left
	T m_left;
  
  /// the top
  T m_top;
  
  /// the right
  T m_right;
  
  /// the bottom
  T m_bottom;

}; // class Rect

#include "Rect.inl"

#endif // Rect_h

// $Log: Rect.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.12  2004/01/13 14:52:55  frank
// Fixed problem with lack of update when toggling full screen mode of curved mpr view
//
// Revision 3.11  2003/05/22 16:34:57  dongqing
// code review
//
// Revision 3.10  2003/05/16 13:14:52  frank
// formatting
//
// Revision 3.9  2003/02/05 22:31:25  jmeade
// Override of Draw3dRect that allows setting of colors.
//
// Revision 3.8  2003/01/02 19:39:03  ingmar
// better documentation structure
//
// Revision 3.7  2002/09/25 20:41:36  ingmar
// replaced Size2D with Point2D
//
// Revision 3.6  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 3.5  2002/07/08 17:34:03  binli
// function for cropbox
//
// Revision 3.4.8.1  2002/07/03 15:18:00  binli
// CropBox stuff
//
// Revision 3.4  2002/01/02 21:20:09  ingmar
// moved to Sec_12_mathGFX
//
// Revision 3.3  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.2  2001/11/12 19:19:35  jenny
// implemented WidthAbs() and HeightAbs()
//
// Revision 3.1  2001/11/06 22:51:08  frank
// GetMin and GetMax functions added.
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:18   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:18   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.15  2001/05/31 21:32:22  ana
// changes regarding code review
//
// Revision 1.14  2001/05/29 18:38:42  ana
// added head comments
//
// Revision 1.13  2001/05/23 21:34:38  ana
// finished TestByType function
//
// Revision 1.12  2001/05/22 21:31:05  ana
// Created tests functions for Rect<T> class.
//
// Revision 1.11  2001/04/12 14:37:28  dmitry
// Sec_3_math base class added
//
// Revision 1.10  2001/04/09 18:03:19  dmitry
// Removed Revision History from Top of File
//
// Revision 1.9  2001/03/28 19:01:00  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.8  2001/02/26 21:50:11  jmeade
// bool Rect<T>::PtInRect(const Tuple<T> &) const
//
// Revision 1.7  2001/02/15 01:27:14  jeff
// just a 2-D rectangle class (exactly what would an OpenGL rectangle be?) :-)
//
// Revision 1.6  2001/01/15 19:54:51  jeff
// comments
//
// Revision 1.5  2001/01/15 17:05:44  dave
// updated lipng108 for UNIX flavors, correct some syntax problems found by gcc
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Rect.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Rect.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
