#include "Stdafx.h"
#include "Consumer.h"
#include "Log.h"


#pragma region usings

using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;

USING_LOGGING_NS

#pragma endregion



/**
 * Constructor
 */
Consumer::Consumer() :
m_pConsumer(NULL),
m_mpEventWaitHandle(gcnew EventWaitHandle(false, EventResetMode::ManualReset))
{
  m_mpCallbackDelegate = gcnew LoggingCallbackDelegate(this, &Consumer::Callback);

} // Consumer()

/**
 * Destructor
 */
Consumer::~Consumer()
{

  if ( m_pConsumer != NULL )
    delete m_pConsumer;

} // ~Consumer()


/**
 * Dumps the specified source etl file to the destination file
 *
 * @param sSource       Source ETL file
 * @param sDestintaion  Destination file
 */
void Consumer::Dump(String ^ sSource, String ^ sDestination)
{
  try
  {
    vxLogConsumer::Dump(ss(sSource), ss(sDestination));
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException("Failed to dump file : " + gcnew String(ex.GetTextMessage().c_str()));
  }
} // Dump(String ^ sSource, String ^ sDestination)

/**
 * Creats the consumer
 *
 * @param sSource   Source
 *
 * @return Returns the newly created consumer
 */
Consumer ^ Consumer::Create( String ^ sSource)
{

  Consumer ^ mpConsumer(gcnew Consumer());

  try
  {
    mpConsumer->m_pConsumer = new vxLogConsumer(ss(sSource));
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException("Fialed to create consumer object : " + gcnew String(ex.GetTextMessage().c_str()));
  }


  return mpConsumer;

} // Create( String ^ sSource)


/** 
 * Starts the store
 */
void Consumer::Start()
{
  // ==============================================
  // Make sure the thread has not wlaredy started
  // =========================_pConsumer====================
  if ( ! m_pConsumer->IsRunning() )
  {
    // Allocate an new handle for the callback delegate
    GCHandle gh = GCHandle::Alloc(m_mpCallbackDelegate);

    // Marshall the function pointer for unmanaged usage
    IntPtr ip = Marshal::GetFunctionPointerForDelegate(m_mpCallbackDelegate);

    // Cast the int pointer the unmanaged callback function pointer for usage in the call
    LOG_CALLBACK_FUNCTION callback = static_cast<LOG_CALLBACK_FUNCTION>(ip.ToPointer());

    m_pConsumer->Start(callback, __nullptr);

    // Free the global handle
    gh.Free();

    ///Sets the event waqit handle to the thread handle
    m_mpEventWaitHandle->SafeWaitHandle = gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(IntPtr(m_pConsumer->GetHandle()), false);

  } // END ... If the store hs not already started

} // Start()


/**
 * Aborts the thread
 *
 * @return void.
 */
void Consumer::Abort()
{

  try
  {
    m_pConsumer->Abort();
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Stop()

/**
 * Waits for the thread
 *
 * @return void.
 */
void Consumer::Wait()
{

  try
  {
    m_pConsumer->Wait(INFINITE);
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Wait()


/**
 * Fires the appropriate event based on the event type in the callback parameters.
 *
 * @param pCallbackParameters       Callback parameters
 * @param pArgs                     Args to pass to event
 */
void Consumer::FireEvent(PLOG_CALLBACK_PARAMETERS pCallbackParameters, ConsumerEventArgs ^ pArgs)
{
  try
  {
    // ========================================================
    // Fires the appropriate evenet based on the event type
    // =======================================================
    switch ( pCallbackParameters->EventType )
    {
    case vxLogEventTypes::E_STARTED:
      OnStarted(pArgs);
      break;
    case vxLogEventTypes::E_COMPLETED:
      OnCompleted(pArgs);
      break;
    case vxLogEventTypes::E_PROGRESS:
      OnProgress(pArgs);
      break;
    case vxLogEventTypes::E_FAILED:
      OnFailed(pArgs);
      break;
    case vxLogEventTypes::E_ABORTED:
      OnAborted(pArgs);
      break;
    };  /// END ... Switch on event type

  }
  catch ( System::Exception ^ ex )
  {
    Log::Error("Exception detected during the firing of the event : " + ex->Message, "Consumer","Calback");
    throw;
  }


} // FireEvent(PLOG_CALLBACK_PARAMETERS pCallbackParameters, ConsumerEventArgs ^ pArgs)


/**
 * Handles the callback from the server
 */
void Consumer::Callback(PLOG_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  try
  {


    /// Create the args
    ConsumerEventArgs ^ pArgs( ConsumerEventArgs::Create(IntPtr(pCallbackParameters)));

    FireEvent(pCallbackParameters, pArgs);

  }
  catch ( System::Exception ^ ex )
  {
    Log::Error("Exception detected during the handling of the callback : " + ex->Message, "Consumer","Calback");
    throw;
  }
} // Callback(PLOG_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
