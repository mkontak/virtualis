// $Id: vxTestVarMap.h,v 1.2 2005/05/25 14:05:55 dongqing Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestVarMap_h
#define vxTestVarMap_h


// includes
#include "vxTest_03_utils.h"


/*
 * This class implements the test interface for the value range class
 */
class vxTestVarMap : public vxTest_03_utils
{
// member functions
public:

  /// default constructor
  vxTestVarMap() {};

  /// destructor
  virtual ~vxTestVarMap() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestVarMap"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestVarMap(const vxTestVarMap & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestVarMap & operator=(const vxTestVarMap & other); // should only be public if really implemented!

  /// validate constructors
  bool ValidateConstructors( std::ostream & stream );

  /// Test setting header variables
  bool ValidateSetHdrVar( std::ostream & stream );

}; // vxTestVarMap


#endif // vxTestVarMap_h


// $Log: vxTestVarMap.h,v $
// Revision 1.2  2005/05/25 14:05:55  dongqing
// added another test
//
// Revision 1.1  2005/05/25 14:02:18  dongqing
// added test for VarMap
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestVarMap.h,v 1.2 2005/05/25 14:05:55 dongqing Exp $
// $Id: vxTestVarMap.h,v 1.2 2005/05/25 14:05:55 dongqing Exp $
