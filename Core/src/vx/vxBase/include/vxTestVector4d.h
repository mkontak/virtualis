// $Id: vxTestVector4d.h,v 1.1 2005/05/10 20:03:15 frank Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#ifndef vxTestVector4d_h
#define vxTestVector4d_h

// includes
#include "vxTest_10_math.h"
#include "Vector4d.h"


/**
 * Optimized Block Volume Neighbor Iterator Test.
 */
class vxTestVector4d : public vxTest_10_math
{
// Member Functions
public:
  /// default constructor
  vxTestVector4d() {};

  /// destructor
  virtual ~vxTestVector4d() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestVector4d"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:
  ////////////////////////////////////////////////////////////////////////
  /// Demo

  /// demonstrate basic class functionality
  bool Demo(std::ostream & stream);

  ////////////////////////////////////////////////////////////////////////
  /// Validation

  // Validate clearing volume
  bool ValidateConstructors(std::ostream & stream);


  ////////////////////////////////////////////////////////////////////////
  /// Performance test


}; // vxTestVector4d


#endif // vxTestVector4d_h

// $Log: vxTestVector4d.h,v $
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.1  2003/05/20 14:53:52  dongqing
// initialization
//
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestVector4d.h,v 1.1 2005/05/10 20:03:15 frank Exp $
// $Id: vxTestVector4d.h,v 1.1 2005/05/10 20:03:15 frank Exp $
