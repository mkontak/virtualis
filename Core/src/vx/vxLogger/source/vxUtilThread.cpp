// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxUtilThread.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "vxUtilStrings.h"
#include "vxUtilThread.h"



// usings
using namespace util;



#pragma region Exceptions

/**
 * Constructor
 *
 * @param  Log record
 *
 */
ThreadException::ThreadException(const vxLogRecord & record) : 
m_record(record)
{
  LogErr( record.GetTextMessage(), record.GetClassName(), record.GetMethod() );
} //   ThreadException(const vxLogRecord & record) 


/** 
 * Test method
 */
void ThreadException::Test()
{
  try
  {
    throw ThreadException( vxLogRecord( "Error message", "ThreadException", "Test") );
  }
  catch(ThreadException & ex)
  {
    throw ThreadException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "ThreadException", "Test"));
  }
} // Test()



/** 
 * Test method
 */
void ThreadAbortedException::Test()
{
  try
  {
    throw ThreadAbortedException( vxLogRecord( "Error message", "ThreadAbortedException", "Test") );
  }
  catch(ThreadAbortedException & ex)
  {
    throw ThreadAbortedException( LogRec(util::Strings::Format(std::string(""),  "%s was caught", ex.GetTextMessage().c_str()), "ThreadAbortedException", "Test"));
  }
} // Test()


#pragma endregion



/**
 * Constructor - Default
 */
Thread::Thread() :
m_dwLastErrorCode(0),
m_bAbortRequested(false),
m_hThread(__nullptr),
m_exception(__nullptr)
{

  // Creates the abort event
  m_hAbort = CreateEvent(__nullptr, TRUE, FALSE, __nullptr);

  // Creates the completed event
  m_hComplete = CreateEvent(__nullptr, TRUE, FALSE, __nullptr);

} // Thread()


/**
 * Destructors
 */
Thread::~Thread()
{
  if ( m_hThread != __nullptr )
    CloseHandle(m_hThread);

  if ( m_hAbort != __nullptr )
    CloseHandle(m_hAbort);

  if ( m_hComplete != __nullptr )
    CloseHandle(m_hComplete);


  /// Set handle to __nullptr
  m_hThread = __nullptr;

} // ~Thread()

/**
 * Starts the thread
 *
 * @param pCallback       Callback function pointer
 * @param lpCallbackData  Callback data pointer
 *
 */
HANDLE Thread::Start()
{

  LPSECURITY_ATTRIBUTES pSecurityAttributes = __nullptr;
  SIZE_T dwStackSize = 0;
  LPTHREAD_START_ROUTINE pStart = __nullptr;
  DWORD dwThreadId = 0;

  /// Create a thread to start a server and accept associations. Pass the server in as a parameter.
  m_hThread = CreateThread(pSecurityAttributes, dwStackSize, &Thread::InternalRun, this, 0, &dwThreadId);

  // ==========================================
  // Make sure the thread object was created
  // ==========================================
  if ( m_hThread == __nullptr )
    throw ThreadException(Strings::Format(std::string(""),"Failed to create thread [%d]", GetLastError()));

  return m_hComplete;

} // Start()

/**
 * Waits for the server to complete
 *
 * @param waitTime      Time in milliseconds that we are to wait for the thread to complete
 *
 * @return Returns the results of the wait
 */
DWORD Thread::Wait(DWORD waitTime)
{

  DWORD dwReturn(0);
  DWORD waitReturn(WAIT_TIMEOUT);

#ifdef WINXP
  DWORD start = GetTickCount();
#else
  ULONGLONG start = GetTickCount64();
#endif

  HANDLE handles[2];
  handles[0] = m_hAbort;    // Abort event
  handles[1] = m_hComplete; // Completed event

  // =================================
  // Make sure the thread was created
  // =================================
  if ( m_hThread != __nullptr )
  {

    // Wait for abort or complete
    dwReturn = WaitForMultipleObjects(2, handles, FALSE, waitTime);

    // ======================
    // Check if we have out
    // ======================
    if ( dwReturn != WAIT_TIMEOUT )
    {
      DWORD index (dwReturn - WAIT_OBJECT_0);

      if ( index == 0 )
        throw ThreadAbortedException(LogRec("Thread aborted","Thread","Wait"));
      else if ( ! ( m_exception == __nullptr ) )
        std::rethrow_exception(m_exception);

    } // END ... If we have not timed out
  }

  // return the wait value
  return dwReturn;

 } // Wait(DWORD waitTime)


/**
 * Aborts the thread.
 */
void Thread::Abort()
{


  // ==================================================
  // We only need to abort if there thread is running
  // ==================================================
  if ( m_hThread != __nullptr )
  {

    // Set the abort requested flag
    m_bAbortRequested = true;

    SCOPE_EXIT
    {
      /// Close the handle
      CloseHandle(m_hThread);

      /// Set handle to __nullptr
      m_hThread = __nullptr;

    };

    try
    {

      /// Terminate thread
      TerminateThread(m_hThread, -1000);


      /// Give .5 seconds to end
      Sleep(500);

      SetEvent(m_hAbort);

      m_bAbortRequested = false;

    }
    catch ( ... )
    {

      /// Throw exception
      throw ThreadException(Strings::Format("Failed to abort thread [%d]", GetLastError()));

    } // END .... catch( ... )

  } // END ... If not finished

 
} // Stop()

/**
 * Internal Run function
 *
 * @param lpParam     Thread parameters
 */
DWORD WINAPI Thread::InternalRun(LPVOID lpParam)
{
  DWORD dwReturn(0);

  Thread * pThread = reinterpret_cast<Thread *>(lpParam);

  SCOPE_EXIT
  {
    CloseHandle(pThread->m_hThread);

    pThread->m_hThread = __nullptr;

  };

  try
  {
    pThread->RunSynchronously();
  }
  catch ( ... )
  {
    pThread->m_exception = std::current_exception();
    pThread->m_dwLastErrorCode = GetLastError();
    dwReturn = -1;
  }

  // Signal we are done
  SetEvent(pThread->m_hComplete);

  return dwReturn;
 
} // InternalRun(LPVOID lpParam)
