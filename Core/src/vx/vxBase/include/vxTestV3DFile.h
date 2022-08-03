// $Id: vxTestV3DFile.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestV3DFile_h
#define vxTestV3DFile_h


// includes
#include "vxTest.h"


///
/// This class implements the test interface for the V3D File class.
///
class vxTestV3DFile : public vxTest
{
// functions
public:

  /// default constructor
  vxTestV3DFile() {};

  /// destructor
  virtual ~vxTestV3DFile() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestV3DFile"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

  /// validate RGB image packing/unpacking
  bool ValidateRGBImage( std::ostream & stream );

  /// validate RGBA image packing/unpacking
  bool ValidateRGBAImage( std::ostream & stream );

  /// validate file I/O
  bool ValidateIO( std::ostream & stream );

private:

  /// copy constructor
  vxTestV3DFile(const vxTestV3DFile & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestV3DFile & operator=(const vxTestV3DFile & other); // should only be public if really implemented!

}; // class vxTestV3DFile


#endif // vxTestV3DFile_h


// $Log: vxTestV3DFile.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/20 13:56:58  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestV3DFile.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestV3DFile.h,v 1.1 2005/05/12 13:57:32 frank Exp $
