// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogger.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once


// includes
#include "vxLogEnums.h"
#include "vxLoggerInfo.h"
#include "vxLogRecord.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

// forward declarations
class VX_LOGGER_DLL vxLogMutex;

/**
 * Implements a logger that allows for tracing of events within the V3D System.
 */
class VX_LOGGER_DLL vxLogger : public vxLoggerInfo
{


// constructors
public:

  ///
  vxLogger( const vxLoggerInfo & loggerInfo ) ;


public:

  ///Default Destructor
  virtual ~vxLogger();

  /// Overloaded == operator
  inline bool operator==(const vxLogger & rhs)
  { return Equals(rhs); }

  /// Overloaded == operator
  inline bool operator!=(const vxLogger & rhs)
  { return !Equals(rhs); }

  ///Static Message method  which logs the Record
  virtual void Trace( const vxLogRecord & record ) = 0;

  /// Creates the mutex
  virtual void CreateMutex();

protected:

  /// Releases the resources
  virtual void Release() { };

  // Initialize the the object
  virtual void initialize() { };

protected:

  ///Check the Log Level
  inline bool IsLoggable( const vxLogRecord::SeverityEnum & eSeverity )
  { return (eSeverity <= this->GetLogLevel() ); }
  


protected:

  std::shared_ptr<vxLogMutex> m_mutex;

};  // class vxLogger


#pragma warning(pop)


