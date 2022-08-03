// $Id: vxUndoAction.h,v 1.4 2006/09/27 17:49:02 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Derive from this class to create an undo action
 */


// pragmas
#pragma once


class VX_VIEWER_DLL vxUndoAction
{
// member functions
public:

  /// perform the undo action
  virtual void ExecuteAction() = 0;
	
	//Destructor
	virtual ~vxUndoAction(){};

}; // class vxUndoAction


/**
 * Use an object of this class to begin a composite sequence of undo actions.
 * This will signal the undo mechanism to continue undoing actions all at once
 * until the end object is received.
 */
class VX_VIEWER_DLL vxUndoActionCompositeBegin : public vxUndoAction
{
// member functions
public:

  /// do nothing
  void ExecuteAction() {}

	//Destructor
	virtual ~vxUndoActionCompositeBegin(){}

}; // class vxUndoActionCompositeBegin


/**
 * Use an object of this class to end a composite sequence of undo actions.
 */
class VX_VIEWER_DLL vxUndoActionCompositeEnd : public vxUndoAction
{
// member functions
public:

  /// do nothing
  void ExecuteAction() {}

	//Destructor
	virtual ~vxUndoActionCompositeEnd(){}

}; // class vxUndoActionCompositeEnd


// $Log: vxUndoAction.h,v $
// Revision 1.4  2006/09/27 17:49:02  romy
// added virtual destructor to UndoActions
//
// Revision 1.3  2006/03/02 15:46:41  frank
// added composite undo actions
//
// Revision 1.2  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.1  2005/12/08 16:39:45  frank
// added undo support
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoAction.h,v 1.4 2006/09/27 17:49:02 romy Exp $
// $Id: vxUndoAction.h,v 1.4 2006/09/27 17:49:02 romy Exp $
