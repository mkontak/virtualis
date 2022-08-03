// $Id: MemoryBufferPool.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#pragma once

// include declarations
#include "MemoryBuffer.h"
#include "CriticalSection.h"
#include "SingleLock.h"

// class declaration
class VX_BASE_DLL MemoryBufferPool
{
// member functions
public:
  
  /// constructor
  MemoryBufferPool();

  /// constructor
  MemoryBufferPool( const int4 iPoolSize, const int4 iBufferSize );
  
  /// destructor
  virtual ~MemoryBufferPool();

  /// initializes the buffer pool
  void Initialize( const int4 iPoolSize, const int4 iBufferSize );

  /// gets the next available buffer (waits for one to become available if needed)
  MemoryBuffer & GetBuffer();

  /// recycles the specified buffer
  void RecycleBuffer( MemoryBuffer & buffer );

  /// gets the size of the buffer pool
  inline int4 GetPoolSize() { return static_cast< int4 >( m_buffers.size() ); }

private:

  /// gets the next available buffer
  inline MemoryBuffer * GetNextAvailableBuffer();

// member variables
private:

  /// memory buffers ( stored as pairs where first = indicates if buffer is available; second = memoryBuffer * )
  std::vector< std::pair< bool, MemoryBuffer * > > m_buffers;

  /// for thread saftey
  CriticalSection m_criticalSection;

  /// event used to signal availability of MemoryBuffers
  HANDLE m_hRecycledEvent;

}; // class MemoryBufferPool

#include "MemoryBufferPool.inl"

// $Log: MemoryBufferPool.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:21  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/MemoryBufferPool.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: MemoryBufferPool.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
