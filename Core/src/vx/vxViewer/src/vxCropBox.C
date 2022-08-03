// $Id: vxCropBox.C,v 1.7 2007/07/20 19:56:57 cqian Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "StdAfx.h"
#include "vxCropBox.h"
#include "Matrix.h"
#include "vxBlockVolume.h"
#include "vxEnvironment.h"
#include "vxUtils.h"


// Defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Default constructor.
 */
vxCropBox::vxCropBox()
{
  m_iActiveHandle = -1;
} // vxCropBox()


/**
 * Destructor.
 */
vxCropBox::~vxCropBox()
{
} // ~vxCropBox()


/**
 * Create a crop box
 *
 * @param fMeasurePoint position in 3D where the mouse was clicked
 * @param majorAxis     major axis of the bos of interest
 */
void vxCropBox::CreateCropBox( const Point<float4> & measurePoint, const Vector<float4> & majorAxis )
{  
  Point<float4> minVolumePoint( measurePoint - majorAxis );
  Point<float4> maxVolumePoint( measurePoint + majorAxis );
  m_box = Box<float4>( minVolumePoint, maxVolumePoint );
} // CreateCropBox()


/**
 * Get number of handle points
 *
 * @param eAxis the orientation
 * @return number of points.
 */
uint4 vxCropBox::GetNumHandles() const
{

  uint4 uNumHandles( 9 );
  return uNumHandles;

} // GetNumHandles()


/**
 * Retrieve a handle location
 *
 * @param uIndex the desired handle index
 * @param eAxis  the orientation
 * @return the desired position
 */
Point<float4> vxCropBox::GetHandlePosition( uint2 uIndex ) const
{

  const uint4 uAxisIndex( GetEnvironment()->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );

  const int4 iPlaneOrthoDistance( vxUtils::CalculateOffsetDistance( GetEnvironment()->GetPlaneOrtho()->GetDistance() / GetEnvironment()->GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] + 0.5F,
    GetEnvironment()->GetIntensityVolume()->GetHeader().GetVolDim()[ uAxisIndex ],
    GetEnvironment()->GetOffset(),
    GetEnvironment()->GetMaximumOffset() ) );

  float4 fOrthoDistanceWorld( iPlaneOrthoDistance * GetEnvironment()->GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] );

  Point<float4> output;
  switch ( uAxisIndex )
  {

  case vxUtils::SAGITTAL:
    switch ( uIndex )
    {
    case 0: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMinY(), m_box.GetMinZ() ); break;
    case 1: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMidY(), m_box.GetMinZ() ); break;
    case 2: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMaxY(), m_box.GetMinZ() ); break;
    case 3: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMinY(), m_box.GetMidZ() ); break;
    case 4: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMidY(), m_box.GetMidZ() ); break;
    case 5: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMaxY(), m_box.GetMidZ() ); break;
    case 6: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMinY(), m_box.GetMaxZ() ); break;
    case 7: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMidY(), m_box.GetMaxZ() ); break;
    case 8: output = Point<float4>( fOrthoDistanceWorld, m_box.GetMaxY(), m_box.GetMaxZ() ); break;
    }
    break;

  case vxUtils::CORONAL:
    switch ( uIndex )
    {
    case 0: output = Point<float4>( m_box.GetMinX(), fOrthoDistanceWorld, m_box.GetMinZ() ); break;
    case 1: output = Point<float4>( m_box.GetMidX(), fOrthoDistanceWorld, m_box.GetMinZ() ); break;
    case 2: output = Point<float4>( m_box.GetMaxX(), fOrthoDistanceWorld, m_box.GetMinZ() ); break;
    case 3: output = Point<float4>( m_box.GetMinX(), fOrthoDistanceWorld, m_box.GetMidZ() ); break;
    case 4: output = Point<float4>( m_box.GetMidX(), fOrthoDistanceWorld, m_box.GetMidZ() ); break;
    case 5: output = Point<float4>( m_box.GetMaxX(), fOrthoDistanceWorld, m_box.GetMidZ() ); break;
    case 6: output = Point<float4>( m_box.GetMinX(), fOrthoDistanceWorld, m_box.GetMaxZ() ); break;
    case 7: output = Point<float4>( m_box.GetMidX(), fOrthoDistanceWorld, m_box.GetMaxZ() ); break;
    case 8: output = Point<float4>( m_box.GetMaxX(), fOrthoDistanceWorld, m_box.GetMaxZ() ); break;
    }
    break;

  case vxUtils::AXIAL:
    switch ( uIndex )
    {
    case 0: output = Point<float4>( m_box.GetMinX(), m_box.GetMinY(), fOrthoDistanceWorld ); break;
    case 1: output = Point<float4>( m_box.GetMidX(), m_box.GetMinY(), fOrthoDistanceWorld ); break;
    case 2: output = Point<float4>( m_box.GetMaxX(), m_box.GetMinY(), fOrthoDistanceWorld ); break;
    case 3: output = Point<float4>( m_box.GetMinX(), m_box.GetMidY(), fOrthoDistanceWorld ); break;
    case 4: output = Point<float4>( m_box.GetMidX(), m_box.GetMidY(), fOrthoDistanceWorld ); break;
    case 5: output = Point<float4>( m_box.GetMaxX(), m_box.GetMidY(), fOrthoDistanceWorld ); break;
    case 6: output = Point<float4>( m_box.GetMinX(), m_box.GetMaxY(), fOrthoDistanceWorld ); break;
    case 7: output = Point<float4>( m_box.GetMidX(), m_box.GetMaxY(), fOrthoDistanceWorld ); break;
    case 8: output = Point<float4>( m_box.GetMaxX(), m_box.GetMaxY(), fOrthoDistanceWorld ); break;
    }
    break;

  } // switch ( uAxisIndex )

  return output;

} // GetHandlePosition()


/**
 * Move the selected handle points
 *
 * @param newMeasurePoint the mouse point in 3D
 * @param iHandleIndex    the handle index that is currently being edited
 * @param uAxisIndex      the orientation of the box
 * @param fWorldBox       the bounding box of the volume in world coordinates
 */
void vxCropBox::SetHandlePosition( const Point<float4> & newMeasurePoint, int2 iHandleIndex,
                                   const uint4 uAxisIndex, Box<float4> fWorldBox )
{

  bool bResult( true );
  
  // getting the end points of the major and minor axes.
  Point<float4> centerPoint( m_box.GetMidPoint() );
  Vector<float4> changeVector( newMeasurePoint - centerPoint );
  
  switch ( uAxisIndex )
  {
  // x axis
  case 0:
  {
    switch ( iHandleIndex )
    {
    case -1:
      bResult = false;
      break;
    case 0:
      // minY, minZ point
      m_box = Box<float4>( Point<float4>(m_box.GetMinX(), newMeasurePoint.m_y, newMeasurePoint.m_z), m_box.GetMaxPoint() );
      break;
    case 1:
      // midY, minZ point
      m_box = Box<float4>( Point<float4>(m_box.GetMinX(), m_box.GetMinY(), newMeasurePoint.m_z), m_box.GetMaxPoint() );
      break;
    case 2:
      // maxY, minZ point
      m_box = Box<float4>( Point<float4>(m_box.GetMinX(), m_box.GetMinY(), newMeasurePoint.m_z ), Point<float4>( m_box.GetMaxX(), newMeasurePoint.m_y, m_box.GetMaxZ() ) );
      break;
    case 3:
      // minY, midZ point
      m_box = Box<float4>( Point<float4>(m_box.GetMinX(), newMeasurePoint.m_y, m_box.GetMinZ()), m_box.GetMaxPoint() );
      break;
    case 4:
      // moving the center handle
      m_box = Box<float4>( m_box.GetMinX(), m_box.GetMinY() + changeVector.m_y, m_box.GetMinZ() + changeVector.m_z,
                            m_box.GetMaxX(), m_box.GetMaxY() + changeVector.m_y, m_box.GetMaxZ() + changeVector.m_z );
      break;
    case 5:
      // maxY, midZ point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( m_box.GetMaxX(), newMeasurePoint.m_y, m_box.GetMaxZ() ) );
      break;
    case 6:
      // minY, maxZ point
      m_box = Box<float4>( Point<float4>( m_box.GetMinX(), newMeasurePoint.m_y, m_box.GetMinZ() ), Point<float4>( m_box.GetMaxX(), m_box.GetMaxY(), newMeasurePoint.m_z ) );
      break;
    case 7:
      // midY, maxZ point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( m_box.GetMaxX(), m_box.GetMaxY(), newMeasurePoint.m_z ) );
      break;
    case 8:
      // maxY, maxZ point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( m_box.GetMaxX(), newMeasurePoint.m_y, newMeasurePoint.m_z ) );
      break;
    default:
      bResult = false;
      break;
    }
  } // case 0
  break;

  // y axis
  case 1:
  {
    switch ( iHandleIndex )
    {
    case -1:
      bResult = false;
      break;
    case 0:
      // minX, minZ point
      m_box = Box<float4>( Point<float4>( newMeasurePoint.m_x, m_box.GetMinY(), newMeasurePoint.m_z ), m_box.GetMaxPoint() );
      break;
    case 1:
      // midX, minZ point
      m_box = Box<float4>( Point<float4>(m_box.GetMinX(), m_box.GetMinY(), newMeasurePoint.m_z ), m_box.GetMaxPoint() );
      break;
    case 2:
      // maxX, minZ point
      m_box = Box<float4>( Point<float4>( m_box.GetMinX(), m_box.GetMinY(), newMeasurePoint.m_z ), Point<float4>( newMeasurePoint.m_x, m_box.GetMaxY(), m_box.GetMaxZ() ) );
      break;
    case 3:
      // minX, midZ point
      m_box = Box<float4>( Point<float4>(newMeasurePoint.m_x, m_box.GetMinY(), m_box.GetMinZ() ), m_box.GetMaxPoint() );
      break;
    case 4:
      // moving the center handle.
      m_box = Box<float4>( m_box.GetMinX() + changeVector.m_x, m_box.GetMinY(), m_box.GetMinZ() + changeVector.m_z,
                            m_box.GetMaxX() + changeVector.m_x, m_box.GetMaxY(), m_box.GetMaxZ() + changeVector.m_z );
      break;
    case 5:
      // maxX, midZ point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( newMeasurePoint.m_x, m_box.GetMaxY(), m_box.GetMaxZ() ) );
      break;
    case 6:
      // minX, maxZ point
      m_box = Box<float4>( Point<float4>(newMeasurePoint.m_x, m_box.GetMinY(), m_box.GetMinZ() ), Point<float4>( m_box.GetMaxX(), m_box.GetMaxY(), newMeasurePoint.m_z ) );
      break;
    case 7:
      // midX, maxZ point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( m_box.GetMaxX(), m_box.GetMaxY(), newMeasurePoint.m_z ) );
      break;
    case 8:
      // maxX, maxZ point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( newMeasurePoint.m_x, m_box.GetMaxY(), newMeasurePoint.m_z ) );
      break;
    default:
      bResult = false;
      break;
    }
  } // case 1
  break;

  // z axis
  case 2:
  {
    switch ( iHandleIndex )
    {
    case -1:
      bResult = false;
      break;
    case 0:
      // minX, minY point
      m_box = Box<float4>( Point<float4>( newMeasurePoint.m_x, newMeasurePoint.m_y, m_box.GetMinZ() ), m_box.GetMaxPoint() );
      break;
    case 1:
      // midX, minY point
      m_box = Box<float4>( Point<float4>( m_box.GetMinX(), newMeasurePoint.m_y, m_box.GetMinZ() ), m_box.GetMaxPoint() );
      break;
    case 2:
      // maxX, minY point
      m_box = Box<float4>( Point<float4>( m_box.GetMinX(), newMeasurePoint.m_y, m_box.GetMinZ() ),
                            Point<float4>( newMeasurePoint.m_x, m_box.GetMaxY(), m_box.GetMaxZ() ) );
      break;
    case 3:
      // minX, midY point
      m_box = Box<float4>( Point<float4>( newMeasurePoint.m_x, m_box.GetMinY(), m_box.GetMinZ() ), m_box.GetMaxPoint() );
      break;
    case 4:
      // moving the center handle.
      m_box = Box<float4>( m_box.GetMinX() + changeVector.m_x, m_box.GetMinY() + changeVector.m_y, m_box.GetMinZ(),
                            m_box.GetMaxX() + changeVector.m_x, m_box.GetMaxY() + changeVector.m_y, m_box.GetMaxZ());
      break;
    case 5:
      // maxX, midY point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( newMeasurePoint.m_x, m_box.GetMaxY(), m_box.GetMaxZ()));
      break;
    case 6:
      // minX, maxY point
      m_box = Box<float4>( Point<float4>( newMeasurePoint.m_x, m_box.GetMinY(), m_box.GetMinZ() ),
                            Point<float4>( m_box.GetMaxX(), newMeasurePoint.m_y, m_box.GetMaxZ() ) );
      break;
    case 7:
      // midX, maxY point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( m_box.GetMaxX(), newMeasurePoint.m_y, m_box.GetMaxZ() ) );
      break;
    case 8:
      // maxX, maxY point
      m_box = Box<float4>( m_box.GetMinPoint(), Point<float4>( newMeasurePoint.m_x, newMeasurePoint.m_y, m_box.GetMaxZ() ) );
      break;
    default:
      bResult = false;
      break;
    }
  } // case 0
  break;

  default:
    bResult = false;
    break;
  }

  // Clamp the crop box to the volume dimensions
  m_box.BoundWithBox( fWorldBox );

} // SetHandlePosition()


/**
 * get the indexed handle
 *
 * @param uHandleIndex the index of the desired handle
 *
 * @return the desired handle
 */
vxHandle vxCropBox::GetHandle( const uint2 uHandleIndex ) const
{

  vxHandle handle;
  handle.SetPosition( GetHandlePosition( uHandleIndex ) );
  return handle;

} // GetHandle()


/**
 * set the indexed handle
 *
 * @param uHandleIndex the index of the desired handle
 * @param handle       the new handle
 */
void vxCropBox::SetHandle( const uint2 uHandleIndex, const vxHandle & handle )
{
} // SetHandle()


/**
 * Return the crop rectangle limits in a particular view
 *
 * @param iAxis the orientation
 *
 * @return the crop rectangle
 */
const BoundingRect<float4> vxCropBox::GetCropRect( const uint4 uAxisIndex )
{
  BoundingRect<float4> fRect( 0, 0, 512, 512 );

  switch ( uAxisIndex )
  {
  case 0:
    fRect.Set( m_box.GetMinY(), m_box.GetMinZ(), m_box.GetMaxY(), m_box.GetMaxZ() );
    break;

  case 1:
    fRect.Set( m_box.GetMinZ(), m_box.GetMinX(), m_box.GetMaxZ(), m_box.GetMaxX() );
    break;

  case 2:
    fRect.Set( m_box.GetMinX(), m_box.GetMinY(), m_box.GetMaxX(), m_box.GetMaxY() );
    break;

  default:
    throw ex::IllegalArgumentException( LogRec( "Bad axis index: " + ToAscii( int4( uAxisIndex ) ), "vxCropBox", "GetCropRect" ) );
    break;
  }

  return fRect;

} // GetCropRect()


