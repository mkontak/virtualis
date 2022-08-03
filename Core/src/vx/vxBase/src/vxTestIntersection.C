// $Id: vxTestIntersection.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTestIntersection.h"
#include "Line.h"
#include "Plane.h"
#include "Timer.h"
#include "Point3D.h"
#include "Intersection.h"


// namespaces
using namespace std;


/**
 * Unit test of Intersection namespace
 *
 * @param stream out stream
 * @return success
 */
bool vxTestIntersection::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestIntersection", "ValidateLinePlane", ValidateLinePlane, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestIntersection", "ValidatePlanePlane", ValidatePlanePlane, bOk );

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;

} // UnitTest()


/**
 * Performance test of Intersection namespace
 *
 * @param stream out stream
 * @return success
 */
bool vxTestIntersection::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Test the line-plane intersection
 *
 * @param stream out stream
 * @return success
 */
bool vxTestIntersection::ValidateLinePlane( std::ostream & stream )
{

  Point3Df originalIntersection( 123.456F, 6.756e-2F, -5.253F );
  Line line( originalIntersection, Point3Df( 1, 1, 1 ) );
  Plane plane( originalIntersection, Normal3Df( 1, 2, 3 ) );

  Point3Df intersectionPoint;

  if ( Intersection::ComputeIntersection( line, plane, intersectionPoint ) == false )
    return false;

  const float4 fEpsilon( 1e-5F );

  if ( Vector3Df( intersectionPoint - originalIntersection ).GetLength() > fEpsilon ) return false;

  return true;

} // ValidateLinePlane()


/**
 * Validate the plane-plane intersection
 *
 * @param stream out stream
 * @return success
 */
bool vxTestIntersection::ValidatePlanePlane( std::ostream & stream )
{
  // four "random" points
  Point3Df point1( 123.456F, 8.356e-2F, -2.753F );
  Point3Df point2( 4.356e-1F, 128.456F, 5.923F );
  Point3Df point3( 28.456F, 5.923F, 4.356e-1F );
  Point3Df point4( -3.923F, 47.356F, 128.456F );

  // two planes sharing points 1 & 2
  Plane plane1( point1, point2, point3 );
  Plane plane2( point1, point2, point4 );

  // prepare for comparison
  Line computedLine( point3, point4 ); // default to wrong line
  Line actualLine( point1, point2 );

  // intersect!
  if ( Intersection::ComputeIntersection( plane1, plane2, computedLine ) == false )
    return false;

  // limit of accuracy
  const float4 fEpsilon( 1e-5F );

  // compare the line direction
  Vector3Df difference( Point3Df( Vector3Df( computedLine.GetDirection() ) ) - Point3Df( Vector3Df( actualLine.GetDirection() ) ) );
  if ( difference.GetLength() > fEpsilon ) return false;

  // see if both points 1 & 2 are on the line
  if ( actualLine.ComputeDistance( point1 ) > fEpsilon ) return false;
  if ( actualLine.ComputeDistance( point2 ) > fEpsilon ) return false;

  return true;

} // ValidateLinePlane()


// $Log: vxTestIntersection.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.2  2003/08/31 03:50:40  frank
// Added test for plane-plane intersection
//
// Revision 1.1  2003/05/20 15:44:12  frank
// Added test for Intersection namespace
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestIntersection.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestIntersection.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
