// $Id: vxTest.h,v 1.3 2005/07/05 18:30:22 vxconfig Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This class is a pure interface class defining the
  frame-work for a testing environment.
*/

#ifndef vxTest_h
#define vxTest_h

#ifndef VX_BASE_DLL
#define VX_BASE_DLL
#endif
#ifndef VX_BLOCK_VOLUME_DLL
#define VX_BLOCK_VOLUME_DLL
#endif


// includes
#include <map>
#include "Global.h"


// defines
#define VXTEST_VALIDATE_SUBMODULE(stream,funcName,func,bOkAll)                \
  {                                                                           \
    bool bOk(true);                                                           \
    for (iter=vpTest.begin(); iter != vpTest.end(); ++iter)                   \
    {                                                                         \
      cout << "================================" << endl;                     \
      cout << funcName << ": " << (*iter)->GetName().c_str() << endl;         \
      bOkAll &= bOk = (*iter)->func;                                        \
      cout << "... completed performance test " << (*iter)->GetName().c_str() << " (" << ((bOk == true) ? "succesful" : "FAILED") << ")" << endl; \
    } \
  }
/*
  // clean up
  /////////////
  for (std::vector<vxTest *>::iterator iter=vpTest.begin(); iter != vpTest.end(); ++iter)
  {
    delete *iter;
    *iter = NULL;
  }
*/

#define VXTEST_VALIDATE_FUNCTION(stream,moduleName,funcName,func,bResult)     \
  {                                                                           \
    stream << "===============================================" << endl;      \
    stream << "Function: " << moduleName << "::" << funcName << "()" << endl; \
    bool __bOk(true);                                                           \
    __bOk = func(stream);                                                       \
    if (__bOk == true)                                                          \
    {                                                                         \
      stream << " ... succeeded" << endl;                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      stream << " ... FAILED" << endl;                                        \
    }                                                                         \
    bResult &= __bOk;                                                           \
  }


// class definition
class vxTest
{
public:
// enums
  // camera view types
  enum vxTestEnum
  {
    UNIT        = 0,
    PERFORMANCE = 1
  }; 

// functions
public:

  /// default constructor
  vxTest() {};

  /// destructor
  virtual ~vxTest() {};

  /// get name of test
  virtual std::string GetName() const = 0;

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream) = 0;

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream) = 0;

  /// get root testing path
  static std::string GetRootFilePath() { return m_sPath; }

  /// set root testing path
  static void SetRootFilePath(const std::string & sPath) { m_sPath = sPath; }

  /// get full testing path
  std::string GetFullFilePath() { return m_sPath + "/" + GetName() + "/"; }

  /// perform test for a submodule
  static bool PerformAndFormatTest(std::ostream & stream, std::vector<vxTest *> & vpTest, vxTestEnum eTest, std::string & sLabel);

  /// clean a vector
  static void CleanVector(std::vector<vxTest *> & vpTest);

private:

  /// copy constructor
  vxTest(const vxTest & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTest & operator=(const vxTest & other); // should only be public if really implemented!

// data
private:

  /// main testing path
  static std::string m_sPath;
}; // vxTest


#endif // vxTest_h


// $Log: vxTest.h,v $
// Revision 1.3  2005/07/05 18:30:22  vxconfig
// Added Map header
//
// Revision 1.2  2005/05/12 13:55:13  frank
// added more unit and performance tests
//
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.4  2003/05/15 20:11:36  frank
// Fixed logic so that cumulative result is the AND of all previous results
//
// Revision 1.3  2003/05/02 20:17:29  michael
// added performance tests
//
// Revision 1.2  2003/05/02 18:28:53  michael
// added more stuff
//
// Revision 1.1  2003/05/01 20:44:33  michael
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTest.h,v 1.3 2005/07/05 18:30:22 vxconfig Exp $
// $Id: vxTest.h,v 1.3 2005/07/05 18:30:22 vxconfig Exp $
