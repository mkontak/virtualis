// $Id: vxControllerVesselSeedpoints.h,v 1.1.2.2 2009/07/28 14:40:06 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once


// includes
#include "vxController.h"
#include "vxSeedpoints.h"
#include "vxShareableObject.h"

/**
 * Controller for vessel seed points
 */
class VX_VIEWER_DLL vxControllerVesselSeedpoints : public vxController
{
// construction
public:

  /// constructor
  vxControllerVesselSeedpoints( vxEnvironment & environment );

  /// destructor
  virtual ~vxControllerVesselSeedpoints() {}

// member functions
public:

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

// member variables
private:

  /// Get the const seedpoints from the environment
  vxShareableObject< vxSeedpoints > * GetSeedpoints();

}; // class vxControllerVesselSeedpoints


// $Log: vxControllerVesselSeedpoints.h,v $
// Revision 1.1.2.2  2009/07/28 14:40:06  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2009/03/31 21:04:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerVesselSeedpoints.h,v 1.1.2.2 2009/07/28 14:40:06 kchalupa Exp $
// $Id: vxControllerVesselSeedpoints.h,v 1.1.2.2 2009/07/28 14:40:06 kchalupa Exp $
