//
// $Id: ThreadBase.cpp,v 1.7.2.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]
//

// Includes
#include "stdafx.h"
#include "ThreadBase.h"


// Namespaces 
USING_THREADING_NS
using namespace Viatronix::Logging;


 
/**
 * Constructor
 */
ThreadBase::ThreadBase() :
/*********************************************************************************************************/
m_bDisposed(false),
m_bEventsEnabled(true),
m_bFailed(false),
m_bFinished(false),
m_sLastErrorMessage(L""),
m_iThreadId(-1)
{

  Init();

  m_pThread->Name = S"";

} // ThreadBase()

/**
 * Constructor
 */
ThreadBase::ThreadBase(System::String * sName) :
/*********************************************************************************************************/
m_bDisposed(false),
m_bEventsEnabled(true),
m_bFailed(false),
m_bFinished(false),
m_sLastErrorMessage(L""),
m_iThreadId(-1)
{


  Init();

  m_pThread->Name = sName;

} // ThreadBase()

/**
 * Default Constructor
 */
ThreadBase::~ThreadBase()
{
  // Dispose of the object's resources
  Dispose(false);

} // ~ThreadBase()

/**
 * Dispose
 */
void ThreadBase::Dispose()
{
  Dispose(true);
  System::GC::SuppressFinalize(this);
} // Dispose()

/**
 * Dispose of objects resources
 *
 * Called from the Finalize(destructor) and the Dispose() methods.
 *
 * @param bDisposing     Flag to indicate if the object is being disposed or not
 */
void ThreadBase::Dispose(bool bDisposing)
{


  if ( ! m_bDisposed )
  {

    Log::Debug(String::Concat(S"Disposing ThreadBase [", Name, S"]"), "ThreadBase", "Dispose");

    if ( bDisposing )
    {
      // Dispose of managed objects
    } // END ... If we are disposing of the object


    try
    {

      /////////////////////////////////////////////////////////
      // If the thread is still running and not finished and 
      // is not already aborting then abort the thread.
      /////////////////////////////////////////////////////////
      if ( IsStarted && ! IsFinished && ! IsAborting )
      {
        Abort(true);
      }
    }
    catch ( System::Exception * ex)
    {
      Log::Warning(String::Concat("Could not abort thread : ", ex->Message), "ThreadBase", "Dispose");
    }
  } // END ... If the object has not already been disposed

  m_bDisposed = true;

} // Dispose()

/** 
 * Initialize thread
 */
void ThreadBase::Init()
{

  m_bFailed = false;
  m_bFinished = false;
  m_sLastErrorMessage = L"";
  m_iThreadId = -1;

  System::Threading::ThreadStart * pThreadStart = new System::Threading::ThreadStart(this, &ThreadBase::internalRun);

  if ( pThreadStart == NULL )
  {
    throw new OutOfMemoryException("Could not allocate thread start");
  }

  // Create Thread
  m_pThread = new System::Threading::Thread(pThreadStart);

  ////////////////////////////////////////////
  // Make sure the thread object was created
  ////////////////////////////////////////////
  if ( m_pThread == NULL )
  {
    throw new System::OutOfMemoryException("Could not create thread ");
  } // END ... If new failed

  

} // Init()

/**
 * Stars the threads process
 */
void ThreadBase::Start()
{

  m_pThread->Start();

  // Yield to the thread
  System::Threading::Thread::Sleep(5);

  Log::Debug(String::Concat("Started Thread [", this->Name, S"]"), "ThreadBase", "Start");


} // Start()


/**
 * Private run funtion.
 */
void ThreadBase::internalRun()
{

  Log::Debug(String::Concat(S"interalRun: Calling threads Run() method [", this->Name, S"]"), "ThreadBase", "internalRun");

  m_bFailed = false;
  m_sLastErrorMessage = L"";

  /// 2005 change
  /// The System::AppDomain::GetCurrentThreadId() is deprecated
  /// changed to use the win32 version
  m_iThreadId = (int)GetCurrentThreadId(); //System::AppDomain::GetCurrentThreadId();

  try
  {
    this->Run();

  }
  catch ( System::Exception * ex )
  {
    SetFailedState(ex->Message);
  }
  __finally
  {
    Log::Debug(String::Concat(S"Marking thread [", this->Name, S"] as finished"), "ThreadBase", "internalRun");

    m_bFinished = true;
  }

} // internalRun()


/**
 * Blocks execution of the current thread until this thread becomes signaled or
 * the specified amount of time has elapsed.
 *
 * @param iMilliseconds     time to wait in milliseconds
 *
 * @return true if successful, false otherwise
 */
bool ThreadBase::Wait(int iMilliseconds)
{

  if ( Equals(System::Threading::Thread::CurrentThread) )
  {
    Log::Warning(L"Thread tried to wait on itself", "ThreadBase", "Wait");
	  return false;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Make sure the thread started otherwise return false to indicate that the 
  // thread has not terminated
  ////////////////////////////////////////////////////////////////////////////////
  if ( ! Started )
  {
    return false;
  }

  ///////////////////////////
  // Are we still running?
  ////////////////////////////
  if( m_bFinished || ! IsRunning  )
  {
	  return true;
  }

  if ( iMilliseconds == System::Threading::Timeout::Infinite )
  {
    m_pThread->Join();
    return true;
  }
  else
  {
    return m_pThread->Join(iMilliseconds);
  }

} // Wait()


/**
 * Sets the failure state of the thread and logs the message
 */
void ThreadBase::SetFailedState(String * sLastErrorMessage)
{
  m_bFailed = true; 
  m_sLastErrorMessage = sLastErrorMessage;

  Log::Error(String::Concat(S"Thread [", Name, S"] failed : ", m_sLastErrorMessage), "ThreadBase", "SetFailedState");
} // SetFailureState()


/**
 * Aborts the thread and waits for it to end
 */
void ThreadBase::Abort(bool bWait)
{

  if ( this->IsRunning )
  {
    Log::Debug(String::Concat(S"Aborting Thread [", Name, S"]"), "ThreadBase", "Abort");

    m_pThread->Abort(); 
  }

  if ( bWait ) 
  { 
    m_pThread->Join(System::Threading::Timeout::Infinite); 
  }
  else
  {
    // Yeild to abort
    System::Threading::Thread::Sleep(1000);
  }
} // Abort()



/**
 * Invokes the proper delegate
 *
 * @param pEventHandler    Reference to the Event Handler delegate
 * @param pEventArgs       Reference to Thread Args
 */
void ThreadBase::InvokeDelegate( ThreadEventHandler * pEventHandler, ThreadEventArgs * pEventArgs )
{

  ///////////////////////////////////////////
  // Make sure the event handler is not NULL
  ///////////////////////////////////////////
  if( pEventHandler != NULL )
  {
    //////////////////////////////////////////////////////////////////
    // Format the delegate arguments (sender = NULL, ClientEventArgs )
    //////////////////////////////////////////////////////////////////
    System::Object * args[] = new System::Object * [2];
    args[0] = NULL; 
    args[1] = pEventArgs;
    
    // Get the connection points
    System::Delegate * pDelegates[] = pEventHandler->GetInvocationList();

    ///////////////////////////////////////////////////////////////////////////////
    // Loop through all connection points to determine the delegate invoketion
    ///////////////////////////////////////////////////////////////////////////////
    for ( int i(0); i < pDelegates->Count; i++ )
    {
      
      System::Delegate * pDelegate = pDelegates[i];

      // Control Type
      System::Type * pControlType = __typeof(System::Windows::Forms::Control);

   
      bool bIsControlType(false);

      ///////////////////////////////////////////////////////////////////////////////////
      // Loop through all the delegates types base types to see if it was derived from 
      // a control.
      ///////////////////////////////////////////////////////////////////////////////////
      System::Type * pType = pDelegate->Target->GetType();
      do
      {

        ///////////////////////////////////////////////////////////////////////////////
        // If derived from a control we are done so set the flag and leave this loop
        ///////////////////////////////////////////////////////////////////////////////
        if ( pType == pControlType )
        {
          bIsControlType = true;
          break;
        } // END ... If the delegate type was derived from a CONTROL

        pType = pType->BaseType;

      } while ( pType != NULL );


      ////////////////////////////////////////////////////////////////////
      // if the target is control based we must invoke the delegate
      // in the main ui thread, this seems to do it! :)
      ////////////////////////////////////////////////////////////////////
      if( bIsControlType )
      {
        System::Windows::Forms::Control * pControl = dynamic_cast<System::Windows::Forms::Control *>(pDelegate->Target);            
        pControl->Invoke( pDelegate, args );
      }
      else
      {
        // otherwise call in this thread
        pEventHandler->Invoke(this, pEventArgs);
      }

    } // END ... for each in the multicast delegate

  }  // END ... If the EventHandle is not NULL

} // InvokeDelegate()



/**
 * Fires the OnStart event
 *
 * @param iRemaining    Remainig count
 */
void ThreadBase::OnStarted(int iRemaining) 
{ 
  if ( m_bEventsEnabled )
  {
    Log::Debug(String::Format("{0} - Firing Start event [Remaining : {1}]", Name, __box(iRemaining)), "ThreadBase", "OnStarted");

    InvokeDelegate(ThreadBase::Started, new ThreadEventArgs(m_iThreadId, iRemaining )); 
  }
} // OnStart()

/**
 * Fires the OnUpdated event
 *
 * @param iRemaining        Remaining count
 * @param iCompleted        Completed count
 * @param sMessage          Message
 */
void ThreadBase::OnUpdated(int iRemaining, int iCompleted, System::String * sMessage ) 
{

  if ( m_bEventsEnabled )
  {
    Log::Debug(String::Format(String::Concat(Name, " - Firing Updated event ({0},{1},{2})"), __box(iRemaining), __box(iCompleted), sMessage), "ThreadBase", "OnUpdated");

    InvokeDelegate(ThreadBase::Updated, new ThreadEventArgs(m_iThreadId, iRemaining, iCompleted, sMessage)); 
  }
} // OnUpdated()


/**
 * Fires the OnComplete event
 *
 * @param  sMessage        Message
 */
void ThreadBase::OnCompleted(String * sMessage) 
{ 
  if ( m_bEventsEnabled )
  {
    Log::Debug(String::Concat(Name," - Firing Completed event : ", sMessage), "ThreadBase", "OnCompleted");

    InvokeDelegate(ThreadBase::Completed, new ThreadEventArgs(m_iThreadId, sMessage)); 
  }
} // OnComplete()


/**
 *  Fires the get aborted event
 */
void ThreadBase::OnAborted()  
{
  if ( m_bEventsEnabled )
  {
    Log::Debug(String::Concat(Name," - Firing Aborted event"), "ThreadBase", "OnAborted");

    InvokeDelegate(ThreadBase::Aborted, new ThreadEventArgs(m_iThreadId)); 
  }
} // OnAborted()
  
/**
 * Fires the get failure event (Use OnFailedConnection for a connection error)
 *
 * @param sErrorMessage       Error message indicating failure
 */
void  ThreadBase::OnFailed(System::String * sErrorMessage) 
{
  SetFailedState(sErrorMessage);

  if ( m_bEventsEnabled )
  {

    Log::Debug(String::Concat(Name," - Firing Failed event"), "ThreadBase", "OnFailed");

    InvokeDelegate(ThreadBase::Failed, new ThreadEventArgs(m_iThreadId,sErrorMessage)); 
  }
} // OnFailed()




// $Log: ThreadBase.cpp,v $
// Revision 1.7.2.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7  2006/12/05 19:16:59  gdavidson
// Consolidated code into Viatronix.Utilities
//
// Revision 1.6  2006/05/31 18:41:19  mkontak
// Change to VS 2005
//
// Revision 1.5  2006/03/20 15:28:24  mkontak
// Fix for threadbase abort
//
// Revision 1.4  2006/02/06 20:05:40  mkontak
// no message
//
// Revision 1.3  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.2  2005/07/01 12:39:08  vxconfig
// Added new Logger.
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
// Revision 1.14  2003/09/16 20:05:58  mkontak
// Fixed SystemInfo
//
// Revision 1.13  2003/09/04 14:46:20  mkontak
// Merge with XP_1_1_09042003
//
// Revision 1.12.2.2  2003/08/25 22:25:11  mkontak
// Minor changes
//
// Revision 1.12.2.1  2003/08/25 20:12:30  mkontak
// Change to ThreadBase
//
// Revision 1.12  2003/08/04 13:31:14  mkontak
// Added log message for finish
//
// Revision 1.11  2003/07/30 11:15:59  mkontak
// CDR writing changes and some restructuring
//
// Revision 1.10  2003/07/10 17:56:03  mkontak
// Coding standards, additions to DcmServerService registry
//
// Revision 1.9  2003/06/20 15:56:37  mkontak
// Coding standards
//
// Revision 1.8  2003/04/25 10:34:46  mkontak
// Coding standards / fixes
//
// Revision 1.7  2003/04/21 20:40:49  mkontak
// CD Writing/Anonymization
//
// Revision 1.6  2003/04/18 15:28:33  mkontak
// Added ThreadBase
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/ThreadBase.cpp,v 1.7.2.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: ThreadBase.cpp,v 1.7.2.1 2011/05/18 01:56:58 mkontak Exp $

