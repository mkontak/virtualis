// $Id: vxBlockVolumeFilterFitLinearZ.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( george@viatronix.com )


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterFitLinearZ.h"
#include "vxBlockVolumeIterator.h"
#include "Timer.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


// explicit instantiations
template class vxBlockVolumeFilterFitLinearZ< int1>;
template class vxBlockVolumeFilterFitLinearZ< int2>;
template class vxBlockVolumeFilterFitLinearZ< int4>;
template class vxBlockVolumeFilterFitLinearZ<uint1>;
template class vxBlockVolumeFilterFitLinearZ<uint2>;
template class vxBlockVolumeFilterFitLinearZ<uint4>;


// namespaces
using namespace std;


/**
 * class to efficiently interpolate between two values.
 * It is most efficient when you set the weight once at the beginning,
 * then call it repeatedly when you need to interpolate two new
 * values with the same weight.
 */
class VoxelInterpolatorInt
{
// Member Functions
public:

  /**
   * Constructor.
   */
  VoxelInterpolatorInt()
  {
    SetWeight0( 1.0F );
  } // VoxelInterpolatorInt()

  /**
   * Constructor.  
   * @param fWeight0 is the weight (0,1) of the first value.
   */
  VoxelInterpolatorInt( const float4 & fWeight0 )
  {
    SetWeight0( fWeight0 );
  } // VoxelInterpolatorInt()

  /**
   * Sets the weight (0,1) of the first value.
   * @param fWeight0
   */
  void SetWeight0( const float4 fWeight0 )
  {
    m_iWeight0 = fWeight0 * (1<<16);
  } // SetWeight0()

  /**
   * Interpolates between the two values.
   * 
   * @param iValue0 first value
   * @param iValue1 second value
   * @return        the interpolated value
   */
  int4 GetInterpolation( const int4 iValue0, const int4 iValue1 )
  {
    const int4 iDiff(iValue0-iValue1);
    const int4 iMult(iDiff * m_iWeight0);
    const int4 iShift(iMult >> 16);
    const int4 iResult(iShift + iValue1);
    return iResult;
  } // GetInterpolation()

// Data Members
private:

  /// the weight of the first value
  uint4 m_iWeight0;

}; // class VoxelInterpolatorInt


/**
 * class to efficiently interpolate between two values.
 * It is most efficient when you set the weight once at the beginning,
 * then call it repeatedly when you need to interpolate two new
 * values with the same weight.
 */
class VoxelInterpolatorFloat
{
// Member Functions
public:

  /**
   * Constructor
   */
  VoxelInterpolatorFloat()
  {
    SetWeight0( 1.0F );
  } // VoxelInterpolatorFloat()

  /**
   * Constructor
   *
   * @param fWeight is the weight (0,1) of the first value
   */
  VoxelInterpolatorFloat( const float4 & fWeight )
  {
    SetWeight0( fWeight );
  } // VoxelInterpolatorFloat()

  /**
   * Sets the weight (0,1) of the first value.
   *
   * @param fWeight the weight of the first value
   */
  void SetWeight0( const float4 fWeight )
  {
    m_fWeight = fWeight;
  } // SetWeight0()

  /**
   * Interpolates between the two values
   *
   * @param iValue0 the first value
   * @param iValue1 the second value
   * @return        the interpolated value
   */
  inline int4 GetInterpolation( const int4 iValue0, const int4 iValue1 )
  {
    
#ifdef _WIN64
    const float4 fResult = (iValue0-iValue1) * m_fWeight + iValue1;
    int4 iResult( static_cast< float4 >( fResult ) );

#else
    
    const float4 fResult = (iValue0-iValue1) * m_fWeight + iValue1;
    int4 iResult(0);
    __asm FLD   DWORD PTR [fResult]
    __asm FISTP DWORD PTR [iResult] 
#endif

    return iResult;
  } // GetInterpolation()

// Data Members
private:

  /// the weight of the first value
  float4 m_fWeight;

}; // class VoxelInterpolatorFloat


/**
 * Interpolate in the Z - direction to create the destination volume.
 */
