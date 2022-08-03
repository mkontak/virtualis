// $Id: vxTestMathGfx.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This class implements the test interface for the logger classes.
*/

#ifndef vxTestMathGfx_h
#define vxTestMathGfx_h


// includes
#include "vxTest.h"


// class definition
class vxTestMathGfx : public vxTest
{
// functions
public:

  /// default constructor
  vxTestMathGfx() {};

  /// destructor
  virtual ~vxTestMathGfx() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestMathGfx"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestMathGfx(const vxTestMathGfx & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestMathGfx & operator=(const vxTestMathGfx & other); // should only be public if really implemented!
}; // vxTestMathGfx


#endif // vxTestMathGfx_h


// $Log: vxTestMathGfx.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestMathGfx.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestMathGfx.h,v 1.1 2005/05/12 13:57:32 frank Exp $
