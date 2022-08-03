// $Id: vxUndoActionPlane.h,v 1.2 2006/09/27 17:54:09 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Implements undo for various plane manipulation actions
 */


// pragmas
#pragma once


// includes
#include "vxUndoAction.h"
#include "vxShareableObject.h"
#include "vxPlaneOrtho.h"
#include "vxPlaneOblique.h"
#include "vxCamera.h"


/**
 * Undo a recently modified ortho plane
 */
class VX_VIEWER_DLL vxUndoActionPlaneOrtho : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionPlaneOrtho( vxShareableObject< vxPlaneOrtho< float4 > > * pPlaneOrtho );

  /// perform the undo action
  virtual void ExecuteAction();

	///destructor
	virtual ~vxUndoActionPlaneOrtho();

// member data
private:

  /// the previous plane object
  vxPlaneOrtho< float4 > m_planeOrthoPrevious;

  /// the current plane object
  vxShareableObject< vxPlaneOrtho< float4 > > * m_pPlaneOrthoCurrent;

}; // class vxUndoActionPlaneOrtho


/**
 * Undo a recently modified oblique plane
 */
class VX_VIEWER_DLL vxUndoActionPlaneOblique : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionPlaneOblique( vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique, vxShareableObject< vxCamera > * pCamera );

  /// perform the undo action
  virtual void ExecuteAction();

// member data
private:

  /// the previous plane object
  vxPlaneOblique<float4> m_planeObliquePrevious;

  /// the current plane object
  vxShareableObject< vxPlaneOblique<float4> > * m_pPlaneObliqueCurrent;

  /// the previous camera data
  vxCamera m_cameraPrevious;

  /// the current camera object
  vxShareableObject< vxCamera > * m_pCameraCurrent;

}; // class vxUndoActionPlaneOblique


// $Log: vxUndoActionPlane.h,v $
// Revision 1.2  2006/09/27 17:54:09  romy
// destructor added
//
// Revision 1.1  2006/02/24 16:47:55  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoActionPlane.h,v 1.2 2006/09/27 17:54:09 romy Exp $
// $Id: vxUndoActionPlane.h,v 1.2 2006/09/27 17:54:09 romy Exp $
