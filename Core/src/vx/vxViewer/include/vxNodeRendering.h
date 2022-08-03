// $Id: vxNodeRendering.h,v 1.2 2007/06/28 19:34:09 jmeade Exp $
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

// include declarations
#include "Triple.h"
#include "vxVisualizationGraphNode.h"
#include "vxRenderBuffer.h"
#include "Thread.h"

// forward declarations
class vxViewer;
class vxViewerType;
class vxHighQualityThread;
class vxRenderingNodeThreadManager;
class vxEnvironment;
class vxRenderingNodePlugin;

/**
 * Provides a framework that supports adaptive rendering, optimized threading and overall frame rate monitoring.
 */
class VX_VIEWER_DLL vxNodeRendering : public vxVisualizationGraphNode
{
// constants
public:

  // temporary registered redraw message
  static UINT VX_REDRAW;

// member functions
public:
  
  /// constructor
  vxNodeRendering();
  
  /// destructor
  virtual ~vxNodeRendering();

  /// initializes the rendering manager with the specified viewers
  void Initialize();

  /// runs the rendering manager
  void Run();

  /// resets the rendering manager to a default state
  void Reset();

  /// aborts the rendering node
  void Abort();
  
  /// renders the specified viewer
  bool RenderViewer( vxViewer * pViewer, const Triple< uint4 > & renderSize, vxRenderBuffer::BufferStatEnum eBufferState, const Thread::ThreadPriorityEnum & ePriority );

  /// redraws the specified viewer ( will move to drawing manager 
  void RedrawViewer( vxViewer * pViewer );

  /// gets the high quality thread enabled flag
  inline const bool & GetHighQualityThreadEnabled() const { return m_bHighQualityThreadEnabled; }

  /// sets the high quality thread enabled flag
  inline void SetHighQualityThreadEnabled( const bool & bEnabled ) { m_bHighQualityThreadEnabled = bEnabled; }

  /// gets the fixed size rendering enabled flag
  inline const bool & GetFixedSizeRenderingEnabled() const { return m_bFixedSizeRenderingEnabled; }

  /// sets the fixed size rendering enabled flag
  inline void SetFixedSizeRenderingEnabled( const bool & bEnabled ) { m_bFixedSizeRenderingEnabled = bEnabled; }

  /// gets the fixed rendering size
  inline const Triple< uint4 > & GetFixedRenderingSize() const { return m_fixedRenderingSize; }

  /// sets the fixed rendering size
  inline void SetFixedRenderingSize( const Triple< uint4 > & fixedRenderingSize ) { m_fixedRenderingSize = fixedRenderingSize; }
 
	/// gets the thread manager instance
	vxRenderingNodeThreadManager * GetThreadManager() { return m_pThreadManager; }

private:

  // struct declaration
  struct vxRenderingInfo
  {
    /// associated viewer
    vxViewer * m_pViewer;

    /// last viewer type used
    vxViewerType * m_pViewerType;

    /// time taken to render last image
    float4 m_fRenderTime;

    /// last render size
    Triple< uint4 > m_renderSize;

    /// indicates if rendering is necessary
    bool m_bNeedsRendering;

    /// indicates initial rendering
    bool m_bInitialRendering;

  }; // struct vxRenderingInfo

  /// adjusts the viewers render size
  Triple< uint4 > AdjustRenderSize( vxRenderingInfo * pInfo, float4 fMinFrameRate, float4 fMaxFrameRate );


// member variables
private:

  /// rendering info
  std::vector< vxRenderingInfo * > m_renderings;

  /// high quality image generation thread
  vxHighQualityThread * m_pHighQualityThread;

  /// previous value of "user interaction" from last run
  bool m_bPreviousUserInteractionFlag;

  /// rendering weights
  std::vector< float4 > m_renderingWieghts;
  
  /// is high quality thread enabled
  bool m_bHighQualityThreadEnabled;

  /// is fixed size rendering enabled
  bool m_bFixedSizeRenderingEnabled;

  /// last render size
  Triple< uint4 > m_fixedRenderingSize;

	/// rendering engine
  vxRenderingNodeThreadManager * m_pThreadManager;

}; // vxRenderingNode


// $Log: vxNodeRendering.h,v $
// Revision 1.2  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.1  2007/05/03 21:42:28  gdavidson
// Initial revision
//
// Revision 1.3  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.2  2007/02/05 20:17:20  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.14  2007/01/25 22:42:58  gdavidson
// Changed the parameters for the RenderViewer method
//
// Revision 1.13  2007/01/24 23:08:40  gdavidson
// Rolled back changes (Threading problem)
//
// Revision 1.12  2007/01/24 20:27:58  gdavidson
// Added a flag to track if it is the initial rendering
//
// Revision 1.11  2007/01/17 16:42:19  geconomos
// moved DrawStatistic to vxVisualizationGraph
//
// Revision 1.10  2007/01/08 17:05:12  gdavidson
// Refactored vxVisualizationGraph code
//
// Revision 1.9  2006/12/04 20:49:48  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.8  2006/10/04 14:35:55  geconomos
// Issue # 5033: Default the "last render time" to max_float to ensure a low quality image is generated when when the viewer type changes for given viewer
//
// Revision 1.7  2006/09/28 18:28:56  geconomos
// added method to dislpay rendering manager statistics given a vxEnvironment
//
// Revision 1.6  2006/09/27 16:08:14  geconomos
// added support for fixed size rendering
//
// Revision 1.5  2006/08/31 14:13:14  geconomos
// +moved vxRenderingInfo as nested class
// + added viewers accessor
//
// Revision 1.4  2006/08/28 21:03:47  geconomos
// added ability to disable the high quality image thread
//
// Revision 1.3  2006/08/28 14:02:35  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.2  2006/08/23 17:58:21  geconomos
// + initial implementation of high quality thread
// + refactored rendering and drawing code
//
// Revision 1.1  2006/08/03 13:58:20  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeRendering.h,v 1.2 2007/06/28 19:34:09 jmeade Exp $
// $Id: vxNodeRendering.h,v 1.2 2007/06/28 19:34:09 jmeade Exp $
