// $Id: vxIncrementalGrowthImage.h,v 1.1 2006/09/25 13:17:33 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <vector>
#include "Point2D.h"
#include "Image.h"
#include "vxIncrementalSlider.h"
#include "vxEnvironment.h"


// forwards
template < class T > class vxBlockVolume;
class vxViewerType;


/**
 * Contains a set of image pixels
 */
class VX_VIEWER_DLL vxIncrementalGrowthImage : public vxElement
{
// Member Functions
public:

  /// Constructor
  vxIncrementalGrowthImage();

  /// Initialize all data
  void Initialize( const uint2 width, const uint2 height );

  /// Add a point to the list of points
  void AddPoint( const Point2D<int2> & position, const uint4 uVoxelsFound, const uint4 uVoxelsProcessed );

  /// Get a vector containing all the pixels displayed in the order they were found
  std::vector< Point2D< int2 > > & GetPoints() { return m_points; }

  /// Get the vector containing the number of voxels that have been found for each new pixel displayed
  std::vector< uint4 > & GetVoxelsFound() { return m_voxelsFound; }

  /// Get the vector containing the number of voxels that have been processed for each new pixel displayed
  std::vector< uint4 > & GetVoxelsProcessed() { return m_voxelsProcessed; }

  /// Clear all data
  void Clear();

  /// Search for the number of points to display given the number of desired voxels
  uint4 GetNumberPointsToDisplay( vxIncrementalSlider & incrementalSlider ) const;

  /// Project a voxel to the image
  void ProjectToImage( const Point3Di & position, const uint4 uVoxelsFound, const uint4 uVoxelsProcessed,
                       const Triple< float4 > & voxelUnits, vxViewerType & viewerType, vxEnvironment & environment );

  /// Get the image
  const Image< bool > & GetImage() const { return m_image; }

// Data Members
private:

  /// The image that contains the selected points
  Image< bool > m_image;

  /// The list of points used for rendering
  std::vector< Point2D< int2 > > m_points;

  /// The list of voxel counts corresponding to the number of points rendered
  std::vector< uint4 > m_voxelsFound;

  /// The list of voxel processed counts corresponding to the number of points rendered
  std::vector< uint4 > m_voxelsProcessed;

}; // class vxIncrementalGrowthImage


// $Log: vxIncrementalGrowthImage.h,v $
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
// Revision 1.4  2004/02/06 19:40:50  frank
// Issue #3726: Fixed Easy Select on first and last slices
//
// Revision 1.3  2004/02/02 20:01:34  frank
// Added an accessor
//
// Revision 1.2  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/05/01 18:49:45  dongqing
// code review
//
// Revision 1.3  2003/04/30 17:04:49  frank
// code review
//
// Revision 1.2  2003/04/23 14:42:53  frank
// Stored the number of voxels processed so that we can be sure about how many times to try to find voxels in the second pass of incremental growth.
//
// Revision 1.1  2003/04/18 17:50:02  frank
// Split out vxIncrementalGrowthImage to its own file
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIncrementalGrowthImage.h,v 1.1 2006/09/25 13:17:33 geconomos Exp $
// $Id: vxIncrementalGrowthImage.h,v 1.1 2006/09/25 13:17:33 geconomos Exp $