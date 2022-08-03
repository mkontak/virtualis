// $Id: v3D_Events.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#include "stdafx.h"
#include "v3D_Events.h"

USING_VISUALIZATION_NS

/**
 * Raises the "EventCompleted"
 */
 void Events::RaiseToolCompleted( System::String ^ mpName )
{
  ToolCompleted( mpName );
} // FireModeComplete()


/**
 * Raises the "EventProgressPercentage"
 * @param fProgress Progress between 0 and 1.
 */
void Events::RaiseProgressPercentage( float fProgress )
{
  ProgressPercentage( fProgress );
} // RaiseProgressPercentage()


/**
 * Raises the "EventProgressMessage"
 * @param mpProgressMessage Progress message text.
 */
void Events::RaiseProgressMessage( System::String ^ mpProgressMessage )
{
  ProgressMessage( mpProgressMessage );
} // RaiseProgressMessage()

/**
 *Raises the UndoStack modified signal
 */

void Events::RaiseUndoActionSignal( )
{
  UndoStackModified();
} // RaiseUndoActionSignal()




// Revision History:
// $Log: v3D_Events.cpp,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/08/28 21:45:36  gdavidson
// Changed ToolCompleted delegate
//
// Revision 1.2  2006/04/25 20:21:38  romy
// StackModifiedFuncPtr added
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.12.2.1  2004/02/24 20:06:59  michael
// coding standard: assign NULL after deletion
//
// Revision 1.12  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.11  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.10  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.9  2003/05/06 17:25:51  geconomos
// Coding standards
//
// Revision 1.8  2003/04/23 20:10:52  geconomos
// Renamed event ModeCompleted to ToolCompleted
//
// Revision 1.7  2003/04/14 16:45:21  frank
// Separated progress percent and progress message interfaces
//
// Revision 1.6  2003/04/09 20:17:28  geconomos
// Updated progress bar to have status text.
//
// Revision 1.5  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.4  2003/02/11 23:41:04  frank
// spacing
//
// Revision 1.3.2.1  2003/02/12 23:01:35  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Events.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Events.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
