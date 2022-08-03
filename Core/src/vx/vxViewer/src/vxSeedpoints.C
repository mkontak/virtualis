// $Id: vxSeedpoints.C,v 1.3 2007/10/03 16:14:13 cqian Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxSeedpoints.h"
#include "CrossPlane.h"
#include "vxBlockVolumeIterator.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * default constructor 
 */
vxSeedpoints::SelectionData::SelectionData()
: m_seedPoint( -1, -1, -1 )
{
} // SelectionData()


/**
 * convenience constructor 
 * @param seedPoint the seed point position
 * @param path0 tracking path along the primary direction
 * @param path1 tracking path along the reverse primary direction
 */
vxSeedpoints::SelectionData::SelectionData( Point3Df & seedPoint, std::vector< Point3Df > & path0, std::vector< Point3Df > & path1 )
: m_seedPoint( seedPoint ),
m_trackPath0( path0 ),
m_trackPath1( path1 )
{
} // SelectionData()


/**
 * Constructor
 */
vxSeedpoints::vxSeedpoints()
{
  Clear();
} // vxSeedpoints()


/**
 * Clear out all the selections
 */
void vxSeedpoints::Clear()
{
  m_selections.clear();
  m_cursorSeedpoint.SetSeedpointInvalid();
} // Clear()


/**
 * Get the click point 
 * @param nth: the Nth seed points clicked by users
 */
const Point<int4> vxSeedpoints::GetSeedPointAsInt( const int2 & iNth )
{
  Point3Df pnt( GetSeedPointAsFloat( iNth ) );
  return Point< int4 >( pnt.m_x + 0.5F, pnt.m_y + 0.5F, pnt.m_z + 0.5F ); 
} // GetSeePointAsInt()


/**
 * Adds a click point to the end of the list
 * @param clickPoint the clicked point
 * @param path0 the primary tracking path
 * @param path1 the minue primary tracking path
 */
void vxSeedpoints::PushBack( Point3Df & seedPoint, std::vector< Point3Df > & path0, std::vector< Point3Df > & path1 )
{
  SelectionData seed( seedPoint, path0, path1 );
  m_selections.push_back( seed );  
} // PushBack()


/**
 * Removes a selection data from the end of the list
 */
void vxSeedpoints::PopBack()
{
  if ( m_selections.size() > 0 )
    m_selections.pop_back();

} // PopBack()


/**
 * remove the end point of path0
 * @param iNumOfPoints how many points to remove
 */
void vxSeedpoints::PopBackOnPath0( const int4 iNumOfPoints )
{  
  std::vector< Point3Df > & path = m_selections.back().m_trackPath0; 

  int4 iPopingPoints( iNumOfPoints );
  while ( ! path.empty() && iPopingPoints > 0 )
  {
    path.pop_back();
    --iPopingPoints;
  }
} // PopBackOnPath0()


/**
 * remove the end point of path1
 * @param iNumOfPoints how many points to remove
 */
void vxSeedpoints::PopBackOnPath1( const int4 iNumOfPoints )
{
  std::vector< Point3Df > & path = m_selections.back().m_trackPath1; 

  int4 iPopingPoints( iNumOfPoints );
  while ( ! path.empty() && iPopingPoints > 0 )
  {
    path.pop_back();
    --iPopingPoints;
  }
} // PopBackOnPath1()


/**
 * Get the index of a point
 * @param iPointIndex     the point index
 * @param iSelectionIndex the index of the seleciton
 * @param iPathIndex      the index of the path
 * @return found
 */
bool vxSeedpoints::GetSelectionIndex( int4 iPointIndex, int4 & iSelectionIndex, int4 & iPathIndex )
{
  int4 iLocalPointIndex(iPointIndex);
  bool bFound( false );

  for (int4 i(0); i < m_selections.size() && !bFound; ++i )
  {
    int4 iNumOfPoints(0);
    iNumOfPoints += 1; // the seed path;
    iNumOfPoints += m_selections[i].m_trackPath0.size();
    iNumOfPoints += m_selections[i].m_trackPath1.size();

    if ( iLocalPointIndex < iNumOfPoints )
    {
      bFound = true;
      iSelectionIndex = i;
      iPathIndex = iLocalPointIndex;
    }
    else
      iLocalPointIndex -= iNumOfPoints;
  }

  return bFound;
} // GetSelectionIndex()


/**
 * Get the point value 
 * @param iSelectionIndex the index of the selection
 * @param iPathIndex      the index of the path
 * @param pathPoint       the value of the point
 * @return found
 */
bool vxSeedpoints::GetPathPoint( const int4 iSelectionIndex, const int4 iPathIndex, Point<float4> & pathPoint )
{
  bool bFound( false );

  if ( iSelectionIndex < 0 || iPathIndex < 0 )
    return false;

  if ( iSelectionIndex < m_selections.size() )
  {
    int4 iPntIndex( iPathIndex );

    if ( iPntIndex == 0 )
    {
      bFound = true;
      pathPoint = m_selections[iSelectionIndex].m_seedPoint;
    }
    else
      iPntIndex -= 1;

    if ( ! bFound )
    {  
      int4 iPath0Len = m_selections[iSelectionIndex].m_trackPath0.size();
      if ( iPntIndex < iPath0Len )
      {
        bFound = true;
        pathPoint = m_selections[iSelectionIndex].m_trackPath0[iPntIndex];
      }
      else
        iPntIndex -= iPath0Len;
    }

    if ( ! bFound )
    {
      int4 iPath1Len = m_selections[iSelectionIndex].m_trackPath1.size();
      if ( iPntIndex < iPath1Len )
      {
        bFound = true;
        pathPoint = m_selections[iSelectionIndex].m_trackPath1[iPntIndex];
      }
    }
  }
  
  return bFound;

} // GetPathPoint()


/**
 * Trim off part of path 
 * @param iSelectionIndex: the Nth segments
 * @param iPathIndex: the Nth points on the path of this segment       
 */
void vxSeedpoints::TrimOffPath( const int4 iSelectionIndex, int4 iPathIndex )
{ 
  std::vector< Point<float4> > & path0( m_selections[iSelectionIndex].m_trackPath0 );
  std::vector< Point<float4> > & path1( m_selections[iSelectionIndex].m_trackPath1 );
  const int4 iPath0Length( path0.size() );
  const int4 iPath1Length( path1.size() );
  bool bDone( false );

  if ( iSelectionIndex < 0 && iPathIndex <= 0 )
    bDone = true;
  else
    iPathIndex -= 1;

  if ( ! bDone )
  {
    if ( iPathIndex < iPath0Length )
    {
      path0.erase( path0.begin()+iPathIndex+1, path0.end() );
      bDone = true;
    }
    else
      iPathIndex -= iPath0Length;
  }

  if ( ! bDone )
  {
    if ( iPathIndex < iPath1Length )
    {       
      path1.erase( path1.begin()+iPathIndex+1, path1.end() );
      bDone = true;
    }
  }
} // TrimOffPath()


/**
 * Estimate the center point of the vessel near the given click point.
 *
 * @param clickedPoint   the input position
 * @param dtbVol         a distance to boundary volume
 * @param estimatedPoint the estimated output center point
 *
 * @return true if a vessel center point is found
 */
