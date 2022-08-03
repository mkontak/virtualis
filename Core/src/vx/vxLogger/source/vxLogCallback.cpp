// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogCallback.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "vxLogCallback.h"
#include "vxLogExceptions.h"




/**
 * Constructor
 */
vxLogCallback::vxLogCallback() :
m_callbackFunction(__nullptr),
m_callbackData(__nullptr)
{ 

  /// Zero's out the parameters structure
  RtlZeroMemory(&m_callbackParameters, sizeof(LOG_CALLBACK_PARAMETERS));

  /// Initialize the parameters
  m_callbackParameters.EventType    = E_NONE;             // No event type
  m_callbackParameters.Abort        = false;              // Default is false for abort
  

} // vxLogCallback()






/**
 * Performs a callback using the supplied parameters
 *
 * @param eEventType      Event type for the callback
 */
void vxLogCallback::Callback(vxLogEventTypes eEventType)
{

  m_callbackParameters.EventType = eEventType;

  Callback(&m_callbackParameters);
  
} // Callback(DcmEvents eEventType)


/**
 * Performs a callback using the supplied parameters
 *
 * @param eEventType      Event type for the callback
 */
void vxLogCallback::Callback(PLOG_CALLBACK_PARAMETERS pParameters)
{

  if ( m_callbackFunction != __nullptr ) 
  {
    (m_callbackFunction)(pParameters, m_callbackData);

    /// If the event type indicates that we are still in progress and the abort flag was set then throw and abort exception
    if ( pParameters->EventType == E_PROGRESS && pParameters->Abort )
      throw vxLogAbortException(LogRec("Aborting", "vxLogCallback", "Callback"));
  }
  
} // Callback(DcmEvents eEventType)

