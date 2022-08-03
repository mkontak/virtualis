#pragma once

#pragma once 

#include <ppltasks.h>


namespace io
{

/**
 * Implements a task to mask concurrency from the CLR
 */
class VX_BASE_DLL Task
{
public:

  void Wait()
  {  m_task.wait(); }


  void Run(Concurrency::task<void> task)
  { m_task = task; }


public:


  Concurrency::task<void> m_task;

};  // class TaskGroup


}  // namespace Task