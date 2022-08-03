// $Id: vxTestTetrahedron.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestTetrahedron_h
#define vxTestTetrahedron_h


// includes
#include "vxTest_10_math.h"


/*
* This class implements the test interface for the volume classes.
* Most of the initial tests were taken from Ingmar's previous tests.
*/
class vxTestTetrahedron : public vxTest_10_math
{
// member functions
public:

  /// default constructor
  vxTestTetrahedron() {};

  /// destructor
  virtual ~vxTestTetrahedron() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestTetrahedron"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestTetrahedron(const vxTestTetrahedron & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestTetrahedron & operator=(const vxTestTetrahedron & other); // should only be public if really implemented!

  /// validate volume
  bool ValidateVolume( std::ostream & stream );

}; // vxTestTetrahedron


#endif // vxTestTetrahedron_h


// $Log: vxTestTetrahedron.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/21 14:21:58  frank
// Added test for vxTetrahedron
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestTetrahedron.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestTetrahedron.h,v 1.1 2005/05/12 13:57:32 frank Exp $
