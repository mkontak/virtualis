
// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net
#pragma once

#include "vxBlockVolume.h"

namespace cvn
{


/**
 * Conversion Callback Parameters
 */
typedef struct CvnCallbackParameters
{
  enum CvnEvents EventType;         // [in] Event Type
  enum CvnStages StageType;         // [in] Stages Type

  const char * SeriesUid;           // [in] Series UID
  int Remaining;                    // [in] Remaining images to process
  int Completed;                    // [in] Completed images processed
  int Total;                        // [in] Total count

  char Message[512];                // [in] Message

} CVN_CALLBACK_PARAMETERS, *PCVN_CALLBACK_PARAMETERS, *LPCVN_CALLBACK_PARAMETERS;

/// Callback function 
typedef void (*CVN_CALLBACK_FUNCTION)(PCVN_CALLBACK_PARAMETERS , LPVOID  );

/// Calllback function pointer
typedef CVN_CALLBACK_FUNCTION (*PCVN_CALLBACK_FUNCTION);


  
#pragma region CvnCallback

/**
 * Implements a class that handles the callback events for conversions
 */
class CVN_DLL CvnCallback
{

protected:

  /// constructor
  CvnCallback();

public:

  // Destructor
  virtual ~CvnCallback() {}

  
protected:

  // Performs a callback using the internal parameters
  inline void Callback()
  { Callback(&m_callbackParameters); }

  /// Callback based on event
  void Callback(CvnEvents eEventType, const std::string & sMessage = "");

  /// Performs the callback using the supplied parameters
  void Callback(PCVN_CALLBACK_PARAMETERS pParameters);
  

  /// Clears the parameters
  void ClearParameters()
  { RtlZeroMemory(&m_callbackParameters, sizeof(CVN_CALLBACK_PARAMETERS)); }


protected:

  /// Callback data
  LPVOID m_callbackData;

  /// Callback function
  std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID ) >  m_callbackFunction;

  /// Callback parameters
  CVN_CALLBACK_PARAMETERS m_callbackParameters;


}; // class CvnCallback

#pragma endregion


}; // namespace cvn