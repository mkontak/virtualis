// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "CvnThread.h"
#include "CvnLog.h"
#include "CvnExceptions.h"

using namespace cvn;


/**
 * Constructor
 */
CvnThread::CvnThread() :
util::Thread(),
CvnCallback()
{
 
} // CvnThread()

/**
 * Starts the thread
 *
 * @param pCallback       Callback function pointer
 * @param lpCallbackData  Callback data pointer
 *
 */
HANDLE CvnThread::Start(std::function< void( PCVN_CALLBACK_PARAMETERS, LPVOID) > callbackFunction, LPVOID callbackData  )
{

  CvnLog::Information("Starting conversion thread","CvnThread", "Start");

  m_callbackData      = callbackData;
  m_callbackFunction  = callbackFunction;

  try
  {
    return util::Thread::Start();
  }
  catch ( ... )
  {
    throw CvnException(LogRec(util::Strings::Format("Failed to create thread [%d]", GetLastError()), "CvnThread", "Start" ));
  }


} // Start(std::function< void( PCVN_CALLBACK_PARAMETERS, LPVOID) > callbackFunction, LPVOID callbackData  )

