// $Id: vxTestMath.C,v 1.1 2005/05/10 20:03:33 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "vxTestMath.h"
#include <iostream>
#include <ios>
#include "Timer.h"


// namespaces
using namespace std;


/**
 * Unit test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTestMath::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  //VXTEST_VALIDATE_FUNCTION(stream, "vxTestMath", "ValidateWhatever", ValidateWhatever, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTestMath::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  stream << "There is currently no performance test implemented in module: " << GetName() << endl;

  return true;
} // PerformanceTest()


// $Log: vxTestMath.C,v $
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestMath.C,v 1.1 2005/05/10 20:03:33 frank Exp $
// $Id: vxTestMath.C,v 1.1 2005/05/10 20:03:33 frank Exp $
