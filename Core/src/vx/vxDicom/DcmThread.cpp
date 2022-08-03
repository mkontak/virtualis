// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmThread.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmThread.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmUtility.h"



using namespace dcm;

/**
 * Constructor
 *
 *  @param sStartLogMessage
 */
DcmThread::DcmThread(const std::string & sStartLogMessage) :
util::Thread(),
DcmCallback(),
m_sId(DcmUtility::GenerateId()),
m_sStartLogMessage(sStartLogMessage)
{   
  m_callbackParameters.Id = m_sId.c_str();

} // DcmThread()


/**
 * Starts the thread
 *
 * @param pCallback       Callback function pointer
 * @param lpCallbackData  Callback data pointer
 * @return handle
 */
HANDLE DcmThread::Start(LPVOID callbackData, std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction )
{

  if ( ! m_sStartLogMessage.empty() )
    DcmLog::Information(m_sStartLogMessage,"DcmThread", "Start");

  m_callbackData = callbackData;
  m_callbackFunction = callbackFunction;

  try
  {
    return util::Thread::Start();
  }
  catch ( ... )
  {
    throw DcmException(LogRec(util::Strings::Format("Failed to create thread [%d]", GetLastError()), "DcmThread", "Start" ));
  }

} // Start(LPVOID callbackData, std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction )

