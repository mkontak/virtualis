// $Id: vxSkeletonator.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen  (dongqing@viatronix.com)
//
// Complete History at bottom of file.


#ifndef vxSkeletonator_h
#define vxSkeletonator_h

/**
 *  compute centerline and related PDR and CSI volume for endo-fly
 */


#include "vxVoxelCost.h"
#include "vxBlockVolume.h"
#include "Vector3D.h"

class Skeleton;
class SegmentList;


/**
 * The class vxSkeletonator requires INS and DFB volumes to create a skeleton.
 */
class vxSkeletonator 
{
  // Member functions
public:

  /// constructor
  vxSkeletonator( vxBlockVolume<uint1> *pIns, vxBlockVolume<uint1> *pSld, 
	                const std::string & destFilePath );

  /// destructor
  ~vxSkeletonator();

  /// apply the skeleton procedure
  bool Run();

private:

  /// Finds the starting point in the first segment.
  Vector3D<int2> GetFirstSegmentSeed( vxBlockVolume<uint1> *pIns );

  /// Finds the next starting point in the next segment.
  Vector3D<int2> GetNextSegmentSeed( vxBlockVolume<uint1> *pIns, const Vector3D<int2> & lastPosition,
                                     const uint4 & numSegments, const uint4 & passNumber );

  /// grow from seed 
  Vector3D<int2> GrowFromSeed( vxBlockVolume<uint1> *pIns, vxBlockVolume<uint1> *pSld,
	                             const Vector3D<int2> & startVoxelLocation, const uint4 & segmentVoxels, const bool & setPdr, 
								               vxBlockVolume<float4> * pPdrVol );

  /// compute centerline
  bool FindCenterline( const Vector3D<int2> startPoint, vxBlockVolume<float4> * pDfsVol, 
	                     vxBlockVolume<uint1> *pIns, Skeleton & skeleton );

  /// compute CSI
  bool ComputeCsi( const Skeleton & skeleton, vxBlockVolume<uint1> *pIns,
                   vxBlockVolume<uint2> *pCsiVol, const uint4 & segmentVoxels);
  
  // member functions
private:
  // Utility functions

  /// add segment
  void AddSegment( SegmentList & segmentList, const uint4 & segmentNumber,
                   Skeleton & skeleton1, Skeleton & skeleton2, const std::string & destFilePath );

  /// get weight from DFB
  inline float4 GetWeightFromDfb( const float4 & dfb );

  /// get weight from SLD 
  inline float4 GetWeightFromSld( const float4 & sld );

  /// count the number of voxels in a segment
  uint4 CountSegmentVoxels( vxBlockVolume<uint1> *pIns, const uint1 & segmentID );

  /// remove a segment
  uint4 RemoveSegment( vxBlockVolume<uint1> *pIns, const Vector3D<int2> & startPosotion );

  // Data members
private:

  vxBlockVolume<vxVoxelCost> m_nodeVol; // store the node volume around for later efficiency

// member variables
private:
  /// INS volume
  vxBlockVolume<uint1> *m_pIns;

  /// SLD volume
  vxBlockVolume<uint1> *m_pSld;

  /// destination path
  std::string m_destFilePath;

// Utility class
public:
  class TwentySixNeighborhood 
  {
  public:
    TwentySixNeighborhood(const Triple<float4> & voxelUnits)
    {
      m_offsets[ 0] = Vector3D<int2>( 1, 0, 0);
      m_offsets[ 1] = Vector3D<int2>(-1, 0, 0);
      m_offsets[ 2] = Vector3D<int2>( 0,-1, 0);
      m_offsets[ 3] = Vector3D<int2>( 0, 1, 0);
      m_offsets[ 4] = Vector3D<int2>( 0, 0,-1);
      m_offsets[ 5] = Vector3D<int2>( 0, 0, 1);
      
      m_offsets[ 6] = Vector3D<int2>( 1, 1, 0);
      m_offsets[ 7] = Vector3D<int2>( 1,-1, 0);
      m_offsets[ 8] = Vector3D<int2>(-1, 1, 0);
      m_offsets[ 9] = Vector3D<int2>(-1,-1, 0);
      
      m_offsets[10] = Vector3D<int2>( 1, 0, 1);
      m_offsets[11] = Vector3D<int2>(-1, 0, 1);
      m_offsets[12] = Vector3D<int2>( 0, 1, 1);
      m_offsets[13] = Vector3D<int2>( 0,-1, 1);
      
      m_offsets[14] = Vector3D<int2>( 1, 0,-1);
      m_offsets[15] = Vector3D<int2>(-1, 0,-1);
      m_offsets[16] = Vector3D<int2>( 0, 1,-1);
      m_offsets[17] = Vector3D<int2>( 0,-1,-1);
      
      m_offsets[18] = Vector3D<int2>( 1, 1, 1);
      m_offsets[19] = Vector3D<int2>( 1,-1, 1);
      m_offsets[20] = Vector3D<int2>(-1, 1, 1);
      m_offsets[21] = Vector3D<int2>(-1,-1, 1);

      m_offsets[22] = Vector3D<int2>( 1, 1,-1);
      m_offsets[23] = Vector3D<int2>( 1,-1,-1);
      m_offsets[24] = Vector3D<int2>(-1, 1,-1);
      m_offsets[25] = Vector3D<int2>(-1,-1,-1);

      register uint4 i;
      for( i=0; i<26; ++i ) 
	    {
        Vector3D<float4> offset(static_cast<float4>(m_offsets[i].m_x),
                                static_cast<float4>(m_offsets[i].m_y),
                                static_cast<float4>(m_offsets[i].m_z));
        offset.ScaleAnIso(voxelUnits);
        m_distances[i] = offset.GetLength();
      }
    } // TwentySixNeighborhood

