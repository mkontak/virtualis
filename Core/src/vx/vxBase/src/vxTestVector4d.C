// $Id: vxTestVector4d.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#include "stdafx.h"
#include "vxTestVector4d.h"
#include "Vector4d.h"

#define FILE_REVISION "$Revision: 1.2 $"

using namespace std;

#define Vector4D Vector4d

/**
 * Unit test of vxVolumeIterators stuff
 * @param out stream
 * @return success
 */
bool vxTestVector4d::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector4d", "ValidateConstructors",  ValidateConstructors, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTestVector4d::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector4d", "Demo",  Demo, bOk);

  return bOk;
} // PerformanceTest()

////////////////////////////////////////////////////////////////////////
/// Demo

/**
 * Demonstrate basic class functionality
 * @param stream
 * @return ture if success
 */
bool vxTestVector4d::Demo(std::ostream & stream)
{
  Vector4D<int4> v(1,2,3,4);

  v.m_x = 5;
  v.Y() = 6;
  v.m_z = 7;
  v.T() = 8;


  LogFyi(v.ToString(), "vxTestVector4d","Demo");

  return(true);
} // Demo


////////////////////////////////////////////////////////////////////////
/// Validation

/**
 * Validate Operator equal.
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector4d::ValidateConstructors(std::ostream & stream)
{
  bool bOk(true);

  //// for int4
  {
    Vector4d<int4> v(1,2,3,4);    LogFyi(v.ToString(), "vxTestVector4d","Demo");
    bOk &= (v.X() == 1);
    bOk &= (v.Y() == 2);
    bOk &= (v.Z() == 3);
    bOk &= (v.T() == 4);
  }

  //// for float4
  {
    Vector4d<float4> v(1.5,2.5,3.5,4.5);    LogFyi(v.ToString(), "vxTestVector4d","Demo");
    bOk &= (v.X() == 1.5);
    bOk &= (v.Y() == 2.5);
    bOk &= (v.Z() == 3.5);
    bOk &= (v.T() == 4.5);
  }

  return(bOk);
} // ValidateConstructors


////////////////////////////////////////////////////////////////////////
/// Performance test


#undef FILE_REVISION


// $Log: vxTestVector4d.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.1  2003/05/20 14:53:52  dongqing
// initialization
//
// Revision 1.3  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.2  2003/01/03 16:00:42  ingmar
// now with union of array and m_x/y/z/t
//
// Revision 1.1  2002/11/21 22:24:12  ingmar
// initial version
//
// Revision 1.1  2002/10/01 19:53:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestVector4d.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestVector4d.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $


