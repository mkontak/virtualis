// $Id: vxNodeDatasetsCyclerThread.h,v 1.1 2007/06/22 15:15:24 geconomos Exp $
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
class vxNodeDatasetsCycler;
class vxViewerCache;


/**
 * Handles the generation of high quality images for dataset cycling.  (Low quality 
 * images are generated in real time)
 */
class VX_VIEWER_DLL vxNodeDatasetsCyclerThread : public Thread 
{   
// member functions
public: 

  /// constructor from vxControllerGlobalDatasets
  vxNodeDatasetsCyclerThread();

  /// destructor
  virtual ~vxNodeDatasetsCyclerThread();

  /// gets the rendering node
  inline vxNodeRendering * GetRenderingNode() { return m_pNodeRendering; }

  /// sets the rendering node
  inline void SetRenderingNode( vxNodeRendering * pRenderingNode ) { m_pNodeRendering = pRenderingNode; }
  
  /// gets the rendering node
  inline vxNodeDatasetsCycler * GetDatasetsNode() { return m_pNodeDatasets; }

  /// sets the rendering node
  inline void SetDatasetsNode( vxNodeDatasetsCycler * pDatasetsNode ) { m_pNodeDatasets = pDatasetsNode; }

  /// begins rendering of the specified viewers in a separate thread from the caller
  void BeginRendering();

  /// stops rendering
  void StopRendering();

  /// thread run function
  void Run(); 

  /// indicates if the datasets thread is rendering
  const bool & IsRendering() const { return m_bIsRendering; }

// member variables
private:
  
  /// rendering manager
  vxNodeRendering * m_pNodeRendering;

  /// dataset cycler node
  vxNodeDatasetsCycler * m_pNodeDatasets;

  /// releases thread to render viewers
  HANDLE m_hRenderViewersEvent;
  
  /// signaled when thread has completed rendering viewers
  HANDLE m_hRenderingCompleteEvent;

  /// ends the thread run funtion
  HANDLE m_hAbortThreadEvent;

  /// abort rendering flag
  bool m_bAbortRendering;

  /// thread rendering flag
  bool m_bIsRendering;

  /// collection of render buffers
  std::vector< vxViewerCache * > * m_pCache;


}; // class vxNodeDatasetsCycler


// $Log: vxNodeDatasetsCyclerThread.h,v $
// Revision 1.1  2007/06/22 15:15:24  geconomos
// - renamed vxDatasetsThread => vxNodeDatasetsCyclerThread
// - reverted to RSNA version
//
// Revision 1.6  2007/05/03 21:30:31  gdavidson
// Refactored vxVisualizationGraph
//
// Revision 1.5  2007/04/02 21:29:59  gdavidson
// Modified switching of the Datasets
//
// Revision 1.4  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.3  2006/12/04 20:33:17  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.2  2006/09/20 14:39:12  geconomos
// added active rendering engine
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeDatasetsCyclerThread.h,v 1.1 2007/06/22 15:15:24 geconomos Exp $
// $Id: vxNodeDatasetsCyclerThread.h,v 1.1 2007/06/22 15:15:24 geconomos Exp $