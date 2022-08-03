// $Id: vxUndoActionStack.C,v 1.9 2007/06/18 15:50:19 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxUndoAction.h"
#include "vxUndoActionStack.h"

// static initialization
VX_VIEWER_DLL vxUndoActionStack::StackModifiedFuncPtr vxUndoActionStack::m_pStackModifiedFuncPtr( NULL );
//const int4 vxUndoActionStack::m_iStackLimit= 100;


/**
 * Constructor
 */
vxUndoActionStack::vxUndoActionStack()
{
	//currently use this value for stck count.
	m_iStackLimit= 100;
}//vxUndoActionStack()


/**
 * Is the stack empty?
 *
 * @return if the stack is empty
 */
bool vxUndoActionStack::IsEmpty()
{
 return m_actions.empty();
} // IsEmpty()


/**
 * Push one action onto the stack.
 * By calling this function you no longer own it so do not dispose of it yourself.
 *
 * @param pUndoAction the action to push onto the stack
 */
void vxUndoActionStack::PushAction( vxUndoAction * pUndoAction )
{
	//clean up all actions which are pending in the stack for a while.
	RemoveGarbageActions();

	if ( m_iStackLimit <= 0 )
		return;

	m_actions.push_front( pUndoAction );
  if ( m_pStackModifiedFuncPtr != NULL )
    ( * m_pStackModifiedFuncPtr ) ( );
} // PushAction( action )


/* 
 * Removes those undoActions which are in the stack for while.
 * Acts like a garbage collector ..
 */
void vxUndoActionStack::RemoveGarbageActions()
{
	//Pop half of the items in a temp stack then clear the 
	//original stack and then fill it back from the temp stack.
	
  if ( m_actions.size() >=  m_iStackLimit  && m_iStackLimit > 0 )
	{
		vxUndoAction * pAction = m_actions.back();
		delete pAction;
		m_actions.pop_back(); 
	}
}//RemoveGarbageActions()


/**
 * Retrieves the action on the top of the stack.
 * Throws an exception if the stack is empty.
 *
 * @return the top action
 */
vxUndoAction * vxUndoActionStack::TopAction()
{
  return m_actions.front();
} // TopAction()


/**
 * Pop one action off the stack.
 * Note that this releases all memory associated with the action.
 * Throws an exception if the stack is empty.
 *
 * @return the top action
 */
void vxUndoActionStack::PopAction()
{
	if ( m_iStackLimit <= 0 )
		return;

  m_actions.pop_front ();

  if ( m_pStackModifiedFuncPtr != NULL )
    ( * m_pStackModifiedFuncPtr ) ( );
} // PopAction()


/*
 * Initializes the Function pointer to which receives 
 * the signal that the stack is modified
 */
void vxUndoActionStack::SetModifiedEventFunctionPtr( StackModifiedFuncPtr pStackModifiedFuncPtr )
{
  m_pStackModifiedFuncPtr = pStackModifiedFuncPtr;
}//SetModifiedEventFunctionPtr( StackModifiedFuncPtr pStackModifiedFuncPtr )



// $Log: vxUndoActionStack.C,v $
// Revision 1.9  2007/06/18 15:50:19  romy
// added code to set the stack limit
//
// Revision 1.8  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.7  2007/01/04 18:21:32  romy
// limits the stack limit to 100
//
// Revision 1.6  2006/09/27 17:57:04  romy
// better memory management added
//
// Revision 1.5  2006/09/26 20:28:53  romy
// modified the stack with dequeue
//
// Revision 1.4  2006/09/26 15:17:38  romy
// modified the Initialize function name
//
// Revision 1.3  2006/09/22 20:49:53  romy
// added comments
//
// Revision 1.2  2006/04/25 20:20:01  romy
// StackModifiedFuncPtr added
//
// Revision 1.1  2005/12/08 16:39:45  frank
// added undo support
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUndoActionStack.C,v 1.9 2007/06/18 15:50:19 romy Exp $
// $Id: vxUndoActionStack.C,v 1.9 2007/06/18 15:50:19 romy Exp $
