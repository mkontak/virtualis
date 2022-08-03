// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CFind.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "CFind.h"
#include "Image.h"
#include "CFindParameters.h"
#include "Host.h"
#include "ImageFactory.h"

// usings
using namespace System;
using namespace System::Collections;
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
CFind::CFind() :
Client(),
m_mpData(gcnew Generic::List<Image ^>()),
m_pDcmFind(scoped_ptr<std::shared_ptr<dcm::DcmFind>>(__nullptr))
{

  m_mpCallbackDelegate = gcnew DicomCallbackDelegate(this, &CFind::Callback);

} // Find()


/** 
 * Starts the store
 */
void CFind::Start(String ^ sCallbackData)
{

  // ==============================================
  // Make sure the thread has not already started
  // =============================================
  if ( ! (*m_pDcmFind)->IsRunning() )
  {
    // Allocate an new handle for the callback delegate
    GCHandle gh = GCHandle::Alloc(m_mpCallbackDelegate);

    // Marshall the function pointer for unmanaged usage
    IntPtr ip = Marshal::GetFunctionPointerForDelegate(m_mpCallbackDelegate);

    // Cast the int pointer the unmanaged callback function pointer for usage in the call
    dcm::DCM_CALLBACK_FUNCTION callback = static_cast<dcm::DCM_CALLBACK_FUNCTION>(ip.ToPointer());

    if ( ! String::IsNullOrEmpty(sCallbackData) )
    {
      m_callbackData = new char [sCallbackData->Length + 1];

      int i(0);
      for each ( char ch in sCallbackData )
      {
        m_callbackData[i++] = ch;
      }
      m_callbackData[i] = '\0';


    }

    (*m_pDcmFind)->Start(m_callbackData, callback);

    // Free the global handle
    gh.Free();

    ///Sets the event wait handle to the thread handle
    m_mpEventWaitHandle->SafeWaitHandle = gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(IntPtr((*m_pDcmFind)->GetHandle()), false);

  } // END ... If the store hs not already started

} // Start()


/**
 * Aborts the thread
 *
 * @return void.
 */
void CFind::Abort()
{

  try
  {
    (*m_pDcmFind)->Abort();
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
void CFind::Wait()
{

  try
  {
    (*m_pDcmFind)->Wait(INFINITE);
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
CFind ^ CFind::Create()
{
  CFind ^ mpFind(gcnew CFind());

  if ( mpFind == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate CFind");

  return mpFind;
} // Create()


/**
 * Creates a find object
 *
 * @param msAETitle         AE Title of this client
 * @param msHostXml        Host Xml
 */
CFind ^ CFind::Create(String ^ msAETitle, String ^ msHostXml )
{

  CFind ^ pFind(Create());

  try
  {
    pFind->m_pDcmFind.reset(new std::shared_ptr<dcm::DcmFind>(dcm::DcmFind::Create(ss(msAETitle), dcm::DcmHost::Create(ss(msHostXml)))));
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return pFind;

} // Create(XmlNode ^ mpNode)


/**
 * Creates a Find object
 *
 * @param mpNode      Xml node
 */
CFind ^ CFind::Create(XmlNode ^ mpNode)
{

  CFind ^ pFind(Create());

  try
  {
    pFind->m_pDcmFind.reset( new std::shared_ptr<dcm::DcmFind>(dcm::DcmFind::Create(ss(mpNode->OuterXml))) );
  }
  catch ( dcm::DcmConfigurationException & ex )
  {
    throw gcnew ConfigurationException( gcnew String(ex.GetTextMessage().c_str()) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return pFind;
} // Create(XmlNode ^ mpNode)


/**
* Creates a Find object
*
* @param msAETitle       AE Title of this client
* @param mpServer        Server
*/
CFind ^ CFind::Create(String ^ msAETitle, Host ^ mpServer)
{

  CFind ^ mpFind(gcnew CFind());

  try
  {
    mpFind->m_pDcmFind.reset(new std::shared_ptr<dcm::DcmFind>(dcm::DcmFind::Create(ss(msAETitle), mpServer->GetInternalHost())));
  }
  catch (dcm::DcmException & ex)
  {
    throw gcnew DicomException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return mpFind;

} // Create(String ^ msAETitle,  Host ^ mpServer)


#pragma endregion

/**
 * Adds the parameters to the vector.
 *
 * @param mpParams  The parameters.
 */
CFindParameters ^ CFind::AddParameters(CFindParameters ^ mpParameters)
{
  (*m_pDcmFind)->AddParameters( mpParameters->GetDcmFindParameters() );

  return mpParameters;
} // AddParameters( mpParams )


/**
 * Handles the callback from the server
 *
 * @param pCallbackParametees     Callback parameters structure
 * @param lpCallbackdata          Callback data pointer
 */
void CFind::Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  try
  {

    if ( pCallbackParameters->CommandType == dcm::DcmCommands::C_FIND)
    {

      /// Create the args
      ClientEventArgs ^ pArgs( ClientEventArgs::Create(IntPtr(pCallbackParameters), IntPtr(lpCallbackData) ));

      if ( pCallbackParameters->Command.Find.Image != __nullptr )
        m_mpData->Add((Image ^)ImageFactory::Create(IntPtr(pCallbackParameters->Command.Find.Image)));
      else
        Viatronix::Logging::Log::Warning(this->LogSource,  "Query received a NULL image", "CFind", "Callback");

      Client::FireEvent(pCallbackParameters, pArgs);

     }
  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(this->LogSource, "Exception detected during the handling of the callback : " + ex->Message, "Server","Calback");
    throw;
  }
} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);


/**
 * Get the server for the client
 *
 * @return server Host object
 */
Host ^ CFind::Server::get()
{ 
  return Host::Create((*m_pDcmFind)->GetServer()); 
} // Server::get()
 
/**
 * Clears the parameter list
 */
void CFind::ClearParameters()
{ 
  (*m_pDcmFind)->ClearParameters(); 
} // ClearParameters()
 
