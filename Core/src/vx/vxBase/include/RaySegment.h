// $Id: RaySegment.h,v 1.2 2004/03/23 21:14:46 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef RaySegment_h
#define RaySegment_h


// includes
#include "Vector4d.h"


/**
 * A Ray segment is a combination of a ray and a line segment.  It is like a line
 * segment in that it has two end points, but it also has a definite direction,
 * like a ray. See http://mathworld.wolfram.com/topics/Lines.html for more details.
 * Its end points are often determined through intersection of a ray with a bounding box. 
 * The beginning and end intersection are computed using a parametrized form of the ray, 
 * and the parameter value t is retained in the ray segment as fouth component of the 4D 
 * begin and end points.
 * 
 * A ray segment is not aware of the type of coordinate system it is in.
 */
class VX_BASE_DLL RaySegment
{
// member functions
public:

  /// default constructor
  RaySegment();

  /// constructor from two points
  RaySegment( const Point3Df & startPoint, const Point3Df & endPoint );

  /// constructor from two 4D vectors
  RaySegment( const Vector4Df & startPoint, const Vector4Df & endPoint );

  /// constructor from a point, direction, and length
  RaySegment( const Vector4Df & startPoint, const Vector4Df & direction, const float4 fLength );

  /// copy constructor
  RaySegment( const RaySegment & other ) : m_bgnPoint( other.m_bgnPoint ), m_endPoint( other.m_endPoint ) {}

  /// assignment operator
  RaySegment & operator =( const RaySegment & other ) { m_bgnPoint = other.m_bgnPoint; m_endPoint = other.m_endPoint; return * this; }

  /// equality operator
  inline bool operator ==( const RaySegment & other ) const { return m_bgnPoint == other.m_bgnPoint && m_endPoint == other.m_endPoint; }

  /// inequality operator
  inline bool operator !=( const RaySegment & other ) const { return m_bgnPoint != other.m_bgnPoint || m_endPoint != other.m_endPoint; }

  /// Ootput to a string
  std::string ToString() const;

  /// get the start point
  inline const Vector4Df & GetStartPoint() const { return m_bgnPoint; }

  /// get the end point
  inline const Vector4Df & GetEndPoint() const { return m_endPoint; }

  /// get the end point
  inline       Vector4Df & GetEndPoint()       { return m_endPoint; }

  /// get the direction
  inline Vector4Df GetDirection() const { return Vector4Df( m_endPoint - m_bgnPoint ); }

  /// get the length
  inline float4 GetLength() const { return ( m_endPoint - m_bgnPoint ).GetLengthXYZ(); }

// data members
private:

  /// start point
  Vector4Df m_bgnPoint;

  /// end point
  Vector4Df m_endPoint;

}; // class RaySegment


#endif // RaySegment_h


// $Log: RaySegment.h,v $
// Revision 1.2  2004/03/23 21:14:46  frank
// code review, working on xml serialization for vessels
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/21 14:20:44  frank
// code review
//
// Revision 1.6  2003/02/27 22:33:03  frank
// Added a constructor from Point3D
//
// Revision 1.5  2003/01/27 21:27:59  ingmar
// added write access for GetEndPoint()
//
// Revision 1.4  2003/01/22 21:14:07  ingmar
// the begin and end points are now represended as Vector4Df
// All interface functions have been adjusted accordingly
//
// Revision 1.3  2003/01/14 16:15:59  dongqing
// Added equality and inequality operators.
//
// Revision 1.2  2003/01/02 19:39:03  ingmar
// better documentation structure
//
// Revision 1.1  2001/01/02 13:20:33  frank
// initial version
//
// Revision 1.1  2002/11/21 19:26:06  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/RaySegment.h,v 1.2 2004/03/23 21:14:46 frank Exp $
// $Id: RaySegment.h,v 1.2 2004/03/23 21:14:46 frank Exp $