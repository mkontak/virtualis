// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogger.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxLogger.h"
#include "vxLogMutex.h"

/**
 * Constructor
 *
 * @param loggerInfo        Logger information
 */
vxLogger::vxLogger(const vxLoggerInfo & loggerInfo ) :
vxLoggerInfo(loggerInfo),
m_mutex(__nullptr)
{
} // vxLogger(const vxLoggerInfo & loggerInfo ) 

/**
 * Destructor
 *
 * Releases the resources.
 */
vxLogger::~vxLogger()
{
  m_mutex = __nullptr;

  Release();
} // ~vxLogger()

/**
 * Creates a mutex for the logger
 */
void vxLogger::CreateMutex()
{
  m_mutex = vxLogMutex::Create(vxLogFactory::GetMutexName() + "_" + GetHandle());
}  // CreateMutex