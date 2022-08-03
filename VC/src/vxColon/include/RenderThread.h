// $Id: RenderThread.h,v 1.3 2006/01/31 14:05:19 frank Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include "VolumeRender.h"


// namespaces
namespace ReaderLib
{

// forwards
class VolumeRender;


class RenderThread
{
// Methods
public:

  /// Constructor
  RenderThread(VolumeRender& rVolRender, Dataset & rDataset, const uint4 uId);

  /// Destructor
  ~RenderThread();

  /// Signals the start render event
  void StartRender();

  /// Waits for the render completed event to occur
  void WaitForRender();

  /// Check whether the thread created succesfully
  bool IsThreadValid();

  /// gets the rendering bounds
  const Rect< int4 > & GetRenderingBounds() const { return m_renderingBounds; }
  
  /// sets the rendering bounds
  void SetRenderingBounds( const Rect< int4 > & renderingBounds ) { m_renderingBounds = renderingBounds; }

protected:

  /// Function that performs rendering
  virtual void Render();

private:

  /// Thread function handles processing when a render when signalled
  static UINT _stdcall ThreadControlFunction(LPVOID pParam);

protected:
  /// Reference to a VolumeRender instance
  VolumeRender& m_rVolRender;

  /// Reference to dataset to be rendered
  Dataset& m_rDataset;

  /// Identifier for the thread, indicates/implies on which processor it will run
  const uint4 m_uID;

private:

  /// rendering bounds
  Rect< int4 > m_renderingBounds;

  /// Render completed wait event handle
  HANDLE m_hDoneRenderingEvent;

  /// Start render wait event handle
  HANDLE m_hStartRenderingEvent;

  /// End thread event
  HANDLE m_hExitThreadEvent;

  /// Thread
  HANDLE m_hThread;

}; // class RenderThread

} // namespace ReaderLib


// $Log: RenderThread.h,v $
// Revision 1.3  2006/01/31 14:05:19  frank
// code review
//
// Revision 1.2  2005/10/25 13:47:37  geconomos
// added rendering bounds
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.5.2.2  2003/04/09 16:17:19  jmeade
// Comments, coding standards.
//
// Revision 3.5.2.1  2003/04/04 20:13:26  kevin
// Issue 2835: The threads were crashing on creatiion. 2 things were
// done to fix this:
// (1) Handle rendierng if the threads are non-existant by switching
// to sequential rendering (this should have been in there nayway)
// (2) Create the threads early when there is plenty of memory blocks
//
// Revision 3.5  2003/01/29 19:36:48  jmeade
// Use an event instead of a flag to end the thread.
//
// Revision 3.4  2003/01/29 18:53:38  jmeade
// Removed code duplication in RenderThread and derivatives.
//
// Revision 3.3  2003/01/07 15:05:53  kevin
// Render threads switched to worker-threads rather than windows-threads
//
// Revision 3.2  2002/12/23 22:29:15  kevin
// Fixed the VERY VERY VERY BAD sleep-busy-wait cycle that
// significantly effected the rendering speed and system performance
// on the newer faster processors
//
// Revision 3.1  2002/04/12 02:13:44  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 08 2001 21:37:48   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/RenderThread.h,v 1.3 2006/01/31 14:05:19 frank Exp $
// $Id: RenderThread.h,v 1.3 2006/01/31 14:05:19 frank Exp $
