// $Id: Segment2DViewer.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  Segment2DViewer.h
// Description: A class to support viewing of segments along the centerline
// Created: June 10, 2002
// Author: Kevin Kreeger  (kevin@viatronix.net)
//
// Complete History at bottom of file.
//


/**
 * Get the number of the last slice for caching purposes.
 *
 * @return the number of slices.
 */
inline int4 Segment2DViewer::GetCurrentPlaneAngle() const
{ 
  return m_iCurrentPlaneAngle; 
} // GetSliceNumber()


/**
 * Set the number of the last slice for caching purposes.
 *
 * @param num the slice number to be set,
 */
inline void Segment2DViewer::SetDesiredPlaneAngle(const int4 iAngle) 
{ 
  m_iDesiredPlaneAngle = iAngle; 
} // SetSliceNumber()

inline void Segment2DViewer::ResetAngle()
{
  m_iDesiredPlaneAngle = 0;
}



/**
 * switch between rendering of 'blocked' or 'smoothed' pixels.
 */
inline void Segment2DViewer::SwitchBlockAndSmooth() 
{ 
  m_bBlockedPixel = ! m_bBlockedPixel; 
} // SwitchBlockAndSmooth()


/**
 * Get the zoom factor.
 * @return the zoom factor
 */
inline const int4 Segment2DViewer::GetZoomPercentage() const
{
  return int(1);
} // GetZoomPercentage()


/**
 * Set dataset type
 * @param eOrientation  dataset orientation
 */
inline void Segment2DViewer::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_eDatasetType = eOrientation;
} // SetDatasetType()


inline DatasetOrientations::TypeEnum Segment2DViewer::GetDatasetType() const
{
  return GetDataset()->GetOrientation();
}


// $Log: Segment2DViewer.inl,v $
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Segment2DViewer.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: Segment2DViewer.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
