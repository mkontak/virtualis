// $Id: vxTest_10_math.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTest_10_math.h"
#include "vxTestNormal3D.h"
#include "vxTestPlane.h"
#include "vxTestVector3D.h"
#include "vxTestVector4d.h"
#include "vxTestPoint3D.h"
#include "vxTestTriple.h"


// namespaces
using namespace std;


/**
 * Unit test of math
 * @param out stream
 * @return success
 */
bool vxTest_10_math::UnitTest(std::ostream & stream)
//*******************************************************************
{

  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  vpTest.push_back( new vxTestNormal3D );
  vpTest.push_back( new vxTestPlane );
  vpTest.push_back( new vxTestVector3D );
  vpTest.push_back( new vxTestVector4d );
  vpTest.push_back( new vxTestPoint3D );
  vpTest.push_back( new vxTestTriple );

  bool bOk = PerformAndFormatTest( stream, vpTest, vxTest::UNIT, sLabel );
  CleanVector( vpTest );

  return bOk;

} // UnitTest()


/**
 * Performance test of math
 * @param out stream
 * @return success
 */
bool vxTest_10_math::PerformanceTest(std::ostream & stream)
//*******************************************************************
{

  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  vpTest.push_back( new vxTestNormal3D );
  vpTest.push_back( new vxTestVector3D );
  vpTest.push_back( new vxTestVector4d );
  vpTest.push_back( new vxTestPoint3D );
  vpTest.push_back( new vxTestTriple );

  bool bOk = PerformAndFormatTest( stream, vpTest, vxTest::PERFORMANCE, sLabel );
  CleanVector( vpTest );

  return bOk;

} // PerformanceTest()


// $Log: vxTest_10_math.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:32  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.6  2003/08/31 03:51:20  frank
// Fixed logic so overall result is carried through to end
//
// Revision 1.5  2003/05/27 13:10:19  frank
// spacing
//
// Revision 1.4  2003/05/20 15:52:35  dongqing
// add more tests
//
// Revision 1.3  2003/05/20 14:54:00  dongqing
// initialization
//
// Revision 1.2  2003/05/14 19:33:38  frank
// code review
//
// Revision 1.1  2003/05/02 18:28:39  michael
// initial version
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTest_10_math.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTest_10_math.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
