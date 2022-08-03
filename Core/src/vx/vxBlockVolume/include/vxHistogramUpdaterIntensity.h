// $Id: vxHistogramUpdaterIntensity.h,v 1.6 2007/03/01 19:04:37 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declaration
#pragma once

// include decarations
#include "vxBlockVolume.h"

// template class definition
template <typename VoxelType>
class vxHistogramUpdaterIntensity
{
// member functions
public:
  
  /**
   * Constructor
   */
  inline vxHistogramUpdaterIntensity( vxBlockVolume< VoxelType > & volume ) : m_rVolume( volume ), m_iMin( 0 ), m_iMax( 0 )
  {
    // compute the maximum voxel value allowed for the specified voxel type
    const int4 iMaxAllowableValue( pow( 2.0, (double)( sizeof( VoxelType ) * 8 ) ) - 1 );

    // determine the maximum voxel value, the scaling specified in the volume may affect this
    m_iMaxValue =  Min( iMaxAllowableValue, vxIntensityConverter::GetMaxValue( volume.GetHeader() ) );
    
    // binary histogram
    m_iMask = GetNextPowerOfTwo( m_iMaxValue + 1 ) - 1;
    m_iShift = log( static_cast< float4 >( GetNextPowerOfTwo( m_iMask ) ) ) / log( 2.0F ) - 5.0F ;

    // base histogram
    m_rVolume.GetHistogram().SetNumOfBins( m_iMaxValue );
    m_pHistogram = &(*m_rVolume.GetHistogram().GetBinsVector().begin());
    m_rVolume.GetHistogram().SetMinValue( 0 );
    m_rVolume.GetHistogram().SetMaxValue( 0 );
  } // vxHistogramUpdater()
  

  /**
   * Updates the binary histograms per block and the base histogram
   */
  inline void Update()
  {
    vxBlockVolumeIterator< VoxelType > iter( m_rVolume );  
    for( ; iter.IsNotAtEnd(); iter.NextBlockZYX() )
    {
      vxBlock< VoxelType > & block( const_cast< vxBlock< VoxelType > & >( iter.GetCurrentBlock() ) );
      UpdateBlock( block, block.m_pBlockInfo->m_iRefCount );
    }
    Complete();
  } // Update()



  /**
   * Updates the base and intensity histogram for the specified block
   *
   * @param   block       block to update
   * @param   iRefCount   value multiplier
   */
  inline void UpdateBlock( vxBlock< VoxelType > & block, int4 iMultiplier = 1 )
  {
    const uint4 uVoxelsToProcess( block.IsHomogenous()? 1 : m_rVolume.GetVoxelsPerBlock() );
    
    int4 iBinaryHistogram( 0 );
    for( uint4 i( 0 ); i < uVoxelsToProcess; ++i )
    {
      // get the voxel value
      const int4 iVoxelValue( block.GetDataPtr()[ i ] );
      
      const int4 iVoxel( static_cast< int4 >(  iVoxelValue ) & m_iMask );
      iBinaryHistogram |= ( 1 << ( iVoxel >> m_iShift ) );

      // update the min and max for the base histogram
      if( iVoxelValue < m_iMin ) m_iMin = iVoxelValue;
      if( iVoxelValue > m_iMax ) m_iMax = iVoxelValue;

      // update the base histogram
      if( iVoxelValue >= 0 && iVoxelValue < m_iMaxValue )
        *(m_pHistogram + iVoxelValue) += iMultiplier;
    }

    block.m_pBlockInfo->m_iBinaryHistogram = iBinaryHistogram;

  } // Update( vxBlock< VoxelType > & block )

  
  /**
   * Completes the update operation
   */
  inline void Complete()
  {
    // update the min and max value for the base histogram
    m_rVolume.GetHistogram().SetMinValue( m_iMin );
    m_rVolume.GetHistogram().SetMaxValue( m_iMax );

    // subtract any extra from histogram as blocks are 16x16x16
    const Triple< uint4 > & voldim = m_rVolume.GetHeader().GetVolDim();
    Triple< uint4 > actualdim( ( voldim.m_x + 15 ) / 16, ( voldim.m_y + 15 ) / 16, ( voldim.m_z + 15 ) / 16 );
    const int4 iDifference( ( actualdim.m_x * actualdim.m_y * actualdim.m_z ) - ( voldim.m_x * voldim.m_y * voldim.m_z ) );

    /// suntract from the background value
    *(m_pHistogram + m_rVolume.GetHeader().GetBackground() ) -= iDifference;
  } // Complete()



  /**
   * Static updater call
   *
   * @param   volume    volume to update histogram
   */
  static inline void Update( vxBlockVolume< VoxelType > & volume )
  {
    vxHistogramUpdaterIntensity< VoxelType > updater( volume );
    updater.Update();
  } // Update( vxBlockVolume< VoxelType > & volume )

// member variables
private:

  /// volume
  vxBlockVolume< VoxelType > & m_rVolume;

  /// binary histogram mask
  int4 m_iMask;

  /// binary histogram shift
  int4 m_iShift;

  /// base histogram
  uint4 * m_pHistogram;

  /// maximum allowable voxel value
  int4 m_iMaxValue;

  /// minimum voxel value
  int4 m_iMin;
  
  /// maximum voxel value
  int4 m_iMax;

}; // class vxHistogramUpdater


// $Log: vxHistogramUpdaterIntensity.h,v $
// Revision 1.6  2007/03/01 19:04:37  geconomos
// code review preparation
//
// Revision 1.5  2006/05/31 16:16:42  geconomos
// porting to visual studio 2005
//
// Revision 1.4  2006/04/03 20:01:15  geconomos
// fixed Complete method
//
// Revision 1.3  2006/04/03 18:46:27  geconomos
// added Complete method that is to be called once the histogram calculation is complete
//
// Revision 1.2  2006/04/03 17:01:51  geconomos
// corrected header
//
// Revision 1.1  2006/04/03 16:44:26  geconomos
// initial revision
//
// Revision 1.2  2006/03/29 21:00:50  geconomos
// added comments
//
// Revision 1.1  2006/03/29 19:46:44  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxHistogramUpdaterIntensity.h,v 1.6 2007/03/01 19:04:37 geconomos Exp $
// $Id: vxHistogramUpdaterIntensity.h,v 1.6 2007/03/01 19:04:37 geconomos Exp $
