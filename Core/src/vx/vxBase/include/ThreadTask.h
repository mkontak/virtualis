// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ThreadTask.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <memory>
#include <functional>


class VX_BASE_DLL ThreadTask
{
  // run method fucntion pointer
  typedef std::function< void( void )> RunMethod;

  // callback function pointer
  typedef std::function< void( void )> TaskCallback;

  friend class InternalThread;

public:

  // constructor
  ThreadTask( RunMethod run );

  // starts the task
  void Begin( TaskCallback callback = nullptr );

  // ends the task
  void End();

private:
   
  /// copy constructor
  ThreadTask(const ThreadTask & other);
  
  /// assignment operator
  ThreadTask & operator=(const ThreadTask & other);

private:

  // internal thread
  std::shared_ptr< InternalThread > m_pimpl;

}; // ThreadTask


