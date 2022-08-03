// $Id: BoundingRect.h,v 1.2 2006/08/03 20:00:30 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)

/**
 * Define the bounding 2-D box
 */

#ifndef BoundingRect_h
#define BoundingRect_h


// includes 
#include "Point3D.h"


/**
 * Class to maintain a 2D bounding box.
 */
template <class T>
class BoundingRect 
{
// member functions
public:

  /// default constructor
  BoundingRect();

  /// Constructor
  BoundingRect( const T & minX, const T & minY, const T & maxX, const T & maxY );

  /// Constructor
  BoundingRect( const T & dimX, const T & dimY );

  /// set parameters
  inline void Set( const T & minX, const T & minY, const T & maxX, const T & maxY );

  /// Compute the intersection of itself and another rectangle (does not modify itself)
  inline BoundingRect<T> GetIntersection( const BoundingRect & anotherRect ) const;

  /// Compute the union of itself and another rectangle (does not modify itself)
  inline BoundingRect<T> GetUnion( const BoundingRect & anotherRect ) const;

  /// Expand the bounding rectangle to include the given point
  inline void Union(const Point3D<T> & pointToInclude);

  /// Get the minimum x value
  inline const T & GetMinX() const { return m_minX; }

  /// Get the middle x value
  inline const T GetMidX() const { return ( m_minX + m_maxX ) / 2; }

  /// Get the maximum x value
  inline const T & GetMaxX() const { return m_maxX; }

  /// Get the minimum y value
  inline const T & GetMinY() const { return m_minY; }

  /// Get the middle y value
  inline const T GetMidY() const { return (m_minY + m_maxY) / 2; }

  /// Get the maximum y value
  inline const T & GetMaxY() const { return m_maxY; }

  /// Set the minimum x value
  inline void SetMinX( const T & x ) { m_minX = x; }

  /// Set the maximum x value
  inline void SetMaxX( const T & x ) { m_maxX = x; }

  /// Set the minimum y value
  inline void SetMinY( const T & y ) {  m_minY = y; }

  /// Set the maximum y value
  inline void SetMaxY(const T & y) { m_maxY = y; }

  /// Return the x dimension
  inline const T GetDimX() const { return m_maxX - m_minX; }

  /// Return the y dimension
  inline const T GetDimY() const { return m_maxY - m_minY; }

  /// access the four corners by index (as in class box)
  inline Point3D<T> GetCorner( const uint2 & i );

  /// Whether the area of the bounding rect is 0
  inline const bool IsEmpty() const;

  /// Whether the position is inside the bounding rect
  inline const bool IsInside( T x, T y ) const;

	///returns an Inflated Rect by delta
	inline BoundingRect<T> Inflate( const T delta ) const { return BoundingRect<T> ( m_minX - delta, m_minY - delta, m_maxX + delta, m_maxY + delta );	}

  /// Scale the rect
  inline void Scale(const T fScale);

  /// Get the area
  inline T GetArea() const { return GetDimX() * GetDimY(); }

  /// Ensure that the minimum is minimum and the maximum is maximum
  inline void Normalize();

  /// Convert to a string
  inline operator std::string() const { return ToString(); }

  /// Convert to a string
  std::string ToString() const;

  /// equality operator
  inline bool operator ==( const BoundingRect<T> & other ) const;

  /// inequality operator
  inline bool operator !=( const BoundingRect<T> & other ) const;

// data members
protected:

  /// the minimum x value
  T m_minX;

  /// the maximum x value
  T m_maxX;

  /// the minimum y value
  T m_minY;

  /// the maximum y value
  T m_maxY;

}; // class BoundingRect


#include "BoundingRect.inl"


#endif // BoundingRect_h


// $Log: BoundingRect.h,v $
// Revision 1.2  2006/08/03 20:00:30  romy
// Added inflate methods
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/05/20 16:13:50  dongqing
// code review
//
// Revision 3.13  2003/05/15 20:05:47  frank
// code review
//
// Revision 3.12  2003/01/02 19:37:17  ingmar
// better documentation structure
//
// Revision 3.11  2002/12/11 21:14:43  ingmar
// added ToString() and operator string
//
// Revision 3.10  2002/08/21 18:29:38  michael
// added method to access the four corners by index (as in class box)
//
// Revision 3.9  2002/05/03 22:25:45  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 3.8  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.7  2002/02/25 21:23:12  ingmar
// added void to function union
//
// Revision 3.6.2.1  2002/04/08 17:14:28  frank
// Added void return type on Union().
//
// Revision 3.6  2002/01/02 21:20:08  ingmar
// moved to Sec_12_mathGFX
//
// Revision 3.5  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.4  2001/11/13 19:35:04  geconomos
// Added Normalize()
//
// Revision 3.3  2001/11/06 01:11:50  frank
// Added Union()
//
// Revision 3.2  2001/11/03 19:35:56  frank
// Added GetMid functions.
//
// Revision 3.1  2001/11/01 16:44:03  frank
// Added extra constructors and output functions.
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 09 2001 18:30:36   liwei
// Added GetArea() member function.
// 
//    Rev 2.2   Sep 24 2001 17:15:08   liwei
// Added Scale() functions.
// 
//    Rev 2.1   Sep 19 2001 14:33:08   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:04   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.4  2001/08/23 00:19:06  liwei
// Added Union().
//
// Revision 1.3  2001/08/22 23:31:12  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.2  2001/08/22 00:15:19  liwei
// Added Set() member function.
//
// Revision 1.1  2001/08/09 21:11:58  liwei
// Box2D renamed to BoundingRect
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BoundingRect.h,v 1.2 2006/08/03 20:00:30 romy Exp $
// $Id: BoundingRect.h,v 1.2 2006/08/03 20:00:30 romy Exp $
