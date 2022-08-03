// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Client.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "Client.h"
#include "DcmThread.h"
#include "DcmClient.h"
#include "DcmCallback.h"

// usings
using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;
USING_DICOM_NS
 


/**
 * Constructor
 */
Client::Client() :
m_mpEventWaitHandle(gcnew EventWaitHandle(false, EventResetMode::ManualReset))
{

  m_callbackData = __nullptr;
  m_mpCallbackDelegate = gcnew DicomCallbackDelegate(this, &Client::Callback);

} // Find()


/**
 * Fires the appropriate event based on the event type in the callback parameters.
 *
 * @param pCallbackParameters       Callback parameters
 * @param pArgs                     Args to pass to event
 */
void Client::FireEvent(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, ClientEventArgs ^ pArgs)
{
  try
  {
    // ========================================================
    // Fires the appropriate event based on the event type
    // =======================================================
    switch ( pCallbackParameters->EventType )
    {
    case dcm::DcmEvents::E_STARTED:
      OnStarted(pArgs);
      break;
    case dcm::DcmEvents::E_COMPLETED:
      OnCompleted(pArgs);
      break;
    case dcm::DcmEvents::E_PROGRESS:
      OnProgress(pArgs);
      break;
    case dcm::DcmEvents::E_FAILED:
      OnFailed(pArgs);
      break;
    case dcm::DcmEvents::E_ABORTED:
      OnAborted(pArgs);
      break;
    };  /// END ... Switch on event type

  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(this->LogSource, "Exception detected during the handling of the callback : " + ex->Message, "Server","Callback");
    throw;
  }


} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);



