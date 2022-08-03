#include "Stdafx.h"
#include "Enums.h"
#include "ConsumerEventArgs.h"



USING_LOGGING_NS


/**
 * Constructor
 */
ConsumerEventArgs::ConsumerEventArgs() :
m_msSource(String::Empty),
m_msMessage(String::Empty),
m_msEvent(String::Empty),
m_bAbort(false)
{
} // ConsumerEventArgs()

/**
 * Creates the ClientEventArgs from the calback data structure
 *
 * @param vxlogCallbackParameters      Callback data structure
 *
 * @returns Returns the ConsumerEventArgs object
 */
ConsumerEventArgs ^ ConsumerEventArgs::Create(System::IntPtr vxlogCallbackParameters)
{

  ConsumerEventArgs ^ mpArgs = gcnew ConsumerEventArgs();

  if ( mpArgs == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ConsumerEventArgs");

  PLOG_CALLBACK_PARAMETERS pCallbackParameters = reinterpret_cast<PLOG_CALLBACK_PARAMETERS>(vxlogCallbackParameters.ToPointer());

  mpArgs->m_eEventType = (Events)pCallbackParameters->EventType;
  mpArgs->m_msSource = gcnew String(pCallbackParameters->Source);

  switch ( mpArgs->m_eEventType )
  {
  case Events::Progress:
    {
      GUID guid = pCallbackParameters->Event.Progress.ProviderId;
      mpArgs->m_providerId = System::Guid(  guid.Data1, guid.Data2, guid.Data3, 
                                                  guid.Data4[ 0 ], guid.Data4[ 1 ], 
                                                  guid.Data4[ 2 ], guid.Data4[ 3 ], 
                                                  guid.Data4[ 4 ], guid.Data4[ 5 ], 
                                                  guid.Data4[ 6 ], guid.Data4[ 7 ] );

      mpArgs->m_msEvent = gcnew String(pCallbackParameters->Event.Progress.Record);

      mpArgs->m_eventDescriptor.Id        = pCallbackParameters->Event.Progress.EventDescriptor->Id;
      mpArgs->m_eventDescriptor.Version   = pCallbackParameters->Event.Progress.EventDescriptor->Version;
      mpArgs->m_eventDescriptor.Channel   = pCallbackParameters->Event.Progress.EventDescriptor->Channel;
      mpArgs->m_eventDescriptor.Level     = pCallbackParameters->Event.Progress.EventDescriptor->Level;
      mpArgs->m_eventDescriptor.Opcode    = pCallbackParameters->Event.Progress.EventDescriptor->Opcode;
      mpArgs->m_eventDescriptor.Task      = pCallbackParameters->Event.Progress.EventDescriptor->Task;
      mpArgs->m_eventDescriptor.Keyword   = pCallbackParameters->Event.Progress.EventDescriptor->Keyword;
    }
    break;
  case Events::Started:
    mpArgs->m_msMessage = gcnew String(pCallbackParameters->Event.Started.Message);
    break;
  case Events::Completed:
    mpArgs->m_msMessage = gcnew String(pCallbackParameters->Event.Completed.Message);
    break;
  case Events::Failed:
    mpArgs->m_msMessage = gcnew String(pCallbackParameters->Event.Failed.Message);
    break;
  case Events::Aborted:
    break;

  };   // END .. Swicth on event type

  return mpArgs;

} // Craete(System::IntPtr vxlogCallbackParameters)

