// $Id: vxTestMathGfx.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestMathGfx.h"


// namespaces
using namespace std;


/**
 * Unit test of math graphics stuff
 * @param out stream
 * @return success
 */
bool vxTestMathGfx::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  //VXTEST_VALIDATE_FUNCTION(stream, "vxTestMathGfx", "ValidateWhatever", ValidateWhatever, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of math graphics stuff
 * @param out stream
 * @return success
 */
bool vxTestMathGfx::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  stream << "There is currently no performance test implemented in module: " << GetName() << endl;

  return true;
} // PerformanceTest()


// $Log: vxTestMathGfx.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestMathGfx.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestMathGfx.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
