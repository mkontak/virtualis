// $Id: PrimitiveUtilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "PrimitiveUtilities.h"
#include "Plane.h"
#include "Line.h"
#include "Point.h"
#include "Intersection.h"
#include "RaySegment.h"
#include "Vector3D.h"


/**
 * Generate a point on a plane.  It could be any point, not necessarily random.
 *
 * @param plane the plane
 *
 * @return a point on the plane
 */
Point3Df PrimitiveUtilities::GetPointOnPlane( const Plane & plane )
{
  return plane.GetProjectionOntoPlane( Point3Df( 0,0,0 ) );
} // GetPointOnPlane()


/**
 * Generate three points on a plane.  They could be any three points, not necessarily random.
 *
 * @param plane the plane
 *
 * @return a vector of points on the plane
 */
std::vector< Point<float4> > PrimitiveUtilities::GetThreePointsOnPlane( const Plane & plane )
{
  // get a point on the plane
  std::vector< Point<float4> > points;
  points.push_back( plane.GetProjectionOntoPlane( Point<float4>( 0, 0, 0 ) ) );

  // get two vectors that are more perpendicular to the normal direction
  const int4 iNotLargestComponent1( ( plane.GetNormal().GetMaxAbsComponentCoord() + 1 ) % 3 );
  const int4 iNotLargestComponent2( ( plane.GetNormal().GetMaxAbsComponentCoord() + 2 ) % 3 );
  const Vector<float4> minorDir1( iNotLargestComponent1==0, iNotLargestComponent1==1, iNotLargestComponent1==2 );
  const Vector<float4> minorDir2( iNotLargestComponent2==0, iNotLargestComponent2==1, iNotLargestComponent2==2 );

  // project the two vectors onto the plane
  points.push_back( plane.GetProjectionOntoPlane( points.front() + minorDir1 ) );
  points.push_back( plane.GetProjectionOntoPlane( points.front() + minorDir2 ) );

  return points;
} // GetThreePointsOnPlane()


/**
 * Compute the point on the line nearest to the given point.
 *
 * @param line  the line
 * @param point the point which will be projected onto the line
 *
 * @return the point on the line nearest to the given point
 */
Point3D<float4> PrimitiveUtilities::ProjectPointOntoLine( const Line & line, const Point3D<float4> & point )
{
  // use the plane intersection to get a plane perpendicular to the line
  Plane plane( point, line.GetDirection() );

  // use the line-plane intersection to get the new point
  Point3Df pointOnLine( point );

  // if the intersection was found, then it's a good resulting point
  // if the intersection was not found, then the original point is the best intersection already
  Intersection::ComputeIntersection( line, plane, pointOnLine );

  return pointOnLine;

} // ProjectPointOntoLine()


/**
 * Utility function to get the percent distance along a ray segment to which a given point projects.
 *
 * @param segment  two points that define a ray segment
 * @param position the position of the point that will get projected to the line
 *
 * @return zero at start of segment, one at end, less than zero past beginning of segment, greater than one past end of segment
 */
float4 PrimitiveUtilities::GetPercentAlongSegment( const RaySegment & segment, const Point3Df & position )
{
  // project the point onto the plane
  Line line( Point3Df( segment.GetStartPoint() ), Point3Df( segment.GetEndPoint() ) );
  Point3Df pointOnLine( PrimitiveUtilities::ProjectPointOntoLine( line, position ) );

  Vector3Df overallVector( segment.GetDirection().m_x, segment.GetDirection().m_y, segment.GetDirection().m_z );
  Vector3Df sampleVector( pointOnLine - Point3Df( segment.GetStartPoint() ) );
  if ( overallVector.GetLength() > 0.0F )
    return static_cast< float4>( Normal3Df( overallVector ).Dot( sampleVector ) / overallVector.GetLength() );
  else
    return 0.0F;

} // GetPercentAlongLength()


/**
 * Projects a vector so that it becomes parallel to the plane.
 *
 * @param vector3D      the vector to proejct
 * @param normalToPlane the normal to the plane
 *
 * @return the projected vector
 */
Vector3Df PrimitiveUtilities::GetProjectionParallelToPlane( const Vector3Df & vector3D, const Normal3Df & normalToPlane )
{
  // Project the vector so that it's perpendicular to the normal vector.

  // The dot product between them should be zero.
  float4 fDotProduct( normalToPlane.Dot( vector3D ) );

  // move the tip of the vector by the amount of the dotproduct against he normal direction.
  return Vector3Df( vector3D ) - Vector3Df( normalToPlane ) * fDotProduct;

  // now computing the dot product again would result in a zero value

} // GetProjectionParallelToPlane() 


/**
 * Returns a new plane parallel to the original one, but offset by a distance
 *
 * @param originalPlane the original plane
 * @param fOffset       the signed distance to offset
 * @return              a new plane parallel to the original one, but offset by a distance
 */
Plane PrimitiveUtilities::GetOffsetPlane( const Plane & originalPlane, const float4 & fOffset )
{
  return Plane( originalPlane.GetPointOnPlane() + Vector<float4>( originalPlane.GetNormal() ) * fOffset, originalPlane.GetNormal() );
} // GetOffsetPlane()


// $Log: PrimitiveUtilities.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/23 12:35:32  frank
// Moved utility functions to PrimitiveUtilities
//
// Revision 1.5  2003/05/20 16:25:18  frank
// cleaned up log
//
// Revision 1.4  2003/02/28 19:26:41  frank
// better comments
//
// Revision 1.3  2003/02/27 22:32:42  frank
// Added point-line projection and percent distance routines
//
// Revision 1.2  2003/01/24 18:20:30  frank
// Moved common typedefs to Typedef.h
//
// Revision 1.1  2003/01/24 15:18:35  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/PrimitiveUtilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: PrimitiveUtilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $