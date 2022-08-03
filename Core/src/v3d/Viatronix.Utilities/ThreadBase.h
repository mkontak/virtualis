// $Id: ThreadBase.h,v 1.3 2006/12/05 19:16:59 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pragmas
#pragma once

// Namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;

OPEN_THREADING_NS


/**
  * Implements a basis set of thread event arguements.
  *
  */
public __gc class ThreadEventArgs : public System::EventArgs
{
public:

  /// Constructor
  ThreadEventArgs(int iThreadID, int iRemaining, int iCompleted, System::String * sMessage) :
      m_iThreadID(iThreadID),  m_iRemaining(iRemaining), m_iCompleted(iCompleted), m_sMessage(sMessage) { }

  /// Constructor.
  ThreadEventArgs(int iThreadID, int iRemaining) : m_iThreadID(iThreadID),  m_iRemaining(iRemaining), m_iCompleted(0), m_sMessage(L"") { }

  /// Constructor
  ThreadEventArgs(int iThreadID, int iRemaining, int iCompleted ) : m_iThreadID(iThreadID), m_iRemaining(iRemaining), m_iCompleted(iCompleted), m_sMessage(L"") { }

  /// Constructor
  ThreadEventArgs(int iThreadID, System::String * sMessage) : m_iThreadID(iThreadID), m_iRemaining(0), m_iCompleted(0), m_sMessage(sMessage) { }

  /// Constructor
  ThreadEventArgs(int iThreadID) : m_iThreadID(iThreadID),  m_iRemaining(0), m_iCompleted(0), m_sMessage(L"") { }

  /// Returns the thread id of the thread raising the event
  __property int get_ThreadID() { return m_iThreadID; }

  /// Returns the number of remaining operations before the thread is done
  __property int get_Remaining() { return(m_iRemaining); }

  /// Returns the number of completed operations.
  __property int get_Completed() { return(m_iCompleted); }

  /// Returns a basic message
  __property System::String * get_Message() { return(m_sMessage); }


private:

  /// Thread id
  int m_iThreadID;

  /// Remianing
  int m_iRemaining;

  /// Completed
  int m_iCompleted;

  /// Messages
  System::String * m_sMessage;

};  // class ThreadEventArgs

/// Thread Event Handler Delegate
public __delegate void ThreadEventHandler(System::Object* sender, ThreadEventArgs * e );



/**
 * Implements a base event driven thread object.
 *
 * EVENTS:
 *   Started     Fired when the thread starts
 *   Completed   Fired when the thread completes
 *   Updated     Fired when there is updated in the thread process
 *   Failed      Fired when the thread fails
 *   Aborted     Fired when the thread is aborted
 **/
