// $Id: vxUndoActionSegmentation.h,v 1.4 2006/09/27 17:55:26 romy Exp $
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


// pragmas
#pragma once


// includes
#include "vxUndoAction.h"
#include "vxShareableObject.h"
#include "vxBlockVolume.h"
#include "vxComponentArray.h"


class VX_VIEWER_DLL vxUndoActionSegmentation : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionSegmentation( vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                            vxShareableObject< vxComponentArray > * pComponents );

  /// perform the undo action
  virtual void ExecuteAction();

	//destructor
	virtual ~vxUndoActionSegmentation();

// member data
private:

  /// the old block volume state
  vxBlockVolume<uint2> m_labelVolumePrevious;

  /// the current block object
  vxShareableObject< vxBlockVolume<uint2> > * m_pLabelVolumeCurrent;

  /// the old component array state
  vxComponentArray m_componentsPrevious;

  /// the current component array object
  vxShareableObject< vxComponentArray > * m_pComponentsCurrent;


}; // class vxUndoActionSegmentation


class VX_VIEWER_DLL vxUndoActionComponents : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionComponents( vxShareableObject< vxComponentArray > * pComponents );

  /// perform the undo action
  virtual void ExecuteAction();

// member data
private:

  /// the old component array state
  vxComponentArray m_componentsPrevious;

  /// the current component array object
  vxShareableObject< vxComponentArray > * m_pComponentsCurrent;

}; // class vxUndoActionComponents


// $Log: vxUndoActionSegmentation.h,v $
// Revision 1.4  2006/09/27 17:55:26  romy
// destructor added
//
// Revision 1.3  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.1  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoActionSegmentation.h,v 1.4 2006/09/27 17:55:26 romy Exp $
// $Id: vxUndoActionSegmentation.h,v 1.4 2006/09/27 17:55:26 romy Exp $
