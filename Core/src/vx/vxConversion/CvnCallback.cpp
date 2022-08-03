// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "CvnCallback.h"
#include "CvnExceptions.h"
#include "CvnEnums.h"

using namespace cvn;


/**
 * Constructor
 */
CvnCallback::CvnCallback() :
m_callbackFunction(__nullptr),
m_callbackData(__nullptr)
{ 

  /// Zero's out the parameters structure
  RtlZeroMemory(&m_callbackParameters, sizeof(CVN_CALLBACK_PARAMETERS));

  /// Initializes the parameters
  m_callbackParameters.EventType    = CvnEvents::E_NONE;    // No event type
 

} // CvnCallback()






/**
 * Performs a callback using the supplied parameters
 *
 * @param eEventType      Event type for the callback
 */
void CvnCallback::Callback(CvnEvents eEventType, const std::string & sMessage)
{
  /// Set the evemet
  m_callbackParameters.EventType = eEventType;

  // Copy the message into the buffer
  strncpy_s(m_callbackParameters.Message, 512, sMessage.c_str(), sMessage.size() );

  /// Perform the callback
  Callback(&m_callbackParameters);
  
} // Callback(CvnEvents eEventType)


/**
 * Performs a callback using the supplied parameters
 *
 * @param eEventType      Event type for the callback
 */
void CvnCallback::Callback(PCVN_CALLBACK_PARAMETERS pParameters)
{

  // ===================================
  // Only perform callback if not NULL
  // ===================================
  if ( m_callbackFunction != __nullptr ) 
  {

    // Callback
    (m_callbackFunction)(pParameters, m_callbackData);

    RtlZeroMemory(m_callbackParameters.Message,sizeof(m_callbackParameters.Message));

  }  // END ... If the callback was set
  
} // Callback(PCVN_CALLBACK_PARAMETERS pParameters)

