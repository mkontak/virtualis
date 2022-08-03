// $Id: vxTestBox.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestBox.h"
#include "Utilities.h"
#include "Box.h"


// namespaces
using namespace std;


/**
 * Unit test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestBox::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateConstructors", ValidateConstructors, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateSet", ValidateSet, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateGetIntersection", ValidateGetIntersection, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateGetUnion", ValidateGetUnion, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateUnion", ValidateUnion, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateGetMinMidMax", ValidateGetMinMidMax, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateSetMinMax", ValidateSetMinMax, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateGetDim", ValidateGetDim, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateGetCorner", ValidateGetCorner, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateIsInside", ValidateIsInside, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateScale", ValidateScale, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateNormalize", ValidateNormalize, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateToString", ValidateToString, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateEquality", ValidateEquality, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateInequality", ValidateInequality, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateInnerRadius", ValidateInnerRadius, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestBox", "ValidateOuterRadius", ValidateOuterRadius, bOk );

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
bool vxTestBox::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Test the bounding box constructor
 *
 * @param stream out stream
 * @return success
 */
bool vxTestBox::ValidateConstructors( std::ostream & stream )
{
  // validate default constructor
  Box<int4> box1;
  if ( box1.GetMinX() != 0 ) return false;
  if ( box1.GetMinY() != 0 ) return false;
  if ( box1.GetMinZ() != 0 ) return false;
  if ( box1.GetMaxX() != 0 ) return false;
  if ( box1.GetMaxY() != 0 ) return false;
  if ( box1.GetMaxZ() != 0 ) return false;

  // validate constructor with six values
  Box<int4> box2( 1, 2, 3, 4, 5, 6 );
  if ( box2.GetMinX() != 1 ) return false;
  if ( box2.GetMinY() != 2 ) return false;
  if ( box2.GetMinZ() != 3 ) return false;
  if ( box2.GetMaxX() != 4 ) return false;
  if ( box2.GetMaxY() != 5 ) return false;
  if ( box2.GetMaxZ() != 6 ) return false;

  // validate copy constructor
  Box<int4> box3( box2 );
  if ( box3.GetMinX() != 1 ) return false;
  if ( box3.GetMinY() != 2 ) return false;
  if ( box3.GetMinZ() != 3 ) return false;
  if ( box3.GetMaxX() != 4 ) return false;
  if ( box3.GetMaxY() != 5 ) return false;
  if ( box3.GetMaxZ() != 6 ) return false;

  // validate point constructor
  Box<int4> box4( Point3D<int4>( 1, 2, 3 ), Point3D<int4>( 4, 5, 6 ) );
  if ( box4.GetMinX() != 1 ) return false;
  if ( box4.GetMinY() != 2 ) return false;
  if ( box4.GetMinZ() != 3 ) return false;
  if ( box4.GetMaxX() != 4 ) return false;
  if ( box4.GetMaxY() != 5 ) return false;
  if ( box4.GetMaxZ() != 6 ) return false;

  return true;
} // ValidateConstructors()


