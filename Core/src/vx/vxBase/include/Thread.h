// $Id: Thread.h,v 1.5 2006/08/23 17:31:26 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

#ifndef Thread_h
#define Thread_h

// forward declarations
class ThreadImpl;

// class definition
class VX_BASE_DLL Thread 
{
// friend declarations
  friend class ThreadImpl;

// enum
public:

  enum ThreadPriorityEnum
  {
    LOWEST,
    LOWER,    
    NORMAL,
    HIGHER,
    HIGHEST
  };

// functions
public:
    
  /// default constructor
  Thread();

  /// destructor
  virtual ~Thread();

  /// starts execution of the thread
  void Start();

	/// blocks exection of the current thread until this tread has completed its work (will propagate any exception thrown on the thread)
  void End();

  /// terminates the thread
  void Terminate();

  /// suspends the thread
  void Suspend();

  /// resumes the thread
  void Resume();

  /// Blocks execution of the current thread of until this thread becomes signaled or the specified amount of time has elapsed.
  bool Wait(uint4 time = ULONG_MAX);
  
  /// returns true if the thread has finished running
  bool IsFinished() const;

  /// returns true if the thread is currently running
  bool IsRunning() const;
  
  /// sets the priority for the thread
  void SetPriority( const ThreadPriorityEnum ePriority );

  /// sets the affinity for the thread
  void SetAffinity( const uint4 uAffinity );
  
  /// gets the priority for the thread
  ThreadPriorityEnum GetPriority() const;  

  /// gets the thread handle
  const HANDLE GetThreadHandle() const;

  /// gets the thread id
  const uint4 GetThreadId() const;

  /// suspends the currently executing thread for the specified time
  static void Sleep(uint4 uMilliseconds);
  
  /// aborts the current thread's execution
  static void Exit();

protected:
    
  /// derived classes must override to provide functionailty
  virtual void Run() = 0;

private:
    
  /// copy constructor
  Thread(const Thread & other);
  
  /// assignment operator
  Thread & operator=(const Thread & other);

// data
private:
    
  // platform-dependant implementation
  ThreadImpl * m_pimpl;

}; // class Thread


#endif


// $Log: Thread.h,v $
// Revision 1.5  2006/08/23 17:31:26  geconomos
// added Suspend and Resume methods
//
// Revision 1.4  2006/07/07 17:59:03  geconomos
// added methods to retrieve the internal thread handle and thread id
//
// Revision 1.3  2006/03/29 19:40:05  geconomos
// fixed file header
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
// Revision 1.4  2002/07/02 20:45:01  geconomos
// Exporting classes .
//
// Revision 1.3  2002/05/17 19:37:47  dmitry
// Terminate method added.
//
// Revision 1.2  2002/03/13 14:26:49  geconomos
// Coding conventions.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Thread.h,v 1.5 2006/08/23 17:31:26 geconomos Exp $
// $Id: Thread.h,v 1.5 2006/08/23 17:31:26 geconomos Exp $
