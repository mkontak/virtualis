// $Id: vxUndoActionPlane.C,v 1.4 2006/09/27 17:53:46 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Implements an undo action for segmentation
 */


// includes
#include "StdAfx.h"
#include "vxUndoActionPlane.h"
#include "vxEnvironment.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * constructor
 *
 * @param pPlaneOrtho the ortho plane object
 */
vxUndoActionPlaneOrtho::vxUndoActionPlaneOrtho( vxShareableObject< vxPlaneOrtho< float4 > > * pPlaneOrtho )
: vxUndoAction(),
m_planeOrthoPrevious( * pPlaneOrtho ),
m_pPlaneOrthoCurrent( pPlaneOrtho )
{
} // vxUndoActionPlaneOrtho()

/**
 * destructor
 */
vxUndoActionPlaneOrtho::~vxUndoActionPlaneOrtho()
{
}//~vxUndoActionPlaneOrtho

/**
 * Undo the action
 */
void vxUndoActionPlaneOrtho::ExecuteAction()
{

  // restore the previous ortho plane
  * m_pPlaneOrthoCurrent = m_planeOrthoPrevious;

  // set modified
  m_pPlaneOrthoCurrent->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );

} // ExecuteAction()


/**
 * constructor
 *
 * @param pPlaneOblique the oblique plane object
 */
vxUndoActionPlaneOblique::vxUndoActionPlaneOblique( vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique,
                                                    vxShareableObject< vxCamera > * pCamera )
: vxUndoAction(),
m_planeObliquePrevious( * pPlaneOblique ),
m_pPlaneObliqueCurrent( pPlaneOblique ),
m_cameraPrevious( * pCamera ),
m_pCameraCurrent( pCamera )
{
} // vxUndoActionPlaneOblique()


/**
 * Undo the action
 */
void vxUndoActionPlaneOblique::ExecuteAction()
{

  // restore the previous oblique plane and camera
  * m_pPlaneObliqueCurrent = m_planeObliquePrevious;
  * m_pCameraCurrent = m_cameraPrevious;

  // set modified
  m_pPlaneObliqueCurrent->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );
  m_pCameraCurrent->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

} // ExecuteAction()


// undefines
#undef FILE_REVISION


// $Log: vxUndoActionPlane.C,v $
// Revision 1.4  2006/09/27 17:53:46  romy
// destructor added
//
// Revision 1.3  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.1  2006/02/24 16:47:55  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUndoActionPlane.C,v 1.4 2006/09/27 17:53:46 romy Exp $
// $Id: vxUndoActionPlane.C,v 1.4 2006/09/27 17:53:46 romy Exp $
