// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxUtilThread.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


/// forward declarations
class VX_LOGGER_DLL vxLogRecord;

// namespace
namespace util
{



#pragma region Thread Exceptions


/**
 * Implements a generic Thread exception. Will throw exceptions in the Wait() function so that errors can be handled in a 
 * normal way. The thread function may also be lambda expression or a regular function.
 */
class VX_LOGGER_DLL ThreadException
{
public:

  /// constructor for log record
  ThreadException(const vxLogRecord & record);

  /// Gets the log record
  virtual const vxLogRecord & GetLogRecord() const
  { return m_record; }
  
  /// Gets the message for the exception
  virtual const std::string & GetTextMessage() 
  { return m_record.GetTextMessage(); }

  /// Test
  static void Test();


private:

  /// log record
  vxLogRecord m_record;

}; // class ThreadException

/**
 * Implements a Thread Aborted exception
 */
class VX_LOGGER_DLL ThreadAbortedException : public ThreadException
{
// functions
public:

  /// constructor for log record
  ThreadAbortedException(const vxLogRecord & record) : ThreadException(record) {}
  
  /// test
  static void Test();

}; // class ThreadAbortedException



#pragma endregion


/**
  * Implements a base threaded object that will allow for starting and waiting were the wait 
  * will throw an exception. 
  */
class  VX_LOGGER_DLL Thread
{
protected:

  /// Default Constructor
  Thread();

   /// Destructor
  virtual ~Thread();

public:


  /// Returns true if the thread failed
  bool IsFailed() 
  { return ! ( m_exception == __nullptr ) ; }

  /// Returns TRUE is running false IF NOT
  virtual bool IsRunning() 
  { return ( m_hThread != __nullptr ? true : false ); }
    
  /// Starts the server. 
  virtual HANDLE Start();

  /// Waits forever
  inline virtual DWORD Wait() 
  { return Wait(INFINITE); }

  /// Wait for the thread to finish
  virtual DWORD Wait(DWORD waitTime);

  /// Stops the thread
  virtual void Abort();

  // Gets the threads sHandle
  const HANDLE GetHandle() const
  { return m_hComplete; }


  // This method must be overridden and runs the thread asynchrouslly
  virtual void RunSynchronously() = 0; 


private:

  // Internal Run 
  static DWORD WINAPI InternalRun(LPVOID lpParam);


protected:

  // Abort requested flag
  bool m_bAbortRequested;
 
private:
 
  /// Thread sHandle
  HANDLE m_hThread;

  // Last error code
  DWORD m_dwLastErrorCode;

  // Last exception
  std::exception_ptr m_exception;

  // Abort event handle
  HANDLE m_hAbort;

  // Completed event handle
  HANDLE m_hComplete;

}; // class Thread

} // namespace util

#pragma warning(pop)
