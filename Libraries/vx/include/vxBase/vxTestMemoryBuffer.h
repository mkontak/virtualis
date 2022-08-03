// $Id: vxTestMemoryBuffer.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestMemoryBuffer_h
#define vxTestMemoryBuffer_h


// includes
#include "vxTest_03_utils.h"


/*
 * This class implements the test interface for the memory buffer class
 */
class vxTestMemoryBuffer : public vxTest_03_utils
{
// member functions
public:

  /// default constructor
  vxTestMemoryBuffer() {};

  /// destructor
  virtual ~vxTestMemoryBuffer() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestMemoryBuffer"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestMemoryBuffer(const vxTestMemoryBuffer & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestMemoryBuffer & operator=(const vxTestMemoryBuffer & other); // should only be public if really implemented!

  /// validate constructors
  bool ValidateConstructors( std::ostream & stream );

  /// validate compression
  bool ValidateCompression( std::ostream & stream );

  /// validate copy
  bool ValidateCopy( std::ostream & stream );

  /// validate comparison
  bool ValidateComparison( std::ostream & stream );

}; // vxTestMemoryBuffer


#endif // vxTestMemoryBuffer_h


// $Log: vxTestMemoryBuffer.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/16 14:55:44  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestMemoryBuffer.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestMemoryBuffer.h,v 1.1 2005/05/12 13:57:32 frank Exp $
