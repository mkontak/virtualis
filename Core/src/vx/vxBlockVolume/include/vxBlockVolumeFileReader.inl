// $Id: vxBlockVolumeFileReader.inl,v 1.5 2007/03/01 19:04:37 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)



/**
 * Deinterlaces 16-bit voxel data
 *
 * @param   pDestCurrent  destination buffer
 * @param   pSrcCurrent   sourcebuffer
 */
template< typename VoxelType, template <typename> class HistogramUpdater >
void vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::Deinterleave12( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent ) const
{
  uint1 * pTmp( pDestCurrent );
  for( int i( 0 ); i < 4096; ++i )
  {
    *pTmp= *pSrcCurrent++;
    pTmp += 2;
  }

  for( int i( 0 ); i < 4096/2; ++i )
  {
    *( pDestCurrent + 1 ) = ( * pSrcCurrent & 0xF0 ) >> 4;
    *( pDestCurrent + 3 ) = ( * pSrcCurrent & 0x0F );
    pSrcCurrent++;
    pDestCurrent += 4;
  }
} // Deinterleave12( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent ) const


/**
 * Deinterlaces 16-bit voxel data
 *
 * @param   pDestCurrent  destination buffer
 * @param   pSrcCurrent   sourcebuffer
 */
template< typename VoxelType, template <typename> class HistogramUpdater >
void vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::Deinterleave16( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent  ) const
{
  uint1 * pOffset( pDestCurrent );
  
  for( int4 i( 0 ); i < 4096; ++i )
  {
    *pDestCurrent = *pSrcCurrent++;
    pDestCurrent += 2;
  }

  pDestCurrent = pOffset + 1;
  for( int4 j( 0 ); j < 4096; ++j )
  {
    *pDestCurrent = *pSrcCurrent++;
    pDestCurrent += 2;
  }
} // Deinterleave16( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent  ) const


// $Log: vxBlockVolumeFileReader.inl,v $
// Revision 1.5  2007/03/01 19:04:37  geconomos
// code review preparation
//
// Revision 1.4  2006/04/11 13:37:26  geconomos
// commenting
//
// Revision 1.3  2006/04/07 19:32:34  geconomos
// expanded to handle previous "volb" data chunk
//
// Revision 1.2  2006/04/03 16:46:34  geconomos
// + added  histogram updater as template argument
//
// Revision 1.1  2006/03/29 19:46:44  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFileReader.inl,v 1.5 2007/03/01 19:04:37 geconomos Exp $
// $Id: vxBlockVolumeFileReader.inl,v 1.5 2007/03/01 19:04:37 geconomos Exp $
