// $Id: Thread.C,v 1.6 2006/08/23 17:31:26 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

// includes
#include "stdafx.h"
#include "thread.h"
#include <exception>


// defines
#define FILE_REVISION "$Revision: 1.6 $"

// class definition
class ThreadImpl
{
// functions
public:
    
  /// default constructor
  ThreadImpl();  
  
  /// destructor
  virtual ~ThreadImpl();

  /// terminates the thread
  void Terminate();

  /// suspends the thread
  void Suspend();

  /// resumes the thread
  void Resume();

  /// gets the thread handle
  HANDLE GetThreadHandle() const { return m_hThread; }
  
  /// gets the thread id
  uint4 GetThreadId() const { return m_uThreadId; }

  /// run function
  static void internalRun(Thread*);

// data
public:
  
  // thread handle
  HANDLE m_hThread;
  
  // thread indentifier
  uint4 m_uThreadId;
  
  // indicates is thread has finished running.
  bool m_bFinished;
  
  // indicates if thread is currenty running.
  bool m_bRunning;

	// exception ptr
	std::exception_ptr m_exceptionPtr;

}; // class ThreadImpl


/**
 * Default constructor
 */
ThreadImpl::ThreadImpl()
:m_hThread(0),
m_bRunning(false),
m_bFinished(false),
m_exceptionPtr(NULL)
//*******************************************************************
{
} // ThreadImpl()


/**
 * Desturctor
 */
ThreadImpl::~ThreadImpl()
//*******************************************************************
{
  if (m_bRunning && !m_bFinished) 
  {
    LogWrn("Thread object destroyed while thread is still m_bRunning. Terminating thread...","ThreadImpl", "~ThreadImpl");
	  TerminateThread(m_hThread, 0);
	  m_bFinished = true;
  } 
  else if (m_hThread && !CloseHandle(m_hThread)) 
  {
    LogWrn("Thread destroy failure","ThreadImpl", "~ThreadImpl");
  }
} // destructor


/**
 * Terminates the thread.
 */
void ThreadImpl::Terminate()
//*******************************************************************
{
  TerminateThread(m_hThread ,0);
}


/**
 * Suspends the thread.
 */
void ThreadImpl::Suspend()
//*******************************************************************
{
  SuspendThread(m_hThread  );
} // Suspend()


/**
 * Resumes the thread.
 */
void ThreadImpl::Resume()
//*******************************************************************
{
  ResumeThread(m_hThread  );
} // Suspend()

/**
 * Run function
 * @param thread instance 
 */
void ThreadImpl::internalRun(Thread * pThread)
//*******************************************************************
{
	try
	{
		pThread->Run();
	}
	catch( ... )
	{
		pThread->m_pimpl->m_exceptionPtr = std::current_exception();
	}
  pThread->m_pimpl->m_bFinished = true;
  pThread->m_pimpl->m_bRunning = false;
} // internalRun()


/**
 * Thread Function for __beginthreadex
 * @param thread
 * @param error id
 */
extern "C"
static unsigned int __stdcall start_thread(void * pThread)
//*******************************************************************
{
  ThreadImpl::internalRun(reinterpret_cast<Thread*>(pThread));
  return 0;
} // start_thread()

 
/**
 * Default constructor
 */
Thread::Thread()
//*******************************************************************
{
  m_pimpl = new ThreadImpl;
} // Thread()


/**
 * Destructor
 */
Thread::~Thread()
//*******************************************************************
{
  if (m_pimpl->m_bRunning && !m_pimpl->m_bFinished) 
  {
	  LogWrn("Thread object destroyed while thread is still m_bRunning.", "Thread", "~Thread");
  } 
  else 
  {
    delete m_pimpl;
  }
} // destructor


/**
 * Starts execution of the thread.
 */
void Thread::Start()
//*******************************************************************
{
  // Is thread already running?
  if (m_pimpl->m_bRunning && !m_pimpl->m_bFinished) 
  {
	  LogWrn("Thread is already m_bRunning", "Thread", "Start");
	  Wait();
  }

  // Update thread state
  m_pimpl->m_bRunning  = true;
  m_pimpl->m_bFinished = false;
  m_pimpl->m_hThread   = 
      reinterpret_cast<HANDLE>(_beginthreadex(NULL, NULL, start_thread,this, 0, &(m_pimpl->m_uThreadId)));

  // Was the thread created successfully?
  if (m_pimpl->m_hThread == NULL)
  {
    LogWrn("Couldn't create thread", "Thread", "Start");
  }
} // Start()


/**
 * Blocks exection of the current thread until this tread has completed its work (will propagate any exception thrown on the thread)
 */
void Thread::End()
{
	Wait();
	if( !( m_pimpl->m_exceptionPtr == NULL ))
		std::rethrow_exception( m_pimpl->m_exceptionPtr );
} // End()


/**
 * Terminates the thread.
 */
void Thread::Terminate()
//*******************************************************************
{
  m_pimpl->Terminate();

  // Update thread state
  m_pimpl->m_bRunning  = false;
  m_pimpl->m_bFinished = true;
  m_pimpl->m_hThread   = NULL;
}


/**
 * Suspends the thread.
 */
void Thread::Suspend()
//*******************************************************************
{
  m_pimpl->Suspend();
} // Suspend()


/**
 * Resumes the thread.
 */
void Thread::Resume()
//*******************************************************************
{
  m_pimpl->Resume();
} // Suspend()

/**
 * Blocks execution of the current thread until this thread becomes signaled or
 * the specified amount of time has elapsed.
 * @param time to wait in milliseconds
 * @return true if successful, false otherwise
 */
bool Thread::Wait(uint4 uTime)
//*******************************************************************
{
  // Are we waiting on ourself?
  if (m_pimpl->m_uThreadId == GetCurrentThreadId()) 
  {
	  LogWrn("Thread tried to wait on itself", "Thread", "Wait");
	  return false;
  }
  
  // Are we still running?
  if(m_pimpl->m_bFinished == true || !m_pimpl->m_bRunning)
  {
	  return true;
  }
    
  // block on this thread
  switch(WaitForSingleObject(m_pimpl->m_hThread, uTime)) 
  {
  
  case WAIT_TIMEOUT: // time ellapsed
	  return false;
  case WAIT_ABANDONED_0: // error
  case WAIT_FAILED:
    LogWrn("Thread wait failure", "Thread", "Wait");
    return false;
  default:
    break;
  }
  
  return true;
} // Wait()


/**
 * Returns true if the thread is currently running.
 * @return true if the thread is currently running, false otherwise.
 */
bool Thread::IsRunning() const
//*******************************************************************
{
  return m_pimpl->m_bRunning;
} // IsRunning()


/**
 * Returns true if the thread has finished running.
 * @return true if the thread is finished, false otherwise.
 */
bool Thread::IsFinished() const
//*******************************************************************
{
  return m_pimpl->m_bFinished;
} // IsFinished()


/**
 * Aborts the current thread's execution.
 */
void Thread::Exit()
//*******************************************************************
{
  _endthreadex(0);
} // Exit()


/**
 * Sets the priority for the thread.
 *
 * @param   ePriority   New priority for thread.
 */
void Thread::SetPriority( const ThreadPriorityEnum ePriority )
//*******************************************************************
{
  switch( ePriority )
  {
    
    case Thread::LOWEST:  
      ::SetThreadPriority( m_pimpl->m_hThread, THREAD_PRIORITY_LOWEST );
      break;
    
    case Thread::LOWER:
      ::SetThreadPriority( m_pimpl->m_hThread, THREAD_PRIORITY_BELOW_NORMAL );
      break;
    
    case Thread::NORMAL:
      ::SetThreadPriority( m_pimpl->m_hThread, THREAD_PRIORITY_NORMAL );
      break;
    
    case Thread::HIGHER:
      ::SetThreadPriority( m_pimpl->m_hThread, THREAD_PRIORITY_ABOVE_NORMAL );
      break;

    case Thread::HIGHEST:
      ::SetThreadPriority( m_pimpl->m_hThread, THREAD_PRIORITY_HIGHEST );
      break;
    
    default: 
      break;
 } // switch( ePriority )
} // SetPriority()


/**
 * Sets the thread affinity
 *
 * @param   uAffinity   affinity mask
 */
void Thread::SetAffinity( const uint4 uAffinity )
//*******************************************************************
{

  uint4 affinityMask = ((uint4)1) << uAffinity;

  ::SetThreadAffinityMask( m_pimpl->m_hThread, affinityMask );
} // SetPriority()


/**
 * Gets the priority for the thread.
 *
 * @return   One of the ThreadPriorityEnum values.
 */
Thread::ThreadPriorityEnum Thread::GetPriority() const
{
  ThreadPriorityEnum ePriority = Thread::NORMAL;
  
  switch( ::GetThreadPriority( m_pimpl->m_hThread ) )
  {
  case THREAD_PRIORITY_LOWEST:        ePriority = Thread::LOWEST;  break;
  case THREAD_PRIORITY_BELOW_NORMAL:  ePriority = Thread::LOWER;   break;
  case THREAD_PRIORITY_NORMAL:        ePriority = Thread::NORMAL;  break;
  case THREAD_PRIORITY_ABOVE_NORMAL:  ePriority = Thread::HIGHER;  break;
  case THREAD_PRIORITY_HIGHEST:       ePriority = Thread::HIGHEST; break;
  default: break;
  } // switch( ePriority )
  
  return ePriority;
} // GetPriority()



/**
 * Suspends the currently executing thread for the specified time.
 * @param time in milliseconds to block the current thread.
 */
void Thread::Sleep(uint4 uMilliseconds)
//*******************************************************************
{
  ::Sleep(uMilliseconds);
} // Sleep()


/**
 *
 * Gets the thread handle
 *
 * @return   thread handle
 */
const HANDLE Thread::GetThreadHandle() const
{ 
  return m_pimpl->GetThreadHandle(); 
} // GetThreadHandle() const


/**
 *
 * Gets the thread id
 *
 * @return   thread id
 */
const uint4 Thread::GetThreadId() const
{ 
  return m_pimpl->GetThreadId(); 
} // GetThreadId() const



// $Log: Thread.C,v $
// Revision 1.6  2006/08/23 17:31:26  geconomos
// added Suspend and Resume methods
//
// Revision 1.5  2006/07/07 17:59:03  geconomos
// added methods to retrieve the internal thread handle and thread id
//
// Revision 1.4  2006/03/29 19:36:46  geconomos
// fixed file header
//
// Revision 1.3  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/10/25 17:30:36  geconomos
// added a method to set the thread's affinity
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/07/23 18:45:11  geconomos
// Added thread priority stuff.
//
// Revision 1.6  2003/05/16 13:07:42  frank
// code formatting
//
// Revision 1.5  2003/05/12 16:31:08  michael
// code review
//
// Revision 1.4  2002/05/17 19:37:47  dmitry
// Terminate method added.
//
// Revision 1.3  2002/03/13 14:26:49  geconomos
// Coding conventions.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Thread.C,v 1.6 2006/08/23 17:31:26 geconomos Exp $
// $Id: Thread.C,v 1.6 2006/08/23 17:31:26 geconomos Exp $
