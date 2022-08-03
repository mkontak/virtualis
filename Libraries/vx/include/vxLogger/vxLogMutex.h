// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogMutex.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once




/**
 * Wraps a mutex so that you can constructor/wait and let the object be destroyed by faling out of scope
 *
 * example:   vxLogMutex    mutex;
 *            mutex.Wait();
 *
 *  The object will be destroyed when you fall out of scope
 */
class VX_LOGGER_DLL vxLogMutex
{


private:

  /// Constructs the mutex 
  vxLogMutex(const std::string & sName);

public:

  /// Releases the mutex
   ~vxLogMutex();

  static std::shared_ptr<vxLogMutex> Create(const std::string & sName);

  ///
  void Release();

  ///
  void Wait();

private:

  void Create();

private:

  /// Mutex Name
  std::string m_sMutexName;

  /// Mutex sHandle
  HANDLE m_hMutex;

}; // class vxLogMutex