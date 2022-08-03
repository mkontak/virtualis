// $Id: vxControllerRibbon.h,v 1.3 2007/03/02 17:19:14 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once


// includes
#include "vxController.h"


// forward declarations
class vxViewerType;


/**
 * Updates the ribbon for the curvetracer when dependent environment elements are modified.
 */
class VX_VIEWER_DLL vxControllerRibbon : vxController
{
// member functions
public:

  /// constructor
  vxControllerRibbon( vxEnvironment & environment );

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

private:

  /// last viewer type
  vxViewerType * m_pLastViewerType;

}; // class vxControllerRibbon


// $Log: vxControllerRibbon.h,v $
// Revision 1.3  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.2  2006/11/22 21:48:54  gdavidson
// Check if the ViewerType has changed.
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerRibbon.h,v 1.3 2007/03/02 17:19:14 geconomos Exp $
// $Id: vxControllerRibbon.h,v 1.3 2007/03/02 17:19:14 geconomos Exp $
