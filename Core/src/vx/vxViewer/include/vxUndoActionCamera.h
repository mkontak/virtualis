// $Id: vxUndoActionCamera.h,v 1.4 2006/09/27 17:53:33 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Implements an undo action for camera manipulation
 */


// pragmas
#pragma once


// includes
#include "vxUndoAction.h"
#include "vxShareableObject.h"
#include "vxCamera.h"


class VX_VIEWER_DLL vxUndoActionCamera : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionCamera( vxShareableObject< vxCamera > * pPreviousCamera );

  /// perform the undo action
  virtual void ExecuteAction();

	//destructor
	virtual ~vxUndoActionCamera();

// member data
private:

  /// the previous camera data
  vxCamera m_cameraPrevious;

  /// the current camera object
  vxShareableObject< vxCamera > * m_pCameraCurrent;

}; // class vxUndoActionCamera


// $Log: vxUndoActionCamera.h,v $
// Revision 1.4  2006/09/27 17:53:33  romy
// destructor added
//
// Revision 1.3  2006/02/24 16:57:01  frank
// added undo information to oblique plane manipulation
//
// Revision 1.2  2006/01/31 14:34:07  frank
// formatting
//
// Revision 1.1  2005/12/08 16:39:45  frank
// added undo support
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoActionCamera.h,v 1.4 2006/09/27 17:53:33 romy Exp $
// $Id: vxUndoActionCamera.h,v 1.4 2006/09/27 17:53:33 romy Exp $