template <class VoxelType> 
void vxBlockVolumeFilterFitLinearZ<VoxelType>::ApplyFilter()
{
  Timer filterTimer;
  Timer withinBlockTimer; withinBlockTimer.Stop();
  Timer acrossBoundaryTimer; acrossBoundaryTimer.Stop();
  Timer justCopyTimer; justCopyTimer.Stop();

  // create iterators for the src and dst volumes
  vxBlockVolumeIterator<VoxelType> srcIter(*m_pSrcVolume);
  vxBlockVolumeIterator<VoxelType> dstIter(*m_pDstVolume);

  // find the scaling factors to backproject a voxel from dst vol to src vol
  Triplei iSrcDim = m_pSrcVolume->GetHeader().GetVolDim();
  Triplei iDstDim = m_pDstVolume->GetHeader().GetVolDim();
  const int4 iScaleZ = (iDstDim.Z() / iSrcDim.Z()) + 1;
  std::vector<float4> vfWeights( iScaleZ, 0.0F );
  std::vector<VoxelInterpolatorInt> vInterpolators( iScaleZ );
  for ( int i(0) ; i<iScaleZ ; ++i )
  {
    vfWeights[i] = float4(i) / float4(iScaleZ);
    vInterpolators[i].SetWeight0( 1.0F - vfWeights[i] );
  }

  // write interpolated values into the destination volume by running
  // through the source volume
  for ( ; srcIter.IsNotAtEnd() ; srcIter.NextBlockZYX() )
  {
    for ( ; srcIter.IsNotAtEndOfBlock(); srcIter.NextZYXinsideBlock() )
    {
      Vector3D<int4> pos( srcIter.GetPos() );
      if ( pos.Z() < iSrcDim.Z()-1 )
      {
        //// for all but the last slice
        const VoxelType vox0 = srcIter.GetVoxel();
        const VoxelType vox1 = srcIter.Peek0px0py1pz();
        const VoxelType vox1MinusVox0(vox1-vox0);
        pos.Z() *= iScaleZ;
        dstIter.SetPos( pos );
        // see if we are completely within a block
        if ( (pos.Z() % vxBlockVolumeIteratorConsts::eL0VoxPosNum) <= (vxBlockVolumeIteratorConsts::eL0VoxPosNum - iScaleZ) )
        {
          withinBlockTimer.Continue();
          // copy first voxel directly (no interpolation necessary)
          dstIter.SetVoxel( vox0 ); // important to use iterator to make sure a brand new block is changed from reference copy to unique

          // completely within a block - use pointer
          VoxelType * pVox = const_cast< VoxelType * >( & dstIter.GetVoxel() );

          pVox += vxBlockVolumeIteratorConsts::eL0VoxOffsetDeltaZ;

          // walk along the values between voxels in two original slices
          for ( int4 i=1 ; i<iScaleZ ; ++i )
          {
            *pVox = vInterpolators[i].GetInterpolation( vox0, vox1 );
            pVox += vxBlockVolumeIteratorConsts::eL0VoxOffsetDeltaZ;

          }
          withinBlockTimer.Stop();
        }
        else
        {
          // may cross block boundaries - use iterator
          acrossBoundaryTimer.Continue();

          // copy first voxel directly (no interpolation necessary)
          dstIter.SetVoxel( vox0 );
          dstIter.IncZ();

          // walk along the values between voxels in two original slices
          for ( int4 i=1 ; i<iScaleZ ; ++i )
          {
            dstIter.SetVoxel( vInterpolators[i].GetInterpolation( vox0, vox1 ) );
            dstIter.IncZ();
          }
          acrossBoundaryTimer.Stop();
        }
      }
      else
      {
        justCopyTimer.Continue();
        if ( pos.Z() == iSrcDim.Z()-1 )
        {
          // if this is the last slice, just copy voxel values.
          dstIter.SetPos(srcIter.GetPos().X(), srcIter.GetPos().Y(), iDstDim.Z()-1);
          dstIter.SetVoxel(srcIter.GetVoxel()) ;
        }
        else
        {
          ; // nop; this is outside the volume, but within the last 16^3 block
        }
        justCopyTimer.Stop();
      }
    }
  }

  LogDbg( "FitLinearZ within block " + ToAscii( withinBlockTimer.ElapsedTime() ), "vxBlockVolumeFilterFitLinearZ", "ApplyFilter");
  LogDbg( "FitLinearZ across boundary " + ToAscii( acrossBoundaryTimer.ElapsedTime() ), "vxBlockVolumeFilterFitLinearZ", "ApplyFilter");
  LogDbg( "FitLinearZ just copy " + ToAscii( justCopyTimer.ElapsedTime() ), "vxBlockVolumeFilterFitLinearZ", "ApplyFilter");
  LogDbg( "FitLinearZ Interpolation Total " + ToAscii( filterTimer.ElapsedTime() ), "vxBlockVolumeFilterFitLinearZ", "ApplyFilter");
} // ApplyFilter()


#undef FILE_REVISION


// $Log: vxBlockVolumeFilterFitLinearZ.C,v $
// Revision 1.3  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.2  2006/02/09 14:03:41  geconomos
// 64-bit conditionals
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.12  2003/05/16 13:16:20  frank
// formatting
//
// Revision 1.11  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.10  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.9  2003/01/22 21:35:44  ingmar
// dimension and units are now Triples
//
// Revision 1.8  2002/10/08 16:49:40  frank
// Added timings.
//
// Revision 1.7  2002/10/07 17:00:55  ingmar
// added float interpolation option and improved speed of integer interpolation
//
// Revision 1.6  2002/10/03 15:15:03  ingmar
// identified room for improvement escpecially when optimizing for P4
//
// Revision 1.5  2002/10/03 15:04:55  ingmar
// ensured interpolation does not write to voxels outside the volume dimension
// also ensured that raw pointer use is not applied to shared blocks
//
// Revision 1.4  2002/09/06 22:57:45  frank
// Sped up interpolation.  4.5s -> 3.4s
//
// Revision 1.3  2002/09/05 22:50:59  frank
// Working on speedup.
//
// Revision 1.2  2002/07/08 19:54:45  uday
// ApplyFilter() update to fix interpolation error.
//
// Revision 1.1  2002/06/10 20:55:35  uday
// Initial Version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterFitLinearZ.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
// $Id: vxBlockVolumeFilterFitLinearZ.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
