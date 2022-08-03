// $Id: vxTestPlane.h,v 1.1 2005/05/10 20:03:15 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestPlane_h
#define vxTestPlane_h


// includes
#include "vxTest_10_math.h"


/*
* This class implements the test interface for the volume classes.
* Most of the initial tests were taken from Ingmar's previous tests.
*/
class vxTestPlane : public vxTest_10_math
{
// member functions
public:

  /// default constructor
  vxTestPlane() {};

  /// destructor
  virtual ~vxTestPlane() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestPlane"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestPlane(const vxTestPlane & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestPlane & operator=(const vxTestPlane & other); // should only be public if really implemented!

  /// validate constructors
  bool ValidateConstructors( std::ostream & stream );

  /// validate get distance
  bool ValidateGetDistance( std::ostream & stream );

  /// validate is on positive side
  bool ValidateIsOnPositiveSide( std::ostream & stream );

  /// validate get point on plane
  bool ValidateGetPointOnPlane( std::ostream & stream );

  /// validate get normal
  bool ValidateGetNormal( std::ostream & stream );

  /// validate get projection onto plane
  bool ValidateGetProjectionOntoPlane( std::ostream & stream );

  /// validate get projection parallel to plane
  bool ValidateGetProjectionParallelToPlane( std::ostream & stream );

}; // vxTestPlane


#endif // vxTestPlane_h


// $Log: vxTestPlane.h,v $
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.2  2003/05/14 19:33:24  frank
// code review
//
// Revision 1.1  2003/05/14 18:10:10  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestPlane.h,v 1.1 2005/05/10 20:03:15 frank Exp $
// $Id: vxTestPlane.h,v 1.1 2005/05/10 20:03:15 frank Exp $
