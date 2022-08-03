// $Id: Intersection.C,v 1.2 2005/05/23 12:47:48 frank Exp $
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
#include "Intersection.h"
#include "Line.h"
#include "Point.h"
#include "Vector.h"
#include "Plane.h"
#include "Matrix.h"
#include "PrimitiveUtilities.h"
#include "Exception.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Compute the intersection with a plane.
 * see http://mathworld.wolfram.com/Line-PlaneIntersection.html for the algorithm.
 *
 * @param line         the line
 * @param plane        the plane
 * @param intersection the point of intersection
 *
 * @return             true if there is an intersection
 */
bool Intersection::ComputeIntersection( const Line & line, const Plane & plane, Point3Df & intersection )
{

  float4 fNumerator( plane.GetNormal().X() * line.GetPoint().X() +
                     plane.GetNormal().Y() * line.GetPoint().Y() +
                     plane.GetNormal().Z() * line.GetPoint().Z() +
                     plane.GetDistance( Point3Df(0,0,0) ) );
  float4 fDenominator( plane.GetNormal().X() * line.GetDirection().X() +
                       plane.GetNormal().Y() * line.GetDirection().Y() +
                       plane.GetNormal().Z() * line.GetDirection().Z() );

  if ( fDenominator == 0.0F )
    return false;

  // get the intersection point
  const float4 fTimeOfIntersection( -fNumerator / fDenominator );
  intersection = line.GetPoint() + Vector<float4>( line.GetDirection() ) * fTimeOfIntersection;

  return true;
} // GetIntersection()


/**
 * Get the intersection of two planes
 * see http://mathworld.wolfram.com/Plane-PlaneIntersection.html
 *
 * @param plane1 the first plane
 * @param plane2 the second plane
 * @param line   the line of intersection
 *
 * @return true if intersecting
 */
bool Intersection::ComputeIntersection( const Plane & plane1, const Plane & plane2, Line & line )
{

  // check to make sure the two planes are not parallel
  if ( plane1.GetNormal().Dot( plane2.GetNormal() ) == 0 ) return false;

  // choose a point that is within one plane
  // first try along z-axis
  Point3Df intersectionPoint( 0, 0, 0 );
  if ( ComputeIntersection( Line( Point3Df( 0, 0, 0 ), Normal3Df( 0, 0, 1 ) ), plane1, intersectionPoint ) == false )
  {
    if ( ComputeIntersection( Line( Point3Df( 0, 0, 0 ), Normal3Df( 0, 0, 1 ) ), plane1, intersectionPoint ) == false )
      throw ex::AbortOperationException( LogRec( "The plane should intersect at least one of two orthogonal lines", "Intersection", "ComputeIntersection" ) );
  }

  // now find the direction that is parallel to both planes
  // it is the cross-product of the two plane normals
  const Normal3Df lineDirection( Normal3Df( plane1.GetNormal().Cross( plane2.GetNormal() ) ) );

  // get a line within the first plane through the intersection point and perpendicular to the final line direction
  const Normal3Df perpLineDirection( Normal3Df( lineDirection.Cross( plane1.GetNormal() ) ) );

  // now intersect the line through plane2
  Point3Df doubleInstersectionPoint;
  if ( ComputeIntersection( Line( intersectionPoint, perpLineDirection ), plane2, doubleInstersectionPoint ) == false )
    throw ex::AbortOperationException( LogRec( "Unable to determine the final intersection", "Intersection", "ComputeIntersection" ) );

  // save results in output parameter
  line = Line( doubleInstersectionPoint, lineDirection );

  return true;

} // ComputeIntersection()

#undef FILE_REVISION

// $Log: Intersection.C,v $
// Revision 1.2  2005/05/23 12:47:48  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/08/31 03:01:52  frank
// Added plane-plane intersection - testing to follow
//
// Revision 1.3  2003/01/30 17:29:42  frank
// Fixed math.
//
// Revision 1.2  2003/01/24 18:19:38  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.1  2003/01/21 20:38:04  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Intersection.C,v 1.2 2005/05/23 12:47:48 frank Exp $
// $Id: Intersection.C,v 1.2 2005/05/23 12:47:48 frank Exp $
