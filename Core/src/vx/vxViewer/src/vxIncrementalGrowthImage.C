// $Id: vxIncrementalGrowthImage.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
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
#include "vxIncrementalGrowthImage.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxUtils.h"


// Defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
* Constructor
*/
vxIncrementalGrowthImage::vxIncrementalGrowthImage()
: m_image( Triple< int4 >( 1, 1, 0 ), false )
{
} // vxIncrementalGrowthImage()


/**
 * Search for the number of points to display given the number of desired voxels
 *
 * @param incrementalSlider the incremental slider
 *
 * @return the number of points to display
 */
uint4 vxIncrementalGrowthImage::GetNumberPointsToDisplay( vxIncrementalSlider & incrementalSlider ) const
{
  // better to binary search, but this loop is sufficiently fast
  for ( uint4 i(0); i<m_voxelsFound.size(); ++i )
  {
    if ( m_voxelsFound[ i ] >= incrementalSlider.GetNumberDesiredVoxels() )
    {
      return i;
    }
  }

  return m_voxelsFound.size();

} // GetNumberPointsToDisplay()


/**
 * Initialize all data
 *
 * @param width       the image width
 * @param height      the image height
 * @param environment the environment
 */
void vxIncrementalGrowthImage::Initialize( const uint2 width, const uint2 height )
{

  m_image = Image< bool >( Triple<int4>( width, height, 0 ), false );

} // Initialize()


/**
 * Project a voxel to the image
 *
 * @param indexCoord       the volume index coordinates
 * @param uVoxelsFound     the number of voxels found
 * @param uVoxelsProcessed the number of voxels processed so far
 * @param voxelUnits       the units of a voxel
 * @param viewerType       the viewer type
 * @param environment      the environment
 */
void vxIncrementalGrowthImage::ProjectToImage( const Point3Di & indexCoord, const uint4 uVoxelsFound, const uint4 uVoxelsProcessed,
                                               const Triple< float4 > & voxelUnits, vxViewerType & viewerType, vxEnvironment & environment )
{
  Point3Df worldCoord( indexCoord.GetScaledAnIso( voxelUnits ) );

  const bool bMPROrthoViewer( dynamic_cast< vxViewerTypeMPROrtho * >( & viewerType ) != NULL );
  if ( bMPROrthoViewer )
  {
    uint1 uAxisIndex( environment.GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() ); // 0, 1, 2
    const uint4 uPlaneOrthoDistance( vxUtils::CalculateOffsetDistance( environment.GetPlaneOrtho()->GetDistance() / environment.GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] + 0.5F,
                                                                       environment.GetIntensityVolume()->GetHeader().GetVolDim()[ uAxisIndex ],
                                                                       environment.GetOffset(),
                                                                       environment.GetMaximumOffset() ) );
    const uint4 uPointOrthoDistance( indexCoord[ uAxisIndex ] );
    if ( uPlaneOrthoDistance != uPointOrthoDistance ) return;
  }
  else
  {
    if ( viewerType.IsVisible( worldCoord ) == false ) return;
  }

  // check if it can be projected to the screen
  Point2D<int2> screenCoord;
  if ( viewerType.ConvertWorldToScreen( worldCoord, screenCoord ) == true )
  {
    // splat to a group of voxels on the screen
    const int4 iRadius( 1 );
    for ( int4 y(-iRadius); y<=iRadius; ++y )
      for ( int4 x(-iRadius); x<=iRadius; ++x )
        AddPoint( Point2D< int2 >( screenCoord.m_x + x, screenCoord.m_y + y ), uVoxelsFound, uVoxelsProcessed );
  }
  else
  {
    //LogDbg( "problem projecting to screen", "IncrementalGrowth", "ProjectToImage" );
  }

} // ProjectToImage()



/**
 * Add a point to the list of points
 *
 * @param position     the 2D image position
 * @param uVoxelsFound the total number of voxels found up to this point
 * @param uVoxelsProcessed the total number of voxels processed up to this point
 */
void vxIncrementalGrowthImage::AddPoint( const Point2D< int2 > & position, const uint4 uVoxelsFound, const uint4 uVoxelsProcessed )
{
  // make sure it's on the screen
  if ( position.m_x < 0 || position.m_x > m_image.GetDim().m_x - 1 ||
       position.m_y < 0 || position.m_y > m_image.GetDim().m_y - 1 )
    return;

  // every other pixel is visible
  if ( ( position.m_x + position.m_y ) % 3 == 0 )
  {
    // check if it's already set
    const Point2D<uint2> pos( position );
    if ( m_image.GetPixel( pos ) == false )
    {
      // set the pixel
      m_image.SetPixel( pos, true );

      const uint4 uPrevious( ( m_voxelsFound.size() ) ? m_voxelsFound.back() : 0 );
      if ( uVoxelsFound < uPrevious )
        LogDbg( "uVoxelsFound < uPrevious : " + ToAscii(uVoxelsFound) + " < " + ToAscii( uPrevious ), "", "" );

      // add to the list
      m_points.push_back( pos );
      m_voxelsFound.push_back( uVoxelsFound );
      m_voxelsProcessed.push_back( uVoxelsProcessed );
    }
  }
} // AddPoint()


/**
 * Clear all data
 */
void vxIncrementalGrowthImage::Clear()
{
  m_image.Clear();
  m_points.clear();
  m_voxelsFound.clear();
} // Clear()


// undefines
#undef FILE_REVISION


// $Log: vxIncrementalGrowthImage.C,v $
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.3  2006/09/20 16:57:31  frank
// added missing include
//
// Revision 1.2  2005/06/08 14:59:02  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:33  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/25 14:57:40  frank
// formatting
//
// Revision 1.5  2004/02/06 19:40:50  frank
// Issue #3726: Fixed Easy Select on first and last slices
//
// Revision 1.4  2004/01/29 18:04:53  frank
// code review
//
// Revision 1.3  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.2  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/04/30 17:04:48  frank
// code review
//
// Revision 1.2  2003/04/23 14:42:53  frank
// Stored the number of voxels processed so that we can be sure about how many times to try to find voxels in the second pass of incremental growth.
//
// Revision 1.1  2003/04/18 17:50:02  frank
// Split out vxIncrementalGrowthImage to its own file
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxIncrementalGrowthImage.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $
// $Id: vxIncrementalGrowthImage.C,v 1.1 2006/09/25 13:17:05 geconomos Exp $