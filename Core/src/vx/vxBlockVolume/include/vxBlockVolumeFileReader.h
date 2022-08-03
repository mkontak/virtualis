// $Id: vxBlockVolumeFileReader.h,v 1.5 2007/03/01 19:04:37 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declarations
#pragma once

// include declarations
#include "V3DFile.h"
#include "vxBlockVolume.h"
#include "Thread.h"
#include "MemoryBufferPool.h"
#include "ThreadSafeQueue.h"
#include "vxHistogramUpdaterNull.h"

// class declaration
template< typename VoxelType, template < typename > class HistogramUpdater = vxHistogramUpdaterNull >
class VX_BLOCK_VOLUME_DLL vxBlockVolumeFileReader : private V3DFile
{
// member functions
public:
  
  /// reads the specified volume
  static void Read( const std::string & sFilename, vxBlockVolume< VoxelType > & volume, std::pair< float4, float4 > & progress = std::make_pair( -1.0F, -1.0F ) );

protected:

  /// override to handle reading of data chunk (contains voxel data)
  virtual void ReadDataChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize, const std::string & sChunkType );

private:

  /// constructor
  vxBlockVolumeFileReader();
  
  /// destructor
  virtual ~vxBlockVolumeFileReader();

  /// internal read
  void ReadInternal( const std::string & sFilename, vxBlockVolume< VoxelType > & volume, std::pair< float4, float4 > & progress = std::make_pair( -1.0F, -1.0F ) );

  /// deinterlaces 12-bit voxel data
  inline void Deinterleave12( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent ) const;

  /// deinterlaces 16-bit voxel data
  inline void Deinterleave16( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent ) const;

  /// thread responsible for inflating compressed samples parallel to disk io
  class InflaterThread : public Thread 
  {   
  // member functions
  public: 
    
    /// sets the file reader
    void SetReader( vxBlockVolumeFileReader< VoxelType, HistogramUpdater > * pReader ) { m_pReader = pReader; }

    /// run function ( runs in separate thread )
    void Run(); 
  
  // member variables
  private:
    
    /// parent reader
    vxBlockVolumeFileReader< VoxelType, HistogramUpdater > * m_pReader;

  }; // class InflaterThread

  /// data passed from io reader to inflater thread
  struct QueueItem
  {
    /// queue item type
    enum QueueItemTypeEnum { CHUNK, BLOCK } m_eType;
    
    /// contains compressed data
    MemoryBuffer * m_pBuffer;
    
    /// size of data
    uint4 m_uBufferSize;
  
    /// consturctor for setting all structs members
    QueueItem( QueueItemTypeEnum  eType, MemoryBuffer * pBuffer, uint4 uBufferSize ) : m_eType( eType ), m_pBuffer( pBuffer ), m_uBufferSize( uBufferSize )  { };
  
  }; // struct QueueItem

  /// deinterleave mode
  enum DeinterleaveEnum { DEINTERLEAVE_NONE, DEINTERLEAVE_12, DEINTERLEAVE_16 };

// member variables
private:

  /// destination volume
  vxBlockVolume< VoxelType > * m_pVolume;

  /// deinterlacing mode
  DeinterleaveEnum m_eDeinterleaveMode;

  /// number of voxels per block ( read from file )
  uint4 m_uNumberOfVoxelsPerBlock;
  
  /// number of unique blocks( read from file )
  uint4 m_uNumberOfUniqueBlocks;

  /// array of blocks
  vxBlock< VoxelType > * m_pUniqueBlocks;

  /// number of block indicies
  uint4 m_uNumberOfIndices;

  /// index array
  int4 * m_pIndices;

  /// array of reference counts for unique blocks
  uint2 * m_pReferenceCounts;

  /// buffer pool
  MemoryBufferPool m_bufferPool;

  /// thread safe queue
  ThreadSafeQueue< QueueItem, CriticalSection > m_queue;

  /// inflater thread
  InflaterThread m_inflaterThread;

  /// signaled when file io has complete
  HANDLE m_hFileReadComplete;
  
  /// signaled if the inflater thread is to be aborted
  HANDLE m_hAbort;
  
  /// signaled when an item is added to the queue
  HANDLE m_hQueueItemAdded;

  /// progress range
  std::pair< float4, float4 > m_progress;

  /// number of blocks per chunk
  static uint4 m_uNumberOfBlocksPerChunk;

}; // class vxBlockVolumeFileReader


#include "vxBlockVolumeFileReader.inl"

// $Log: vxBlockVolumeFileReader.h,v $
// Revision 1.5  2007/03/01 19:04:37  geconomos
// code review preparation
//
// Revision 1.4  2006/04/11 13:37:50  geconomos
// + made constructor private
// + added static Read function
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
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFileReader.h,v 1.5 2007/03/01 19:04:37 geconomos Exp $
// $Id: vxBlockVolumeFileReader.h,v 1.5 2007/03/01 19:04:37 geconomos Exp $
