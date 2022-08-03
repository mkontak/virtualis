// $Id: SegmentViewer.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  SegmentViewer.h
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
inline int4 SegmentViewer::GetCurrentPlaneAngle() const
{ 
  return m_iCurrentPlaneAngle; 
} // GetSliceNumber()


/**
 * Set the number of the last slice for caching purposes.
 *
 * @param num the slice number to be set,
 */
inline void SegmentViewer::SetDesiredPlaneAngle(const int4 iAngle) 
{ 
  m_iDesiredPlaneAngle = iAngle; 
} // SetSliceNumber()

inline void SegmentViewer::ForceRecreateImage()
{
  m_iCurrentPlaneAngle = -1;
}



/**
 * switch between rendering of 'blocked' or 'smoothed' pixels.
 */
inline void SegmentViewer::SwitchBlockAndSmooth() 
{ 
  m_bBlockedPixel = ! m_bBlockedPixel; 
} // SwitchBlockAndSmooth()


/**
 * Get the zoom factor.
 * @return the zoom factor
 */
inline const int4 SegmentViewer::GetZoomPercentage() const
{
  return int(1);
} // GetZoomPercentage()


/**
 * Set dataset type
 * @param eOrientation  dataset orientation
 */
inline void SegmentViewer::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_eDatasetType = eOrientation;
} // SetDatasetType()


inline DatasetOrientations::TypeEnum SegmentViewer::GetDatasetType() const
{
  return GetDataset()->GetOrientation();
}


// $Log: SegmentViewer.inl,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1  2002/08/15 15:53:53  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SegmentViewer.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: SegmentViewer.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