/**
 * validate set
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateSet( std::ostream & stream )
{
  Box<int4> box( 1, 2, 3, 4, 5, 6 );
  box.Set( 5, 6, 7, 8, 9, 10 );

  return box == Box<int4>( 5, 6, 7, 8, 9, 10 );
} // ValidateSet()


/**
 * validate get intersection
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateGetIntersection( std::ostream & stream )
{
  Box<int4> box1( 1, 2, 3, 7, 8, 9 );
  Box<int4> box2( -1, -2, -3, 4, 5, 12 );
  Box<int4> out( box1.GetIntersection( box2 ) );

  return out == Box<int4>( 1, 2, 3, 4, 5, 9 );
} // ValidateGetIntersection()


/**
 * validate get union
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateGetUnion( std::ostream & stream )
{
  Box<int4> box1( 1, 2, 3, 7, 8, 9 );
  Box<int4> box2( -1, -2, -3, 4, 5, 12 );
  Box<int4> out( box1.GetUnion( box2 ) );

  return out == Box<int4>( -1, -2, -3, 7, 8, 12 );
} // ValidateGetUnion()


/**
 * validate union
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateUnion( std::ostream & stream )
{
  Box<int4> box( 1, 1, 1, 5, 5, 5 );
  box.Union( Point3D<int4>( 7, 7, 7 ) );
  if ( box != Box<int4>( 1, 1, 1, 7, 7, 7 ) ) return false;

  box.Union( Point3D<int4>( -7, -7, -7 ) );
  if ( box != Box<int4>( -7, -7, -7, 7, 7, 7 ) ) return false;

  return true;
} // ValidateUnion()


/**
 * validate get min/mid/max
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateGetMinMidMax( std::ostream & stream )
{
  Box<int4> box( 1, 2, 3, 5, 6, 7 );

  if ( box.GetMinX() != 1 ) return false;
  if ( box.GetMinY() != 2 ) return false;
  if ( box.GetMinZ() != 3 ) return false;
  if ( box.GetMidX() != 3 ) return false;
  if ( box.GetMidY() != 4 ) return false;
  if ( box.GetMidZ() != 5 ) return false;
  if ( box.GetMaxX() != 5 ) return false;
  if ( box.GetMaxY() != 6 ) return false;
  if ( box.GetMaxZ() != 7 ) return false;

  return true;
} // ValidateGetMinMidMax()


/**
 * validate set min/max
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateSetMinMax( std::ostream & stream )
{
  Box<int4> box;

  box.SetMinX( 1 );
  box.SetMinY( 2 );
  box.SetMinZ( 3 );
  box.SetMaxX( 4 );
  box.SetMaxY( 5 );
  box.SetMaxZ( 6 );

  if ( box.GetMinX() != 1 ) return false;
  if ( box.GetMinY() != 2 ) return false;
  if ( box.GetMinZ() != 3 ) return false;
  if ( box.GetMaxX() != 4 ) return false;
  if ( box.GetMaxY() != 5 ) return false;
  if ( box.GetMaxZ() != 6 ) return false;

  return true;
} // ValidateSetMinMax()


/**
 * validate get dim
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateGetDim( std::ostream & stream )
{
  Box<int4> box( 1, 2, -3, 4, 5, 6 );
  return box.GetDim() == Triple<int4>( 3, 3, 9 );
} // ValidateGetDim()


/**
 * validate get corner
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateGetCorner( std::ostream & stream )
{
  Box<int4> box( 1, 2, 3, 4, 5, 6 );

  if ( box.GetCorner( 0 ) != Point3D<int4>( 1, 2, 3 ) ) return false;
  if ( box.GetCorner( 1 ) != Point3D<int4>( 4, 2, 3 ) ) return false;
  if ( box.GetCorner( 2 ) != Point3D<int4>( 1, 5, 3 ) ) return false;
  if ( box.GetCorner( 3 ) != Point3D<int4>( 4, 5, 3 ) ) return false;
  if ( box.GetCorner( 4 ) != Point3D<int4>( 1, 2, 6 ) ) return false;
  if ( box.GetCorner( 5 ) != Point3D<int4>( 4, 2, 6 ) ) return false;
  if ( box.GetCorner( 6 ) != Point3D<int4>( 1, 5, 6 ) ) return false;
  if ( box.GetCorner( 7 ) != Point3D<int4>( 4, 5, 6 ) ) return false;

  return true;
} // ValidateGetCorner()


/**
 * validate is inside
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateIsInside( std::ostream & stream )
{
  Box<int4> box( 1, 2, 3, 4, 5, 6 );

  if ( box.IsInside( Triple<int4>( -2, -5, -9 ) ) == true ) return false;
  if ( box.IsInside( Triple<int4>( 2, 3, 5 ) ) == false ) return false;
  if ( box.IsInside( Triple<int4>( 6, 12, 60 ) ) == true ) return false;

  return true;
} // ValidateIsInside()


/**
 * validate scale
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateScale( std::ostream & stream )
{
  Box<int4> box( 1, 2, 3, 4, 5, 6 );
  box.Scale( 2 );

  if ( box != Box<int4>( 2, 4, 6, 8, 10, 12 ) ) return false;

  box.ScaleAnIso( Triple<float4>( 0.5F, 0.5F, 0.5F ) );

  if ( box != Box<int4>( 1, 2, 3, 4, 5, 6 ) ) return false;

  return true;
} // ValidateScale()


/**
 * validate normalize
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateNormalize( std::ostream & stream )
{
  Box<int4> box( 3, 4, 5, 1, -2, -10 );

  box.Normalize();

  return box == Box<int4>( 1, -2, -10, 3, 4, 5 );
} // ValidateNormalize()


/**
 * validate to string
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateToString( std::ostream & stream )
{
  Box<int4> box( 1, 2, 3, 4, 5, 6 );

  return box.ToString() == "Box( 1<=x<=4, 2<=y<=5, 3<=z<=6 )";
} // ValidateToString()


/**
 * validate equality
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateEquality( std::ostream & stream )
{
  Box<int4> box1( 1, 2, 3, 4, 5, 6 );
  Box<int4> box2( 5, 6, 7, 8, 9, 10 );
  Box<int4> box3( 5, 6, 7, 8, 9, 10 );

  if ( ( box1 == box2 ) == true ) return false;
  if ( ( box1 == box3 ) == true ) return false;
  if ( ( box2 == box3 ) == false ) return false;

  return true;
} // ValidateEquality()


/**
 * validate inequality
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateInequality( std::ostream & stream )
{
  Box<int4> box1( 1, 2, 3, 4, 5, 6 );
  Box<int4> box2( 5, 6, 7, 8, 9, 10 );
  Box<int4> box3( 5, 6, 7, 8, 9, 10 );

  if ( ( box1 != box2 ) == false ) return false;
  if ( ( box1 != box3 ) == false ) return false;
  if ( ( box2 != box3 ) == true ) return false;

  return true;
} // ValidateInequality()


/**
 * validate inner radius
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateInnerRadius( std::ostream & stream )
{
  Box<float4> box( 1, 2, 3, 4, 5, 6 );
  return box.GetInnerRadius() == 1.5F;
} // ValidateInnerRadius()


/**
 * validate outer radius
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestBox::ValidateOuterRadius( std::ostream & stream )
{
  Box<float4> box( 1, 2, 3, 4, 5, 6 );
  const float4 fEpsilon( 1e-5F );
  return Abs( box.GetOuterRadius() - sqrt(27.0f)/2 ) < fEpsilon;
} // ValidateOuterRadius()


// $Log: vxTestBox.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/15 20:11:56  frank
// code review
//
// Revision 1.1  2003/05/15 16:00:36  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestBox.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestBox.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
