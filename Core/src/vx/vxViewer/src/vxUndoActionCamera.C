// $Id: vxUndoActionCamera.C,v 1.6 2007/03/07 17:04:39 geconomos Exp $
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


// includes
#include "StdAfx.h"
#include "vxUndoActionCamera.h"
#include "vxEnvironment.h"
#include "vxUtils.h"


/**
 * constructor
 *
 * @param pEnvironment the environment
 */
vxUndoActionCamera::vxUndoActionCamera( vxShareableObject< vxCamera > * pCamera )
: vxUndoAction(),
m_cameraPrevious( * pCamera ),
m_pCameraCurrent( pCamera )
{
} // vxUndoActionCamera()


/**
 * destructor
 */
vxUndoActionCamera::~vxUndoActionCamera()
{
}//~vxUndoActionCamera


/**
 * Undo the action
 */
void vxUndoActionCamera::ExecuteAction()
{

  // restore the previous oblique plane and camera
  * m_pCameraCurrent = m_cameraPrevious;

  // set modified
  m_pCameraCurrent->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

} // ExecuteAction()


// $Log: vxUndoActionCamera.C,v $
// Revision 1.6  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.5  2006/09/27 17:53:13  romy
// destructor added
//
// Revision 1.4  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.3  2006/02/24 16:57:00  frank
// added undo information to oblique plane manipulation
//
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1  2005/12/08 16:39:45  frank
// added undo support
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUndoActionCamera.C,v 1.6 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxUndoActionCamera.C,v 1.6 2007/03/07 17:04:39 geconomos Exp $
