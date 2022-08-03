// $Id: vxElementStack.h,v 1.1 2004/03/26 15:52:23 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxElementStack_h
#define vxElementStack_h


// includes
#include <stack>
#include "vxElement.h"


/**
 * A stack of vxElements
 */
class VX_VIEWER_DLL vxElementStack : public std::stack< void * >, public vxElement
{
// member functions
public:

  /// constructor
  vxElementStack() {}

  /// destructor
  virtual ~vxElementStack() {}

}; // vxElementStack


#endif // vxElementStack_h


// $Log: vxElementStack.h,v $
// Revision 1.1  2004/03/26 15:52:23  frank
// added fusion element stack
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementStack.h,v 1.1 2004/03/26 15:52:23 frank Exp $
// $Id: vxElementStack.h,v 1.1 2004/03/26 15:52:23 frank Exp $
