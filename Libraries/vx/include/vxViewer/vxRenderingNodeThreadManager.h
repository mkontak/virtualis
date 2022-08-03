// $Id: vxRenderingNodeThreadManager.h,v 1.5 2007/06/28 19:34:09 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

// pragama declarations
#pragma once

// include declarations
#include "vxRenderingNodeThread.h"
#include "vxRenderingNodePlugin.h"
#include "vxRenderingNodeThreadArgs.h"


// forward declarations
class vxRenderingNodeThread;
class vxRenderingEngineRenderer;
class vxRenderBuffer;


/**
 * Manages all the rendering threads for the rendering node.
 */
class VX_VIEWER_DLL vxRenderingNodeThreadManager
{
// typedefs and enums
public:

  // typedef'd vxRenderingNodeThread * container
  typedef std::vector< vxRenderingNodeThread * > vxRenderingNodeThreads;
 
 
// member functions
public:

  /// constructor
  vxRenderingNodeThreadManager();

  /// destructor
  virtual ~vxRenderingNodeThreadManager();
  
  /// performs necessary initialization
  void Initialize();

  /// renders and image into the active buffer
  void Render( vxRenderingNodePlugin * pPlugin );
  
  /// renders and image into the active buffer and blocks until finished
  bool RenderAndWait( vxRenderingNodePlugin * pPlugin, const Thread::ThreadPriorityEnum & ePriority );

  /// aborts rendering
  void AbortRendering();

  /// aborts rendering and blocks until complete
  void AbortRenderingAndWait( );
  
  /// indicates if rendering
  bool IsRendering() const;

  /// sets the threads priorities
  void SetThreadPriority( const Thread::ThreadPriorityEnum ePriority );

  /// gets the number of threads 
  // TODO: changed to GetAvailableNumberOfThreads
  inline const uint4 GetNumberOfThreads() const { return m_threads.size(); }

  /// gets the number of available rrender threads
  inline const int4 GetNumberOfRenderThreads() const { return m_iNumberOfRenderThreads; }

  /// gets the number of available rrender threads
  inline void SetNumberOfRenderThreads( int4 iThreadCount ) { m_iNumberOfRenderThreads = iThreadCount; }

  /// pauses the threads
  void Pause();

  /// resumes the threads
  void Resume();

  /// gets the render size
  inline const Triple< uint4 > & GetRenderSize() const { return m_renderSize; }

  /// sets the render size
  inline void SetRenderSize( const Triple< uint4 > & renderSize ){ m_renderSize = renderSize; }
  
private:

  /// private copy constructor
  vxRenderingNodeThreadManager( const vxRenderingNodeThreadManager & other );

  /// private assignment operator  
  vxRenderingNodeThreadManager & operator=( const vxRenderingNodeThreadManager & rhs );
  
  /// blocks until finished
  void Wait();
  
// member variables  
private:
 
  // render threads 
  vxRenderingNodeThreads m_threads;

  /// shared thread arguments
  vxRenderingNodeThreadArgs m_threadArgs;

  // keeps the kurrent rendering plugin
  vxRenderingNodePlugin * m_pCurrentPlugin;

  /// cache of "rendering completed" handles for the engine threads
  std::vector< HANDLE > m_renderingCompleteHandles;

  /// indicates if rendering should be aborted
  bool m_bAbortRendering;

	/// renderer size
  Triple< uint4 > m_renderSize;

  /// cap on number of render threads
  int4 m_iNumberOfRenderThreads;

}; // class vxRenderThreadModel


// $Log: vxRenderingNodeThreadManager.h,v $
// Revision 1.5  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.4  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.3  2007/03/22 18:25:46  gdavidson
// Added GetNumberOfRenderThreads
//
// Revision 1.2.2.1  2007/04/10 14:47:01  geconomos
// added mechanism for setting the rendering threads priorities
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
// Revision 1.8  2006/12/04 20:47:53  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.7  2006/08/28 14:02:06  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.6  2006/08/23 17:54:23  geconomos
// + moved MaintainFramerate() to vxRenderingNode
// + added plugin data synchronization methods
// + changed RenderAndWait() to return if it was aborted
//
// Revision 1.5  2006/08/03 14:12:22  geconomos
// renamed vxRenderingEngineRenderer to vxRenderingNodePlugin
//
// Revision 1.4  2006/07/21 15:07:33  geconomos
// initial support for planetracer added
//
// Revision 1.3  2006/07/07 18:02:33  geconomos
// + modified Wait class method to use WaitForMultipleObjects function instead of multiple WaitForSingleObjects
// + added NumberOfThreads property
//
// Revision 1.2  2006/07/07 13:41:50  geconomos
// added vxRenderingEngineSettings and vxRenderingEngineBuffers class variables
//
// Revision 1.1  2006/06/27 20:11:24  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingNodeThreadManager.h,v 1.5 2007/06/28 19:34:09 jmeade Exp $
// $Id: vxRenderingNodeThreadManager.h,v 1.5 2007/06/28 19:34:09 jmeade Exp $
