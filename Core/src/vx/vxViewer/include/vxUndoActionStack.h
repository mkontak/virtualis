// $Id: vxUndoActionStack.h,v 1.6 2007/06/18 15:50:20 romy Exp $
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


// includes
#include <stack>


// forwards
class vxUndoAction;


class VX_VIEWER_DLL vxUndoActionStack
{
private:
	///Function pointer to indicate the modification of Undo stack
  typedef void ( * StackModifiedFuncPtr )( );

	///Removes old Undo actions which are in the stack for a while.
	void RemoveGarbageActions();
	
// member functions
public:

	//default constructor
	vxUndoActionStack();

  /// is the stack empty
  bool IsEmpty();

  /// push one action onto the stack
  void PushAction( vxUndoAction * pUndoAction );

  /// retrieves the action on the top of the stack
  vxUndoAction * TopAction();

  /// pop one action off the stack
  void PopAction();

  //Initialize the Undo Action stack method.
  static void SetModifiedEventFunctionPtr( StackModifiedFuncPtr pStackModifiedFuncPtr );

	///Set the stack count which determines the total number of undo actions.
	void SetStackLimit( int4 count ) { m_iStackLimit = count; }
    

// data members
private:

  /// the stack of actions
	std::deque< vxUndoAction * > m_actions;

  ///Stack Modified function pointer
  static StackModifiedFuncPtr m_pStackModifiedFuncPtr;

	///keeps a limit for the Undo stack.
	int4 m_iStackLimit;

}; // class vxUndoActionStack


// $Log: vxUndoActionStack.h,v $
// Revision 1.6  2007/06/18 15:50:20  romy
// added code to set the stack limit
//
// Revision 1.5  2007/01/04 18:21:32  romy
// limits the stack limit to 100
//
// Revision 1.4  2006/09/26 20:28:53  romy
// modified the stack with dequeue
//
// Revision 1.2  2006/04/25 20:18:51  romy
// added function pointer
//
// Revision 1.1  2005/12/08 16:39:45  frank
// added undo support
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoActionStack.h,v 1.6 2007/06/18 15:50:20 romy Exp $
// $Id: vxUndoActionStack.h,v 1.6 2007/06/18 15:50:20 romy Exp $
