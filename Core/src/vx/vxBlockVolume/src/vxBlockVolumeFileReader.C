// $Id: vxBlockVolumeFileReader.C,v 1.8 2007/03/01 19:04:36 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "StdAfx.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"
#include "vxHistogramUpdaterLabel.h"
#include "vxIntensityConverter.h"
#include "CRC.h"
#include "vxInflate.h"
#include "Timer.h"
#include "vxProgressbar.h"

#define FILE_REVISION "$Revision: 1.8 $"

template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< int1,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< int2,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< int4,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint1,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint2,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint4,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< bool,  vxHistogramUpdaterIntensity >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<float4, vxHistogramUpdaterIntensity >;

template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< int1 >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< int2 >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< int4 >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint1 >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint2 >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint4 >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader< bool >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<float4 >;

template class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader<uint2,  vxHistogramUpdaterLabel  >;



// 32 blocks per chunk
template< typename VoxelType, template <typename > class HistogramUpdater >
uint4 vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::m_uNumberOfBlocksPerChunk =  32 ;


/**
 * Constructor
 */
template< typename VoxelType, template < typename > class HistogramUpdater >
vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::vxBlockVolumeFileReader() : 
  m_uNumberOfVoxelsPerBlock( 0 ),
  m_uNumberOfUniqueBlocks( 0 ),
  m_pUniqueBlocks(NULL ),
  m_uNumberOfIndices( 0 ),
  m_pIndices( NULL ),
  m_pReferenceCounts( NULL ),
  m_eDeinterleaveMode( DEINTERLEAVE_NONE )
{
  m_hFileReadComplete = CreateEvent( NULL, TRUE, FALSE, NULL );  
  m_hAbort = CreateEvent( NULL, TRUE, FALSE, NULL );
  m_hQueueItemAdded = CreateEvent( NULL, TRUE, FALSE, NULL );
} // vxBlockVolumeFileReader()


/**
 * Destructor
 */
template< typename VoxelType, template <typename> class HistogramUpdater >
vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::~vxBlockVolumeFileReader()
{
  if( m_pUniqueBlocks )
    delete[] m_pUniqueBlocks;

  if( m_pIndices )
    delete[] m_pIndices;

  if( m_pReferenceCounts )
    delete[] m_pReferenceCounts;

  if( m_hFileReadComplete != NULL)
    CloseHandle( m_hFileReadComplete );
  
  if( m_hAbort != NULL)
    CloseHandle( m_hAbort );

  if( m_hQueueItemAdded != NULL)
    CloseHandle( m_hQueueItemAdded );

} // ~vxBlockVolumeFileReader()


/**
 * Reads the specified volume
 *
 * @param   sFilename     volume file name
 * @param   volume        volume to read in to
 * @param   progress      progress range
 */
