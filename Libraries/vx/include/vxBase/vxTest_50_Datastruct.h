// $Id: vxTest_50_Datastruct.h,v 1.1 2005/05/12 13:57:44 frank Exp $
// 
// Copyright © 2003, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Owner: Frank Dachille (dachille@viatronix.com)

/*
  This class implements the test interface for the vxUtils classes.
*/

#ifndef vxTest_50_Datastruct_h
#define vxTest_50_Datastruct_h


// includes
#include "vxTest.h"


// class definition
class vxTest_50_datastruct : public vxTest
{
// functions
public:

  /// default constructor
  vxTest_50_datastruct() {};

  /// destructor
  virtual ~vxTest_50_datastruct() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTest_50_datastruct"; }

  /// unit test
  virtual bool UnitTest(std::ostream & stream);

  /// performance test
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTest_50_datastruct(const vxTest_50_datastruct & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTest_50_datastruct & operator=(const vxTest_50_datastruct & other); // should only be public if really implemented!

}; // vxTest_50_datastruct


#endif // vxTest_50_datastruct_h


// $Log: vxTest_50_Datastruct.h,v $
// Revision 1.1  2005/05/12 13:57:44  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/12 16:19:35  frank
// Added 50_datastruct tests
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTest_50_Datastruct.h,v 1.1 2005/05/12 13:57:44 frank Exp $
// $Id: vxTest_50_Datastruct.h,v 1.1 2005/05/12 13:57:44 frank Exp $
