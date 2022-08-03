// $Id: ThreadSafeQueue.inl,v 1.1 2006/03/29 19:34:21 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (geconomos@viatronix.com)

/**
 * Constructor
 **/
template< typename T >
ThreadSafeQueue<T>::ThreadSafeQueue()
{
} // ThreadSafeQueue()


/**
 * Destructor
 */
template< typename T >
ThreadSafeQueue<T>::~ThreadSafeQueue()
{
} // ~ThreadSafeQueue()


/// adds an item to the end of the queue
template< typename T >
void ThreadSafeQueue<T>::Enqueue( T & item )
{
} // Endqueue( T & item )


/// removes an item from the fron of the queue
template< typename T >
T & ThreadSafeQueue<T>::Dequeue()
{
  SingleLock< CriticalSection > lock( &m_criticalSection, true );
  T & item = Peek();
  m_queue.pop();
  return item;
} // Dequeue()


/// returns the first  item in the queue
template< typename T >
T & ThreadSafeQueue<T>::Peek()
{
  SingleLock< CriticalSection > lock( &m_criticalSection, true ); 
  return m_queue.front();
} // Peek()

/// clears all items from the queue
template< typename T >
bool ThreadSafeQueue<T>::IsEmpty()
{
  SingleLock< CriticalSection > lock( &m_criticalSection, true ); 
  return m_queue.empty();
} // Count()


/// clears all items from the queue
template< typename T >
void ThreadSafeQueue<T>::Clear()
{
  SingleLock< CriticalSection > lock( &m_criticalSection, true ); 
  while( !m_queue.empty() )
    m_queue.pop();
} // Clear()


// $Log: ThreadSafeQueue.inl,v $
// Revision 1.1  2006/03/29 19:34:21  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ThreadSafeQueue.inl,v 1.1 2006/03/29 19:34:21 geconomos Exp $
// $Id: ThreadSafeQueue.inl,v 1.1 2006/03/29 19:34:21 geconomos Exp $
