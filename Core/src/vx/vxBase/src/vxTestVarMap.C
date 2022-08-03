// $Id: vxTestVarMap.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestVarMap.h"
#include "Utilities.h"
#include "VarMap.h"


// namespaces
using namespace std;


/**
 * Unit test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestVarMap::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestVarMap", "ValidateConstructors", ValidateConstructors, bOk );

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
bool vxTestVarMap::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Test the constructor
 *
 * @param stream out stream
 * @return success
 */
bool vxTestVarMap::ValidateConstructors( std::ostream & stream )
{
  // test default constructor
  try
  {
    VarMap map;
    map.SetHdrVar( "asdf", "asdf", "asdf" );
    map.SetHdrVar( "asdf", "asdf", "asdf" );
  }
  catch (...)
  {
    return false;
  }

  return true;
} // ValidateConstructors()


/**
 * Test setting header variables
 *
 * @param stream out stream
 * @return success
 */
bool vxTestVarMap::ValidateSetHdrVar( std::ostream & stream )
{
  try
  {
    VarMap map;

    map.SetHdrVar( "bias", "0", "" );

    int iWindow(52);
    map.SetHdrVar( "Window", iWindow, "" );
  }
  catch (...)
  {
    return false;
  }

  return true;
} // ValidateSetHdrVar()


// $Log: vxTestVarMap.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/25 14:05:55  dongqing
// added another test
//
// Revision 1.1  2005/05/25 14:02:18  dongqing
// added test for VarMap
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestVarMap.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestVarMap.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
