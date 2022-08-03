// $Id: vxIncrementalSlider.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <list>
#include <vector>
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeRegionGrowIteratorHeap.h"
#include "vxDecider.h"
#include "Image.h"
#include "vxViewerType.h"


/**
 * Class to hold data for visualizing the intermediate results of incremental growth.
 */
class VX_VIEWER_DLL vxIncrementalSlider
{
// Member Functions
public:

  /// Constructor
  vxIncrementalSlider();

  /// Initialize all the data based on the starting point
  void Initialize( const Point2D<int2> & anchorPoint );

  /// Get the anchor points
  const Point2D<int2> & GetAnchorPoint() const { return m_anchorPoint; }

  /// Get the number of steps desired either positive or negative (how far up or down the mouse is from the start point)
  const int4 & GetNumberSteps() const { return m_iNumberSteps; }

  /// Get the distance from the anchor point
  const uint4 GetNumberStepsAbs() const;

  /// Set the current mouse position
  void SetCurrentMousePos( const Point2D<int2> & currentMousePosition );

  /// If the slider is currently active and should be drawn
  const bool & GetActive() const { return m_bActive; }

  /// Set whether the slider is currently active and should be drawn
  void SetActive( const bool & bActive ) { m_bActive = bActive; }

  /// Clear out the data
  void Clear();

  /// Get the desired number of voxels
  uint4 GetNumberDesiredVoxels() const;

  /// The the number of voxels in each step
  uint4 GetVoxelsPerStep() const { return m_uVoxelsPerStep; }

  /// The the number of voxels in each step
  void SetVoxelsPerStep( const uint4 uVoxelsPerStep ) { m_uVoxelsPerStep = uVoxelsPerStep; }

// Data Members
private:

  /// The mouse anchor point
  Point2D<int2> m_anchorPoint;

  /// The number of steps
  int4 m_iNumberSteps;

  /// If the slider is active
  bool m_bActive;

  /// The number of active voxels
  uint4 m_uActiveVoxels;

  /// The total number of voxels
  uint4 m_uTotalVoxels;

  /// The number of voxels to segment per step (a step is one pixel on the screen)
  uint4 m_uVoxelsPerStep;

}; // class vxIncrementalSlider


// $Log: vxIncrementalSlider.h,v $
// Revision 1.2  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:33  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 14:59:02  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.2  2003/04/28 19:09:48  frank
// code review
//
// Revision 1.1  2003/04/18 16:00:20  frank
// Moved vxIncrementalSlider to its own file.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIncrementalSlider.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxIncrementalSlider.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $