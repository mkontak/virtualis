// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ThreadTask.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "ThreadTask.h"
#include "Thread.h"
#include <exception>


class InternalThread : public Thread
{
public:

  /// constructor
  InternalThread( ThreadTask::RunMethod run ) :
    m_runPtr( run )
  {
    m_waitEvent = std::shared_ptr< void >( CreateEvent( NULL, FALSE, FALSE, NULL ), []( HANDLE handle )
    {
      SetEvent( handle );
      CloseHandle( handle );
    });
  }

protected:

  /// run method
  void Run()
  {
    if( m_runPtr )
    {
      try
      {
        m_runPtr();
      }
      catch( ... )
      {
        m_exception = std::current_exception();
      }

      SetEvent( m_waitEvent.get() );
      if( m_callbackPtr )
        m_callbackPtr();
    }
  } // Run
	 
public:

  /// run method fucntion pointer
  ThreadTask::RunMethod m_runPtr;

  /// callback fucntion pointer 
  ThreadTask::TaskCallback m_callbackPtr;

  /// exception object
  std::exception_ptr m_exception;

  /// wait event
  std::shared_ptr< void > m_waitEvent;

}; // InternalThread


/**
 * constructor
 *
 * @param       run             run function pointer
 */
ThreadTask::ThreadTask( ThreadTask::RunMethod run ) :
  m_pimpl( std::make_shared< InternalThread >( run ))
{
} // ThreadTask


/**
 * Starts the task
 *
 * @param         callback            callback method
 */
void ThreadTask::Begin( TaskCallback callback )
{
  m_pimpl->m_callbackPtr = callback;
  m_pimpl->Start();
} // Begin( callback )


/**
 * ends the task
 */
void ThreadTask::End()
{
  WaitForSingleObject( m_pimpl->m_waitEvent.get(), INFINITE );
  if( !( m_pimpl->m_exception == nullptr ))
    std::rethrow_exception( m_pimpl->m_exception );
} // End()