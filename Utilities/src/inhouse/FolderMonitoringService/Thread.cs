// $Id: Thread.cs,v 1.10 2007/04/19 21:17:25 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )



using System;
using System.Threading;

namespace Viatronix
{

	/// <summary>
	/// Summary description for Thread.
	/// </summary>
	public class Thread : System.IDisposable
	{

    #region fields 

    /// <summary>
    /// Thread ID
    /// </summary>  
    private int m_threadId = 0;

    /// <summary>
    /// Filed flag
    /// </summary>  
    private bool m_failed = false;

    /// <summary>
    /// Finished flag
    /// </summary>  
    private bool m_finished = false;

    /// <summary>
    /// Last Error Message
    /// </summary>
    private string m_lastErrorMessage = string.Empty;

    /// <summary>
    /// Thread
    /// </summary>  
    private System.Threading.Thread m_thread = null;

    /// <summary>
    /// Events enabled 
    /// </summary>  
    private bool m_eventsEnabled = true;

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// Auto Reset Event for this thread
    /// </summary>
    private ManualResetEvent m_finishEvent = new ManualResetEvent(false);

    /// <summary>
    /// Auto Reset Event for this thread
    /// </summary>
    private ManualResetEvent m_abortEvent = new ManualResetEvent(false);

    #endregion

    #region construction and finalizers

    /// <summary>
    /// Base constructor
    /// </summary>
		public Thread() 
		{
      Init();
		} // Thread()

    /// <summary>
    /// Constructor using the name option
    /// </summary>
    /// <param name="name">Thread name</param>
    public Thread(string name) 
    {
      Init();

      m_thread.Name = name;
    } // Thread

    /// <summary>
    /// Dispose method exposed for the finalizer to use
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      System.GC.SuppressFinalize(this);
    } // Dispose()

    /// <summary>
    /// Dispose of this object
    /// </summary>
    /// <param name="disposing">Flag indicating if we are disposing or not</param>
    protected virtual void Dispose(bool disposing)
    {


      if ( ! m_disposed )
      {


        if ( disposing )
        {
          // Dispose of managed objects
        } // END ... If we are disposing of the object


        try
        {

          /////////////////////////////////////////////////////////
          // If the thread is still running and not finished and 
          // is not already aborting then abort the thread.
          /////////////////////////////////////////////////////////
          if ( IsRunning && ! IsFinished && ! IsAborting )
          {
            Abort();
          }
        }
        catch ( System.Exception  ex)
        {
          // Could not abort (Dont Error)
        }
      } // END ... If the object has not already been disposed

      m_disposed = true;

    } // Dispose()

    /// <summary>
    /// Initializes object
    /// </summary>
    private void Init()
    {

      // Create Thread
      m_thread = new System.Threading.Thread(new ThreadStart(this.internalRun));

      ////////////////////////////////////////////
      // Make sure the thread object was created
      ////////////////////////////////////////////
      if ( m_thread == null )
      {
        throw new System.OutOfMemoryException("Could not create thread ");
      } // END ... If new failed



    } // Init()

    #endregion
    
    #region properties

    /// <summary>
    /// Gets the auto reset event
    /// </summary>
    public ManualResetEvent FinishEvent
    {
      get { return m_finishEvent; }
    } // FinishEvent

    /// <summary>
    /// Gets the auto reset event
    /// </summary>
    public ManualResetEvent AbortEvent
    {
      get { return m_abortEvent; }
    } // AbortEvent

    /// <summary>
    /// Gets the threads name
    /// </summary>
    public string ThreadName
    { 
      get { return m_thread.Name; }
    } // ThreadName
    
    /// <summary>
    /// Gets the is running flag ( true if the thread is running false otherwise )
    /// </summary>
    public bool IsRunning
    { 
      get { return ( m_thread.ThreadState == ThreadState.Running ? true : false ); }
    } // IsRunnning

    /// <summary>
    /// Gets the IsSTarted flag ( true if the thread has been started false otherwise )
    /// </summary>
    public bool IsStarted
    { 
      get { return ( m_thread.ThreadState == ThreadState.Unstarted ? false : true ); }
    } // IsStarted

    /// <summary>
    /// Gets the is aborting flag ( true if the thread is aborting false otherwise )
    /// </summary>
    public bool IsAborting
    { 
      get { return ( m_thread.ThreadState == ThreadState.AbortRequested ? true : false ); }
    } // IsAborting

    /// <summary>
    /// Gets the is alive flag (true if the thread is alive false otherwose)
    /// </summary>
    public bool IsAlive
    { 
      get { return m_thread.IsAlive; }
    } // IsAlive

    /// <summary>
    /// Gets the is failed flag ( true if the thread failed (Last Error Message should be set) )
    /// </summary>  
    public bool IsFailed 
    { 
      get { return m_failed; }
    } // IsFailed

    /// <summary>
    /// Gets the last error message 
    /// </summary>  
    public string LastErrorMessage 
    { 
      get { return m_lastErrorMessage; }
    } // LastErrorMessae

