// $Id: Line.C,v 1.2 2004/03/04 19:05:33 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "Line.h"
#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"



// Defines
#define FILE_REVISION "$Revision: 1.7"


/**
 * Constructor taking a point and a direction
 * @param point
 * @param direction
 */
Line::Line( const Point3Df & point, const Normal3Df & direction )
{
  m_point = point;
  m_direction = direction;
} // Line()


/**
 * Constructor taking two points
 * @param pointOne
 * @param pointTwo
 */
Line::Line( const Point3Df & pointOne, const Point3Df & pointTwo )
{
  m_point = pointOne;
  if ( pointOne == pointTwo )
    LogWrn( "Tried to create a line with identical ends: " + pointOne.ToString() + " " + pointTwo.ToString(), "Line", "Line" )
  else
    m_direction = Normal3Df(pointTwo - pointOne);
} // Line()


/**
 * Compute the point of nearest approach with another (potentially skew) line.
 * @param other
 * @return nearest point
 */
Point3Df Line::GetIntersection( const Line & other )
{
  throw ex::UnsupportedOperationException( LogRec( "unsupported at this time", "Line", "GetIntersection" ) );

  // Two or more lines which have no intersections but are not parallel, also called agonic lines.
  // Since two lines in the plane must intersect or be parallel, skew lines can exist only in three
  // or more dimensions.

  // see http://mathworld.wolfram.com/Line-LineIntersection.html for a description of the equation
  // which solves the point of intersection between two potentially skew lines

  const Point3Df p1( m_point );

  const Point3Df p2( other.m_point );

  const Vector<float4> v1( m_direction );
  
  const Vector<float4> v2( other.m_direction );
  
  const Vector<float4> v12( v1.Cross( v2 ) );
  
  const float4 fLengthV12Squared( static_cast< float4 >( pow( static_cast<float4>(v12.GetLength()), 2.0F )));
  
  const Matrix<float4> m1( p2.m_x - p1.m_x, v2.m_x, v12.m_x, 1,
                           p2.m_y - p1.m_y, v2.m_y, v12.m_y, 1,
                           p2.m_z - p1.m_z, v2.m_z, v12.m_z, 1,
                           1              , 1     , 1      , 1 );
  const float4 s1( m1.GetTranspose().GetDeterminant() / fLengthV12Squared );
  
  const Matrix<float4> m2( p2.m_x - p1.m_x, v1.m_x, v12.m_x, 1,
                           p2.m_y - p1.m_y, v1.m_y, v12.m_y, 1,
                           p2.m_z - p1.m_z, v1.m_z, v12.m_z, 1,
                           1              , 1     , 1      , 1 );
  const float4 s2( m2.GetTranspose().GetDeterminant() / fLengthV12Squared );

  const Point3Df nearest( ( Vector<float4>(p1) + v1 * s1 + Vector<float4>(p2) + v2 * s2 ) / 2.0F );

  return nearest;

} // GetIntersection()


/**
 * Compute the distance of a point from this line.
 * see http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html for derivation
 * 
 * @param point3D the point of interest
 * @return distance
 */
float4 Line::ComputeDistance( const Point3Df & point3D ) const
{
  Normal3Df vec21( m_direction );
  Vector3Df vec10( m_point - point3D );

  Vector3Df crossProduct( vec21.Cross( vec10 ) );

  return static_cast< float4 >( crossProduct.GetLength() );
} // ComputeDistance()

#undef FILE_REVISION


// $Log: Line.C,v $
// Revision 1.2  2004/03/04 19:05:33  geconomos
// Fixed up code to resolve ambiguity errors with math functions caught with the 7.1 complier
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/20 16:13:50  dongqing
// code review
//
// Revision 1.7  2003/05/14 20:40:38  frank
// code review
//
// Revision 1.6  2003/03/03 18:28:46  frank
// Made line creation safer
//
// Revision 1.5  2003/01/24 18:20:45  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.4  2003/01/22 21:12:24  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.3  2003/01/21 22:09:48  frank
// Added another constructor and a compute distance function.
//
// Revision 1.2  2003/01/13 21:01:58  ingmar
// added missing include
//
// Revision 1.1  2002/11/21 19:26:06  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Line.C,v 1.2 2004/03/04 19:05:33 geconomos Exp $
// $Id: Line.C,v 1.2 2004/03/04 19:05:33 geconomos Exp $