// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogThread.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "vxUtilThread.h"
#include "vxLogCallback.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

/**
 * Implements a logger thread with callback 
 */
class VX_LOGGER_DLL vxLogThread : public util::Thread, public vxLogCallback 
{
protected:

  /// Constructor
  vxLogThread();

  /// Destructor
  virtual ~vxLogThread() {}
  


public: 
 
  /// Starts the server. 
  virtual HANDLE Start( LPVOID callbackData, std::function< void( PLOG_CALLBACK_PARAMETERS, LPVOID) > callbackFunction );

  /// Starts the server. 
  virtual HANDLE Start( std::function< void( PLOG_CALLBACK_PARAMETERS, LPVOID) > callbackFunction )
  { return Start(__nullptr, callbackFunction); }

  /// Starts the thread no parameters
  virtual HANDLE Start()
  { return util::Thread::Start(); }

}; // class vxLogThread

#pragma warning(pop)