    /// <summary>
    /// Gets the is finished flag ( true if finished false otherwise )
    /// </summary>  
    public bool IsFinished 
    { 
      get { return m_finished; }
    } // IsFinished
      
    /// <summary>
    /// Gets/Sets the threads priority
    /// </summary>  
    public ThreadPriority Priority 
    { 
      get { return m_thread.Priority; }
      set { m_thread.Priority = value; }
    } // Priority


    /// <summary>
    /// Gets the thread state
    /// </summary>
    public ThreadState ThreadState 
    { 
      get { return m_thread.ThreadState; }
    } // ThreadState

    /// <summary>
    /// Gets the thread state
    /// </summary>  
    public int ThreadId 
    { 
      get { return m_threadId; }
    } // ThreadId


    #endregion
    
    #region methods

    /// <summary>
    /// Starts the thread
    /// </summary>
    public void Start()
    {
      m_thread.Start();

      // Yield to the thread
      System.Threading.Thread.Sleep(1);


    } // Start()

    /// <summary>
    /// Private run function
    /// </summary>
    private void internalRun()
    {


      m_failed = false;
      m_lastErrorMessage = String.Empty;

      try
      {
        Run();

      }
      catch ( ThreadAbortException )
      {
        throw;
      }
      catch ( System.Exception ex )
      {
        SetFailedState(ex.Message);
      }
      finally
      {
        m_finished = true;

         // Signal that the thread is complete
        m_finishEvent.Set();
      }

    } // internalRun()

    /// <summary>
    /// Blocks execution of the current thread until this thread becomes signaled or
    /// the specified amount of time has elapsed.
    /// </summary>
    /// <param name="iMilliseconds">time to wait in miliseconds</param>
    /// <returns>true if successful, false otherwise</returns>
    public bool Wait(int iMilliseconds)
    {

      if ( Equals(System.Threading.Thread.CurrentThread) )
      {
        return false;
      }

      ////////////////////////////////////////////////////////////////////////////////
      // Make sure the thread started otherwise return false to indicate that the 
      // thread has not terminated
      ////////////////////////////////////////////////////////////////////////////////
      if ( ! IsStarted )
      {
        return false;
      }

      ///////////////////////////
      // Are we still running?
      ////////////////////////////
      if( m_finished || ! IsRunning  )
      {
        return true;
      }

      return m_thread.Join(iMilliseconds);

    } // Wait()

    /// <summary>
    /// Wait forever for the thread to complete
    /// </summary>
    /// <returns></returns>
    public bool Wait()
    {
      return Wait(System.Threading.Timeout.Infinite);
    } // Wait()


    /// <summary>
    /// Sets the failure state of the thread and logs the message
    /// </summary>
    /// <param name="errorMessage">Error messages associated with failure</param>
    protected void SetFailedState(string errorMessage)
    {
      m_failed = true; 
      m_lastErrorMessage = errorMessage;
    } // SetFailureState()


    /// <summary>
    /// Aborts the thread and waits for it to complete
    /// </summary>
    public virtual  void Abort()
    {

      if ( ! IsAborting )
      {
        m_thread.Abort(); 

        // Give the thread time
        System.Threading.Thread.Sleep(1000);

        m_abortEvent.Set();
      }


    } // Abort()

    #endregion

    #region virtual methods

    /// This is the function that utimatly gets called and is implementation specific
    public virtual void Run() { }

    #endregion

 
  } // class Thread
}


#region revisions history

// Revision History:
// $Log: Thread.cs,v $
// Revision 1.10  2007/04/19 21:17:25  jmeade
// code review.
//
// Revision 1.9  2007/03/06 16:29:45  mkontak
// Coding standards
//
// Revision 1.8  2007/03/06 16:24:59  mkontak
// Coding standards
//
// Revision 1.5  2006/06/09 19:54:02  mkontak
// Deprecated code
//
// Revision 1.4  2005/10/03 13:11:30  mkontak
// Moved code from server to client and rearranged code
//
// Revision 1.3  2005/06/22 20:33:06  mkontak
// Import
//
// Revision 1.2  2005/05/13 16:40:39  mkontak
// Added synchronization between clients and server
//
// Revision 1.1  2005/04/19 19:14:42  mkontak
// Initial
//
// Revision 1.1  2004/10/27 19:19:42  mkontak
// Renamed process to job
//
// Revision 1.1  2004/10/21 21:04:29  mkontak
// Initial check in
//
// Revision 1.5  2004/06/03 18:17:07  mkontak
// Fixes for the WORK QUEUE service
//
// $Header: /cvs/cvsRepository/V3D/src/Viatronix.v3D.Cache.Server/Thread.cs,v 1.10 2007/04/19 21:17:25 jmeade Exp $
// $Id: Thread.cs,v 1.10 2007/04/19 21:17:25 jmeade Exp $

#endregion