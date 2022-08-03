// $Id: vxTest_20_image.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTest_20_image.h"
//#include "vxTestImage.h"
#include "vxTestMemoryBuffer.h"
#include "vxTestV3DFile.h"


// namespaces
using namespace std;


/**
 * Unit test of image module
 *
 * @param out stream
 * @return success
 */
bool vxTest_20_image::UnitTest(std::ostream & stream)
{
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  //vpTest.push_back(new vxTestImage);
  vpTest.push_back(new vxTestMemoryBuffer);
  vpTest.push_back(new vxTestV3DFile);

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::UNIT, sLabel);
  CleanVector(vpTest);

  return bOk;
} // UnitTest()


/**
 * Performance test of image module
 *
 * @param out stream
 * @return success
 */
bool vxTest_20_image::PerformanceTest(std::ostream & stream)
{
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  vpTest.push_back(new vxTestMemoryBuffer);
  vpTest.push_back(new vxTestV3DFile);

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::PERFORMANCE, sLabel);
  CleanVector(vpTest);

  return bOk;
} // PerformanceTest()


// $Log: vxTest_20_image.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:32  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/12 13:57:44  frank
// added more unit and performance tests
//
// Revision 1.3  2003/05/20 14:01:56  frank
// Moved V3DFile test to test environment
//
// Revision 1.2  2003/05/16 15:07:10  frank
// added some tests
//
// Revision 1.1  2003/05/02 18:28:39  michael
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTest_20_image.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTest_20_image.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
