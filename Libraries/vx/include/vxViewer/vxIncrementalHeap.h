// $Id: vxIncrementalHeap.h,v 1.1 2006/09/25 13:17:33 geconomos Exp $
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
#include <list>
#include <vector>
#include "Point3D.h"
#include "vxBlockVolumeIterator.h"


/**
 * Class to hold incremental growth voxels as they are on the boundary of the selected growth.
 */
class VX_VIEWER_DLL vxIncrementalHeap
{
// Inner classes
public:

  /**
   * An element in the heap - really just a voxel position
   */
  class vxHeapListElement : public Point3Di
  {
  public:

    /// Constructor
    vxHeapListElement( const Point3Di & other ) { m_x = other.m_x; m_y = other.m_y; m_z = other.m_z; }

    /// Get the current position as a Point3Di
    Point3Di GetPosition() const { return Point3Di( m_x, m_y, m_z ); }

  }; // class vxHeapListElement

  /**
   * A list of heap elements (voxels) having similar probability of being next
   */
  class vxHeapList : public std::list< vxHeapListElement > {};

  /**
   * Handles the similarity of voxels within the incremental heap
   */
  class vxIncrementalSimilarity
  {
    // Member Functions
  public:

    /// Constructor
    vxIncrementalSimilarity();

    /// Set up for growing in an intensity volume
    void SetGrowingInIntensity( vxBlockVolumeIterator< uint2 > * pIntensityIter, const int4 iInitialIntensity, const float4 fInitialStandardDeviation );

    /// Set up for growing in a vesselness volume
    void SetGrowingInVesselness( vxBlockVolumeIterator< uint2 > * pVesselnessIter );

    /// Is growing in intensity volume
    bool IsGrowingInIntensity() { return m_pIntensityIter != NULL; }

    /// Is growing in vesselness volume
    bool IsGrowingInVesselness() { return m_pVesselnessIter != NULL; }

    /// Get the iterator for the intensity volume
    vxBlockVolumeIterator< uint2 > * GetIntensityIterator() { return m_pIntensityIter; }

    /// Get the iterator for the vesselness volume
    vxBlockVolumeIterator< uint2 > * GetVesselnessIterator() { return m_pVesselnessIter; }

    /// get the initial intensity
    int4 GetInitialIntensity() { return m_iInitialIntensity; }

    /// get the initial standard deviation
    float4 GetInitialStandardDeviation() { return m_fInitialStandardDeviation; }

  // Data Members
  private:

    /// Intensity volume iterator
    vxBlockVolumeIterator< uint2 > * m_pIntensityIter;

    /// Vesselness volume iterator
    vxBlockVolumeIterator< uint2 > * m_pVesselnessIter;

    /// initial intensity
    int4 m_iInitialIntensity;

    /// initial standard deviation
    float4 m_fInitialStandardDeviation;

  }; // class vxIncrementalSimilarity

// Member Functions
public:

  /// Constructor
  vxIncrementalHeap();

  /// Clear the heap
  void Clear();

  /// Push an element onto the heap
  void Push( vxHeapListElement & element );

  /// If the heap is empty
  bool IsEmpty() const;

  /// Pop an element off the top of the heap - only call if you are sure the heap is not empty
  void Pop();

  /// Get a reference to the top heap element - only call if you are sure the heap is not empty
  const vxHeapListElement & Top() const;

  /// Get the incremental similarity structure
  static vxIncrementalSimilarity & GetIncrementalSimilarity() { return m_incrementalSimilarity; }

  /// helper function to get the standard deviation in a standard way for computing similarity
  static float4 GetStandardDeviation( vxBlockVolumeIterator<uint2> & volIter );

// Member Functions
private:
  
  /// Find the next most likely voxel in the heap
  void FindNext();

  /// Determine the similarity of a voxel to the desired voxel
  uint4 GetSimilarityMeasure( vxHeapListElement & element );

// Data Members
private:

  /// The list of heap lists
  std::vector< vxHeapList > m_heapList;

  /// The position of the next known list
  uint4 m_uTopList;

  /// The incremental similarity structure
  static vxIncrementalSimilarity m_incrementalSimilarity;

}; // class vxIncrementalHeap


// $Log: vxIncrementalHeap.h,v $
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
// Revision 1.4  2004/01/09 15:41:03  frank
// code review
//
// Revision 1.3  2004/01/07 21:47:54  wenli
// add function IsGrowingInIntensity()
//
// Revision 1.2  2003/10/28 15:51:20  frank
// comments
//
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIncrementalHeap.h,v 1.1 2006/09/25 13:17:33 geconomos Exp $
// $Id: vxIncrementalHeap.h,v 1.1 2006/09/25 13:17:33 geconomos Exp $
