// $Id: MemoryBufferPool.C,v 1.3 2007/03/07 19:55:26 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "StdAfx.h"
#include "MemoryBufferPool.h"
#include "SingleLock.h"


/**
 * Constructor
 */
MemoryBufferPool::MemoryBufferPool()
{
  // create event that is signaled when a MemoryBuffer is return to the pool
  m_hRecycledEvent = CreateEvent( NULL, FALSE, FALSE, NULL) ;

} // MemoryBufferPool( )


/**
 * Constructor
 *
 * @param   iPoolSize     number of memory buffers in the pool
 * @param   iBufferSize   size in bytes of memory buffer
 */
MemoryBufferPool::MemoryBufferPool( const int4 iPoolSize, const int4 iBufferSize )
{
  // create event that is signaled when a MemoryBuffer is return to the pool
  m_hRecycledEvent = CreateEvent (NULL, TRUE, FALSE, NULL) ;

  Initialize( iPoolSize, iBufferSize );

} // MemoryBufferPool( int4 iPoolSize , int4 iBufferSize )


/**
 * Destructor
 */
MemoryBufferPool::~MemoryBufferPool()
{
  // free buffers
  for( int i( 0 ); i < m_buffers.size(); ++i )
    delete m_buffers[ i ].second;

  // free event
  if( m_hRecycledEvent != NULL )
    DeleteObject( m_hRecycledEvent );
} // ~MemoryBufferPool()


/**
 * Initializes the buffer pool
 *
 * @param   iPoolSize     number of memory buffers in the pool
 * @param   iBufferSize   size in bytes of memory buffer
 */
void MemoryBufferPool::Initialize( const int4 iPoolSize, const int4 iBufferSize )
{
  // reserve initial pool size
  m_buffers.reserve( iPoolSize );

  // populate the vector 
  for( int i( 0 ); i <= iPoolSize; ++i )
    m_buffers.push_back( std::make_pair( true, new MemoryBuffer( iBufferSize ) ) );

} // Initialize( int4 iPoolSize, int4 iBufferSize )


/**
 * Gets the next available buffer (waits for one to become available if needed)
 *
 * @return    next available memory buffer
 */
MemoryBuffer & MemoryBufferPool::GetBuffer()
{
  // lock for thread safety
  SingleLock< CriticalSection > lock( &m_criticalSection, true );

  // grab the next available buffer
  MemoryBuffer * pBuffer( NULL );
  while( ( pBuffer = GetNextAvailableBuffer() ) == NULL )
  {
    // release the lock and wait
    lock.Unlock();

    // wait for a buffer to be recycled
    WaitForSingleObject( m_hRecycledEvent, INFINITE );

    // event has been signaled; reacquire the lock
    lock.Lock();
  } // if( pBuffer == NULL )

  // return a reference to the memory buffer object
  return *pBuffer;
} // GetBuffer()


/**
 * Recycles the specified buffer
 *
 * @param       buffer            memory buffer being recycled
 */
void MemoryBufferPool::RecycleBuffer( MemoryBuffer & buffer )
{
  // lock for thread safety
  SingleLock< CriticalSection > lock( &m_criticalSection, true );

  // iterate of all buffers to find a match
  for( int i( 0 ); i < m_buffers.size(); ++i )
  {
    // compare memoryBuffer addresses for a match
    if( m_buffers[ i ].second == &buffer )
    {
      // indicate the buffer is avaiable
      m_buffers[ i ].first = true;
      PulseEvent( m_hRecycledEvent );
      break;
    }
  } // for( int i( 0 ); i < m_buffers.size(); ++i )
} // RecycleBuffer( MemoryBuffer & buffer )



// $Log: MemoryBufferPool.C,v $
// Revision 1.3  2007/03/07 19:55:26  gdavidson
// Code review
//
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:36  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/MemoryBufferPool.C,v 1.3 2007/03/07 19:55:26 gdavidson Exp $
// $Id: MemoryBufferPool.C,v 1.3 2007/03/07 19:55:26 gdavidson Exp $
