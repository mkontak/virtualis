// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogTimer.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// include
#include "vxLogRecord.h"

// forward declarations
class VX_LOGGER_DLL vxLogger;


/**
 * Implements logging timer that can be used in source code
 *
 *  Example:
 *
 *   {
 *      vxLogTimer timer;   // Create
 *
 *        ...               // processing
 *
 *      timer.trace()       // Call one of the trace methods
 *    }
 */
class VX_LOGGER_DLL vxLogTimer 
{
public:

  /// Constructor
  vxLogTimer(vxLogRecord::SeverityEnum eLogLevel = vxLogRecord::SL_DEBUG);

  /// Destructor
  virtual ~vxLogTimer() { }
  
  /// Traces to the default logger
  void Trace(const std::string & sClass, const std::string & sMethod,  const std::string & sMessage);

  /// Traces to the default logger
  inline void Trace(const std::string & sMessage)
  { Trace("vxLogTimer", "Trace", sMessage); }

  /// Trace to the specified logger
  void Trace(std::shared_ptr<vxLogger> logger, const std::string & sFormat);
 
  /// Trace to the logger specified
  void Trace(const std::string & sHandle, const std::string & sFormat);

  /// Gets the elapsed time from the creation of the object in milliseconds
  float GetElapsedTime();

  /// Resets the timer
  void Reset();

private:

  /// Starting tick count
  ULONGLONG m_start;
 
  /// Level for writing message
  vxLogRecord::SeverityEnum m_eLogLevel;


}; // class vxLogThread

