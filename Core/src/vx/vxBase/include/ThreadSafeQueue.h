// $Id: ThreadSafeQueue.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
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
#include "CriticalSection.h"
#include "SingleLock.h"

// template definition
template< typename ItemType, class SyncObject >
class ThreadSafeQueue
{
//  member functions
public:
  
  /**
   * Constructor
   */
  inline ThreadSafeQueue()
  {
  } // ThreadSafeQueue()
  
 
  /**
   * Destructor
   */
  inline virtual ~ThreadSafeQueue()
  {
  } // ~ThreadSafeQueue()

  
  /**
   * Adds an item to the end of the queue
   *
   * @param    item      item to add
   */
  inline void Enqueue( ItemType & item )
  {
    SingleLock< SyncObject > lock( &m_syncobject, true ); 
    m_queue.push( item );
  } // Enqueue( ItemType & item );


  /**
   * Removes an item from the front of the queue
   *
   * @return  item from front of queue
   */
  inline ItemType & Dequeue()
  {
    SingleLock< SyncObject > lock( &m_syncobject, true ); 
    ItemType & item = Peek();
    m_queue.pop();
    return item;
  } // Dequeue()

  /**
   * Returns the first item in the queue
   *
   * @return first item in the queue
   */
  inline ItemType & Peek()
  {
    SingleLock< SyncObject > lock( &m_syncobject, true ); 
    return m_queue.front();
  } // Peek()

  
  /**
   * Gets if the queue is empty
   *
   * @return  true if queue is empty
   */
  inline bool IsEmpty()
  {
    SingleLock< SyncObject > lock( &m_syncobject, true ); 
    return m_queue.empty();
  } // IsEmpty()


  /** 
   * Clears all items from the queue
   */
  inline void Clear()
  {
    SingleLock< SyncObject > lock( &m_syncobject, true ); 
    while( !m_queue.empty() )
      m_queue.pop();
  } // Clear()


  /// member variables
private:

  /// underlying queue
  std::queue< ItemType > m_queue;

  /// thread sync object
  SyncObject m_syncobject;

}; // class ThreadSafeQueue


// $Log: ThreadSafeQueue.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:21  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ThreadSafeQueue.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: ThreadSafeQueue.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
