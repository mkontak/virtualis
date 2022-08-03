// $Id: vxControllerGlobalSliceShadows.h,v 1.3 2007/03/02 17:19:14 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxControllerGlobal.h"


// forward declarations
class vxEnvironment;


/**
 * Notifies all viewers with slice shadows enabled when they need to update.
 */
class VX_VIEWER_DLL vxControllerGlobalSliceShadows  : public vxControllerGlobal
{
// member functions
public:

  /// constructor
  vxControllerGlobalSliceShadows();

  /// destructor
  virtual ~vxControllerGlobalSliceShadows();

  /// determines if processing is necessary
  virtual bool IsProcessingNecessary() { return true; }

  /// processes the controller
  virtual void Process();

}; // class vxControllerGlobalActiveEnvironment


// $Log: vxControllerGlobalSliceShadows.h,v $
// Revision 1.3  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.2  2007/02/22 16:07:37  geconomos
// code cleanup
//
// Revision 1.1  2006/10/24 12:30:02  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerGlobalSliceShadows.h,v 1.3 2007/03/02 17:19:14 geconomos Exp $
// $Id: vxControllerGlobalSliceShadows.h,v 1.3 2007/03/02 17:19:14 geconomos Exp $
