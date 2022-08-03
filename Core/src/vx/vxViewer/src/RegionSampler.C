// $Id: RegionSampler.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates Region Sampler for image processing.
 */


// includes
#include "stdafx.h"
#include "RegionSampler.h"
#include "vxBlockVolumeRegionGrowIterator.h"
#include "vxDecider.h"
#include "vxIntensityStatistic.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 */
RegionSampler::RegionSampler()
: m_fSampleRadius( 5.0F ),
m_eSamplePlane( XY_PLANE ),
m_uThresholdRange( std::make_pair( 0, 1 ) )
{
} // RegionSampler()


/**
 * sample an area to determine threshold range in an orthographic MPR slice
 *
 * @param seedPoint reference to a 3D point coordinates of seed used in region grow
 * @param pScanVol  pointer to a scan volume
 */
void RegionSampler::SampleIntensityRangeOrtho( const Point<uint2> & seedPoint, vxBlockVolume<uint2> * pIntensityVol,
                                               const enum SampleRestrictionEnum & planeRestriction )
{

  m_eSamplePlane = planeRestriction;
  m_seedPoint = seedPoint;
  SampleIntensityRange( seedPoint, pIntensityVol );

} // SampleIntensityRangeOrtho()


/**
 * sample an area to determine threshold range in an oblique MPR slice
 *
 * @param cornerPoint        one of the corners
 * @param verticalDiameter   the first direction away from the corner
 * @param horizontalDiameter the second direction away from the corner
 * @param pIntensityVolume   the intensity volume
 */
void RegionSampler::SampleIntensityRangeOblique( const Quadrilateral & quadrilateral, vxBlockVolume<uint2> * pIntensityVol )
{

  m_eSamplePlane = RegionSampler::OBLIQUE_PLANE;

  // compute the ellipse definition
  m_obliqueQuadrilateral = quadrilateral;
  Point3Df cornerPoint( quadrilateral.GetPoint( Quadrilateral::BOTTOM_LEFT ) );
  Vector3Df verticalEdge( quadrilateral.GetPoint( Quadrilateral::BOTTOM_RIGHT ) - quadrilateral.GetPoint( Quadrilateral::BOTTOM_LEFT ) );
  Vector3Df horizontalEdge( quadrilateral.GetPoint( Quadrilateral::TOP_LEFT ) - quadrilateral.GetPoint( Quadrilateral::BOTTOM_LEFT ) );

  // compute the statistics
  vxIntensityStatistic statistics;
  vxUtils::ComputeEllipseStatistics( statistics, cornerPoint, verticalEdge, horizontalEdge, * pIntensityVol );

  // update the results
  m_uThresholdRange.first = statistics.GetMinimum();
  m_uThresholdRange.second = statistics.GetMaximum();

} // SampleIntensityRangeOblique()


/**
 * sample an area to determine threshold range in an spherical region
 *
 * @param seedPoint reference to a 3D point coordinates of seed used in region grow
 * @param pScanVol  pointer to a scan volume
 */
void RegionSampler::SampleIntensityRangeSphere( const Point<uint2> & seedPoint, vxBlockVolume<uint2> * pIntensityVol )
{

  m_eSamplePlane = RegionSampler::SPHERICAL;
  m_seedPoint = seedPoint;
  SampleIntensityRange( seedPoint, pIntensityVol );

} // SampleIntensityRangeSphere()


/**
 * samples an area to determine threshold range
 *
 * @param seedPoint reference to a 3D point coordinates of seed used in region grow
 * @param pScanVol  pointer to a scan volume
 */
void RegionSampler::SampleIntensityRange( const Point<uint2> & seedPoint, vxBlockVolume<uint2> * pScanVol )
{

  // center point for sample
  Vector3D<int4> centerPt( seedPoint.m_x, seedPoint.m_y, seedPoint.m_z );

  // decider that declares a voxel as part of a region if within a specified range
  vxDeciderSphere<uint2> decider( centerPt, m_fSampleRadius, pScanVol->GetHeader().GetVoxelUnits() );

  // define the neighborhood
  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood;
  switch ( m_eSamplePlane )
  {
  case XY_PLANE:  eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04xy; break;
  case YZ_PLANE:  eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04yz; break;
  case XZ_PLANE:  eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04xz; break;
  case SPHERICAL: eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26  ; break;
  default: break;
  }

  // region grow iterator
  vxBlockVolumeRegionGrowIterator<uint2> regGrowIter( centerPt, *pScanVol, decider, eNeighborhood );

  // grow through the voxels in the region and sample voxels
  m_uThresholdRange.first = regGrowIter.GetVoxel();
  m_uThresholdRange.second = regGrowIter.GetVoxel();
  for ( ; regGrowIter.IsNotAtEnd(); ++regGrowIter )
  {
    m_uThresholdRange.first = Min( m_uThresholdRange.first, regGrowIter.GetVoxel() );
    m_uThresholdRange.second = Max( m_uThresholdRange.second, regGrowIter.GetVoxel() );
  } // end for all voxels

} // SampleIntensityRange()


#undef FILE_REVISION


// $Log: RegionSampler.C,v $
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/01/23 16:30:06  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.21  2003/12/22 16:20:57  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.20  2003/07/08 14:42:19  michael
// cosmetics
//
// Revision 1.19  2003/06/19 20:20:06  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.18  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.17  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.16  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.15  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.14  2003/04/29 12:15:34  frank
// Removed compile warning for missing #undef argument
//
// Revision 1.13  2003/04/28 17:01:31  michael
// more code reviews
//
// Revision 1.12  2003/04/25 19:16:42  dongqing
// code review
//
// Revision 1.11  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.10  2003/01/03 16:58:33  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.9  2001/01/01 06:07:52  ingmar
// made the decider sphere consider unisotropic scaling of the volume data
//
// Revision 1.8  2002/06/21 18:16:02  jaddonisio
// Modified region sample to use different planes(XY,YZ,XZ).
//
// Revision 1.7  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.6  2002/05/01 14:06:44  jaddonisio
// Clean-up.
//
// Revision 1.5  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.4  2001/12/17 16:38:11  jaddonisio
// i. Changed thresholds to int2 instead of uint2.
// ii. Changed Seta and Get to pass iPropID by value(to fix problem when value is an enum).
//
// Revision 1.3  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.2  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.1  2001/11/06 22:41:47  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/RegionSampler.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
// $Id: RegionSampler.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
