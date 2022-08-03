// $Id: vxTestIntersection.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestIntersection_h
#define vxTestIntersection_h


// includes
#include "vxTest_10_math.h"


/*
 * This class implements the test interface for the volume classes.
 * Most of the initial tests were taken from Ingmar's previous tests.
 */
class vxTestIntersection : public vxTest_10_math
{
// member functions
public:

  /// default constructor
  vxTestIntersection() {};

  /// destructor
  virtual ~vxTestIntersection() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestIntersection"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestIntersection(const vxTestIntersection & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestIntersection & operator=(const vxTestIntersection & other); // should only be public if really implemented!

  /// validate line-plane intersection
  bool ValidateLinePlane( std::ostream & stream );

  /// validate plane-plane intersection
  bool ValidatePlanePlane( std::ostream & stream );

}; // vxTestIntersection


#endif // vxTestIntersection_h


// $Log: vxTestIntersection.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.2  2003/08/31 03:50:40  frank
// Added test for plane-plane intersection
//
// Revision 1.1  2003/05/20 15:44:12  frank
// Added test for Intersection namespace
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestIntersection.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestIntersection.h,v 1.1 2005/05/12 13:57:32 frank Exp $
