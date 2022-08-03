// $Id: vxSeedpoints.h,v 1.2 2006/09/01 13:24:58 frank Exp $
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
#include "vxElement.h"
#include "Point3D.h"


// forward declarations
template <class T> class vxBlockVolume;


/**
 * This class holds the endpoint selections by the user.
 */
class VX_VIEWER_DLL vxSeedpoints : public vxElement
{
// inner classes
public:

  /**
   * A class to hold the seed point and two tracking paths
   */
  class SelectionData
  {
  // member functions
  public:

    /// default constructor
    SelectionData();

    /// convenience constructor
    SelectionData( Point3Df & seedPoint, std::vector< Point3Df > & path0, std::vector< Point3Df > & path1 );

    /// get the seedpoint
    const Point3Df GetSeedpoint() { return m_seedPoint; }

    /// set the seedpoint
    void SetSeedpoint( const Point3Df & seedpoint ) { m_seedPoint = seedpoint; }

    /// is the seedpoint valid
    bool IsSeedpointValid() const { return m_seedPoint != Point3Df( -1, -1, -1 ); }

    /// set the seedpoint invalid
    void SetSeedpointInvalid() { m_seedPoint = Point3Df( -1, -1, -1 ); }

  // data members
  public:

    /// the seed point clicked by user in volume space (any negative values indicates inactivity)
    Point3Df m_seedPoint;

    /// the first path starting from the seed point
    std::vector< Point3Df > m_trackPath0;

    /// the second path starting from the seed point
    std::vector< Point3Df > m_trackPath1;

  }; // class SelectionData

// member functions
public:

  /// Constructor
  vxSeedpoints();

  /// Clear out all the selections
  void Clear();

  /// Return the number of clicks so far.
  const unsigned int GetNumClicks() const { return m_selections.size(); }

  /// Get the nth selection data
  const SelectionData & GetSelectionData( const int2 & iNth ) const { return m_selections[ iNth ]; }

  /// Return the nth click point
  const Point3Df & GetSeedPointAsFloat( const int2 & iNth ) const { return m_selections[ iNth ].m_seedPoint; }

  /// Get the nth track path0
  const std::vector< Point3Df > & GetTrackPath0( const int2 & iNth ) const { return m_selections[ iNth ].m_trackPath0; }

  /// Get the nth track path1
  const std::vector< Point3Df > & GetTrackPath1( const int2 & iNth ) const { return m_selections[ iNth ].m_trackPath1; }
  
  /// Trim off the part of path 
  void TrimOffPath( const int4 iSelectionIndex, int4 iPathIndex );

  /// Get the position of the point in the selection data
  bool GetSelectionIndex( int4 iPointIndex, int4 & iSelectionIndex, int4 & iPathIndex );

  /// Get the point of the selectiondata
  bool GetPathPoint( const int4 iSelectionIndex, const int4 iPathIndex, Point3D<float4> & pathPoint );

  /// Return the last click point, either the first or the second
  const Point3Df & GetLastSeedPoint() const { return m_selections.back().m_seedPoint; }

  /// Return the nth click point as an integer point
  const Point3D<int4> GetSeedPointAsInt( const int2 & iNth );

  /// Adds a click point and its tracking paths to the end of the list
  void PushBack( Point3Df & seedPoint, std::vector< Point3Df > & path0, std::vector< Point3Df > & path1  );

  /// Removes a click point and its tracking paths from the end of the list
  void PopBack();

  /// remove the end point of the path0 at current selection
  void PopBackOnPath0( const int4 iNumOfPoints );

  /// remove the end point of the path1 at current selection
  void PopBackOnPath1( const int4 iNumOfPoints );

  /// Estimate the closest centerpoint 
  static bool EstimateCenterPoint( const Point3D<float4> & initialPoint, vxBlockVolume<uint2> & DTBVol, Point3Df & estimatedPoint );

  /// Get the cursor seedpoint
  SelectionData & GetCursorSeedpoint() { return m_cursorSeedpoint; }

  /// toggle the end of backspace to remove points
  void SwitchBackSpaceAlt() { m_iBackSpaceAlt = m_iBackSpaceAlt ? 0 : 1; }
  
  /// get the end to remove points
  int2 GetBackSpaceAlt() { return m_iBackSpaceAlt ? 0 : 1; }

// data members
private:

  /// The selection ( click point and its tracking paths)
  std::vector< SelectionData > m_selections;

  /// The seedpoint near the cursor
  SelectionData m_cursorSeedpoint;

  /// current backspace point path
  int2 m_iBackSpaceAlt;

}; // class vxSeedpoints


// $Log: vxSeedpoints.h,v $
// Revision 1.2  2006/09/01 13:24:58  frank
// made the first argument constant
//
// Revision 1.1  2006/08/30 21:19:26  frank
// moved here from vascular
//
// Revision 1.4  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/08 18:56:09  frank
// spelling
//
// Revision 1.3  2005/06/08 15:02:33  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:14:38  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/01/13 19:13:28  frank
// code review
//
// Revision 1.6  2003/12/08 21:57:07  wenli
// using 'Tab' key to switch between two ends, using 'BackSpace' to remove the points.
//
// Revision 1.5  2003/10/13 21:49:06  wenli
// clear up codes
//
// Revision 1.4  2003/10/13 20:21:29  wenli
// vxSeedPoint data structure are changed
//
// Revision 1.3  2003/10/07 21:07:44  wenli
// change the vxSeedpoint list to handle the tracking seeds
//
// Revision 1.2  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.5  2003/04/28 18:20:17  frank
// code review
//
// Revision 1.4  2003/04/22 18:45:20  wenli
// multi-seed points
//
// Revision 1.3  2003/04/15 17:45:29  frank
// Added preview of the vessel underneath the cursor
//
// Revision 1.2  2003/03/24 14:16:53  frank
// Permitted selection point undo
//
// Revision 1.1  2003/03/19 16:19:30  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSeedpoints.h,v 1.2 2006/09/01 13:24:58 frank Exp $
// $Id: vxSeedpoints.h,v 1.2 2006/09/01 13:24:58 frank Exp $
