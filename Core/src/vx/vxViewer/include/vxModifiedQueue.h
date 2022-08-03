// $Id: vxModifiedQueue.h,v 1.9 2007/02/05 19:28:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)

/*
  Modified queue class keeping track of modified objects and cause
  a redraw on all modified clients upon flush()
*/

#ifndef vxModifiedQueue_h
#define vxModifiedQueue_h



// forward declaration
class vxModifiedInfo;
class vxModifiedSet;
class vxObserver;
class vxRenderingNode;
class vxControllerGlobal;
class vxVisualizationGraph;


// class definition
class VX_VIEWER_DLL vxModifiedQueue 
{ 
// functions
public:

  /// sends all of the modified messages to the sink clients
  static void Flush();

  /// appends the modified message to the others associated with this sink client
  static void Push(vxObserver * pClient, const vxModifiedInfo & info);

  /// removes all stored messages for the disconnected client
  static void Disconnected(vxObserver * pClient);

  /// returns a collection of modified messages
  static inline std::vector< vxModifiedSet * > & GetMessages() { return m_messages; }

  /// returns a collection of modified messages
  static inline std::vector< vxModifiedSet * > & GetBuffer() { return m_buffer; }

  /// clears all of the modified messages without forewarding to the sink clients
  static void Clear();

  /// gets the visualization graph
  static vxVisualizationGraph & GetVisualizationGraph() { return * m_pVisualizationGraph; }

  /// sets the visualization graph
  static void SetVisualizationGraph( vxVisualizationGraph * pVisualizationGraph ) { m_pVisualizationGraph = pVisualizationGraph; }

  //Sets the flush flag
  static void EnableCreatingMovies( bool bEnable ) { m_bCreatingMovies = bEnable; }
  
private:

  /// default constructor
  vxModifiedQueue() {};

// data
private:
  
  // contains modified messages and the sink client pointer
  static std::vector<vxModifiedSet *> m_messages;

  // buffers pushed messages if the queue is being flushed
  static std::vector<vxModifiedSet *> m_buffer;

  /// visualization graph
  static vxVisualizationGraph * m_pVisualizationGraph;

  /// Flag indicates whether to do flushing or not. Added to save VAS pipeline from crash
  static bool m_bCreatingMovies;


}; // vxModifiedQueue


#endif // vxModifiedQueue_h


// Revision History:
// $Log: vxModifiedQueue.h,v $
// Revision 1.9  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.8  2007/01/31 15:06:34  romy
// Added a method called EnableCreatingMovies
//
// Revision 1.7  2007/01/29 20:37:47  romy
// Added a flush flag to skip the GetVisualizationGraph Run Process. This fixes the VAS movie creation issue
//
// Revision 1.6  2007/01/08 17:03:40  gdavidson
// Refactored vxVisualizationGraph code
//
// Revision 1.5  2006/10/20 20:48:11  geconomos
// added support for global controllers
//
// Revision 1.4  2006/08/31 14:13:42  geconomos
// hacked in vxGlobalControllerDatasets
//
// Revision 1.3  2006/08/23 18:00:03  geconomos
// moved original Flush code to InternalFlush
//
// Revision 1.2  2006/08/03 14:17:36  geconomos
// wired in the rendering manager
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/09/25 12:55:20  michael
// made Clear() function private
//
// Revision 1.5  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.4  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.3  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.6  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.5  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.4  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.3  2002/05/10 21:14:36  michael
// replaced typedef
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxModifiedQueue.h,v 1.9 2007/02/05 19:28:40 geconomos Exp $
// $Id: vxModifiedQueue.h,v 1.9 2007/02/05 19:28:40 geconomos Exp $
