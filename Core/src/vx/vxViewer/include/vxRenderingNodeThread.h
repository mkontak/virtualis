// $Id: vxRenderingNodeThread.h,v 1.3 2007/04/12 14:13:41 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#pragma once

// includes
#include "Thread.h"
#include "BoundingRect.h"


// forward declaration
class vxRenderingNodePlugin;
struct vxRenderingNodeThreadArgs;


/**
 * Provides the interface for a tile-based rendering thread.
 */
class VX_VIEWER_DLL vxRenderingNodeThread : public Thread
{
// functions
public:
  
  /// constructor from shared thread arguments
  vxRenderingNodeThread( vxRenderingNodeThreadArgs & args );

  /// destructor
  ~vxRenderingNodeThread();

  /// pulses the wait event to render a frame
  void StartRendering( int4 iInitialTileIndex );

  /// sets the kill event to exit the run loop
  void ExitRendering();

  /// waits for the window to be rendered
  void WaitForRenderingToComplete( DWORD dwTimeout = INFINITE );

  /// returns if the thread is suspended
  bool IsRendering() { return m_bIsRendering; }

  /// calls the rendering method with the given window
  void Run(); 

  /// set the thread Id
  void SetId( uint2 iId ) { m_uThreadID = iId; }
 
  /// return the id of the current thread.
  uint2 GetId() { return m_uThreadID; }

  /// gets the complete event
  const HANDLE GetCompleteEvent() const { return m_hCompleteEvent; }

  /// gets the render event
  const HANDLE GetRenderEvent() const { return m_hRenderEvent; }

private:

  /// private copy constructor
  vxRenderingNodeThread( const vxRenderingNodeThread & other );

  /// private assignment operator
  const vxRenderingNodeThread & operator=( const vxRenderingNodeThread & other );

  // creates the thread synchronization  objects
  void CreateEvents();

// member variables
private:

  /// thread arguments shared accros all threads
  vxRenderingNodeThreadArgs &  m_args;

  /// initial tile index to render
  int4 m_iInitialTileIndex;
  
  /// thread id
  uint2 m_uThreadID;

  /// handle for rendering
  HANDLE m_hRenderEvent;  

  /// handle for completing
  HANDLE m_hCompleteEvent;  

  /// handle for killing
  HANDLE m_hKillEvent;

  /// in rendering mode?
  bool m_bIsRendering;

}; // class vxRenderingNodeThread;


// $Log: vxRenderingNodeThread.h,v $
// Revision 1.3  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.2  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.3  2006/08/03 14:12:01  geconomos
// renamed vxRenderingEngineRenderer to vxRenderingNodePlugin
//
// Revision 1.2  2006/07/07 18:02:58  geconomos
// added properties for Complete and Render events
//
// Revision 1.1  2006/06/27 20:11:24  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingNodeThread.h,v 1.3 2007/04/12 14:13:41 geconomos Exp $
// $Id: vxRenderingNodeThread.h,v 1.3 2007/04/12 14:13:41 geconomos Exp $