    inline const Vector3D<int2> GetOffsetRaw(const uint4 & index) const
    {
      return (m_offsets[index]);
    } // GetOffsetRaw

    inline const Vector3D<int2> GetNeighbor(const Vector3D<int2> & currentLocation, const uint4 & index) const
    {
      if ( index < 26 ) 
	    {
        return (Vector3D<int2>( currentLocation.m_x + m_offsets[index].m_x,
                                currentLocation.m_y + m_offsets[index].m_y,
                                currentLocation.m_z + m_offsets[index].m_z));
      } 
	    else 
	    {
        return (currentLocation);
      }
    } // GetNeighbor

    inline const Vector3D<int2> GetNeighborRaw(const Vector3D<int2> & currentLocation, const uint4 & index) const
    {
      return (Vector3D<int2>( currentLocation.m_x + m_offsets[index].m_x,
                              currentLocation.m_y + m_offsets[index].m_y,
                              currentLocation.m_z + m_offsets[index].m_z));
    } // GetNeighborRaw

    inline const Vector3D<int2> GetOffset(const uint4 & index) const
    {
      if ( index < 26 ) 
	    {
        return (m_offsets[index]);
      } 
	    else 
	    {
        return (Vector3D<int2>(0,0,0));
      }
    } // GetOffset

    inline const float4 GetDistanceRaw(const uint4 & index) const
    {
      return (m_distances[index]);
    } // GetDistanceRaw

    inline const float4 GetDistance(const uint4 & index) const
    {
      if ( index < 26 ) 
	    {
        return (m_distances[index]);
      } 
	    else 
	    {
        return (0.0F);
      }
    }  // GetDistance

    inline const bool IsInsideVolume(const Vector3D<int2> & position, const Triple<uint4> & dimension) const
    {
      if ( position.m_x < dimension.m_x &&
           position.m_y < dimension.m_y &&
           position.m_z < dimension.m_z &&
           position.m_x >= 0            &&
           position.m_y >= 0            && 
           position.m_z >= 0 ) 
	    {
        return (true);
      } 
	    else 
	    {
        return (false);
      }
    } // IsInsideVolume

  public:
    /// offsets for 26 neighbors
    Vector3D<int2> m_offsets[26];

    /// distance for the 26 neighbors
    float4 m_distances[26];
  }; // class TwentySixNeighborhood

}; // class vxSkeletonator


/**
 * Compute weight, given a distance from boundary.
 * @param dfb : the distance from boundary.
 */
float4 vxSkeletonator::GetWeightFromDfb(const float4 & dfb)
{
  //return(5000*pow(1 - dfb/30.0F ,  8));
  return(5000*pow(1 - dfb/30.0F ,  4));
} // WeightFromDfb(const float4 & dfb)


/**
 * Compute weight, given a space leap distance.
 * @param sld : the space leap distance.
 */
float4 vxSkeletonator::GetWeightFromSld(const float4 & sld)
{
  const float4 dfb = sld * 0.25;
  //return(5000*pow(1 - sld/30.0F ,  8));  /// This results better centerline, but cost more on calculation
  return(5000*pow(1 - sld/30.0F ,  4));  /// This costs less calculation, but achieve doable centerline for practical use
} // WeightFromSld(const float4 & sld)


#endif // vxSkeletonator_h


// $Log: vxSkeletonator.h,v $
// Revision 1.7  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.6  2006/04/26 16:40:26  dongqing
// fix the CSI bug
//
// Revision 1.5  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.4  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.3  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.2  2006/03/13 18:39:24  dongqing
// finialize SKE codes
//
// Revision 1.1  2006/03/09 18:40:13  dongqing
// ad vxSkeletonator class initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxSkeletonator.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxSkeletonator.h,v 1.7 2007/03/06 16:34:01 dongqing Exp $
