// $Id: v3D_UndoActionStack.cpp,v 1.2 2007/06/18 15:50:54 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan(mailto:romy@viatronix.com)

// includes
#include "stdafx.h"
#include "v3D_UndoActionStack.h"

#include "vxUndoAction.h"
#include "vxUndoActionStack.h"
#include "vxEnvironment.h"

// defines
#define FILE_REVISION "$Revision: 1.2 $"


// namespaces
USING_UNDOACTIONS_NS


/**
 *Undo the last action 
 */
void UndoActionStack::UndoLastAction()
{
	try
  {
	  if ( !  vxEnvironment::GetUndoActions().IsEmpty() )
    {
      // undo it
			vxUndoAction * pAction = vxEnvironment::GetUndoActions().TopAction();
      const bool bEnd = ( dynamic_cast< vxUndoActionCompositeEnd * >( pAction ) != NULL );
      pAction->ExecuteAction();
      vxEnvironment::GetUndoActions().PopAction();
			delete pAction;

      // check for a composite action
      if ( bEnd )
      {
        bool bBegin = false;
        do
        {
          pAction = vxEnvironment::GetUndoActions().TopAction();
          bBegin = ( dynamic_cast< vxUndoActionCompositeBegin * >( pAction ) != NULL );
          pAction->ExecuteAction();
          vxEnvironment::GetUndoActions().PopAction();
					delete pAction;
        } while ( ! bBegin );
      }

      vxModifiedQueue::Flush();

    } // if something to do
  } // try
  catch (...)
  {
    LogWrn( "Unhandled exception", "UndoActionStack", "UndoAction" );
  }
}//UndoLastAction


/**
 *Checks for the emptiness of UndoAction Stack
 */
bool UndoActionStack::IsUndoStackEmpty()
{
	return vxEnvironment::GetUndoActions().IsEmpty();
}//IsUndoStackEmpty


/**
 * Sets the stack count
 */
void UndoActionStack::SetStackLimit( int4 iCount )
{
	vxEnvironment::GetUndoActions().SetStackLimit( iCount );
}//SetStackCount( int4 iCount )


// undefines
#undef FILE_REVISION

// $Log: v3D_UndoActionStack.cpp,v $
// Revision 1.2  2007/06/18 15:50:54  romy
// added code to set the stack limit
//
// Revision 1.1  2006/09/28 21:04:45  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_UndoActionStack.cpp,v 1.2 2007/06/18 15:50:54 romy Exp $
// $Id: v3D_UndoActionStack.cpp,v 1.2 2007/06/18 15:50:54 romy Exp $
