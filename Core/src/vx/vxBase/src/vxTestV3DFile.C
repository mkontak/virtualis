// $Id: vxTestV3DFile.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestV3DFile.h"
#include "V3DFile.h"
#include "VarMap.h"
#include "File.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Unit test of V3D File
 * @param out stream
 * @return success
 */
bool vxTestV3DFile::UnitTest(std::ostream & stream)
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestV3DFile", "ValidateRGBImage", ValidateRGBImage, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestV3DFile", "ValidateRGBAImage", ValidateRGBAImage, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestV3DFile", "ValidateIO", ValidateIO, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of V3D File stuff
 * @param out stream
 * @return success
 */
bool vxTestV3DFile::PerformanceTest(std::ostream & stream)
{
  stream << "There is currently no performance test implemented in module: " << GetName() << endl;

  return true;
} // PerformanceTest()


/**
 * validate RGB image packing/unpacking
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestV3DFile::ValidateRGBImage( std::ostream & stream )
{
  V3DFile x;
  Image<PixelRGB<uint1> > image1(Triple<uint4>(2,1,0), PixelRGB<uint1>(0,0,0));
  image1.SetPixel(Point2D<uint2>(0,0), PixelRGB<uint1>(1,2,3));
  image1.SetPixel(Point2D<uint2>(1,0), PixelRGB<uint1>(1,2,3));
  MemoryBuffer memBuf = x.PackImage(image1);
  Image<PixelRGB<uint1> > image2(Triple<uint4>(2,1,0), PixelRGB<uint1>(0,0,0));
  image2 = x.UnpackImage(memBuf, image1.GetDim());
  return image1 == image2;
} // ValidateRGBImage()


/**
 * validate RGBA image packing/unpacking
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestV3DFile::ValidateRGBAImage( std::ostream & stream )
{
  V3DFile x;
  Image<PixelRGBA<uint1> > image3(Triple<uint4>(2,1,0), PixelRGBA<uint1>(0,0,0,0));
  image3.SetPixel(Point2D<uint2>(0,0), PixelRGBA<uint1>(1,2,3,4));
  image3.SetPixel(Point2D<uint2>(1,0), PixelRGBA<uint1>(1,2,3,4));
  MemoryBuffer memBuf2 = x.PackImageRGBA(image3);
  Image<PixelRGBA<uint1> > image4(Triple<uint4>(2,1,0), PixelRGBA<uint1>(0,0,0,0));
  image4 = x.UnpackImageRGBA(memBuf2, image3.GetDim());
  return image3 == image4;
} // ValidateRGBAImage()


/**
 * validate file I/O
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestV3DFile::ValidateIO( std::ostream & stream )
{
  MemoryBuffer someData(3);
  someData[0] = 'a'; someData[1] = 'b'; someData[2] = 'c';
  vx::File tempFile1( vx::File::CreateTempFile( "TestV3DFile", "png" ) );
  vx::File tempFile2( vx::File::CreateTempFile( "TestV3DFile", "png" ) );
  V3DFile a,b,c;
  if ( a != b ) return false;

  try
  {
    a.SetTitle("Distance from Boundary volume");
    VarMap myVarMap;
    myVarMap.SetHdrVar("maxDfb", 10.2283F, "Maximum distance from boundary");
    a.SetComment(myVarMap.ToString());
    a.SetVariable(Variable("x", "value of x", ""));
    a.SetData(someData);
    a.Write( tempFile1.GetAbsolutePath() );
  }
  catch ( ex::VException & e )
  {
    stream << e.GetLogRecord().GetTextMessage() << endl;
    return false;
  }
  catch ( ... )
  {
    stream << "Unhandled exception" << endl;
    return false;
  }

  // test reading
  try
  {
    b.Read( tempFile1.GetAbsolutePath(), std::make_pair( 0.0F, 100.0F ) );
    b.Write( tempFile2.GetAbsolutePath() );
    c.Read( tempFile2.GetAbsolutePath(), std::make_pair( 0.0F, 100.0F ) );
  }
  catch ( ex::VException & e )
  {
    stream << e.GetLogRecord().GetTextMessage() + "v3d file read failed!" << endl;
    return false;
  }
  catch ( ... )
  {
    stream << "unhandled exception v3d file read test failed!" << endl;
    return false;
  }

  // equality test
  if ( a != b || a != c )
  {
    stream << "equality test failed!" << endl;
    return(false);
  }

  // print out the third generation variables
  stringstream tt;
  tt << "Variables:" << endl
    << c.GetVariable("x").GetName() << " - "
    << c.GetVariable("x").GetValue() << endl << endl;
  LogDbg(tt.str(),"V3DFile","Test"); 
  tt.str("");

  // print out the stored data
  tt << "Data:" << endl
    << "size=" << c.GetDataBuffer().GetSize() << " data=";
  LogDbg(tt.str(),"V3DFile","Test");
  tt.str("");
  for ( uint4 i(0); i<c.GetDataBuffer().GetSize(); ++i )
  {
    tt << c.GetDataBuffer()[i];
    LogDbg(tt.str(),"V3DFile","Test");
    tt.str("");
  }
  tt << endl;
  LogDbg(tt.str(),"V3DFile","Test");
  tt.str("");


  // test external buffer
  if ( c.GetDataBuffer() != someData )
  {
    stream << "external buffer test failed" << endl;
    return false;
  }

  // remove temporary files
  tempFile1.Delete();
  tempFile2.Delete();

  return true;
} // ValidateIO()


// undefines
#undef FILE_REVISION


// $Log: vxTestV3DFile.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/23 12:54:55  frank
// fixed location of temporary files
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/20 13:56:58  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestV3DFile.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestV3DFile.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
