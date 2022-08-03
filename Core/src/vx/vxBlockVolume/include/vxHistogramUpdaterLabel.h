// $Id: vxHistogramUpdaterLabel.h,v 1.5 2007/03/01 19:04:37 geconomos Exp $
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

// include declaration
#include "vxBlockVolume.h"
#include "vxIntensityConverter.h"
#include "vxBlockVolumeIterator.h"

// template class definition
template< typename VoxelType >
class vxHistogramUpdaterLabel
{
// member functions
public:

  /**
   * Constructor
   *
   * @param    volume    volume to generate histogram for
   */
  vxHistogramUpdaterLabel( vxBlockVolume< VoxelType > & volume ) : m_rVolume( volume )
  {
  } // vxHistogramUpdaterLabel
  
  
  /**
   * Updates the components binary histograms for all blocks contained within the specified volume
   *
   * @param   volume   label volume to update
   */
  inline void  Update()
  {
    vxBlockVolumeIterator< VoxelType > iter( m_rVolume );  
    for( ; iter.IsNotAtEnd(); iter.NextBlockZYX() )
      UpdateBlock( const_cast< vxBlock< VoxelType > & >( iter.GetCurrentBlock() ) );

    Complete();
  } // UpdateLabelHistogram( vxBlockVolume< VoxelType > & volume  )

  
  /** 
   * Updates the binary histogram for the specified block
   */
  inline void UpdateBlock( vxBlock< VoxelType > & block, int4 iMultiplier = 1 )
  {
    block.m_pBlockInfo->m_iBinaryHistogram = 0;

    if( block.IsHomogenous() )
    {
      block.m_pBlockInfo->m_iBinaryHistogram = ( 1 << min( block.GetDataPtr()[ 0 ], 31 ) );
    }
    else
    {
      for( uint4 i( 0 ); i < m_rVolume.GetVoxelsPerBlock(); ++i )
      {
        const int4 iLabel( min( block.GetDataPtr()[ i ], 31 ) );
        block.m_pBlockInfo->m_iBinaryHistogram |= 1 <<  iLabel;
      }
    }
  } // UpdateLabelBlock( vxBlock< VoxelType > & block, const uint4 uVoxelsPerBlock )


  /**
   * Completes the update operation
   */
  inline void Complete()
  {
  } // Complete()

  /**
   * Static updater call
   *
   * @param   volume    volume to update histogram
   */
  template< typename VoxelType >
  static inline void Update( vxBlockVolume< VoxelType > & volume )
  {
    vxHistogramUpdaterLabel< VoxelType > updater( volume );
    updater.Update();
  } // Update( vxBlockVolume< VoxelType > & volume )

// member variables
private:

  // underlying volume
  vxBlockVolume< VoxelType > & m_rVolume;

}; // class vxHistogramUpdaterLabel


// $Log: vxHistogramUpdaterLabel.h,v $
// Revision 1.5  2007/03/01 19:04:37  geconomos
// code review preparation
//
// Revision 1.4  2006/05/31 16:16:42  geconomos
// porting to visual studio 2005
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
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxHistogramUpdaterLabel.h,v 1.5 2007/03/01 19:04:37 geconomos Exp $
// $Id: vxHistogramUpdaterLabel.h,v 1.5 2007/03/01 19:04:37 geconomos Exp $
