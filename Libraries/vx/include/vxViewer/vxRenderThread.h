// $Id: vxRenderThread.h,v 1.2 2004/10/20 21:50:23 michael Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderThread.h,v 1.2 2004/10/20 21:50:23 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * RenderThread for screen space parallelism
 */

#ifndef vxRenderThread_h
#define vxRenderThread_h


// includes
#include "Thread.h"
#include "BoundingRect.h"


// forward declaration
class vxRenderer;


// class definition
class VX_VIEWER_DLL vxRenderThread : public Thread
{
// functions
public:
  
  /// default constructor
  vxRenderThread();

  /// constructor
  vxRenderThread(vxRenderer * pRenderer, uint1 uThreadID);

  /// destructor
  ~vxRenderThread();

  /// set the renderer
  void SetRenderer(vxRenderer *pRenderer) { m_pRenderer = pRenderer; }

  /// set the thread Id
  void SetId(uint2 iId) { m_uThreadID = iId; }

  /// set the portion of the image to be drawn
  void SetWindow(BoundingRect<uint2> & uWindow) { m_uWindow = uWindow; }
  
  /// return the id of the current thread.
  uint2 GetId() { return m_uThreadID; }

  /// pulses the wait event to render a frame
  void StartRendering();

  /// sets the kill event to exit the run loop
  void ExitRendering();

  /// waits for the frame to be rendered
  void WaitForRenderingToComplete( DWORD dwTimeout = INFINITE );

  /// returns if the thread is suspended
  bool IsRendering() { return m_bRendering; }

  /// calls the rendering method with the given window
  void Run(); 

private:

  /// copy constructor
  vxRenderThread(const vxRenderThread & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRenderThread & operator=(const vxRenderThread & other); // should only be public if really implemented!

  // creates the thread synchronization  objects
  void CreateEvents();

// data
private:

  /// renderer to call
  vxRenderer * m_pRenderer;

  /// thread id
  uint1 m_uThreadID;

  /// bounding rectangle
  BoundingRect<uint2> m_uWindow;

  /// handle for rendering
  HANDLE m_hRenderEvent;  

  /// handle for completing
  HANDLE m_hCompleteEvent;  

  /// handle for killing
  HANDLE m_hKillEvent;

  /// in rendering mode?
  bool m_bRendering;
}; // class vxRenderThread;


#endif // vxRenderThread_h


// Revision History:
// $Log: vxRenderThread.h,v $
// Revision 1.2  2004/10/20 21:50:23  michael
// added VX_VIEWER_DLL to export classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.9  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.8  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.7  2003/05/14 16:10:20  michael
// code review
//
// Revision 1.6  2002/11/20 21:37:54  geconomos
// Added events for synchronization.
//
// Revision 1.5  2002/08/14 22:28:51  uday
// Added GetId().
//
// Revision 1.4  2002/08/09 15:40:48  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.3  2002/04/08 13:35:44  michael
// added missing privately non implemented copy constructor and assignment operator
//
// Revision 1.2  2002/03/29 23:48:25  ingmar
// first step to enableing multithreading in CPU raycaster
//
// Revision 1.1  2002/03/15 01:27:24  michael
// added simple threaded renderer class (initial version)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderThread.h,v 1.2 2004/10/20 21:50:23 michael Exp $
// $Id: vxRenderThread.h,v 1.2 2004/10/20 21:50:23 michael Exp $
