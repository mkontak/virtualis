// $Id: vxControllerCursor3D.h,v 1.2 2005/05/20 19:09:39 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxController.h"


// forwards
class vxExplorer;


/**
 * Controller to update the plane based on the cursor 3D
 */
class VX_VIEWER_DLL vxControllerCursor3D : public vxController
{
// member functions
public:

  /// constructor
  vxControllerCursor3D( vxEnvironment & environment );

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

}; // class vxControllerCursor3D


// $Log: vxControllerCursor3D.h,v $
// Revision 1.2  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.1  2004/10/27 18:37:33  frank
// moved cursor 3D functionality to a controller
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerCursor3D.h,v 1.2 2005/05/20 19:09:39 frank Exp $
// $Id: vxControllerCursor3D.h,v 1.2 2005/05/20 19:09:39 frank Exp $