template< typename VoxelType, template <typename> class HistogramUpdater >
void vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::ReadInternal( const std::string & sFilename, vxBlockVolume< VoxelType > & volume,  std::pair< float4, float4 > & progress )
{
  try
  {
    // cache inputs
    m_pVolume = &volume;
    m_progress = progress;
    
    // read the header
    volume.GetHeader().Read( sFilename );

    // check that this is indeed a block volume version 2.0 or greater
    const uint4 uCurrentMajorVersion( 2 );
    if( volume.GetHeader().GetMemoryLayoutEnum() != vxVolumeHeader::BLOCK || volume.GetHeader().GetFileVersion().first < uCurrentMajorVersion )
    {

      // default to old read
      LogWrn( "Volume not supported. Reverting to original vxBlockVolume::Read()", "vxBlockVolumeFileReader", "Read" );
      volume.Read( sFilename, progress );
      return;
    }

    // verify the voxels per block
    volume.GetHeader().GetVariableVolumeHeader().GetHdrVar( "uNumberOfVoxelsPerBlock",  m_uNumberOfVoxelsPerBlock );
    if( m_uNumberOfVoxelsPerBlock != vxBlockVolumeIteratorConsts::eL0VoxVoxelNum )
      throw ex::VException( LogRec( "Error reading" + sFilename + " reason: incorrect voxels per block", "vxBlockVolumeFileReader", "Read" ) );

    // read in the number of unique blocks
    volume.GetHeader().GetVariableVolumeHeader().GetHdrVar( "uNumberOfUniqueBlocks",    m_uNumberOfUniqueBlocks );

    /// allocate the unique blocks
    m_pUniqueBlocks = new vxBlock< VoxelType >[ m_uNumberOfUniqueBlocks ];
    for( int i( 0 ); i < m_uNumberOfUniqueBlocks; ++i )
      m_pUniqueBlocks[ i ].Resize( m_uNumberOfVoxelsPerBlock );

    // read in the size of the index array ( stored in bytes for some strange reason )
    uint4 uByteSizeOfIndexArray( 0 );
    volume.GetHeader().GetVariableVolumeHeader().GetHdrVar( "uByteSizeOfIndexArray", uByteSizeOfIndexArray );
    
    // create the index array
    m_uNumberOfIndices = uByteSizeOfIndexArray / 4;
    m_pIndices = new int4[  m_uNumberOfIndices ];

    // create the reference counts array
    m_pReferenceCounts =  new uint2[ m_uNumberOfUniqueBlocks ];
    memset( m_pReferenceCounts, 0, m_uNumberOfUniqueBlocks * 2 );

     // indicate that we are done with 5 percent of the reading
    const float4 fFivePercent( Interpolate( 0.0F, 5.0F, 100.0F, m_progress.first, m_progress.second ) );
    if ( m_progress.first >= 0 && m_progress.second >= 0 )
      vxProgressBar::SetProgressPercentage( fFivePercent );

    // rest of the work is done in ReadDataChunk() below
    V3DFile::Read( sFilename, progress );

    int4 * pTmpIndices( m_pIndices );
    vxBlockVolumeIterator< VoxelType > iterVol( volume );
    for( iterVol.SetPos(0,0,0) ; iterVol.IsNotAtEnd(); iterVol.NextBlockZYX(), ++pTmpIndices )
      iterVol.SetBlock( m_pUniqueBlocks[ *pTmpIndices ] );

    /// update the progress
    if( m_progress.first >= 0 && m_progress.second >= 0 )
      vxProgressBar::SetProgressPercentage( m_progress.second );

  }
  catch( ex::VException & )
  {
  }
  catch( ... )
  {
    LogErr( "Unhandled exception caught", "vxBlockVolumeFileReader", "Read" );
    throw;
  }
} // Read( const std::string & sFilename, vxBlockVolume< VoxelType > & volume )


/**
 * Overidden. Reads the data chunk containing the voxel data
 *
 * @param   instream      file input stream
 * @param   uChunkSize    sizeof the the data chunk in bytes
 * @param   sChunkType    chunk type identifier
 */
