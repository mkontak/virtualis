// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CStore.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "CStore.h"
#include "Image.h"
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
CStore::CStore() :
Client(),
m_pDcmStore(scoped_ptr<std::shared_ptr<dcm::DcmStore>>(__nullptr))
{

  m_mpCallbackDelegate = gcnew DicomCallbackDelegate(this, &CStore::Callback);

} // Store()



/** 
 * Starts the store
 */
void CStore::Start(String ^ sCallbackData)
{

  // ==============================================
  // Make sure the thread has not already started
  // =============================================
  if ( ! (*m_pDcmStore)->IsRunning() )
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

    (*m_pDcmStore)->Start(m_callbackData, callback);

    // Free the global handle
    gh.Free();

    ///Sets the event wait handle to the thread handle
    m_mpEventWaitHandle->SafeWaitHandle = gcnew Microsoft::Win32::SafeHandles::SafeWaitHandle(IntPtr((*m_pDcmStore)->GetHandle()), false);

  } // END ... If the store the not already started

} // Start()


/**
 * Aborts the thread
 *
 * @return void.
 */
void CStore::Abort()
{

  try
  {
    (*m_pDcmStore)->Abort();
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
void CStore::Wait()
{

  try
  {
    (*m_pDcmStore)->Wait(INFINITE);
   }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Wait()


/**
 * Creates a store object
 */
CStore ^ CStore::Create()
{
  CStore ^ mpStore(gcnew CStore());

  if ( mpStore == nullptr )
    throw gcnew System::OutOfMemoryException("Failed to allocate CStore object");

  return mpStore;
} // Create()

/**
* Creates a store object
*
 * @param msName      Name
 * @param msAETitle   AE Title
 * @param msServer    Server host name/ip
 * @param iPort       Port
 *
 * @return CStor
 */
CStore ^ CStore::Create(String ^ msAETitle, String ^ msServerAEtitle, String ^ msServer, uint iPort)
{
  CStore ^ pStore(Create());

  try
  {
    pStore->m_pDcmStore.reset(new std::shared_ptr<dcm::DcmStore>(dcm::DcmStore::Create(ss(msAETitle), dcm::DcmHost::Create(ss(msAETitle), ss(msServerAEtitle), ss(msServer), iPort))));
  }
  catch (dcm::DcmConfigurationException & ex)
  {
    throw gcnew ConfigurationException(gcnew String(ex.GetTextMessage().c_str()));
  }
  catch (dcm::DcmException & ex)
  {
    throw gcnew DicomException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return pStore;

} // Create()

/**
 * Creates a Store object
 *
 * @param mpNode      Xml node
 */
CStore ^ CStore::Create(XmlNode ^ mpNode)
{

  CStore ^ pStore(Create());

  try
  {
    pStore->m_pDcmStore.reset(new std::shared_ptr<dcm::DcmStore>(dcm::DcmStore::Create(ss(mpNode->OuterXml))));
  }
  catch ( dcm::DcmConfigurationException & ex )
  {
    throw gcnew ConfigurationException( gcnew String(ex.GetTextMessage().c_str()) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return pStore;

} // Create(XmlNode ^ mpNode)

/**
 * Creates a Move object
 *
 * @param msAETitle        AE Title of this client
 * @param msHostXml        Host XML
 */
CStore ^ CStore::Create(String ^ msAETitle, String ^ msHostXml )
{

 CStore ^ mpStore(Create());

  try
  {
    mpStore->m_pDcmStore.reset(new std::shared_ptr<dcm::DcmStore>(dcm::DcmStore::Create(ss(msAETitle), dcm::DcmHost::Create(ss(msHostXml)))));
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( gcnew String(ex.GetTextMessage().c_str()) );
  }

  return mpStore;

} // Create(String ^ msAETitle, String ^ msHostname )

/**
* Creates a CStore object
*
* @param msAETitle       AE Title of this client
* @param mpServer        Server
*/
CStore ^ CStore::Create(String ^ msAETitle, Host ^ mpServer)
{

  CStore ^ mpStore(gcnew CStore());

  try
  {
    mpStore->m_pDcmStore.reset(new std::shared_ptr<dcm::DcmStore>(dcm::DcmStore::Create(ss(msAETitle), mpServer->GetInternalHost())));
  }
  catch (dcm::DcmException & ex)
  {
    throw gcnew DicomException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return mpStore;

} // Create(String ^ msAETitle,  Host ^ mpServer)

/**
 * Handles the callback from the server
 */
void CStore::Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  try
  {

    if ( pCallbackParameters->CommandType == dcm::DcmCommands::C_STORE)
    {

      /// Create the args
      ClientEventArgs ^ pArgs( ClientEventArgs::Create(IntPtr(pCallbackParameters), IntPtr(lpCallbackData) ));
 
      /// Fire the appropriate event
      Client::FireEvent(pCallbackParameters, pArgs);

    } // END ... If the callback parameters came from a C-STORE command

  }
  catch ( System::Exception ^ ex )
  {
    Viatronix::Logging::Log::Error(this->AETitle, "Exception detected during the handling of the callback : " + ex->Message, "Server","Callback");
    throw;
  }


} // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

/**
 * Adds files to the list of files to be stored
 *
 * @param msFile    File to be added
 */
void CStore::AddFile(String ^ msFile)
{
  try
  {
    (*m_pDcmStore)->AddFile(ss(msFile));
  }
  catch ( dcm::DcmIOException & ex )
  {
    throw gcnew IOException( "Failed to add file " + msFile + " : " + gcnew String(ex.GetTextMessage().c_str()) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( "Failed to add file " + msFile + " : " + gcnew String(ex.GetTextMessage().c_str()) );
  }

} // AddFile(String ^ msFile)

/**
 * Adds a directory
 */
void CStore::AddDirectory(String ^ msDirectory, String ^ msPattern, bool recursive)
{
  try
  {
    (*m_pDcmStore)->AddDirectory(ss(msDirectory), ss(msPattern), recursive);
  }
  catch ( dcm::DcmIOException & ex )
  {
    throw gcnew IOException( "Failed to add directory " + msDirectory + " : " + gcnew String(ex.GetTextMessage().c_str()) );
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( "Failed to add directory " + msDirectory + " : " + gcnew String(ex.GetTextMessage().c_str()) );
  }
} // AddDirectory()


/**
 * Adds a dataset to the list of images the store has to send.
 *
 * @param mpImage  The dataset to add.
 */
void CStore::AddImage(Image ^ mpImage)
{
  try
  {
    (*m_pDcmStore)->AddImage(*((std::shared_ptr<dcm::DcmImage> *)(mpImage->GetIDcmImagePtr().ToPointer())));
  }
  catch ( dcm::DcmException & ex )
  {
    throw gcnew DicomException( "Failed to add dataset.  [ERR=" + gcnew String(ex.GetTextMessage().c_str()) + "]" );
  }
} // AddImage( mpImage )

/**
 * Get the server for the client
 *
 * @return server Host object
 */
Host ^ CStore::Server::get()
{ 
  return Host::Create((*m_pDcmStore)->GetServer()); 
} // Server::get()
 
