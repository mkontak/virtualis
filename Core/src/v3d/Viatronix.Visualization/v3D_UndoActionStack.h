// $Id: v3D_UndoActionStack.h,v 1.3 2007/06/18 15:50:54 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )

// pragmas
#pragma once

// includes

// namespace declaration
OPEN_UNDOACTIONS_NS

// class declaration
public ref class UndoActionStack sealed
{
public:

	/// Undo the last action in the stack
  static void UndoLastAction();

	/// check the emptiness of the undo stack
	static bool IsUndoStackEmpty();

	///Sets the stack count.
	static void SetStackLimit( const int4 iCount );

private:
  
  // default constructor
  UndoActionStack() {}
		
}; // class Animator

CLOSE_UNDOACTIONS_NS

// $Log: v3D_UndoActionStack.h,v $
// Revision 1.3  2007/06/18 15:50:54  romy
// added code to set the stack limit
//
// Revision 1.2  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2006/09/28 21:04:45  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_UndoActionStack.h,v 1.3 2007/06/18 15:50:54 romy Exp $
// $Id: v3D_UndoActionStack.h,v 1.3 2007/06/18 15:50:54 romy Exp $