template< typename VoxelType, template <typename> class HistogramUpdater >
void vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::ReadDataChunk( std::shared_ptr<io::FilePtr>  & instream, const uint4 uChunkSize, const std::string & sChunkType )
{
  // check that this chunk type is handled
  if( sChunkType[ 3 ] != 'c'  && sChunkType[ 3 ] != 'b' )
  {
    LogWrn( "Data chunk type not supported. Reverting to original ReadDataChunk()", "vxBlockVolumeFileReader", "ReadDataChunk" );
    V3DFile::ReadDataChunk( instream, uChunkSize, sChunkType );
    return;
  }

  // see which deinterleaving method we are using ( only for 16 bit voxel types )
  if( sizeof( VoxelType ) == 2 )
  {
    if( sChunkType[ 3 ] == 'c' )
      m_eDeinterleaveMode = DEINTERLEAVE_16;

    else if( sChunkType[ 3 ] == 'b' ) 
      m_eDeinterleaveMode = DEINTERLEAVE_12;
  }

////////////////////////////////////////////////////////
//  initialize crc
////////////////////////////////////////////////////////

  CRC crc;
  uint4 uCheckSum( crc.UpdateCrc( crc.CrcInit(), ( uint1* )( sChunkType.c_str() ), 4 ) );


////////////////////////////////////////////////////////
//  initialize the memory buffer pool
////////////////////////////////////////////////////////

  // read in the block size ( bytes )
  uint4 uBlockByteSize( 0 );
  instream->Read( &uBlockByteSize, sizeof( uBlockByteSize ) );

  const uint4 uNumberOfBuffers( 50 );
  const uint4 uBufferSize( static_cast< uint4 >( ( uBlockByteSize + 4 ) * m_uNumberOfBlocksPerChunk * 1.1 ) ); 
  m_bufferPool.Initialize( uNumberOfBuffers, uBufferSize );
  

////////////////////////////////////////////////////////
//  read in the index array
////////////////////////////////////////////////////////
  
  // get compressed block size for the index array
  uint4 uCompressedBlockSize( 0 );
  instream->Read( &uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
      
  // read compressed index array
  MemoryBuffer & buffer = m_bufferPool.GetBuffer();
  instream->Read( buffer.GetDataPtr(), uCompressedBlockSize );

  // update the checksum
  uCheckSum = crc.UpdateCrc( uCheckSum, buffer.GetDataPtr(), uCompressedBlockSize );

  // inflate index array
  Inflate::InflateIt( hdr::LZO, buffer.GetDataPtr(), uCompressedBlockSize, reinterpret_cast< uint1 * >( m_pIndices ),  m_uNumberOfIndices * 4 );

  // read in unused int4 
  int4 iUnused( 0 );
  instream->Read( &iUnused, sizeof( iUnused ) );

  // return the buffer to the pool
  m_bufferPool.RecycleBuffer( buffer );

  // update the reference counts array
  for( int4 i( 0 ); i < m_uNumberOfIndices; ++i )
    m_pReferenceCounts [ m_pIndices[ i ] ] += 1;


////////////////////////////////////////////////////////
// read in blocks
////////////////////////////////////////////////////////

  // start the inflator thread
  m_inflaterThread.SetReader( this );
  m_inflaterThread.Start();
  
  // number of blocks to read
  const uint4 uNumberOfBlocksToRead( m_uNumberOfUniqueBlocks + 2 );

  // read 32-block chunks
  uint4 uBlocksRead( 2 );
  float4 fLastProgressPercentage( -1000.0F );
  while( ( uBlocksRead + m_uNumberOfBlocksPerChunk ) < uNumberOfBlocksToRead )
  {
    // get the next available memory buffer from the pool; this will block until one becomes avaiable
    MemoryBuffer & buffer = m_bufferPool.GetBuffer();
    
    // read in compressed chunk size
    uint4 uCompressedChunkSize( 0 );
    instream->Read( &uCompressedChunkSize, sizeof( uCompressedChunkSize ) );
    
    // read compressed chunk
    instream->Read( buffer.GetDataPtr(), uCompressedChunkSize );

    // update checksum
    uCheckSum = crc.UpdateCrc( uCheckSum, buffer.GetDataPtr(), uCompressedChunkSize );

    // add chunk to queue
    m_queue.Enqueue( QueueItem( QueueItem::CHUNK, &buffer, uCompressedChunkSize ) );

    /// indicate that an item has been added to the queue
    SetEvent( m_hQueueItemAdded );

    // increment blocks read 
    uBlocksRead += m_uNumberOfBlocksPerChunk;

    // update the progress bar
    if( m_progress.first >= 0 && m_progress.second >= 0 )
    {
      const float4 fPercentage( Interpolate( float4( m_uNumberOfBlocksPerChunk ), float4( uBlocksRead+m_uNumberOfBlocksPerChunk ),
                                             float4( uNumberOfBlocksToRead ), m_progress.first, m_progress.second ) );
      const float4 fPercentageThreshold( 5.0F );
      if ( abs( static_cast< int4 >( fPercentage - fLastProgressPercentage ) ) > fPercentageThreshold )
      {
        fLastProgressPercentage = fPercentage;
        vxProgressBar::SetProgressPercentage( fPercentage );
      }
    } // if updating the progress meter

  } // finished read of 32-block chunks 


  // read remaining blocks in last chunk( < 32 )
  for( ; uBlocksRead < uNumberOfBlocksToRead; ++uBlocksRead )
  {
    // get the next available memory buffer from the pool; this will block until one becomes avaiable
    MemoryBuffer & buffer = m_bufferPool.GetBuffer();

    // get compressed block size
    uint4 uCompressedBlockSize;
    instream->Read( &uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
    
    // read compressed block
    instream->Read( buffer.GetDataPtr(), uCompressedBlockSize );

    // update checksum
    uCheckSum = crc.UpdateCrc( uCheckSum, buffer.GetDataPtr(), uCompressedBlockSize );

    // read the flag into the end of the memory buffer
    instream->Read( buffer.GetDataPtr() + uCompressedBlockSize, sizeof( int4 ) );
    
    // add chunk to queue
    m_queue.Enqueue( QueueItem( QueueItem::BLOCK, &buffer, uCompressedBlockSize ) );

    /// indicate that an item has been added to the queue
    SetEvent( m_hQueueItemAdded );
            
  } // finished reading remaining blocks


////////////////////////////////////////////////////////
// verify the checksum
////////////////////////////////////////////////////////

  uCheckSum = crc.CrcComplement( uCheckSum );
  
  uint4 uSavedCheckSum( 0 );
  instream->Read( & uSavedCheckSum, sizeof( uSavedCheckSum ) );

  // do the checksums match?
  if( uSavedCheckSum != uCheckSum )
  {
    SetEvent( m_hAbort );
    throw ex::DataCorruptionException( LogRec( "Invalid checksum.", "vxBlockVolumeFileReader", "ReadDataChunk" ) );
  }

  // set the file_read_complete event signaled
  SetEvent( m_hFileReadComplete );

  // wait for the inflater thread to end
  m_inflaterThread.Wait();

} // ReadDataChunk( FilePtr & instream, const uint4 uChunkSize, const std::string & sChunkType )


/**
  * Inflater thread
  */
template< typename VoxelType, template <typename> class HistogramUpdater >
void vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::InflaterThread::Run()
{
  HistogramUpdater< VoxelType > updater( *m_pReader->m_pVolume );

  uint4 uVoxelsPerBlock( m_pReader->m_uNumberOfVoxelsPerBlock );
  if( m_pReader->m_eDeinterleaveMode == DEINTERLEAVE_12 )
    uVoxelsPerBlock = 3072;
  
  // voxel data size
  const uint4 uVoxelDataSize( uVoxelsPerBlock * sizeof( VoxelType ) );
  
  // uncompressed block size ( voxel data size plus 4 bytes for flag )
  const uint4 uUncompressedBlockSize(  uVoxelDataSize + 4  ); // + 4 for the flag
  
  // uncompressed chunk size
  const uint4 uUncompressedChunkSize( uUncompressedBlockSize * m_uNumberOfBlocksPerChunk );
  
  // index of the next unique block to read in to
  uint4 uCurrentUniqueBlockIndex( 0 );
  
  // scratch memory to inflate into
  MemoryBuffer scratchBuffer( uUncompressedChunkSize );

  // array containing the abort, file_read_complete and queue_item_added handles
  const int4 iNumberOfEvents( 3 );
  HANDLE handles[ iNumberOfEvents ] = { m_pReader->m_hAbort, m_pReader->m_hQueueItemAdded, m_pReader->m_hFileReadComplete };
  
  // wait for any of one the events to become signaled
  DWORD dwWaitRetval( 0 );
  while( dwWaitRetval = WaitForMultipleObjects( iNumberOfEvents, handles, FALSE, INFINITE ) )
  {
    // get the signaled event handle
    HANDLE hSignaledEvent( handles[ dwWaitRetval - WAIT_OBJECT_0 ] );

    // break if either the abort or file_read_complete event has been signaled
    if( hSignaledEvent == m_pReader->m_hAbort || hSignaledEvent == m_pReader->m_hFileReadComplete )
      break;

    // reset the queue_item_added event
    ResetEvent( m_pReader->m_hQueueItemAdded );

    // process the queue
    while( !m_pReader->m_queue.IsEmpty() )
    {
      // pop an item from the queue
      QueueItem item( m_pReader->m_queue.Dequeue() );

      // inflate the data
      if( item.m_eType == QueueItem::CHUNK )
      {
        Inflate::InflateIt( hdr::LZO, item.m_pBuffer->GetDataPtr(), item.m_uBufferSize, scratchBuffer.GetDataPtr(), uUncompressedChunkSize );

        uint1 * pSrc = scratchBuffer.GetDataPtr();

        for( int4 i( 0 ); i < m_uNumberOfBlocksPerChunk; ++i )
        {
          vxBlock< VoxelType > * pCurrentBlock( &m_pReader->m_pUniqueBlocks[ uCurrentUniqueBlockIndex ] );

          uint1 * pDst = reinterpret_cast< uint1 * >( pCurrentBlock->GetDataPtr() );
          
          if( m_pReader->m_eDeinterleaveMode == DEINTERLEAVE_16 )
          {
            m_pReader->Deinterleave16( pDst, pSrc );
          }
          else if( m_pReader->m_eDeinterleaveMode == DEINTERLEAVE_12 )
          {
           m_pReader->Deinterleave12( pDst, pSrc );
          }
          else
          {
            memcpy( pDst, pSrc, uVoxelDataSize );
            pSrc += uVoxelDataSize;
          }

          int4 iFlag = *pSrc;
          pSrc += 4;
          
          pCurrentBlock->m_pBlockInfo->m_bIsDirty =  (iFlag & 0x000000FF) == 0 ? false : true;
          pCurrentBlock->m_pBlockInfo->m_bIsHomogenous = (iFlag & 0x0000FF00) >> 8 == 0 ? false : true;
          updater.UpdateBlock( *pCurrentBlock, m_pReader->m_pReferenceCounts[ uCurrentUniqueBlockIndex++ ] );
        }
      }
      else
      {
        Inflate::InflateIt( hdr::LZO, item.m_pBuffer->GetDataPtr(), item.m_uBufferSize, scratchBuffer.GetDataPtr(), uUncompressedBlockSize );

        vxBlock< VoxelType > * pCurrentBlock( &m_pReader->m_pUniqueBlocks[ uCurrentUniqueBlockIndex ] );

        uint1 * pSrc = scratchBuffer.GetDataPtr();

        uint1 * pDst = reinterpret_cast< uint1 * >( pCurrentBlock->GetDataPtr() );
        
        if( m_pReader->m_eDeinterleaveMode == DEINTERLEAVE_16 )
        {
           m_pReader->Deinterleave16( pDst, pSrc );
        }
        else if( m_pReader->m_eDeinterleaveMode == DEINTERLEAVE_12 )
        {
           m_pReader->Deinterleave12( pDst, pSrc );
        }
        else
        {
          memcpy( pDst, pSrc, uVoxelDataSize );
          pSrc += uVoxelDataSize;
        }
        
        int4 iFlag = *( item.m_pBuffer->GetDataPtr() + item.m_uBufferSize );
        pCurrentBlock->m_pBlockInfo->m_bIsDirty =  (iFlag & 0x000000FF) == 0 ? false : true;
        pCurrentBlock->m_pBlockInfo->m_bIsHomogenous = (iFlag & 0x0000FF00) >> 8 == 0 ? false : true;

        updater.UpdateBlock( *pCurrentBlock, m_pReader->m_pReferenceCounts[ uCurrentUniqueBlockIndex++ ] );
      }
        
      // return the MemoryBuffer back to the pool
      m_pReader->m_bufferPool.RecycleBuffer( *item.m_pBuffer );

    } // while( !m_pReader->m_queue.IsEmpty() )
  } // while( dwWaitRetval = WaitForMultipleObjects( iNumberOfEvents, handles, FALSE, INFINITE ) )

  updater.Complete();

} // Run()


/**
 * Reads the specified volume
 *
 * @param   sFilename     volume file name
 * @param   volume        volume to read in to
 * @param   progress      progress range
 */
template< typename VoxelType, template <typename> class HistogramUpdater >
void vxBlockVolumeFileReader< VoxelType, HistogramUpdater >::Read( const std::string & sFilename, vxBlockVolume< VoxelType > & volume,  std::pair< float4, float4 > & progress )
{
  vxBlockVolumeFileReader< VoxelType, HistogramUpdater > reader;
  reader.ReadInternal( sFilename, volume, progress );
} // Read( const std::string & sFilename, vxBlockVolume< VoxelType > & volume,  std::pair< float4, float4 > & progress )
  
// $Log: vxBlockVolumeFileReader.C,v $
// Revision 1.8  2007/03/01 19:04:36  geconomos
// code review preparation
//
// Revision 1.7  2006/05/30 12:48:58  geconomos
// exported vxBlockVolumeReader with "null" histogram updater
//
// Revision 1.6  2006/04/11 13:38:18  geconomos
// + made constructor private
// + added static Read function
//
// Revision 1.5  2006/04/07 19:32:34  geconomos
// expanded to handle previous "volb" data chunk
//
// Revision 1.4  2006/04/03 18:47:14  geconomos
// call to histogram updater complete
//
// Revision 1.3  2006/04/03 16:47:49  geconomos
// + added  histogram updater as template argument
//
// Revision 1.2  2006/03/30 15:12:59  geconomos
// enabled precompiled headers
//
// Revision 1.1  2006/03/29 19:46:24  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFileReader.C,v 1.8 2007/03/01 19:04:36 geconomos Exp $
// $Id: vxBlockVolumeFileReader.C,v 1.8 2007/03/01 19:04:36 geconomos Exp $
