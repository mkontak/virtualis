// $Id: vxControllerGlobalSyncViewers.h,v 1.1.2.1 2009/07/28 14:40:06 kchalupa Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: cqian (mailto:cqian@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxControllerGlobal.h"


// forward declarations
class vxEnvironment;


/**
 * Notifies all viewers with slice shadows enabled when they need to update.
 */
class VX_VIEWER_DLL vxControllerGlobalSyncViewers  : public vxControllerGlobal
{
// construction
public:

  /// constructor
  vxControllerGlobalSyncViewers();

  /// destructor
  virtual ~vxControllerGlobalSyncViewers();

// member functions
public:

  /// determines if processing is necessary
  virtual bool IsProcessingNecessary() { return true; }

  /// processes the controller
  virtual void Process();

// member variables
private:

  /// synch enabled
  bool m_bIsLastEnabled;

}; // class vxControllerGlobalSyncViewers


// $Log: vxControllerGlobalSyncViewers.h,v $
// Revision 1.1.2.1  2009/07/28 14:40:06  kchalupa
// Code Walkthrough
//
// Revision 1.1  2008/05/02 04:58:41  cqian
// check in
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerGlobalSyncViewers.h,v 1.1.2.1 2009/07/28 14:40:06 kchalupa Exp $
// $Id: vxControllerGlobalSyncViewers.h,v 1.1.2.1 2009/07/28 14:40:06 kchalupa Exp $
