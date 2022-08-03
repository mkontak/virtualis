// $Id: vxTestMemoryBuffer.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestMemoryBuffer.h"
#include "MemoryBuffer.h"


// namespaces
using namespace std;


/**
 * Unit test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestMemoryBuffer::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestMemoryBuffer", "ValidateConstructors", ValidateConstructors, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestMemoryBuffer", "ValidateCompression", ValidateCompression, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestMemoryBuffer", "ValidateCopy", ValidateCopy, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestMemoryBuffer", "ValidateComparison", ValidateComparison, bOk );

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
bool vxTestMemoryBuffer::PerformanceTest(std::ostream & stream)
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
bool vxTestMemoryBuffer::ValidateConstructors( std::ostream & stream )
{
  MemoryBuffer a(10);
  a.Resize(3);
  a.GetDataPtr()[2] = 0;
  if (a.GetSize() != 3)
    return false;

  return true;
} // ValidateConstructors()


/**
 * validate compression
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestMemoryBuffer::ValidateCompression( std::ostream & stream )
{
  // Test compression
  const uint4 uSize( 3 );
  uint1 vData[ uSize ] = { 43, 52, 61 };
  MemoryBuffer a(uSize);
  a.CopyIn(vData, uSize);
  MemoryBuffer b(20), c(20);
  a.DeflateInto(hdr::LZO, b);
  b.InflateInto(hdr::LZO, c);
  if (a != c)
  {
    return false;
  }

  return true;
} // ValidateCompression()


/**
 * validate copy
 *
 * @param stream out stream
 *
 *  @return success
 */
bool vxTestMemoryBuffer::ValidateCopy( std::ostream & stream )
{
  const uint4 uSize( 3 );
  uint1 vData[ uSize ] = { 43, 52, 61 };
  MemoryBuffer a(10);
  a.CopyIn(vData, uSize);
  for( uint4 i(0); i<uSize; ++i )
  {
    if ( a.GetDataPtr()[i] != vData[i] )
    {
      return false;
    }
  }

  return true;
} // ValidateCopy()


/**
 * validate comparison
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestMemoryBuffer::ValidateComparison( std::ostream & stream )
{
  const uint4 uSize( 3 );
  uint1 vData[ uSize ] = { 43, 52, 61 };
  MemoryBuffer a(uSize);
  a.CopyIn(vData, uSize);

  MemoryBuffer b(uSize);
  b.CopyIn(vData, uSize);

  MemoryBuffer c(uSize);

  // validate ==
  if ( ( a == b ) == false ) return false;
  if ( ( a == c ) == true  ) return false;
  if ( ( b == c ) == true  ) return false;

  // validate !=
  if ( ( a != b ) == true  ) return false;
  if ( ( a != c ) == false ) return false;
  if ( ( b != c ) == false ) return false;

  return true;
} // ValidateComparison()


// $Log: vxTestMemoryBuffer.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/16 15:06:58  frank
// improved test
//
// Revision 1.1  2003/05/16 14:55:44  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestMemoryBuffer.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestMemoryBuffer.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