[event_source(managed)]
public  __abstract __gc  class ThreadBase : public System::IDisposable
{

public:

  /// Constructor
  ThreadBase();

  /// Constructor
  ThreadBase(System::String * sName);

  /// Destructor
  virtual ~ThreadBase();

  /// This is the function that utimatly gets called and is implementation specific
  virtual void Run(void) = 0;

  /// Starts the thread
  void Start(); 

  /// Wait for the thread to finish
  virtual bool Wait(int iMilliseconds);

  /// Wait for the thread to finish
  virtual inline bool Wait() { return Wait(System::Threading::Timeout::Infinite); }

  /// 2005 changes
  /// Suspends the threads executaion
  /// Suspend has been depreciated
  ///virtual inline void Suspend() { m_pThread->Suspend(); }

  /// 2005 changes
  /// Resumes the suspended threads executaion
  /// Resume has been depreciated
  ///virtual inline void Resume() { m_pThread->Resume(); }

  /// Aborts the thread
  virtual void Abort(bool bWait); 

  /// Aborts the thread
  void Abort() { Abort( false ); }

  /// Puts thread to sleep for the specified time
  static inline void  Sleep(int iMilliseconds) { System::Threading::Thread::Sleep(iMilliseconds); }

  /// Dispose of object
  void Dispose();

/// Public Events
public:   

  /// Event is fired when the threads process is started
  __event ThreadEventHandler * Started;

  /// Event is fired when a threads pocesses state is updated
  __event ThreadEventHandler * Updated;

  /// Event is fired when the threads process has completed
  __event ThreadEventHandler * Completed;

  /// Event is fired when the thread has been aborted
  __event ThreadEventHandler * Aborted;

  /// Event is fired if the threads process has failed
  __event ThreadEventHandler * Failed;


/// Private Methods
private:          

  ///  Private run function.
  void internalRun();

  /// Initialize
  void Init();

/// Protected Methods
protected:       

  /// Sets the failed status (Can be used, but calling OnFailed() will set the failed state)
  void SetFailedState(String * sLastErrorMessage); 

  /// Overidable dispose method
  virtual void Dispose(bool bDisposing);


  //// Invokes the delegates
  virtual void InvokeDelegate( ThreadEventHandler * pEventHandler, ThreadEventArgs * pEventArgs);

  /// Fires the get started event
  virtual void OnStarted(int iRemaining);

  /// Fires the get started event
  virtual void OnStarted() { OnStarted(0); }

  /// Fires the on updated event with all arguements
  virtual void OnUpdated(int iRemaining, int iCompleted,  System::String * sMessage );

  /// Fires the on updated event with no message
  virtual void OnUpdated(int iRemaining, int iCompleted ) { OnUpdated(iRemaining, iCompleted, System::String::Empty); }

  /// Fires the on updated event with just a message
  virtual void OnUpdated(System::String * sMessage) { OnUpdated(0,0,sMessage); }

  /// Fires the on completed event
  virtual void OnCompleted() { OnCompleted(System::String::Empty); }

  /// Fires the on completed event with a message
  virtual void OnCompleted(System::String * sMessage);

  /// Fires the get aborted event
  virtual void OnAborted();

  /// Fires the get failure event (Use OnFailedConnection for a connection error)
  virtual void OnFailed(System::String * sErrorMessage);

  /// Fires the failed event with no message
  virtual void OnFailed() { OnFailed(System::String::Empty); }

/// Public Properties       
public:           

  /// Returns the threads name
  __property System::String * get_Name() { return m_pThread->Name; }

  /// Sets the threads name
  __property void set_Name(System::String * sName) { m_pThread->Name = sName; }

  /// Returns true if the thread is running false otherwise
  __property bool get_IsRunning() { return ( m_pThread->ThreadState == System::Threading::ThreadState::Running ? true : false ); }

  /// Returns true if the thread has been started false otherwise
  __property bool get_IsStarted() { return ( m_pThread->ThreadState == System::Threading::ThreadState::Unstarted ? false : true ); }

  /// Returns true if the thread is aborting false otherwise
  __property bool get_IsAborting() { return ( m_pThread->ThreadState == System::Threading::ThreadState::AbortRequested ? true : false ); }

  /// Returns true if the thread is alive false otherwose
  __property bool get_IsAlive() { return m_pThread->IsAlive; }

  /// Returns true if the thread failed (Last Error Message should be set)
  __property bool get_IsFailed() { return m_bFailed; }

  /// Returns the last error message 
  __property String * get_LastErrorMessage() { return m_sLastErrorMessage; }

  /// Returns true if finished false otherwise
  __property bool get_IsFinished() { return m_bFinished; }

  /// Returns the threads priority
  __property System::Threading::ThreadPriority get_Priority() { return m_pThread->Priority; }

  /// Sets the threads priority
  __property void set_Priority(System::Threading::ThreadPriority ePriority) { m_pThread->Priority = ePriority; }

  /// Returns the thread state
  __property System::Threading::ThreadState get_ThreadState() { return m_pThread->ThreadState; }

  /// Returns the thread state
  __property int get_ThreadId() { return m_iThreadId; }


/// Protected Properties
protected:    

  /// Returns the events enabled flag
  __property bool get_EventsEnabled() { return m_bEventsEnabled; }

  /// Set the events enabled flag
  __property void set_EventsEnabled(bool bEnabled) { m_bEventsEnabled = bEnabled; }


/// Private Members
private:

  /// Thread ID
  int m_iThreadId;

  /// Filed flag
  bool m_bFailed;

  /// Finished flag
  bool m_bFinished;

  /// Last Error Message
  System::String * m_sLastErrorMessage;

  /// Thread
  System::Threading::Thread * m_pThread;

  /// Events enabled 
  bool m_bEventsEnabled;

  /// Disposed flag
  bool m_bDisposed;

};  // class ThreadBase





CLOSE_THREADING_NS



// $Log: ThreadBase.h,v $
// Revision 1.3  2006/12/05 19:16:59  gdavidson
// Consolidated code into Viatronix.Utilities
//
// Revision 1.2  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.1  2004/03/02 20:02:52  geconomos
// Merged Viatronix.v3D.Logger, Viatronix.v3D.Imaging, Viatronix.v3D.Threading and Viatronix.v3D.Licensing into Viatronix.Utilities.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/11/14 20:03:49  mkontak
// Fixes
//
// Revision 1.1  2003/09/17 20:20:03  mkontak
// Split from utilities
//
// Revision 1.15  2003/09/16 20:05:58  mkontak
// Fixed SystemInfo
//
// Revision 1.14  2003/09/04 14:46:20  mkontak
// Merge with XP_1_1_09042003
//
// Revision 1.13.2.2  2003/08/25 22:25:11  mkontak
// Minor changes
//
// Revision 1.13.2.1  2003/08/25 20:12:31  mkontak
// Change to ThreadBase
//
// Revision 1.13  2003/07/30 11:15:59  mkontak
// CDR writing changes and some restructuring
//
// Revision 1.12  2003/07/10 17:56:03  mkontak
// Coding standards, additions to DcmServerService registry
//
// Revision 1.11  2003/06/20 15:56:38  mkontak
// Coding standards
//
// Revision 1.10  2003/05/06 15:58:44  mkontak
// Added command line arguments and fixed problem with MessageSender
//
// Revision 1.9  2003/04/30 14:03:33  mkontak
// Pipeline
//
// Revision 1.8  2003/04/25 10:34:46  mkontak
// Coding standards / fixes
//
// Revision 1.7  2003/04/22 15:01:42  mkontak
// Fixed ThreaBaseWithEvents class (removed __abstract)
//
// Revision 1.6  2003/04/22 14:00:21  mkontak
// Used #pragma once
//
// Revision 1.5  2003/04/18 15:28:33  mkontak
// Added ThreadBaseWithEvents
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/ThreadBase.h,v 1.3 2006/12/05 19:16:59 gdavidson Exp $
// $Id: ThreadBase.h,v 1.3 2006/12/05 19:16:59 gdavidson Exp $

