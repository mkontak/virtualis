// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxTaskGroup.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once 

#include <ppl.h>
/**
 * Implements a task list to mask concurrency from the CLR
 */
class VX_LOGGER_DLL TaskGroup
{
public:

  // Cancel all tasks 
  ~TaskGroup() 
  { Group.cancel(); }

public:

  Concurrency::task_group Group;
};  // class TaskGroup




