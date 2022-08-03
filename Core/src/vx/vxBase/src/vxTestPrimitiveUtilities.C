// $Id: vxTestPrimitiveUtilities.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestPrimitiveUtilities.h"
#include "Line.h"
#include "Utilities.h"
#include "Vector3D.h"
#include "PrimitiveUtilities.h"
#include "Plane.h"
#include "RaySegment.h"


// namespaces
using namespace std;


/**
 * Unit test of Plane
 *
 * @param stream out stream
 * @return success
 */
bool vxTestPrimitiveUtilities::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPrimitiveUtilities", "ValidateGetPointOnPlane", ValidateGetPointOnPlane, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPrimitiveUtilities", "ValidateGetThreePointsOnPlane", ValidateGetThreePointsOnPlane, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPrimitiveUtilities", "ValidateProjectPointOntoLine", ValidateProjectPointOntoLine, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPrimitiveUtilities", "ValidateGetPercentAlongSegment", ValidateGetPercentAlongSegment, bOk );

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;

} // UnitTest()


/**
 * Performance test of vxVolume stuff
 *
 * @param stream out stream
 * @return success
 */
bool vxTestPrimitiveUtilities::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Validate GetPointOnPlane
 *
 * @param stream out stream
 * @return success
 */
bool vxTestPrimitiveUtilities::ValidateGetPointOnPlane( std::ostream & stream )
{
  Plane plane( Point3Df( 1, 2, 3 ), Normal3Df( 0.423F, 5634.0F, 38.6F ) );
  const Point3Df point( PrimitiveUtilities::GetPointOnPlane( plane ) );
  const float4 fEpsilon( 1e-5F );

  if ( fabs( plane.GetDistance( point ) ) > fEpsilon ) return false;

  return true;
} // ValidateGetPointOnPlane()


/**
 * validate GetThreePointsOnPlane
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestPrimitiveUtilities::ValidateGetThreePointsOnPlane( std::ostream & stream )
{

  Plane plane( Point3Df( -1, 2.342F, 3 ), Normal3Df( 0.423F, 5634.0F, 38.6F ) );
  std::vector< Point3Df > points( PrimitiveUtilities::GetThreePointsOnPlane( plane ) );

  for ( std::vector< Point3Df >::iterator iter( points.begin() ); iter!=points.end(); ++iter )
  {
    const float4 fEpsilon( 1e-5F );
    if ( fabs( plane.GetDistance( * iter ) ) > fEpsilon )
      return false;
  }

  return true;

} // ValidateGetThreePointsOnPlane()


/**
 * validate ProjectPointOntoLine
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestPrimitiveUtilities::ValidateProjectPointOntoLine( std::ostream & stream )
{
  // setup
  Line line( Point3Df( -1, 2.342F, 3 ), Normal3Df( 0.423F, 5634.0F, 38.6F ) );
  Point3Df pointOnLine( PrimitiveUtilities::ProjectPointOntoLine( line, Point3Df( 342, 3, -64 ) ) );
  const float4 fEpsilon( 1e-5F );

  // tests
  if ( line.ComputeDistance( pointOnLine ) > fEpsilon ) return false;

  return true;
} // ValidateProjectPointOntoLine()


/**
 * validate GetPercentAlongSegment
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestPrimitiveUtilities::ValidateGetPercentAlongSegment( std::ostream & stream )
{
  // setup
  const Point3Df start( 4, -6, 36.F );
  const Point3Df end( 4.63e-2F, 73, -452.7F );
  const Point3Df middle( ( Vector3Df( start ) + Vector3Df( end ) ) / 2 );
  RaySegment segment( start, end );
  const float4 fEpsilon( 1e-5F );

  // tests
  if ( PrimitiveUtilities::GetPercentAlongSegment( segment, start ) > fEpsilon ) return false;
  if ( fabs( 1 - PrimitiveUtilities::GetPercentAlongSegment( segment, end ) ) > fEpsilon ) return false;
  if ( fabs( 0.5F - PrimitiveUtilities::GetPercentAlongSegment( segment, middle ) ) > fEpsilon ) return false;

  return true;
} // ValidateGetPercentAlongSegment()


// $Log: vxTestPrimitiveUtilities.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/20 16:28:36  frank
// cleaned up log
//
// Revision 1.1  2003/05/20 16:23:26  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestPrimitiveUtilities.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestPrimitiveUtilities.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
