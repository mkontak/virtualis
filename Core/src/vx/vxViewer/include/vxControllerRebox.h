// $Id: vxControllerRebox.h,v 1.3 2004/09/27 14:14:32 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxControllerRebox_h
#define vxControllerRebox_h


// includes
#include "vxController.h"


/**
 * An abstract base class to define the operations of a controller.
 * A controller takes the current environment (ideally a modified queue)
 * and modifies it according to some logic.
 */
class VX_VIEWER_DLL vxControllerRebox : public vxController
{
// functions
public:

  /// constructor
  vxControllerRebox(vxEnvironment & environment);

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

}; // class vxControllerRebox


#endif // vxControllerRebox_h


// Revision History:
// $Log: vxControllerRebox.h,v $
// Revision 1.3  2004/09/27 14:14:32  frank
// added pre-processing check to controller interface
//
// Revision 1.2  2004/08/30 18:19:38  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.1  2004/05/20 13:25:06  frank
// initial version of class
//
// Revision 1.1  2004/05/19 15:47:08  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerRebox.h,v 1.3 2004/09/27 14:14:32 frank Exp $
// $Id: vxControllerRebox.h,v 1.3 2004/09/27 14:14:32 frank Exp $
