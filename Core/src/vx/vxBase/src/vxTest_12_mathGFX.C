// $Id: vxTest_12_mathGFX.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTest_12_mathGFX.h"
#include "vxTestLine.h"
#include "vxTestBoundingRect.h"
#include "vxTestBox.h"
#include "vxTestIntersection.h"
#include "vxTestPrimitiveUtilities.h"
#include "vxTestTetrahedron.h"
//#include "vxTestSingularValueDecomp.h"


// namespaces
using namespace std;


/**
 * Unit test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTest_12_mathGFX::UnitTest(std::ostream & stream)
{

  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  vpTest.push_back(new vxTestLine);
  vpTest.push_back(new vxTestBoundingRect);
  vpTest.push_back(new vxTestBox);
  vpTest.push_back(new vxTestIntersection);
  vpTest.push_back(new vxTestPrimitiveUtilities);
  vpTest.push_back(new vxTestTetrahedron);
  //vpTest.push_back(new vxTestSingularValueDecomp);

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::UNIT, sLabel);
  CleanVector(vpTest);

  return bOk;

} // UnitTest()


/**
 * Performance test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTest_12_mathGFX::PerformanceTest(std::ostream & stream)
{

  stream << "There is currently no performance test implemented in module: " << GetName() << endl;
  return true;

} // PerformanceTest()


// $Log: vxTest_12_mathGFX.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:32  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/12 13:57:44  frank
// added more unit and performance tests
//
// Revision 1.7  2003/07/25 15:31:36  dongqing
// add singular value decomposition test
//
// Revision 1.6  2003/05/21 14:21:40  frank
// Added test for vxTetrahedron
//
// Revision 1.5  2003/05/20 16:24:06  frank
// Added test for namespace PrimitiveUtilities
//
// Revision 1.4  2003/05/20 15:44:56  frank
// Added test for Intersection namespace
//
// Revision 1.3  2003/05/15 15:56:27  frank
// Added bounding rect, box, and value range tests
//
// Revision 1.2  2003/05/14 20:41:11  frank
// added Line unit test
//
// Revision 1.1  2003/05/02 18:28:39  michael
// initial version
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTest_12_mathGFX.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTest_12_mathGFX.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
