// $Id: vxControllerGlobalSyncPlane.h,v 1.1 2007/10/01 15:00:47 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxControllerGlobal.h"


// forward declarations
class vxEnvironment;


/**
 * Notifies all viewers with slice shadows enabled when they need to update.
 */
class VX_VIEWER_DLL vxControllerGlobalSyncPlane  : public vxControllerGlobal
{
// member functions
public:

  /// constructor
  vxControllerGlobalSyncPlane();

  /// destructor
  virtual ~vxControllerGlobalSyncPlane();

  /// determines if processing is necessary
  virtual bool IsProcessingNecessary() { return true; }

  /// processes the controller
  virtual void Process();

private:

  /// synch enabled
  bool m_bIsLastEnabled;

}; // class vxControllerGlobalSyncPlane


// $Log: vxControllerGlobalSyncPlane.h,v $
// Revision 1.1  2007/10/01 15:00:47  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerGlobalSyncPlane.h,v 1.1 2007/10/01 15:00:47 gdavidson Exp $
// $Id: vxControllerGlobalSyncPlane.h,v 1.1 2007/10/01 15:00:47 gdavidson Exp $
