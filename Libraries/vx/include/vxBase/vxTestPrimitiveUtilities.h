// $Id: vxTestPrimitiveUtilities.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestPrimitiveUtilities_h
#define vxTestPrimitiveUtilities_h


// includes
#include "vxTest_10_math.h"


/*
* This class implements the test interface for the volume classes.
* Most of the initial tests were taken from Ingmar's previous tests.
*/
class vxTestPrimitiveUtilities : public vxTest_10_math
{
// member functions
public:

  /// default constructor
  vxTestPrimitiveUtilities() {};

  /// destructor
  virtual ~vxTestPrimitiveUtilities() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestPrimitiveUtilities"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestPrimitiveUtilities(const vxTestPrimitiveUtilities & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestPrimitiveUtilities & operator=(const vxTestPrimitiveUtilities & other); // should only be public if really implemented!

  /// validate GetPointOnPlane
  bool ValidateGetPointOnPlane( std::ostream & stream );

  /// validate GetThreePointsOnPlane
  bool ValidateGetThreePointsOnPlane( std::ostream & stream );

  /// validate ProjectPointOntoLine
  bool ValidateProjectPointOntoLine( std::ostream & stream );

  /// validate GetPercentAlongSegment
  bool ValidateGetPercentAlongSegment( std::ostream & stream );

}; // vxTestPrimitiveUtilities


#endif // vxTestPrimitiveUtilities_h


// $Log: vxTestPrimitiveUtilities.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/20 16:28:36  frank
// cleaned up log
//
// Revision 1.1  2003/05/20 16:23:26  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestPrimitiveUtilities.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestPrimitiveUtilities.h,v 1.1 2005/05/12 13:57:32 frank Exp $
