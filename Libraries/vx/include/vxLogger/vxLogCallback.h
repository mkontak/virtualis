// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogCallback.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// includes
#include "vxLogEnums.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


#pragma region callback structors

// Started parameters
typedef struct vxLogStartedParameters
{
  LPCSTR Message;
} vxLogStartedParameters;

// Completed parameters
typedef struct vxLogCompletedParameters
{
  LPCSTR Message;
} vxLogCompletedParameters;

// Progress parameters
typedef struct vxLogProgressParameters
{
  PEVENT_DESCRIPTOR EventDescriptor;
  GUID ProviderId;
  LPCSTR Record;
} vxLogProgressParameters;

// Failed parameters
typedef struct vxLogFailedParameters
{
  LPCSTR Message;
} vxLogFailedParameters;

/**
  * Logger Callback Parameters
  */
typedef struct vxLogCallbackParameters
{
  vxLogEventTypes EventType;
  LPCTSTR Source;
  bool Abort;

  // The type is based on the event type
  union
  {
    vxLogStartedParameters Started;
    vxLogCompletedParameters Completed;
    vxLogProgressParameters Progress;
    vxLogFailedParameters Failed;
  } Event;

 } LOG_CALLBACK_PARAMETERS, *PLOG_CALLBACK_PARAMETERS, *LPLOG_CALLBACK_PARAMETERS;

/// Callback function 
typedef void (*LOG_CALLBACK_FUNCTION)(PLOG_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

/// Callback function pointer
typedef LOG_CALLBACK_FUNCTION (*PLOG_CALLBACK_FUNCTION);

#pragma endregion

/**
 * Impelements a class that handles the callback events from the logger
 */
class VX_LOGGER_DLL vxLogCallback
{
protected:

  /// constructor
  vxLogCallback();

  // Destructor
  virtual ~vxLogCallback() {}

  
protected:

  // Performs a callback using the internal parameters
  inline void Callback()
  { Callback(&m_callbackParameters); }

  /// Callback based on event
  void Callback(vxLogEventTypes eEventType);

  /// Performs the callback using the supplied parameters
  void Callback(PLOG_CALLBACK_PARAMETERS pParameters);
  

protected:

  /// Callback data
  LPVOID m_callbackData;

  /// Callback function
  std::function< void( PLOG_CALLBACK_PARAMETERS, LPVOID ) > m_callbackFunction;

  /// Callback parameters
  LOG_CALLBACK_PARAMETERS m_callbackParameters;


}; // class vxLogCallback



#pragma warning(pop)