// $Id: vxHighQualityThread.h,v 1.6.2.2 2009/07/28 15:15:15 kchalupa Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


// include  declarations
#include "Thread.h"


// forward declarations
class vxNodeRendering;
class vxViewer;


/**
 * Generates high quality images in a background thread for rendering plugins that support adaptive rendering.
 */
class VX_VIEWER_DLL vxHighQualityThread : public Thread 
{   
// construction
public: 

  /// constructor from vxRenderingNode
  vxHighQualityThread( vxNodeRendering * pRenderingNode );

  /// destructor
  virtual ~vxHighQualityThread();

// member functions
public:

  /// determines if the thread is rendering
  inline bool IsRendering() const { return m_bIsRendering; }

  /// determines if the thread has unfinished viewers
  inline bool HasUnfinishedRendering() const { return m_viewers.size() > 0; }
  
  /// begins rendering of the specified viewers in a separate thread from the caller
  void BeginRendering( const std::vector< vxViewer * > & viewers );

  /// stops rendering
  void StopRendering();

  /// thread run function
  void Run(); 

  /// clears the viewers list
  void Reset() { m_viewers.clear(); }

// member variables
private:
  
  /// rendering manager
  vxNodeRendering * m_pRenderingNode;

  /// releases thread to render 
  HANDLE m_hRenderViewersEvent;
  
  /// signaled when thread has completed rendering viewers
  HANDLE m_hRenderingCompleteEvent;

  /// ends the thread run funtion
  HANDLE m_hAbortThreadEvent;

  /// viewers that need to be rendered
  std::vector< vxViewer * > m_viewers;

  /// index of viewer rendering in thread
  int4 m_iCurrentViewerIndex;

  /// abort rendering flag
  bool m_bAbortRendering;

  /// thread rendering flag
  bool m_bIsRendering;
  
}; // class HighQualityImageThread


// $Log: vxHighQualityThread.h,v $
// Revision 1.6.2.2  2009/07/28 15:15:15  kchalupa
// Code Walkthrough
//
// Revision 1.6.2.1  2009/03/06 16:12:20  gdavidson
// Corrected problem that prevented all viewers from reaching HQ
//
// Revision 1.6  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.5  2007/05/03 21:33:29  gdavidson
// Changed vxRenderingNode -> vxNodeRendering
//
// Revision 1.4.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.4.2.1  2007/03/22 21:33:13  gdavidson
// Issue #5505: Modified the render loop
//
// Revision 1.4  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.3  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.2  2006/12/12 20:50:01  romy
// interface Reset added
//
// Revision 1.1  2006/08/23 17:52:20  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxHighQualityThread.h,v 1.6.2.2 2009/07/28 15:15:15 kchalupa Exp $
// $Id: vxHighQualityThread.h,v 1.6.2.2 2009/07/28 15:15:15 kchalupa Exp $