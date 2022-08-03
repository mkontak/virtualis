// $Id: vxIncrementalSlider.C,v 1.2 2007/03/07 18:06:10 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxIncrementalSlider.h"


/**
 * Constructor
 */
vxIncrementalSlider::vxIncrementalSlider()
: m_bActive( false ),
m_uVoxelsPerStep( 300 )
{
} // vxIncrementalSlider()


/**
 * Initialize all the data based on the starting point
 *
 * @param anchorPoint the first mouse position
 */
void vxIncrementalSlider::Initialize( const Point2D<int2> & anchorPoint )
{
  m_anchorPoint = anchorPoint;
  SetCurrentMousePos( anchorPoint );
  m_bActive = true;
} // Initialize()


/**
 * Get the distance from the anchor point
 *
 * @return the distance from the anchor point
 */
const uint4 vxIncrementalSlider::GetNumberStepsAbs() const
{
  return ( m_iNumberSteps > 0 ) ? m_iNumberSteps : - m_iNumberSteps;
} // GetNumberStepsAbs()


/**
 * Set the current mouse position
 *
 * @param currentMousePosition the current mouse position
 */
void vxIncrementalSlider::SetCurrentMousePos( const Point2D<int2> & currentMousePosition )
{
  m_iNumberSteps = currentMousePosition.m_y - m_anchorPoint.m_y;
} // SetCurrentMousePos()


/**
 * Clear out the data
 */
void vxIncrementalSlider::Clear()
{
  m_bActive = false;
  m_uActiveVoxels = 0;
  m_uTotalVoxels = 0;
} // Clear()


/**
 * Get the desired number of voxels
 *
 * @return the desired number of voxels
 */
uint4 vxIncrementalSlider::GetNumberDesiredVoxels() const
{
  return GetNumberStepsAbs() * GetVoxelsPerStep();
} // GetNumberDesiredVoxels()


// $Log: vxIncrementalSlider.C,v $
// Revision 1.2  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
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
// Revision 1.2  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxIncrementalSlider.C,v 1.2 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxIncrementalSlider.C,v 1.2 2007/03/07 18:06:10 geconomos Exp $