bool vxSeedpoints::EstimateCenterPoint( const Point3D<float4> & initialPoint, vxBlockVolume<uint2> & DTBVol, Point3Df & estimatedPoint )
{
  //Qian added: initialPoints is by units 
  //Qian Added: DTBVol can be DTB or can be vesselness

  const int2 iInsideTh( 600 );   // Qian Added: a threshold may be reconsidered ????????
  const int2 iPlaneSize( 128 );
  const Vector<float4> xAxis( 1.0F, 0.0F, 0.0F );
  const Vector<float4> yAxis( 0.0F, 1.0F, 0.0F );
  const Vector<float4> zAxis( 0.0F, 0.0F, 1.0F );

  vxBlockVolumeIterator<uint2> dtbVoliter( DTBVol );
  dtbVoliter.SetPos( Vector3D<int4>(initialPoint) );
  if ( dtbVoliter.GetVoxel() < iInsideTh )
    return false;

  Triplef vxUnits = DTBVol.GetHeader().GetVoxelUnits();
  Point<float4> orgPnt = initialPoint.GetScaledAnIso( vxUnits );

  CrossPlane plane( iPlaneSize );
  plane.SetScale( 1.0F );

  float4 zCrossRatio, yCrossRatio, xCrossRatio;
  Point<float4> zCenter, yCenter, xCenter;

  // z-plane
  plane.MoveCrossPlane( orgPnt, zAxis, yAxis );
  plane.FillCrossPlane( DTBVol );
  plane.IsolateMask( iInsideTh );    
  plane.CalShapePars( );
  if ( plane.IsValid() )
  {
    zCrossRatio = plane.GetWidth()/plane.GetDiameter();
    zCenter = plane.ConvertPnt2VolPos( vxUnits, plane.GetCentroid() ); //Qian added: zCenter by units
  }
  else
    zCrossRatio = 0.0F;


  // y-plane
  plane.MoveCrossPlane( orgPnt, yAxis, xAxis );
  plane.FillCrossPlane( DTBVol );
  plane.IsolateMask( iInsideTh );    
  plane.CalShapePars( );
  if ( plane.IsValid() )
  {
    yCrossRatio = plane.GetWidth()/plane.GetDiameter();
    yCenter = plane.ConvertPnt2VolPos( vxUnits, plane.GetCentroid() );
  }
  else
    yCrossRatio = 0.0F;

  // x-plane
  plane.MoveCrossPlane( orgPnt, xAxis, zAxis );
  plane.FillCrossPlane( DTBVol );
  plane.IsolateMask( iInsideTh );    
  plane.CalShapePars( );
  if ( plane.IsValid() )
  {
    xCrossRatio = plane.GetWidth()/plane.GetDiameter(); //Qian Added: must  be smaller than 1 
    xCenter = plane.ConvertPnt2VolPos( vxUnits, plane.GetCentroid() );
  }
  else
    xCrossRatio = 0.0F;

  float4 ratio = Max( zCrossRatio, yCrossRatio, xCrossRatio );
  Point<float4> center;
  if ( ratio == 0.0F )
    return false;

  if ( ratio == zCrossRatio )
    center = zCenter;
  else if ( ratio == yCrossRatio )
    center = yCenter;
  else if ( ratio == xCrossRatio )
    center = xCenter;

  estimatedPoint = center;  //Qian Added: by units

  return true;

} // EstimateCenterPoint()


// undefines
#undef FILE_REVISION


// $Log: vxSeedpoints.C,v $
// Revision 1.3  2007/10/03 16:14:13  cqian
// make it able to handle anisotropic blockvolume
//
// Revision 1.2  2006/09/01 13:24:58  frank
// made the first argument constant
//
// Revision 1.1  2006/08/30 21:19:26  frank
// moved here from vascular
//
// Revision 1.2  2005/06/08 15:02:33  vxconfig
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
// Revision 1.10  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.2.1  2004/02/25 15:00:40  frank
// formatting
//
// Revision 1.9  2004/01/14 18:59:28  wenli
// Add Comments
//
// Revision 1.8  2004/01/13 19:13:28  frank
// code review
//
// Revision 1.7  2003/12/08 21:57:07  wenli
// using 'Tab' key to switch between two ends, using 'BackSpace' to remove the points.
//
// Revision 1.6  2003/10/13 21:49:06  wenli
// clear up codes
//
// Revision 1.5  2003/10/13 20:21:29  wenli
// vxSeedPoint data structure are changed
//
// Revision 1.4  2003/10/07 21:07:44  wenli
// change the vxSeedpoint list to handle the tracking seeds
//
// Revision 1.3  2003/10/01 17:38:49  wenli
// update the threshold of estimated centerpoint
//
// Revision 1.2  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.7  2003/04/28 18:20:17  frank
// code review
//
// Revision 1.6  2003/04/22 18:45:20  wenli
// multi-seed points
//
// Revision 1.5  2003/04/18 17:48:33  frank
// Removed some debug messages
//
// Revision 1.4  2003/04/15 17:45:29  frank
// Added preview of the vessel underneath the cursor
//
// Revision 1.3  2003/03/24 14:16:53  frank
// Permitted selection point undo
//
// Revision 1.2  2003/03/20 21:29:24  frank
// Made it work like a queue as you select new points
//
// Revision 1.1  2003/03/19 16:19:30  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxSeedpoints.C,v 1.3 2007/10/03 16:14:13 cqian Exp $
// $Id: vxSeedpoints.C,v 1.3 2007/10/03 16:14:13 cqian Exp $
