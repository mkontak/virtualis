// $Id: vxControllerVesselDetail.h,v 1.4 2007/03/02 17:19:14 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
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


// forwards
class vxVessel;
class vxViewerType;


/**
 * Synchronizes the detailed view of the selected vessel
 */
class VX_VIEWER_DLL vxControllerVesselDetail : public vxController
{
// member functions
public:

  /// constructor
  vxControllerVesselDetail( vxEnvironment & environment );

  /// destructor
  virtual ~vxControllerVesselDetail() {}

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

// data members
private:

  /// the last selected vessel
  vxVessel * m_pLastSelectedVessel;

  /// moving from start to end along vessel path
  bool m_bDraggingdEndoCameraForward;

  // previous vessel section index
  float4 m_fPreviousVesselSectionIndex;

  /// last viewer type
  vxViewerType * m_pLastViewerType;

}; // class vxControllerVesselDetail


// $Log: vxControllerVesselDetail.h,v $
// Revision 1.4  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.3  2006/11/22 21:48:54  gdavidson
// Check if the ViewerType has changed.
//
// Revision 1.2  2006/09/25 20:08:13  geconomos
// added support for 3d detail and endoluminal viewer types
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerVesselDetail.h,v 1.4 2007/03/02 17:19:14 geconomos Exp $
// $Id: vxControllerVesselDetail.h,v 1.4 2007/03/02 17:19:14 geconomos Exp $
