// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLoggerClassic.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "vxLogProvider.h"
#include "vxLogSession.h"
#include "vxLogRecord.h"
#include "vxLogger.h"


// pragmas
#pragma warning(push)
#pragma warning(disable:4251)




/**
 * Logger Class declaration.
 */
class VX_LOGGER_DLL vxLoggerClassic : public vxLogger
{

// member functions
protected:

  ///
  vxLoggerClassic(const vxLoggerInfo & loggerInfo );

public:

  ///Default Destructor
  virtual ~vxLoggerClassic();


   /// Creates the logger
  static std::shared_ptr<vxLogger>  Create(const vxLoggerInfo & loggerInfo);

 
  ///Static Message method  which logs the Record
  virtual void Trace( const vxLogRecord & record );

 


protected:

  /// Releases the resources
  virtual void Release() { }

  

};  // class vxLoggerClassic



#pragma warning(pop)