#undef FILE_REVISION


// $Log: vxCropBox.C,v $
// Revision 1.7  2007/07/20 19:56:57  cqian
// Check in for testing
//
// Revision 1.6  2007/03/22 01:07:54  geconomos
// removed incorrect "inline" fom GetCropRect
//
// Revision 1.5  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/05/16 13:48:38  frank
// made the crop box easier to handle
//
// Revision 1.3  2004/04/06 16:24:17  frank
// unified environment access naming
//
// Revision 1.2  2004/03/11 17:36:54  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/06/03 19:28:14  michael
// variable naming
//
// Revision 1.7  2003/05/22 14:57:30  dongqing
// code review
//
// Revision 1.6  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.5  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.4  2002/09/24 15:33:55  frank
// Clarified variable names.
//
// Revision 1.3  2002/08/09 20:54:25  uday
// GetHandlePoint() takes the volume dimensions into account.
//
// Revision 1.2  2002/08/08 15:52:19  uday
// Removed display m_longAxis.
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/06/11 13:06:31  manju
// Modified the GetNoHandlePoints method.
//
// Revision 1.1  2002/06/10 15:13:04  manju
// Renamed AnnotationBOI as vxCropBox and moved it from 51_annotation to
// 74_vxUtils.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCropBox.C,v 1.7 2007/07/20 19:56:57 cqian Exp $
// $Id: vxCropBox.C,v 1.7 2007/07/20 19:56:57 cqian Exp $
