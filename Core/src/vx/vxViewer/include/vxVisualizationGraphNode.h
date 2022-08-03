// $Id: vxVisualizationGraphNode.h,v 1.2 2007/05/03 21:30:07 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once


// forward declarations
class vxVisualizationGraph;


class VX_VIEWER_DLL vxVisualizationGraphNode
{
// functions
public:

  /// constructor
  vxVisualizationGraphNode();

  /// destructor
  virtual ~vxVisualizationGraphNode() {}

  /// gets a reference to the parent graph
  inline vxVisualizationGraph * GetVisualizationGraph() { return m_pVisualizationGraph; }

  /// gets a reference to the parent graph
  inline const vxVisualizationGraph * GetVisualizationGraph() const { return m_pVisualizationGraph; }

  /// set the visualization graph
  inline void SetVisualizationGraph( vxVisualizationGraph * pVisualizationGraph ) { m_pVisualizationGraph = pVisualizationGraph; }

  /// does the node requires a flush
  virtual inline bool IsFlushNecessary() const { return false; }

  /// initializes the graph node
  virtual void Initialize() {}; 

  /// abort the node
  virtual void Abort() {}

  /// resets the node
  virtual void Reset() {};

  /// executes the node
  virtual void Run() = 0;

// data
private:

  /// reference to the parent graph
  vxVisualizationGraph * m_pVisualizationGraph;

}; // class vxVisualizationGraphNode


// $Log: vxVisualizationGraphNode.h,v $
// Revision 1.2  2007/05/03 21:30:07  gdavidson
// Refactored vxVisualizationGraph
//
// Revision 1.1  2007/01/08 17:02:35  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVisualizationGraphNode.h,v 1.2 2007/05/03 21:30:07 gdavidson Exp $
// $Id: vxVisualizationGraphNode.h,v 1.2 2007/05/03 21:30:07 gdavidson Exp $
