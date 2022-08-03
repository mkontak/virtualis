// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogThread.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "vxLogThread.h"
#include "vxLogExceptions.h"
#include "vxLogUtility.h"
#include "vxUtilStrings.h"



/**
 * Constructor
 */
vxLogThread::vxLogThread() :
util::Thread(),
vxLogCallback()
{
 

} // vxLogThread()


/**
 * Starts the thread
 *
 * @param pCallback       Callback function pointer
 * @param lpCallbackData  Callback data pointer
 *
 */
HANDLE vxLogThread::Start(LPVOID callbackData, std::function< void( PLOG_CALLBACK_PARAMETERS, LPVOID) > callbackFunction )
{

  LogFyi("Starting logger thread","vxLogThread", "Start");

  m_callbackData = callbackData;
  m_callbackFunction = callbackFunction;

  try
  {
    return util::Thread::Start();
  }
  catch ( ... )
  {
    throw vxLogException(LogRec(util::Strings::Format("Failed to create thread [%d]", GetLastError()), "DcmThread", "Start" ));
  }


} // Start(LPVOID callbackData, std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction )

