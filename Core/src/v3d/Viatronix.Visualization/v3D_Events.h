// $Id: v3D_Events.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class Events
{

// delegates
public:

  // delegate for "ToolCompleted" event
  delegate void ToolCompletedHandler( System::String ^ mpName );

  // delegate for "ProgressPercentage" event
  delegate void ProgressPercentageHandler( float fProgress );

  // delegate for "ProgressMessage" event
  delegate void ProgressMessageHandler( System::String ^ mpProgress );

  //delegate for "UndoStackModified" event
  delegate void UndoStackModifiedHandler();


// events
public:

  // "ModeComplete" event
  static event ToolCompletedHandler ^ ToolCompleted;

  // "ModeProgressPercentage" event
  static event ProgressPercentageHandler ^ ProgressPercentage;

  // "ModeProgressMessage" event
  static event ProgressMessageHandler ^ ProgressMessage;

  //"UndoStackModified" event
  static event UndoStackModifiedHandler ^ UndoStackModified;

// event raising
public:

  // raises the "mode complete" event
  static void RaiseToolCompleted( System::String ^ mpName );

  // rasies the "progress percentage" event
  static void RaiseProgressPercentage( float Progress );

  // rasies the "progress message" event
  static void RaiseProgressMessage( System::String ^ mpProgressMessage );

  //Raises the "UndoStackModified" event
  static void RaiseUndoActionSignal();

}; // Events


CLOSE_VISUALIZATION_NS


// $Log: v3D_Events.h,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/08/28 21:45:36  gdavidson
// Changed ToolCompleted delegate
//
// Revision 1.2  2006/04/25 20:22:27  romy
// StackModifiedFuncPtr added
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2003/08/20 14:31:10  michael
// added "mailto:"
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
// Revision 1.7  2003/04/14 16:45:12  frank
// Separated progress percent and progress message interfaces
//
// Revision 1.6  2003/04/09 20:17:28  geconomos
// Updated progress bar to have status text.
//
// Revision 1.5  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.4  2003/02/11 23:40:49  frank
// spacing
//
// Revision 1.3.2.1  2003/02/12 23:01:35  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2003/01/09 19:04:20  geconomos
// Coding conventions.
//
// Revision 1.2  2002/11/27 18:50:36  geconomos
// Added progress for volume loading.
//
// Revision 1.1  2002/11/26 14:46:52  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Events.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Events.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
