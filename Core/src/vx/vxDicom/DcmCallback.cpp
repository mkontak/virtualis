// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmCallback.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

// include
#include "Stdafx.h"
#include "DcmLog.h"
#include "DcmCallback.h"
#include "DcmExceptions.h"

// using
using namespace dcm;


/**
 * Constructor
 */
DcmCallback::DcmCallback() :
m_callbackFunction(__nullptr),
m_callbackData(__nullptr)
{ 
  /// Zero's out the parameters structure
  RtlZeroMemory(&m_callbackParameters, sizeof(DCM_CALLBACK_PARAMETERS));

  /// Initialize the parameters
  m_callbackParameters.CommandType  = C_NONE;             // No command type
  m_callbackParameters.EventType    = E_NONE;             // No event type
  m_callbackParameters.Status       = STATUS_Success;     // Default is success
 

} // DcmCallback()



/**
 * Performs a callback using the supplied parameters
 *
 * @param eEventType      Event type for the callback
 */
void DcmCallback::Callback(DcmEvents eEventType)
{

  m_callbackParameters.EventType = eEventType;

  Callback(&m_callbackParameters);
  
} // Callback(DcmEvents eEventType)


/**
 * Performs a callback using the supplied parameters
 *
 * @param eEventType      Event type for the callback
 */
void DcmCallback::Callback(PDCM_CALLBACK_PARAMETERS pParameters)
{
  
  try
  {
    if ( m_callbackFunction != __nullptr )
       (m_callbackFunction)(pParameters, m_callbackData);
  }
  catch ( ... )
  {
    DcmLog::Error( "Unspecified error occurred in callback", "DcmCallback", "Callback" );
  }
 
} // Callback(DcmEvents eEventType)

