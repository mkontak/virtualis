// $Id: vxTestBoundingRect.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestBoundingRect.h"
#include "Utilities.h"
#include "BoundingRect.h"


// namespaces
using namespace std;


/**
 * Unit test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestBoundingRect::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateConstructors", ValidateConstructors, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateSet", ValidateSet, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateGetIntersection", ValidateGetIntersection, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateGetUnion", ValidateGetUnion, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateUnion", ValidateUnion, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateGetMinMidMax", ValidateGetMinMidMax, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateSetMinMax", ValidateSetMinMax, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateGetDim", ValidateGetDim, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateGetCorner", ValidateGetCorner, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateIsEmpty", ValidateIsEmpty, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateIsInside", ValidateIsInside, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateScale", ValidateScale, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateGetArea", ValidateGetArea, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateNormalize", ValidateNormalize, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateToString", ValidateToString, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateEquality", ValidateEquality, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBoundingRect", "ValidateInequality", ValidateInequality, bOk );

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;

} // UnitTest()


/**
 * Performance test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestBoundingRect::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Test the bounding rect constructor
 *
 * @param stream out stream
 * @return success
 */
bool vxTestBoundingRect::ValidateConstructors( std::ostream & stream )
{
  // validate default constructor
  BoundingRect<int4> rect1;
  if ( rect1.GetMinX() != 0 ) return false;
  if ( rect1.GetMinY() != 0 ) return false;
  if ( rect1.GetMaxX() != 0 ) return false;
  if ( rect1.GetMaxY() != 0 ) return false;

  // validate second constructor
  BoundingRect<int4> rect2( 1, 2, 3, 4 );
  if ( rect2.GetMinX() != 1 ) return false;
  if ( rect2.GetMinY() != 2 ) return false;
  if ( rect2.GetMaxX() != 3 ) return false;
  if ( rect2.GetMaxY() != 4 ) return false;

  // validate third constructor
  BoundingRect<int4> rect3( 10, 20 );
  if ( rect3.GetMinX() != 0 ) return false;
  if ( rect3.GetMinY() != 0 ) return false;
  if ( rect3.GetMaxX() != 10 ) return false;
  if ( rect3.GetMaxY() != 20 ) return false;

  return true;
} // ValidateConstructors()


/**
 * validate set
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateSet( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );
  rect.Set( 5, 6, 7, 8 );

  return rect == BoundingRect<int4>( 5, 6, 7, 8 );

} // ValidateSet()


/**
 * validate get intersection
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateGetIntersection( std::ostream & stream )
{
  BoundingRect<int4> rect1( 1, 2, 6, 7 );
  BoundingRect<int4> rect2( -1, -2, 2, 8 );
  BoundingRect<int4> out( rect1.GetIntersection( rect2 ) );

  return out == BoundingRect<int4>( 1, 2, 2, 7 );
} // ValidateGetIntersection()


/**
 * validate get union
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateGetUnion( std::ostream & stream )
{
  BoundingRect<int4> rect1( 1, 2, 3, 4 );
  BoundingRect<int4> rect2( -1, -2, 0, 8 );
  BoundingRect<int4> out( rect1.GetUnion( rect2 ) );

  return out == BoundingRect<int4>( -1, -2, 3, 8 );
} // ValidateGetUnion()


/**
 * validate union
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateUnion( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 1, 5, 5 );
  rect.Union( Point3D<int4>( 7, 7, 7 ) );
  return rect == BoundingRect<int4>( 1, 1, 7, 7 );
} // ValidateUnion()


/**
 * validate get min/mid/max
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateGetMinMidMax( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );

  if ( rect.GetMinX() != 1 ) return false;
  if ( rect.GetMinY() != 2 ) return false;
  if ( rect.GetMidX() != 2 ) return false;
  if ( rect.GetMidY() != 3 ) return false;
  if ( rect.GetMaxX() != 3 ) return false;
  if ( rect.GetMaxY() != 4 ) return false;

  return true;
} // ValidateGetMinMidMax()


/**
 * validate set min/max
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateSetMinMax( std::ostream & stream )
{
  BoundingRect<int4> rect;

  rect.SetMinX( 1 );
  rect.SetMinY( 2 );
  rect.SetMaxX( 3 );
  rect.SetMaxY( 4 );

  if ( rect.GetMinX() != 1 ) return false;
  if ( rect.GetMinY() != 2 ) return false;
  if ( rect.GetMaxX() != 3 ) return false;
  if ( rect.GetMaxY() != 4 ) return false;

  return true;
} // ValidateSetMinMax()


/**
 * validate get dim
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateGetDim( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );

  if ( rect.GetDimX() != 2 ) return false;
  if ( rect.GetDimY() != 2 ) return false;

  return true;
} // ValidateGetDim()


/**
 * validate get corner
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateGetCorner( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );

  if ( rect.GetCorner( 0 ) != Point3D<int4>( 1, 2, 0 ) ) return false;
  if ( rect.GetCorner( 1 ) != Point3D<int4>( 3, 2, 0 ) ) return false;
  if ( rect.GetCorner( 2 ) != Point3D<int4>( 1, 4, 0 ) ) return false;
  if ( rect.GetCorner( 3 ) != Point3D<int4>( 3, 4, 0 ) ) return false;

  return true;
} // ValidateGetCorner()


/**
 * validate is empty
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateIsEmpty( std::ostream & stream )
{
  BoundingRect<int4> rectFull( 1, 2, 3, 4 );
  BoundingRect<int4> rectEmpty( 5, 8, 5, 8 );

  if ( rectFull.IsEmpty() == true ) return false;
  if ( rectEmpty.IsEmpty() == false ) return false;

  return true;
} // ValidateIsEmpty()


/**
 * validate is inside
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateIsInside( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );

  if ( rect.IsInside( -2, -5 ) == true ) return false;
  if ( rect.IsInside( 2, 3 ) == false ) return false;
  if ( rect.IsInside( 6, 12 ) == true ) return false;

  return true;
} // ValidateIsInside()


/**
 * validate scale
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateScale( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );
  rect.Scale( 2 );

  if ( rect.GetMinX() != 2 ) return false;
  if ( rect.GetMinY() != 4 ) return false;
  if ( rect.GetMaxX() != 6 ) return false;
  if ( rect.GetMaxY() != 8 ) return false;

  return true;
} // ValidateScale()


/**
 * validate get area
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateGetArea( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );

  if ( rect.GetArea() != 4 ) return false;

  return true;
} // ValidateGetArea()


/**
 * validate normalize
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateNormalize( std::ostream & stream )
{
  BoundingRect<int4> rect( 3, 4, 1, -2 );

  rect.Normalize();

  return rect == BoundingRect<int4>( 1, -2, 3, 4 );
} // ValidateNormalize()


/**
 * validate to string
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateToString( std::ostream & stream )
{
  BoundingRect<int4> rect( 1, 2, 3, 4 );

  return rect.ToString() == "BoundingRect( 1<=x<=3, 2<=y<=4 )";
} // ValidateToString()


/**
 * validate equality
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateEquality( std::ostream & stream )
{
  BoundingRect<int4> rect1( 1, 2, 3, 4 );
  BoundingRect<int4> rect2( 5, 6, 7, 8 );
  BoundingRect<int4> rect3( 5, 6, 7, 8 );

  if ( ( rect1 == rect2 ) == true ) return false;
  if ( ( rect1 == rect3 ) == true ) return false;
  if ( ( rect2 == rect3 ) == false ) return false;

  return true;
} // ValidateEquality()


/**
 * validate inequality
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBoundingRect::ValidateInequality( std::ostream & stream )
{
  BoundingRect<int4> rect1( 1, 2, 3, 4 );
  BoundingRect<int4> rect2( 5, 6, 7, 8 );
  BoundingRect<int4> rect3( 5, 6, 7, 8 );

  if ( ( rect1 != rect2 ) == false ) return false;
  if ( ( rect1 != rect3 ) == false ) return false;
  if ( ( rect2 != rect3 ) == true ) return false;

  return true;
} // ValidateInequality()


// $Log: vxTestBoundingRect.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/15 16:00:36  frank
// initial version
//
// Revision 1.2  2003/05/15 15:56:43  frank
// completed tests
//
// Revision 1.1  2003/05/15 15:24:16  frank
// intial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestBoundingRect.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestBoundingRect.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
