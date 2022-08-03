// $Id: vxControllerGlobalActiveEnvironment.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
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
 * Notifies all viewers when the active environment has changed.
 */
class VX_VIEWER_DLL vxControllerGlobalActiveEnvironment  : public vxControllerGlobal
{
// member functions
public:

  /// constructor
  vxControllerGlobalActiveEnvironment();

  /// destructor
  virtual ~vxControllerGlobalActiveEnvironment();

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary() { return true; }

  /// processes the controller
  virtual void Process();

// member variables
private:

  /// last active environment
  vxEnvironment * m_pLastActiveEnvironment;

}; // class vxControllerGlobalActiveEnvironment


// $Log: vxControllerGlobalActiveEnvironment.h,v $
// Revision 1.2  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.1  2006/10/20 20:46:24  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerGlobalActiveEnvironment.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
// $Id: vxControllerGlobalActiveEnvironment.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
