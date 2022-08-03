// $Id: Segment3DViewer.inl,v 1.3 2006/10/09 19:15:55 frank Exp $
//
// Copyright © 2002-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger  (kevin@viatronix.net)


// A class to support viewing of segments along the centerline


/**
 * Set the number of the last slice for caching purposes.
 *
 * @param num the slice number to be set,
 */
inline void Segment3DViewer::SetDesiredPlaneAngle(const int4 iAngle) 
{
  if ( m_iDesiredPlaneAngle != iAngle )
    m_bDirtyImage = true;

  m_iDesiredPlaneAngle = iAngle;
} // SetSliceNumber()


/**
 * switch between rendering of 'blocked' or 'smoothed' pixels.
 */
inline void Segment3DViewer::SwitchBlockAndSmooth() 
{ 
  m_bBlockedPixel = ! m_bBlockedPixel;
} // SwitchBlockAndSmooth()


/**
 * Set dataset type
 * @param eOrientation  dataset orientation
 */
inline void Segment3DViewer::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  if ( m_eDatasetType != eOrientation )
    m_bDirtyImage = true;

  m_eDatasetType = eOrientation;
} // SetDatasetType()


inline DatasetOrientations::TypeEnum Segment3DViewer::GetDatasetType() const
{
  return GetDataset()->GetOrientation();
}


// $Log: Segment3DViewer.inl,v $
// Revision 1.3  2006/10/09 19:15:55  frank
// Issue #5048: Fixed scrollbar behavior of UD segments view
//
// Revision 1.2  2006/09/28 15:59:42  frank
// added image inversion (for taking snapshots)
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.3  2002/11/11 14:32:38  kevin
// Dim the border for segmetnal view
//
// Revision 1.2  2002/08/28 19:21:40  kevin
// Lots of updates to segmental viewing
//
// Revision 1.1  2002/08/20 19:21:50  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:53:53  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Segment3DViewer.inl,v 1.3 2006/10/09 19:15:55 frank Exp $
// $Id: Segment3DViewer.inl,v 1.3 2006/10/09 19:15:55 frank Exp $
