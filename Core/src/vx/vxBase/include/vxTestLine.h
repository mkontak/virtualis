// $Id: vxTestLine.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestLine_h
#define vxTestLine_h


// includes
#include "vxTest_10_math.h"


/*
* This class implements the test interface for the volume classes.
* Most of the initial tests were taken from Ingmar's previous tests.
*/
class vxTestLine : public vxTest_10_math
{
// member functions
public:

  /// default constructor
  vxTestLine() {};

  /// destructor
  virtual ~vxTestLine() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestLine"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestLine(const vxTestLine & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestLine & operator=(const vxTestLine & other); // should only be public if really implemented!

  /// validate constructors
  bool ValidateConstructors( std::ostream & stream );

  /// validate get point
  bool ValidateGetPoint( std::ostream & stream );

  /// validate get direction
  bool ValidateGetDistance( std::ostream & stream );

  /// validate get intersection
  bool ValidateGetIntersection( std::ostream & stream );

}; // vxTestLine


#endif // vxTestLine_h


// $Log: vxTestLine.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/14 20:41:00  frank
// added Line unit test
//
// Revision 1.2  2003/05/14 19:33:24  frank
// code review
//
// Revision 1.1  2003/05/14 18:10:10  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestLine.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestLine.h,v 1.1 2005/05/12 13:57:32 frank Exp $
