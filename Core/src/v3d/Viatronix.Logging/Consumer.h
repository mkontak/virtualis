//

#pragma once

#include "ConsumerEventArgs.h"


using namespace System;

OPEN_LOGGING_NS



/**
 * Log object allow creating and logging to specified logger 
 */
public ref class Consumer
{
private:

  Consumer();

public:

  /// Destructor
  virtual ~Consumer();

  /// Finalizer
  !Consumer()
  { this->~Consumer(); }

  /// Creates the consumer object for the source
  static Consumer ^ Create(String ^ sSource);

  /// Dumps the source etl file to the destination file specified
  static void Dump(String ^ sSource, String ^ sDestination);

  /// Starts the log consuming
  virtual void Start() ;

  /// Aborts the log consuming
  virtual void Abort() ;

  /// Waits the log consoumer to end
  virtual void Wait() ;
 
private:

  /// Callback
  void Callback(PLOG_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData);

  /// Fire the proper event
  void FireEvent(PLOG_CALLBACK_PARAMETERS pCallbackParameters, ConsumerEventArgs ^ pArgs);

#pragma region events

public:

  /// Event is fired when the client is started
  event ConsumerEventHandler ^ Started;

  /// Event is fired when the client completed
  event ConsumerEventHandler ^ Completed;

  /// Event is fired when the client has aborted
  event ConsumerEventHandler ^ Aborted;

  /// Event is fired when the client has failed
  event ConsumerEventHandler ^ Failed;

    /// Event is fired when the client has made progress
  event ConsumerEventHandler ^ Progress;

#pragma endregion

#pragma region fire event methods

protected:       

  /// Fires the get started event
  virtual inline void OnStarted(ConsumerEventArgs ^ mpEventArgs)
  { Started(this, mpEventArgs); }

  /// Fires the on completed event
  virtual inline void OnCompleted(ConsumerEventArgs ^ mpEventArgs)
  { Completed(this, mpEventArgs);  }

  /// Fires the aborted event
  virtual inline void OnAborted(ConsumerEventArgs ^ mpEventArgs)
  { Aborted(this, mpEventArgs); }

  /// Fires the failed event with no message
  virtual inline void OnFailed(ConsumerEventArgs ^ mpEventArgs)
  { Failed(this, mpEventArgs); }

  /// On Progress
  inline void OnProgress(ConsumerEventArgs ^ mpEventArgs)
  { Progress(this, mpEventArgs); }



#pragma endregion



#pragma region properties

public:

  // Gets the source
  property String ^ Source
  {
    String ^ get() { return gcnew String(m_pConsumer->GetSource().c_str()); }
  } // Source

  /// Gets the running flag
  property bool IsRunning
  { 
    bool get() { return m_pConsumer->IsRunning(); }
  } // IsRunning

    /// Gets the wait handle
  property System::Threading::EventWaitHandle ^ Handle
  {
    System::Threading::EventWaitHandle ^ get() { return m_mpEventWaitHandle; }
  } // EventWaitHandle


#pragma endregion


private:

  vxLogConsumer * m_pConsumer;

  /// Event wait handle
  System::Threading::EventWaitHandle ^ m_mpEventWaitHandle;
 
  /// Callback delegate used for mapping the native callback
  LoggingCallbackDelegate ^ m_mpCallbackDelegate;

};  // class Log


CLOSE_LOGGING_NS
