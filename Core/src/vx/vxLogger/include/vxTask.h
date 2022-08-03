// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxTask.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once 

#include <ppl.h>
#include <ppltasks.h>

/**
 * Implements a task to mask concurrency from the CLR
 */
class VX_LOGGER_DLL Task
{
public:

  /// Constructors
  Task() : m_bAssigned(false) { }
 

  /// Wait for task to complete
  void Wait(bool bThrowException = false)
  { 

    if ( m_bAssigned )
    {
      try
      {
        m_task.wait(); 
      }
      catch ( std::exception & ex )
      {
        LogErr(util::Strings::Format("Task caught %s",ex.what()), "Task", "Wait");

        if ( bThrowException )
          throw;
      }
      catch ( ...  )
      {
        LogErr("Task caught unspecified exception","Task","Wait");

        if ( bThrowException )
          throw;
      }
    }
  }

  /// Run task
  void Run(Concurrency::task<void> task)
  {
    m_task = task; 
    m_bAssigned = true;

  }


public:

  bool m_bAssigned;

  Concurrency::task<void> m_task;

};  // class TaskGroup
