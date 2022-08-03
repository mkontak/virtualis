// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CMove.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "CMove.h"
#include "CMoveParameters.h"
#include "Host.h"


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
CMove::CMove() :
Client(),
m_pDcmMove(scoped_ptr<std::shared_ptr<dcm::DcmMove>>(__nullptr))
{

  m_mpCallbackDelegate = gcnew DicomCallbackDelegate(this, &CMove::Callback);

} // Move()



/** 
 * Starts the store
 */
void CMove::Start(String ^ sCalbackData)
{

  // ==============================================
  // Make sure the thread has not already started
  // =============================================
  if ( ! (*m_pDcmMove)->IsRunning() )
  {
    // Allocate an new handle for the callback delegate
    GCHandle gh = GCHandle::Alloc(m_mpCallbackDelegate);

    // Marshall the function pointer for unmanaged usage
    IntPtr ip = Marshal::GetFunctionPointerForDelegate(m_mpCallbackDelegate);

    // Cast the int pointer the unmanaged callback function pointer for usage in the call
    dcm::DCM_CALLBACK_FUNCTION callback = static_cast<dcm::DCM_CALLBACK_FUNCTION>(ip.ToPointer());


    if ( ! String::IsNullOrEmpty(sCalbackData) )
    {
      m_callbackData = new char [sCalbackData->Length + 1];

      int i(0);
      for each ( char ch in sCalbackData )
      {
        m_callbackData[i++] = ch;
      }
      m_callbackData[i] = '\0';


    }

    (*m_pDcmMove)->Start(m_callbackData, callback);

    // Free the global handle
    gh.Free();

    ///Sets the event wait handle to the thread handle
    m_mpEventWaitHandle->SafeWaitHandle = gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(IntPtr((*m_pDcmMove)->GetHandle()), false);

  } // END ... If the store hs not already started

} // Start()


/**
 * Aborts the thread
 *
 * @return void.
 */
void CMove::Abort()
{

  try
  {
    (*m_pDcmMove)->Abort();
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Stop()

/**
 * Waits for the thread
 *
 * @return void.
 */
void CMove::Wait()
{

  try
  {
    (*m_pDcmMove)->Wait(INFINITE);
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Wait()


#pragma region Create Methods

/**
 * Creates the CMove object
 *
 * @return CMove object
 */
CMove ^ CMove::Create()
{
  CMove ^ mpMove(gcnew CMove());

  if ( mpMove == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate CMove");

  
  return mpMove;
} // Create()


/**
 * Creates a Move object
 *
 * @param msAETitle         AE Title of this client
 * @param msHostname        Host XML
 */
CMove ^ CMove::Create(String ^ msAETitle, String ^ msHostXml )
{

 CMove ^ mpMove(Create());

  try
  {
    mpMove->m_pDcmMove.reset( new std::shared_ptr<dcm::DcmMove>(dcm::DcmMove::Create(ss(msAETitle), dcm::DcmHost::Create(ss(msHostXml)))) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return mpMove;

} // Create(XmlNode ^ mpNode)


/**
 * Creates a Move object
 *
 * @param mpNode      Xml node
 */
CMove ^ CMove::Create(XmlNode ^ mpNode)
{

 CMove ^ mpMove(Create());

  try
  {
    mpMove->m_pDcmMove.reset( new std::shared_ptr<dcm::DcmMove>( dcm::DcmMove::Create(ss(mpNode->OuterXml))) );
  }
  catch ( dcm::DcmConfigurationException & ex )
  {
    throw gcnew ConfigurationException( gcnew String(ex.GetTextMessage().c_str()) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return mpMove;

} // Create(XmlNode ^ mpNode)


/**
* Creates a Get object
*
* @param msAETitle       AE Title of this client
* @param mpServer        Server
*/
CMove ^ CMove::Create(String ^ msAETitle, Host ^ mpServer)
{

  CMove ^ mpMove(gcnew CMove());

  try
  {
    mpMove->m_pDcmMove.reset(new std::shared_ptr<dcm::DcmMove>(dcm::DcmMove::Create(ss(msAETitle), mpServer->GetInternalHost())));
  }
  catch (dcm::DcmException & ex)
  {
    throw gcnew DicomException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return mpMove;

} // Create(String ^ msAETitle,  Host ^ mpServer)

#pragma endregion

/**
 * Adds the parameters to the MOVE. Multiple parameters can be added and the move will 
 * perform them sequentially.
 *
 * @param mpParameters      Parameters
 */
CMoveParameters ^ CMove::AddParameters( CMoveParameters ^ mpParameters )
{
   (*m_pDcmMove)->AddParameters( mpParameters->GetDcmMoveParameters() );

   return mpParameters;

} // AddParameters( CMoveParameters ^ mpParameters )




/**
 * Handles the callback from the server
 *
 * @param pCallbackParametees     Callback parameters structure
 * @param lpCallbackdata          Callback data pointer
 */
void CMove::Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  try
  {

    // ==========================================================
    // Make sure the callback event is from a C-MOVE operation
    // ==========================================================
    if ( pCallbackParameters->CommandType == dcm::DcmCommands::C_MOVE)
    {

      /// Create the args
      ClientEventArgs ^ pArgs( ClientEventArgs::Create(IntPtr(pCallbackParameters), IntPtr(lpCallbackData) ));
 

      Client::FireEvent(pCallbackParameters, pArgs);


      Viatronix::Logging::Log::Debug("Returned from the FireEvent","CMove","Callback");
    } // END ... Only handle C-MOVE events
   

  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(this->AETitle, "Exception detected during the handling of the callback : " + ex->Message, "CMove","Callback");
    throw;
  }


} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

/**
 * Get the server for the client
 *
 * @return server Host object
 */
Host ^ CMove::Server::get()
{ 
  return Host::Create((*m_pDcmMove)->GetServer()); 
} // Server::get()
 
/**
 * Clears the parameter list
 */
void CMove::ClearParameters()
{ 
  (*m_pDcmMove)->ClearParameters(); 
} // ClearParameters()
 